
#include "EventWriter.hh"

#include "Event.h"
#include "AnalysisParticle.hh"

#include "ConfigHandler.hh"
#include "FileManager.hh"
#include "TrdHitFactory.hh"
#include "TrackFactory.hh"
#include "SplineTrack.hh"
#include "TrdQt.hh"
#include "BadRunManager.hh"
#include "CutoffCalculator.hh"
#include "TrdPdfLookup.hh"

#include <TFile.h>
#include <TTree.h>

#define DEBUG 0
#define INFO_OUT_TAG "EventWriter> "
#include <debugging.hh>

Analysis::EventWriter::EventWriter( Utilities::ConfigHandler* cfghandler,
                                    std::string resultDirectory,
                                    std::string resultFileSuffix ) :
  fResultDirectory(resultDirectory),
  fResultFileSuffix(resultFileSuffix)
{

  // read options from cfghandler here and save them in the class
  fOutputFilePrefix = "events";
  if(cfghandler)
    cfghandler->GetValue( "EventWriter", "OutputFilePrefix", fOutputFilePrefix,
                          "Prefix for output file. Directory and suffix are set via global option." );

  Clear();
}


/** Reset all variables.
  */
void Analysis::EventWriter::Clear() {

  Run = -1;
  Event = -1;
  RunTag = -1;
  BadRunTag = -1;
  EventRndm = 0;

  EventTime = 0.0;
  TriggerRate = 0.; TriggerLifeTime = 0.; LongitudeDeg = 1000.0; LatitudeDeg = 1000.0; MagLatDeg = 1000.0; MagLongDeg = 1000.0;
  GeomagneticCutOff = -1.0;

  nAccHits = -1; nTofHits = -1; nEcalShr = -1; nTrkTrks = -1;

  TrkHits = -1;

  Rig = 0.; eRinv = 0.; Chi2TrkX = -1.; Chi2TrkY = 1.;
  for( int i=0 ; i<12 ; ++i ) Rigs[i] = 0.;

  TrkPhi[0] = 1000.0; TrkPhi[1] = 1000.0;
  TrkTht[0] = 1000.0; TrkTht[1] = 1000.0;
  for( int i=0 ; i<8 ; ++i ) TrkXcors[i] = 1000.;
  for( int i=0 ; i<8 ; ++i ) TrkYcors[i] = 1000.;
  TrkXhits[0] = 0.; TrkXhits[1] = 0.;
  TrkYhits[0] = 0.; TrkYhits[1] = 0.;
  TrkSigLay1 = -1.0;

  TofC = -1; TofCe = -1; TrkC = -1; TrkCe = -1; RichC = -1; RichCe = -1;
  PartBeta = 0.; TofBeta = 0.; RichBeta = 0.;
  EcalEnergy = 0.; EcalBDT = -1.5; EcalSum = 0.;
  TofMeanE = 0.; TofMaxE = 0.;

  TrdInAcceptance     =  0;
  TrdCandidateLayers  = -1;
  TrdActiveLayers     = -1;
  TrdActiveStraws     = -1;
  TrdActivePathLen    = -1;
  TrdCandidatePathLen = -1;
  TrdUnassignedHits   =  0;
  ParticleId          = -1;
  LR_Elec_Prot        = -1;
  LR_Heli_Elec        = -1;
  LR_Heli_Prot        = -1;
  LR_Elec_Prot_MC.clear();
  LR_Heli_Elec_MC.clear();
  XePressure          =  0;
  TrdAngleXZ          =  0;
  TrdAngleYZ          =  0;

  nTrdRawHits = -1;
}


