#include "G4EmProcessSubType.hh"
#include "G4LossTableManager.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ParticleDefinition.hh"
#include "G4Poisson.hh"
#include "g4rich.h"

#include "richradid.h"

// Functions implemented in richradid.C
extern "C" geant getphotons_(geant *charge,geant *min_index,geant *vect,geant *step);
extern "C" geant getmomentum_(geant *index);

RichG4Cerenkov::RichG4Cerenkov(const G4String& processName, G4ProcessType type)
  : G4Cerenkov(processName,type){
  G4cout<<"        RichG4Cerenkov::dealing with Rich fine grained refractive indexes"<<G4endl;  
}

const G4String aerogel_name("RICH_AEROGEL");
const G4String naf_name("RICH_NaF");

G4VParticleChange*
RichG4Cerenkov::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep){

  // Do not do RICH stuff
  return G4Cerenkov::PostStepDoIt(aTrack,aStep);


  // Get the current point
  G4Material &material=*aTrack.GetMaterial();

  if(!(material.GetName()==aerogel_name) && 
     !(material.GetName()==naf_name))
    return G4Cerenkov::PostStepDoIt(aTrack,aStep);

  // This is a RICH radiator, use the tools already available to compute everything
  // Copy and paste of original G4Cerenkov class

  aParticleChange.Initialize(aTrack);

  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  const G4Material* aMaterial = aTrack.GetMaterial();

  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();

  G4ThreeVector x0 = pPreStepPoint->GetPosition();
  G4ThreeVector p0 = aStep.GetDeltaPosition().unit();
  G4double t0 = pPreStepPoint->GetGlobalTime();


  G4MaterialPropertiesTable* aMaterialPropertiesTable =
    aMaterial->GetMaterialPropertiesTable();
  if (!aMaterialPropertiesTable) return pParticleChange;
  const G4MaterialPropertyVector* Rindex =
    aMaterialPropertiesTable->GetProperty("RINDEX");
  if (!Rindex) return pParticleChange;
  
  // particle charge
  const G4double charge = aParticle->GetDefinition()->GetPDGCharge();
  
  // particle beta
  const G4double beta = (pPreStepPoint ->GetBeta() +
			 pPostStepPoint->GetBeta())/2.;

  G4double step_length = aStep.GetStepLength();

  G4double MeanNumberOfPhotons=GetAverageNumberOfPhotons(charge,beta,step_length/cm,x0,p0);

  // Copy and paste from the original function
  if (MeanNumberOfPhotons <= 0.0) {
    // return unchanged particle and no secondaries
    aParticleChange.SetNumberOfSecondaries(0);
    return pParticleChange;
  }
  
  MeanNumberOfPhotons *= step_length;
  
  G4int NumPhotons = (G4int) G4Poisson(MeanNumberOfPhotons);
  
  if (NumPhotons <= 0) {
    // return unchanged particle and no secondaries  
    aParticleChange.SetNumberOfSecondaries(0);
    return pParticleChange;
  }

  ////////////////////////////////////////////////////////////////
  
  aParticleChange.SetNumberOfSecondaries(NumPhotons);
  
  if (fTrackSecondariesFirst) {
    if (aTrack.GetTrackStatus() == fAlive )
      aParticleChange.ProposeTrackStatus(fSuspend);
  }
  
  ////////////////////////////////////////////////////////////////

  G4double BetaInverse = 1./beta;
  for (G4int i = 0; i < NumPhotons; i++) {
    G4double sampledEnergy, sampledRI; 
    G4double cosTheta, sin2Theta;

    geant _RINDEX;
    geant _p;
    _p=getmomentum_(&_RINDEX); // _p is in GeV/c, _RINDEX is the refractive index for this guy

    sampledEnergy=_p*GeV;     // energy in G4 units
    sampledRI=_RINDEX;
    cosTheta = BetaInverse / sampledRI;  
    sin2Theta = (1.0 - cosTheta)*(1.0 + cosTheta);

    // Generate random position of photon on cone surface 
    // defined by Theta 
    G4double rand = G4UniformRand();
    G4double phi = twopi*rand;
    G4double sinPhi = sin(phi);
    G4double cosPhi = cos(phi);
    // calculate x,y, and z components of photon energy
    // (in coord system with primary particle direction 
    //  aligned with the z axis)
    
    G4double sinTheta = sqrt(sin2Theta); 
    G4double px = sinTheta*cosPhi;
    G4double py = sinTheta*sinPhi;
    G4double pz = cosTheta;
    // Create photon momentum direction vector 
    // The momentum direction is still with respect
    // to the coordinate system where the primary
    // particle direction is aligned with the z axis  
    G4ParticleMomentum photonMomentum(px, py, pz);
    // Rotate momentum direction back to global reference
    // system 
    photonMomentum.rotateUz(p0);
    // Determine polarization of new photon 
    G4double sx = cosTheta*cosPhi;
    G4double sy = cosTheta*sinPhi; 
    G4double sz = -sinTheta;
    G4ThreeVector photonPolarization(sx, sy, sz);
    // Rotate back to original coord system 
    photonPolarization.rotateUz(p0);
    // Generate a new photon:
    G4DynamicParticle* aCerenkovPhoton =
      new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(), 
			    photonMomentum);
    aCerenkovPhoton->SetPolarization
      (photonPolarization.x(),
       photonPolarization.y(),
       photonPolarization.z());
    aCerenkovPhoton->SetKineticEnergy(sampledEnergy);
    rand = G4UniformRand();
    G4double delta=rand*aStep.GetStepLength();
    G4double deltaTime = delta /
      ((pPreStepPoint->GetVelocity()+
	pPostStepPoint->GetVelocity())/2.);
    
    G4double aSecondaryTime = t0 + deltaTime;
    
    G4ThreeVector aSecondaryPosition = x0 + rand * aStep.GetDeltaPosition();
    
    G4Track* aSecondaryTrack = new G4Track(aCerenkovPhoton,aSecondaryTime,aSecondaryPosition);
    aSecondaryTrack->SetTouchableHandle(aStep.GetPreStepPoint()->GetTouchableHandle());
    aSecondaryTrack->SetParentID(aTrack.GetTrackID());
    aParticleChange.AddSecondary(aSecondaryTrack);
  }

  if (verboseLevel>0) {
    G4cout << "\n Exiting from RichG4Cerenkov::DoIt -- NumberOfSecondaries = " 
	   << aParticleChange.GetNumberOfSecondaries() << G4endl;
  }
  
  return pParticleChange;
}


