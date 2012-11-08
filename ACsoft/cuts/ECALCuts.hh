#ifndef ECALCUTS_HH
#define ECALCUTS_HH

#include "Cut.hh"
#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** Make sure selected tracker track passes through ECAL acceptance. */
class CutTrackerTrackInEcalAcceptance : public Cut {
public:
  CutTrackerTrackInEcalAcceptance() : Cut( "Tracker track in ECAL acceptance" ) {}
  
  virtual bool TestCondition(const Analysis::Particle &p) {

    const AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    if( !trackFit ) return true;

    // Cut used in production of likelihoods up to now
    // FIXME make sure we can extrapolate tracker track to ECAL bottom (caveat: spline track does not
    // FIXME have enough points to do that in the lower half at the moment), then cut on 32.4 cm
    return (fabs(trackFit->XLayer9())<30.0 && fabs(trackFit->YLayer9())<30.0);

//    // Check if extrapolated tracker track fit is going into the ECAL
//    // FIXME: the track should be extrapolated to the lower ECAL, but I don't know the z value
//    float X = fabs(trackFit->Extrapolate_to_zECAL(AC::AMSGeometry::ZECALUpper, AC::XZMeasurement));
//    float Y = fabs(trackFit->Extrapolate_to_zECAL(AC::AMSGeometry::ZECALUpper, AC::YZMeasurement));
    
//    return X <= 32.4 && Y <= 32.4; // FIXME why 32.4 cm ??
}
  
  ClassDef(Cuts::CutTrackerTrackInEcalAcceptance,1)
};
  
/** Make sure that selected ECAL shower is inside ECAL fiducial volume.
 *
 *  Check if the exit point of the ECAL shower is well (2 cells margin) in the fiducial volume (+/- 30.6 cm) of the ECAL
 */
class CutEcalShowerInFiducialVolume : public Cut {
public:
  CutEcalShowerInFiducialVolume() : Cut( "ECAL shower in fiducial volume" ) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::ECALShower* shower = p.MainEcalShower();
    if( !shower ) return true;
    return fabs(shower->ExitX()) <= 30.6 && fabs(shower->ExitY()) <= 30.6;
  }

  ClassDef(Cuts::CutEcalShowerInFiducialVolume,1)
};

/** %Cut on ECAL shower energy (GeV) of selected shower in the event. */
class CutEcalEnergy : public TwoSidedCut {
public:
  CutEcalEnergy( float minimumEnergy, float maximumEnergy )
    : TwoSidedCut( "ECAL energy", minimumEnergy, maximumEnergy) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainEcalShower() ) return true;
    return ValueIsInRange( p.MainEcalShower()->ReconstructedEnergy());
  }

  CutEcalEnergy() : TwoSidedCut( "", 0.0, 0.0 ) {}
  ClassDef(Cuts::CutEcalEnergy,1)
};

/** %Cut on ECAL BDT estimator of selected shower in the event. */
class CutEcalBdtEstimator : public TwoSidedCut {
public:
  CutEcalBdtEstimator( float minimumBDT = -1., float maximumBDT = 1. ) // FIXME remove defaults
    : TwoSidedCut( "ECAL BDT", minimumBDT, maximumBDT) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    if( !p.MainEcalShower() ) return true;
    if( !p.MainEcalShower()->Estimators().size() ) return true;
    return ValueIsInRange(p.MainEcalShower()->Estimators()[0]);
  }

  ClassDef(Cuts::CutEcalBdtEstimator,1)
};

/** %Cut on positional match between selected ECAL shower and tracker track.
 *
 * Distance is specified in cm.
 */
class CutEcalTrackerTrackMatch : public TwoSidedCut {
public:
  CutEcalTrackerTrackMatch(float minimumDistance = 0., float maximumDistance = 3.)  // FIXME remove defaults
    : TwoSidedCut( "Positional match ECAL <-> tracker track", minimumDistance, maximumDistance) { }

  virtual bool TestCondition(const Analysis::Particle& p) {

    const AC::TrackerTrackFit* trackFit = p.MainTrackerTrackFit();
    const AC::ECALShower* shower = p.MainEcalShower();

    // Check if the direction of the ECAL shower corresponds to the Tracker track direction
    // The distance between the extrapolated X, Y position and the center of gravity of the ECAL shower is calculated and should be within a certain limit
    float distanceX = fabs(shower->X() - trackFit->Extrapolate_to_zECAL(shower->Z(), AC::XZMeasurement));
    float distanceY = fabs(shower->Y() - trackFit->Extrapolate_to_zECAL(shower->Z(), AC::YZMeasurement));

    return ValueIsInRange( sqrt(distanceX*distanceX + distanceY*distanceY) );
  }

  ClassDef(Cuts::CutEcalTrackerTrackMatch,1)
};

}

#endif
