#include "TrackFactory.hh"

#include <SplineTrack.hh>

#include "Event.h"

#define DEBUG 0
#define INFO_OUT_TAG "TrackFactory> "
#include <debugging.hh>

ACsoft::Analysis::TrackFactory::TrackFactory()
{
}



ACsoft::Analysis::SplineTrack* ACsoft::Analysis::TrackFactory::CreateSplineTrackFrom( const AC::TrackerTrackFit& TRKfit ) {

  Double_t x[7];
  Double_t y[7];
  Double_t z[7];

  float Mxb = tan(TRKfit.ThetaTOFUpper() * TMath::DegToRad()) * cos(TRKfit.PhiTOFUpper() * TMath::DegToRad()); // x-slope at bottom
  float Myb = tan(TRKfit.ThetaTOFUpper() * TMath::DegToRad()) * sin(TRKfit.PhiTOFUpper() * TMath::DegToRad()); // y-slope at bottom

  /// \todo fix good values for dZ, dY...
  const Float_t dZ_UT = 1.0;
  const Float_t dZ_L1 = 1.0;
  const Float_t dY_UT = 0.0;
  const Float_t dY_L1 = 0.0;

  z[0] = AC::AMSGeometry::ZTOFUpper - dZ_UT;
  DEBUG_OUT << z[0] << std::endl;
  x[0] = TRKfit.XTOFUpper()    - Mxb*dZ_UT;
  y[0] = TRKfit.YTOFUpper()    - Myb*dZ_UT + Myb*dZ_UT*dY_UT/1000./TRKfit.Rigidity()*sqrt(1.+Myb*Myb); /// \todo explain purpose of this term!

  z[1] = AC::AMSGeometry::ZTOFUpper;
  x[1] = TRKfit.XTOFUpper();
  y[1] = TRKfit.YTOFUpper();

  z[2] = AC::AMSGeometry::ZTRDLower;
  x[2] = TRKfit.XTRDLower();
  y[2] = TRKfit.YTRDLower();

  z[3] = AC::AMSGeometry::ZTRDCenter;
  x[3] = TRKfit.XTRDCenter();
  y[3] = TRKfit.YTRDCenter();

  z[4] = AC::AMSGeometry::ZTRDUpper;
  x[4] = TRKfit.XTRDUpper();
  y[4] = TRKfit.YTRDUpper();

  float Mxt = tan(TRKfit.ThetaLayer1() * TMath::DegToRad()) * cos(TRKfit.PhiLayer1() * TMath::DegToRad()); // x-slope at top
  float Myt = tan(TRKfit.ThetaLayer1() * TMath::DegToRad()) * sin(TRKfit.PhiLayer1() * TMath::DegToRad()); // y-slope at top

  z[5] = AC::AMSGeometry::ZTrackerLayer1;
  x[5] = TRKfit.XLayer1();
  y[5] = TRKfit.YLayer1();

  z[6] = AC::AMSGeometry::ZTrackerLayer1 + dZ_L1;
  x[6] = TRKfit.XLayer1()   + Mxt*dZ_L1;
  y[6] = TRKfit.YLayer1()   + Myt*dZ_L1 - Myt*dZ_L1*dY_L1/1000./TRKfit.Rigidity()*sqrt(1.+Myt*Myt);


  ACsoft::Analysis::SplineTrack* track = new ACsoft::Analysis::SplineTrack;
  track->fRigidity = TRKfit.Rigidity();
  track->fRigidityUncertainty = (TRKfit.Rigidity() * TRKfit.Rigidity()) * TRKfit.InverseRigidityError();

  track->fPoints.reserve(7);
  for( int i=0 ; i<7 ; ++i )
    track->fPoints.push_back(TVector3(x[i],y[i],z[i]));

  track->fSplineZX = TSpline3("ZX",z,x,7);
  track->fSplineZY = TSpline3("ZY",z,y,7);

  /// \todo set source (from AC_TRKfit.Par ?)

  return track;
}

ACsoft::Analysis::SplineTrack* ACsoft::Analysis::TrackFactory::CreateSplineTrackFullDetectorFrom( const AC::TrackerTrackFit& TRKfit) {

  /// \todo fix good values for dZ, dY...
  const Float_t dZ_UT = 1.0;
  const Float_t dZ_L1 = 1.0;

  double z[12];
  double y[12];
  double x[12];
  
  // Extrapolate two points from layer z direction, assuming straight line
  z[0] = -158.5;
  y[0] = TRKfit.Extrapolate_to_zECAL(-158.5,AC::YZMeasurement);
  x[0] = TRKfit.Extrapolate_to_zECAL(-158.5,AC::XZMeasurement);
  
  z[1] = -142.8;
  y[1] = TRKfit.Extrapolate_to_zECAL(-142.8,AC::YZMeasurement);
  x[1] = TRKfit.Extrapolate_to_zECAL(-142.8,AC::XZMeasurement);
  
  z[2] = AC::AMSGeometry::ZTrackerLayer9;
  y[2] = TRKfit.YLayer9();
  x[2] = TRKfit.XLayer9();
  
  z[3] = AC::AMSGeometry::ZRICH;
  y[3] = TRKfit.YRICH();
  x[3] = TRKfit.XRICH();
  
  z[4] = AC::AMSGeometry::ZTrackerLayer56;
  y[4] = TRKfit.YLayer56();
  x[4] = TRKfit.XLayer56();
  
  z[5] = AC::AMSGeometry::ZTOFUpper - dZ_UT;
  y[5] = TRKfit.YTOFUpperMinus1cm();
  x[5] = TRKfit.XTOFUpperMinus1cm();

  z[6] = AC::AMSGeometry::ZTOFUpper;
  y[6] = TRKfit.YTOFUpper();
  x[6] = TRKfit.XTOFUpper();

  z[7] = AC::AMSGeometry::ZTRDLower;
  y[7] = TRKfit.YTRDLower();
  x[7] = TRKfit.XTRDLower();
  
  z[8] = AC::AMSGeometry::ZTRDCenter;
  y[8] = TRKfit.YTRDCenter();
  x[8] = TRKfit.XTRDCenter();

  z[9] = AC::AMSGeometry::ZTRDUpper;
  y[9] = TRKfit.YTRDUpper();
  x[9] = TRKfit.XTRDUpper();

  z[10] = AC::AMSGeometry::ZTrackerLayer1;
  y[10] = TRKfit.YLayer1();
  x[10] = TRKfit.XLayer1();

  z[11] = AC::AMSGeometry::ZTrackerLayer1 + dZ_L1;
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

