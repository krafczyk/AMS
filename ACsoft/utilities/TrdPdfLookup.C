#include "TrdPdfLookup.hh"

#include "AMSGeometry.h"
#include "Settings.h"
#include "Binning.hh"

#include <math.h>
#include <assert.h>
#include <sstream>
#include <stdexcept>

#include <TFile.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPdfLookup> "
#include <debugging.hh>

namespace ACsoft {

// Test using the same binning for all species, if we have sufficient statistics for this.
const std::string Utilities::TrdPdfLookup::fParticleNames[fNumberOfParticles] = {"Electron", "Proton", "Helium", "Lithium", "Beryllium", "Boron", "Carbon"};
const float Utilities::TrdPdfLookup::fRigidityLowerLimits[fNumberOfParticles] = {         1,        1,        1,         1,           1,     1,        1};
const float Utilities::TrdPdfLookup::fRigidityUpperLimits[fNumberOfParticles] = {      2000,     2000,     2000,      2000,        2000,  2000,     2000};
const int Utilities::TrdPdfLookup::fNumberOfRigidityBins[fNumberOfParticles] =  {        40,       40,       40,        40,          40,    40,       40};

Utilities::TrdPdfLookup::TrdPdfLookup() :
  fPdfFunctionsInitialized(false),
  fIdentifier(0),
  fLastQueryOk(false)
{

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

  for (int i=0; i<fNumberOfParticles; i++) {
      PdfLookup[i]    = new Utilities::SimpleGraphLookup(file, grPdfNames[i],   "",0,nGraphs[i]-1, nBin[i]);
      pValueLookup[i] = new Utilities::SimpleGraphLookup(file, grPvalueNames[i],"",0,nGraphs[i]-1, nBin[i]);
  }
}


void Utilities::TrdPdfLookup::InitializePdfFunctions() {

  if( fPdfFunctionsInitialized ) return;

  INFO_OUT << "Initializing pdf functions..." << std::endl;

  double TrdMinDeDx = 0.;
  double TrdMaxDeDx = AC::AMSGeometry::TRDMaxADCCount / ::AC::Settings::TrdTubeDefaultMinPathLength;

  for (int xenonBin = 0; xenonBin < fNumberOfXenonBins; ++xenonBin) {
    for (int idParticle = 0; idParticle < fNumberOfParticles; ++idParticle) {
      for (int rigidityBin = 0; rigidityBin < fNumberOfRigidityBins[idParticle]; ++rigidityBin) {
        for (unsigned int layer = 0; layer < AC::AMSGeometry::TRDLayers; ++layer) {
          std::stringstream name;
          name << "fPDFs_" << xenonBin << "_" << idParticle << "_" << rigidityBin << "_" << layer;
          // adjust low and high end of range to exlude underflow and overflow bins
          fPDFs[xenonBin][rigidityBin][layer][idParticle] = new TF1(name.str().c_str(), this, &Utilities::TrdPdfLookup::FunPDF, 1.0, TrdMaxDeDx - 1.0, 7, "Utilities::TrdPdfLookup", "FunPDF");
          fPDFs[xenonBin][rigidityBin][layer][idParticle]->SetParameters(double(xenonBin),double(rigidityBin),double(layer),double(idParticle),
                                                                         TrdMinDeDx,TrdMaxDeDx,::AC::Settings::TrdMinimumPdfValue);
          fPDFs[xenonBin][rigidityBin][layer][idParticle]->SetNpx(1000);
        }
      }
    }
  }

  INFO_OUT << "Done." << std::endl;
  fPdfFunctionsInitialized = true;
}


/** Helper function needed for generation of toy MCs because there is no GetRandom function for TGraph objects.
*
* This function does not contain the under- and overflow bins, since these cannot be handled properly by TF1::GetRandom().
*/
Double_t Utilities::TrdPdfLookup::FunPDF(Double_t *x, Double_t *par) {

  Int_t    xenonBin    = TMath::Nint(par[0]);
  Int_t    rigidityBin = TMath::Nint(par[1]);
  Int_t    layer       = TMath::Nint(par[2]);
  Int_t    particleId  = TMath::Nint(par[3]);
  double   TrdMinDeDx  = par[4];
  double   TrdMaxDeDx  = par[5];
  double   MinimumPdfValue = par[6];
  double   dEdX        = x[0];

  double pdf=0.0;
  if (dEdX > TrdMinDeDx+0.5 && dEdX < TrdMaxDeDx-0.5) pdf = GetTrdPdfValue(xenonBin, rigidityBin, layer, dEdX, ParticleId(particleId));

  return std::max(MinimumPdfValue,pdf);
}

/** Returns the xenon partial pressure for a given time */
double Utilities::TrdPdfLookup::QueryXenonPressure(const TTimeStamp& time, bool& queryOk) {

  return QueryXenonPressure(time.AsDouble(),queryOk);
}

/** Returns the xenon partial pressure for a given time (seconds since epoch)*/
double Utilities::TrdPdfLookup::QueryXenonPressure( double time, bool &queryOk ) {

  static Utilities::SimpleGraphLookup pXeLookup(::AC::Settings::gTrdQtSlowControlFileName,
                                                ::AC::Settings::gTrdQtSlowControlFileNameExpectedGitSHA,
                                                ::AC::Settings::gTrdQtSlowControlFileNameExpectedVersion,
                                                std::string("trd_online"), std::string("xe"));
  double pXe =  pXeLookup.QueryTreeGraph(time);
  queryOk = pXeLookup.LastQueryOk();

  return pXe;

}

int Utilities::TrdPdfLookup::GetXenonBin( const TTimeStamp& time, bool& queryOk ) {

  double pressureXe = QueryXenonPressure(time,queryOk);
  return GetXenonBin(pressureXe);
}


int Utilities::TrdPdfLookup::GetXenonBin( double pressureXe ) {

  return std::min(fNumberOfXenonBins - 1,std::max(0,int((pressureXe-700.0)/50.0)));
}


int Utilities::TrdPdfLookup::GetRigidityBin( double rigidity, ParticleId pid ) {

  assert( int(pid) >= 0 && int(pid) < fNumberOfParticles );

  rigidity = fabs(rigidity);

  double binwidth = ( log(fRigidityUpperLimits[pid]) - log(fRigidityLowerLimits[pid]) ) / fNumberOfRigidityBins[pid];

  int bin = ( log(rigidity)-log(fRigidityLowerLimits[pid]) ) / binwidth;

  if( bin < 0 ) bin = 0;
  if( bin >= fNumberOfRigidityBins[pid] ) bin = fNumberOfRigidityBins[pid] - 1;

  return bin;
}


std::vector<double> Utilities::TrdPdfLookup::GetBinningForParticle( ParticleId particle ) {

  assert( int(particle) >= 0 && int(particle) < fNumberOfParticles );
  return Utilities::Binning::GenerateLogBinning(fNumberOfRigidityBins[particle], fRigidityLowerLimits[particle], fRigidityUpperLimits[particle]);
}


Utilities::TrdPdfLookup::~TrdPdfLookup() {

  for (int i=0; i<fNumberOfParticles; i++) {
    if (PdfLookup[i]) delete PdfLookup[i];
    if (pValueLookup[i]) delete pValueLookup[i];
  }
}


/** Returns the Trd PDF value for a given Xenon pressure \p pXe, \p rigidity, %TRD \p layer, measured \p dEdX and particle type \p pid */
double Utilities::TrdPdfLookup::GetTrdPdfValue( double pXe, double rigidity, unsigned int layer, double dEdX, ParticleId pid ) const {

  DEBUG_OUT << "Query: pXe " << pXe << " rig " << rigidity << " layer " << layer << " dEdx: " << dEdX << " particle id " << pid << std::endl;

  int xeBin  = GetXenonBin(pXe);
  int rigBin = GetRigidityBin(fabs(rigidity),pid);

  return GetTrdPdfValue(xeBin,rigBin,layer,dEdX,pid);
}

/** Generate a toy-MC dE/dx value for the given particle \p pid, Xenon partial pressure \p pXe, \p rigidity and \p layer.*/
double Utilities::TrdPdfLookup::GetToymcDedxValue( double pXe, double rigidity, unsigned int layer, ParticleId pid ) {

  int xeBin  = GetXenonBin(pXe);
  int rigBin = GetRigidityBin(fabs(rigidity),pid);

  return GetToymcDedxValue(xeBin,rigBin,layer,pid);
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
double Utilities::TrdPdfLookup::GetTrdPdfValue( int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const {

  fLastQueryOk = true;

  DEBUG_OUT << "Query: xe " << XenonBin << " rigBin " << rigidityBin << " layer " << layer << " dEdx: " << dEdX << " particle id " << pid << std::endl;
  assert( XenonBin>=0 && XenonBin<fNumberOfXenonBins );

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  static long long nWarnings = 0;
#endif

  unsigned int graphNumber = 0;
  if (pid==0) { // electrons
      graphNumber = XenonBin*AC::AMSGeometry::TRDLayers + layer;
      double val = PdfLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool ok = PdfLookup[pid]->LastQueryOk();
      if( !ok ){
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
        nWarnings++;
        if (nWarnings<10)  {
            WARN_OUT << "Failed query for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        } else if (nWarnings==10) {
            WARN_OUT << "Suppressing further warnings for particle id " << pid << " graph " << graphNumber << " dEdX " << dEdX << std::endl;
        }
#endif
        fLastQueryOk = false;
        return -1.0;
      }
      return val;

  } else { // protons, He, nuclei
      graphNumber = XenonBin*fNumberOfRigidityBins[pid] + rigidityBin;
      double val = PdfLookup[pid]->Query(graphNumber,dEdX,false).value;
      bool ok = PdfLookup[pid]->LastQueryOk();
      if( !ok ){
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
            fLastQueryOk = false;
            return -1.0;
        }
      }
      return val;
  }

  return 0.0;
}


/** Generate a toy-MC dE/dx value for the given particle and parameters.
  *
  * \param XenonBin bin number for Xenon partial pressure (in the same binning as used when filling the pdf lookups)
  * \param rigidityBin bin number for rigidity binning (in the same binning as used when filling the pdf lookups)
  * \param layer TRD layer number (0..19)
  * \param pid ParticleId of particle for which the pdf is to be retrieved.
  */
double Utilities::TrdPdfLookup::GetToymcDedxValue( int XenonBin, unsigned int rigidityBin, unsigned int layer, ParticleId pid ) {

  assert(XenonBin < fNumberOfXenonBins);
  assert(rigidityBin < (unsigned int)MaximumNumberOfRigidityBins);
  assert(layer < AC::AMSGeometry::TRDLayers);
  assert(pid < fNumberOfParticles);
  if(!fPdfFunctionsInitialized) InitializePdfFunctions();

  static TRandom3* sRandom = new TRandom3();

  // FIXME unify all occurences of TrdMinDeDx and TrdMaxDeDx across the AC software
  double TrdMinDeDx = 0.;
  double TrdMaxDeDx = AC::AMSGeometry::TRDMaxADCCount / ::AC::Settings::TrdTubeDefaultMinPathLength;

  double  ZeroEntries     = GetTrdPdfValue(XenonBin, rigidityBin, layer, TrdMinDeDx, pid);
  double  OverflowEntries = GetTrdPdfValue(XenonBin, rigidityBin, layer, TrdMaxDeDx, pid);
  double  xRandom         = sRandom->Uniform();
  double deDx = 0.0;

  // We have to use the internal fPDFs TF1 objects because the TGraph objects in the lookup do not have
  // a GetRandom() function. Under- and Overflow bins are handled manually here, in order not to confuse TF1::GetRandom().

  if( xRandom <= ZeroEntries )
    deDx = TrdMinDeDx;
  else if( xRandom >= 1.0-OverflowEntries )
    deDx = TrdMaxDeDx;
  else
    deDx = fPDFs[XenonBin][rigidityBin][layer][pid]->GetRandom();

  return deDx;
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
double Utilities::TrdPdfLookup::GetTrdPvalue( int XenonBin, unsigned int rigidityBin, unsigned int layer, double dEdX, ParticleId pid ) const {

  fLastQueryOk = true;

  DEBUG_OUT << "Query: xe " << XenonBin << " rigBin " << rigidityBin << " layer " << layer << " dEdx: " << dEdX << " particle id " << pid << std::endl;
  assert( XenonBin>=0 && XenonBin<fNumberOfXenonBins );

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  static long long nWarnings = 0;
#endif

  unsigned int graphNumber = 0;
  if (pid==0) {
      graphNumber = XenonBin*AC::AMSGeometry::TRDLayers + layer;
      double val = pValueLookup[pid]->Query(graphNumber,dEdX,false).value;
      fLastQueryOk = pValueLookup[pid]->LastQueryOk();
      if( !fLastQueryOk ){
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
      fLastQueryOk = pValueLookup[pid]->LastQueryOk();
      if( !fLastQueryOk ){
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

}
