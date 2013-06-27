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

#ifdef G4_USE_DPMJET
#include "G4DPMJET2_5Model.hh"
#include "G4DPMJET2_5Interface.hh"
#include "G4DPMJET2_5CrossSection.hh"
#endif

using namespace std;

// -----------------------------------------------------------
IonDPMJETPhysics::IonDPMJETPhysics(G4bool val)
  : G4VHadronPhysics("ionInelasticDPMJET"),theIonBC(0),theIonBC1(0),useDPMJETXS(val)
{
  fTripathi = fTripathiLight = fShen = fIonH = 0;
#ifdef G4_USE_DPMJET
   theDPM=0;
#endif
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
  theIonBC->SetMaxEnergy(dpmemin);
//--High Energy
  theIonBC1 = new G4BinaryLightIonReaction();
  theIonBC1->SetMinEnergy(dpmemin);
  theIonBC1->SetMaxEnergy(emax);

#ifdef G4_USE_DPMJET
//--Model DPMJET
  theDPM = new G4DPMJET2_5Model();
  theDPM->SetMinEnergy(dpmemin);
  theDPM->SetMaxEnergy(emax);
  G4int dpmAmax=58;
  G4ElementTable::iterator iter;
  G4ElementTable *elementTable =const_cast<G4ElementTable*>(G4Element::GetElementTable());
  for (iter = elementTable->begin(); iter != elementTable->end(); ++iter) {
       G4int AA  =(*iter)->GetN();
//       G4cout<<"Element AA="<<AA<<G4endl;
       if(AA<=dpmAmax){theDPM   ->ActivateFor(*iter);theIonBC1->DeActivateFor(*iter);}
       else           {theIonBC1->ActivateFor(*iter);theDPM   ->DeActivateFor(*iter);}
   }
//  theDPM->SetVerboseLevel(10);
#endif

//---CrossSection
  fTripathi = new G4TripathiCrossSection();//< 1GeV  all A
  fTripathiLight = new G4TripathiLightCrossSection();//K/n <10GeV t d he3 he
  fIonH = new G4IonProtonCrossSection();//proton Target <20GeV (Inject A>4)
  fShen = new G4IonsShenCrossSection();
#ifdef G4_USE_DPMJET
  if(G4FFKEY.IonPhysicsModel==3){
    dpmXS = new G4DPMJET2_5CrossSection;//<1000TeV Shen(Z >58)+ PMJET2.5 
  }
#endif  
  AddProcess("dInelastic", G4Deuteron::Deuteron(),false);
  AddProcess("tInelastic",G4Triton::Triton(),false);
  AddProcess("He3Inelastic",G4He3::He3(),true);
  AddProcess("alphaInelastic", G4Alpha::Alpha(),true);
  AddProcess("ionInelastic",G4GenericIon::GenericIon(),true);
  G4cout << "IonDPMJETPhysics::ConstructProcess done! " << G4endl;
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
#ifdef G4_USE_DPMJET
  if(G4FFKEY.IonPhysicsModel==3)hadi->AddDataSet(dpmXS);
#endif
/*  hadi->AddDataSet(fTripathi);
//fTripathiLight or fIonH first use
  hadi->AddDataSet(fTripathiLight);*/
  if(isIon) { hadi->AddDataSet(fIonH); }
  hadi->RegisterMe(theIonBC);
  hadi->RegisterMe(theIonBC1);
#ifdef G4_USE_DPMJET
  hadi->RegisterMe(theDPM);
#endif

}
// -----------------------------------------------------------
