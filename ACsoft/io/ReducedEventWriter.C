#include "ReducedEventWriter.hh"

#include "Event.h"
#include "AnalysisParticle.hh"

#include "ConfigHandler.hh"
#include "FileManager.hh"
#include "TrdHitFactory.hh"
#include "TrackFactory.hh"
#include "SplineTrack.hh"
#include "TrdQt.hh"
#include "ReducedEvent.hh"
#include "BadRunManager.hh"
#include "TrdPdfLookup.hh"
#include "ReducedFileHeader.hh"
#include "TrdCandidateMatching.hh"

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>

#define DEBUG 0
#define INFO_OUT_TAG "ReducedEventWriter> "
#include <debugging.hh>
#include <QDebug>

/** Constructor.
  * The "onlyWriteExtraBranches" parameter defaults to false. If set to true, the standard tree branches won't be
  * created only the user registered extra branches. This is needed to write an additional tree with a custom branch
  * that should be read in together with the standard tree.
  */ 
ACsoft::IO::ReducedEventWriter::ReducedEventWriter( Utilities::ConfigHandler* cfghandler,
                                    std::string resultDirectory,
                                    std::string resultFileSuffix, bool onlyWriteExtraBranches ) :
  fResultDirectory(resultDirectory),
  fResultFileSuffix(resultFileSuffix),
  fFile(0),
  fTree(0),
  fReducedEvent(0),
  fTrdQt(0),
  fOnlyWriteExtraBranches(onlyWriteExtraBranches)
{

  // read options from cfghandler here and save them in the class
  fOutputFilePrefix = fOnlyWriteExtraBranches ? "custom_events" : "events";
  if(cfghandler)
    cfghandler->GetValue( "ReducedEventWriter", "OutputFilePrefix", fOutputFilePrefix,
                          "Prefix for output file. Directory and suffix are set via global option." );

}

ACsoft::IO::ReducedEventWriter::~ReducedEventWriter() {

  delete fTree;
  delete fFile;
  delete fReducedEvent;
  delete fTrdQt;
}

/** Reset all variables.
  */
void ACsoft::IO::ReducedEventWriter::Clear() {

  if (fReducedEvent)
    fReducedEvent->Clear();
}

