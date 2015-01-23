
// $Id$

#include "TRD_EmStandardPhysics.hh"

//#include "TRD_DetectorConstruction.hh"
#include "TRD_StepMax.hh"
#include "TG4RegionsManager.h"
#include "TG4GeometryServices.h"
#include "TG4GeometryManager.h"
#include "G4LogicalVolumeStore.hh"
//#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"

#include "G4PAIModel.hh"
#include "amsvmc_MCApplication.h"
#include "G4Version.hh"
#if G4VERSION_NUMBER  > 899 
#include "G4EmProcessOptions.hh"
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#endif

//#include "TRD_PAIModel.hh"
//#include "G4PAIPhotonModel.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TRD_EmStandardPhysics::TRD_EmStandardPhysics(const G4String& name, G4int ver) :
  G4VPhysicsConstructor(name), fTrdGasRegionCuts(0), fTrdRadRegionCuts(0), fMaxStep( 1.0*mm ), verbose(ver)
{
  G4LossTableManager::Instance();

  // default cut values
//   defaultCutValue    = 0.7*mm;
  defaultCutValue    = 1.0*mm;
  fTrdGasGammaCut    = defaultCutValue;
  fTrdGasElectronCut = defaultCutValue;
  fTrdGasPositronCut = defaultCutValue;

  //  defaultCutValue    = 1.0*mm;
  fTrdRadGammaCut    = defaultCutValue;
  fTrdRadElectronCut = defaultCutValue;
  fTrdRadPositronCut = defaultCutValue;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TRD_EmStandardPhysics::~TRD_EmStandardPhysics()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void TRD_EmStandardPhysics::SetGasCut( const G4double val ){

  fTrdGasGammaCut = val;
  fTrdGasElectronCut = val;
  fTrdGasPositronCut = val;

  return;
}

void TRD_EmStandardPhysics::ConstructParticle()
{
// gamma
  G4Gamma::Gamma();

// leptons
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();

// mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

// barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

// ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TRD_EmStandardPhysics::ConstructProcess()
{
  G4cout << "Enter TRD_EmStandardPhysics::ConstructProcess" << G4endl;
  //  G4Region* trdGas = (G4RegionStore::GetInstance())->GetRegion("TrdGasRegion");
  TG4GeometryManager::Instance()->FinishGeometry();

  TG4GeometryServices *geomserv=TG4GeometryServices::Instance();
  if(!geomserv)G4cout<<"TG4GeometryServices::Instance() not found"<< G4endl;
  
  G4VPhysicalVolume* worldPV = geomserv->GetWorld();
  if(!worldPV)G4cout<<"geomserv->GetWorld() not found"<< G4endl;

  G4LogicalVolume* worldLV = worldPV->GetLogicalVolume();
  if(!worldLV)G4cout<<"worldPV->GetLogicalVolume() not found"<< G4endl;

  G4cout<<"world volume name "<<worldLV->GetName()<<G4endl;
  G4cout<<"world volume region name"<<worldLV->GetRegion()->GetName()<<G4endl;;

  TG4RegionsManager* regMan=(TG4RegionsManager*)TG4RegionsManager::Instance();
  //  regMan->DefineRegions();

  int counter=0;
  G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
  for (G4int i=0; i<G4int(lvStore->size()); i++) {
    G4LogicalVolume* lv = (*lvStore)[i];
    G4Material* material = lv->GetMaterial();
    //    if(material->GetName()!="TrdGasGmat")continue;
    G4String regionName = material->GetName();
    G4Region* region 
      = G4RegionStore::GetInstance()->GetRegion(regionName, false );
     if ( region ) {
      region->AddRootLogicalVolume(lv);
      continue;
    } 
    region = new G4Region(regionName);
    region->AddRootLogicalVolume(lv);
    ++counter;

    G4ProductionCuts* cuts = new G4ProductionCuts();
    cuts->SetProductionCut(1.0*mm, 0);
    cuts->SetProductionCut(1.0*mm, 1);
    cuts->SetProductionCut(1.0*mm, 2);
    region->SetProductionCuts(cuts);
  }
  


  G4String logicTrdGasName = "TrdGasGmat";

  regMan->DumpRegion(logicTrdGasName);
  G4Region* gasregion = G4RegionStore::GetInstance()->GetRegion(logicTrdGasName, false );
  if(!gasregion)exit(1);

  // Add standard EM Processes

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {

      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

    } else if (particleName == "e-") {

      if(verbose > 1)
        G4cout << "### EmStandard instantiates eIoni and msc80 for " 
               << particleName << G4endl;

      TRD_StepMax* theeminusStepCut = new TRD_StepMax( "TRD_StepMax", logicTrdGasName );
      theeminusStepCut->SetMaxStep(fMaxStep);

      G4eIonisation* eioniPAI= new G4eIonisation("eIoniPAI");
      G4PAIModel *pai=new G4PAIModel();
      eioniPAI->AddEmModel(0,pai,pai,gasregion);
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 1, 3);
      //      pmanager->AddDiscreteProcess(theeminusStepCut);

    } else if (particleName == "e+") {

      if(verbose > 1)
        G4cout << "### EmStandard instantiates eIoni and msc80 for " 
               << particleName << G4endl;

      TRD_StepMax* theeplusStepCut = new TRD_StepMax( "TRD_StepMax", logicTrdGasName );
      theeplusStepCut->SetMaxStep(fMaxStep);

      G4eIonisation* eioniPAI= new G4eIonisation("eIoniPAI");
      G4PAIModel *pai=new G4PAIModel();
      eioniPAI->AddEmModel(0,pai,pai,gasregion);
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 1, 3);
      pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
      //      pmanager->AddDiscreteProcess(theeplusStepCut);

    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {

      if(verbose > 1)
        G4cout << "### EmStandard instantiates muIoni and msc80 for " 
               << particleName << G4endl;

      TRD_StepMax* muonStepCut = new TRD_StepMax( "TRD_StepMax", logicTrdGasName );
      muonStepCut->SetMaxStep(fMaxStep);

      G4MuIonisation* muioni = new G4MuIonisation("muIoniPAI") ;
      G4PAIModel*     pai    = new G4PAIModel(particle,"PAIModel");
      muioni->AddEmModel(0,pai,pai,gasregion);
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4MuMultipleScattering,-1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering,-1, 1, 1);
#endif
      pmanager->AddProcess( muioni,                 -1, 2, 2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);
      //      pmanager->AddProcess( muonStepCut,-1,-1,5);

    } else if (particleName == "alpha" ||
               particleName == "He3" ||
               particleName == "GenericIon") {

      G4ionIonisation* theIonIonisation = new G4ionIonisation("ionIoniPAI");
      G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
      //G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
      theIonIonisation->AddEmModel(0,pai,pai,gasregion);

      if(verbose > 1)
        G4cout << "### EmStandard instantiates ionIoni and msc80 for " 
               << particleName << G4endl;
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(theIonIonisation,         -1, 2, 2);

    } else if (particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
               particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "omega-" ||
               particleName == "pi+" ||
               particleName == "pi-" ||
               particleName == "proton" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi-" ) {

      if(verbose > 1)
        G4cout << "### EmStandard instantiates hIoni and msc80 for " 
               << particleName << G4endl;

      TRD_StepMax* thehadronStepCut = new TRD_StepMax( "TRD_StepMax", logicTrdGasName );
      thehadronStepCut->SetMaxStep(fMaxStep) ;

      G4hIonisation* thehIonisation = new G4hIonisation("hIoniPAI");
      G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
      thehIonisation->AddEmModel(0,pai,pai,gasregion);

#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess( new G4hMultipleScattering,-1, 1, 1);
#else
      pmanager->AddProcess( new G4MultipleScattering,-1, 1, 1);
#endif
      pmanager->AddProcess( thehIonisation,          -1, 2, 2);
      //      pmanager->AddProcess( new G4hIonisation,       -1, 2, 2);
      //      pmanager->AddProcess( thehadronStepCut,         -1,-1,3);

    }

  }
  G4EmProcessOptions opt;
  opt.SetApplyCuts(true);
  opt.SetVerbose(verbose);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TRD_EmStandardPhysics::SetCuts()
{
    G4cout << "~~~~~~~~~~~~~******~~~~~~~~~~~~~~~[TRD_EmStandardPhysics::SetCuts]~~~~~******~~~~~~~~~~~~~~~~~" << G4endl;
    G4cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~[TRD_EmStandardPhysics::SetCuts]~~~~~~~~~~~~~~~~~~~~~~" << G4endl;
    G4cout << "~~~~~******~~~~~~~~~~~~~~~~~~~~~~~[TRD_EmStandardPhysics::SetCuts]~~~~~~~~~~~******~~~~~~~~~~~" << G4endl;

  if (verboseLevel > 0)
  {
    G4cout << "[TRD_EmStandardPhysics::SetCuts]" << G4endl;
    G4cout << "CutLength for e-, e+ and gamma is: " 
           << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  
  if( !fTrdGasRegionCuts ) SetTrdGasRegionCuts();
  else G4cout << "[TRD_EmStandardPhysics::SetCuts] >>> fTrdGasRegionCuts object exists!!!" << G4endl;

  //  (G4RegionStore::GetInstance())->GetRegion("TrdGasRegion")->SetProductionCuts(fTrdGasRegionCuts);
  G4cout << "TRD gas region cuts are set" << G4endl;
  
  if (verboseLevel > 0)
  {
    G4cout << "[TRD_EmStandardPhysics::SetCuts]" << G4endl;
    G4cout << "CutLength for e-, e+ and gamma is: " 
           << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }
  
  if( !fTrdRadRegionCuts ) SetTrdRadRegionCuts();
  else G4cout << "[TRD_EmStandardPhysics::SetCuts] >>> fTrdRadRegionCuts object exists!!!" << G4endl;

  //  (G4RegionStore::GetInstance())->GetRegion("TrdRadRegion")->SetProductionCuts(fTrdRadRegionCuts);
  G4cout << "TRD rad region cuts are set" << G4endl;

}

/////////////////////////////////////////////////////////////

void TRD_EmStandardPhysics::SetTrdGasRegionCuts()
{
  fTrdGasRegionCuts = new G4ProductionCuts();

  fTrdGasRegionCuts->SetProductionCut(fTrdGasGammaCut, idxG4GammaCut);
  fTrdGasRegionCuts->SetProductionCut(fTrdGasElectronCut, idxG4ElectronCut);
  fTrdGasRegionCuts->SetProductionCut(fTrdGasPositronCut, idxG4PositronCut);

  G4cout<<"TrdGasRegion gamma cut    = "<<fTrdGasGammaCut/mm<<" mm"<<G4endl;
  G4cout<<"TrdGasRegion electron cut = "<<fTrdGasElectronCut/mm<<" mm"<<G4endl;
  G4cout<<"TrdGasRegion positron cut = "<<fTrdGasPositronCut/mm<<" mm"<<G4endl;

}

void TRD_EmStandardPhysics::SetTrdRadRegionCuts()
{
  fTrdRadRegionCuts = new G4ProductionCuts();

  fTrdRadRegionCuts->SetProductionCut(fTrdRadGammaCut, idxG4GammaCut);
  fTrdRadRegionCuts->SetProductionCut(fTrdRadElectronCut, idxG4ElectronCut);
  fTrdRadRegionCuts->SetProductionCut(fTrdRadPositronCut, idxG4PositronCut);

  G4cout<<"TrdRadRegion gamma cut    = "<<fTrdRadGammaCut/mm<<" mm"<<G4endl;
  G4cout<<"TrdRadRegion electron cut = "<<fTrdRadElectronCut/mm<<" mm"<<G4endl;
  G4cout<<"TrdRadRegion positron cut = "<<fTrdRadPositronCut/mm<<" mm"<<G4endl;

}

