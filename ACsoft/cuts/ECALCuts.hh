#ifndef ECALCUTS_HH
#define ECALCUTS_HH

#include "Cut.hh"
#include "TwoSidedCut.hh"

#ifndef __CINT__
#include "ECALShower.h"
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** Cut on ECAL number of showers in the event. */
class CutEcalNumberOfShowers : public TwoSidedCut {
public:
  CutEcalNumberOfShowers(float minimumShowers, float maximumShowers ) :
    TwoSidedCut( "Number of ECAL Showers", minimumShowers, maximumShowers) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) { return ValueIsInRange(p, p.NumberOfEcalShower()); }

  CutEcalNumberOfShowers() : TwoSidedCut("",0.,0.) {}
  ClassDef(Cuts::CutEcalNumberOfShowers,1)
};

/** Make sure selected tracker track passes through ECAL acceptance. */
class CutTrackerTrackInEcalAcceptance : public Cut {
public:
  CutTrackerTrackInEcalAcceptance() : Cut( "Tracker track in ECAL acceptance" ) {}

  virtual bool TestCondition(const ACsoft::Analysis::Particle& particle) {

    if (!particle.HasTrackerTrack()) return true;
    if (!particle.HasTrackerTrackFit()) return true;

    TVector3 PointOnPlane9(particle.TrackerXcoordinates()[5], particle.TrackerYcoordinates()[5], ACsoft::AC::AMSGeometry::ZTrackerLayer9);
    TVector3 DirectionOnPlane9(sin(particle.TrackerTheta()[2]) * cos(particle.TrackerPhi()[2]),
                               sin(particle.TrackerTheta()[2]) * sin(particle.TrackerPhi()[2]),
                               cos(particle.TrackerTheta()[2]));

    double lambdaEcalUpper = (ACsoft::AC::AMSGeometry::ZECALUpper - PointOnPlane9.Z()) / DirectionOnPlane9.Z();
    double lambdaEcalLower = (ACsoft::AC::AMSGeometry::ZECALLower - PointOnPlane9.Z()) / DirectionOnPlane9.Z();
    TVector3 PointAtEcalEntrance = PointOnPlane9 + lambdaEcalUpper * DirectionOnPlane9;
    TVector3 PointAtEcalExit = PointOnPlane9 + lambdaEcalLower * DirectionOnPlane9;
    return fabs(PointAtEcalEntrance.X()) < 32.4 && fabs(PointAtEcalEntrance.Y()) < 32.4 &&
           fabs(PointAtEcalExit.X()) < 31.0 && fabs(PointAtEcalExit.Y()) < 31.0;
  }

  ClassDef(Cuts::CutTrackerTrackInEcalAcceptance,2)
};

/** Make sure that selected ECAL shower is inside ECAL fiducial volume.
 *
 *  Check if the centre of gravity of the ECAL shower is well in the fiducial volume (+/- 30.0 cm) of the ECAL
 */
class CutEcalShowerInFiducialVolume : public Cut {
public:
  CutEcalShowerInFiducialVolume() : Cut( "ECAL shower in fiducial volume" ) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasEcalShower() ) return true;

    TVector3 ecalCog = p.EcalCentreOfGravity();
    bool isInEcal = ( fabs(ecalCog.X()) <= 30.0 && fabs(ecalCog.Y()) <= 30.0 );
    return isInEcal;
  }

  ClassDef(Cuts::CutEcalShowerInFiducialVolume,1)
};

/** %Cut on ECAL shower energy (GeV) of selected shower in the event. */
class CutEcalEnergy : public TwoSidedCut {
public:
  CutEcalEnergy( float minimumEnergy, float maximumEnergy )
    : TwoSidedCut( "ECAL energy", minimumEnergy, maximumEnergy) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasEcalShower() ) return true;
    return ValueIsInRange(p, p.EcalEnergy());
  }

  CutEcalEnergy() : TwoSidedCut( "", 0.0, 0.0 ) {}
  ClassDef(Cuts::CutEcalEnergy,1)
};

/** %Cut on ECAL standalone estimator (version 3) of selected shower in the event. */
class CutEcalStandaloneEstimatorV3 : public TwoSidedCut {
public:
  CutEcalStandaloneEstimatorV3( float minimumValue, float maximumValue )
    : TwoSidedCut( "ECAL ESEv3", minimumValue, maximumValue) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    if( !p.HasEcalShower() ) return true;
    const ACsoft::AC::ECALShower::EstimatorsVector& estimators = p.EcalShower()->Estimators();
    if (estimators.size() < 2) return true;
    return ValueIsInRange(p, estimators.at(2));
  }

  CutEcalStandaloneEstimatorV3() : TwoSidedCut("",0.0,0.0) { }
  ClassDef(Cuts::CutEcalStandaloneEstimatorV3,1)
};

/** %Cut on ECAL BDT estimator of selected shower in the event. */
class CutEcalBdtEstimator : public TwoSidedCut {
public:
  CutEcalBdtEstimator( float minimumBDT, float maximumBDT )
    : TwoSidedCut( "ECAL BDT", minimumBDT, maximumBDT) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    if( !p.HasEcalShower() ) return true;
    if( !p.HasEcalShowerEstimator() ) return true;
    return ValueIsInRange(p, p.EcalBDT());
  }

  CutEcalBdtEstimator() : TwoSidedCut("",0.0,0.0) { }
  ClassDef(Cuts::CutEcalBdtEstimator,1)
};

