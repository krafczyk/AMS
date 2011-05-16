#ifndef _richG4Cerenkov_
#define _richG4Cerenkov_

#include "G4Cerenkov.hh"

// A implementation of a Cerenkov process, smart enough
// to simulate the RICH idiosincracies

class RichG4Cerenkov: public G4Cerenkov{
 public:
  G4bool fTrackSecondariesFirst;
  G4int  fMaxPhotons;
  G4double fMaxBetaChange;

  G4double GetAverageNumberOfPhotons(const G4double charge,
				     const G4double beta, 
				     const G4double step, // in cm
				     const G4ThreeVector& x0,
				     const G4ThreeVector& p0);


  RichG4Cerenkov(const G4String& processName = "Cerenkov",
		 G4ProcessType type = fElectromagnetic);

  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
				  const G4Step&  aStep);

  // Stupid accessor
  void SetTrackSecondariesFirst(const G4bool state){fTrackSecondariesFirst = state;G4Cerenkov::SetTrackSecondariesFirst(state);}
  G4double PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
  						G4double,
						G4ForceCondition* condition);

  void SetMaxNumPhotonsPerStep(const G4int NumPhotons){fMaxPhotons = NumPhotons; G4Cerenkov::SetMaxNumPhotonsPerStep(NumPhotons);}

  void SetMaxBetaChangePerStep(const G4double value){fMaxBetaChange = value*perCent;G4Cerenkov::SetMaxBetaChangePerStep(value);}

};



#endif
