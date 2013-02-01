
#include "pathlength_functions.hh"

#include "dumpstreamers.hh"

#include <math.h>

#define DEBUG 0
#include <debugging.hh>

/** Calculate three-dimensional pathlength of straight line through cylinder.
  *
  * See maple/pathlength3D.mw
  *
  * \param D orientation, 0: tube for XZ measurement, 1: tube for YZ measurement
  * \param trackPos A point on the track
  * \param trackDir A vector parallel to the track
  * \param tubePos Tube center
  * \param R tube radius
  *
  */
Double_t pathlength3d( int D, TVector3 trackPos, TVector3 trackDir, TVector3 tubePos, Double_t R ) {

  trackDir = trackDir.Unit();

  Double_t x0 = ( D==0 ? trackPos.X() : trackPos.Y() );
  Double_t z0 = trackPos.Z();
  Double_t nx = ( D==0 ? trackDir.X() : trackDir.Y() );
  Double_t nz = trackDir.Z();
  Double_t xc = ( D==0 ? tubePos.X() : tubePos.Y() );
  Double_t zc = tubePos.Z();

  Double_t sqrtterm = -2. * x0 * nx * zc * nz + 2. * x0 * nx * z0 * nz + 2. * xc * nx * zc * nz - 2. * xc * nx * z0 * nz - nx * nx * z0 * z0 +
      nx * nx * R * R - nx * nx * zc * zc + 2. * nx * nx * z0 * zc - nz * nz * x0 * x0 + 2. * nz * nz * x0 * xc - nz * nz * xc * xc +
      nz * nz * R * R;
  if( sqrtterm < 0.0 ) return 0.0;

  sqrtterm = sqrt(sqrtterm);
  Double_t lambda1 = (-(x0 * nx) + (xc * nx) + (zc * nz) - (z0 * nz) + sqrtterm ) / (nx * nx + nz * nz);
  Double_t lambda2 = -( (x0 * nx) - (xc * nx) - (zc * nz) + (z0 * nz) + sqrtterm ) / (nx * nx + nz * nz);

  TVector3 p1 = trackPos + lambda1*trackDir;
  TVector3 p2 = trackPos + lambda2*trackDir;
  TVector3 diff = p2 - p1;

  Double_t len3d = diff.Mag();

  return len3d;
}

/** Calculate three-dimensional pathlength of straight line through a cylinder that is parallel to the x- or y-axis.
  *
  * See maple/pathlength3D.mw
  *
  * \param D orientation, 0: tube for XZ measurement, 1: tube for YZ measurement
  * \param trackPos A point on the track
  * \param trackTheta theta (angle to z-axis) of track (in radians!)
  * \param trackPhi phi (azimuth angle, measured from x-axis) of track (in radians!)
  * \param tubePos Tube center
  * \param R tube radius
  *
  */
Double_t pathlength3d( int D, TVector3 trackPos, Double_t trackTheta, Double_t trackPhi, TVector3 tubePos, Double_t R ) {

  TVector3 trackDir( sin(trackTheta) * cos(trackPhi),
                     sin(trackTheta) * sin(trackPhi),
                     cos(trackTheta) );

  return pathlength3d(D, trackPos, trackDir, tubePos, R );
}



/** Calculate three-dimensional pathlength of straight line through a cylinder.
  *
  * Both the cylinder center and the intersecting line are defined in the usual 3D parametrization:
  * \f[
  * t: \vec{t} = \vec{x} + \lambda\vec{a}
  * \f]
  *
  * where \f$ \vec{x} \f$ is a point on the line and \f$ \vec{a} \f$ is a direction vector.
  *
  * \param[in] radius radius of cylinder
  * \param[in] x1 point on cylinder center
  * \param[in] dir1 vector parallel to cylinder axis
  * \param[in] x2 point on intersecting line
  * \param[in] dir2 direction vector of intersecting line
  *
  */
double Pathlength3d( double radius, const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 ){

  TVector3 a1 = dir1.Unit();
  TVector3 a2 = dir2.Unit();
  TVector3 x12 = x2 - x1;

  double a12 = a1*a2;
  double denom = 1.0 - a12*a12;
  if(denom<=0.0) return 0.0;

  double a1x12 = a1*x12;

  double p = ( (x12*a2)-(a1*x12)*a12 ) / denom;
  double q = ( x12.Mag2() - a1x12*a1x12 - radius*radius ) / denom;

  double radicand = p*p - q;
  if( radicand <= 0.0 ) return 0.0;

  double len = 2.0*sqrt(radicand);
  return len;
}

/** Calculate distance between two skew lines.
  *
  * \param[in] x1 point on first line
  * \param[in] dir1 direction vector of first line
  * \param[in] x2 point on second line
  * \param[in] dir2 direction vector of second line
  *
  */
double Distance( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 ){

  TVector3 a1 = dir1.Unit();
  TVector3 a2 = dir2.Unit();

  TVector3 a1xa2 = a1.Cross(a2);
  TVector3 x12 = x2 - x1;

  double denom = a1xa2.Mag();
  if( denom <= 0.0 ) return 0.0; // not correct (lines are parallel)

  double d = fabs( x12*a1xa2 ) / denom;
  return d;
}

/** For two given skew lines, calculate the point of closest approach for the two lines that is located on the first line.
  *
  * \param[in] x1 point on first line
  * \param[in] dir1 direction vector of first line
  * \param[in] x2 point on second line
  * \param[in] dir2 direction vector of second line
  *
  * \returns point of closest approach on first line
  */
TVector3 BasePoint1( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 ){

  TVector3 a1 = dir1.Unit();
  TVector3 a2 = dir2.Unit();

  double a12 = a1*a2;
  double D = -1.0 + a12*a12;

  double lambda1 = 1.0/D * ( a12*(x2*a2-x1*a2) - (x2*a1-x1*a1) );

  TVector3 bp1 = x1 + (lambda1*a1);

//  // TEST
//  double lambda2 = 1.0/D * ( (x2*a2-x1*a2) - ( a12*(x2*a1 - x1*a1) ) );
//  TVector3 bp2 = x2 + (lambda2*a2);

//  double lhs1 = x1*a1 + lambda1     - x2*a1 - lambda2*a12;
//  double lhs2 = x1*a2 + lambda1*a12 - x2*a2 - lambda2;

//  DEBUG_OUT << "lhs1: " << lhs1 << " lhs2: " << lhs2 << std::endl;
//  DEBUG_OUT << "bp1: " << bp1 << " bp2: " << bp2 << std::endl;

  return bp1;
}

/** For two given skew lines, calculate the point of closest approach for the two lines that is located on the second line.
  *
  * \param[in] x1 point on first line
  * \param[in] dir1 direction vector of first line
  * \param[in] x2 point on second line
  * \param[in] dir2 direction vector of second line
  *
  * \returns point of closest approach on second line
  */
TVector3 BasePoint2( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 ){

  TVector3 a1 = dir1.Unit();
  TVector3 a2 = dir2.Unit();

  double a12 = a1*a2;
  double D = -1.0 + a12*a12;

  double lambda2 = 1.0/D * ( (x2*a2-x1*a2) - ( a12*(x2*a1 - x1*a1) ) );

  TVector3 bp2 = x2 + (lambda2*a2);
  return bp2;
}

