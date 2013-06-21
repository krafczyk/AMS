

#ifndef RICHCUTS_HH
#define RICHCUTS_HH

#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

  /** %Cut on %RICH ring of selected particle in the event. */
  class CutRichRingAvailable : public Cut
  {
  public:
    CutRichRingAvailable() : Cut("RICH ring available") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

	  return p.HasRichRing();
    }

    ClassDef(Cuts::CutRichRingAvailable, 1)
  };

/** %Cut on %RICH ring hits of selected particle in the event. */
class CutRichRingHits : public TwoSidedCut {
public:
  CutRichRingHits(float minimumHits, float maximumHits)
    : TwoSidedCut("RICH ring hits", minimumHits, maximumHits) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasRichRing() ) return true;
    return ValueIsInRange(p, p.RichRing()->NumberOfHits());
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

    if( !p.HasRichRing() ) return true;
    return ValueIsInRange(p, p.RichRing()->Beta()); }

  CutRichRingBeta() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutRichRingBeta,1)
};

/** %Cut on error of %RICH ring beta of selected particle in the event. */
class CutRichRingBetaError : public TwoSidedCut {
public:
  CutRichRingBetaError(float minimumError, float maximumError)
    : TwoSidedCut("RICH ring beta error" , minimumError, maximumError) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasRichRing() ) return true;
    return ValueIsInRange(p, p.RichRing()->BetaError()); }

  CutRichRingBetaError() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutRichRingBetaError,1)
};

/** %Cut on %RICH ring beta consistency (RichRingR vs RichRingBR) of selected particle in the event. */
class CutRichRingBetaConsistency : public TwoSidedCut {
public:
  CutRichRingBetaConsistency(float minimumDifference, float maximumDifference)
    : TwoSidedCut("RICH ring beta consistency" , minimumDifference, maximumDifference) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasRichRing() ) return true;
    return ValueIsInRange(p, p.RichRing()->BetaConsistency()); }

  CutRichRingBetaConsistency() : TwoSidedCut("", 0.0, 0.0) {}
  ClassDef(Cuts::CutRichRingBetaConsistency, 1)
};

  /** %Cut on %RICH ring Kolmogorov probability of selected particle in the event. */
  class CutRichRingProbability : public TwoSidedCut
  {
  public:
    CutRichRingProbability(float minimumProb, float maximumProb) : TwoSidedCut("RICH ring probability", minimumProb, maximumProb) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      return ValueIsInRange(p, p.RichRing()->Probability());
    }

    CutRichRingProbability() : TwoSidedCut("", 0.0, 0.0) {}
    ClassDef(Cuts::CutRichRingProbability, 1)
  };

/** %Cut on %RICH ring thickness of selected particle in the event. */
  class CutRichRingWidth : public TwoSidedCut
  {
  public:
    CutRichRingWidth(float minimumWidth, float maximumWidth) : TwoSidedCut("RICH ring width", minimumWidth, maximumWidth) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      return ValueIsInRange(p, p.RichRing()->Width());
    }

    CutRichRingWidth() : TwoSidedCut("", 0.0, 0.0) {}
    ClassDef(Cuts::CutRichRingWidth, 1)
  };

  /** %Cut on location of Cherenkov ring emission of selected particle in the event. */
  class CutRichRingAGL : public Cut
  {
  public:
    CutRichRingAGL() : Cut("RICH ring originated in AGL radiator") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      return !(p.RichRing()->IsNaF());
    }

    ClassDef(Cuts::CutRichRingAGL, 1)
  };

  /** %Cut on location of Cherenkov ring emission of selected particle in the event. */
  class CutRichRingNaF : public Cut
  {
  public:
    CutRichRingNaF() : Cut("RICH ring originated in NaF radiator") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      return p.RichRing()->IsNaF();
    }

    ClassDef(Cuts::CutRichRingNaF, 1)
  };

  /** %Cut on number of photoelectrons expected in %RICH ring produced in the Aerogel radiator of selected particle in the event. */
  class CutRichRingExpectedPhotoElectronsAGL : public Cut
  {
  public:
    CutRichRingExpectedPhotoElectronsAGL() : Cut("RICH ring minimum number of expected PE in AGL") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      
	  bool value = !p.RichRing()->IsNaF() && p.RichRing()->NExpectedPhotoElectrons() <= 2;
	  return !value;
    }

    ClassDef(Cuts::CutRichRingExpectedPhotoElectronsAGL, 1)
  }; 

  /** %Cut on number of photoelectrons expected in %RICH ring produced in the NaF radiator of selected particle in the event. */
  class CutRichRingExpectedPhotoElectronsNaF : public Cut
  {
  public:
    CutRichRingExpectedPhotoElectronsNaF() : Cut("RICH ring minimum number of expected PE in NaF") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      
	  bool value = p.RichRing()->IsNaF() && p.RichRing()->NExpectedPhotoElectrons() <= 2;
	  return !value;
    }

    ClassDef(Cuts::CutRichRingExpectedPhotoElectronsNaF, 1)
  };

  /** %Cut on fraction of photo electrons in the %RICH ring to collected photo electrons in the event of selected particle. This allows to efficiently discard events with fragmentation. */
  class CutRichRingFractionTotalPhotoElectrons : public Cut
  {
  public:
    CutRichRingFractionTotalPhotoElectrons() : Cut("RICH ring PE / total number of PE") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;

      // We wish to have more than the half of collected photons to be in the reconstructed RICH ring.
	  bool value = p.RichRing()->NPhotoElectrons()/p.RichRing()->NCollectedPhotoElectrons() <= 0.5;
	  return !value;
    }

    ClassDef(Cuts::CutRichRingFractionTotalPhotoElectrons, 1);
  };

  /** %Cut on distance to AGL tile border of Cherenkov ring emission point of selectred particle in the event. */
  class CutRichRingDistanceToTileBorder : public TwoSidedCut
  {
  public:
    CutRichRingDistanceToTileBorder(float minimumDistance, float maximumDistance) : TwoSidedCut("RICH ring originated too close to border", minimumDistance, maximumDistance) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p)
    {
      if(!p.HasRichRing()) return true;
      return ValueIsInRange(p, p.RichRing()->DistanceToTileBorder());
    }

    CutRichRingDistanceToTileBorder() : TwoSidedCut("", 0.0, 0.0) {}
    ClassDef(Cuts::CutRichRingDistanceToTileBorder, 1)
  }; 

}

#endif
