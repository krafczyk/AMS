
// $Id: TRD_PhysicsListMessenger.C,v 1.1 2010/07/14 14:23:01 zweng Exp $

#include "TRD_PhysicsListMessenger.hh"

#include "TRD_PhysicsList.hh"
#include "TRD_TrdPhysics.hh"

#include "G4Tokenizer.hh"

TRD_PhysicsListMessenger::TRD_PhysicsListMessenger( TRD_PhysicsList* list ) :
  thePhysicsList( list )
{ 

  physicsDir = new G4UIdirectory("/trd/physics/");
  physicsDir->SetGuidance("Commands for TRD simulation physics list.");

  trdDir = new G4UIdirectory("/trd/physics/trd/");
  trdDir->SetGuidance("Commands for transition radiation process.");

  trdPhysicsTableResultDirCmd = new G4UIcmdWithAString("/trd/physics/trd/physicsTableDirectory",this);
  trdPhysicsTableResultDirCmd->SetGuidance("Set directory for storage and retrieval of physics tables.");
  trdPhysicsTableResultDirCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  emPhysicsStepMaxCmd = new G4UIcmdWithADoubleAndUnit("/trd/physics/trd/stepMax",this);
  emPhysicsStepMaxCmd->SetGuidance("Set step maximum inside TRD gas.");
  emPhysicsStepMaxCmd->SetParameterName("s",false);
  emPhysicsStepMaxCmd->SetRange("s>=0");
  emPhysicsStepMaxCmd->SetUnitCategory( "Length" );
  emPhysicsStepMaxCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  emPhysicsGasCutCmd = new G4UIcmdWithADoubleAndUnit("/trd/physics/trd/gasCut",this);
  emPhysicsGasCutCmd->SetGuidance("Set range cut value inside TRD gas.");
  emPhysicsGasCutCmd->SetParameterName("s",false);
  emPhysicsGasCutCmd->SetRange("s>=0");
  emPhysicsGasCutCmd->SetUnitCategory( "Length" );
  emPhysicsGasCutCmd->AvailableForStates(G4State_PreInit);
  
  trdPhysicsAlphaFiberCmd = new G4UIcmdWithADouble( "/trd/physics/alphaFiber", this );
  trdPhysicsAlphaFiberCmd->SetGuidance("Set thickness variation parameter for radiator fibers.");
  trdPhysicsAlphaFiberCmd->SetParameterName("a",false);
  trdPhysicsAlphaFiberCmd->SetRange("a>=0");
  trdPhysicsAlphaFiberCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  trdPhysicsAlphaGasCmd = new G4UIcmdWithADouble( "/trd/physics/alphaGas", this );
  trdPhysicsAlphaGasCmd->SetGuidance("Set thickness variation parameter for radiator gass.");
  trdPhysicsAlphaGasCmd->SetParameterName("a",false);
  trdPhysicsAlphaGasCmd->SetRange("a>=0");
  trdPhysicsAlphaGasCmd->AvailableForStates(G4State_PreInit,G4State_Idle);


}


TRD_PhysicsListMessenger::~TRD_PhysicsListMessenger(){

  delete trdPhysicsTableResultDirCmd;
  delete emPhysicsStepMaxCmd;
  delete emPhysicsGasCutCmd;

  delete trdPhysicsAlphaFiberCmd;
  delete trdPhysicsAlphaGasCmd;
  delete trdPhysicsArgonFractionCmd;

  delete trdDir;
  delete physicsDir;

}

void TRD_PhysicsListMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == trdPhysicsTableResultDirCmd )
    { thePhysicsList->GetTrdPhysics()->SetPhysicsTableDirectories( newValue ); }

  if( command == emPhysicsStepMaxCmd )
    { thePhysicsList->SetMaxStep( emPhysicsStepMaxCmd->GetNewDoubleValue( newValue ) ); }
  
  if( command == emPhysicsGasCutCmd )
    { thePhysicsList->GetEmStandardPhysics()->SetGasCut( emPhysicsGasCutCmd->GetNewDoubleValue( newValue ) ); }

  if( command == trdPhysicsAlphaFiberCmd )
    { thePhysicsList->GetTrdPhysics()->SetAlphaFiber( trdPhysicsAlphaFiberCmd->GetNewDoubleValue( newValue ) ); }

  if( command == trdPhysicsAlphaGasCmd )
    { thePhysicsList->GetTrdPhysics()->SetAlphaGas( trdPhysicsAlphaGasCmd->GetNewDoubleValue( newValue ) ); }


  return;
}

  
