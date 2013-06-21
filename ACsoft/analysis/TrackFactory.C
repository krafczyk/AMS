#include "TrackFactory.hh"

#include <SplineTrack.hh>

#include "Event.h"

#define DEBUG 0
#define INFO_OUT_TAG "TrackFactory> "
#include <debugging.hh>

ACsoft::Analysis::TrackFactory::TrackFactory()
{
}

ACsoft::Analysis::SplineTrack* ACsoft::Analysis::TrackFactory::CreateSplineTrackFrom( const ACsoft::AC::TrackerTrackFit& TRKfit ) {

  /// \todo fix good values for dZ, dY...
  const Float_t dZ_UT = 1.0;
  const Float_t dZ_L1 = 1.0;

  double z[12];
  double y[12];
  double x[12];
  
  // Extrapolate two points from layer z direction, assuming straight line
  z[0] = -158.7;
  y[0] = TRKfit.Extrapolate_to_zECAL(z[0],ACsoft::AC::YZMeasurement);
  x[0] = TRKfit.Extrapolate_to_zECAL(z[0],ACsoft::AC::XZMeasurement);
  
  z[1] = -142.8;
  y[1] = TRKfit.Extrapolate_to_zECAL(z[1],ACsoft::AC::YZMeasurement);
  x[1] = TRKfit.Extrapolate_to_zECAL(z[1],ACsoft::AC::XZMeasurement);
  
  z[2] = ACsoft::AC::AMSGeometry::ZTrackerLayer9;
  y[2] = TRKfit.YLayer9();
  x[2] = TRKfit.XLayer9();
  
  z[3] = ACsoft::AC::AMSGeometry::ZRICH;
  y[3] = TRKfit.YRICH();
  x[3] = TRKfit.XRICH();
  
  z[4] = ACsoft::AC::AMSGeometry::ZTrackerLayer56;
  y[4] = TRKfit.YLayer56();
  x[4] = TRKfit.XLayer56();
  
  z[5] = ACsoft::AC::AMSGeometry::ZTOFUpper - dZ_UT;
  y[5] = TRKfit.YTOFUpperMinus1cm();
  x[5] = TRKfit.XTOFUpperMinus1cm();

  z[6] = ACsoft::AC::AMSGeometry::ZTOFUpper;
  y[6] = TRKfit.YTOFUpper();
  x[6] = TRKfit.XTOFUpper();

  z[7] = ACsoft::AC::AMSGeometry::ZTRDLower;
  y[7] = TRKfit.YTRDLower();
  x[7] = TRKfit.XTRDLower();
  
  z[8] = ACsoft::AC::AMSGeometry::ZTRDCenter;
  y[8] = TRKfit.YTRDCenter();
  x[8] = TRKfit.XTRDCenter();

  z[9] = ACsoft::AC::AMSGeometry::ZTRDUpper;
  y[9] = TRKfit.YTRDUpper();
  x[9] = TRKfit.XTRDUpper();

  z[10] = ACsoft::AC::AMSGeometry::ZTrackerLayer1;
  y[10] = TRKfit.YLayer1();
  x[10] = TRKfit.XLayer1();

  z[11] = ACsoft::AC::AMSGeometry::ZTrackerLayer1 + dZ_L1;
  y[11] = TRKfit.YLayer1Plus1cm();
  x[11] = TRKfit.XLayer1Plus1cm();

  ACsoft::Analysis::SplineTrack* track = new ACsoft::Analysis::SplineTrack;
  track->fRigidity = TRKfit.Rigidity();
  track->fRigidityUncertainty = (TRKfit.Rigidity() * TRKfit.Rigidity()) * TRKfit.InverseRigidityError();

  track->fPoints.reserve(12);

  for( int i=0 ; i<12 ; ++i )
    track->fPoints.push_back(TVector3(x[i],y[i],z[i]));

  track->fSplineZX = TSpline3("ZX",z,x,12);
  track->fSplineZY = TSpline3("ZY",z,y,12);

  /// \todo set source (from AC_TRKfit.Par ?)

  return track;
}