void Analysis::EventWriter::Initialize() {

  std::string outputFileName = Analysis::FileManager::MakeStandardRootFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);

  fFile = new TFile( outputFileName.c_str(), "RECREATE" );
  if( !fFile->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << outputFileName << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }

  fFile->cd();

  INFO_OUT << "Opening file for event tree: " << fFile->GetName() << std::endl;

  fTree = new TTree("ACtree", "ACsoft EventWriter tree");

  fTree->Branch("Run",                  &Run,                     "Run/I");
  fTree->Branch("Event",                &Event,                   "Event/I");
  fTree->Branch("RunTag",               &RunTag,                  "RunTag/I");
  fTree->Branch("BadRunTag",            &BadRunTag,               "BadRunTag/I");
  fTree->Branch("EventTime",            &EventTime,               "EventTime/D");
  fTree->Branch("TriggerRate",          &TriggerRate,             "TriggerRate/F");
  fTree->Branch("TriggerLifeTime",      &TriggerLifeTime,         "TriggerLifeTime/F");
  fTree->Branch("LongitudeDeg",         &LongitudeDeg,            "LongitudeDeg/F");
  fTree->Branch("LatitudeDeg",          &LatitudeDeg,             "LatitudeDeg/F");
  fTree->Branch("MagLatDeg",            &MagLatDeg,               "MagLatDeg/F");
  fTree->Branch("MagLongDeg",           &MagLongDeg,              "MagLongDeg/F");
  fTree->Branch("GeomagneticCutOff",    &GeomagneticCutOff,       "GeomagneticCutOff/F");

  fTree->Branch("nTrkTrks",             &nTrkTrks,                "nTrkTrks/B");
  fTree->Branch("TrkHits",              &TrkHits,                 "TrkHits/I");
  fTree->Branch("Rig",                  &Rig,                     "Rig/F");
  fTree->Branch("eRinv",                &eRinv,                   "eRinv/F");
  fTree->Branch("Chi2TrkX",             &Chi2TrkX,                "Chi2TrkX/F");
  fTree->Branch("Chi2TrkY",             &Chi2TrkY,                "Chi2TrkY/F");
  fTree->Branch("Rigs",                 Rigs,                     "Rigs[12]/F");
  fTree->Branch("TrkPhi",               TrkPhi,                   "TrkPhi[2]/F");
  fTree->Branch("TrkTht",               TrkTht,                   "TrkTht[2]/F");
  fTree->Branch("TrkXcors",             TrkXcors,                 "TrkXcors[8]/F");
  fTree->Branch("TrkYcors",             TrkYcors,                 "TrkYcors[8]/F");
  fTree->Branch("TrkXhits",             TrkXhits,                 "TrkXhits[2]/F");
  fTree->Branch("TrkYhits",             TrkYhits,                 "TrkYhits[2]/F");
  fTree->Branch("TrkSigLay1",           &TrkSigLay1,              "TrkSigLay/F");
  fTree->Branch("TrkC",                 &TrkC,                    "TrkC/F");
  fTree->Branch("TrkCe",                &TrkCe,                   "TrkCe/F");

  fTree->Branch("RichC",                &RichC,                   "RichC/F");
  fTree->Branch("RichCe",               &RichCe,                  "RichCe/F");
  fTree->Branch("PartBeta",             &PartBeta,                "PartBeta/F");
  fTree->Branch("RichBeta",             &RichBeta,                "RichBeta/F");

  fTree->Branch("nEcalShr",             &nEcalShr,                "nEcalShr/B");
  fTree->Branch("EcalEnergy",           &EcalEnergy,              "EcalEnergy/F");
  fTree->Branch("EcalBDT",              &EcalBDT,                 "EcalBDT/F");
  fTree->Branch("EcalSum",              &EcalSum,                 "EcalSum/F");

  fTree->Branch("nTofHits",             &nTofHits,                "nTofHits/B");
  fTree->Branch("TofBeta",              &TofBeta,                 "TofBeta/F");
  fTree->Branch("TofC",                 &TofC,                    "TofC/F");
  fTree->Branch("TofCe",                &TofCe,                   "TofCe/F");
  fTree->Branch("TofMeanE",             &TofMeanE,                "TofMeanE/F");
  fTree->Branch("TofMaxE",              &TofMaxE,                 "TofMaxE/F");
  fTree->Branch("nAccHits",             &nAccHits,                "nAccHits/B");

  fTree->Branch("ParticleId",           &ParticleId,              "ParticleId/S");
  fTree->Branch("LR_Elec_Prot",         &LR_Elec_Prot,            "LR_Elec_Prot/F");
  fTree->Branch("LR_Heli_Elec",         &LR_Heli_Elec,            "LR_Heli_Elec/F");
  fTree->Branch("LR_Heli_Prot",         &LR_Heli_Prot,            "LR_Heli_Prot/F");
  fTree->Branch("LR_Elec_Prot_MC",      "std::vector<Float_t>",    &LR_Elec_Prot_MC);
  fTree->Branch("LR_Heli_Elec_MC",      "std::vector<Float_t>",    &LR_Heli_Elec_MC);

  fTree->Branch("XePressure",           &XePressure,              "XePressure/F");
  fTree->Branch("TrdInAcceptance",      &TrdInAcceptance,         "TrdInAcceptance/B");
  fTree->Branch("TrdActivePathLen",     &TrdActivePathLen,        "TrdActivePathLen/F");
  fTree->Branch("TrdCandidatePathLen",  &TrdCandidatePathLen,     "TrdCandidatePathLen/F");
  fTree->Branch("TrdActiveLayers",      &TrdActiveLayers,         "TrdActiveLayers/S");
  fTree->Branch("TrdCandidateLayers",   &TrdCandidateLayers,      "TrdCandidateLayers/S");
  fTree->Branch("TrdActiveStraws",      &TrdActiveStraws,         "TrdActiveStraws/S");
  fTree->Branch("TrdUnassignedHits",    &TrdUnassignedHits,       "TrdUnassignedHits/S");
  fTree->Branch("nTrdRawHits",          &nTrdRawHits,             "nTrdRawHits/S");
  fTree->Branch("TrdAngleXZ",           &TrdAngleXZ,              "TrdAngleXZ/F");
  fTree->Branch("TrdAngleYZ",           &TrdAngleYZ,              "TrdAngleYZ/F");


  unsigned int nToyMC = 10;
  fTrdQt = new TrdQt(nToyMC);

}

