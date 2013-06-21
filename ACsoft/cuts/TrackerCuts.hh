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

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
    if( !p.HasTrackerTrack() ) return true;
    return p.HasTrackerCharge();
  }

  ClassDef(Cuts::CutTrackerChargeAvailable,1)
};


/** Cut on Tracker Charge */
class CutTrackerCharge : public TwoSidedCut {
public:
  CutTrackerCharge( float minimum, float maximum )
    : TwoSidedCut( "Tracker charge", minimum, maximum) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrack() ) return true;
    if( !p.HasTrackerCharge() ) return true;

    return ValueIsInRange(p, p.TrackerCharge());
  }

  CutTrackerCharge() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerCharge,1)
};


/** Require at least one outer tracker layer for the selected particle. */
class CutTrackerLayerOneOrNine : public Cut {
public:
  CutTrackerLayerOneOrNine() : Cut( "Tracker Y hit on layers 1 or 9" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    if( !p.HasTrackerTrack() ) return true;
    std::bitset<10> layers = p.TrackerTrack()->LayerYPatternBitset();
    return layers.test(1) || layers.test(9);
  }

  ClassDef(Cuts::CutTrackerLayerOneOrNine,1)
};

/** Require at least one hit in either layer 1, 2 or 9. */
class CutHasAdditionalNonInnerTrackerLayers : public Cut {
public:
  CutHasAdditionalNonInnerTrackerLayers() : Cut( "Hit in layer 1 or 9 or 2") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
    return p.TrackerLayerPatternClassification() != -1;
  }

  ClassDef(Cuts::CutHasAdditionalNonInnerTrackerLayers,1)
};

/** %Cut on relative sagitta error for the selected track fit. */
class CutRelativeSagittaError : public TwoSidedCut {
public:
  CutRelativeSagittaError(float minimumError = 0, float maximumError = 0)
    : TwoSidedCut( "Tracker track relative sagitta error", minimumError, maximumError) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, fabs(p.InverseRigidityError() * p.Rigidity()));
  }

  ClassDef(Cuts::CutRelativeSagittaError,1)
};

/** Make sure track associated with the selected particle is a full-span track.*/
class CutIsFullSpanTrackerTrack : public Cut {
public:
  CutIsFullSpanTrackerTrack() : Cut( "Full-span tracker track" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrack() ) return true;
    return p.TrackerLayerPatternClassification() == 0;
  }

  ClassDef(Cuts::CutIsFullSpanTrackerTrack,1)
};

/** Require hits with X and Y measurment in Layer 1 and 9 */

class CutIsFullSpanWithXandY : public Cut {
  public:
  CutIsFullSpanWithXandY() : Cut("Full-span with X and Y hit in Layer 1 and 9") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle & p) {

    AssureCutIsAppliedToACQtFile(p);
    if( !p.HasTrackerTrack() ) return true;
    std::bitset<10> layers = p.TrackerTrack()->LayerXYPatternBitset();
    return (layers.test(1) && layers.test(9));
  }
  ClassDef(Cuts::CutIsFullSpanWithXandY, 1)
};

/** Make sure track associated with the selected particle is a good track (XY hit on layers 2 and (3 or 4) and (5 or 6) and (7 or 8)).
  * \sa ACsoft::AC::TrackerTrack::IsGoodInnerTrack()
  */
class CutIsGoodInnerTrackerTrack : public Cut {
public:
  CutIsGoodInnerTrackerTrack() : Cut( "Good inner track (2&&(3||4)&&(5||6)&&(7||8))" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.TrackerTrack() ) return true;
    return p.TrackerTrack()->IsGoodInnerTrack();
  }

  ClassDef(Cuts::CutIsGoodInnerTrackerTrack,1)
};


/** Make sure track associated with the selected particle is a good central track (XY hit on (3 or 4) and (5 or 6) and (7 or 8)).
  * \sa ACsoft::AC::TrackerTrack::IsGoodCentralInnerTrack()
  */
class CutIsGoodCentralInnerTrackerTrack : public Cut {
public:
  CutIsGoodCentralInnerTrackerTrack() : Cut( "Good central inner track ((3||4)&&(5||6)&&(7||8))" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.TrackerTrack() ) return true;
    return p.TrackerTrack()->IsGoodCentralInnerTrack();
  }

  ClassDef(Cuts::CutIsGoodCentralInnerTrackerTrack,1)
};


