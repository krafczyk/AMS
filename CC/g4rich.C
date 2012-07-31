//---Modify by Qi Yan OpBoundaryProcess->public TOFG4OpBoundaryProcess
#include "G4EmProcessSubType.hh"
#include "G4LossTableManager.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ParticleDefinition.hh"
#include "G4Poisson.hh"
#include "G4ios.hh"
#include "G4OpProcessSubType.hh"
#include "G4GeometryTolerance.hh"
#include "g4rich.h"

#include "richradid.h"

// Functions implemented in richradid.C
extern "C" geant getphotons_(geant *charge,geant *min_index,geant *vect,geant *step);
extern "C" geant getmomentum_(geant *index);

RichG4Cerenkov::RichG4Cerenkov(const G4String& processName, G4ProcessType type)
  : G4Cerenkov(processName,type){
  G4cout<<"RichG4Cerenkov::dealing with Rich fine grained refractive indexes"<<G4endl;  
}

const G4String aerogel_name("RICH_AEROGEL");
const G4String naf_name("RICH_NaF");

G4VParticleChange*
RichG4Cerenkov::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep){

  // Do not do RICH stuff
  //  return G4Cerenkov::PostStepDoIt(aTrack,aStep);


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
  if(step<=0) return 0;
  geant _charge=charge;                 // charge of the particle
  geant _min_index=1/beta;              // minimum index to get emission for that beta
  geant _step=step;
  geant _norma=sqrt(p0[0]*p0[0]+p0[1]*p0[1]+p0[2]*p0[2]);
  geant _vec[6];
  for(int i=0;i<3;i++) _vec[i]=x0[i]/cm;
  if(_norma==0){_vec[3]=_vec[4]=0;_vec[5]=-1;_norma=1;}
  else for(int i=0;i<3;i++) _vec[i+3]=p0[i]/_norma;
  for(int i=0;i<3;i++) _vec[i]+=0.5*_step*_vec[i+3];
  return getphotons_(&_charge,&_min_index,_vec,&_step)*0.1/_step;
}


