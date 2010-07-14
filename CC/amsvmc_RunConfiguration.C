// $Id: amsvmc_RunConfiguration.C,v 1.1 2010/07/14 14:23:52 zweng Exp $
//Run configuration for AMS_VMC_Geant4, in order to create physics_list
//Based on the examples from VMC E03

#include "amsvmc_RunConfiguration.h"
//#include "amsvmc_RegionConstruction.h"

#include "TG4ComposedPhysicsList.h"
#include "TG4SpecialPhysicsList.h"

//#include <LHEP_BERT.hh>

#include <TRD_PhysicsList.hh>
//_____________________________________________________________________________
amsvmc_RunConfiguration::amsvmc_RunConfiguration(const TString& userGeometry,
					     const TString& specialProcess)
  : TG4RunConfiguration(userGeometry, "emStandard", specialProcess)
{
/// Standard constructor
/// \param userGeometry    Selection of geometry input and navigation
/// \param specialProcess  Selection of the special processes
///
/// The physics physics list selection ("emStandard") is not used,
/// \see More on the available option in class TG4RunConfiguration:
/// http://ivana.home.cern.ch/ivana/g4vmc_html/classTG4RunConfiguration.html
}

//_____________________________________________________________________________
amsvmc_RunConfiguration::~amsvmc_RunConfiguration()
{
/// Destructor
}

//
// protected methods
//


//_____________________________________________________________________________
G4VUserPhysicsList*  amsvmc_RunConfiguration::CreatePhysicsList()
{
/// Override the default physics list with user defined physics list;
/// LHEP_BERT physics list should be replaced with user own physics list

  TG4ComposedPhysicsList* builder = new TG4ComposedPhysicsList();
  // User physics list
  G4cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
  G4cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Adding user physics list:TRD_PhysicsList() " << G4endl;
  G4cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
  //  builder->AddPhysicsList(new LHEP_BERT());

  //  G4cout << "Adding SpecialPhysicsList " << G4endl;
  builder->AddPhysicsList(new TRD_PhysicsList());
  //  builder->AddPhysicsList(new TG4SpecialPhysicsList());


  //  fSpecialProcessSelection.Data()));

  return builder;
}


/*
//_____________________________________________________________________________
G4VUserDetectorConstruction*  amsvmc_RunConfiguration::CreateDetectorConstruction()
{
/// Create detector construction

  return new ExN03DetectorConstruction();
}


//_____________________________________________________________________________
G4VUserPrimaryGeneratorAction* amsvmc_RunConfiguration::CreatePrimaryGenerator()
{
/// Create primary generator

  return new ExN03PrimaryGeneratorAction();
}
*/
//TG4VUserRegionConstruction*  amsvmc_RunConfiguration::CreateUserRegionConstruction(){
//  printf("create user region construction\n");

//  return new amsvmc_RegionConstruction();
//}
