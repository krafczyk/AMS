#include "TrdDetector.hh"

#include "AMSGeometry.h"
#include "TrdModule.hh"
#include "TrdLayer.hh"
#include "TrdSublayer.hh"
#include "FileManager.hh"
#include "TimeHistogramManager.hh"
#include "Utilities.hh"

#include "dumpstreamers.hh"

#include <TH3F.h>
#include <TFile.h>
#include <TTimeStamp.h>

#include <sstream>
#include <stdexcept>
#include <math.h>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "Trd> "
#include <debugging.hh>

Detector::Trd::Trd()
{

  fNominalPosition = TVector3(0.0,0.0,AC::AMSGeometry::ZTRDCenter);
  fOffsetPosition  = TVector3(0.0,0.0,0.0);
  fNominalRotation.SetToIdentity();
  fExtraRotation.SetToIdentity();

  ConstructTrdLayers();
  FillConvenienceVectors();




  for (int i = 0; i < Utilities::TrdPdfLookup::fNumberOfParticles; ++i) {
    fADCSpectrum_vs_Rigidity[i] = 0;
    fADCSpectrum_vs_Layer[i] = 0;
  }
}

Detector::Trd::~Trd()
{
  for( unsigned int i = 0 ; i < fLayers.size() ; ++i )
    delete fLayers[i];
}


void Detector::Trd::ConstructTrdLayers() {

  INFO_OUT << "Constructing TRD with " << AC::AMSGeometry::TRDLayers << " layers..." << std::endl;

  // create layers, which will in turn create the additional substructure (sublayers, modules, straws)
  for( unsigned int lay=0 ; lay < AC::AMSGeometry::TRDLayers ; ++lay ){

    fLayers.push_back( new Detector::TrdLayer(lay,this) );
  }

  // trigger calculation of global positions and rotations for all substructure
  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    Detector::TrdLayer* layer = fLayers[i];
    layer->UpdateGlobalPositionAndDirection();
  }
}


void Detector::Trd::FillConvenienceVectors() {

  fSublayers.clear();
  fModules.clear();

  fSublayers.assign(AC::AMSGeometry::TRDSubLayers,0);
  fModules.assign(AC::AMSGeometry::TRDModules,0);

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    Detector::TrdLayer* layer = fLayers[i];

    for( unsigned int j=0 ; j<layer->NumberOfSublayers() ; ++j ){

      Detector::TrdSublayer* sublayer = layer->GetSublayer(j);
      int globalSublayerNumber = sublayer->GlobalSublayerNumber();
      fSublayers.at(globalSublayerNumber) = sublayer;

      for( unsigned int m = 0 ; m<sublayer->NumberOfModules() ; ++m ){
        TrdModule* module = sublayer->GetModule(m);
        int moduleNumber = module->ModuleNumber();
        fModules.at(moduleNumber) = module;
      }
    }
  }
}


/** Change the alignment parameters for the TRD and trigger
  * a recalculation of the global coordinates of all the layers.
  *
  * The nominal position and orientation of the TRD will remain unchanged.
  * This function sets the additional offset and rotation due to alignment.
  *
  * The recalculation of the parameters for the layers will automatically trigger a recalculation
  * also for all other components further down the logical detector hierarchy.
  *
  */
void Detector::Trd::ChangePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot ) {

  fOffsetPosition = offsetPos;
  fExtraRotation  = extraRot;

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    Detector::TrdLayer* layer = fLayers[i];
    layer->UpdateGlobalPositionAndDirection();
  }
}


void Detector::Trd::Dump() const {

  INFO_OUT << "TRD position: " << fNominalPosition << " + " << fOffsetPosition << " rotation: " << fExtraRotation << " * " << fNominalRotation << std::endl;

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    Detector::TrdLayer* layer = fLayers[i];
    layer->Dump();
  }
}


void Detector::Trd::WriteHistosToCurrentFile() {

  for( unsigned int i=0 ; i<fModules.size() ; ++i )
    fModules[i]->WriteHistogramsToCurrentFile();

  if(!fADCSpectrum_vs_Rigidity[0]) return;
  for (int i = 0; i < Utilities::TrdPdfLookup::fNumberOfParticles; ++i) {
	fADCSpectrum_vs_Rigidity[i]->Write();
    fADCSpectrum_vs_Layer[i]->Write();
  }
}

