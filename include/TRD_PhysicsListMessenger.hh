
// $Id: TRD_PhysicsListMessenger.hh,v 1.1 2010/07/14 14:24:37 zweng Exp $

#ifndef TRD_PhysicsListMessenger_h
#define TRD_PhysicsListMessenger_h

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

class TRD_PhysicsList;

///  \brief Commands for modifying the physics list parameters.
///
///   \author Henning Gast
///
///   based partially on Geant4 examples\n
///   reference: Geant4 Collaboration (S. Agostinelli et al), NIM A 506 (2003), 250-303
///
class TRD_PhysicsListMessenger : public G4UImessenger
{
 public:
  explicit TRD_PhysicsListMessenger(TRD_PhysicsList* list );
  ~TRD_PhysicsListMessenger();
    
  void SetNewValue(G4UIcommand* command, G4String newValue);
    
 private:
  TRD_PhysicsList*           thePhysicsList;
    
  G4UIdirectory*             physicsDir;
  G4UIdirectory*             trdDir;

  G4UIcmdWithAString*        trdPhysicsTableResultDirCmd;
  G4UIcmdWithADouble*        trdPhysicsAlphaFiberCmd;
  G4UIcmdWithADouble*        trdPhysicsArgonFractionCmd;
  G4UIcmdWithADouble*        trdPhysicsAlphaGasCmd;

  G4UIcmdWithADoubleAndUnit* emPhysicsStepMaxCmd;
  G4UIcmdWithADoubleAndUnit* emPhysicsGasCutCmd;

};


#endif

