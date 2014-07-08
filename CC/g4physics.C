//  $Id$
//
//
// 
#include "g4physics_ion.h"
#include "G4EmExtraPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4QStoppingPhysics.hh"

#include "g4physics.h"
#include "g4xray.h"
#include "g4strangelet.h"
#include "cern.h"
#include "commons.h"
#include "amsstl.h"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4CrossSectionElastic.hh"
#if G4VERSION_NUMBER  > 945
#include "G4ComponentGGNuclNuclXsc.hh"
#endif
#include "G4IonTable.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "G4UserSpecialCuts.hh"
#include "G4FastSimulationManagerProcess.hh"
#include "G4EmStandardPhysics.hh"
#include "G4GammaXTRadiator.hh"
#include "HadronPhysicsQGSP.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSC_CHIPS.hh"
#include "G4IonPhysics.hh"
#include "G4Version.hh"
#if G4VERSION_NUMBER  > 899 
#include "G4EmProcessOptions.hh"
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#endif

vector<int> AMSG4Physics::XSId;
#include "TRD_SimUtil.h"
#include "G4HadronElasticPhysics.hh"
AMSG4Physics::AMSG4Physics():  AMSNode(AMSID("AMSG4Physics",0)),G4VUserPhysicsList(),_pg3tog4(0),_pg4tog3(0),_Ng3tog4(0)
{
  // default cut value  (1.0mm) 
  defaultCutValue = 1.0*mm;

  SetVerboseLevel(AMSFFKEY.Debug);
  
}


AMSG4Physics::~AMSG4Physics(){
  if(_pg3tog4)delete [] _pg3tog4;
  if(_pg4tog3)delete [] _pg4tog3;
}


#ifdef __AMSVMC__
void AMSG4Physics::ConstructParticle()
{
  //DUMMY FUNCTION
	
}

void AMSG4Physics::ConstructProcess()
{
  //DUMMY FUNCTION
	
}

void AMSG4Physics::SetCuts()
{
  //DUMMY FUNCTION
	
}

#else
void AMSG4Physics::ConstructParticle()
{

  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  // create all particles
  ConstructAllBosons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllBarions();
  ConstructAllIons();
  ConstructAllShortLiveds();

 G4XRay::XRayDefinition();
  //   _init();                // We now construct Tables in Begin of Run Action, to avoid conflict of "GenericIon" implementation in Geant4
}

void AMSG4Physics::ConstructProcess()
{
  AddTransportation();
  theParticleIterator->reset();
  bool first=true;
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if(G4FFKEY.Geant3CutsOn)pmanager->AddDiscreteProcess(new AMSUserSpecialCuts());
    else if(first){
      cout <<"  AMSG4Physics-I-Geant3LikeCutsSwitchedOff"<<endl;
      first=false;
    }
  }

  if(G4FFKEY.PhysicsListUsed==0){
    cout<<"Old Physics List will be used. "<<endl;
    if(GCPHYS.ILOSS){
      ConstructEM();
    }
    if(GCPHYS.IHADR)ConstructHad();
  }
  else if(G4FFKEY.PhysicsListUsed==1 || G4FFKEY.PhysicsListUsed==2){
    
    if(GCPHYS.ILOSS){
      ConstructEM2();
    }
    if(GCPHYS.IHADR){

      G4HadronElasticPhysics *hadronelastic = new G4HadronElasticPhysics("elastic");
      hadronelastic->ConstructProcess();

//    add elastice scattering to ions
#if G4VERSION_NUMBER  > 945 
  G4HadronElastic* lhep3 = 0;
  if((G4FFKEY.IonPhysicsModel/1000)%10==2)lhep3=new G4HadronElastic(G4String("ionelasticVC"));
  else lhep3=new G4HadronElastic();
  G4CrossSectionElastic* nucxs = 
    new G4CrossSectionElastic(new G4ComponentGGNuclNuclXsc());
  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String pname = particle->GetParticleName();
    if( (G4FFKEY.IonPhysicsModel/1000)%10 &&(
       pname == "GenericIon"    || 
       pname == "alpha"     ||
       pname == "deuteron"  ||
       pname == "triton"    ||
       pname == "He3"       )) {

      G4HadronElasticProcess* hel = new G4HadronElasticProcess("ionelastic");
      hel->AddDataSet(nucxs);
      hel->RegisterMe(lhep3);
      pmanager->AddDiscreteProcess(hel);
  }
}
#endif
        



    if(G4FFKEY.PhysicsListUsed==1){
      cout<<"QGSP Physics List will be used. "<<endl;
      HadronPhysicsQGSP* pqgsp=new HadronPhysicsQGSP();
      if(G4FFKEY.ProcessOff/100%10==0)pqgsp->ConstructProcess();    
      if(G4FFKEY.HCrossSectionBias!=1){
      cout<<"HadronicCrossectionWillBeBiasedBy   "<<G4FFKEY.HCrossSectionBias<<endl;
       pqgsp->thePro->theProtonInelastic->BiasCrossSectionByFactor2(G4FFKEY.HCrossSectionBias);
      }
    }
    if(G4FFKEY.PhysicsListUsed==2){
      cout<<"QGSC Physics List will be used. "<<endl;
      HadronPhysicsQGSC_CHIPS* pqgsp=new HadronPhysicsQGSC_CHIPS();  // default in geant4.9.6 
      if(G4FFKEY.ProcessOff/100%10==0)pqgsp->ConstructProcess();    
    }
   
//--Qi Yan
      G4QStoppingPhysics* hardonstop=new G4QStoppingPhysics("stopping");
      hardonstop->ConstructProcess();
      if(G4FFKEY.IonPhysicsModel%10==1||G4FFKEY.IonPhysicsModel%10==2){
        cout<<"AMSPhysicsList_HadronIon  will be used. "<<endl;
        AMSPhysicsList_HadronIon* pamshi = new AMSPhysicsList_HadronIon("TestIonAbrasian");
        if(G4FFKEY.ProcessOff/10%10==0)pamshi->ConstructProcess();
      }
//--Qi Yan
      else if (G4FFKEY.IonPhysicsModel%10==3) {
         cout<<"AMS Ion NewList will be used(DPMJET or LightIon model; DPMJET+Shen+HEAO cross section)."<<endl; 
         bool vers=1;
         IonDPMJETPhysics* pamshi = new IonDPMJETPhysics(vers); //Not only DPMJET, This is full package of all kinds of Ion-inelastic Procss and Cross-section
         if(G4FFKEY.ProcessOff/10%10==0)pamshi->ConstructProcess();
      } 
    }
  }
  
//---
  else{
    cerr<<"Physics List no "<<G4FFKEY.PhysicsListUsed<<" Not Yet Supported"<<endl;
    abort();
  }
//--Qi Yan
   if(G4FFKEY.ExEmPhysics==1){
      G4EmExtraPhysics *exph=new G4EmExtraPhysics("extra EM");
      exph->ConstructProcess();
   }
   if(G4FFKEY.NeutronTkCut==1){
      G4NeutronTrackingCut *neutroncut=new G4NeutronTrackingCut("Neutron tracking cut");
//      neutroncut->SetTimeLimit(10.*microsecond);//time should <
//      neutroncut->SetKineticEnergyLimit(10*MeV);//Energy should >
      neutroncut->ConstructProcess();
   }

  ConstructGeneral();
  //  if(TRDMCFFKEY.mode>=0)ConstructXRay();
  if(GCTLIT.ITCKOV&&(G4FFKEY.ProcessOff%10==0))ConstructOp();
}



#if G4VERSION_NUMBER  < 945 
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4LowEnergyCompton.hh"
#include "G4LowEnergyGammaConversion.hh"
#include "G4LowEnergyPhotoElectric.hh"
#include "G4LowEnergyRayleigh.hh"
#include "G4LowEnergyIonisation.hh"
#include "G4LowEnergyBremsstrahlung.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4hMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hIonisation.hh"
#else
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4hMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hIonisation.hh"
#endif

