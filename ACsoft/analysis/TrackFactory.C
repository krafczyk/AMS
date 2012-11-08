#include "TrackFactory.hh"

#include <SplineTrack.hh>

#include "Event.h"

#define DEBUG 0
#define INFO_OUT_TAG "TrackFactory> "
#include <debugging.hh>

Analysis::TrackFactory::TrackFactory()
{
}



Analysis::SplineTrack* Analysis::TrackFactory::CreateSplineTrackFrom( const AC::TrackerTrackFit& TRKfit ) {

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


  Analysis::SplineTrack* track = new Analysis::SplineTrack;
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