void ACsoft::IO::ReducedEventWriter::SetBranches(ReducedEvent* event) {

  fTree->Branch("Run",                  &event->Run,                     "Run/I");
  fTree->Branch("Event",                &event->Event,                   "Event/I");
  if (fOnlyWriteExtraBranches)
    return;

  fTree->Branch("RunTag",               &event->RunTag,                  "RunTag/I");
  fTree->Branch("BadRunTag",            &event->BadRunTag,               "BadRunTag/I");
  fTree->Branch("EventTime",            &event->EventTime,               "EventTime/D");
  fTree->Branch("TriggerRate",          &event->TriggerRate,             "TriggerRate/F");
  fTree->Branch("TriggerLifeTime",      &event->TriggerLifeTime,         "TriggerLifeTime/F");
  fTree->Branch("LongitudeDeg",         &event->LongitudeDeg,            "LongitudeDeg/F");
  fTree->Branch("LatitudeDeg",          &event->LatitudeDeg,             "LatitudeDeg/F");
  fTree->Branch("MagLatDeg",            &event->MagLatDeg,               "MagLatDeg/F");
  fTree->Branch("MagLongDeg",           &event->MagLongDeg,              "MagLongDeg/F");
  fTree->Branch("GeomagneticMaxCutOff", &event->GeomagneticMaxCutOff,    "GeomagneticMaxCutOff/F");
  fTree->Branch("GeomagneticMaxCutOffCone", &event->GeomagneticMaxCutOffCone, "eeomagneticMaxCutOffCone/F");

  fTree->Branch("nTrkTrks",             &event->nTrkTrks,                "nTrkTrks/B");
  fTree->Branch("TrkHits",              &event->TrkHits,                 "TrkHits/I");
  fTree->Branch("TrackerLayerPatternClassification", &event->TrackerLayerPatternClassification, "TrackerLayerPatternClassification/S");
  fTree->Branch("Rig",                  &event->Rig,                     "Rig/F");
  fTree->Branch("eRinv",                &event->eRinv,                   "eRinv/F");
  fTree->Branch("Chi2TrkX",             &event->Chi2TrkX,                "Chi2TrkX/F");
  fTree->Branch("Chi2TrkY",             &event->Chi2TrkY,                "Chi2TrkY/F");
  fTree->Branch("Rigs",                 event->Rigs,                     "Rigs[5]/F");
  fTree->Branch("TrkPhi",               event->TrkPhi,                   "TrkPhi[2]/F");
  fTree->Branch("TrkTht",               event->TrkTht,                   "TrkTht[2]/F");
  fTree->Branch("TrkXcors",             event->TrkXcors,                 "TrkXcors[6]/F");
  fTree->Branch("TrkYcors",             event->TrkYcors,                 "TrkYcors[6]/F");
  fTree->Branch("TrkXhits",             event->TrkXhits,                 "TrkXhits[2]/F");
  fTree->Branch("TrkYhits",             event->TrkYhits,                 "TrkYhits[2]/F");
  fTree->Branch("TrkSigLay1",           &event->TrkSigLay1,              "TrkSigLay/F");
  fTree->Branch("TrkC",                 &event->TrkC,                    "TrkC/F");
  fTree->Branch("TrkCe",                &event->TrkCe,                   "TrkCe/F");

  fTree->Branch("RichC",                &event->RichC,                   "RichC/F");
  fTree->Branch("RichCe",               &event->RichCe,                  "RichCe/F");
  fTree->Branch("RichBeta",             &event->RichBeta,                "RichBeta/F");

  fTree->Branch("nEcalShr",             &event->nEcalShr,                "nEcalShr/B");
  fTree->Branch("EcalEnergy",           &event->EcalEnergy,              "EcalEnergy/F");
  fTree->Branch("EcalBDT",              &event->EcalBDT,                 "EcalBDT/F");
  fTree->Branch("EcalSum",              &event->EcalSum,                 "EcalSum/F");

  fTree->Branch("nTofHits",             &event->nTofHits,                "nTofHits/B");
  fTree->Branch("TofBeta",              &event->TofBeta,                 "TofBeta/F");
  fTree->Branch("TofC",                 &event->TofC,                    "TofC/F");
  fTree->Branch("TofCe",                &event->TofCe,                   "TofCe/F");
  fTree->Branch("TofMeanE",             &event->TofMeanE,                "TofMeanE/F");
  fTree->Branch("TofMaxE",              &event->TofMaxE,                 "TofMaxE/F");
  fTree->Branch("nAccHits",             &event->nAccHits,                "nAccHits/B");

  fTree->Branch("ParticleId",           &event->ParticleId,              "ParticleId/S");
  fTree->Branch("LR_Elec_Prot",         &event->LR_Elec_Prot,            "LR_Elec_Prot/F");
  fTree->Branch("LR_Heli_Elec",         &event->LR_Heli_Elec,            "LR_Heli_Elec/F");
  fTree->Branch("LR_Heli_Prot",         &event->LR_Heli_Prot,            "LR_Heli_Prot/F");
  fTree->Branch("LR_Elec_Prot_MC",      "std::vector<Float_t>",          event->LR_Elec_Prot_MC );
  fTree->Branch("LR_Heli_Elec_MC",      "std::vector<Float_t>",          event->LR_Heli_Elec_MC );
  fTree->Branch("LR_Elec_Prot_Rndm",    "std::vector<Float_t>",          event->LR_Elec_Prot_Rndm );

  fTree->Branch("XePressure",           &event->XePressure,              "XePressure/F");
  fTree->Branch("TrdInAcceptance",      &event->TrdInAcceptance,         "TrdInAcceptance/B");
  fTree->Branch("TrdActivePathLen",     &event->TrdActivePathLen,        "TrdActivePathLen/F");
  fTree->Branch("TrdCandidatePathLen",  &event->TrdCandidatePathLen,     "TrdCandidatePathLen/F");
  fTree->Branch("TrdActiveLayers",      &event->TrdActiveLayers,         "TrdActiveLayers/S");
  fTree->Branch("TrdCandidateLayers",   &event->TrdCandidateLayers,      "TrdCandidateLayers/S");
  fTree->Branch("TrdActiveStraws",      &event->TrdActiveStraws,         "TrdActiveStraws/S");
  fTree->Branch("TrdUnassignedHits",    &event->TrdUnassignedHits,       "TrdUnassignedHits/S");
  fTree->Branch("nTrdRawHits",          &event->nTrdRawHits,             "nTrdRawHits/S");

  fTree->Branch("TrdTrkDeltaAngleXZ",  &event->TrdTrkDeltaAngleXZ ,      "TrdTrkDeltaAngleXZ/F");
  fTree->Branch("TrdTrkDeltaAngleYZ",  &event->TrdTrkDeltaAngleYZ ,      "TrdTrkDeltaAngleYZ/F");
  fTree->Branch("TrdTrackCombinedFitXZChi2",   &event->TrdTrackCombinedFitXZChi2 ,   "TrdTrackCombinedFitXZChi2/F");
  fTree->Branch("TrdTrackCombinedFitYZChi2",   &event->TrdTrackCombinedFitYZChi2 ,   "TrdTrackCombinedFitYZChi2/F");
  fTree->Branch("TrdTrackStandAloneFitXZChi2", &event->TrdTrackStandAloneFitXZChi2 , "TrdTrackStandAloneFitXZChi2/F");
  fTree->Branch("TrdTrackStandAloneFitYZChi2", &event->TrdTrackStandAloneFitYZChi2 , "TrdTrackStandAloneFitYZChi2/F");
  
  fTree->Branch("TofCLay1",           &event->TofCLay1,              "TofCLay1/F");
  fTree->Branch("TofCLay2",           &event->TofCLay2,              "TofCLay2/F");
  fTree->Branch("TofCLay3",           &event->TofCLay3,              "TofCLay3/F");
  fTree->Branch("TofCLay4",           &event->TofCLay4,              "TofCLay4/F");
}