/** %Cut on chisquared Y of track fit belonging to the selected track. */
class CutTrackerTrackYsideGoodnessOfFit : public TwoSidedCut {
public:
  CutTrackerTrackYsideGoodnessOfFit(float minimumChi2, float maximumChi2 )
    : TwoSidedCut( "Tracker track goodness of fit (chi2 y)", minimumChi2, maximumChi2) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, p.Chi2TrackerY());
  }
  CutTrackerTrackYsideGoodnessOfFit() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackYsideGoodnessOfFit,1)
};


/** %Cut on chisquared X of track fit belonging to the selected track. */
class CutTrackerTrackXsideGoodnessOfFit : public TwoSidedCut {
public:
  CutTrackerTrackXsideGoodnessOfFit(float minimumChi2, float maximumChi2 )
    : TwoSidedCut( "Tracker track goodness of fit (chi2 x)", minimumChi2, maximumChi2) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, p.Chi2TrackerX());
  }

  CutTrackerTrackXsideGoodnessOfFit() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackXsideGoodnessOfFit,1)
};


/** %Cut on relative error of sagitta (inverse rigidity). */
class CutTrackerRelativeSagittaError : public TwoSidedCut {
public:
  CutTrackerRelativeSagittaError(float minimum, float maximum )
    : TwoSidedCut( "Tracker track relative sagitta error", minimum, maximum) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, p.InverseRigidityError()*fabs(p.Rigidity()));
  }

  CutTrackerRelativeSagittaError() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerRelativeSagittaError,1)
};


/** Make sure there are at least three X-hits for track of the selected particle. */
class CutTrackerTrackHasAtLeastThreeXHits : public Cut {
public:
  CutTrackerTrackHasAtLeastThreeXHits() : Cut( "Tracker track has three X-hits"){ }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    if( !p.TrackerTrack() ) return true;
    return p.TrackerTrack()->NumberOfHitsX() >= 3;
  }

  ClassDef(Cuts::CutTrackerTrackHasAtLeastThreeXHits,1)
};


/** %Cut on rigidity (including charge sign!) of selected track fit in event. */
class CutTrackerTrackFitRigidity : public TwoSidedCut {
public:
  CutTrackerTrackFitRigidity(float minimumRigidity, float maximumRigidity )
    : TwoSidedCut( "Track rigidity", minimumRigidity, maximumRigidity) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, p.Rigidity());
  }

  CutTrackerTrackFitRigidity() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutTrackerTrackFitRigidity,1)
};


/** %Cut on absolute rigidity of selected track fit in event. */
class CutTrackerTrackFitAbsoluteRigidity : public TwoSidedCut {
public:
  CutTrackerTrackFitAbsoluteRigidity(float minimumRigidity, float maximumRigidity )
    : TwoSidedCut( "Track rigidity", minimumRigidity, maximumRigidity) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasTrackerTrackFit() ) return true;
    return ValueIsInRange(p, fabs(p.Rigidity()));
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
  CutTrackerTrackReliableRigidity( float f ) :
    Cut( "Reliable rigidity measurement" ),
    fMDRFraction(f)
  { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    const ACsoft::AC::TrackerTrack* track = p.TrackerTrack();
    const ACsoft::AC::TrackerTrackFit* trackFit = p.TrackerTrackFit();
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

  CutTrackerTrackReliableRigidity() : Cut("") { }
  ClassDef(Cuts::CutTrackerTrackReliableRigidity,1)
};

/** %Cut on tracker charge in layer 1, if existant.
  */
class CutTrackerChargeLayer1 : public TwoSidedCut {
public:
  CutTrackerChargeLayer1(float min, float max)
    : TwoSidedCut("Tracker Charge in Layer1", min, max) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p){

    float trkLay1 = p.TrackerChargeFor(ACsoft::Analysis::Particle::TrkLay1);
    if( trkLay1 < 0 ) return true;

    return ValueIsInRange(p, trkLay1);
  }

  CutTrackerChargeLayer1() : TwoSidedCut("", 0, 0) {}
  ClassDef(Cuts::CutTrackerChargeLayer1, 1)
};

/** %Cut on tracker charge in layer 9, if existant.
  */