G4double RichG4Cerenkov::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
							      G4double,
							      G4ForceCondition* condition)
{
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




/////////////////////// IMPLEMENTATION OF REFRACTION ///////////////////////


G4VParticleChange*
RichG4OpBoundaryProcess::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
  // I do not know where these guys come, but I kill them explicitly
  if(isnan(aTrack.GetPosition()[0]) || isnan(aTrack.GetMomentum()[0])){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  // Initialization
  aParticleChange.Initialize(aTrack);

  // Get the properties of the particle
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();

  G4double thePhotonMomentum      = aParticle->GetTotalMomentum();
  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();


  if(isnan(pPreStepPoint->GetPosition()[0])){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  
  if (pPostStepPoint->GetStepStatus() != fGeomBoundary){
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  if (aTrack.GetStepLength()<=1e-7){
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  
  G4Material *Material1 = pPreStepPoint  -> GetMaterial();
  G4Material *Material2 = pPostStepPoint -> GetMaterial();
  
  // Follow the standard path if not RICH radiator to vacuum transition
  if(!(Material1->GetName()==aerogel_name) && 
     !(Material1->GetName()==naf_name))
    return G4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);
//      return TOFG4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);

  if(!(Material2->GetName()=="VACUUM"))
    return G4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);
//      return TOFG4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);
   
  if(!Material1->GetMaterialPropertiesTable() ||
     !Material2->GetMaterialPropertiesTable()){
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }


  OldMomentum       = aParticle->GetMomentumDirection();
  OldPolarization   = aParticle->GetPolarization();


  // Get the normal of the surface
  G4ThreeVector theGlobalPoint = pPostStepPoint->GetPosition();


  G4Navigator* theNavigator =
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
  
  G4ThreeVector theLocalPoint = theNavigator->
    GetGlobalToLocalTransform().
    TransformPoint(theGlobalPoint);
  
  G4ThreeVector theLocalNormal;   // Normal points back into volume
  
  G4bool valid;
  theLocalNormal = theNavigator->GetLocalExitNormal(&valid);
  
  if (valid) {
    theLocalNormal = -theLocalNormal;
  }
  else {
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

    G4cerr << " RichG4OpBoundaryProcess/PostStepDoIt(): "
	   << " The Navigator reports that it returned an invalid normal"
	   << G4endl;
    G4Exception("RichG4OpBoundaryProcess::PostStepDoIt",
		"Invalid Surface Normal",
		EventMustBeAborted,
		"Geometry must return valid surface normal");
    //    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  
  theGlobalNormal = theNavigator->GetLocalToGlobalTransform().
    TransformAxis(theLocalNormal);
  
  if (OldMomentum * theGlobalNormal > 0.0) {
    theGlobalNormal = -theGlobalNormal;
  }
  
  // Get the refractive index
  G4ThreeVector position = aTrack.GetPosition();
  const G4double cvpwl=1.2398E-6;
  G4double wavelength=cvpwl/thePhotonMomentum*GeV;

  Rindex1=RichRadiatorTileManager::get_refractive_index(position[0]/cm,position[1]/cm,wavelength);
  Rindex2=1.00;
  
  // SKIP photons exiting through the lateral sides of the tiles
   if(abs(theGlobalNormal[2])<1e-6)  return G4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);
//   if(abs(theGlobalNormal[2])<1e-6)  return TOFG4OpBoundaryProcess::PostStepDoIt(aTrack,aStep);  

  DielectricDielectric();
  //  return G4OpBoundaryProcess::PostStepDoIt(aTrack, aStep); //CJD this solves the fucking problem 

  NewMomentum = NewMomentum.unit();
  NewPolarization = NewPolarization.unit();

  if(Material1->GetName()==aerogel_name && G4UniformRand()<RICHDB::scatprob){
    // Aerogel scattering effect
    double phi=2*M_PI*G4UniformRand();
    double theta=sqrt(-2*log(G4UniformRand()))*RICHDB::scatang;
    
    // Build the vector
    G4ThreeVector direction(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
    direction.rotateUz(NewMomentum);
    NewMomentum=direction.unit();
  }
  
  aParticleChange.ProposeMomentumDirection(NewMomentum);
  aParticleChange.ProposePolarization(NewPolarization);

  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}


////// Copy and paste of the G4OpBoundaryProcess with some cleaning
void RichG4OpBoundaryProcess::DielectricDielectric()
{
  G4ThreeVector theFacetNormal;

  theFacetNormal = theGlobalNormal;
  G4double PdotN = OldMomentum * theFacetNormal;
  G4double EdotN = OldPolarization * theFacetNormal;

  cost1 = - PdotN;
  if (std::abs(cost1) < 1.0){
    sint1 = std::sqrt(1.-cost1*cost1);
    sint2 = sint1*Rindex1/Rindex2;     // *** Snell's Law ***
  }
  else {
    sint1 = 0.0;
    sint2 = 0.0;
  }

  if (sint2 >= 1.0) {
    // Total reflection
    PdotN = OldMomentum * theFacetNormal;
    NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
    EdotN = OldPolarization * theFacetNormal;
    NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
  }else if (sint2 < 1.0) {
    // Calculate amplitude for transmission (Q = P x N)
    if (cost1 > 0.0) cost2 =  std::sqrt(1.-sint2*sint2); else  cost2 = -std::sqrt(1.-sint2*sint2);
    
    G4ThreeVector A_trans, A_paral, E1pp, E1pl;
    G4double E1_perp, E1_parl;
    
    if (sint1 > 0.0) {
      A_trans = OldMomentum.cross(theFacetNormal);
      A_trans = A_trans.unit();
      E1_perp = OldPolarization * A_trans;
      E1pp    = E1_perp * A_trans;
      E1pl    = OldPolarization - E1pp;
      E1_parl = E1pl.mag();
    }else {
      A_trans  = OldPolarization;
      // Here we Follow Jackson's conventions and we set the
      // parallel component = 1 in case of a ray perpendicular
      // to the surface
      E1_perp  = 0.0;
      E1_parl  = 1.0;
    }
    G4double s1 = Rindex1*cost1;
    G4double E2_perp = 2.*s1*E1_perp/(Rindex1*cost1+Rindex2*cost2);
    G4double E2_parl = 2.*s1*E1_parl/(Rindex2*cost1+Rindex1*cost2);
    G4double E2_total = E2_perp*E2_perp + E2_parl*E2_parl;
    G4double s2 = Rindex2*cost2*E2_total;
    
    G4double TransCoeff=0.0;
    if (cost1 != 0.0) TransCoeff = s2/s1;
    
    G4double E2_abs, C_parl, C_perp;
    if ( !G4BooleanRand(TransCoeff) ) {
      // Simulate reflection
      
      PdotN = OldMomentum * theFacetNormal;
      NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
      
      if (sint1 > 0.0) {   // incident ray oblique
	E2_parl   = Rindex2*E2_parl/Rindex1 - E1_parl;
	E2_perp   = E2_perp - E1_perp;
	E2_total  = E2_perp*E2_perp + E2_parl*E2_parl;
	A_paral   = NewMomentum.cross(A_trans);
	A_paral   = A_paral.unit();
	E2_abs    = std::sqrt(E2_total);
	C_parl    = E2_parl/E2_abs;
	C_perp    = E2_perp/E2_abs;
	
	NewPolarization = C_parl*A_paral + C_perp*A_trans;
	
      }else {               // incident ray perpendicular
	if (Rindex2 > Rindex1) {
	  NewPolarization = - OldPolarization;
	}
	else {
	  NewPolarization =   OldPolarization;
	}
	
      }
    }else { // photon gets transmitted
      if (sint1 > 0.0) {      // incident ray oblique
	
	G4double alpha = cost1 - cost2*(Rindex2/Rindex1);
	NewMomentum = OldMomentum + alpha*theFacetNormal;
	NewMomentum = NewMomentum.unit();
	PdotN = -cost2;
	A_paral = NewMomentum.cross(A_trans);
	A_paral = A_paral.unit();
	E2_abs     = std::sqrt(E2_total);
	C_parl     = E2_parl/E2_abs;
	C_perp     = E2_perp/E2_abs;
	
	NewPolarization = C_parl*A_paral + C_perp*A_trans;
      }else {                  // incident ray perpendicular
	NewMomentum = OldMomentum;
	NewPolarization = OldPolarization;
	
      }
    }
  }
  
  OldMomentum = NewMomentum.unit();
  OldPolarization = NewPolarization.unit();
}