class CutEcalTrackerTrackMatchR : public TwoSidedCut {
public:
  CutEcalTrackerTrackMatchR(float minimumDistance, float maximumDistance )
    : TwoSidedCut( "Positional match ECAL <-> tracker track radially", minimumDistance, maximumDistance) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    AssureCutIsAppliedToACQtFile(p);

    if (!p.HasEcalShower()) return true;
    if (!p.HasTrackerTrack()) return true;
    if (!p.HasTrackerTrackFit()) return true;

    const ACsoft::AC::TrackerTrackFit* trackFit = p.TrackerTrackFit();
    const ACsoft::AC::ECALShower* shower = p.EcalShower();

    // Check if the direction of the ECAL shower corresponds to the Tracker track direction
    // The distance between the extrapolated X, Y position and the center of gravity of the ECAL shower is calculated and should be within a certain limit
    float distanceX = fabs(shower->X() - trackFit->Extrapolate_to_zECAL(shower->Z(), ACsoft::AC::XZMeasurement));
    float distanceY = fabs(shower->Y() - trackFit->Extrapolate_to_zECAL(shower->Z(), ACsoft::AC::YZMeasurement));

    return ValueIsInRange(p, sqrt(distanceX*distanceX + distanceY*distanceY));
  }

  CutEcalTrackerTrackMatchR() : TwoSidedCut( "", 0.0, 0.0 ) { }
  ClassDef(Cuts::CutEcalTrackerTrackMatchR,1)
};



/** %Cut on positional match between selected ECAL shower and tracker track in x-direction.
 * Distance is specified in cm.
 */
class CutEcalTrackerTrackMatchX : public TwoSidedCut {
public:
  CutEcalTrackerTrackMatchX(float minimumDistance, float maximumDistance )
    : TwoSidedCut( "x-pos. match ECAL CoG <-> tracker track", minimumDistance, maximumDistance) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& particle) {

    if (!particle.HasEcalShower()) return true;
    if (!particle.HasTrackerTrack()) return true;
    if (!particle.HasTrackerTrackFit()) return true;

    TVector3 PointOnPlane9(particle.TrackerXcoordinates()[5], particle.TrackerYcoordinates()[5], ACsoft::AC::AMSGeometry::ZTrackerLayer9);
    TVector3 DirectionOnPlane9(sin(particle.TrackerTheta()[2]) * cos(particle.TrackerPhi()[2]),
                               sin(particle.TrackerTheta()[2]) * sin(particle.TrackerPhi()[2]),
                               cos(particle.TrackerTheta()[2]));

    TVector3 EcalCentreOfGravity = particle.EcalCentreOfGravity();
    double lambdaEcalCentreOfGravity = (EcalCentreOfGravity.Z() - PointOnPlane9.Z()) / DirectionOnPlane9.Z();
    TVector3 PointAtEcalCentreOfGravity = PointOnPlane9 + lambdaEcalCentreOfGravity * DirectionOnPlane9;
    return ValueIsInRange(particle, fabs(EcalCentreOfGravity.X() - PointAtEcalCentreOfGravity.X()));
  }

  CutEcalTrackerTrackMatchX() : TwoSidedCut( "", 0.0, 0.0 ) { }
  ClassDef(Cuts::CutEcalTrackerTrackMatchX,1)
};

/** %Cut on positional match between selected ECAL shower and tracker track in y-direction.
 * Distance is specified in cm.
 */
class CutEcalTrackerTrackMatchY : public TwoSidedCut {
public:
  CutEcalTrackerTrackMatchY(float minimumDistance, float maximumDistance )
    : TwoSidedCut( "y-pos. match ECAL CoG <-> tracker track", minimumDistance, maximumDistance) { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& particle) {

    if (!particle.HasEcalShower()) return true;
    if (!particle.HasTrackerTrack()) return true;
    if (!particle.HasTrackerTrackFit()) return true;

    TVector3 PointOnPlane9(particle.TrackerXcoordinates()[5], particle.TrackerYcoordinates()[5], ACsoft::AC::AMSGeometry::ZTrackerLayer9);
    TVector3 DirectionOnPlane9(sin(particle.TrackerTheta()[2]) * cos(particle.TrackerPhi()[2]),
                               sin(particle.TrackerTheta()[2]) * sin(particle.TrackerPhi()[2]),
                               cos(particle.TrackerTheta()[2]));

    TVector3 EcalCentreOfGravity = particle.EcalCentreOfGravity();
    double lambdaEcalCentreOfGravity = (EcalCentreOfGravity.Z() - PointOnPlane9.Z()) / DirectionOnPlane9.Z();
    TVector3 PointAtEcalCentreOfGravity = PointOnPlane9 + lambdaEcalCentreOfGravity * DirectionOnPlane9;
    return ValueIsInRange(particle, fabs(EcalCentreOfGravity.Y() - PointAtEcalCentreOfGravity.Y()));
  }

  CutEcalTrackerTrackMatchY() : TwoSidedCut( "", 0.0, 0.0 ) { }
  ClassDef(Cuts::CutEcalTrackerTrackMatchY,1)
};

}

#endif
