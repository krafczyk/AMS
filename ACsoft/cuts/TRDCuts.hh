#ifndef TRDCUTS_HH
#define TRDCUTS_HH

#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {
 
/** Cut on TRD number of raw hits in the event. */
class CutTrdNumberOfRawHits : public TwoSidedCut {
public:
  CutTrdNumberOfRawHits(float minimumHits = 12., float maximumHits = 100.) :// FIXME remove defaults (remember to introduce private constructor with zero arguments!)
    TwoSidedCut( "Number of TRD raw hits", minimumHits, maximumHits) { }

  virtual bool TestCondition(const Analysis::Particle& p) { return ValueIsInRange(p.RawEvent()->TRD().RawHits().size()); }

  ClassDef(Cuts::CutTrdNumberOfRawHits,1)
};


}

#endif
