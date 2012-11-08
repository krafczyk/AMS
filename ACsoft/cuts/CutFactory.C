#include "CutFactory.hh"

#include "ECALCuts.hh"
#include "ParticleSelectionCuts.hh"
#include "PreselectionCuts.hh"
#include "TOFCuts.hh"
#include "TRDCuts.hh"
#include "TrackerCuts.hh"
#include "TriggerCuts.hh"


Cuts::Cut* Cuts::CutFactory::CreateCut(std::string ) const {

  // FIXME implement me!
  return 0;
}
