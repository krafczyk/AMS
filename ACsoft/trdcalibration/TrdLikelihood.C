
#include "TrdLikelihood.hh"

#include <gain_functions.hh>
#include <ConfigHandler.hh>
#include <TrdDetector.hh>
#include <TrdModule.hh>
#include <TrdHit.hh>
#include <TrdCandidateMatching.hh>
#include <TrackFactory.hh>
#include <SplineTrack.hh>
#include <TrdHitFactory.hh>
#include <TimeHistogramManager.hh>
#include <FileManager.hh>
#include <StringTools.hh>
#include <Statistics.hh>
#include <ParticleSelectionCuts.hh>
#include "Utilities.hh"
#include "TrdQt.hh"

#include "Event.h"
#include "RunHeader.h"
#include "AnalysisParticle.hh"

#include <assert.h>

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>

#include <iostream>
#include <vector>

#define DEBUG 0
#define INFO_OUT_TAG "TrdLikelihood> "
#include <debugging.hh>

Calibration::TrdLikelihood::TrdLikelihood( Utilities::ConfigHandler* config ) :
  fTrd(0),
  fIsInitialized(false),
  fVersion(0),
  fMinPathLength(AC::AMSGeometry::TRDTubeMinPathLength),
  fUseActiveStraws(false),
  fTrdRawHits(0),
  fTrdSignalHeightPerStraw(0),
  fTrdHitCountPerStraw(0),
  fTrdHitsOnTrackerTrack(0),
  fTOFCharge_vs_TrackerCharge(0) {

  for (int i = 0; i < 3; ++i)
    fTrdGainPerParticle[i] = 0;

  for (int i = 0; i < 2; ++i) {
    fTrackerCharge_vs_rigidity[i] = 0;
    fTrackerCharge_vs_time[i] = 0;
    fTOFCharge_vs_rigidity[i] = 0;
    fTOFCharge_vs_time[i] = 0;
  }

  // read options from cfghandler here and save them in the class if needed
  // ...

  // FIXME only main() is supposed to set options in the "OPTIONS" category!
  config->GetValue( "OPTIONS", "version", fVersion, "Set the version to store in the ROOT file.");
  config->GetValue( "OPTIONS", "minPathLength", fMinPathLength, "Override the default 0.1 mm path length for candidate matching." );
  config->GetValue( "OPTIONS", "useActiveStraws", fUseActiveStraws, "Enable activeStraws=true for candidate matching." );

  fTrdCandidateMatching = new Analysis::TrdCandidateMatching;

  // TRD and histograms are not initialized here because we need the information from the TimeHistogramsManager, only available after FileManager has been initialized 
}

void Calibration::TrdLikelihood::Initialize() {

  assert(!fIsInitialized);

  INFO_OUT << "Constructing TRD" << std::endl;

  // construct TRD
  fTrd = new Detector::Trd();

  fTrdRawHits = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdRawHits", "Number of TRD raw hits", "N", 101, -0.5, 100.5 );
  fTrdSignalHeightPerStraw = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdSignalHeightPerStraw", "Signal height per straw", "Straw number", 5248, -0.5, 5247.5 );
  fTrdHitCountPerStraw= Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdHitCountPerStraw", "Number of TRD hits per straw", "Straw number", 5248, -0.5, 5247.5 );
  fTrdHitsOnTrackerTrack = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdHitsOnTrackerTrack", "Number of TRD hits on tracker track", "N", 31, -0.5, 30.5 );
  fTrdGainPerParticle[0] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdGainElectrons", "Gain stability, electrons", "ADC counts [ADC/cm]", 501, -1, 1001 );
  fTrdGainPerParticle[1] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdGainProtons", "Gain stability, protons", "ADC counts [ADC/cm]", 501, -1, 1001 );
  fTrdGainPerParticle[2] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrdGainHelium", "Gain stability, helium", "ADC counts [ADC/cm]", 501, -1, 1001 );
 
  std::vector<double> v_RigLog = Utilities::GenerateLogBinning(30, 2, 600);
  fTrackerCharge_vs_rigidity[0] = new TH2F("fTrackerCharge_vs_rig_Negative", "Tracker Charge vs. Rigidity, for negative rigidities; Rigiditiy [GV]; Charge [e]", v_RigLog.size()-1, v_RigLog.data(), 100, 0.0, 10.0);
  fTrackerCharge_vs_rigidity[1] = new TH2F("fTrackerCharge_vs_rig_Positive", "Tracker Charge vs. Rigidity, for positive rigidities; Rigiditiy [GV]; Charge [e]", v_RigLog.size()-1, v_RigLog.data(), 100, 0.0, 10.0);
  fTrackerCharge_vs_time[0] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrackerCharge_vs_time_Negative", "Tracker Charges vs. Time, for negative rigidities", "Charge [e]", 100, 0, 10 );
  fTrackerCharge_vs_time[1] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTrackerCharge_vs_time_Positive", "Tracker Charges vs. Time, for positive rigidities", "Charge [e]", 100, 0, 10 );

  fTOFCharge_vs_rigidity[0] = new TH2F("fTOFCharge_vs_rig_Negative", "TOF Charge vs. Rigidity, for negative rigidities; Rigiditiy [GV]; Charge [e]", v_RigLog.size()-1, v_RigLog.data(), 100, 0.0, 10.0);
  fTOFCharge_vs_rigidity[1] = new TH2F("fTOFCharge_vs_rig_Positive", "TOF Charge vs. Rigidity, for positive rigidities; Rigiditiy [GV]; Charge [e]", v_RigLog.size()-1, v_RigLog.data(), 100, 0.0, 10.0);
  fTOFCharge_vs_time[0] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTOFCharge_vs_time_Negative", "TOF Charges vs. Time, for negative rigidities", "Charge [e]", 100, 0, 10 );
  fTOFCharge_vs_time[1] = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2F>( "fTOFCharge_vs_time_Positive", "TOF Charges vs. Time, for positive rigidities", "Charge [e]", 100, 0, 10 );

  fTOFCharge_vs_TrackerCharge = new TH2F("fTOFCharge_vs_TrackerCharge", "TOF Charges vs. Tracker charge", 100, 0.0, 10.0, 100, 0.0, 10.0);

  fIsInitialized = true;
}

