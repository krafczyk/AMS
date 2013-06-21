#include "ReducedEventWriter.hh"

#include "Event.h"
#include "AnalysisParticle.hh"
#include "ObjectManager.hh"
#include "ConfigHandler.hh"
#include "FileManager.hh"
#include "TrdHitFactory.hh"
#include "TrackFactory.hh"
#include "SplineTrack.hh"
#include "TrdQt.hh"
#include "ReducedEvent.hh"
#include "TrdPdfLookup.hh"
#include "ReducedFileHeader.hh"
#include "TrdCandidateMatching.hh"
#include "ParticleSelectionCuts.hh"

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

  fTree->Branch("RunTag",                   &event->RunTag,                   "RunTag/I");
  fTree->Branch("EventTime",                &event->EventTime,                "EventTime/D");
  fTree->Branch("TriggerFlags",             &event->TriggerFlags,             "TriggerFlags/b");
  fTree->Branch("TriggerRate",              &event->TriggerRate,              "TriggerRate/F");
  fTree->Branch("TriggerLifeTime",          &event->TriggerLifeTime,          "TriggerLifeTime/F");
  fTree->Branch("LongitudeDeg",             &event->LongitudeDeg,             "LongitudeDeg/F");
  fTree->Branch("LatitudeDeg",              &event->LatitudeDeg,              "LatitudeDeg/F");
  fTree->Branch("MagLatDeg",                &event->MagLatDeg,                "MagLatDeg/F");
  fTree->Branch("MagLongDeg",               &event->MagLongDeg,               "MagLongDeg/F");
  fTree->Branch("GeomagneticMaxCutOff",     &event->GeomagneticMaxCutOff,     "GeomagneticMaxCutOff/F");

  fTree->Branch("nTrkTrks",             &event->nTrkTrks,                "nTrkTrks/B");
  fTree->Branch("TrkHits",              &event->TrkHits,                 "TrkHits/I");
  fTree->Branch("TrackerLayerPatternClassification", &event->TrackerLayerPatternClassification, "TrackerLayerPatternClassification/S");
  fTree->Branch("Rig",                  &event->Rig,                     "Rig/F");
  fTree->Branch("eRinv",                &event->eRinv,                   "eRinv/F");
  fTree->Branch("Chi2TrkX",             &event->Chi2TrkX,                "Chi2TrkX/F");
  fTree->Branch("Chi2TrkY",             &event->Chi2TrkY,                "Chi2TrkY/F");
  fTree->Branch("Rigs",                 event->Rigs,                     "Rigs[5]/F");
  fTree->Branch("TrkPhi",               event->TrkPhi,                   "TrkPhi[3]/F");
  fTree->Branch("TrkTht",               event->TrkTht,                   "TrkTht[3]/F");
  fTree->Branch("TrkXcors",             event->TrkXcors,                 "TrkXcors[6]/F");
  fTree->Branch("TrkYcors",             event->TrkYcors,                 "TrkYcors[6]/F");
  fTree->Branch("TrkXhits",             event->TrkXhits,                 "TrkXhits[2]/F");
  fTree->Branch("TrkYhits",             event->TrkYhits,                 "TrkYhits[2]/F");
  fTree->Branch("TrkC",                 &event->TrkC,                    "TrkC/F");
  fTree->Branch("TrkCe",                &event->TrkCe,                   "TrkCe/F");
  fTree->Branch("TrkCCLikelihood",      &event->TrkCCLikelihood,         "TrkCCLikelihood/F");

  fTree->Branch("RichC",                &event->RichC,                   "RichC/F");
  fTree->Branch("RichCe",               &event->RichCe,                  "RichCe/F");
  fTree->Branch("RichBeta",             &event->RichBeta,                "RichBeta/F");

  fTree->Branch("nEcalShr",             &event->nEcalShr,                "nEcalShr/B");
  fTree->Branch("EcalEnergy",           &event->EcalEnergy,              "EcalEnergy/F");
  fTree->Branch("EcalBDT",              &event->EcalBDT,                 "EcalBDT/F");
  fTree->Branch("EcalSum",              &event->EcalSum,                 "EcalSum/F");
  fTree->Branch("EcalCog",              &event->EcalCog,                 "EcalCog[3]/F");
  fTree->Branch("EcalTheta",            &event->EcalTheta,               "EcalTheta/F");
  fTree->Branch("EcalPhi",              &event->EcalPhi,                 "EcalPhi/F");

  fTree->Branch("nTofHits",             &event->nTofHits,                "nTofHits/B");
  fTree->Branch("TofBeta",              &event->TofBeta,                 "TofBeta/F");
  fTree->Branch("TofC",                 &event->TofC,                    "TofC/F");
  fTree->Branch("TofCe",                &event->TofCe,                   "TofCe/F");
  fTree->Branch("nAccHits",             &event->nAccHits,                "nAccHits/B");

  fTree->Branch("ParticleId",           &event->ParticleId,              "ParticleId/S");
  fTree->Branch("TrdKLikelihoodRatios", "std::vector<Float_t>",          event->TrdKLikelihoodRatios);
  fTree->Branch("TrdQtLikelihoodRatios","std::vector<Float_t>",          event->TrdQtLikelihoodRatios);
  fTree->Branch("TrdStraws",            "std::vector<UShort_t>",         event->TrdStraws);
  fTree->Branch("Trd2DeDx",             "std::vector<UShort_t>",         event->Trd2DeDx);


  fTree->Branch("TrackerCharges",       "std::vector<Float_t>",          event->TrackerCharges);
  fTree->Branch("TofCharges",           "std::vector<Float_t>",          event->TofCharges);

  fTree->Branch("XePressure",           &event->XePressure,              "XePressure/S");
  fTree->Branch("TrdInAcceptance",      &event->TrdInAcceptance,         "TrdInAcceptance/O");
  fTree->Branch("TrdActivePathLen",     &event->TrdActivePathLen,        "TrdActivePathLen/F");
  fTree->Branch("TrdCandidatePathLen",  &event->TrdCandidatePathLen,     "TrdCandidatePathLen/F");
  fTree->Branch("TrdActiveLayers",      &event->TrdActiveLayers,         "TrdActiveLayers/b");
  fTree->Branch("TrdCandidateLayers",   &event->TrdCandidateLayers,      "TrdCandidateLayers/b");
  fTree->Branch("TrdUnassignedHits",    &event->TrdUnassignedHits,       "TrdUnassignedHits/b");
  fTree->Branch("TrdKActiveHits",       &event->TrdKActiveHits,          "TrdKActiveHits/b");
  fTree->Branch("TrdKUnassignedHits",   &event->TrdKUnassignedHits,      "TrdKUnassignedHits/b");
  fTree->Branch("nTrdRawHits",          &event->nTrdRawHits,             "nTrdRawHits/s");

  fTree->Branch("McParticleId",         &event->McParticleId,            "McParticleId/S");
  fTree->Branch("McMomentum",           &event->McMomentum,              "McMomentum/F");
  fTree->Branch("McMinimumMomentum",    &event->McMinimumMomentum,       "McMinimumMomentum/F");
  fTree->Branch("McMaximumMomentum",    &event->McMaximumMomentum,       "McMaximumMomentum/F");
  fTree->Branch("McNumberOfTriggeredEvents", &event->McNumberOfTriggeredEvents, "McNumberOfTriggeredEvents/l");

}

