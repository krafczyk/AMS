#ifndef PATHLENGTH_FUNCTIONS_HH
#define PATHLENGTH_FUNCTIONS_HH

#include <Rtypes.h>
#include <TVector3.h>

/** \file pathlength_functions.hh
  * \brief Functions useful for calculating pathlenths in cylinders, among other things.
  */

Double_t pathlength3d_obsolete( int D, TVector3 trackPos, TVector3 trackDir, TVector3 tubePos, Double_t R );
Double_t pathlength3d_obsolete( int D, TVector3 trackPos, Double_t trackTheta, Double_t trackPhi, TVector3 tubePos, Double_t R );

double Pathlength3d( double radius, const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 );
double Distance( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 );
TVector3 BasePoint1( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 );
TVector3 BasePoint2( const TVector3& x1, const TVector3& dir1, const TVector3& x2, const TVector3& dir2 );


#endif // PATHLENGTH_FUNCTIONS_HH
