#ifndef TRD_GammaXTRadiator_h
#define TRD_GammaXTRadiator_h 1

#include "TRD_VXTenergyLoss.hh"

class TRD_GammaXTRadiator : public TRD_VXTenergyLoss
{
public:

  TRD_GammaXTRadiator (G4String volName,
			G4double,G4double,
			G4Material*,G4Material*,
                        G4double,G4double,G4int,
                        const G4String & processName = "XTRgammaRadiator");
  ~TRD_GammaXTRadiator ();

  // Pure virtual function from base class

  G4double GetStackFactor( G4double energy, G4double gamma, G4double varAngle);

private:

};

#endif






