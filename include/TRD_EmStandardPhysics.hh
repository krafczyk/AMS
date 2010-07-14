#ifndef TRD_EmStandardPhysics_h
#define TRD_EmStandardPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

// for lib list detection....
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4EmProcessOptions.hh"


/// \brief Standard electromagnetic processes.
///
/// Based on G4's G4EmStandardPhysics class, but using the PAI model
/// for ionisation losses in the TRD gas.
class TRD_EmStandardPhysics : public G4VPhysicsConstructor
{
 public:
  TRD_EmStandardPhysics(const G4String& name = "EMstandard+PAI", G4int ver = 0);
  virtual ~TRD_EmStandardPhysics();

 public:
  virtual void ConstructParticle();
  virtual void ConstructProcess();

  virtual void SetCuts();
  virtual void SetMaxStep( const G4double val ) { fMaxStep = val; }

  virtual void SetGasCut( const G4double val );

 private:

  virtual void SetTrdGasRegionCuts();
  virtual void SetTrdRadRegionCuts();

 private:

  G4double defaultCutValue;
  G4ProductionCuts* fTrdGasRegionCuts;
  G4ProductionCuts* fTrdRadRegionCuts;
  G4double fTrdGasGammaCut;
  G4double fTrdGasElectronCut;
  G4double fTrdGasPositronCut;

  G4double fTrdRadGammaCut;
  G4double fTrdRadElectronCut;
  G4double fTrdRadPositronCut;

  G4double fMaxStep;

  G4int  verbose;
};



#endif






