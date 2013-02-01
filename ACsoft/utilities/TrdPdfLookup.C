#include "TrdPdfLookup.hh"

#include "AMSGeometry.h"
#include "Settings.h"
#include "Utilities.hh"

#include <TFile.h>
#include <assert.h>
#include <sstream>
#include <stdexcept>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPdfLookup> "
#include <debugging.hh>

const std::string ACsoft::Utilities::TrdPdfLookup::fParticleNames[fNumberOfParticles] = {"Electron", "Proton", "Helium", "Lithium", "Beryllium", "Boron", "Carbon"};
const float ACsoft::Utilities::TrdPdfLookup::fRigidityLowerLimits[fNumberOfParticles] = {         1,        1,        1,         1,           1,     1,        1};
const float ACsoft::Utilities::TrdPdfLookup::fRigidityUpperLimits[fNumberOfParticles] = {        60,      600,      600,       600,         600,   600,      600};
const int ACsoft::Utilities::TrdPdfLookup::fNumberOfRigidityBins[fNumberOfParticles] =  {        10,       30,       30,        20,          20,    20,       20};

ACsoft::Utilities::TrdPdfLookup::TrdPdfLookup() : fIdentifier(0) {

  const char* acrootlookups = getenv("ACROOTLOOKUPS");
  if( !acrootlookups ){
    WARN_OUT << "ACROOTLOOKUPS variable not set!" << std::endl;
    throw std::runtime_error("ACROOTLOOKUPS variable not set!");
  }

  unsigned int nGraphs[fNumberOfParticles];
  unsigned int nBin[fNumberOfParticles];
  std::string grPdfNames[fNumberOfParticles];
  std::string grPvalueNames[fNumberOfParticles];

  nGraphs[0]       = fNumberOfXenonBins*AC::AMSGeometry::TRDLayers;
  nBin[0]          = AC::AMSGeometry::TRDLayers;
  grPdfNames[0]    = "grTrdQt_PDF_ADCVsLayer_" + fParticleNames[0];
  grPvalueNames[0] = "grTrdQt_pValue_ADCVsLayer_" + fParticleNames[0];
  for (int i=1; i<fNumberOfParticles; i++) {
      nGraphs[i]       = fNumberOfXenonBins * fNumberOfRigidityBins[i];
      nBin[i]          = fNumberOfRigidityBins[i];
      grPdfNames[i]    = "grTrdQt_PDF_ADCVsRigidity_" + fParticleNames[i];
      grPvalueNames[i] = "grTrdQt_pValue_ADCVsRigidity_" + fParticleNames[i];
  }

  std::string lookupfile = ::AC::Settings::gTrdQtPdfFileName;
  std::string expectedGitSHA = ::AC::Settings::gTrdQtPdfFileNameExpectedGitSHA;
  unsigned short expectedVersion = ::AC::Settings::gTrdQtPdfFileNameExpectedVersion;

  std::stringstream filename;
  filename << acrootlookups << "/" << lookupfile;
  INFO_OUT << "Opening lookup file " << filename.str() << std::endl;
  TFile* file = new TFile( filename.str().c_str(), "READ" );
  if( !file->IsOpen() ){
    WARN_OUT << "ERROR opening lookup file " << filename.str() <<  std::endl;
    throw std::runtime_error("ERROR opening lookup file.");
  }

  // FIXME: Once we've produced new lookups, always rely on the existance of a 'ACQtVersion' object!
  fIdentifier = (::AC::ACQtPDFLookupFileIdentifier*) file->Get("ACQtPDFVersion");
  assert(fIdentifier);
  if (fIdentifier->fGitSHA != expectedGitSHA) {
    WARN_OUT << "ERROR validating lookup file " << filename.str() << ". File mismatch! Expected Git SHA: \""
             << expectedGitSHA << "\". Actual Git SHA: \"" << fIdentifier->fGitSHA << "\"" << std::endl;
    throw std::runtime_error("ERROR validating lookup file.");
  }

  if (fIdentifier->fVersion != expectedVersion) {
    WARN_OUT << "ERROR validating lookup file " << filename.str() << ". File mismatch! Expected version: \""
            << expectedVersion << "\". Actual version: \"" << fIdentifier->fVersion << "\"" << std::endl;
    throw std::runtime_error("ERROR validating lookup file.");
  }

  fAddNearTrackHits = fIdentifier->fAddNearTrackHits;

  // FIXME: Hand out the TFile to SimpleGraphLookup, instead of opening the same file NumberOfParticle * 2 times seperated!
  for (int i=0; i<fNumberOfParticles; i++) {
      PdfLookup[i]    = new ACsoft::Utilities::SimpleGraphLookup(lookupfile, expectedGitSHA, expectedVersion, grPdfNames[i],   "",0,nGraphs[i]-1, nBin[i]);
      pValueLookup[i] = new ACsoft::Utilities::SimpleGraphLookup(lookupfile, expectedGitSHA, expectedVersion, grPvalueNames[i],"",0,nGraphs[i]-1, nBin[i]);
  }
}


double ACsoft::Utilities::TrdPdfLookup::QueryXenonPressure(const TTimeStamp& time) {

  static ACsoft::Utilities::SimpleGraphLookup pXeLookup(::AC::Settings::gTrdQtSlowControlFileName,
                                                ::AC::Settings::gTrdQtSlowControlFileNameExpectedGitSHA,
                                                ::AC::Settings::gTrdQtSlowControlFileNameExpectedVersion,
                                                std::string("trd_online"), std::string("xe"));
  return pXeLookup.QueryTreeGraph(time);
}


