// ------------------------------------------------------------
//      AMS GEANT4 new Ion Physics 
//      Ion Eleastic for high Energy->DPM-JET
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09  Q.Yan
//        Modified:
// -----------------------------------------------------------

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
  : G4VHadronPhysics("ionInelasticDPMJET"),theIonBC(0),useDPMJETXS(val)
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
//--Model Binary Cascade
  theIonBC = new G4BinaryLightIonReaction();
  theIonBC->SetMinEnergy(0.0);

#ifdef G4_USE_DPMJET
  theIonBC->SetMaxEnergy(dpmemin);
//--Model DPMJET
  theDPM = new G4DPMJET2_5Model();
  theDPM->SetMinEnergy(dpmemin);
  theDPM->SetMaxEnergy(emax);
//  theDPM->SetVerboseLevel(10);
#else
  theIonBC->SetMaxEnergy(emax); 
#endif

//---CrossSection
  fTripathi = new G4TripathiCrossSection();//< 1GeV  all A
  fTripathiLight = new G4TripathiLightCrossSection();//K/n <10GeV t d he3 he
  fIonH = new G4IonProtonCrossSection();//proton Target <20GeV (Inject A>4)
  fShen = new G4IonsShenCrossSection();
#ifdef G4_USE_DPMJET
  dpmXS = new G4DPMJET2_5CrossSection;//<1000TeV Shen(Z >58)+ PMJET2.5 
  dpmXS->SetMinKinEnergy(10.*MeV);
  dpmXS->SetMaxKinEnergy(emax);
  dpmXS->SetMaxShenEnergy(dpmemin);//< 5GeV-ShenCross >5GeV DPMJETCross
//  dpmXS->SetVerboseLevel(100);
  dpmXS->PrintMessage();
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
  hadi->AddDataSet(dpmXS);
#endif
  hadi->AddDataSet(fTripathi);
//fTripathiLight or fIonH first use
  hadi->AddDataSet(fTripathiLight);
  if(isIon) { hadi->AddDataSet(fIonH); }
  hadi->RegisterMe(theIonBC);
#ifdef G4_USE_DPMJET
  hadi->RegisterMe(theDPM);
#endif

}
// -----------------------------------------------------------
