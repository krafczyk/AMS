#ifndef BADRUNCUTS_HH
#define BADRUNCUTS_HH

#include "Cut.hh"
#include "BadRunManager.hh"

#ifndef __CINT__
#include "AnalysisParticle.hh"
#include "Event.h"
#endif

namespace Cuts {

/** Cut on bad run lists.
 *
 *  The subdetector to be taken into account is specified in the constructor.
 */
class CutBadRun : public Cut {
public:
  CutBadRun( ACsoft::Analysis::BadRunManager::SubD subdetector ) : Cut(),
    fSubd(subdetector)
  {
    fDescription = std::string("Bad Runs: ") + ACsoft::Analysis::BadRunManager::Self()->SubdToString(subdetector);
  }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& particle) {
    if (!particle.IsISS()) return true;
    return !ACsoft::Analysis::BadRunManager::Self()->IsBad(fSubd, particle);
  }

  CutBadRun() : Cut() {} // default constructor needed by ROOT, do not use!

protected:
  ACsoft::Analysis::BadRunManager::SubD fSubd;
  ClassDef(Cuts::CutBadRun,1)
};

}

#endif