/** Initialize the ReducedEventWriter. NumberOfToyMcEvents sets the number of toy MC events to be used for TrdQt.
  */

void ACsoft::IO::ReducedEventWriter::Initialize(::IO::ReducedFileHeader* standardTreeFileHeader, unsigned int NumberOfToyMcEvents) {

  assert(standardTreeFileHeader);
  std::string outputFileName = Utilities::ObjectManager::MakeStandardRootFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);

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
    assert(fReducedEvent->FileHeader);
    if (const AC::Event* rawEvent = particle.RawEvent())
      fReducedEvent->FileHeader->Initialize(rawEvent->RunHeader()->GitSHA());
    else
      fReducedEvent->FileHeader->Initialize("No Git SHA available, as this tree was created from an ACROOT tree");
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

  fReducedEvent->TriggerFlags      = particle.TriggerFlags();
  fReducedEvent->TriggerRate       = particle.TriggerRate();
  fReducedEvent->TriggerLifeTime   = particle.TriggerLiveTime();
  fReducedEvent->LongitudeDeg      = particle.LongitudeDegree();
  fReducedEvent->LatitudeDeg       = particle.LatitudeDegree();
  fReducedEvent->MagLatDeg         = particle.MagneticLatitudeDegree();
  fReducedEvent->MagLongDeg        = particle.MagneticLongitudeDegree();

  if (particle.IsMC())
    fReducedEvent->GeomagneticMaxCutOff = 0;
  else
    fReducedEvent->GeomagneticMaxCutOff = particle.GeomagneticMaxCutOff();

  fReducedEvent->nAccHits          = particle.NumberOfAccHits();

  //
  // Tracker
  //
  fReducedEvent->nTrkTrks         = particle.NumberOfTrackerTracks();
  fReducedEvent->TrkHits          = particle.TrackHits();
  fReducedEvent->TrackerLayerPatternClassification = particle.TrackerLayerPatternClassification();
  fReducedEvent->Rig              = particle.Rigidity();
  fReducedEvent->eRinv            = particle.InverseRigidityError();
  fReducedEvent->Chi2TrkX         = particle.Chi2TrackerX();
  fReducedEvent->Chi2TrkY         = particle.Chi2TrackerY();
  fReducedEvent->TrkC             = particle.TrackerCharge();
  fReducedEvent->TrkCe            = particle.TrackerChargeError();
  fReducedEvent->TrkCCLikelihood  = particle.TrackerChargeConfusionLikelihood();

  // FIXME: This is slow, the vector should be stored in a local variable here!
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

  TVector3 ecalCog = particle.EcalCentreOfGravity();
  fReducedEvent->EcalCog[0] = ecalCog.X();
  fReducedEvent->EcalCog[1] = ecalCog.Y();
  fReducedEvent->EcalCog[2] = ecalCog.Z();

  particle.FillTrackerCharges(*fReducedEvent->TrackerCharges);

  //
  // RICH
  //
  fReducedEvent->RichBeta = particle.RichBeta();
  fReducedEvent->RichC    = particle.RichCharge();
  fReducedEvent->RichCe   = particle.RichChargeError();

  //
  // ECAL
  //
  fReducedEvent->nEcalShr   = particle.NumberOfEcalShower();
  fReducedEvent->EcalEnergy = particle.EcalEnergy();
  fReducedEvent->EcalBDT    = particle.EcalBDT();
  fReducedEvent->EcalSum    = particle.EcalSum();
  fReducedEvent->EcalTheta  = particle.EcalTheta();
  fReducedEvent->EcalPhi    = particle.EcalPhi();

  //
  // TOF
  //
  fReducedEvent->nTofHits = particle.NumberOfTofHits();
  fReducedEvent->TofC     = particle.TofCharge();
  fReducedEvent->TofCe    = particle.TofChargeError();
  fReducedEvent->TofBeta  = particle.BetaTof();
  particle.FillTofCharges(*fReducedEvent->TofCharges);

  //
  // TRD
  //
  // ParticleID only used inside TrdQt for ToyMC Generation
  fReducedEvent->ParticleId = Cuts::ParticleID(particle.TrackerCharge(),particle.Rigidity());
  fTrdQt->Process(particle,fReducedEvent->ParticleId);

  fReducedEvent->TrdInAcceptance     = fTrdQt->IsInsideTrdGeometricalAcceptance();
  fReducedEvent->TrdActiveLayers     = fTrdQt->GetNumberOfActiveLayers();
  fReducedEvent->TrdCandidateLayers  = fTrdQt->GetNumberOfCandidateLayers();
  fReducedEvent->TrdActivePathLen    = fTrdQt->GetActivePathLength();
  fReducedEvent->TrdCandidatePathLen = fTrdQt->GetCandidatePathLength();
  fReducedEvent->TrdUnassignedHits   = fTrdQt->GetNumberOfUnassignedHits();

  const std::vector<Analysis::TRDCandidateHit>trdHits = fTrdQt->GetCandidateHits();
  for (unsigned int i=0; i<trdHits.size(); i++) {
    fReducedEvent->TrdStraws->push_back(trdHits.at(i).straw);
    // we store 2*dEdX to better use the dynamic range of the UShort_t
    fReducedEvent->Trd2DeDx->push_back(std::min(TMath::Nint(2.0*trdHits.at(i).deDx),std::numeric_limits<unsigned short>::max() - 1));
  }

  bool queryOk = false;
  fReducedEvent->XePressure          = TMath::Nint(fTrdQt->QueryXenonPressure(particle.TimeStamp(),queryOk));
  if(!queryOk) fReducedEvent->XePressure = 0;
  fReducedEvent->nTrdRawHits         = particle.NumberOfTrdRawHits();
  fReducedEvent->TrdQtLikelihoodRatios->push_back(fTrdQt->LogLikelihoodRatioElectronProton());
  fReducedEvent->TrdQtLikelihoodRatios->push_back(fTrdQt->LogLikelihoodRatioHeliumProton());
  fReducedEvent->TrdQtLikelihoodRatios->push_back(fTrdQt->LogLikelihoodRatioHeliumElectron());
  fTrdQt->GetTrdKLikelihoodRatios(particle, *fReducedEvent->TrdKLikelihoodRatios);

  fReducedEvent->TrdKActiveHits      = std::min(int(fTrdQt->TrdKActiveHits(particle)),std::numeric_limits<unsigned char>::max() - 1);
  fReducedEvent->TrdKUnassignedHits  = std::min(int(fTrdQt->TrdKUnassignedHits(particle)),std::numeric_limits<unsigned char>::max() - 1);

  fReducedEvent->McParticleId        = particle.McParticleId();
  fReducedEvent->McMomentum          = particle.McMomentum();
  fReducedEvent->McMaximumMomentum   = particle.McMaximumMomentum();
  fReducedEvent->McMinimumMomentum   = particle.McMinimumMomentum();
  fReducedEvent->McNumberOfTriggeredEvents = particle.McNumberOfTriggeredEvents();

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
