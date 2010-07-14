#ifndef TRD_TRDPHYSICS_H
#define TRD_TRDPHYSICS_H

#include "G4VPhysicsConstructor.hh"

#include "G4LogicalVolume.hh"

#include "TRD_VXTenergyLoss.hh"


#include <vector>

class TRD_TrdPhysics : public G4VPhysicsConstructor {

 public:

  TRD_TrdPhysics(const G4String& name = "TRD physics");
  virtual ~TRD_TrdPhysics();

  void ConstructParticle();
  void ConstructProcess();

  void SetPhysicsTableDirectories( const G4String dir ); ///< Tell TR processes to store and look for physics tables in \c dir.

  void SetAlphaFiber( const G4double val ) { fAlphaFiber = val; }
  void SetAlphaGas( const G4double val ) { fAlphaGas = val; }

 private:

//   TRD_VXTenergyLoss* processXTR;
  
  std::vector<TRD_VXTenergyLoss*> theXTRprocesses;

  G4double fAlphaFiber; ///< fiber thickness fluctuation parameter
  G4double fAlphaGas; ///< gas thickness fluctuation parameter

};


#endif
