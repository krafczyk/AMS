#ifndef PATHLENGTH_FUNCTIONS_HH
#define PATHLENGTH_FUNCTIONS_HH

#include <Rtypes.h>
#include <TVector3.h>

Double_t pathlength3d( int D, TVector3 trackPos, TVector3 trackDir, TVector3 tubePos, Double_t R );
Double_t pathlength3d( int D, TVector3 trackPos, Double_t trackTheta, Double_t trackPhi, TVector3 tubePos, Double_t R );

#endif // PATHLENGTH_FUNCTIONS_HH
