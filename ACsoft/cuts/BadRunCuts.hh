#ifndef BADRUNCUTS_HH
#define BADRUNCUTS_HH

#include "Cut.hh"
#include "BadRunManager.hh"

#ifndef __CINT__
#include "Event.h"
#endif

#include "AnalysisParticle.hh"


namespace Cuts {

/** Cut on bad run lists.
 *
 *  The subdetector to be taken into account is specified in the constructor.
 */
class CutBadRun : public Cut {
public:
  CutBadRun( Analysis::BadRunManager::SubD subdetector ) : Cut(),
    fSubd(subdetector)
  {
    fDescription = std::string("Bad Runs: ") + Analysis::BadRunManager::Self()->SubdToString(subdetector);
  }

  virtual bool TestCondition(const Analysis::Particle& particle) {
    return !Analysis::BadRunManager::Self()->IsBad(fSubd,particle);
  }

  CutBadRun() : Cut() {} // default constructor needed by ROOT, do not use!

protected:
  Analysis::BadRunManager::SubD fSubd;
  ClassDef(Cuts::CutBadRun,1)
};

}

#endif
