// ------------------------------------------------------------
//      AMS GEANT4 new Ion Physics 
//      Ion Eleastic for high Energy->DPM-JET
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09   Q.Yan
//        Modified:      2013-March-23 Q.Yan
//                                Fix Gap of Cross-Section
// -----------------------------------------------------------

#include "commons.h"
#include "g4physics_ion.h"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4HadronInelasticProcess.hh"
#include "G4BinaryLightIonReaction.hh"
#include "G4TripathiCrossSection.hh"
#include "G4TripathiLightCrossSection.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4IonProtonCrossSection.hh"
#include "G4GeneralSpaceNNCrossSection.hh"
#include "G4EMDissociation.hh"

#include "G4Version.hh"
#if G4VERSION_NUMBER  > 945 
#include  "G4GGNuclNuclCrossSection.hh"
#endif

#include "G4DPMJET2_5Model.hh"
#include "G4DPMJET2_5Interface.hh"
#include "G4DPMJET2_5CrossSection.hh"
#include "G4IonsHEAOCrossSection.hh"

using namespace std;

// -----------------------------------------------------------
IonDPMJETPhysics::IonDPMJETPhysics(G4bool val)
  : G4VHadronPhysics("ionInelasticDPMJET"),theIonBC(0),theIonBC1(0),useDPMJETXS(val)
{
  fTripathi = fTripathiLight = fShen = fIonH =0;
   theDPM=0;
}

// -----------------------------------------------------------
IonDPMJETPhysics::~IonDPMJETPhysics()
{}

// -----------------------------------------------------------
void IonDPMJETPhysics::ConstructProcess()
{
  G4double dpmemin=5.*GeV; 
  G4double emax = 1000.*TeV;
  //--Model Binary Cascade Low Energy
  theIonBC = new G4BinaryLightIonReaction();
  theIonBC->SetMinEnergy(0.0);
  theIonBC->SetMaxEnergy(dpmemin+0.1*GeV);
  //--High Energy
  theIonBC1 = new G4BinaryLightIonReaction();
  theIonBC1->SetMinEnergy(dpmemin);
  theIonBC1->SetMaxEnergy(emax);
  //--Model DPMJET
  theDPM = new G4DPMJET2_5Model();
  theDPM->SetMinEnergy(dpmemin);
  theDPM->SetMaxEnergy(emax);
  G4int dpmAmax=58;
  G4ElementTable::iterator iter;
  G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
  for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
    G4int AA  =(*iter)->GetN();
    if (AA<=dpmAmax && AA>1) { theDPM   ->ActivateFor(*iter); theIonBC1->DeActivateFor(*iter); }
    else             { theIonBC1->ActivateFor(*iter); theDPM   ->DeActivateFor(*iter); }
  }
  // theDPM->SetVerboseLevel(10);

  //---CrossSection
  fTripathi = new G4TripathiCrossSection();//< 1GeV  all A
  fTripathiLight = new G4TripathiLightCrossSection();//K/n <10GeV t d he3 he
  fIonH = new G4IonProtonCrossSection();//proton Target <20GeV (Inject A>4)
  fShen = new G4IonsShenCrossSection();
  dpmXS = new G4DPMJET2_5CrossSection;//DPMJET Cross-section<1000TeV
  HEAOXS = new G4IonsHEAOCrossSection();//HEAO  Cross-section
  AddProcess("dInelastic",G4Deuteron::Deuteron(),false);

  AddProcess("tInelastic",G4Triton::Triton(),false);
  AddProcess("He3Inelastic",G4He3::He3(),true);
  AddProcess("alphaInelastic",G4Alpha::Alpha(),true);
  AddProcess("ionInelastic",G4GenericIon::GenericIon(),true);
  G4cout << "IonDPMJETPhysics::ConstructProcess done! " << G4endl;

  if(G4FFKEY.UseEMDModel==1){
    cout<<"Also Use EMD Model"<<endl;
    G4EMDissociationCrossSection *EMDCrossSection = new G4EMDissociationCrossSection;
    G4ParticleDefinition* particle = G4GenericIon::GenericIon();
    G4ProcessManager* pManager = particle->GetProcessManager();
    G4EMDissociation *theEMD = new G4EMDissociation();
    theEMD->SetMinEnergy(100.0*MeV);
    theEMD->SetMaxEnergy(2000.0*GeV);
    // theEMD->SetVerboseLevel(10);
    G4ElementTable::iterator iter;
    G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
    for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
      theEMD->ActivateFor(*iter);
    }
    G4HadronInelasticProcess* fGenericIon_EMD = new G4HadronInelasticProcess("IonEMD",particle);
    G4HadronInelasticProcess* falpha_EMD = new G4HadronInelasticProcess("AlphaEMD",G4Alpha::Alpha());
    G4HadronInelasticProcess* fhe3_EMD = new G4HadronInelasticProcess("He3EMD",G4He3::He3());
    fGenericIon_EMD->AddDataSet(EMDCrossSection);
    fGenericIon_EMD->RegisterMe(theEMD);
    pManager->AddDiscreteProcess(fGenericIon_EMD);

    falpha_EMD->AddDataSet(EMDCrossSection);
    falpha_EMD->RegisterMe(theEMD);
    pManager->AddDiscreteProcess(falpha_EMD);

    fhe3_EMD->AddDataSet(EMDCrossSection);
    fhe3_EMD->RegisterMe(theEMD);
    pManager->AddDiscreteProcess(fhe3_EMD);
  }

}

// -----------------------------------------------------------
void IonDPMJETPhysics::AddProcess(const G4String& name,
                                  G4ParticleDefinition* part,
                                  G4bool isIon)
{
  G4HadronInelasticProcess* hadi = new G4HadronInelasticProcess(name, part);
  G4ProcessManager* pManager = part->GetProcessManager();
  pManager->AddDiscreteProcess(hadi);
  hadi->AddDataSet(fShen); 
  if ((G4FFKEY.IonPhysicsModel/10)%10==4) hadi->AddDataSet(dpmXS);
  if ((G4FFKEY.IonPhysicsModel/10)%10==3) hadi->AddDataSet(HEAOXS);
  if ((G4FFKEY.IonPhysicsModel/10)%10==2) {
    G4GeneralSpaceNNCrossSection* generalCrossSection = new G4GeneralSpaceNNCrossSection;
    hadi->AddDataSet(generalCrossSection);
  }
#if G4VERSION_NUMBER  > 945 
  if ((G4FFKEY.IonPhysicsModel/10)%10==1) {
    G4GGNuclNuclCrossSection* fGG = new G4GGNuclNuclCrossSection();
    hadi->AddDataSet(fGG);
  }
#endif
// Should be not be used with GG
  if(isIon && (G4FFKEY.IonPhysicsModel/100)%10==1) { hadi->AddDataSet(fIonH); }
  hadi->RegisterMe(theIonBC);
  hadi->RegisterMe(theIonBC1);
  hadi->RegisterMe(theDPM);
  if (G4FFKEY.HCrossSectionBias!=1) hadi->BiasCrossSectionByFactor2(G4FFKEY.HCrossSectionBias);
}
// -----------------------------------------------------------