/** Initialize the ReducedEventWriter. NumberOfToyMcEvents sets the number of toy MC events to be used for TrdQt.
  */

void ACsoft::IO::ReducedEventWriter::Initialize(::IO::ReducedFileHeader* standardTreeFileHeader, unsigned int NumberOfToyMcEvents) {

  assert(standardTreeFileHeader);
  std::string outputFileName = ACsoft::IO::FileManager::MakeStandardRootFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);

  fFile = new TFile( outputFileName.c_str(), "RECREATE" );
  if( !fFile->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << outputFileName << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }

  fFile->cd();

  INFO_OUT << "Opening file for event tree: " << fFile->GetName() << std::endl;

  gROOT->ProcessLine("#include <vector>");
  if (fOnlyWriteExtraBranches)
    fTree = new TTree("ACtree_custom", "ACsoft ReducedEventWriter tree");
  else
    fTree = new TTree("ACtree", "ACsoft ReducedEventWriter tree");

  fReducedEvent = new ReducedEvent(standardTreeFileHeader);
  SetBranches(fReducedEvent);

  fTrdQt = new TrdQt(NumberOfToyMcEvents);
}

/** Extract variables from event \p evt and fill the tree.
  */
void ACsoft::IO::ReducedEventWriter::Process( const Analysis::Particle& particle ) {

  static bool gProcessedFirstEvent = false;
  if (!gProcessedFirstEvent) {
    gProcessedFirstEvent = true;
    const AC::Event* rawEvent = particle.RawEvent();
    assert(rawEvent);
    assert(fReducedEvent->FileHeader);
    fReducedEvent->FileHeader->Initialize(rawEvent->RunHeader()->GitSHA());
    fTree->GetUserInfo()->Add(fReducedEvent->FileHeader);
  }

  Clear();
  fReducedEvent->Run               = particle.Run();
  fReducedEvent->Event             = particle.Event();

  if (fOnlyWriteExtraBranches) {
    fTree->Fill();
    return;
  }

  fReducedEvent->RunTag            = particle.RunTag();
  fReducedEvent->EventTime         = particle.EventTime();
  fReducedEvent->EventRndm         = particle.EventRandom();
  fReducedEvent->BadRunTag         = particle.BadRunTag();

  fReducedEvent->TriggerRate       = particle.TriggerRate();
  fReducedEvent->TriggerLifeTime   = particle.TriggerLiveTime();
  fReducedEvent->LongitudeDeg      = particle.LongitudeDegree();
  fReducedEvent->LatitudeDeg       = particle.LatitudeDegree();
  fReducedEvent->MagLatDeg         = particle.MagneticLatitudeDegree();
  fReducedEvent->MagLongDeg        = particle.MagneticLongitudeDegree();
  
  fReducedEvent->GeomagneticMaxCutOff = particle.GeomagneticMaxCutOff();
  fReducedEvent->GeomagneticMaxCutOffCone = particle.GeomagneticMaxCutOffCone();
  
  fReducedEvent->nAccHits          = particle.NumberOfAccHits();
  
  //
  // Tracker
  //
  fReducedEvent->nTrkTrks = particle.NumberOfTrackerTracks();
  fReducedEvent->TrkHits = particle.TrackHits();
  fReducedEvent->TrackerLayerPatternClassification = particle.TrackerLayerPatternClassification();
  fReducedEvent->Rig = particle.Rigidity();
  fReducedEvent->eRinv = particle.InverseRigidityError();
  fReducedEvent->Chi2TrkX = particle.Chi2TrackerX();
  fReducedEvent->Chi2TrkY = particle.Chi2TrackerY();
  fReducedEvent->TrkC    = particle.TrackerCharge();
  fReducedEvent->TrkCe   = particle.TrackerChargeError();
  
  for (unsigned int i = 0; i < particle.Rigidities().size(); ++i)
    fReducedEvent->Rigs[i] = particle.Rigidities()[i];

  for (unsigned int i = 0; i < particle.TrackerPhi().size(); ++i)
    fReducedEvent->TrkPhi[i] = particle.TrackerPhi()[i];

  for (unsigned int i = 0; i < particle.TrackerTheta().size(); ++i)
    fReducedEvent->TrkTht[i] = particle.TrackerTheta()[i];

  for (unsigned int i = 0; i < particle.TrackerXcoordinates().size(); ++i)
    fReducedEvent->TrkXcors[i] = particle.TrackerXcoordinates()[i];
  
  for (unsigned int i = 0; i < particle.TrackerYcoordinates().size(); ++i)
    fReducedEvent->TrkYcors[i] = particle.TrackerYcoordinates()[i];

  for (unsigned int i = 0; i < particle.TrackerXhits().size(); ++i)
    fReducedEvent->TrkXhits[i] = particle.TrackerXhits()[i];

  for (unsigned int i = 0; i < particle.TrackerYhits().size(); ++i)
    fReducedEvent->TrkYhits[i] = particle.TrackerYhits()[i];

  fReducedEvent->TrkSigLay1 = particle.TrackerSignalLayer1();

  //
  // RICH
  //
  fReducedEvent->RichBeta = particle.RichBeta();
  fReducedEvent->RichC = particle.RichCharge();
  fReducedEvent->RichCe = particle.RichChargeError();
  
  //
  // ECAL
  //
  fReducedEvent->nEcalShr = particle.NumberOfEcalShower();
  fReducedEvent->EcalEnergy = particle.EcalEnergy();
  fReducedEvent->EcalBDT = particle.EcalBDT();
  fReducedEvent->EcalSum = particle.EcalSum();
  
  //
  // TOF
  //
  fReducedEvent->nTofHits = particle.NumberOfTofHits();
  fReducedEvent->TofC     = particle.TofCharge();
  fReducedEvent->TofCe    = particle.TofChargeError();
  fReducedEvent->TofBeta  = particle.BetaTof();
  fReducedEvent->TofMeanE = particle.TofMeanEnergy();
  fReducedEvent->TofMaxE  = particle.TofMaxEnergy();

  //
  // TRD
  //
  // ParticleID only used inside TrdQt for ToyMC Generation
  fReducedEvent->ParticleId = fTrdQt->ParticleID(particle.TrackerCharge(),particle.Rigidity());
  fTrdQt->Process(particle,fReducedEvent->ParticleId);
  
  fReducedEvent->TrdInAcceptance     = fTrdQt->IsInsideTrdGeometricalAcceptance();
  fReducedEvent->TrdActiveLayers     = fTrdQt->GetNumberOfActiveLayers();
  fReducedEvent->TrdCandidateLayers  = fTrdQt->GetNumberOfCandidateLayers();
  fReducedEvent->TrdActiveStraws     = fTrdQt->GetNumberOfActiveStraws();
  fReducedEvent->TrdActivePathLen    = fTrdQt->GetActivePathLength();
  fReducedEvent->TrdCandidatePathLen = fTrdQt->GetCandidatePathLength();
  fReducedEvent->TrdUnassignedHits   = fTrdQt->GetUnassignedHits().size();
  fReducedEvent->XePressure          = fTrdQt->QueryXenonPressure(particle.RawEvent()->EventHeader().TimeStamp());
  fReducedEvent->LR_Elec_Prot        = fTrdQt->LogLikelihoodRatioElectronProton();
  fReducedEvent->LR_Heli_Elec        = fTrdQt->LogLikelihoodRatioHeliumElectron();
  fReducedEvent->LR_Heli_Prot        = fTrdQt->LogLikelihoodRatioHeliumProton();
 
  fTrdQt->GetLogLikelihoodRatiosElectronsProtonsToyMC(*fReducedEvent->LR_Elec_Prot_MC);
  fTrdQt->GetLogLikelihoodRatiosHeliumElectronToyMC(*fReducedEvent->LR_Heli_Elec_MC);
  fTrdQt->GetLogLikelihoodRatioElectronProtonRandom(*fReducedEvent->LR_Elec_Prot_Rndm);

  fReducedEvent->nTrdRawHits         = particle.NumberOfTrdRawHits();
  
  fReducedEvent->TrdTrkDeltaAngleXZ = fTrdQt->GetTrdTrkAngle(0);
  fReducedEvent->TrdTrkDeltaAngleYZ = fTrdQt->GetTrdTrkAngle(1);
  fReducedEvent->TrdTrackCombinedFitXZChi2 = fTrdQt->GetTrdTrackCombinedFit(0).Chi2;
  fReducedEvent->TrdTrackCombinedFitYZChi2 = fTrdQt->GetTrdTrackCombinedFit(1).Chi2;
  fReducedEvent->TrdTrackStandAloneFitXZChi2 = fTrdQt->GetTrdTrackStandAloneFit(0).Chi2;
  fReducedEvent->TrdTrackStandAloneFitYZChi2 = fTrdQt->GetTrdTrackStandAloneFit(1).Chi2;

  fReducedEvent->TofCLay1 = particle.TofChargeLayer1();
  fReducedEvent->TofCLay2 = particle.TofChargeLayer2();
  fReducedEvent->TofCLay3 = particle.TofChargeLayer3();
  fReducedEvent->TofCLay4 = particle.TofChargeLayer4();

  fTree->Fill();
}

/** Write tree to output file.
  */
void ACsoft::IO::ReducedEventWriter::Finish() {

  if( !fFile || !fTree ) return;

  INFO_OUT << "Writing tree to file " << fFile->GetName() << " ..." << std::endl;

  fFile->cd();

  fTree->Write();

  fFile->Close();

}
