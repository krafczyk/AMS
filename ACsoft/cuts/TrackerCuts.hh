#ifndef TRACKERCUTS_HH
#define TRACKERCUTS_HH

#include "Cut.hh"
#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace  Cuts {

/** Cut on Tracker charge availability. */
class CutTrackerChargeAvailable : public Cut {
public:
  CutTrackerChargeAvailable() : Cut( "Tracker charge available" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {
    if( !p.MainTrackerTrack() ) return true;
    return p.MainTrackerTrack()->ChargesNew().size() > 0;
  }

  ClassDef(Cuts::CutTrackerChargeAvailable,1)
};

/** Cut on Tracker Charge */
class CutTrackerCharge : public TwoSidedCut {
public:
  CutTrackerCharge( float minimum, float maximum )
    : TwoSidedCut( "Tracker charge", minimum, maximum) { }

  virtual bool TestCondition(const Analysis::Particle& p) {
    if( !p.MainTrackerTrack() ) return true;
    if( !p.MainTrackerTrack()->ChargesNew().size() ) return true;
    return ValueIsInRange(p.MainTrackerTrack()->ChargesNew()[0].charge);
  }

  CutTrackerCharge() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerCharge,1)
};

/** Require at least one outer tracker layer for the selected particle. */
class CutTrackerLayerOneOrNine : public Cut {
public:
  CutTrackerLayerOneOrNine() : Cut( "Tracker Y hit on layers 1 or 9" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrack() ) return true;
    std::bitset<10> layers = p.MainTrackerTrack()->LayerYPatternBitset();
    return layers.test(1) || layers.test(9);
  }

  ClassDef(Cuts::CutTrackerLayerOneOrNine,1)
};

/** Make sure track associated with the selected particle is a good track (XY hit on layers 2 and (3 or 4) and (5 or 6) and (7 or 8)).
  * \sa AC::TrackerTrack::IsGoodInnerTrack()
  */
class CutIsGoodInnerTrackerTrack : public Cut {
public:
  CutIsGoodInnerTrackerTrack() : Cut( "Good inner track (2&&(3||4)&&(5||6)&&(7||8))" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrack() ) return true;
    return p.MainTrackerTrack()->IsGoodInnerTrack();
  }

  ClassDef(Cuts::CutIsGoodInnerTrackerTrack,1)
};

/** Make sure track associated with the selected particle is a good central track (XY hit on (3 or 4) and (5 or 6) and (7 or 8)).
  * \sa AC::TrackerTrack::IsGoodCentralInnerTrack()
  */
class CutIsGoodCentralInnerTrackerTrack : public Cut {
public:
  CutIsGoodCentralInnerTrackerTrack() : Cut( "Good central inner track ((3||4)&&(5||6)&&(7||8))" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrack() ) return true;
    return p.MainTrackerTrack()->IsGoodCentralInnerTrack();
  }

  ClassDef(Cuts::CutIsGoodCentralInnerTrackerTrack,1)
};

/** %Cut on chisquared Y of track fit belonging to the selected track. */
class CutTrackerTrackGoodnessOfFit : public TwoSidedCut {
public:
  CutTrackerTrackGoodnessOfFit(float minimumChi2, float maximumChi2 ) // FIXME remove defaults
    : TwoSidedCut( "Tracker track goodness of fit (chi2)", minimumChi2, maximumChi2) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrackFit() ) return true;
    return ValueIsInRange(p.MainTrackerTrackFit()->ChiSquareNormalizedY());
  }
  
  CutTrackerTrackGoodnessOfFit() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackGoodnessOfFit,1)
};

/** Make sure there are at least three X-hits for track of the selected particle. */
class CutTrackerTrackHasAtLeastThreeXHits : public Cut {
public:
  CutTrackerTrackHasAtLeastThreeXHits() : Cut( "Tracker track has three X-hits"){ }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrack() ) return true;
    return p.MainTrackerTrack()->NumberOfHitsX() >= 3;
  }

  ClassDef(Cuts::CutTrackerTrackHasAtLeastThreeXHits,1)
};

/** %Cut on rigidity (including charge sign!) of selected track fit in event. */
class CutTrackerTrackFitRigidity : public TwoSidedCut {
public:
  CutTrackerTrackFitRigidity(float minimumRigidity, float maximumRigidity )
    : TwoSidedCut( "Track rigidity", minimumRigidity, maximumRigidity) { }
  
  virtual bool TestCondition(const Analysis::Particle& p) {
    
    if( !p.MainTrackerTrackFit() ) return true;
    return ValueIsInRange(p.MainTrackerTrackFit()->Rigidity());
  }
  
  CutTrackerTrackFitRigidity() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackFitRigidity,1)
};
  
/** %Cut on absolute rigidity of selected track fit in event. */
class CutTrackerTrackFitAbsoluteRigidity : public TwoSidedCut {
public:
  CutTrackerTrackFitAbsoluteRigidity(float minimumRigidity, float maximumRigidity )
    : TwoSidedCut( "Track rigidity", minimumRigidity, maximumRigidity) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainTrackerTrackFit() ) return true;
    return ValueIsInRange(fabs(p.MainTrackerTrackFit()->Rigidity()));
  }

  CutTrackerTrackFitAbsoluteRigidity() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackFitAbsoluteRigidity,1)
};

// FIXME justify these numbers! (documentation?)
static const float gMDRInnerTracker = 220;           // Maximum detectable rigidity [GV]
static const float gMDRInnerTrackerPlusLayer1 = 610; // Maximum detectable rigidity [GV]
static const float gMDRInnerTrackerPlusLayer9 = 840; // Maximum detectable rigidity [GV]
static const float gMDRFullSpan = 2180;              // Maximum detectable rigidity [GV]

/** %Cut on reliability of rigidity measurement of selected particle in event.
  *
  * Depending on the layer pattern used for the fit of the tracker track, a different maximally
  * detectable rigidity (MDR) is achieved. Here we cut on \f$R<f*MDR\f$
  * where \p f is the fraction (0..1) of the MDR that is allowed in the worst case.
  */
class CutTrackerTrackReliableRigidity : public Cut {
public:
  CutTrackerTrackReliableRigidity( float f = 0.3 ) : // FIXME remove default (remember to introduce private constructor with zero arguments!)
    Cut( "Reliable rigidity measurement" ),
    fMDRFraction(f)
  { }
  
  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::TrackerTrack* track = p.MainTrackerTrack();
    const AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    if( !track || !trackFit ) return true;
    float absoluteRigidity = fabs(trackFit->Rigidity());

    // Inner tracker sufficient for rigidities below 60GV.
    if (absoluteRigidity <= fMDRFraction * gMDRInnerTracker)
      return true;
    
    // Check availability of external alignment.
    if (trackFit->Pattern() == 3)
      return false;
    
    // Require at least one outer layer for rigidities > 60.
    // strictly speaking, we implicitly assume here that main track and main track fit are based on the same track object
    std::bitset<10> layers = track->LayerYPatternBitset();
    if (layers.test(1) && !layers.test(9) && absoluteRigidity <= fMDRFraction * gMDRInnerTrackerPlusLayer1)
      return true;

    if (!layers.test(1) && layers.test(9) && absoluteRigidity <= fMDRFraction * gMDRInnerTrackerPlusLayer9)
      return true;

    if (layers.test(1) && layers.test(9)  && absoluteRigidity <= fMDRFraction * gMDRFullSpan)
      return true;
    
    return false;
  }
  
  float fMDRFraction;

  ClassDef(Cuts::CutTrackerTrackReliableRigidity,1)
};

}

#endif
