#ifndef _AMSPhysicsList_HadronIon
#define _AMSPhysicsList_HadronIon 1
#include "stdio.h"
#include "commons.h"
#include "G4ParticleDefinition.hh" 
#include "G4ProcessManager.hh" 
#include "G4LFission.hh" 
#include "G4LCapture.hh"
#include "G4Deuteron.hh" 
#include "G4Triton.hh" 
#include "G4He3.hh" 
#include "G4Alpha.hh" 
#include "G4GenericIon.hh"
#include "G4ExcitationHandler.hh" 
#include "G4Evaporation.hh" 
#include "G4FermiBreakUp.hh" 
#include "G4StatMF.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4HadronInelasticProcess.hh" 
#include "G4IonInelasticProcess.hh" 
#include "G4BinaryLightIonReaction.hh" 
#include "G4WilsonAbrasionModel.hh" 
#include "G4GeneralSpaceNNCrossSection.hh" 
#include "G4LEDeuteronInelastic.hh" 
#include "G4LETritonInelastic.hh" 
#include "G4LEAlphaInelastic.hh"
#include "G4EMDissociation.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "AMSPhysicsList_HadronIon.h" 
#include "G4LElastic.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4IonsHEAOCrossSection.hh"
#include "G4Version.hh"
#if G4VERSION_NUMBER  > 945 
#include  "G4GGNuclNuclCrossSection.hh"
#endif
#include "G4HadronElasticProcess.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4QMDReaction.hh"

//////////////////////////////////////////////////////////////////////////////// 
// 
AMSPhysicsList_HadronIon::AMSPhysicsList_HadronIon(const G4String& name)
    :G4VPhysicsConstructor(name)
{
} 
//////////////////////////////////////////////////////////////////////////////// 
// 
AMSPhysicsList_HadronIon::~AMSPhysicsList_HadronIon() 
{
  cout<<"~~~~~~~~~~~~~~~AMSPhysicsList_HadronIon::~AMSPhysicsList_HadronIon()~~~~~~~~~~~~~~"<<endl;
} 
//////////////////////////////////////////////////////////////////////////////// 
// 


void AMSPhysicsList_HadronIon::ConstructProcess() {

    SetVerboseLevel(10);

/*
    G4LElastic* elasticModel = new G4LElastic();
    G4HadronElasticProcess* hadElastProc = new G4HadronElasticProcess();
    hadElastProc->AddDataSet(new G4HadronElasticDataSet);
    hadElastProc->RegisterMe(elasticModel);
    //       pmanager->AddDiscreteProcess(hadElastProc);
*/

    G4ParticleDefinition* particle = 0;
    G4ProcessManager* pManager = 0;

    G4BinaryLightIonReaction * theGenIonBC = new G4BinaryLightIonReaction;
    theGenIonBC->SetMinEnergy(0.0*MeV);
    theGenIonBC->SetMaxEnergy(20000.0*GeV);

    G4QMDReaction * theGenIonQMD= new G4QMDReaction;
    theGenIonQMD->SetMinEnergy(0.0*MeV);
    theGenIonQMD->SetMaxEnergy(20000.0*GeV);

    G4EMDissociationCrossSection *EMDCrossSection = new G4EMDissociationCrossSection;
    G4GeneralSpaceNNCrossSection * generalCrossSection =  new G4GeneralSpaceNNCrossSection;
    G4IonsShenCrossSection* fShen = new G4IonsShenCrossSection();
    G4IonsHEAOCrossSection* HEAOXS = new G4IonsHEAOCrossSection();

#if G4VERSION_NUMBER  > 945 
    G4GGNuclNuclCrossSection* fGG = new G4GGNuclNuclCrossSection();
#endif

    G4LEDeuteronInelastic*  fDeuteronModel = new G4LEDeuteronInelastic;
    fDeuteronModel->SetMaxEnergy(100.0*MeV);

    G4LETritonInelastic*  fTritonModel = new G4LETritonInelastic;
    fTritonModel->SetMaxEnergy(100.0*MeV);

    G4LEAlphaInelastic* fAlphaModel = new G4LEAlphaInelastic;
    fAlphaModel->SetMaxEnergy(100.0*MeV);

    G4DeuteronInelasticProcess* fDeuteronProcess = new G4DeuteronInelasticProcess();
 
    G4TritonInelasticProcess* fTritonProcess = new G4TritonInelasticProcess();
 
    G4AlphaInelasticProcess* fAlphaProcess = new G4AlphaInelasticProcess();



    if(G4FFKEY.IonPhysicsModel%10==1){

	cout<<"Use G4BinaryLightIonReaction for All the Ions"<<endl;

	//	G4HadronElasticProcess* theElasticProcess = new G4HadronElasticProcess();

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
        fDeuteronProcess->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fDeuteronProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fDeuteronProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fDeuteronProcess->AddDataSet(fGG);
#endif
	fDeuteronProcess->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fDeuteronProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);

	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
        fTritonProcess->AddDataSet(fShen);	
        if (G4FFKEY.IonPhysicsModel/10==3) fTritonProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fTritonProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fTritonProcess->AddDataSet(fGG);	
#endif
	fTritonProcess->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fTritonProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);
	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
        fAlphaProcess->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fAlphaProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fAlphaProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fAlphaProcess->AddDataSet(fGG);