G4double 
RichG4Cerenkov::GetAverageNumberOfPhotons(const G4double charge,
					  const G4double beta, 
					  const G4double step, // in cm
					  const G4ThreeVector& x0,
					  const G4ThreeVector& p0)
{
  geant _charge=charge;                 // charge of the particle
  geant _min_index=1/beta;              // minimum index to get emission for that beta
  geant _step=step;
  geant _norma=sqrt(p0[0]*p0[0]+p0[1]*p0[1]+p0[2]*p0[2]);
  geant _vec[6]={x0[0]/cm,x0[1]/cm,x0[2]/cm,p0[0]/_norma,p0[1]/_norma,p0[2]/_norma};
  for(int i=0;i<3;i++) _vec[i]+=0.5*_step*_vec[i+3];

  return getphotons_(&_charge,&_min_index,_vec,&_step)*0.1/_step;
}


G4double RichG4Cerenkov::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
							      G4double,
							      G4ForceCondition* condition)
{

  // Skip rich specific
  return G4Cerenkov::PostStepGetPhysicalInteractionLength(aTrack,0,condition);

  // Get the current radiator and skip the rich part if it is not rich
  const G4Material* aMaterial = aTrack.GetMaterial();
  const G4Material &material=*aMaterial;
  if(!(material.GetName()==aerogel_name) && 
     !(material.GetName()==naf_name))
    return G4Cerenkov::PostStepGetPhysicalInteractionLength(aTrack,0,condition);

  const G4Step &aStep=*aTrack.GetStep();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
  G4ThreeVector p0 = aStep.GetDeltaPosition().unit();
  G4ThreeVector x0 = pPostStepPoint->GetPosition();
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  const G4double charge = aParticle->GetDefinition()->GetPDGCharge();
  const G4double beta = aParticle->GetTotalMomentum()/aParticle->GetTotalEnergy();
  *condition=NotForced;
  G4double MeanNumberOfPhotons = GetAverageNumberOfPhotons(charge,beta,0.1,x0,p0);
  if(MeanNumberOfPhotons==0) return DBL_MAX; 
  *condition=StronglyForced;
  if(fMaxPhotons<=0)  return 800.0/MeanNumberOfPhotons; 
  return fMaxPhotons/MeanNumberOfPhotons;
}