void AMSG4Physics::ConstructEM()
{
#if G4VERSION_NUMBER  < 945 
  G4Region *gasregion=trdSimUtil.gasregion;

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    //cout <<"  particle "<<particleName<<" "<<G4FFKEY.LowEMagProcUsed<<endl; 
    if (particleName == "gamma") {
      // gamma
      if(!G4FFKEY.LowEMagProcUsed){
	// Construct processes for gamma
	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
	pmanager->AddDiscreteProcess(new G4ComptonScattering());      
	pmanager->AddDiscreteProcess(new G4GammaConversion());
      }
      else{
	pmanager->AddDiscreteProcess(new G4LowEnergyPhotoElectric() );
	pmanager->AddDiscreteProcess(new G4LowEnergyCompton());
	pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
	pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion());
      }
    }
    else if (particleName == "e-") {
      //electron
      if(G4FFKEY.LowEMagProcUsed){
#if G4VERSION_NUMBER  > 899 
	G4eMultipleScattering* aMultipleScattering = new G4eMultipleScattering();
#else
	G4MultipleScattering* aMultipleScattering = new G4MultipleScattering();
#endif
	pmanager->AddProcess(aMultipleScattering,     -1, 1, 1);
	pmanager->AddProcess(new G4LowEnergyIonisation(),-1,2,2);
	pmanager->AddProcess(new G4LowEnergyBremsstrahlung(),-1,-1,3); 
      }
      else{   

	// Construct processes for electron
#if G4VERSION_NUMBER  > 899 
	G4VProcess* theeminusMultipleScattering = new G4eMultipleScattering();
#else
	G4VProcess* theeminusMultipleScattering = new G4MultipleScattering();
#endif
	G4eIonisation* theeminusIonisation = new G4eIonisation();
	G4VProcess* theeminusBremsstrahlung = new G4eBremsstrahlung();
	if(TRDMCFFKEY.PAIModel){
	  G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	  theeminusIonisation->AddEmModel(0,pai,pai,gasregion);
	}

	// add processes
	pmanager->AddProcess(theeminusMultipleScattering);
	pmanager->AddProcess(theeminusIonisation);
	pmanager->AddProcess(theeminusBremsstrahlung);      
	// set ordering for AlongStepDoIt
	pmanager->SetProcessOrdering(theeminusMultipleScattering, idxAlongStep,  1);
	pmanager->SetProcessOrdering(theeminusIonisation, idxAlongStep,  2);
	// set ordering for PostStepDoIt
	pmanager->SetProcessOrdering(theeminusMultipleScattering, idxPostStep, 1);
	pmanager->SetProcessOrdering(theeminusIonisation, idxPostStep, 2);
	pmanager->SetProcessOrdering(theeminusBremsstrahlung, idxPostStep, 3);

      }

    } else if (particleName == "e+") {
      //positron
      // Construct processes for positron


      if(G4FFKEY.LowEMagProcUsed){
#if G4VERSION_NUMBER  > 899
        G4eMultipleScattering* aMultipleScattering = new G4eMultipleScattering();
#else
        G4MultipleScattering* aMultipleScattering = new G4MultipleScattering();
#endif
	G4eIonisation* eIoni=new G4eIonisation();
	if(TRDMCFFKEY.PAIModel){
	  G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	  eIoni->AddEmModel(0,pai,pai,gasregion);
	}
        pmanager->AddProcess(aMultipleScattering,     -1, 1, 1);
        pmanager->AddProcess(eIoni              ,     -1, 2, 2);
        pmanager->AddProcess(new G4eBremsstrahlung(), -1,-1, 3);
        pmanager->AddProcess(new G4eplusAnnihilation(),0,-1, 4);


      }
      else{   

#if G4VERSION_NUMBER  > 899
	G4VProcess* theeplusMultipleScattering = new G4eMultipleScattering();
#else    
	G4VProcess* theeplusMultipleScattering = new G4MultipleScattering();
#endif
	G4eIonisation* theeplusIonisation = new G4eIonisation();
	if(TRDMCFFKEY.PAIModel){
	  G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	  theeplusIonisation->AddEmModel(0,pai,pai,gasregion);
	}
	G4VProcess* theeplusBremsstrahlung = new G4eBremsstrahlung();
	G4VProcess* theeplusAnnihilation = new G4eplusAnnihilation();
	// add processes
	pmanager->AddProcess(theeplusMultipleScattering);
	pmanager->AddProcess(theeplusIonisation);
	pmanager->AddProcess(theeplusBremsstrahlung);
	pmanager->AddProcess(theeplusAnnihilation);
	// set ordering for AtRestDoIt
	pmanager->SetProcessOrderingToFirst(theeplusAnnihilation, idxAtRest);
	// set ordering for AlongStepDoIt
	pmanager->SetProcessOrdering(theeplusMultipleScattering, idxAlongStep,  1);
	pmanager->SetProcessOrdering(theeplusIonisation, idxAlongStep,  2);
	// set ordering for PostStepDoIt
	pmanager->SetProcessOrdering(theeplusMultipleScattering, idxPostStep, 1);
	pmanager->SetProcessOrdering(theeplusIonisation, idxPostStep, 2);
	pmanager->SetProcessOrdering(theeplusBremsstrahlung, idxPostStep, 3);
	pmanager->SetProcessOrdering(theeplusAnnihilation, idxPostStep, 4);


      }

  
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
      //muon  
      // Construct processes for muon+
#if G4VERSION_NUMBER  > 899 
      G4VProcess* aMultipleScattering = new G4MuMultipleScattering();
#else
      G4VProcess* aMultipleScattering = new G4MultipleScattering();
#endif
      G4VProcess* aBremsstrahlung = new G4MuBremsstrahlung();
      G4VProcess* aPairProduction = new G4MuPairProduction();
      G4MuIonisation* anIonisation = new G4MuIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	anIonisation->AddEmModel(0,pai,pai,gasregion);
      }
      // add processes
      pmanager->AddProcess(anIonisation);
      pmanager->AddProcess(aMultipleScattering);
      pmanager->AddProcess(aBremsstrahlung);
      pmanager->AddProcess(aPairProduction);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
      pmanager->SetProcessOrdering(aBremsstrahlung, idxPostStep, 3);
      pmanager->SetProcessOrdering(aPairProduction, idxPostStep, 4);
     
    } else if( particleName == "GenericIon" ) {
      //     G4VProcess* aionIonization = new G4ionIonisation;
      G4hIonisation* aionIonization = new G4hIonisation;
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	aionIonization->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      G4VProcess* aMultipleScattering = new G4hMultipleScattering();
#else
      G4VProcess* aMultipleScattering = new G4MultipleScattering();
#endif
      pmanager->AddProcess(aionIonization);
      pmanager->AddProcess(aMultipleScattering);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(aionIonization, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(aionIonization, idxPostStep, 2);

    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      // all others charged particles except geantino
#if G4VERSION_NUMBER  > 899
      G4VProcess* aMultipleScattering = new G4hMultipleScattering();
#else
      G4VProcess* aMultipleScattering = new G4MultipleScattering();
#endif
      G4hIonisation* anIonisation = new G4hIonisation();
      // add processes
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	anIonisation->AddEmModel(0,pai,pai,gasregion);
      }
      pmanager->AddProcess(anIonisation);
      pmanager->AddProcess(aMultipleScattering);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
    }
  }
#else
       cerr<<" AMSG4Physics::ConstructEM-F-NotSupportedinGeant>4.9.4"<<endl;
       abort();

#endif

}


// Hadron Processes

#include "G4HadronElasticProcess.hh"

#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4AntiLambdaInelasticProcess.hh"
#include "G4SigmaPlusInelasticProcess.hh"
#include "G4SigmaMinusInelasticProcess.hh"
#include "G4AntiSigmaPlusInelasticProcess.hh"
#include "G4AntiSigmaMinusInelasticProcess.hh"
#include "G4XiZeroInelasticProcess.hh"
#include "G4XiMinusInelasticProcess.hh"
#include "G4AntiXiZeroInelasticProcess.hh"
#include "G4AntiXiMinusInelasticProcess.hh"
#include "G4DeuteronInelasticProcess.hh"
#include "G4TritonInelasticProcess.hh"
#include "G4AlphaInelasticProcess.hh"
#include "G4OmegaMinusInelasticProcess.hh"
#include "G4AntiOmegaMinusInelasticProcess.hh"

// Low-energy Models

#include "G4LElastic.hh"

#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEKaonPlusInelastic.hh"
#include "G4LEKaonZeroSInelastic.hh"
#include "G4LEKaonZeroLInelastic.hh"
#include "G4LEKaonMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LELambdaInelastic.hh"
#include "G4LEAntiLambdaInelastic.hh"
#include "G4LESigmaPlusInelastic.hh"
#include "G4LESigmaMinusInelastic.hh"
#include "G4LEAntiSigmaPlusInelastic.hh"
#include "G4LEAntiSigmaMinusInelastic.hh"
#include "G4LEXiZeroInelastic.hh"
#include "G4LEXiMinusInelastic.hh"
#include "G4LEAntiXiZeroInelastic.hh"
#include "G4LEAntiXiMinusInelastic.hh"
#include "G4LEDeuteronInelastic.hh"
#include "G4LETritonInelastic.hh"
#include "G4LEAlphaInelastic.hh"
#include "G4LEOmegaMinusInelastic.hh"
#include "G4LEAntiOmegaMinusInelastic.hh"
// High-energy Models

#include "G4HEPionPlusInelastic.hh"
#include "G4HEPionMinusInelastic.hh"
#include "G4HEKaonPlusInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonZeroInelastic.hh"
#include "G4HEKaonMinusInelastic.hh"
#include "G4HEProtonInelastic.hh"
#include "G4HEAntiProtonInelastic.hh"
#include "G4HENeutronInelastic.hh"
#include "G4HEAntiNeutronInelastic.hh"
#include "G4HELambdaInelastic.hh"
#include "G4HEAntiLambdaInelastic.hh"
#include "G4HESigmaPlusInelastic.hh"
#include "G4HESigmaMinusInelastic.hh"
#include "G4HEAntiSigmaPlusInelastic.hh"
#include "G4HEAntiSigmaMinusInelastic.hh"
#include "G4HEXiZeroInelastic.hh"
#include "G4HEXiMinusInelastic.hh"
#include "G4HEAntiXiZeroInelastic.hh"
#include "G4HEAntiXiMinusInelastic.hh"
#include "G4HEOmegaMinusInelastic.hh"
#include "G4HEAntiOmegaMinusInelastic.hh"

// Stopping processes

