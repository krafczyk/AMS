
#include "pathlength_functions.hh"

#include <math.h>

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

/** Calculate three-dimensional pathlength of straight line through cylinder.
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


