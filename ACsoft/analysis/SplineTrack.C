#include "SplineTrack.hh"
#include "AMSGeometry.h"
#include "pathlength_functions.hh"

#include <TMath.h>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "SplineTrack> "
#include <debugging.hh>

Analysis::SplineTrack::SplineTrack() :
  fSource(Analysis::none),
  fRigidity(0.0),
  fRigidityUncertainty(0.0)
{
}

/** Interpolate track position at given z.
  *
  *  \param z z-coordinate (cm)
  */
TVector3 Analysis::SplineTrack::InterpolateToZ( Double_t z ) const {

  assert(fPoints.size()>2);

  // check z out of range!
  Double_t zmin = fPoints.front().z();
  Double_t zmax = fPoints.back().z();
  if( zmin > zmax ){
    Double_t ztmp = zmin;
    zmin = zmax;
    zmax = ztmp;
  }
  if( z > zmax || z < zmin ){
    WARN_OUT << "z value " << z << " out of range ("<<zmin<<".."<<zmax<<") !" << std::endl;
  }

  Double_t x = fSplineZX.Eval(z);
  Double_t y = fSplineZY.Eval(z);

  assert( !TMath::IsNaN(x) );
  assert( !TMath::IsNaN(y) );

  return TVector3(x,y,z);
}


/** Interpolate track position at given z.
  *
  *  \param[in] z z-coordinate (cm)
  *  \param[out] pos interpolated position (x,y,z) (cm)
  *  \param[out] dir interpolated direction (unit vector in tangential direction to track)
  */
void Analysis::SplineTrack::CalculateLocalPositionAndDirection( Double_t z, TVector3& pos, TVector3& dir ) const {

  /// \todo good value?
  static const Double_t dz = 0.1;

  pos = InterpolateToZ(z);

  TVector3 pos1 = InterpolateToZ(z-dz);
  TVector3 pos2 = InterpolateToZ(z+dz);

  TVector3 diff = pos2 - pos1;

  dir = diff.Unit();
}

/** Calculate 3D path length for a given position.
  */
float Analysis::SplineTrack::PathLength3D(int direction, const TVector3& position) const {

  TVector3 trackPos, trackDir;
  CalculateLocalPositionAndDirection(position.z(), trackPos, trackDir);
  return pathlength3d(direction, trackPos, trackDir, position, AC::AMSGeometry::TRDTubeRadius);
}

/** Calculate smallest 2-D distance from given point.
  *
  * \param D 0: \p XY is X-coordinate, 1: \p XY is Y-coordinate.
  * \param Z Z-coordinate of point.
  * \param XY X- or Y-coordinate of point (depending on value of D).
  */
Double_t Analysis::SplineTrack::TrackResidual( int D, Double_t Z, Double_t XY ) const {

  assert( !TMath::IsNaN(Z) );
  Double_t XY2 	= ( D==0 ? InterpolateToZ(Z+0.1).X() : InterpolateToZ(Z+0.1).Y() );
  Double_t XY1 	= ( D==0 ? InterpolateToZ(Z-0.1).X() : InterpolateToZ(Z-0.1).Y() );
  Double_t XY3 	= ( D==0 ? InterpolateToZ(Z).X()     : InterpolateToZ(Z).Y() );
  assert( !TMath::IsNaN(XY1) && !TMath::IsNaN(XY2) && !TMath::IsNaN(XY3) );
  Double_t M     = (XY2-XY1)/(0.2);
  Double_t Resid = (XY - XY3)*cos(atan(M));

  return Resid;
}


/** Calculate distance of track and given point along a line perpendicular to the z-axis.
  *
  * \param D 0: \p XY is X-coordinate, 1: \p XY is Y-coordinate.
  * \param Z Z-coordinate of point.
  * \param XY X- or Y-coordinate of point (depending on value of D).
  */
Double_t Analysis::SplineTrack::DeltaXY( int D, Double_t Z, Double_t XY ) const {

  TVector3 trackPos = InterpolateToZ(Z);

  return XY - ( D==0 ? trackPos.X() : trackPos.Y() );
}


/** Print contents.
  *
  */
void Analysis::SplineTrack::Dump() const {

  INFO_OUT << "Source: " << fSource << " p=" << fRigidity << "+-" << fRigidityUncertainty << std::endl;
  std::cout << "     Points: " << std::endl;
  for( unsigned i=0 ; i<fPoints.size() ; ++i )
    std::cout << "       ["<<i<<"] " << "("<<fPoints[i].x()<<","<<fPoints[i].y()<<","<<fPoints[i].z()<<")" << std::endl;
  std::cout << "     x-z spline: \n";
  fSplineZX.Dump();
  std::cout << "     y-z spline: \n";
  fSplineZY.Dump();

}