#endif
	fAlphaProcess->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fAlphaProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);
	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
        fhe3Ion->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fhe3Ion->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fhe3Ion->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fhe3Ion->AddDataSet(fGG);
#endif
	fhe3Ion->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fhe3Ion);
	//pManager->AddDiscreteProcess(hadElastProc);
	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
        fGenericIon->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fGenericIon->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fGenericIon->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fGenericIon->AddDataSet(fGG);
#endif
	fGenericIon->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fGenericIon);
	//pManager->AddDiscreteProcess(hadElastProc);

	if(G4FFKEY.UseEMDModel==1){
	  cout<<"Also Use EMD Model"<<endl;
	  G4EMDissociation *theEMD = new G4EMDissociation();
	  theEMD->SetMinEnergy(100.0*MeV);
	  theEMD->SetMaxEnergy(2000.0*GeV);
	  // theEMD->SetVerboseLevel(2);
	  G4ElementTable::iterator iter;
	  G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
	  for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
	    theEMD->ActivateFor(*iter);
	  }
	  G4HadronInelasticProcess* fGenericIon_EMD = new G4HadronInelasticProcess("IonEMD",particle);
	  
	  fGenericIon_EMD->AddDataSet(EMDCrossSection);
	  fGenericIon_EMD->RegisterMe(theEMD);
	  pManager->AddDiscreteProcess(fGenericIon_EMD);
	}
    }



    if(G4FFKEY.IonPhysicsModel%10==2){

	cout<<"Use G4QMD for All the Ions"<<endl;

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
        fDeuteronProcess->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fDeuteronProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fDeuteronProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fDeuteronProcess->AddDataSet(fGG);
#endif
	fDeuteronProcess->RegisterMe(theGenIonQMD);
	pManager->AddDiscreteProcess(fDeuteronProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);

	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
        fTritonProcess->AddDataSet(fShen);	
        if (G4FFKEY.IonPhysicsModel/10==3) fTritonProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fTritonProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fTritonProcess->AddDataSet(fGG);
#endif
	fTritonProcess->RegisterMe(theGenIonQMD);
	pManager->AddDiscreteProcess(fTritonProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);

	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
        fAlphaProcess->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fAlphaProcess->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fAlphaProcess->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fAlphaProcess->AddDataSet(fGG);
#endif
	fAlphaProcess->RegisterMe(theGenIonQMD);
	pManager->AddDiscreteProcess(fAlphaProcess);
	//	pManager->AddDiscreteProcess(hadElastProc);

	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
        fhe3Ion->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fhe3Ion->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fhe3Ion->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fhe3Ion->AddDataSet(fGG);
#endif
	fhe3Ion->RegisterMe(theGenIonQMD);
	pManager->AddDiscreteProcess(fhe3Ion);
        //	pManager->AddDiscreteProcess(hadElastProc);

	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
        fGenericIon->AddDataSet(fShen);
        if (G4FFKEY.IonPhysicsModel/10==3) fGenericIon->AddDataSet(HEAOXS);
        if (G4FFKEY.IonPhysicsModel/10==2) fGenericIon->AddDataSet(generalCrossSection);
#if G4VERSION_NUMBER  > 945 
        if (G4FFKEY.IonPhysicsModel/10==1) fGenericIon->AddDataSet(fGG);
#endif
	fGenericIon->RegisterMe(theGenIonQMD);
	pManager->AddDiscreteProcess(fGenericIon);
        //	pManager->AddDiscreteProcess(hadElastProc);

	if(G4FFKEY.UseEMDModel==1){
	  cout<<"Also Use EMD Model"<<endl;
	  G4EMDissociation *theEMD = new G4EMDissociation();
	  theEMD->SetMinEnergy(100.0*MeV);
	  theEMD->SetMaxEnergy(2000.0*GeV);
	  // theEMD->SetVerboseLevel(2);
	  G4ElementTable::iterator iter;
	  G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
	  for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
	    theEMD->ActivateFor(*iter);
	  }
	  G4HadronInelasticProcess* fGenericIon_EMD = new G4HadronInelasticProcess("IonEMD",particle);
	  
	  fGenericIon_EMD->AddDataSet(EMDCrossSection);
	  fGenericIon_EMD->RegisterMe(theEMD);
	  pManager->AddDiscreteProcess(fGenericIon_EMD);
	}

    }

}
#endif
