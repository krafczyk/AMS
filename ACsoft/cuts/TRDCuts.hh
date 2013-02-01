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
  CutTrdNumberOfRawHits(float minimumHits, float maximumHits ) :
    TwoSidedCut( "Number of TRD raw hits", minimumHits, maximumHits) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return ValueIsInRange(p.NumberOfTrdRawHits()); }

  CutTrdNumberOfRawHits() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrdNumberOfRawHits,1)
};


}

#endif
