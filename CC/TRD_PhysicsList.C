
// $Id: TRD_PhysicsList.C,v 1.2 2014/01/31 20:44:38 choutko Exp $

#include "TRD_PhysicsList.hh"

#include "TRD_TrdPhysics.hh"
#include "TRD_PhysicsListMessenger.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip>   

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4QStoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"

#include "G4DataQuestionaire.hh"
// #include "HadronPhysicsQGSP_BIC.hh"
#include "HadronPhysicsQGSP_BERT.hh"
//#include "HadronPhysicsLHEP.hh"

#include "TRD_EmStandardPhysics.hh"
//#include "PhysListEmLivermore.hh"
// #include "PhysListEmPenelope.hh"


TRD_PhysicsList::TRD_PhysicsList():  G4VModularPhysicsList()
{
cerr<<"TRD_PhysicsList::TRD_PhysicsList-F-NotSupported "<<endl;
  abort(); 
  fEmStandardPhysics = 0;
  fTrdPhysics = 0;

//  fMessenger = new TRD_PhysicsListMessenger(this);

  G4DataQuestionaire it(photon);
//   G4cout << "<<< Geant4 Physics List simulation engine: QGSP_BIC 3.1" << G4endl;
//   G4cout << "<<< ---------------------------------------------------" << G4endl;
//   G4cout << G4endl;

  G4int ver(0);

  this->defaultCutValue = 1.0*mm;  
  this->SetVerboseLevel(ver);

  // EM Physics
  fEmStandardPhysics = new TRD_EmStandardPhysics("standard EM + PAI",ver);
  //   fEmStandardPhysics = new PhysListEmLivermore( "Livermore" );
  //   fEmStandardPhysics = new PhysListEmPenelope( "Penelope" );

  this->RegisterPhysics( fEmStandardPhysics );

 
  // Synchroton Radiation & GN Physics
  this->RegisterPhysics( new G4EmExtraPhysics("extra EM"));

  // TR
  fTrdPhysics = new TRD_TrdPhysics("trd");
  this->RegisterPhysics( fTrdPhysics );

  // Decays
  this->RegisterPhysics( new G4DecayPhysics("decay",ver) );

   // Hadron Elastic scattering
  this->RegisterPhysics( new G4HadronElasticPhysics("elastic",ver,false));

   // Hadron Physics
//   this->RegisterPhysics( new HadronPhysicsQGSP_BIC("hadron"));
  this->RegisterPhysics( new HadronPhysicsQGSP_BERT("hadron"));
//   this->RegisterPhysics( new HadronPhysicsLHEP("hadron"));

  // Stopping Physics
  this->RegisterPhysics( new G4QStoppingPhysics("stopping",ver));

  // Ion Physics
  this->RegisterPhysics( new G4IonBinaryCascadePhysics("ionBIC",ver));
 
}

TRD_PhysicsList::~TRD_PhysicsList()
{
}

void TRD_PhysicsList::SetCuts()
{

   G4cout << "<<< -----------------------TRD_PhysicsList::SetCuts()----------------------------" << G4endl;

  SetCutValue(defaultCutValue, "gamma", "DefaultRegionForTheWorld");
  SetCutValue(defaultCutValue, "e-", "DefaultRegionForTheWorld");
  SetCutValue(defaultCutValue, "e+", "DefaultRegionForTheWorld");

    fEmStandardPhysics->SetCuts();

  DumpCutValuesTable();

  return;
}

void TRD_PhysicsList::SetMaxStep( const G4double val ){

  if( fEmStandardPhysics )
    fEmStandardPhysics->SetMaxStep( val );

  return;
}
