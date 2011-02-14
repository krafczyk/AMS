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
#include "G4HadronElasticProcess.hh"
//////////////////////////////////////////////////////////////////////////////// 
// 
AMSPhysicsList_HadronIon::AMSPhysicsList_HadronIon(const G4String& name)
    :G4VPhysicsConstructor(name)
{


} 
//////////////////////////////////////////////////////////////////////////////// 
// 
AMSPhysicsList_HadronIon::~AMSPhysicsList_HadronIon() 
{} 
//////////////////////////////////////////////////////////////////////////////// 
// 


void AMSPhysicsList_HadronIon::ConstructProcess() {

    SetVerboseLevel(10);

    G4LElastic* elasticModel = new G4LElastic();
    G4HadronElasticProcess* hadElastProc = new G4HadronElasticProcess();
    hadElastProc->AddDataSet(new G4HadronElasticDataSet);
    hadElastProc->RegisterMe(elasticModel);
    //       pmanager->AddDiscreteProcess(hadElastProc);



    G4ParticleDefinition* particle = 0;
    G4ProcessManager* pManager = 0;

    G4BinaryLightIonReaction * theGenIonBC = new G4BinaryLightIonReaction;
    theGenIonBC->SetMinEnergy(100.0*MeV);
    theGenIonBC->SetMaxEnergy(2000.0*GeV);

    G4WilsonAbrasionModel *theAM = new G4WilsonAbrasionModel(true);
    theAM->SetMinEnergy(100.0*MeV);
    theAM->SetMaxEnergy(2000.0*GeV);
    theAM->SetVerboseLevel(2);

    G4EMDissociation *theEMD = new G4EMDissociation();
    theEMD->SetMinEnergy(100.0*MeV);
    theEMD->SetMaxEnergy(2000.0*GeV);
    theEMD->SetVerboseLevel(2);

    G4ElementTable::iterator iter;
    G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
    for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
	theAM->ActivateFor(*iter);
	theEMD->ActivateFor(*iter);
    }

    G4EMDissociationCrossSection *EMDCrossSection = new G4EMDissociationCrossSection;
    G4GeneralSpaceNNCrossSection * generalCrossSection =  new G4GeneralSpaceNNCrossSection;


   G4LEDeuteronInelastic*  fDeuteronModel = new G4LEDeuteronInelastic;
    fDeuteronModel->SetMaxEnergy(100.0*MeV);

  G4LETritonInelastic*  fTritonModel = new G4LETritonInelastic;
    fTritonModel->SetMaxEnergy(100.0*MeV);

   G4LEAlphaInelastic* fAlphaModel = new G4LEAlphaInelastic;
    fAlphaModel->SetMaxEnergy(100.0*MeV);



    if(G4FFKEY.IonPhysicsModel==1){

	cout<<"Use G4BinaryLightIonReaction for All the Ions"<<endl;

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
	fDeuteronProcess.AddDataSet(generalCrossSection);
	fDeuteronProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fDeuteronProcess);
	pManager->AddDiscreteProcess(hadElastProc);

	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
	fTritonProcess.AddDataSet(generalCrossSection);
	fTritonProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fTritonProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
	fAlphaProcess.AddDataSet(generalCrossSection);
	fAlphaProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fAlphaProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
	fhe3Ion->AddDataSet(generalCrossSection);
	fhe3Ion->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fhe3Ion);
	pManager->AddDiscreteProcess(hadElastProc);
	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
	fGenericIon->AddDataSet(generalCrossSection);
	fGenericIon->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fGenericIon);
	pManager->AddDiscreteProcess(hadElastProc);

    }


    if(G4FFKEY.IonPhysicsModel==2){

	cout<<"Use G4WilsonAbrasionModel for All the Ions"<<endl;

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
	fDeuteronProcess.AddDataSet(generalCrossSection);
	fDeuteronProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fDeuteronProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
	fTritonProcess.AddDataSet(generalCrossSection);
	fTritonProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fTritonProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
	fAlphaProcess.AddDataSet(generalCrossSection);
	fAlphaProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fAlphaProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
	fhe3Ion->AddDataSet(generalCrossSection);
	fhe3Ion->RegisterMe(theAM);
	pManager->AddDiscreteProcess(fhe3Ion);
	pManager->AddDiscreteProcess(hadElastProc);
	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
	fGenericIon->AddDataSet(generalCrossSection);
	fGenericIon->RegisterMe(theAM);
	pManager->AddDiscreteProcess(fGenericIon);
	pManager->AddDiscreteProcess(hadElastProc);
    }



    if(G4FFKEY.IonPhysicsModel==3){

	cout<<"Use G4BinaryLightIonReaction for Deuteron, Triton, He3,He4, use G4EMDissociation for GenericIon"<<endl;

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
	fDeuteronProcess.AddDataSet(generalCrossSection);
	fDeuteronProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fDeuteronProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
	fTritonProcess.AddDataSet(generalCrossSection);
	fTritonProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fTritonProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
	fAlphaProcess.AddDataSet(generalCrossSection);
	fAlphaProcess.RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(&fAlphaProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
	fhe3Ion->AddDataSet(generalCrossSection);
	fhe3Ion->RegisterMe(theGenIonBC);
	pManager->AddDiscreteProcess(fhe3Ion);
	pManager->AddDiscreteProcess(hadElastProc);
	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
	fGenericIon->AddDataSet(generalCrossSection);
	fGenericIon->RegisterMe(theEMD);
	pManager->AddDiscreteProcess(fGenericIon);
	pManager->AddDiscreteProcess(hadElastProc);
    }


    if(G4FFKEY.IonPhysicsModel==4){

	cout<<"Use G4WilsonAbrasionModel for Deuteron, Triton, He3,He4, use G4EMDissociation for GenericIon"<<endl;

	//======Deuteron
	particle = G4Deuteron::Deuteron();
	pManager = particle->GetProcessManager();
	fDeuteronProcess.AddDataSet(generalCrossSection);
	fDeuteronProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fDeuteronProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Triton
	particle = G4Triton::Triton();
	pManager = particle->GetProcessManager();
	fTritonProcess.AddDataSet(generalCrossSection);
	fTritonProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fTritonProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======Alpha
	particle = G4Alpha::Alpha();
	pManager = particle->GetProcessManager();
	fAlphaProcess.AddDataSet(generalCrossSection);
	fAlphaProcess.RegisterMe(theAM);
	pManager->AddDiscreteProcess(&fAlphaProcess);
	pManager->AddDiscreteProcess(hadElastProc);
	//======He3
	particle = G4He3::He3();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
	fhe3Ion->AddDataSet(generalCrossSection);
	fhe3Ion->RegisterMe(theAM);
	pManager->AddDiscreteProcess(fhe3Ion);
	pManager->AddDiscreteProcess(hadElastProc);
	//======GenericIon
	particle = G4GenericIon::GenericIon();
	pManager = particle->GetProcessManager();
	G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
	fGenericIon->AddDataSet(generalCrossSection);
	fGenericIon->RegisterMe(theEMD);
	pManager->AddDiscreteProcess(fGenericIon);
	pManager->AddDiscreteProcess(hadElastProc);
    }



    /**
 if(G4FFKEY.IonPhysicsModel==2){


     cout<<"Use G4WilsonAbrasionModel for All the Ions"<<endl;



//======Deuteron
    particle = G4Deuteron::Deuteron();
    pManager = particle->GetProcessManager();
    fDeuteronProcess.AddDataSet(generalCrossSection);
    fDeuteronProcess.RegisterMe(theAM);
    fDeuteronProcess.RegisterMe(fDeuteronModel);
    pManager->AddDiscreteProcess(&fDeuteronProcess);

//======Triton
    particle = G4Triton::Triton();
    pManager = particle->GetProcessManager();
    fTritonProcess.AddDataSet(generalCrossSection);
    fTritonProcess.RegisterMe(theAM);
    fTritonProcess.RegisterMe(fTritonModel);
    pManager->AddDiscreteProcess(&fTritonProcess);

//======Alpha
    particle = G4Alpha::Alpha();
    pManager = particle->GetProcessManager();
    fAlphaProcess.AddDataSet(generalCrossSection);
    fAlphaProcess.RegisterMe(theAM);
    fAlphaProcess.RegisterMe(fAlphaModel);
    pManager->AddDiscreteProcess(&fAlphaProcess);

//======He3
    particle = G4He3::He3();
    pManager = particle->GetProcessManager();
    G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
    fhe3Ion->AddDataSet(generalCrossSection);
    fhe3Ion->RegisterMe(theAM);
    fhe3Ion->RegisterMe(theGenIonBC);
    pManager->AddDiscreteProcess(fhe3Ion);


//======GenericIon
    particle = G4GenericIon::GenericIon();
    pManager = particle->GetProcessManager();
    G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
    fGenericIon->AddDataSet(generalCrossSection);
    fGenericIon->RegisterMe(theAM);
    fGenericIon->RegisterMe(theGenIonBC);

    G4HadronInelasticProcess *fGenericIonEMD = new G4HadronInelasticProcess("EMDissociation",particle);
    //fGenericIonEMD->AddDataSet(EMDCrossSection);
    fGenericIonEMD->AddDataSet(generalCrossSection);
    fGenericIonEMD->RegisterMe(theEMD);
    fGenericIonEMD->RegisterMe(theGenIonBC);


    pManager->AddDiscreteProcess(fGenericIonEMD);
    //       pManager->AddDiscreteProcess(fGenericIon);




 }
**/








}
#endif