#ifdef TRIUMF_STOP_PIMINUS
#include "G4PionMinusAbsorptionAtRest.hh"
#else
#include "G4PiMinusAbsorptionAtRest.hh"
#endif
#ifdef TRIUMF_STOP_KMINUS
#include "G4KaonMinusAbsorption.hh"
#else
#include "G4KaonMinusAbsorptionAtRest.hh"
#endif

//
// ConstructHad()
//
// Makes discrete physics processes for the hadrons, at present limited
// to those particles with GHEISHA interactions (INTRC > 0).
// The processes are: Elastic scattering and Inelastic scattering.
//
// F.W.Jones  09-JUL-1998
//

void AMSG4Physics::ConstructHad()
{
#if G4VERSION_NUMBER  < 945 
  G4HadronElasticProcess* theElasticProcess = 
    new G4HadronElasticProcess;
  G4LElastic* theElasticModel = new G4LElastic;
  theElasticProcess->RegisterMe(theElasticModel);

  theParticleIterator->reset();
  while ((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "pi+") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4PionPlusInelasticProcess* theInelasticProcess = 
	new G4PionPlusInelasticProcess("inelastic");
      G4LEPionPlusInelastic* theLEInelasticModel = 
	new G4LEPionPlusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEPionPlusInelastic* theHEInelasticModel = 
	new G4HEPionPlusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "pi-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4PionMinusInelasticProcess* theInelasticProcess = 
	new G4PionMinusInelasticProcess("inelastic");
      G4LEPionMinusInelastic* theLEInelasticModel = 
	new G4LEPionMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEPionMinusInelastic* theHEInelasticModel = 
	new G4HEPionMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
#ifdef TRIUMF_STOP_PIMINUS
      pmanager->AddRestProcess(new G4PionMinusAbsorptionAtRest, ordDefault);
#else
      G4String prcNam;
      pmanager->AddRestProcess(
			       new G4PiMinusAbsorptionAtRest(
							     prcNam="PiMinusAbsorptionAtRest"), ordDefault);
#endif
    }
    else if (particleName == "kaon+") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4KaonPlusInelasticProcess* theInelasticProcess = 
	new G4KaonPlusInelasticProcess("inelastic");
      G4LEKaonPlusInelastic* theLEInelasticModel = 
	new G4LEKaonPlusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEKaonPlusInelastic* theHEInelasticModel = 
	new G4HEKaonPlusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "kaon0S") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4KaonZeroSInelasticProcess* theInelasticProcess = 
	new G4KaonZeroSInelasticProcess("inelastic");
      G4LEKaonZeroSInelastic* theLEInelasticModel = 
	new G4LEKaonZeroSInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEKaonZeroInelastic* theHEInelasticModel = 
	new G4HEKaonZeroInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "kaon0L") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4KaonZeroLInelasticProcess* theInelasticProcess = 
	new G4KaonZeroLInelasticProcess("inelastic");
      G4LEKaonZeroLInelastic* theLEInelasticModel = 
	new G4LEKaonZeroLInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEKaonZeroInelastic* theHEInelasticModel = 
	new G4HEKaonZeroInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "kaon-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4KaonMinusInelasticProcess* theInelasticProcess = 
	new G4KaonMinusInelasticProcess("inelastic");
      G4LEKaonMinusInelastic* theLEInelasticModel = 
	new G4LEKaonMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEKaonMinusInelastic* theHEInelasticModel = 
	new G4HEKaonMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
#ifdef TRIUMF_STOP_KMINUS
      pmanager->AddRestProcess(new G4KaonMinusAbsorption, ordDefault);
#else
      pmanager->AddRestProcess(new G4KaonMinusAbsorptionAtRest, ordDefault);
#endif
    }
    else if (particleName == "proton") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4ProtonInelasticProcess* theInelasticProcess = 
	new G4ProtonInelasticProcess("inelastic");
      G4LEProtonInelastic* theLEInelasticModel = new G4LEProtonInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEProtonInelastic* theHEInelasticModel = new G4HEProtonInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_proton") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiProtonInelasticProcess* theInelasticProcess = 
	new G4AntiProtonInelasticProcess("inelastic");
      G4LEAntiProtonInelastic* theLEInelasticModel = 
	new G4LEAntiProtonInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiProtonInelastic* theHEInelasticModel = 
	new G4HEAntiProtonInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "neutron") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4NeutronInelasticProcess* theInelasticProcess = 
	new G4NeutronInelasticProcess("inelastic");
      G4LENeutronInelastic* theLEInelasticModel = 
	new G4LENeutronInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HENeutronInelastic* theHEInelasticModel = 
	new G4HENeutronInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }  
    else if (particleName == "anti_neutron") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiNeutronInelasticProcess* theInelasticProcess = 
	new G4AntiNeutronInelasticProcess("inelastic");
      G4LEAntiNeutronInelastic* theLEInelasticModel = 
	new G4LEAntiNeutronInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiNeutronInelastic* theHEInelasticModel = 
	new G4HEAntiNeutronInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "lambda") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4LambdaInelasticProcess* theInelasticProcess = 
	new G4LambdaInelasticProcess("inelastic");
      G4LELambdaInelastic* theLEInelasticModel = new G4LELambdaInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HELambdaInelastic* theHEInelasticModel = new G4HELambdaInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_lambda") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiLambdaInelasticProcess* theInelasticProcess = 
	new G4AntiLambdaInelasticProcess("inelastic");
      G4LEAntiLambdaInelastic* theLEInelasticModel = 
	new G4LEAntiLambdaInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiLambdaInelastic* theHEInelasticModel = 
	new G4HEAntiLambdaInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "sigma+") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4SigmaPlusInelasticProcess* theInelasticProcess = 
	new G4SigmaPlusInelasticProcess("inelastic");
      G4LESigmaPlusInelastic* theLEInelasticModel = 
	new G4LESigmaPlusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HESigmaPlusInelastic* theHEInelasticModel = 
	new G4HESigmaPlusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "sigma-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4SigmaMinusInelasticProcess* theInelasticProcess = 
	new G4SigmaMinusInelasticProcess("inelastic");
      G4LESigmaMinusInelastic* theLEInelasticModel = 
	new G4LESigmaMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HESigmaMinusInelastic* theHEInelasticModel = 
	new G4HESigmaMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_sigma+") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiSigmaPlusInelasticProcess* theInelasticProcess = 
	new G4AntiSigmaPlusInelasticProcess("inelastic");
      G4LEAntiSigmaPlusInelastic* theLEInelasticModel = 
	new G4LEAntiSigmaPlusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiSigmaPlusInelastic* theHEInelasticModel = 
	new G4HEAntiSigmaPlusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_sigma-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiSigmaMinusInelasticProcess* theInelasticProcess = 
	new G4AntiSigmaMinusInelasticProcess("inelastic");
      G4LEAntiSigmaMinusInelastic* theLEInelasticModel = 
	new G4LEAntiSigmaMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiSigmaMinusInelastic* theHEInelasticModel = 
	new G4HEAntiSigmaMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "xi0") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4XiZeroInelasticProcess* theInelasticProcess = 
	new G4XiZeroInelasticProcess("inelastic");
      G4LEXiZeroInelastic* theLEInelasticModel = 
	new G4LEXiZeroInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEXiZeroInelastic* theHEInelasticModel = 
	new G4HEXiZeroInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "xi-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4XiMinusInelasticProcess* theInelasticProcess = 
	new G4XiMinusInelasticProcess("inelastic");
      G4LEXiMinusInelastic* theLEInelasticModel = 
	new G4LEXiMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEXiMinusInelastic* theHEInelasticModel = 
	new G4HEXiMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_xi0") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiXiZeroInelasticProcess* theInelasticProcess = 
	new G4AntiXiZeroInelasticProcess("inelastic");
      G4LEAntiXiZeroInelastic* theLEInelasticModel = 
	new G4LEAntiXiZeroInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiXiZeroInelastic* theHEInelasticModel = 
	new G4HEAntiXiZeroInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_xi-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiXiMinusInelasticProcess* theInelasticProcess = 
	new G4AntiXiMinusInelasticProcess("inelastic");
      G4LEAntiXiMinusInelastic* theLEInelasticModel = 
	new G4LEAntiXiMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiXiMinusInelastic* theHEInelasticModel = 
	new G4HEAntiXiMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "deuteron") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4DeuteronInelasticProcess* theInelasticProcess = 
	new G4DeuteronInelasticProcess("inelastic");
      G4LEDeuteronInelastic* theLEInelasticModel = 
	new G4LEDeuteronInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "triton") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4TritonInelasticProcess* theInelasticProcess = 
	new G4TritonInelasticProcess("inelastic");
      G4LETritonInelastic* theLEInelasticModel = 
	new G4LETritonInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "alpha") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AlphaInelasticProcess* theInelasticProcess = 
	new G4AlphaInelasticProcess("inelastic");
      G4LEAlphaInelastic* theLEInelasticModel = 
	new G4LEAlphaInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "omega-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4OmegaMinusInelasticProcess* theInelasticProcess = 
	new G4OmegaMinusInelasticProcess("inelastic");
      G4LEOmegaMinusInelastic* theLEInelasticModel = 
	new G4LEOmegaMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEOmegaMinusInelastic* theHEInelasticModel = 
	new G4HEOmegaMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
    else if (particleName == "anti_omega-") {
      pmanager->AddDiscreteProcess(theElasticProcess);
      G4AntiOmegaMinusInelasticProcess* theInelasticProcess = 
	new G4AntiOmegaMinusInelasticProcess("inelastic");
      G4LEAntiOmegaMinusInelastic* theLEInelasticModel = 
	new G4LEAntiOmegaMinusInelastic;
      theInelasticProcess->RegisterMe(theLEInelasticModel);
      G4HEAntiOmegaMinusInelastic* theHEInelasticModel = 
	new G4HEAntiOmegaMinusInelastic;
      theInelasticProcess->RegisterMe(theHEInelasticModel);
      pmanager->AddDiscreteProcess(theInelasticProcess);
    }
  }