/** Extract variables from event \p evt and fill the tree.
  */

void Analysis::EventWriter::Process( const Analysis::Particle& particle ) {

  Clear();

  const AC::Event* rawEvt = particle.RawEvent();

  Run               = rawEvt->RunHeader()->Run();
  Event             = rawEvt->EventHeader().Event();
  RunTag            = rawEvt->RunHeader()->RunTag();
  EventTime         = rawEvt->EventHeader().TimeStamp().AsDouble();
  EventRndm         = rawEvt->EventHeader().Random();

  Analysis::BadRunManager* brm = Analysis::BadRunManager::Self();
  BadRunTag         = 0;
  if( brm->IsBad(Analysis::BadRunManager::General,particle) ) BadRunTag += 1<<int(Analysis::BadRunManager::General);
  if( brm->IsBad(Analysis::BadRunManager::DAQ,particle) )     BadRunTag += 1<<int(Analysis::BadRunManager::DAQ);
  if( brm->IsBad(Analysis::BadRunManager::TRD,particle) )     BadRunTag += 1<<int(Analysis::BadRunManager::TRD);
  if( brm->IsBad(Analysis::BadRunManager::TOF,particle) )     BadRunTag += 1<<int(Analysis::BadRunManager::TOF);
  if( brm->IsBad(Analysis::BadRunManager::ACC,particle) )     BadRunTag += 1<<int(Analysis::BadRunManager::ACC);
  if( brm->IsBad(Analysis::BadRunManager::Tracker,particle) ) BadRunTag += 1<<int(Analysis::BadRunManager::Tracker);
  if( brm->IsBad(Analysis::BadRunManager::Rich,particle) )    BadRunTag += 1<<int(Analysis::BadRunManager::Rich);
  if( brm->IsBad(Analysis::BadRunManager::Ecal,particle) )    BadRunTag += 1<<int(Analysis::BadRunManager::Ecal);


  TriggerRate       = rawEvt->Trigger().TriggerRateFT();
  TriggerLifeTime   = rawEvt->Trigger().LiveTime();
  LongitudeDeg      = rawEvt->EventHeader().ISSLongitude();
  if (LongitudeDeg>180.0) LongitudeDeg -= 360.0;
  LatitudeDeg       = rawEvt->EventHeader().ISSLatitude();
  MagLatDeg         = rawEvt->EventHeader().MagneticLatitude();
  MagLongDeg        = rawEvt->EventHeader().MagneticLongitude();

  GeomagneticCutOff = Analysis::CutoffCalculator::CalculateCutoff(particle);

  nAccHits          = rawEvt->ACC().Clusters().size();
  PartBeta          = rawEvt->Particles().first().Beta();

  //
  // Tracker
  //
  nTrkTrks = rawEvt->Tracker().Tracks().size();

  const AC::TrackerTrack* TRKtrk  = particle.MainTrackerTrack();
  if( !TRKtrk ){
    WARN_OUT << "No main tracker track! Please use suitable preselection cuts to prevent this!" << std::endl;
    return;
  }

  TrkHits = TRKtrk->MakeBitPattern().to_ulong();

  // Which track fit is used ?
  int TrkFit = particle.TrackerTrackFitID();
  std::bitset<32> TrkBits=(TrkHits);
  TrkBits.set(10+TrkFit);
  TrkHits = TrkBits.to_ulong();

  const AC::TrackerTrackFit* TRKfit = particle.MainTrackerTrackFit();
  if( !TRKfit ){
    WARN_OUT << "No main tracker track fit! Please use suitable preselection cuts to prevent this!" << std::endl;
    return;
  }

  Rig          = TRKfit->Rigidity();
  eRinv        = TRKfit->InverseRigidityError() ;
  Chi2TrkX     = TRKfit->ChiSquareNormalizedX();
  Chi2TrkY     = TRKfit->ChiSquareNormalizedY();

  AC::ChargeAndError trkChargeAndError = TRKtrk->GetChargeAndErrorNew(3);
  TrkC    = trkChargeAndError.charge;
  TrkCe   = trkChargeAndError.error;

  if (TRKtrk->GetFit(1,0,4)>=0) Rigs[0] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,4)).Rigidity();
  if (TRKtrk->GetFit(1,0,5)>=0) Rigs[1] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,5)).Rigidity();
  if (TRKtrk->GetFit(2,0,4)>=0) Rigs[2] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,0,4)).Rigidity();
  if (TRKtrk->GetFit(2,0,5)>=0) Rigs[3] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,0,5)).Rigidity();
  if (TRKtrk->GetFit(1,0,3)>=0) Rigs[4] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,3)).Rigidity();
  if (TRKtrk->GetFit(2,0,3)>=0) Rigs[5] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,0,3)).Rigidity();
  if (TRKtrk->GetFit(1,3,4)>=0) Rigs[6] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,4)).Rigidity();
  if (TRKtrk->GetFit(1,3,5)>=0) Rigs[7] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,5)).Rigidity();
  if (TRKtrk->GetFit(1,3,3)>=0) Rigs[8] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,3)).Rigidity();
  if (TRKtrk->GetFit(2,3,4)>=0) Rigs[9] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,3,4)).Rigidity();
  if (TRKtrk->GetFit(2,3,5)>=0) Rigs[10] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,3,5)).Rigidity();
  if (TRKtrk->GetFit(2,3,3)>=0) Rigs[11] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,3,3)).Rigidity();

  TrkPhi[0]   = TRKfit->PhiTOFUpper()   * TMath::DegToRad();
  TrkPhi[1]   = TRKfit->PhiLayer1()     * TMath::DegToRad();
  TrkTht[0]   = TRKfit->ThetaTOFUpper() * TMath::DegToRad();
  TrkTht[1]   = TRKfit->ThetaLayer1()   * TMath::DegToRad();

  TrkXcors[0] = TRKfit->XTOFUpper();
  TrkXcors[1] = TRKfit->XTRDLower();
  TrkXcors[2] = TRKfit->XTRDCenter();
  TrkXcors[3] = TRKfit->XTRDUpper();
  TrkXcors[4] = TRKfit->XLayer1();
  TrkXcors[5] = TRKfit->XLayer9();  // historical order, could be improved

  TrkYcors[0] = TRKfit->YTOFUpper();
  TrkYcors[1] = TRKfit->YTRDLower();
  TrkYcors[2] = TRKfit->YTRDCenter();
  TrkYcors[3] = TRKfit->YTRDUpper();
  TrkYcors[4] = TRKfit->YLayer1();
  TrkYcors[5] = TRKfit->YLayer9();

  // tracker hits in Layer 1 and 9
  const AC::TrackerTrack::ReconstructedHitsVector& rhits = TRKtrk->ReconstructedHits();
  for( unsigned int i=0 ; i<rhits.size() ; ++i ){
    const AC::TrackerReconstructedHit& hit = rhits[i];
    if( hit.Layer() == 1) {
      TrkXhits[0] = hit.X();
      TrkYhits[0] = hit.Y();
      TrkSigLay1 = hit.SignalX() + hit.SignalY();
    }
    if( hit.Layer() == 9) {
      TrkXhits[1] = hit.X();
      TrkYhits[1] = hit.Y();
    }
  }

  // inner tracker track at tracker layer 1 and tracker layer 9
  int innerTrackFit = TRKtrk->GetFit(1, 3, 3); // Choutko
  if (innerTrackFit == -1) innerTrackFit = TRKtrk->GetFit(2, 3, 3); // Alcaraz
  if (innerTrackFit != -1) {
    const AC::TrackerTrackFit* innerTRKfit = TRKfit;
    TrkXcors[6] = innerTRKfit->XLayer1();
    TrkXcors[7] = innerTRKfit->XLayer9();
    TrkYcors[6] = innerTRKfit->YLayer1();
    TrkYcors[7] = innerTRKfit->YLayer9();
  }

  //
  // RICH
  //
  Short_t iRICHring = rawEvt->Particles().first().RICHRingIndex();
  if (iRICHring>=0 && iRICHring<Short_t(rawEvt->RICH().Rings().size())) {
    const AC::RICHRing& ring = rawEvt->RICH().Rings().at(iRICHring);
    RichBeta  = ring.Beta();

    // Rich charges needs to be implemented similar to Tof and Tracker
    RichC   = -1;
    RichCe  = -1;
  }

  //
  // ECAL
  //
  nEcalShr          = rawEvt->ECAL().Showers().size();
  Short_t iECALshwr = rawEvt->Particles().first().ECALShowerIndex();
  if (iECALshwr>=0 && iECALshwr<Short_t(rawEvt->ECAL().Showers().size())) {
    const AC::ECALShower& shwr = rawEvt->ECAL().Showers().at(iECALshwr);
    EcalEnergy  = shwr.ReconstructedEnergy();
    EcalBDT 	= shwr.Estimators().at(0);
  }
  EcalSum = 0.;
  for (unsigned int i=0; i<rawEvt->ECAL().Showers().size(); i++)
    EcalSum += rawEvt->ECAL().Showers().at(i).ReconstructedEnergy();

  //
  // TOF
  //
  nTofHits          = rawEvt->TOF().Clusters().size();
  if (rawEvt->TOF().Betas().size()>0) {
    const AC::TOFBeta& tofb = rawEvt->TOF().Betas().first();
    TofBeta  = tofb.Beta();
    AC::ChargeAndError tofChargeAndError = tofb.GetChargeAndErrorNew();
    TofC     = tofChargeAndError.charge;
    TofCe    = tofChargeAndError.error;
  }

  TofMeanE  = 0.;
  TofMaxE   = 0.;
  for (unsigned int i=0; i<rawEvt->TOF().Clusters().size(); i++){
    Float_t E = rawEvt->TOF().Clusters().at(i).Energy();
    TofMeanE += E;
    if( E > TofMaxE )
      TofMaxE = E;
  }
  TofMeanE /= rawEvt->TOF().Clusters().size();

  //
  // TRD
  //
  // ParticleID only used inside TrdQt for ToyMC Generation
  ParticleId = fTrdQt->ParticleID(TrkC,Rig);
  fTrdQt->Process(particle,ParticleId);

  TrdInAcceptance     = fTrdQt->IsInsideTrdGeometricalAcceptance();
  TrdActiveLayers     = fTrdQt->GetNumberOfActiveLayers();
  TrdCandidateLayers  = fTrdQt->GetNumberOfCandidateLayers();
  TrdActiveStraws     = fTrdQt->GetNumberOfActiveStraws();
  TrdActivePathLen    = fTrdQt->GetActivePathLength();
  TrdCandidatePathLen = fTrdQt->GetCandidatePathLength();
  TrdUnassignedHits   = fTrdQt->GetUnassignedHits().size();
  XePressure          = fTrdQt->QueryXenonPressure(particle.RawEvent()->EventHeader().TimeStamp());
  LR_Elec_Prot        = fTrdQt->LogLikelihoodRatioElectronProton();
  LR_Heli_Elec        = fTrdQt->LogLikelihoodRatioHeliumElectron();
  LR_Heli_Prot        = fTrdQt->LogLikelihoodRatioHeliumProton();

  const std::vector<std::vector<double> >& logLikelihoodRatiosProtonsToyMC   = fTrdQt->LogLikelihoodRatiosProtonsToyMC();
  const std::vector<std::vector<double> >& logLikelihoodRatiosElectronsToyMC = fTrdQt->LogLikelihoodRatiosElectronsToyMC();
  for (unsigned int i=0; i<logLikelihoodRatiosProtonsToyMC.size(); i++) {
      LR_Elec_Prot_MC.push_back(logLikelihoodRatiosProtonsToyMC.at(i)[0]);
      LR_Heli_Elec_MC.push_back(logLikelihoodRatiosElectronsToyMC.at(i)[2]);
  }
  nTrdRawHits         = rawEvt->TRD().NumberOfHits();

//  int D;
//  D=0; TrdAngleXZ     = fTrdQt->GetTrdTrkAngle(D);
//  D=1; TrdAngleYZ     = fTrdQt->GetTrdTrkAngle(D);

  fTree->Fill();
}

/** Write tree to output file.
  */
void Analysis::EventWriter::Finish() {

  if( !fFile || !fTree ) return;

  INFO_OUT << "Writing tree to file " << fFile->GetName() << " ..." << std::endl;

  fFile->cd();
  fTree->Write();

  fFile->Close();

}