void Detector::Trd::CreateADCSpectraHistos() {

  std::vector<double> v_adc;
  float TrdMaxDeDx = AC::AMSGeometry::TRDMaxADCCount / AC::AMSGeometry::TRDTubeMinPathLength;

  // underflow Bin, i.e. dEdX=0, no TrdHit found, binWidth should be 1
   v_adc.push_back(-0.5);

   int   nBinLogAdcTemp = 400;
   std::vector<double> v_adcTemp = Utilities::GenerateLogBinning(nBinLogAdcTemp, 0.5, TrdMaxDeDx-0.5);
   for (int i=0; i<=nBinLogAdcTemp; i++) v_adc.push_back(v_adcTemp.at(i));

   // overflow Bin, i.e. dEdX after calibration larger that (TRDMaxADCCount=4096ADC)/(TRDTubeMinPathLength=0.1cm) = 40960
   // binWidth should be 1
   v_adc.push_back(TrdMaxDeDx+0.5);

  std::vector<double> v_layer;
  for (unsigned int i=0; i<=AC::AMSGeometry::TRDLayers; i++) v_layer.push_back(i-0.5);

  for (int i = 0; i < Utilities::TrdPdfLookup::fNumberOfParticles; ++i) {
    std::vector<double> v_RigLog = Utilities::TrdPdfLookup::GetBinningForParticle(i);
    std::stringstream nameRigidity, titleRigidity;
    nameRigidity << "ADCVsRigidity_" << Utilities::TrdPdfLookup::fParticleNames[i];
    titleRigidity << Utilities::TrdPdfLookup::fParticleNames[i] << " - dE/dX [ADC/cm]";
    fADCSpectrum_vs_Rigidity[i] = Utilities::TimeHistogramManager::MakeNewTimeHistogram3D<TH3F>( nameRigidity.str(), titleRigidity.str(), "dE/dX [ADC/cm]", "Rigidity [GV]", v_adc.size()-1, v_adc.data(), Utilities::TrdPdfLookup::fNumberOfRigidityBins[i], v_RigLog.data() );
    
    std::stringstream nameLayer, titleLayer;
    nameLayer << "ADCVsLayer_" << Utilities::TrdPdfLookup::fParticleNames[i];
    titleLayer << Utilities::TrdPdfLookup::fParticleNames[i] << " - dE/dX [ADC/cm]";
    fADCSpectrum_vs_Layer[i] = Utilities::TimeHistogramManager::MakeNewTimeHistogram3D<TH3F>( nameLayer.str(), titleLayer.str(), "dE/dX [ADC/cm]", "Layer [#]", v_adc.size()-1, v_adc.data(), AC::AMSGeometry::TRDLayers, v_layer.data() );
  }
}

void Detector::Trd::StoreADCSpectra( const TTimeStamp& time, int particleID, double dedx, double rig, short layer ) {

  assert(particleID >= 0);
  assert(particleID < Utilities::TrdPdfLookup::fNumberOfParticles);

  if(!fADCSpectrum_vs_Rigidity[0]) CreateADCSpectraHistos();

  double TrdMaxDeDx  = AC::AMSGeometry::TRDMaxADCCount / AC::AMSGeometry::TRDTubeMinPathLength;
  double value       = std::min(dedx,TrdMaxDeDx);
  fADCSpectrum_vs_Rigidity[particleID]->Fill(double(time), value, rig);
  fADCSpectrum_vs_Layer[particleID]->Fill(double(time), value, layer);
}


void Detector::Trd::WriteHistosToNewFile( std::string resultdir, std::string prefix, std::string suffix ) {

  std::string outputFileName = Analysis::FileManager::MakeStandardRootFileName(resultdir,prefix,suffix);

  TFile* f = new TFile( outputFileName.c_str(), "RECREATE");
  if( !f->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << outputFileName << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }

  f->cd();

  INFO_OUT << "Writing histograms to file " << f->GetName() << std::endl;

  WriteHistosToCurrentFile();

  f->Close();

}
