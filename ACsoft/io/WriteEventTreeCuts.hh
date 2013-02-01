#ifndef WriteEventTreeCuts_hh
#define WriteEventTreeCuts_hh

#include "TrackerCuts.hh"
#include "TriggerCuts.hh"
#include "TRDCuts.hh"
#include "BadRunManager.hh"
#include "BadRunCuts.hh"

#include "PreselectionCuts.hh"
#include "ParticleSelectionCuts.hh"
#include "Selector.hh"

// FIXME: Convert this file to a shared cut config file that can be read in, instead of hardcoding these cuts.

Cuts::Selector& WriteEventTreePreselectionCuts() {

  static Cuts::Selector* preselection = 0;
  if (!preselection) {
    preselection = new Cuts::Selector("Preselection");
    preselection->RegisterCut(new Cuts::CutSingleParticle);
    preselection->RegisterCut(new Cuts::CutSingleTrackerTrack);
    preselection->RegisterCut(new Cuts::CutEventConsistency);
    preselection->RegisterCut(new Cuts::CutHasTofBeta);
    preselection->RegisterCut(new Cuts::CutIsDowngoing);
    preselection->RegisterCut(new Cuts::CutTriggerInformationAvailable);
    preselection->RegisterCut(new Cuts::CutTriggerLiveTime);
    preselection->RegisterCut(new Cuts::CutHasTrackerTrack);
    preselection->RegisterCut(new Cuts::CutHasTrackerTrackFit);
    preselection->RegisterCut(new Cuts::CutIsGoodCentralInnerTrackerTrack);
    preselection->RegisterCut(new Cuts::CutTrackerTrackGoodnessOfFit(0.01,15.0));
    preselection->RegisterCut(new Cuts::CutTrackerTrackFitAbsoluteRigidity(0.4, 1.e5));
  }
    
  return *preselection;
}

Cuts::Selector& WriteEventTreeBadRunCuts() {

  static Cuts::Selector* badruns = 0;
  if (!badruns) {
    badruns = new Cuts::Selector("Bad runs");
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::General));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::DAQ ));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::TRD));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::TOF));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::ACC));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::Tracker));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::Rich));
    badruns->RegisterCut(new Cuts::CutBadRun(Analysis::BadRunManager::Ecal));
  }

  return *badruns;
}

}

#endif // WriteEventTreeCuts_hh
