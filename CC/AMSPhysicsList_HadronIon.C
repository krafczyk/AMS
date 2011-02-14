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

  std::cout<<"Test, Do Nothing"<<std::endl;

  int test=0;
  
} 
//////////////////////////////////////////////////////////////////////////////// 
// 
AMSPhysicsList_HadronIon::~AMSPhysicsList_HadronIon() 
{} 
//////////////////////////////////////////////////////////////////////////////// 
// 

void AMSPhysicsList_HadronIon::ConstructParticle(){ 

  // Construct light ions 
G4IonConstructor pConstructor; 
pConstructor.ConstructParticle();

}





void AMSPhysicsList_HadronIon::ConstructProcess() {

  SetVerboseLevel(10);

  /**

G4ParticleDefinition* particle = 0; G4ProcessManager* pmanager = 0;





G4BinaryLightIonReaction  *theGenIonBIC = new G4BinaryLightIonReaction;
 theGenIonBIC->SetMinEnergy(0.0*MeV);
 theGenIonBIC->SetMaxEnergy(10.0*GeV);


G4BinaryLightIonReaction  *theGenIonBIC_ForTriton = new G4BinaryLightIonReaction;
 theGenIonBIC_ForTriton->SetMinEnergy(0.0*MeV);
 theGenIonBIC_ForTriton->SetMaxEnergy(1000.0*GeV);





 G4HadronicInteraction* theGenIon;
 

 if(G4FFKEY.IonPhysicsModel==1){
   theGenIon = new G4BinaryLightIonReaction;
   theGenIon->SetMinEnergy(10.0*GeV);
   theGenIon->SetMaxEnergy(2000.0*GeV);
 }
 else if(G4FFKEY.IonPhysicsModel==2){
   theGenIon = new G4WilsonAbrasionModel();
   theGenIon->SetVerboseLevel(0);
   theGenIon->SetMinEnergy(10.0*GeV);
   theGenIon->SetMaxEnergy(2000.0*GeV);
   G4ElementTable::iterator iter; 
   G4ElementTable *elementTable =	 const_cast <G4ElementTable*> (G4Element::GetElementTable());
   for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
     theGenIon->ActivateFor(*iter);
   } 
 }else if(G4FFKEY.IonPhysicsModel==3){
   
   theGenIon = new G4EMDissociation();
   theGenIon->SetMinEnergy(100.0*MeV);
   G4ElementTable::iterator iter; 
   G4ElementTable *elementTable =	 const_cast <G4ElementTable*> (G4Element::GetElementTable());
   for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
     theGenIon->ActivateFor(*iter);
   } 

   G4EMDissociationCrossSection *EMDCrossSection = new G4EMDissociationCrossSection;
   fGenericIon->AddDataSet(EMDCrossSection); 
fGenericIon->RegisterMe(theEMD);



   
}



 G4LElastic* elasticModel = new G4LElastic();
 G4HadronElasticProcess* hadElastProc = new G4HadronElasticProcess();
 hadElastProc->RegisterMe(elasticModel);


 theGenIon->SetVerboseLevel(10);

G4GeneralSpaceNNCrossSection * generalCrossSection = new G4GeneralSpaceNNCrossSection;





// deuteron 
particle = G4Deuteron::Deuteron(); 
pmanager = particle->GetProcessManager(); 
theIPdeuteron=new G4DeuteronInelasticProcess();
//G4LEDeuteronInelastic* theDIModel = new G4LEDeuteronInelastic; 
//theDIModel->SetMaxEnergy(100*MeV); 
//theIPdeuteron.RegisterMe(theDIModel); 
theIPdeuteron->AddDataSet(generalCrossSection); 
//theIPdeuteron->RegisterMe(theGenIon); 
//theIPdeuteron->RegisterMe(theGenIonBIC); 
theIPdeuteron->RegisterMe(theGenIonBIC_ForTriton);
pmanager->AddDiscreteProcess(hadElastProc);
pmanager->AddDiscreteProcess(theIPdeuteron);

// triton 
particle = G4Triton::Triton(); 
pmanager = particle->GetProcessManager(); 
theIPtriton =new G4TritonInelasticProcess();
theIPtriton->AddDataSet(generalCrossSection);
//G4LETritonInelastic* theTIModel = new G4LETritonInelastic; 
//theTIModel->SetMaxEnergy(100*MeV);
//theIPtriton.RegisterMe(theTIModel);
//theIPtriton->RegisterMe(theGenIon);
theIPtriton->RegisterMe(theGenIonBIC_ForTriton);
pmanager->AddDiscreteProcess(hadElastProc);
pmanager->AddDiscreteProcess(theIPtriton);



// alpha 
particle = G4Alpha::Alpha(); 
pmanager = particle->GetProcessManager(); 
//G4LEAlphaInelastic* theAIModel = new G4LEAlphaInelastic; 
//theAIModel->SetMaxEnergy(100*MeV); 
theIPalpha =new G4AlphaInelasticProcess();
theIPalpha->AddDataSet(generalCrossSection); 
//theIPalpha.RegisterMe(theAIModel); 
theIPalpha->RegisterMe(theGenIon); 
theIPalpha->RegisterMe(theGenIonBIC); 
pmanager->AddDiscreteProcess(hadElastProc);
pmanager->AddDiscreteProcess(theIPalpha);

// He3 
particle = G4He3::He3(); 
pmanager = particle->GetProcessManager(); 
theIPHe3 =new G4HadronInelasticProcess("He3Inelastic",particle);
theIPHe3->AddDataSet(generalCrossSection);
theIPHe3->RegisterMe(theGenIon);
theIPHe3->RegisterMe(theGenIonBIC);
//theIPHe3->RegisterMe(theAM);
pmanager->AddDiscreteProcess(hadElastProc);
pmanager->AddDiscreteProcess(theIPHe3);



// GenericIon 
particle = G4GenericIon::GenericIon(); 
pmanager = particle->GetProcessManager(); 
 theIPGenericIon =new G4HadronInelasticProcess("IonInelastic",particle);
theIPGenericIon->AddDataSet(generalCrossSection); 
theIPGenericIon->RegisterMe(theGenIon); 
theIPGenericIon->RegisterMe(theGenIonBIC); 
pmanager->AddDiscreteProcess(hadElastProc);
pmanager->AddDiscreteProcess(theIPGenericIon);
  **/

/**
       G4BinaryLightIonReaction* fBC= new G4BinaryLightIonReaction();








G4WilsonAbrasionModel *theAM = new G4WilsonAbrasionModel();
 theAM->SetVerboseLevel(2);
theAM->SetMinEnergy(100.0*MeV);
G4BinaryLightIonReaction * theGenIonBC = new G4BinaryLightIonReaction; 
theGenIonBC->SetMinEnergy(0.0*MeV); 
theGenIonBC->SetMaxEnergy(100.0*MeV);
G4ElementTable::iterator iter; 
G4ElementTable *elementTable =	 const_cast <G4ElementTable*> (G4Element::GetElementTable());
 for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
  theAM->ActivateFor(*iter);
 } 
G4GeneralSpaceNNCrossSection * generalCrossSection = new G4GeneralSpaceNNCrossSection;

G4HadronInelasticProcess* theIPGenericIon =new G4HadronInelasticProcess("IonInelastic",particle);
theIPGenericIon->AddDataSet(generalCrossSection); 
theIPGenericIon->RegisterMe(theGenIonBC); 
theIPGenericIon->RegisterMe(theAM); 
pmanager->AddDiscreteProcess(theIPGenericIon);
**/
       //       G4LElastic* elasticModel = new G4LElastic();
       //      G4HadronElasticProcess* hadElastProc = new G4HadronElasticProcess();
       //       hadElastProc->AddDataSet(new G4HadronElasticDataSet);
       //       hadElastProc->RegisterMe(elasticModel);
       //       pmanager->AddDiscreteProcess(hadElastProc);


// //  	  G4EMDissociation* ionModel = new G4EMDissociation();
      
//       G4BinaryLightIonReaction* ionModel = new G4BinaryLightIonReaction();
//       G4IonInelasticProcess* inelProcess = new G4IonInelasticProcess();
//       inelProcess->RegisterMe(ionModel);
//       pmanager->AddDiscreteProcess(inelProcess);

// // 	  G4WilsonAbrasionModel* ionModel = new G4WilsonAbrasionModel();
// // 	  G4IonInelasticProcess* inelProcess = new G4IonInelasticProcess();
// // 	  inelProcess->RegisterMe(ionModel);
// // 	  pmanager->AddDiscreteProcess(inelProcess);







/**	       
       G4LElastic* elasticModel = new G4LElastic();
       G4HadronElasticProcess* hadElastProc = new G4HadronElasticProcess();
       hadElastProc->RegisterMe(elasticModel);
       pmanager->AddDiscreteProcess(hadElastProc);


       G4BinaryLightIonReaction * theIonBC= new G4BinaryLightIonReaction;
       theIonBC->SetMinEnergy(1*MeV);
       theIonBC->SetMaxEnergy(20*GeV);
       G4TripathiCrossSection * TripathiCrossSection= new G4TripathiCrossSection;
       G4IonsShenCrossSection * aShen = new G4IonsShenCrossSection;

       // this will be the model class for high energies                                                                                                                                                                          
       G4TheoFSGenerator * theTheoModel = new G4TheoFSGenerator;

       // here come the high energy parts                                                                                                                                                                                         
       G4Evaporation * theEvaporation = new G4Evaporation;
       G4FermiBreakUp * theFermiBreakUp = new G4FermiBreakUp;
       G4StatMF * theMF = new G4StatMF;
       // Evaporation logic                                                                                                                                                                                                       
       G4ExcitationHandler * theHandler = new G4ExcitationHandler;
       theHandler->SetEvaporation(theEvaporation);
       theHandler->SetFermiModel(theFermiBreakUp);
       theHandler->SetMultiFragmentation(theMF);
       theHandler->SetMaxAandZForFermiBreakUp(12, 6);
       theHandler->SetMinEForMultiFrag(5*MeV);
       // Pre equilibrium stage                                                                                                                                                                                                   
       G4PreCompoundModel * thePreEquilib = new G4PreCompoundModel(theHandler);

       // the string model; still not quite according to design - Explicite use of the forseen interfaces                                                                                                                         

       // a no-cascade generator-precompound interaface                                                                                                                                                                           
       G4GeneratorPrecompoundInterface * theCascade = new G4GeneratorPrecompoundInterface;
       theCascade->SetDeExcitation(thePreEquilib);

       G4VPartonStringModel * theStringModel;
       theStringModel = new G4QGSModel<G4QGSParticipants>;
       theTheoModel->SetTransport(theCascade);
       theTheoModel->SetHighEnergyGenerator(theStringModel);
       theTheoModel->SetMinEnergy(10*GeV);  // 15 GeV may be the right limit                                                                                                                                                      
       theTheoModel->SetMaxEnergy(100*TeV);

G4IonInelasticProcess*  theIonInelasticProcess =    new G4IonInelasticProcess();
  theIonInelasticProcess->AddDataSet(TripathiCrossSection);
  theIonInelasticProcess->AddDataSet(aShen);



  theIonInelasticProcess->RegisterMe(theIonBC);
  theIonInelasticProcess->RegisterMe(theTheoModel);
  pmanager->AddDiscreteProcess(theIonInelasticProcess);




**/



G4ParticleDefinition* particle = 0; 
G4ProcessManager* pManager = 0;


// Elastic Process 
//theElasticModel = new G4LElastic(); 
//theElasticProcess.RegisterMe(theElasticModel);

 G4WilsonAbrasionModel *theAM = new G4WilsonAbrasionModel(true);
 theAM->SetVerboseLevel(2);
 theAM->SetMinEnergy(100.0*MeV);
 // theAM->SetMaxEnergy(1000.0*GeV);
 G4BinaryLightIonReaction * theGenIonBC = new G4BinaryLightIonReaction; 
 theGenIonBC->SetMinEnergy(0.0*MeV); 
theGenIonBC->SetMaxEnergy(100.0*MeV);
 
 G4ElementTable::iterator iter; 
G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable()); 
for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
   theAM->ActivateFor(*iter);
 } 