#else
       cerr<<" AMSG4Physics::ConstructHad-F-NotSupportedinGeant>4.9.6"<<endl;
       abort();

#endif
}

#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "g4rich.h"
#include "g4tof.h"

void AMSG4Physics::ConstructOp()
{
//--TOF part
  TOFG4Scintillation *theScintProcessDef= new TOFG4Scintillation("Scintillation");
  theScintProcessDef->SetScintillationYieldFactor(1.0);
  theScintProcessDef->SetScintillationExcitationRatio(0.0);
//--saturation
  theScintProcessDef->SetTrackSecondariesFirst(true);
  G4EmSaturation* emSaturation =G4LossTableManager::Instance()->EmSaturation();
  theScintProcessDef->AddSaturation(emSaturation);
//---

  G4cout << " Construction Optical Processes "<<endl;
  RichG4Cerenkov*   theCerenkovProcess = new RichG4Cerenkov("Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  G4OpRayleigh*   theRayleighScatteringProcess = new G4OpRayleigh();
  RichG4OpBoundaryProcess* theBoundaryProcess = new RichG4OpBoundaryProcess();

  //  theCerenkovProcess->DumpPhysicsTable();
  //  theAbsorptionProcess->DumpPhysicsTable();
  //  theRayleighScatteringProcess->DumpPhysicsTable();
#ifdef __AMSDEBUG__
  theCerenkovProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theAbsorptionProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theRayleighScatteringProcess->SetVerboseLevel(AMSFFKEY.Debug);
  theBoundaryProcess->SetVerboseLevel(AMSFFKEY.Debug);
#endif
  G4int MaxNumPhotons = 3000;

  theCerenkovProcess->SetTrackSecondariesFirst(true);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(MaxNumPhotons);

#if G4VERSION_NUMBER  <945 
  G4OpticalSurfaceModel themodel = unified;
  theBoundaryProcess->SetModel(themodel);
#else
cerr<<"  AMSG4Physics::ConstructOP-W-SetModelRemoved????? "<<endl;
#endif
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (theCerenkovProcess->IsApplicable(*particle) && !particle->IsShortLived()) {
#if G4VERSION_NUMBER  > 899 
      pmanager->AddProcess(theCerenkovProcess);
      pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
#else
      pmanager->AddContinuousProcess(theCerenkovProcess);
#endif
//--TOF Geant4
      pmanager->AddProcess(theScintProcessDef);
      pmanager->SetProcessOrderingToLast(theScintProcessDef,idxAtRest);
      pmanager->SetProcessOrderingToLast(theScintProcessDef,idxPostStep);
    }
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << endl;
      pmanager->AddDiscreteProcess(theAbsorptionProcess);
      pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(theBoundaryProcess);
    }
  }
}

void AMSG4Physics::ConstructXRay()
{
#if G4VERSION_NUMBER  < 945 

  G4cout << " Construction TR Processes "<<endl;
  G4XRayTRDP*   pd = new G4XRayTRDP("XRayDiscrete");
  G4XRayTRCP*   pc = new G4XRayTRCP("XRayCont");

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    //    if (pd->IsApplicable(*particle))pmanager->AddDiscreteProcess(pd);
    if (pc->IsApplicable(*particle))pmanager->AddContinuousProcess(pc);
    if (particleName == "xrayphoton") {
      cout <<" Add Discrete Procs to xrayphoton "<<endl; 
      pmanager->AddDiscreteProcess(new G4LowEnergyPhotoElectric() );
      pmanager->AddDiscreteProcess(new G4LowEnergyCompton());
      //      pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
      pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion());
    } 
  }
#endif
}




#include "G4Decay.hh"
void AMSG4Physics::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

void AMSG4Physics::SetCuts()
{
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  //  (G4RegionStore::GetInstance())->Register(trdSimUtil.gasregion);
  //  G4cout << "gasregion " << trdSimUtil.gasregion << G4endl;




  SetCutsWithDefault();   
  
  G4double cut = defaultCutValue;
  cout <<"AMSG4Physics::SetCuts-I-DefaultCut "<<cut<<endl;
  
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma

if(G4FFKEY.DetectorCut%10==1){
   G4double acutv=G4FFKEY.AMSCut*mm;
//   G4cout<<"acut="<<acutv<<G4endl;
   SetCutValue(acutv, "gamma");
   SetCutValue(acutv, "xrayphoton");
   SetCutValue(acutv, "e-");
   SetCutValue(acutv, "e+");
   SetCutValue(acutv, "proton");
   SetCutValue(acutv, "anti_proton");
   SetCutValue(acutv, "alpha");
   SetCutValue(acutv, "He3");
 }
 else {
   SetCutValue(cut/10., "gamma");
   SetCutValue(cut/10., "xrayphoton");
   SetCutValue(cut/10., "e-");
   SetCutValue(cut/10., "e+");
  // set cut values for proton and anti_proton before all other hadrons
  // because some processes for hadrons need cut values for proton/anti_proton
   SetCutValue(cut, "proton");
   SetCutValue(cut, "anti_proton");
 } 

  trdSimUtil.trdregion->SetProductionCuts(trdSimUtil.fTrdRegionCuts);
  trdSimUtil.gasregion->SetProductionCuts(trdSimUtil.fTrdGasRegionCuts);
  trdSimUtil.radregion->SetProductionCuts(trdSimUtil.fTrdRadRegionCuts);
    G4Region* EcalRegion = (G4RegionStore::GetInstance())->GetRegion("ECVolumeR");
    G4ProductionCuts *EcalCuts = new G4ProductionCuts();

  if(G4FFKEY.DetectorCut/10%10==1){//Ecal
    G4double ecutv=G4FFKEY.EcalCut*mm;
//    EcalCuts->SetProductionCut(ecutv, "gamma");  // removed gamma for backspash
    EcalCuts->SetProductionCut(ecutv, "e+");
    EcalCuts->SetProductionCut(ecutv, "e-");
    EcalCuts->SetProductionCut(ecutv, "proton");
    EcalCuts->SetProductionCut(ecutv, "anti_proton");
    EcalCuts->SetProductionCut(ecutv, "alpha");
    EcalCuts->SetProductionCut(ecutv, "He3");
    EcalRegion->SetProductionCuts(EcalCuts);  
    cout <<"AMSG4Physics::SetCuts-I-SettingProductionCutsForParticles "<<ecutv<<endl;
  }
  else if(1){
    EcalCuts->SetProductionCut(cut/10./ECMCFFKEY.g4cutge, "e+");
    EcalCuts->SetProductionCut(cut/10./ECMCFFKEY.g4cutge, "e-");
    EcalCuts->SetProductionCut(cut/10./ECMCFFKEY.g4cutge, "gamma");
    EcalRegion->SetProductionCuts(EcalCuts);  
    cout <<"AMSG4Physics::SetCuts-I-SettingProductionCutsForElectronsPhotons "<<cut/10./ECMCFFKEY.g4cutge<<endl;
  }

  //  G4VPersistencyManager *persM = G4VPersistencyManager::GetPersistencyManager() ;
  //  G4VPhysicalVolume *theWorld= G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume() ;
  //  persM->Store(theWorld);
 

#ifdef __AMSDEBUG__
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    cout <<particleName<<endl;
  }
#endif  
}