class CutTrackerChargeLayer9 : public TwoSidedCut {
public:
  CutTrackerChargeLayer9(float min, float max)
    : TwoSidedCut("Tracker Charge in Layer9", min, max) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p){

    float trkLay9 = p.TrackerChargeFor(ACsoft::Analysis::Particle::TrkLay9);
    if( trkLay9 < 0 ) return true;

    return ValueIsInRange(p, trkLay9);
  }

  CutTrackerChargeLayer9() : TwoSidedCut("", 0, 0) {}
  ClassDef(Cuts::CutTrackerChargeLayer9, 1)
};

/** %Cut on default tracker track rigidity. Must be negative to pass this cut.
  */
class CutNegativeRigidity : public Cut {
public:
  CutNegativeRigidity() : Cut("Rigidity < 0") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p){

    if( !p.HasTrackerTrack()) return true;
    if( !p.HasTrackerTrackFit()) return true;
    return p.Rigidity() < 0;
  }

  ClassDef(Cuts::CutNegativeRigidity, 1)
};

/** %Cut on default tracker track rigidity. Must be positive to pass this cut.
  */
class CutPositiveRigidity : public Cut {
public:
  CutPositiveRigidity() : Cut("Rigidity > 0") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p){

    if( !p.HasTrackerTrack()) return true;
    if( !p.HasTrackerTrackFit()) return true;
    return p.Rigidity() > 0;
  }

  ClassDef(Cuts::CutPositiveRigidity, 1)
};

/** %Cut on the incidence angle cos(theta) extrapolated to layer1.
 */
class CutTrackerThetaLayer1 : public TwoSidedCut {
public:
  CutTrackerThetaLayer1(float min, float max)
  : TwoSidedCut("Cut certain zenith angles", min, max) {}

  virtual bool TestCondition(const ACsoft::Analysis::Particle &p){
    double TrackerTheta = p.TrackerTheta().at(1);
    if( TrackerTheta == 1000) return true;
    return ValueIsInRange(p, cos(TrackerTheta));
  }

  CutTrackerThetaLayer1() : TwoSidedCut("", 0, 0) {}
  ClassDef(Cuts::CutTrackerThetaLayer1,1)
};

/** %Cut on the existence of a layer2 Y hit
 */
class CutTrackerHasLayer2Hit : public Cut {
public:
  CutTrackerHasLayer2Hit() : Cut("Checks if Layer2 has a Y hit") {}

  virtual bool TestCondition(const ACsoft::Analysis::Particle &p){
    std::bitset<32> hits = p.TrackHits();
    return hits.test(1);
  }

  ClassDef(Cuts::CutTrackerHasLayer2Hit, 1)
};

class CutTrackInCentralTofAcceptance : public Cut {
public:
  CutTrackInCentralTofAcceptance() : Cut("TrackerTrack in central upper tof") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {

    const double Rtof = 65.;
    double TrackerTrackAtTofX = fabs(p.TrackerXcoordinates().at(0));
    double TrackerTrackAtTofY = fabs(p.TrackerYcoordinates().at(0));
    if( TrackerTrackAtTofX == 1000 || TrackerTrackAtTofY == 1000 ) return true;
    return TrackerTrackAtTofX*TrackerTrackAtTofX + TrackerTrackAtTofY*TrackerTrackAtTofY < Rtof*Rtof;
  }

  ClassDef(Cuts::CutTrackInCentralTofAcceptance, 1)
};

class CutTrackInCentralLayer1Acceptance : public Cut {
public:
  CutTrackInCentralLayer1Acceptance() : Cut("TrackerTrack in central layer 1") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {

    const double Rlayer1 = 45.;
    double TrackerTrackAtLayer1X = fabs(p.TrackerXcoordinates().at(4));
    double TrackerTrackAtLayer1Y = fabs(p.TrackerYcoordinates().at(4));
    if( TrackerTrackAtLayer1X == 1000 || TrackerTrackAtLayer1Y == 1000 ) return true;
    return TrackerTrackAtLayer1X * TrackerTrackAtLayer1X + TrackerTrackAtLayer1Y*TrackerTrackAtLayer1Y < Rlayer1 * Rlayer1;
  }

  ClassDef(Cuts::CutTrackInCentralLayer1Acceptance, 1)
};

}

#endif