int ACsoft::Utilities::TrdPdfLookup::GetXenonBin(const TTimeStamp& time) {

  double pressureXe = QueryXenonPressure(time);
  return std::min(ACsoft::Utilities::TrdPdfLookup::fNumberOfXenonBins - 1,std::max(0,int((pressureXe-700.0)/50.0)));
}


int ACsoft::Utilities::TrdPdfLookup::GetXenonBin(const double pressureXe) {

  return std::min(ACsoft::Utilities::TrdPdfLookup::fNumberOfXenonBins - 1,std::max(0,int((pressureXe-700.0)/50.0)));
}


std::vector<double> ACsoft::Utilities::TrdPdfLookup::GetBinningForParticle(int particle) {

  if (particle < 0 || particle >= fNumberOfParticles)
   return std::vector<double>();
  return GenerateLogBinning(fNumberOfRigidityBins[particle], fRigidityLowerLimits[particle], fRigidityUpperLimits[particle]);
}


ACsoft::Utilities::TrdPdfLookup::~TrdPdfLookup() {

  for (int i=0; i<fNumberOfParticles; i++) {
    if (PdfLookup[i]) delete PdfLookup[i];
    if (pValueLookup[i]) delete pValueLookup[i];
  }
}


/** Query the lookup.
  *
  * \param XenonBin bin number for Xenon partial pressure (in the same binning as used when filling the pdf lookups)
  * \param rigidityBin bin number for rigidity binning (in the same binning as used when filling the pdf lookups)
  * \param layer TRD layer number (0..19)
  * \param dEdX Value of dE/dx (in ADC/cm) where pdf value is to be retrieved.
  * \param pid ParticleId of particle for which the pdf is to be retrieved.
  *
  * \note For electrons, the \p rigidityBin argument is ignored. For other elements the \p layer argument is ignored.
  *
  * \todo explain where the global binning for the input parameters is defined
  */
double ACsoft::Utilities::TrdPdfLookup::GetTrdPdfValue( unsigned int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const {

  DEBUG_OUT << "Query: xe " << XenonBin << " rigBin " << rigidityBin << " layer " << layer << " dEdx: " << dEdX << " particle id " << pid << std::endl;

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  static long long nWarnings = 0;
#endif

  unsigned int graphNumber = 0;
  if (pid==0) {
      graphNumber = XenonBin*AC::AMSGeometry::TRDLayers + layer;
      double val = PdfLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool queryOk = PdfLookup[pid]->LastQueryOk();
      if( !queryOk ){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
        nWarnings++;
        if (nWarnings<10)  {
            WARN_OUT << "Failed query for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        } else if (nWarnings==10) {
            WARN_OUT << "Suppressing further warnings for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        }
#endif
        return -1.0;
      }
      return val;

  } else {
      graphNumber = XenonBin*fNumberOfRigidityBins[pid] + rigidityBin;
      double val = PdfLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool queryOk = PdfLookup[pid]->LastQueryOk();
      if( !queryOk ){
        DEBUG_OUT << "Failed query for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        // call recursively for lower rigidity bin
        if( rigidityBin > 0 )
            return GetTrdPdfValue(XenonBin,rigidityBin-1,layer,dEdX,pid);
        else {
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
            nWarnings++;
            if (nWarnings<10) {
                WARN_OUT << "Failed all queries for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
            } else if (nWarnings==10) {
                WARN_OUT << "Suppressing further warnings for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
            }
#endif
            return -1.0;
        }
      }
      return val;
  }

  return 0.0;
}

/** Query the lookup.
  *
  * \param XenonBin bin number for Xenon partial pressure (in the same binning as used when filling the pdf lookups)
  * \param rigidityBin bin number for rigidity binning (in the same binning as used when filling the pdf lookups)
  * \param layer TRD layer number (0..19)
  * \param dEdX Value of dE/dx (in ADC/cm) where pdf value is to be retrieved.
  * \param pid ParticleId of particle for which the pdf is to be retrieved.
  *
  * \note For electrons, the \p rigidityBin argument is ignored. For other elements the \p layer argument is ignored.
  *
  * \todo explain where the global binning for the input parameters is defined
  */
double ACsoft::Utilities::TrdPdfLookup::GetTrdPvalue( unsigned int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const {

  DEBUG_OUT << "Query: xe " << XenonBin << " rigBin " << rigidityBin << " layer " << layer << " dEdx: " << dEdX << " particle id " << pid << std::endl;

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  static long long nWarnings = 0;
#endif

  unsigned int graphNumber = 0;
  if (pid==0) {
      graphNumber = XenonBin*AC::AMSGeometry::TRDLayers + layer;
      double val = pValueLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool queryOk = pValueLookup[pid]->LastQueryOk();
      if( !queryOk ){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
        nWarnings++;
        if (nWarnings<2) {
            WARN_OUT << "Failed query for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        } else if (nWarnings==2) {
            WARN_OUT << "Suppressing further warnings for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        }
#endif
        return -1.0;
      }
      return val;

  } else {
      graphNumber = XenonBin*fNumberOfRigidityBins[pid] + rigidityBin;
      double val = pValueLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool queryOk = pValueLookup[pid]->LastQueryOk();
      if( !queryOk ){
        DEBUG_OUT << "Failed query for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        // call recursively for lower rigidity bin
        if( rigidityBin > 0 )
            return GetTrdPvalue(XenonBin,rigidityBin-1,layer,dEdX,pid);
        else {
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
            nWarnings++;
            if (nWarnings<2) {
                WARN_OUT << "Failed all queries for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
            } else if (nWarnings==2) {
                WARN_OUT << "Suppressing further warnings for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
            }
#endif
            return -1.0;
        }
      }
      return val;
  }

  return 0.0;
}