void AMSG4Physics::ConstructAllBosons()
{
  // Construct all bosons
  G4BosonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllLeptons()
{
  // Construct all leptons
  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllMesons()
{
  //  Construct all mesons
  G4MesonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllBarions()
{
  //  Construct all barions
  G4BaryonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

void AMSG4Physics::ConstructAllIons()
{
  //  Construct light ions
  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();  

  
}

void AMSG4Physics::ConstructAllShortLiveds()
{
  //  Construct  resonaces and quarks
  G4ShortLivedConstructor pConstructor;
  pConstructor.ConstructParticle();  
}

#endif



const char* AMSG4Physics::G3toG4(G4int pid){

  int found=AMSbins(_pg3tog4,AMSIDi(pid),_Ng3tog4);
  if(found>0){
    return _pg3tog4[found-1].getname();
  }
  else {
    cerr<<"AMSG4Physics::G3toG4-F-NoG4ParticleFoundFor"<<pid<<endl;
    exit(1);
  }
}

G4int AMSG4Physics::G4toG3(const G4String & particle,int &error){
  error=0;
  AMSIDs ids((const char*)(particle));
  int found=AMSbins(_pg4tog3,ids,_Ng3tog4);
  if(found>0){
    return _pg4tog3[found-1].getid();
  }
  else {
    if(GetVerboseLevel()>1)cerr<<"AMSG4Physics::G4toG3-I-NoG3ParticleFoundFor"<<(const char *)(particle)<<endl;
    G4ParticleTable *ppart=G4ParticleTable::GetParticleTable();
      G4ParticleDefinition *part=ppart->FindParticle(particle);
      if(part){
         error=1;
         return part->GetPDGEncoding();
      }
      else{
       static int l=0;
       if(l++<100)cerr<<"AMSG4Physics::G4toG3-I-NoG4ParticleFoundFor"<<(const char *)(particle)<<endl;
      error=2;
      return _G3DummyParticle;
      }
  }

}

void AMSG4Physics::_init(){

 






  integer g3part=0;
  int ipart;
  for(ipart=0;ipart<1000;ipart++){
    char chp[22]="";
    integer itrtyp=0;
    geant mass=0;
    geant charge=0;
    geant tlife=0;
    geant ub[1];
    integer nwb=0;
    GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
    if(itrtyp && ipart!=4)g3part++;
  }
  cout <<"AMSG4Physics::_init-I-"<<g3part<<" Geant3 Particles Found"<<endl;
  integer *g3pid = new integer[g3part];
  geant *g3mass = new geant[g3part];
  geant *g3charge = new geant[g3part];
  G4ParticleDefinition** g3tog4p= new G4ParticleDefinition*[g3part];
  g3part=0;
  for(ipart=0;ipart<1000;ipart++){
    char chp[22]="";
    integer itrtyp=0;
    geant mass=0;
    geant charge=0;
    geant tlife=0;
    geant ub[1];
    integer nwb=0;
    GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
    if(itrtyp && ipart!=4){
      g3pid[g3part]=ipart;
      g3mass[g3part]=mass;
      g3charge[g3part++]=charge;
    }
  }







  G4ParticleTable *ppart=G4ParticleTable::GetParticleTable();
  const G4IonTable *pIonT= ppart->GetIonTable();
  for(ipart=0;ipart<g3part;ipart++){
    if(g3pid[ipart]==113)continue; 
    double fdelta=1000000;
    G4ParticleDefinition* cand=0;
    theParticleIterator->reset();
    while( (*theParticleIterator)() ){
      G4ParticleDefinition* particle = theParticleIterator->value();
      if(g3charge[ipart] == particle->GetPDGCharge() && particle->GetParticleName()!=G4String("GenericIon") ){
        if(fabs(g3mass[ipart]*GeV-particle->GetPDGMass())<fdelta){
          fdelta=fabs(g3mass[ipart]*GeV-particle->GetPDGMass());
          cand=particle;
        }
      }
    }
    if(fdelta<0.01*g3mass[ipart]*GeV){
      if(g3pid[ipart]==13){
	g3tog4p[ipart]=ppart->FindParticle("neutron");
      }  
      else {
	g3tog4p[ipart]=cand;
      }
    }
    else if (fdelta==0){
      if(g3pid[ipart]==1){
	g3tog4p[ipart]=ppart->FindParticle("gamma");
      }
      else if(g3pid[ipart]==48){
	g3tog4p[ipart]=ppart->FindParticle("geantino");
      }
      else if(g3pid[ipart]==50){
	g3tog4p[ipart]=ppart->FindParticle("opticalphoton");
      }
      else if(g3pid[ipart]==51){
	g3tog4p[ipart]=ppart->FindParticle("chargedgeantino");
      }
      else{
	g3tog4p[ipart]=ppart->FindParticle("xrayphoton");
     } 
      //       cout <<" b "<<g3pid[ipart]<<" "<<g3tog4p[ipart]->GetParticleName()<<endl;
    }
    else{
      G4int Z=abs(g3charge[ipart]);
      G4int A=g3mass[ipart]/.93;
      G4int J=0;
      G4int Q=g3charge[ipart];
      if(Q<0){
	//         cout <<"  starting anti "<<endl;
      }
      G4ParticleDefinition* dummy=((G4IonTable *)pIonT)->GetIon(Z,A,J,Q);
      double fdelta=1000000;
      G4ParticleDefinition* cand=0;
      theParticleIterator->reset();
      while( (*theParticleIterator)() ){
        G4ParticleDefinition* particle = theParticleIterator->value();
        if(g3charge[ipart] == particle->GetPDGCharge()){
          if(fabs(g3mass[ipart]*GeV-particle->GetPDGMass())<fdelta){
            fdelta=fabs(g3mass[ipart]*GeV-particle->GetPDGMass());
            cand=particle;
          }
        }
      }
      if(fdelta<0.01*g3mass[ipart]*GeV){
        g3tog4p[ipart]=cand;
	// cout <<" c "<<g3pid[ipart]<<" "<<g3tog4p[ipart]->GetParticleName()<<endl;
      }
      else{
        cerr <<"AMSG4Physics-WG4IonNotFound "<<g3pid[ipart]<<" "<<fdelta<<" "<<g3mass[ipart]*GeV<<endl;
      }
    }
  }



G4ParticleDefinition *thepart= ConstructStrangelet(CCFFKEY.StrMass,CCFFKEY.StrCharge);


  for(ipart=0;ipart<g3part;ipart++){
       if(g3pid[ipart]==113){
	g3tog4p[ipart]=ppart->FindParticle(thepart);
        if(g3tog4p[ipart]){
        cout <<"AMSG4Physics-I-G4StrangeletFound"<<g3tog4p[ipart]->GetParticleName()<<" "<<" "<<g3mass[ipart]*GeV<<endl;
        }
        break;
  }
 }  




  //  NowBuildTable
  _Ng3tog4=0;
  for(ipart=0;ipart<g3part;ipart++){
    if(g3tog4p[ipart])_Ng3tog4++;
  }
  cout <<"AMSG4Physics::_init-I-"<<_Ng3tog4<<"G4ParticlesFoundOutOf"<<g3part<<endl;
  _pg3tog4= new AMSIDi[_Ng3tog4];
  _pg4tog3= new AMSIDs[_Ng3tog4];
  _Ng3tog4=0;
  for(ipart=0;ipart<g3part;ipart++){
    if(g3tog4p[ipart]){
      _pg3tog4[_Ng3tog4].setid(g3pid[ipart]);
      _pg4tog3[_Ng3tog4].setid(g3pid[ipart]);
      g3tog4p[ipart]->SetAntiPDGEncoding(g3pid[ipart]);
      _pg3tog4[_Ng3tog4].setname(g3tog4p[ipart]->GetParticleName());
      _pg4tog3[_Ng3tog4++].setname(g3tog4p[ipart]->GetParticleName());
    }
  }

  delete[] g3pid;
  delete[] g3mass;
  delete[] g3charge;
  delete[] g3tog4p;
  AMSsortNAGa(_pg3tog4, _Ng3tog4);
  AMSsortNAGa(_pg4tog3, _Ng3tog4);
#ifdef __AMSDEBUG__
  cout << "G4toG3 Conversion Table:"<<endl;
  for(int ipt=0;ipt<_Ng3tog4;ipt++)cout<<_pg4tog3[ipt];
#endif


    


}




/* 


integer g4part=0;

theParticleIterator->reset();
while( (*theParticleIterator)() ){
G4ParticleDefinition* particle = theParticleIterator->value();
G4String particleName = particle->GetParticleName();
g4part++;
cout <<particleName <<" "<<g4part<<" "<<particle->GetPDGCharge()<<" "<<particle->GetPDGMass()<<endl;


// Construct antiions


cout << " ent " <<  pIonT->Entries()<<endl;
int idx;
int nentries=pIonT->Entries();
for (idx= 0; idx < nentries ; idx++) {
const G4ParticleDefinition *pIon=pIonT->GetParticle(idx);
G4String aname("a");
aname+=pIon->GetParticleName();
new G4VIon(aname,pIon->GetPDGMass(),pIon->GetPDGWidth(),
-pIon->GetPDGCharge(),pIon->GetPDGiSpin(),pIon->GetPDGiParity(),
pIon->GetPDGiConjugation(),pIon->GetPDGiIsospin(),pIon->GetPDGiIsospin3(),
pIon->GetPDGiGParity(),pIon->GetParticleType(),pIon->GetLeptonNumber(),
pIon->GetBaryonNumber(),pIon->GetPDGEncoding(),pIon->GetPDGStable(),
pIon->GetPDGLifeTime(),0);
}  

*/



G4int AMSG4Physics::_G3DummyParticle=556;



//     AMSUserSpecialCuts  Class
G4double AMSUserSpecialCuts::PostStepGetPhysicalInteractionLength(
								  const G4Track& aTrack,
								  G4double   previousStepSize,
								  G4ForceCondition* condition){
  // condition is set to "Not Forced"
  *condition = NotForced;

  G4double ProposedStep = DBL_MAX;
  AMSUserLimits* pUserLimits = (AMSUserLimits*)aTrack.GetVolume()->GetLogicalVolume()->GetUserLimits();
  G4LogicalVolume* logicalVolume = aTrack.GetVolume()->GetLogicalVolume();
  if (pUserLimits){ 
    G4ParticleDefinition* ParticleDef = aTrack.GetDefinition();
    G4double Ekine    = aTrack.GetKineticEnergy();
    //       const G4DynamicParticle* Particle = aTrack.GetDynamicParticle();
    //ekin & range limits
    //        G4String particleName = ParticleDef->GetParticleName();
    G4String particleType = ParticleDef->GetParticleType();
    int g3=ParticleDef->GetAntiPDGEncoding();
    //        cout <<particleName<<" "<<particleType<<" "<<ParticleDef->GetAntiPDGEncoding()<<" "<<ParticleDef->GetBaryonNumber();
    G4double Emin=0;
    G4double Charge=ParticleDef->GetPDGCharge();
    //        if(particleName=="gamma"){
    if(g3==1){       
      Emin=pUserLimits->PhotonECut();
      if (logicalVolume->GetRegion()->GetName() == "TrdRegion" ||
          logicalVolume->GetRegion()->GetName() == "TrdRadRegion" ||
          logicalVolume->GetRegion()->GetName() == "TrdGasRegion") {
        Emin = TRDMCFFKEY.photonEnergyCut * MeV;
      }
    }
    //        else if (particleName=="e-" || particleName=="e+"){
    else if (g3==2 || g3==3){
      Emin=pUserLimits->ElectronECut();
      if (logicalVolume->GetRegion()->GetName() == "TrdRegion" ||
          logicalVolume->GetRegion()->GetName() == "TrdRadRegion" ||
          logicalVolume->GetRegion()->GetName() == "TrdGasRegion") {
        Emin = TRDMCFFKEY.electronEnergyCut * MeV;
      }
    }
    //        else if (particleName=="mu-" || particleName=="mu+"){
    else if (g3==5 || g3==6){
      Emin=pUserLimits->MuonECut();
    }         
    else if ((g3>6 && g3<33) || particleType=="baryon" || particleType=="meson"){
      if(Charge != 0.){
	Emin=pUserLimits->HadronECut();
      }
      else{
	Emin=pUserLimits->HNeutralECut();
      }
    }         
    else if (particleType=="nucleus"){
      Emin=pUserLimits->HadronECut()*abs(ParticleDef->GetBaryonNumber());
    }
    else{
      Emin=pUserLimits->GetUserMinEkine(aTrack);
    }
    if(Ekine<Emin)return 0;
    /*
      else if(Charge !=0){
      G4Material* Material = aTrack.GetMaterial();
      G4double RangeNow = G4EnergyLossTables::GetRange(ParticleDef,Ekine,Material);
      G4double RangeMin = G4EnergyLossTables::GetRange(ParticleDef,Emin, Material);
      if(pUserLimits->GetUserMinRange(aTrack)> RangeMin ){
      RangeMin=pUserLimits->GetUserMinRange(aTrack);    
      if(RangeNow<RangeMin)return 0;
      }
      if(ProposedStep > RangeNow-RangeMin) ProposedStep =RangeNow-RangeMin;
      }
    */
    /*         
    //max time limit
    G4double beta = (Particle->GetTotalMomentum())/(aTrack.GetTotalEnergy());
    G4double dTime= (pUserLimits->GetUserMaxTime(aTrack) - aTrack.GetGlobalTime());
    G4double temp = beta*c_light*dTime;
    if (temp < 0.) return 0.;
    if (ProposedStep > temp) ProposedStep = temp;
    */
  }
  return ProposedStep;
}



G4VParticleChange* AMSUserSpecialCuts::PostStepDoIt(
						    const G4Track& aTrack,
						    const G4Step& step){
  //
  // Stop the current particle, if requested by G4UserLimits
  //
#ifdef __AMSDEBUGG__
  G4ParticleDefinition* ParticleDef = aTrack.GetDefinition();
  G4double Ekine    = aTrack.GetKineticEnergy();
  const G4DynamicParticle* Particle = aTrack.GetDynamicParticle();
  G4String particleName = ParticleDef->GetParticleName();
  G4String particleType = ParticleDef->GetParticleType();
  G4Material* Material = aTrack.GetMaterial();
  cout <<"stopped: "<< particleName<<" "<<ParticleDef->GetAntiPDGEncoding()<<" "<<  Ekine<<" "<<Particle->GetKineticEnergy()<<" at "<<Material->GetName()<<endl;
#endif
  aParticleChange.Initialize(aTrack);
  aParticleChange.ProposeEnergy(0.) ;
  aParticleChange.ProposeLocalEnergyDeposit (aTrack.GetKineticEnergy()) ;
  aParticleChange.ProposeTrackStatus(fStopAndKill);
  return &aParticleChange;
}

void AMSG4Physics::ConstructEM2( void ){
#if G4VERSION_NUMBER  < 945 
  int debug=TRDMCFFKEY.debug;
  int verbose=(TRDMCFFKEY.debug>1);
  if(debug)G4cout << "Enter TrdSimUtil::ConstructEM" << G4endl;
  
  std::stringstream name;
  name << "GammaXTRadiator" ;

  if(debug)G4cout<<"alpha fiber "<<trdSimUtil.GetAlphaFiber()
		 <<" alpha gas "<<trdSimUtil.GetAlphaGas()
		 <<" fleece material "<<trdSimUtil.GetG4FleeceMaterial()->GetName()
		 <<" gas material "<< trdSimUtil.GetG4TubeGasMaterial()->GetName()
		 <<" foil thickness "<< trdSimUtil.GetTrdFoilThickness()
		 <<" gas thickness "<<trdSimUtil.GetTrdGasThickness()
		 << " nfoils "<<trdSimUtil.GetTrdFoilNumber()<<G4endl;
  
  TRD_VXTenergyLoss *processXTR = new TRD_GammaXTRadiator(trdSimUtil.radregion,
							  trdSimUtil.GetAlphaFiber(),
							  trdSimUtil.GetAlphaGas(),
							  trdSimUtil.GetG4FleeceMaterial(),
							  trdSimUtil.GetG4FleeceGasMaterial(),
							 
							  trdSimUtil.GetTrdFoilThickness(),
							  trdSimUtil.GetTrdGasThickness(),
							  (G4int)trdSimUtil.GetTrdFoilNumber(),
							  "GammaXTRadiator" );
  
  if( !processXTR ){
    printf("not xtr process\n");
  }
  else{
    processXTR->SetVerboseLevel(verbose);
    processXTR->G4VProcess::SetVerboseLevel(verbose);
  }
  
  /*  G4ProcessManager * pmanager = 0;
      pmanager = G4PionPlus::PionPlus()->GetProcessManager();
      pmanager->AddDiscreteProcess(processXTR);
  
      pmanager = G4PionMinus::PionMinus()->GetProcessManager();
      pmanager->AddDiscreteProcess(processXTR);
    
      pmanager = G4Electron::Electron()->GetProcessManager();
      pmanager->AddDiscreteProcess(processXTR);
    
      pmanager = G4Positron::Positron()->GetProcessManager();
      pmanager->AddDiscreteProcess(processXTR);
    
      pmanager = G4Proton::Proton()->GetProcessManager();
      pmanager->AddDiscreteProcess(processXTR);
  */
  G4Region *gasregion=trdSimUtil.gasregion;
  gasregion->UpdateMaterialList();

  if(debug)G4cout << "gasregion root volumes: " << gasregion->GetNumberOfRootVolumes() << " materials : " << gasregion->GetNumberOfMaterials() << G4endl;
    
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(debug > 1)
      G4cout << "Construct Processes for "<< particleName << G4endl;
    
    if (particleName == "gamma") {
      
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

      
    } else if (particleName == "e-") {

      G4eIonisation* eioniPAI= new G4eIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	eioniPAI->AddEmModel(0,pai,pai,gasregion);
      }
      eioniPAI->SetVerboseLevel(debug);
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "e+") {
      G4eIonisation* eioniPAI= new G4eIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	eioniPAI->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {
      G4MuIonisation* muioni = new G4MuIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai    = new G4PAIModel(particle,"PAIModel");
	muioni->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess( muioni,                 -1, 2, 2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);
      pmanager->AddDiscreteProcess(processXTR);
      
    } else if (particleName == "alpha" ||
               particleName == "He3" ||
               particleName == "GenericIon"){

      G4ionIonisation* theIonIonisation = new G4ionIonisation();
      if(TRDMCFFKEY.PAIModel ){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	theIonIonisation->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(theIonIonisation,         -1, 2, 2);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "pi+" ||
               particleName == "pi-" ||
               particleName == "proton" ) {

      G4hIonisation* thehIonisation = new G4hIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	thehIonisation->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
#else
      pmanager->AddProcess(new G4MultipleScattering, -1, 1, 1);
#endif
      pmanager->AddProcess(thehIonisation,            -1, 2, 2);
      pmanager->AddDiscreteProcess(processXTR);
      //      pmanager->AddProcess(new G4IonBremsstrahlung,     -1,-3, 3);
      //      pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);


    } else if (particleName == "B+" ||
               particleName == "B-" ||
               particleName == "D+" ||
               particleName == "D-" ||
               particleName == "Ds+" ||
               particleName == "Ds-" ||
               particleName == "anti_lambda_c+" ||
               particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma_c+" ||
               particleName == "anti_sigma_c++" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_xi_c+" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
               particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "lambda_c+" ||
               particleName == "omega-" ||
               particleName == "sigma_c+" ||
               particleName == "sigma_c++" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi_c+" ||
               particleName == "xi-" ) {

      G4hIonisation* thehIonisation = new G4hIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	thehIonisation->AddEmModel(0,pai,pai,gasregion);
      }
#if G4VERSION_NUMBER  > 899
      pmanager->AddProcess( new G4hMultipleScattering,-1, 1, 1);
#else
      pmanager->AddProcess( new G4MultipleScattering,-1, 1, 1);
#endif
      pmanager->AddProcess( thehIonisation,          -1, 2, 2);
      pmanager->AddDiscreteProcess(processXTR);
    }
  }
  G4EmProcessOptions opt;
  opt.SetApplyCuts(true);
  opt.SetVerbose(debug);

  if(debug)G4cout << "Exit TrdSimUtil::ConstructEM" << G4endl;

#else


// new geant > 9.4.4

//G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  int debug=TRDMCFFKEY.debug;
  int verbose=(TRDMCFFKEY.debug>1);
  if(debug)G4cout << "Enter TrdSimUtil::ConstructEM" << G4endl;
  
  std::stringstream name;
  name << "GammaXTRadiator" ;

  if(debug)G4cout<<"alpha fiber "<<trdSimUtil.GetAlphaFiber()
		 <<" alpha gas "<<trdSimUtil.GetAlphaGas()
		 <<" fleece material "<<trdSimUtil.GetG4FleeceMaterial()->GetName()
		 <<" gas material "<< trdSimUtil.GetG4TubeGasMaterial()->GetName()
		 <<" foil thickness "<< trdSimUtil.GetTrdFoilThickness()
		 <<" gas thickness "<<trdSimUtil.GetTrdGasThickness()
		 << " nfoils "<<trdSimUtil.GetTrdFoilNumber()<<G4endl;

#if G4VERSION_NUMBER  < 963
  TRD_VXTenergyLoss *processXTR = new TRD_GammaXTRadiator(trdSimUtil.radregion,
  							  trdSimUtil.GetAlphaFiber(),
  							  trdSimUtil.GetAlphaGas(),
  							  trdSimUtil.GetG4FleeceMaterial(),
  							  trdSimUtil.GetG4FleeceGasMaterial(),
  							  trdSimUtil.GetTrdFoilThickness(),
  							  trdSimUtil.GetTrdGasThickness(),
  							  (G4int)trdSimUtil.GetTrdFoilNumber(),
  							  "GammaXTRadiator" );
#else
  G4VXTRenergyLoss* processXTR = new G4GammaXTRadiator(trdSimUtil.radregion,
						       trdSimUtil.GetAlphaFiber(),
						       trdSimUtil.GetAlphaGas(),
						       trdSimUtil.GetG4FleeceMaterial(),
						       trdSimUtil.GetG4FleeceGasMaterial(),
						       trdSimUtil.GetTrdFoilThickness(),
						       trdSimUtil.GetTrdGasThickness(),
						       (G4int)trdSimUtil.GetTrdFoilNumber(),
						       "GammaXTRadiator" );
#endif

  if( !processXTR ){
    printf("not xtr process\n");
  }
  else{
    processXTR->SetVerboseLevel(verbose);
    processXTR->G4VProcess::SetVerboseLevel(verbose);
  }
  
  G4Region *gasregion=trdSimUtil.gasregion;
  gasregion->UpdateMaterialList();

  if(debug)G4cout << "gasregion root volumes: " << gasregion->GetNumberOfRootVolumes() << " materials : " << gasregion->GetNumberOfMaterials() << G4endl;
    
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(debug > 1)
      G4cout << "Construct Processes for "<< particleName << G4endl;
    
    if (particleName == "gamma") {
      
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);
      pmanager->AddDiscreteProcess(new G4GammaConversion);

      
    } else if (particleName == "e-") {

      G4eIonisation* eioniPAI= new G4eIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	eioniPAI->AddEmModel(0,pai,pai,gasregion);
      }
      eioniPAI->SetVerboseLevel(debug);
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "e+") {
      G4eIonisation* eioniPAI= new G4eIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel *pai=new G4PAIModel(particle,"PAIModel");
	eioniPAI->AddEmModel(0,pai,pai,gasregion);
      }
      pmanager->AddProcess(new G4eMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(eioniPAI,                 -1, 2, 2);
      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
      pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {
      G4MuIonisation* muioni = new G4MuIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai    = new G4PAIModel(particle,"PAIModel");
	muioni->AddEmModel(0,pai,pai,gasregion);
      }
      pmanager->AddProcess(new G4MuMultipleScattering, -1, 1, 1);
      pmanager->AddProcess( muioni,                 -1, 2, 2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3, 3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4, 4);
      pmanager->AddDiscreteProcess(processXTR);
      
    } else if (particleName == "alpha" ||
               particleName == "He3" ||
               particleName == "GenericIon"){

      G4ionIonisation* theIonIonisation = new G4ionIonisation();
      if(TRDMCFFKEY.PAIModel ){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	theIonIonisation->AddEmModel(0,pai,pai,gasregion); 
      }
      pmanager->AddProcess(new G4hMultipleScattering,-1,1,1);
      pmanager->AddProcess(theIonIonisation,-1,2,2);
      pmanager->AddDiscreteProcess(processXTR);

    } else if (particleName == "pi+" ||
               particleName == "pi-" ||
               particleName == "proton" ) {

      G4hIonisation* thehIonisation = new G4hIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	thehIonisation->AddEmModel(0,pai,pai,gasregion);
      }
      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
      pmanager->AddProcess(thehIonisation,            -1, 2, 2);
      pmanager->AddDiscreteProcess(processXTR);
      //      pmanager->AddProcess(new G4IonBremsstrahlung,     -1,-3, 3);
      //      pmanager->AddProcess(new G4hPairProduction,     -1,-4, 4);


    } else if (particleName == "B+" ||
               particleName == "B-" ||
               particleName == "D+" ||
               particleName == "D-" ||
               particleName == "Ds+" ||
               particleName == "Ds-" ||
               particleName == "anti_lambda_c+" ||
               particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma_c+" ||
               particleName == "anti_sigma_c++" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_xi_c+" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
               particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "lambda_c+" ||
               particleName == "omega-" ||
               particleName == "sigma_c+" ||
               particleName == "sigma_c++" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi_c+" ||
               particleName == "xi-" ) {

      G4hIonisation* thehIonisation = new G4hIonisation();
      if(TRDMCFFKEY.PAIModel){
	G4PAIModel*     pai = new G4PAIModel(particle,"PAIModel");
	thehIonisation->AddEmModel(0,pai,pai,gasregion);
      }
      pmanager->AddProcess( new G4hMultipleScattering,-1, 1, 1);
      pmanager->AddProcess( thehIonisation,          -1, 2, 2);
      pmanager->AddDiscreteProcess(processXTR);
    }
  }
  G4EmProcessOptions opt;
  opt.SetApplyCuts(true);
  opt.SetVerbose(debug);

  if(debug)G4cout << "Exit TrdSimUtil::ConstructEM" << G4endl;