G4GeneralSpaceNNCrossSection * generalCrossSection =  new G4GeneralSpaceNNCrossSection;

// theParticleIterator->reset();

//  while( (*theParticleIterator)() ){ 
// G4ParticleDefinition* particle = theParticleIterator->value(); 
// G4ProcessManager* pManager = particle->GetProcessManager(); 
// G4String particleName = particle->GetParticleName();
//      //     pManager->AddDiscreteProcess(&theElasticProcess);

//     if (particleName == "deuteron") {


// deuteron                                                                                                                       
 particle = G4Deuteron::Deuteron();  
 pManager = particle->GetProcessManager();                                                                                         
 fDeuteronProcess.AddDataSet(generalCrossSection); 
       fDeuteronProcess.RegisterMe(theAM); 
       fDeuteronModel = new G4LEDeuteronInelastic; 
       fDeuteronModel->SetMaxEnergy(100.0*MeV); 
       fDeuteronProcess.RegisterMe(fDeuteronModel); 
       pManager->AddDiscreteProcess(&fDeuteronProcess);
       //     }
       //     else if (particleName == "triton") {

       particle = G4Triton::Triton();
       pManager = particle->GetProcessManager(); 

       fTritonProcess.AddDataSet(generalCrossSection); 
       fTritonProcess.RegisterMe(theAM); 
       fTritonModel = new G4LETritonInelastic; 
       fTritonModel->SetMaxEnergy(100.0*MeV); 
       fTritonProcess.RegisterMe(fTritonModel); 
       pManager->AddDiscreteProcess(&fTritonProcess);
       //     }
       //     else if (particleName == "alpha") {
                                                                                                            
       particle = G4Alpha::Alpha(); 
       pManager = particle->GetProcessManager(); 
       fAlphaProcess.AddDataSet(generalCrossSection); 
       fAlphaProcess.RegisterMe(theAM); 
       fAlphaModel = new G4LEAlphaInelastic; 
       fAlphaModel->SetMaxEnergy(100.0*MeV); 
       fAlphaProcess.RegisterMe(fAlphaModel); 
       pManager->AddDiscreteProcess(&fTritonProcess);
       //     }else if (particleName == "he3") {

       particle = G4He3::He3();
       pManager = particle->GetProcessManager(); 
       G4HadronInelasticProcess* fhe3Ion = new G4HadronInelasticProcess("He3Inelastic",particle);
       fhe3Ion->AddDataSet(generalCrossSection); 
       fhe3Ion->RegisterMe(theAM); 
       fhe3Ion->RegisterMe(theGenIonBC);
       pManager->AddDiscreteProcess(fhe3Ion);
       //     } 
       //     else if (particleName == "GenericIon") {








       G4EMDissociation *theEMD = new G4EMDissociation(); 
       theEMD->SetMinEnergy(100.0*MeV);
       //theEMD->SetMaxEnergy(1000.0*GeV);
       for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) theEMD->ActivateFor(*iter); 
       theEMD->SetVerboseLevel(20);
       G4EMDissociationCrossSection *EMDCrossSection = new G4EMDissociationCrossSection;



       particle = G4GenericIon::GenericIon();
       pManager = particle->GetProcessManager();


       G4HadronInelasticProcess* fGenericIon = new G4HadronInelasticProcess("IonInelastic",particle);
       fGenericIon->AddDataSet(generalCrossSection); 
       fGenericIon->RegisterMe(theAM); 
       fGenericIon->RegisterMe(theGenIonBC); 
       fGenericIon->SetVerboseLevel(20);

       G4HadronInelasticProcess *fGenericIonEMD = new G4HadronInelasticProcess("EMDissociation",particle); 
       fGenericIonEMD->SetVerboseLevel(20);
       fGenericIonEMD->AddDataSet(EMDCrossSection); 
       //       fGenericIonEMD->AddDataSet(generalCrossSection); 
       fGenericIonEMD->RegisterMe(theEMD); 
       fGenericIonEMD->RegisterMe(theGenIonBC); 


       pManager->AddDiscreteProcess(fGenericIonEMD);
       //       pManager->AddDiscreteProcess(fGenericIon); 
       



       //     }
     //##############################################################################
       //}     
   
 
} //////////////////////////////////////////////////////////////////////////////// //

#endif
