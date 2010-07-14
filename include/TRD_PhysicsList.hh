#ifndef TRD_PhysicsList_h
#define TRD_PhysicsList_h


#include "G4VModularPhysicsList.hh"
#include "globals.hh"

#include "TRD_EmStandardPhysics.hh"

class TRD_TrdPhysics;

///  \brief Definition of physics processes.
///
///   based partially on Geant4 examples\n
///   reference: Geant4 Collaboration (S. Agostinelli et al), NIM A 506 (2003), 250-303
///
class TRD_PhysicsList: public G4VModularPhysicsList
{
 public:
  TRD_PhysicsList();
  virtual ~TRD_PhysicsList();

 public:

  virtual void SetCuts();

//   virtual void Update();

  virtual TRD_EmStandardPhysics* GetEmStandardPhysics() { return( fEmStandardPhysics ); }
  virtual TRD_TrdPhysics* GetTrdPhysics() { return( fTrdPhysics ); }

  virtual void SetMaxStep( const G4double val );

 private:

  TRD_EmStandardPhysics* fEmStandardPhysics;
//   PhysListEmLivermore* fEmStandardPhysics;
//   PhysListEmPenelope* fEmStandardPhysics;

  TRD_TrdPhysics*        fTrdPhysics;

//  TRD_PhysicsListMessenger* fMessenger;

};


#endif
