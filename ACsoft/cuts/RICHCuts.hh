#ifndef RICHCUTS_HH
#define RICHCUTS_HH

#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** %Cut on %RICH ring hits of selected particle in the event. */
class CutRichRingHits : public TwoSidedCut {
public:
  CutRichRingHits(float minimumHits, float maximumHits)
    : TwoSidedCut("RICH ring hits", minimumHits, maximumHits) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasMainRichRing() ) return true;
    return ValueIsInRange(p.MainRichRing()->NumberOfHits());
  }

  CutRichRingHits() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutRichRingHits,1)
};

/** %Cut on %RICH ring beta of selected particle in the event. */
class CutRichRingBeta : public TwoSidedCut {
public:
  CutRichRingBeta(float minimumBeta, float maximumBeta)
    : TwoSidedCut("RICH ring beta" , minimumBeta, maximumBeta) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasMainRichRing() ) return true;
    return ValueIsInRange(p.MainRichRing()->Beta()); }

  CutRichRingBeta() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutRichRingBeta,1)
};

}

#endif