#endif
}


G4ParticleDefinition * AMSG4Physics::ConstructStrangelet(double mass, int z, int a){
// create strangelet



  
 G4double life = -1.0;
  G4DecayTable* decayTable =0;
  G4bool stable = true;
  G4double mu = 0.0;
  G4double charge =  G4double(z)*eplus;
  if(a<=0)a=mass/0.930;
  mass=a*0.930;
  int J=0;
  double E=0;  
if(a>9999){
     cerr<<"AMSG4Physics::ConstructStrangelet-E-UnableToCreateStrangeltWithA "<<a<<endl;
     a=9999;
     
}
  int encoding=z*100000+a*10+1000000000;
  string name="strangelet";
  char tmp[80];
  sprintf(tmp,"%d",encoding);
   name+="_";
   name+=tmp;
if(!G4ParticleTable::GetParticleTable()->FindParticle(name.c_str())){   
  // create an ion
  //   spin, parity, isospin values are fixed
  //
  G4ParticleDefinition*ion = new G4Ions(   name.c_str(),            mass*GeV,       0.0*MeV,     charge,
                         J,              +1,             0,
                         0,               0,             0,
                 "nucleus",               0,             a,    encoding,
                    stable,            life,    decayTable,       false,
                  "generic",              0,
                      E                       );
  ion->SetPDGMagneticMoment(mu);

  //No Anti particle registered
  ion->SetAntiPDGEncoding(0);
  AddProcessManager(ion);
  G4ProcessManager* ionMan=ion->GetProcessManager();
  G4ProcessVector* plist=ionMan->GetProcessList() ;
  for(int k=ionMan->GetProcessListLength()-1;k>=0;k--){
   G4VProcess *process= (*plist)[k];
//   cout<<process->GetProcessName()<< " "<<process->GetProcessType()<<endl;
   if(process->GetProcessType()==4 || process->GetProcessType()==4){
      cout << " AMSG4Physics-I-Removed "<<process->GetProcessName()<<"for "<<ion->GetParticleName()<<endl;
     ionMan->RemoveProcess(process);
  }
}

  G4HadronInelasticProcess* hadi = new G4HadronInelasticProcess("strangeleteInelastic", ion);
  ionMan->AddDiscreteProcess(hadi);
  hadi->AddDataSet(new StrCS());
  hadi->RegisterMe(new StrHP()); 
  
}
  return G4ParticleTable::GetParticleTable()->FindParticle(name.c_str());
}
#include "root.h"
#include "job.h"
#include <memory>
void AMSG4Physics::SaveXS(int ipart){
   XSId.clear();
   const char *name=AMSJob::gethead()->getg4physics()->G3toG4(ipart);
  G4ParticleTable *theParticleTable = G4ParticleTable::GetParticleTable();
   if(name){
    G4ParticleDefinition* theParticle =
        theParticleTable->FindParticle(G4String(name));
      if(theParticle){
        G4ThreeVector lv(0,0,0);
        G4DynamicParticle p(theParticle,lv);
        const G4int nElements = 13;
        auto_ptr<G4Element> elements[nElements] = { auto_ptr<G4Element>(new G4Element("Hydrogen" ,"H"  ,   1,  1.00794  *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Helium4"  ,"He4",   1)),//http://en.wikipedia.org/wiki/Helium-4
                                                  auto_ptr<G4Element>(new G4Element("Helium3"   ,"He3"  ,  1)),
                                                  auto_ptr<G4Element>(new G4Element("Carbon"   ,"C"  ,   6, 12.0107   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Oxygen"   ,"O"  ,   8, 15.9994   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Nitrogen"   ,"N"  ,   7, 14   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Fluoride"   ,"F"  ,   9, 19   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Natrium"   ,"Na"  ,  11, 23   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Aluminium","Al" ,  13, 26.9815386*g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Silicon"  ,"Si" ,  14, 28.0855   *g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Gold"  ,"Au",79,197*g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Copper"  ,"Cu",29,63.5*g/mole)),
                                                  auto_ptr<G4Element>(new G4Element("Lead208"  ,"Pb208",82,207.9766521*g/mole)) };

           elements[1].get()->AddIsotope(new G4Isotope("Helium4"   ,   2, 4,   4*g/mole),1) ;
           elements[2].get()->AddIsotope(new G4Isotope("Helium3"   ,   2, 3,   3*g/mole),1) ;
 
       for(G4int i=0; i<nElements; i++){
           const Double_t minR = log10(0.1), maxR = log10(1e5); // GV
           const Int_t nbins = 10000;
           double st=(maxR-minR)/nbins;
           double arr[nbins+1];
           for(int k=0;k<nbins+1;k++)arr[k]=pow(10.,minR+k*st);
           G4Element * element = elements[i].get();
           string hname=name;
           hname+=" HadronInElastic ";
           hname+=element->GetName();
           const int id=element->GetN();
           AMSEventR::hbook1(id,hname.c_str(),nbins,arr);
           hname=name;
           hname+= " EMD ";
           hname+=element->GetName();
           AMSEventR::hbook1(-id,hname.c_str(),nbins,arr);
           XSId.push_back(id);
           hname=name;
           hname+= " HadronElastic ";
           hname+=element->GetName();
           AMSEventR::hbook1(100000+id,hname.c_str(),nbins,arr);
           XSId.push_back(100000+id);
           hname=name;
           hname+= " HadronQuElastic ";
           hname+=element->GetName();
           AMSEventR::hbook1(-100000-id,hname.c_str(),nbins,arr);
              G4ProcessManager * processManager = theParticle->GetProcessManager();
              G4ProcessVector * processVector = processManager->GetProcessList();
              for(G4int j=0; j<processVector->size(); j++){
                bool inelok=false;
                 if(theParticle==G4Proton::Definition())inelok= (*processVector)[j]->GetProcessName()=="ProtonInelastic" ;
                 else if(theParticle== G4Alpha::Definition())inelok= (*processVector)[j]->GetProcessName()=="alphaInelastic"  ;
                 else if(theParticle==   G4He3::Definition())inelok= (*processVector)[j]->GetProcessName()=="He3Inelastic"    ;
                 else if(theParticle==   G4Triton::Definition())inelok=  (*processVector)[j]->GetProcessName()=="tInelastic"    ;
                 else if(theParticle==   G4Deuteron::Definition())inelok= (*processVector)[j]->GetProcessName()=="dInelastic"    ;
                 else if ( strstr((const char*)theParticle->GetParticleName(),"strangelet"))inelok=  (*processVector)[j]->GetProcessName()=="strangeletInelastic"    ;
                 else if( strstr((const char*)theParticle->GetParticleType(),"nucleus"))inelok=  ((*processVector)[j]->GetProcessName()=="ionInelastic" || (*processVector)[j]->GetProcessName()=="IonInelastic");               
                 G4HadronInelasticProcess * hadronInelasticProcess = dynamic_cast<G4HadronInelasticProcess*>((*processVector)[j]);
                 if(inelok && hadronInelasticProcess){
                  for(int k=1;k<nbins+1;k++){
                    double xs=0;
                    const G4double R = AMSEventR::h1(id)->GetBinCenter(k)*GeV;
                    lv.setZ(R*theParticle->GetAtomicNumber());
                    p.SetMomentum(lv);
#if G4VERSION_NUMBER  > 945 
                    G4Material mat("mymat",1,1);
                    mat.AddElement(element,1);
                    xs = hadronInelasticProcess->GetElementCrossSection(&p, element,&mat) / millibarn; // v9.6.p02
#else
                    xs = hadronInelasticProcess->GetMicroscopicCrossSection(&p, element, 295*kelvin) / millibarn; // v9.4.p04
#endif
//                    if(k%100==1)cout << element->GetName()<<" "<<R<<" "<<xs<<endl;
                                     AMSEventR::h1(id)->SetBinContent(k,xs);
                                     AMSEventR::h1(id)->SetBinError(k,0);
                    }
                                     break;
                                }// end of if
                 }// end of for

              for(G4int j=0; j<processVector->size(); j++){
                bool inelok=false;
                 if(theParticle== G4Alpha::Definition())inelok= (*processVector)[j]->GetProcessName()=="AlphaEMD"  ;
                 else if(theParticle==   G4He3::Definition())inelok= (*processVector)[j]->GetProcessName()=="He3EMD"    ;
                 else if( strstr((const char*)theParticle->GetParticleType(),"nucleus"))inelok=  ((*processVector)[j]->GetProcessName()=="IonEMD");
                 G4HadronInelasticProcess * hadronInelasticProcess = dynamic_cast<G4HadronInelasticProcess*>((*processVector)[j]);
                 if(inelok && hadronInelasticProcess){
                  for(int k=0;k<nbins+1;k++){
                    double xs=0;
                    const G4double R = AMSEventR::h1(-id)->GetBinCenter(k)*GeV;
                    lv.setZ(R*theParticle->GetAtomicNumber());
                    p.SetMomentum(lv);
#if G4VERSION_NUMBER  > 945 
                    G4Material mat("mymat",1,1);
                    mat.AddElement(element,1);
                    xs = hadronInelasticProcess->GetElementCrossSection(&p, element, &mat) / millibarn; // v9.6.p02
#else
                    xs = hadronInelasticProcess->GetMicroscopicCrossSection(&p, element, 295*kelvin) / millibarn; // v9.4.p04
#endif
                                     AMSEventR::h1(-id)->SetBinContent(k,xs);
                                     AMSEventR::h1(-id)->SetBinError(k,0);
                    }
                                     break;
                                }// end of if
                 }// end of for

              for(G4int j=0; j<processVector->size(); j++){
                bool inelok=false;
                 if(theParticle==G4Proton::Definition())inelok= (*processVector)[j]->GetProcessName()=="protonelastic" ;
                 else if(theParticle== G4Alpha::Definition())inelok= (*processVector)[j]->GetProcessName()=="ionelastic"  ;
                 else if(theParticle==   G4He3::Definition())inelok= (*processVector)[j]->GetProcessName()=="ionelastic"    ;
                 else if(theParticle==   G4Triton::Definition())inelok=  (*processVector)[j]->GetProcessName()=="ionelastic"    ;
                 else if(theParticle==   G4Deuteron::Definition())inelok= (*processVector)[j]->GetProcessName()=="ionelastic"    ;
                 else if ( strstr((const char*)theParticle->GetParticleName(),"strangelet"))inelok=  (*processVector)[j]->GetProcessName()=="strangeletelastic"    ;
                 else if( strstr((const char*)theParticle->GetParticleType(),"nucleus"))inelok=  ((*processVector)[j]->GetProcessName()=="ionelastic" );               
                 G4HadronElasticProcess * hadronInelasticProcess = dynamic_cast<G4HadronElasticProcess*>((*processVector)[j]);
                 if(inelok && hadronInelasticProcess){
                  for(int k=1;k<nbins+1;k++){
                    double xs=0;
                    const G4double R = AMSEventR::h1(100000+id)->GetBinCenter(k)*GeV;
                    lv.setZ(R*theParticle->GetAtomicNumber());
                    p.SetMomentum(lv);
#if G4VERSION_NUMBER  > 945 
                    G4Material mat("mymat",1,1);
                    mat.AddElement(element,1);
                    xs = hadronInelasticProcess->GetElementCrossSection(&p, element,&mat) / millibarn; // v9.6.p02
#else
                    xs = hadronInelasticProcess->GetMicroscopicCrossSection(&p, element, 295*kelvin) / millibarn; // v9.4.p04
#endif
//                    if(k%100==1)cout << element->GetName()<<" "<<R<<" "<<xs<<endl;
                                     AMSEventR::h1(100000+id)->SetBinContent(k,xs);
                                     AMSEventR::h1(100000+id)->SetBinError(k,0);
                    }
                                     break;
                                }// end of if
                 }// end of for




               }// end of block    
           }

      else{
         cerr<<"AMSG4Physics:SaveXS-E-UnableTofindParticle  "<<name<<endl;
      }

     }
     else   cerr<<"AMSG4Physics:SaveXS-E-UnableToConvertG3toG4Particle  "<<ipart<<endl;
}