/**
  * \todo split events into calibration and test sample
  */
void Calibration::TrdLikelihood::Process( const Analysis::Particle& particle ) {

  if(!fIsInitialized)
    Initialize();
 
  TTimeStamp evttime = particle.RawEvent()->EventHeader().TimeStamp();

  const AC::TrackerTrack* trktrk = particle.MainTrackerTrack();
  const AC::TrackerTrackFit* trackFit = particle.MainTrackerTrackFit();
  assert(trktrk);
  assert(trackFit);
  float rigidity = trackFit->Rigidity();
  float aRig = fabs(rigidity);

  fTrdCandidateMatching->Process(particle, fMinPathLength, fUseActiveStraws);

  AC::ChargeAndError trkChargeAndError = trktrk->GetChargeAndErrorNew(3);
  float TrkCharge = trkChargeAndError.charge;
  short IdPart = TrdQt::ParticleID(TrkCharge, rigidity);
  if (IdPart < 0 || IdPart >= Utilities::TrdPdfLookup::fNumberOfParticles) return;

  DEBUG_OUT << "IdPart " << IdPart << " rig " << rigidity << std::endl;

  int index = 0;
  if (rigidity>0) index = 1;

  if (trkChargeAndError.error != -1) {
    fTrackerCharge_vs_rigidity[index]->Fill(aRig, TrkCharge);
    fTrackerCharge_vs_time[index]->Fill(evttime, TrkCharge);
  }

  const AC::TOFBeta* tofb = particle.TofBeta();
  assert(tofb);
  unsigned int tofbChargesSize = tofb->ChargesNew().size();
  if (tofbChargesSize > 1) {
    AC::ChargeAndError tofChargeAndError = tofb->GetChargeAndErrorNew();
    fTOFCharge_vs_rigidity[index]->Fill(aRig, tofChargeAndError.charge);
    fTOFCharge_vs_time[index]->Fill(evttime, tofChargeAndError.charge);
    if (TrkCharge != -1) fTOFCharge_vs_TrackerCharge->Fill(tofChargeAndError.charge, TrkCharge);
  }

  unsigned int numberOfRawHits = particle.RawEvent()->TRD().RawHits().size();
  fTrdRawHits->Fill(evttime, numberOfRawHits);

  const std::vector<Analysis::TRDCandidateHit> candidateHits = fTrdCandidateMatching->CandidateHits();
  unsigned int numberOfCandidateHits = candidateHits.size();
  fTrdHitsOnTrackerTrack->Fill(evttime, fTrdCandidateMatching->MatchedHits());

  if (!fTrdCandidateMatching->UsefulForTrdParticleId()) return;
  DEBUG_OUT << "Survives cand matching" << std::endl;

  for( unsigned int i=0 ; i<numberOfCandidateHits; ++i ){
    const Analysis::TRDCandidateHit& hit = candidateHits[i];
    unsigned short layer = AC::TRDStrawToLayer(hit.straw);

    float dEdX = hit.deDx;
    fTrd->StoreADCSpectra(evttime, IdPart, dEdX, aRig, layer);
    fTrdSignalHeightPerStraw->Fill(evttime, hit.straw, dEdX);
    fTrdHitCountPerStraw->Fill(evttime, hit.straw);
  }
}

void Calibration::TrdLikelihood::WriteResultsToCurrentFile() {

  AC::ACQtPDFLookupFileIdentifier* identifier = new AC::ACQtPDFLookupFileIdentifier;
  identifier->fUseActiveStrawsForCandidateMatching = fUseActiveStraws;
  identifier->fMinimumPathLengthForCandidateMatching = fMinPathLength;

#ifdef ACPRODUCTIONSHA
  identifier->fGitSHA = ACPRODUCTIONSHA;
#else
  identifier->fGitSHA = AC::calculateSoftwareVersionHash();
#endif

  identifier->fVersion = fVersion;
  identifier->Write();

  if (!fTrd) return;
  fTrd->WriteHistosToCurrentFile();

  fTrdRawHits->Write();
  fTrdSignalHeightPerStraw->Write();
  fTrdHitCountPerStraw->Write();
  fTrdHitsOnTrackerTrack->Write();
  fTrdGainPerParticle[0]->Write();
  fTrdGainPerParticle[1]->Write();
  fTrdGainPerParticle[2]->Write();

  fTOFCharge_vs_TrackerCharge->Write();
  for (int i = 0; i < 2; ++i) {
    fTrackerCharge_vs_rigidity[i]->Write();
    fTrackerCharge_vs_time[i]->Write();
    fTOFCharge_vs_rigidity[i]->Write();
    fTOFCharge_vs_time[i]->Write();
  }
}
