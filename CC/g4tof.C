//  $Id: g4tof.C,v 1.2 2012/06/24 13:53:31 qyan Exp 

// ------------------------------------------------------------
//      AMS GEANT4 TOF Process Physics 
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09  Q.Yan
//        Modified:
// -----------------------------------------------------------

#include "G4Version.hh"
#include "g4tof.h"
#include "G4ios.hh"
#include "G4OpProcessSubType.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4GeometryTolerance.hh"

#include "G4EmProcessSubType.hh"

using namespace std;
//--Scint
TOFG4Scintillation::TOFG4Scintillation(const G4String& processName,
                                       G4ProcessType type)
                  : G4VRestDiscreteProcess(processName, type)
{
        SetProcessSubType(fScintillation);
        fTrackSecondariesFirst = false;
        YieldFactor = 1.0;
        ExcitationRatio = 1.0;
        theFastIntegralTable = NULL;
        theSlowIntegralTable = NULL;
      //  verboseLevel=1;
        if (verboseLevel>0) {
           G4cout << GetProcessName() << " is created " << G4endl;
        }

        BuildThePhysicsTable();
        emSaturation = NULL;
}


TOFG4Scintillation::~TOFG4Scintillation()
{
        if (theFastIntegralTable != NULL) {
           theFastIntegralTable->clearAndDestroy();
           delete theFastIntegralTable;
        }
        if (theSlowIntegralTable != NULL) {
           theSlowIntegralTable->clearAndDestroy();
           delete theSlowIntegralTable;
        }
}

G4VParticleChange*
TOFG4Scintillation::AtRestDoIt(const G4Track& aTrack, const G4Step& aStep)
{
        return TOFG4Scintillation::PostStepDoIt(aTrack, aStep);
}

G4VParticleChange*
TOFG4Scintillation::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
#if G4VERSION_NUMBER  <945 

        aParticleChange.Initialize(aTrack);

        const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
        const G4Material* aMaterial = aTrack.GetMaterial();

        G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
        G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();

        G4ThreeVector x0 = pPreStepPoint->GetPosition();
        G4ThreeVector p0 = aStep.GetDeltaPosition().unit();
        G4double      t0 = pPreStepPoint->GetGlobalTime();

        G4double TotalEnergyDeposit = aStep.GetTotalEnergyDeposit();

        G4MaterialPropertiesTable* aMaterialPropertiesTable =
                               aMaterial->GetMaterialPropertiesTable();
        if (!aMaterialPropertiesTable)
             return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

        const G4MaterialPropertyVector* Fast_Intensity =
                aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");
        const G4MaterialPropertyVector* Slow_Intensity =
                aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");

       if (!Fast_Intensity && !Slow_Intensity )
             return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);

        G4int nscnt = 1;
        if (Fast_Intensity && Slow_Intensity) nscnt = 2;

        G4double ScintillationYield = aMaterialPropertiesTable->
                                      GetConstProperty("SCINTILLATIONYIELD");
        ScintillationYield *= YieldFactor;

        G4double ResolutionScale    = aMaterialPropertiesTable->
                                      GetConstProperty("RESOLUTIONSCALE");

        // Birks law saturation:

        G4double constBirks = 0.0;
        constBirks = aMaterial->GetIonisation()->GetBirksConstant();

        G4double MeanNumberOfPhotons;

        if (emSaturation) {
           MeanNumberOfPhotons = ScintillationYield*
                              (emSaturation->VisibleEnergyDeposition(&aStep));
        } else {
           MeanNumberOfPhotons = ScintillationYield*TotalEnergyDeposit;
        }

        G4int NumPhotons;

        if (MeanNumberOfPhotons > 10.)
        {
          G4double sigma = ResolutionScale * sqrt(MeanNumberOfPhotons);
          NumPhotons = G4int(G4RandGauss::shoot(MeanNumberOfPhotons,sigma)+0.5);
        }
        else
        {
          NumPhotons = G4int(G4Poisson(MeanNumberOfPhotons));
        }

        if (NumPhotons <= 0)
        {
           // return unchanged particle and no secondaries 
           aParticleChange.SetNumberOfSecondaries(0);
           return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
        }

        aParticleChange.SetNumberOfSecondaries(NumPhotons);

        if (fTrackSecondariesFirst) {
           if (aTrack.GetTrackStatus() == fAlive )
                   aParticleChange.ProposeTrackStatus(fSuspend);
        }

        G4int materialIndex = aMaterial->GetIndex();
        G4int Num = NumPhotons;

        for (G4int scnt = 1; scnt <= nscnt; scnt++) {

            G4double ScintillationTime = 0.*ns;
            G4double ScintillationRiseTime = 0.*ns;
            G4PhysicsOrderedFreeVector* ScintillationIntegral = NULL;

            if (scnt == 1) {
               if (nscnt == 1) {
                 if(Fast_Intensity){
                   ScintillationTime   = aMaterialPropertiesTable->
                                           GetConstProperty("FASTTIMECONSTANT");
                   ScintillationRiseTime = aMaterialPropertiesTable->
                                  GetConstProperty("FASTSCINTILLATIONRISETIME");

                   ScintillationIntegral =
                   (G4PhysicsOrderedFreeVector*)((*theFastIntegralTable)(materialIndex));
                 }
                 if(Slow_Intensity){
                   ScintillationTime   = aMaterialPropertiesTable->
                                           GetConstProperty("SLOWTIMECONSTANT");
                   ScintillationRiseTime = aMaterialPropertiesTable->
                                  GetConstProperty("SLOWSCINTILLATIONRISETIME");
                   ScintillationIntegral =
                   (G4PhysicsOrderedFreeVector*)((*theSlowIntegralTable)(materialIndex));
                 }
               }

        else {
                 G4double YieldRatio = aMaterialPropertiesTable->
                                          GetConstProperty("YIELDRATIO");
                 if ( ExcitationRatio == 1.0 ) {
                    Num = G4int (min(YieldRatio,1.0) * NumPhotons);
                 }
                 else {
                    Num = G4int (min(ExcitationRatio,1.0) * NumPhotons);
                 }
                 ScintillationTime   = aMaterialPropertiesTable->
                                          GetConstProperty("FASTTIMECONSTANT");
                 ScintillationRiseTime = aMaterialPropertiesTable->
                                 GetConstProperty("FASTSCINTILLATIONRISETIME");

                 ScintillationIntegral =
                  (G4PhysicsOrderedFreeVector*)((*theFastIntegralTable)(materialIndex));
               }
            }
            else {
               Num = NumPhotons - Num;
               ScintillationTime   =   aMaterialPropertiesTable->
                                          GetConstProperty("SLOWTIMECONSTANT");
               ScintillationRiseTime = aMaterialPropertiesTable->
                                 GetConstProperty("SLOWSCINTILLATIONRISETIME");
               ScintillationIntegral =
                  (G4PhysicsOrderedFreeVector*)((*theSlowIntegralTable)(materialIndex));
            }

            if (!ScintillationIntegral) continue;

            G4double CIImax = ScintillationIntegral->GetMaxValue();
            for (G4int i = 0; i < Num; i++) {
                G4double CIIvalue = G4UniformRand()*CIImax;
                G4double sampledEnergy =
                              ScintillationIntegral->GetEnergy(CIIvalue);

                if (verboseLevel>1) {
                   G4cout << "sampledEnergy = " << sampledEnergy << G4endl;
                   G4cout << "CIIvalue =        " << CIIvalue << G4endl;
                }
                G4double cost = 1. - 2.*G4UniformRand();
                G4double sint = sqrt((1.-cost)*(1.+cost));

                G4double phi = twopi*G4UniformRand();
                G4double sinp = sin(phi);
                G4double cosp = cos(phi);

                G4double px = sint*cosp;
                G4double py = sint*sinp;
                G4double pz = cost;
                G4ParticleMomentum photonMomentum(px, py, pz);
//--pola
                G4double sx = cost*cosp;
                G4double sy = cost*sinp;
                G4double sz = -sint;
                G4ThreeVector photonPolarization(sx, sy, sz);

                G4ThreeVector perp = photonMomentum.cross(photonPolarization);

                phi = twopi*G4UniformRand();
                sinp = sin(phi);
                cosp = cos(phi);
                photonPolarization = cosp * photonPolarization + sinp * perp;

                photonPolarization = photonPolarization.unit();
                // Generate a new photon:

                G4DynamicParticle* aScintillationPhoton =
                  new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(),
                                                         photonMomentum);
                aScintillationPhoton->SetPolarization
                                     (photonPolarization.x(),
                                      photonPolarization.y(),
                                      photonPolarization.z());

                aScintillationPhoton->SetKineticEnergy(sampledEnergy);
                   // Generate new G4Track object:

                G4double rand;

                if (aParticle->GetDefinition()->GetPDGCharge() != 0) {
                   rand = G4UniformRand();
                } else {
                   rand = 1.0;
                }

                G4double delta = rand * aStep.GetStepLength();
                G4double deltaTime = delta /
                       ((pPreStepPoint->GetVelocity()+pPostStepPoint->GetVelocity())/2.);
               deltaTime = deltaTime +
                          sample_time(ScintillationRiseTime, ScintillationTime);

                G4double aSecondaryTime = t0 + deltaTime;

                G4ThreeVector aSecondaryPosition =
                                    x0 + rand * aStep.GetDeltaPosition();

                G4Track* aSecondaryTrack =
                new G4Track(aScintillationPhoton,aSecondaryTime,aSecondaryPosition);

                aSecondaryTrack->SetTouchableHandle(
                                 aStep.GetPreStepPoint()->GetTouchableHandle());

                aSecondaryTrack->SetParentID(aTrack.GetTrackID());
                aParticleChange.AddSecondary(aSecondaryTrack);
            }
        }
        if (verboseLevel>0) {
        G4cout << "\n Exiting from TOFG4Scintillation::DoIt -- NumberOfSecondaries = "
             << aParticleChange.GetNumberOfSecondaries() << G4endl;
        }
#else
cerr<<"TOFG4Scintillation::PostStepDoIt-F-NotYetSupportedInGeant>9.4.4 "<<endl;
abort();
#endif
        return G4VRestDiscreteProcess::PostStepDoIt(aTrack, aStep);
}

void TOFG4Scintillation::BuildThePhysicsTable()
{
        if (theFastIntegralTable && theSlowIntegralTable) return;

#if G4VERSION_NUMBER  <945 
        const G4MaterialTable* theMaterialTable =
                               G4Material::GetMaterialTable();
        G4int numOfMaterials = G4Material::GetNumberOfMaterials();

        // create new physics table

        if(!theFastIntegralTable)theFastIntegralTable = new G4PhysicsTable(numOfMaterials);
        if(!theSlowIntegralTable)theSlowIntegralTable = new G4PhysicsTable(numOfMaterials);

        // loop for materials

      for (G4int i=0 ; i < numOfMaterials; i++)
          {
                G4PhysicsOrderedFreeVector* aPhysicsOrderedFreeVector =
                                        new G4PhysicsOrderedFreeVector();
                G4PhysicsOrderedFreeVector* bPhysicsOrderedFreeVector =
                                        new G4PhysicsOrderedFreeVector();

                // Retrieve vector of scintillation wavelength intensity for
                // the material from the material's optical properties table.

                G4Material* aMaterial = (*theMaterialTable)[i];

                G4MaterialPropertiesTable* aMaterialPropertiesTable =
                                aMaterial->GetMaterialPropertiesTable();

                if (aMaterialPropertiesTable) {

                   G4MaterialPropertyVector* theFastLightVector =
                   aMaterialPropertiesTable->GetProperty("FASTCOMPONENT");

                   if (theFastLightVector) {
                      // Retrieve the first intensity point in vector
                      // of (photon energy, intensity) pairs 

                      theFastLightVector->ResetIterator();
                      ++(*theFastLightVector);  // advance to 1st entry 

                      G4double currentIN = theFastLightVector->GetProperty();

                      if (currentIN >= 0.0) {

                         // Create first (photon energy, Scintillation 
                         // Integral pair  

                         G4double currentPM = theFastLightVector-> GetPhotonEnergy();

                         G4double currentCII = 0.0;

                         aPhysicsOrderedFreeVector->InsertValues(currentPM , currentCII);

                         // Set previous values to current ones prior to loop

                         G4double prevPM  = currentPM;
                         G4double prevCII = currentCII;
                         G4double prevIN  = currentIN;

                         // loop over all (photon energy, intensity)
                         // pairs stored for this material  

                        while(++(*theFastLightVector))
                         {
                                currentPM = theFastLightVector->GetPhotonEnergy();

                                currentIN=theFastLightVector->GetProperty();

                                currentCII = 0.5 * (prevIN + currentIN);

                                currentCII = prevCII +(currentPM - prevPM) * currentCII;

                                aPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                                prevPM  = currentPM;
                                prevCII = currentCII;
                                prevIN  = currentIN;
                         }
                      }
                   }

                   G4MaterialPropertyVector* theSlowLightVector =
                   aMaterialPropertiesTable->GetProperty("SLOWCOMPONENT");
                    if (theSlowLightVector) {

                      // Retrieve the first intensity point in vector
                      // of (photon energy, intensity) pairs

                      theSlowLightVector->ResetIterator();
                      ++(*theSlowLightVector);  // advance to 1st entry

                      G4double currentIN = theSlowLightVector->
                                           GetProperty();

                      if (currentIN >= 0.0) {

                         // Create first (photon energy, Scintillation
                         // Integral pair

                         G4double currentPM = theSlowLightVector->GetPhotonEnergy();

                         G4double currentCII = 0.0;

                         bPhysicsOrderedFreeVector->InsertValues(currentPM , currentCII);

                         // Set previous values to current ones prior to loop

                         G4double prevPM  = currentPM;
                         G4double prevCII = currentCII;
                         G4double prevIN  = currentIN;
                         // loop over all (photon energy, intensity)
                         // pairs stored for this material

                         while(++(*theSlowLightVector))
                         {
                                currentPM = theSlowLightVector->GetPhotonEnergy();

                                currentIN=theSlowLightVector->GetProperty();

                                currentCII = 0.5 * (prevIN + currentIN);

                                currentCII = prevCII +(currentPM - prevPM) * currentCII;

                                bPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

                                prevPM  = currentPM;
                                prevCII = currentCII;
                                prevIN  = currentIN;
                         }
                      }
                   }
                }

          // The scintillation integral(s) for a given material
         // will be inserted in the table(s) according to the
        // position of the material in the material table.
        theFastIntegralTable->insertAt(i,aPhysicsOrderedFreeVector);
        theSlowIntegralTable->insertAt(i,bPhysicsOrderedFreeVector);
        }
#else
cerr<<"TOFG4Scintillation::BuildThePhysicsTable-S-NotYetSupportedInGeant>9.4.4 "<<endl;
#endif
}

// GetMeanFreePath
// ---------------
G4double TOFG4Scintillation::GetMeanFreePath(const G4Track&,G4double ,
                                          G4ForceCondition* condition)
{
        *condition = StronglyForced;
        return DBL_MAX;
}

// GetMeanLifeTime
// ---------------
G4double TOFG4Scintillation::GetMeanLifeTime(const G4Track&,
                                          G4ForceCondition* condition)
{
        *condition = Forced;
        return DBL_MAX;
}


G4double TOFG4Scintillation::sample_time(G4double tau1, G4double tau2)
{
// tau1: rise time and tau2: decay time
        while(1) {
          // two random numbers
          G4double ran1 = G4UniformRand();
          G4double ran2 = G4UniformRand();
          //
          // exponential distribution as envelope function: very efficient
          //
          G4double d = (tau1+tau2)/tau2;
          // make sure the envelope function is 
          // always larger than the bi-exponential
          G4double t = -1.0*tau2*std::log(1-ran1);
          G4double g = d*single_exp(t,tau2);
          if (ran2 <= bi_exp(t,tau1,tau2)/g) return t;
        }
        return -1.0;
}


G4double TOFG4Scintillation::bi_exp(G4double t, G4double tau1, G4double tau2)
{
         return std::exp(-1.0*t/tau2)*(1-std::exp(-1.0*t/tau1))/tau2/tau2*(tau1+tau2);
}

G4double TOFG4Scintillation::single_exp(G4double t, G4double tau2)
{
         return std::exp(-1.0*t/tau2)/tau2;
}

//---Boundary
TOFG4OpBoundaryProcess::TOFG4OpBoundaryProcess(const G4String& processName,
                                               G4ProcessType type)
             : G4OpBoundaryProcess(processName, type)
{
  theStatus= Undefined;
  theModel = glisur;
  theFinish = polished;

  kCarTolerance = G4GeometryTolerance::GetInstance()
                        ->GetSurfaceTolerance();
  verboseLevel1=0;
 
}


G4VParticleChange*
TOFG4OpBoundaryProcess::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep)
{
#if G4VERSION_NUMBER  <945 
  aParticleChange.Initialize(aTrack);
  theStatus= Undefined;
 
  G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
  G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();
 
  if (pPostStepPoint->GetStepStatus() != fGeomBoundary){
          theStatus = NotAtBoundary;
	  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }
  if (aTrack.GetStepLength()<=kCarTolerance/2){
          theStatus = StepTooSmall;
	  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  Material1 = pPreStepPoint  -> GetMaterial();
  Material2 = pPostStepPoint -> GetMaterial();

//----- 
  const G4DynamicParticle* aParticle = aTrack.GetDynamicParticle();
  thePhotonMomentum = aParticle->GetTotalMomentum();
  OldMomentum       = aParticle->GetMomentumDirection();
  OldPolarization   = aParticle->GetPolarization();
  
  aMaterialPropertiesTable = Material1->GetMaterialPropertiesTable();
  if (aMaterialPropertiesTable) {
      Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
  }
   if (Rindex) {
      Rindex1 = Rindex->GetProperty(thePhotonMomentum);
   }
   else {
       theStatus = NoRINDEX;
       aParticleChange.ProposeTrackStatus(fStopAndKill);
       return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }

    //avoid rich, rich has own process vaccum also belong to rich
 /* if((Material1->GetName()==aerogel_name)||(Material1->GetName()==naf_name)){
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
   }
*/
//--get surface
  G4LogicalSurface* Surface = NULL;
  Surface = G4LogicalBorderSurface::GetSurface
	  (pPreStepPoint ->GetPhysicalVolume(),pPostStepPoint->GetPhysicalVolume());
  if (Surface == NULL){
     G4bool enteredDaughter=(pPostStepPoint->GetPhysicalVolume()->GetMotherLogical() 
                          ==pPreStepPoint->GetPhysicalVolume()->GetLogicalVolume());
     if(enteredDaughter){
         Surface = G4LogicalSkinSurface::GetSurface
              (pPostStepPoint->GetPhysicalVolume()->GetLogicalVolume());
         if(Surface == NULL){
            Surface = G4LogicalSkinSurface::GetSurface(pPreStepPoint->GetPhysicalVolume()->GetLogicalVolume());
          }
     }
    else {
        Surface = G4LogicalSkinSurface::GetSurface
              (pPreStepPoint->GetPhysicalVolume()->GetLogicalVolume());
       if(Surface == NULL){
           Surface = G4LogicalSkinSurface::GetSurface(pPostStepPoint->GetPhysicalVolume()->GetLogicalVolume());
       }
    } 
  }
  if(!Surface){
      return G4OpBoundaryProcess::PostStepDoIt(aTrack, aStep);
    }
   
   OpticalSurface =dynamic_cast <G4OpticalSurface*> (Surface->GetSurfaceProperty());
   type      = OpticalSurface->GetType();
   theModel  = OpticalSurface->GetModel();
   theFinish = OpticalSurface->GetFinish();
   aMaterialPropertiesTable = OpticalSurface->GetMaterialPropertiesTable();

   if(theFinish!= polishedbackpainted|| theModel!=unified || ! aMaterialPropertiesTable){
      return G4OpBoundaryProcess::PostStepDoIt(aTrack, aStep);
   }
   

//--property table
   Rindex = aMaterialPropertiesTable->GetProperty("RINDEX");
   if(Rindex ){
      Rindex2 = Rindex->GetProperty(thePhotonMomentum);
   }
   else  {
       theStatus = NoRINDEX;
       aParticleChange.ProposeTrackStatus(fStopAndKill);
       return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep); 
    }
 
//raw polishedbackpainted 
    PropertyPointer = aMaterialPropertiesTable->GetProperty("ABSORPTIVITY");
    if(PropertyPointer){
        theAbsorptivity =PropertyPointer->GetProperty(thePhotonMomentum);
    }
    else {
         return G4OpBoundaryProcess::PostStepDoIt(aTrack, aStep);
    }

 
    PropertyPointer = aMaterialPropertiesTable->GetProperty("REFLECTIVITY");
    if(PropertyPointer){
        theReflectivity =PropertyPointer->GetProperty(thePhotonMomentum);
    }
    else { 
      theReflectivity=0.;
    }

    PropertyPointer=aMaterialPropertiesTable->GetProperty("EFFICIENCY");
    if (PropertyPointer) {
	    theEfficiency=PropertyPointer->GetProperty(thePhotonMomentum);
    }
    else {
       theEfficiency = 0.;
    }

    PropertyPointer=aMaterialPropertiesTable->GetProperty("SPECULARLOBECONSTANT");
    if (PropertyPointer) {
	prob_sl=PropertyPointer->GetProperty(thePhotonMomentum);
    } else {
        prob_sl = 0.0;
    }

    PropertyPointer=aMaterialPropertiesTable->GetProperty("SPECULARSPIKECONSTANT");
    if (PropertyPointer) {
	prob_ss=PropertyPointer->GetProperty(thePhotonMomentum);
    } else {
	prob_ss = 0.0;
    }
    
    PropertyPointer=aMaterialPropertiesTable->GetProperty("BACKSCATTERCONSTANT");
    if (PropertyPointer) {
	    prob_bs=PropertyPointer->GetProperty(thePhotonMomentum);
    } else {
	 prob_bs = 0.0;
    }

    G4Navigator* theNavigator =
        G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4ThreeVector theLocalNormal;   // Normal points back into volume
    G4bool valid;
    theLocalNormal = theNavigator->GetLocalExitNormal(&valid);
    if (valid) {
       theLocalNormal = -theLocalNormal;
     }
    else {
      G4cerr << " G4OpBoundaryProcess/PostStepDoIt(): "
                 << " The Navigator reports that it returned an invalid normal"
                 << G4endl;
      G4Exception("G4OpBoundaryProcess::PostStepDoIt",
                      "Invalid Surface Normal",
                    EventMustBeAborted,
                   "Geometry must return valid surface normal");
      aParticleChange.ProposeTrackStatus(fStopAndKill);
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
    theGlobalNormal = theNavigator->GetLocalToGlobalTransform().
                                        TransformAxis(theLocalNormal);

//--process
//--absorb by dielectric face
    if(type== dielectric_dielectric)DielectricDielectric_backpainted();
    else                            DielectricMetal_backpainted();

//---new
    NewMomentum = NewMomentum.unit();
    NewPolarization = NewPolarization.unit();
    aParticleChange.ProposeMomentumDirection(NewMomentum);
    aParticleChange.ProposePolarization(NewPolarization);
#else
cerr<<"TOFG4OpBoundaryProcess::PostStepDoIt-F-NotYetSupportedInGeant>9.4.4 "<<endl;
abort();
#endif

    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}



void TOFG4OpBoundaryProcess::DielectricDielectric_backpainted()
{
    G4bool Inside = false;
    G4bool Pkill = false;
    insleap:

    theStatus=Undefined;
//---touch surface
     if(G4BooleanRand(theAbsorptivity)){
        DoAbsorption();
        Pkill=1;
        return;
      }
    theFacetNormal=GetFacetNormal(OldMomentum,theGlobalNormal,0);
    G4double PdotN = OldMomentum * theFacetNormal;
    G4double EdotN = OldPolarization * theFacetNormal;

//--
    cost1 = - PdotN;
    if (std::abs(cost1) < 1.0-kCarTolerance){
	    sint1 = std::sqrt(1.-cost1*cost1);
	    sint2 = sint1*Rindex1/Rindex2;     // *** Snell's Law ***
    }
    else {
	    sint1 = 0.0;
	    sint2 = 0.0;
    }
   
   if (sint2 >= 1.0) {
         
        ChooseReflection();
        if(theStatus==LambertianReflection||theStatus == BackScattering)DoReflection();  
        else {
          PdotN = OldMomentum * theFacetNormal;
          NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
          EdotN = OldPolarization * theFacetNormal;
          NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
       }
       if(NewMomentum*theGlobalNormal < 0.0){//still at surface go to surface again
          OldMomentum = NewMomentum.unit();
          OldPolarization = NewPolarization.unit();
          goto insleap;//reflect must >=0
       }
       theStatus = TotalInternalReflection;
       if( verboseLevel1 > 0)G4cout<<" TOF Total Reflection"<<" sint2="<<sint2<<G4endl;
    }


   else if (sint2 < 1.0){
        if (cost1 > 0.0) {
            cost2 =  std::sqrt(1.-sint2*sint2);
	}
	else {
            cost2 = -std::sqrt(1.-sint2*sint2);
	}
//---
	if (sint1 > 0.0) {
		A_trans = OldMomentum.cross(theFacetNormal);
		A_trans = A_trans.unit();
		E1_perp = OldPolarization * A_trans;
		E1pp    = E1_perp * A_trans;
		E1pl    = OldPolarization - E1pp;
		E1_parl = E1pl.mag();
	}
	else {
		A_trans  = OldPolarization;
		E1_perp  = 0.0;
		E1_parl  = 1.0;
	}
	s1 = Rindex1*cost1;
//--    
	E2_perp = 2.*s1*E1_perp/(Rindex1*cost1+Rindex2*cost2);
	E2_parl = 2.*s1*E1_parl/(Rindex2*cost1+Rindex1*cost2);
	E2_total   = E2_perp*E2_perp + E2_parl*E2_parl;
	s2      = Rindex2*cost2*E2_total;
        TransCoeff = s2/s1;
        if( cost1 == 0.)TransCoeff = 0.0;
//---
        if ( !G4BooleanRand(TransCoeff) ) {
          // Simulate reflection
            ChooseReflection();
            if(theStatus==LambertianReflection||theStatus == BackScattering)DoReflection();
            else{ 
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
	      }
              else {               // incident ray perpendicular
                 if (Rindex2 > Rindex1) {
                     NewPolarization = - OldPolarization;
                  }
                  else {
                     NewPolarization =   OldPolarization;
                  }
                }
             }
            if(NewMomentum*theGlobalNormal < 0.0){//still at surface region
               OldMomentum = NewMomentum.unit();
               OldPolarization = NewPolarization.unit();
               goto insleap;
            }
            theStatus = FresnelReflection;
            if( verboseLevel1 > 0){ G4cout<<" TOF Fresnel Reflection"<<" sint2="<<sint2<<" TransCoeff="<<TransCoeff<<G4endl; }
         }//end reflection
        else { // photon gets transmitted
               if (sint1 > 0.0) {   // incident ray oblique
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
                     
                  }
                else {
                    
                    NewMomentum = OldMomentum;
                    NewPolarization = OldPolarization;
                }
              Inside = !Inside;
              if(NewMomentum*theGlobalNormal > 0.0){//transmit to surface again
                 OldMomentum = NewMomentum.unit();
                 theGlobalNormal = -theGlobalNormal;
                 OldPolarization = NewPolarization.unit();
                 G4SwapObj(&Rindex1,&Rindex2);
                 goto insleap;
              }
              else {
               theGlobalNormal = -theGlobalNormal;
              }
              theStatus = FresnelRefraction;
           }//end transmit 
       }//end refle+tranmit

       
        if(Inside && !Pkill){
              OldMomentum = NewMomentum.unit();
              OldPolarization = NewPolarization.unit();
              if( !G4BooleanRand(theReflectivity) ) {//include reflection sc-surface absorption
                DoAbsorption();
                if( verboseLevel1 > 0){ G4cout<<" TOF Al Surface Absorption"<<G4endl; }
              }
              else {
               theStatus=SpikeReflection;
               theGlobalNormal = -theGlobalNormal;
               DoReflection();
                if( verboseLevel1 > 0){ G4cout<<" TOF Al Surface Reflection"<<G4endl; }
               theGlobalNormal = -theGlobalNormal;
               OldMomentum = NewMomentum.unit(); 
               OldPolarization = NewPolarization.unit();              
               G4SwapObj(&Rindex1,&Rindex2);
               goto insleap;
             }
         }
 
}


void TOFG4OpBoundaryProcess::DielectricMetal_backpainted()
{

   G4bool Inside = false;
    G4bool Pkill = false;
    insleap:

    theStatus=Undefined;
    G4double PdotN;
    do {
         theFacetNormal=GetFacetNormal(OldMomentum,theGlobalNormal,0);
         PdotN = OldMomentum * theFacetNormal;
         FakeNewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
       } while (FakeNewMomentum*theGlobalNormal<0.);
    G4double EdotN = OldPolarization * theFacetNormal;

//--
    cost1 = - PdotN;
    if (std::abs(cost1) < 1.0-kCarTolerance){
            sint1 = std::sqrt(1.-cost1*cost1);
            sint2 = sint1*Rindex1/Rindex2;     // *** Snell's Law ***
    }
    else {
            sint1 = 0.0;
            sint2 = 0.0;
    }

   if (sint2 >= 1.0) {
      if(Inside){
         DoAbsorption(); Pkill=1;
         if( verboseLevel1 > 0)G4cout<<" TOF Inside Total Reflection"<<" sint2="<<sint2<<G4endl;
       }
      else {
        theStatus = TotalInternalReflection;
        if( verboseLevel1 > 0)G4cout<<" TOF Total Reflection"<<" sint2="<<sint2<<G4endl;
        ChooseReflection();
        if(theStatus==LambertianReflection||theStatus == BackScattering)DoReflection();
        else {
          PdotN = OldMomentum * theFacetNormal;
          NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
          EdotN = OldPolarization * theFacetNormal;
          NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
        }
      }
    }


   else if (sint2 < 1.0){
        if (cost1 > 0.0) {
            cost2 =  std::sqrt(1.-sint2*sint2);
        }
        else {
            cost2 = -std::sqrt(1.-sint2*sint2);
        }
//---
        if (sint1 > 0.0) {
                A_trans = OldMomentum.cross(theFacetNormal);
                A_trans = A_trans.unit();
                E1_perp = OldPolarization * A_trans;
                E1pp    = E1_perp * A_trans;
                E1pl    = OldPolarization - E1pp;
                E1_parl = E1pl.mag();
        }
 
       else {
                A_trans  = OldPolarization;
                E1_perp  = 0.0;
                E1_parl  = 1.0;
        }
        s1 = Rindex1*cost1;
//--    

        if(!Inside){
           E2_perp = 2.*s1*E1_perp/(Rindex1*cost1+Rindex2*cost2);
           E2_parl = 2.*s1*E1_parl/(Rindex2*cost1+Rindex1*cost2);
           E2_total   = E2_perp*E2_perp + E2_parl*E2_parl;
           s2      = Rindex2*cost2*E2_total;
           TransCoeff = s2/s1;
       }
       else {
          sp = 2.*s1/(Rindex1*cost1+Rindex2*cost2);
          sP = sp*sp*Rindex2*cost2/s1;
          t_perp= sP/(1.-(1.-sP)*theReflectivity*(1.-theAbsorptivity));

          sl = 2.*s1/(Rindex2*cost1+Rindex1*cost2);
          sL = sl*sl*Rindex2*cost2/s1;
          t_parl = sL/(1.-(1.-sL)*theReflectivity*(1.-theAbsorptivity));

          E2_perp = E1_perp*sqrt(t_perp);
          E2_parl = E1_parl*sqrt(t_parl);
          E2_total= E2_perp*E2_perp + E2_parl*E2_parl;
          TransCoeff = E2_total;
       }
       if( cost1 == 0.)TransCoeff = 0.0;

      //---

        if ( !G4BooleanRand(TransCoeff) ) {
          if(Inside){
             DoAbsorption(); Pkill=1;
             if( verboseLevel1 > 0)G4cout<<" TOF Inside Absorption"<<" sint2="<<sint2<<" TransCoeff="<<TransCoeff<<" "
                                         <<" perp refle="<<t_perp<<" parl refle="<<t_parl<<" TsP="<<sP<<" TsL="<<sL<<" Ref="<<theReflectivity
                                        <<" p+l="<<(E1_perp* E1_perp+E1_parl*E1_parl)<<G4endl;
           }
          else {
          // Simulate reflection
            theStatus = FresnelReflection;
            if( verboseLevel1 > 0){ G4cout<<" TOF Fresnel Reflection"<<" sint2="<<sint2<<" TransCoeff="<<TransCoeff<<G4endl; }

            ChooseReflection();
            if(theStatus==LambertianReflection||theStatus == BackScattering)DoReflection();
            else{
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
              }
               else {               // incident ray perpendicular
                   if (Rindex2 > Rindex1) {
                        NewPolarization = - OldPolarization;
                    }
                    else {
                        NewPolarization =   OldPolarization;
                    }
                }
             }
          }//end reflection
        }//end ! transmit 
        else { // photon gets transmitted

               if (sint1 > 0.0) {   // incident ray oblique

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

                  }
                else {

                    NewMomentum = OldMomentum;
                    NewPolarization = OldPolarization;
                }
              if(NewMomentum*theGlobalNormal>0.)goto insleap;
              if(Inside&&verboseLevel1 > 0)G4cout<<" TOF Inside Reflection"<<" sint2="<<sint2<<" TransCoeff="<<TransCoeff<<G4endl;
              Inside = !Inside;
              theStatus = FresnelRefraction;
              if( verboseLevel1 > 0){ G4cout<<" TOF Gap Transmit"<<" sint2="<<sint2<<" TransCoeff="<<TransCoeff<<G4endl; }

           }//end transmit 
       }//end refle+tranmit


        if(Inside && !Pkill){
              OldMomentum = NewMomentum.unit();
              OldPolarization = NewPolarization.unit();
              if( !G4BooleanRand(theReflectivity*(1.-theAbsorptivity)) ) {
                DoAbsorption();
                if( verboseLevel1 > 0){ G4cout<<" TOF Al Surface Absorption"<<G4endl; }
              }
              else {
               theStatus=SpikeReflection;
               DoReflection();
                if( verboseLevel1 > 0){ G4cout<<" TOF Al Surface Reflection"<<G4endl; }

               theGlobalNormal = -theGlobalNormal;
               OldMomentum = NewMomentum.unit();
               OldPolarization = NewPolarization.unit();

               G4SwapObj(&Rindex1,&Rindex2);
               goto insleap;
             }
        }

}


G4ThreeVector
TOFG4OpBoundaryProcess::GetFacetNormal(const G4ThreeVector& Momentum,
                                    const G4ThreeVector&  Normal,G4int mode ) const
{
  G4ThreeVector FacetNormal;
 
 if(mode==0){
     G4double alpha;
     G4double sigma_alpha = OpticalSurface->GetSigmaAlpha();
     if(sigma_alpha>0){ 
       G4double f_max = std::min(1.0,4.*sigma_alpha);
      
      do {
        do {
            alpha = G4RandGauss::shoot(0.0,sigma_alpha);
	  } while (G4UniformRand()*f_max > std::sin(alpha) || alpha >= halfpi );

	G4double phi = G4UniformRand()*twopi;

	G4double SinAlpha = std::sin(alpha);
	G4double CosAlpha = std::cos(alpha);
	G4double SinPhi = std::sin(phi);
	G4double CosPhi = std::cos(phi);

	G4double unit_x = SinAlpha * CosPhi;
	G4double unit_y = SinAlpha * SinPhi;
	G4double unit_z = CosAlpha;

	FacetNormal.setX(unit_x);
	FacetNormal.setY(unit_y);
	FacetNormal.setZ(unit_z);

	G4ThreeVector tmpNormal = Normal;

	FacetNormal.rotateUz(tmpNormal);
      } while (Momentum * FacetNormal >= 0.0);
     }
   else {
     FacetNormal=Normal;
    }
  
  }
  else {
        G4double  polish = 1.0;
         if (OpticalSurface) polish = OpticalSurface->GetPolish();

         if (polish < 1.0) {
              do {
                 G4ThreeVector smear;
                 do {
                    smear.setX(2.*G4UniformRand()-1.0);
                    smear.setY(2.*G4UniformRand()-1.0);
                    smear.setZ(2.*G4UniformRand()-1.0);
                 } while (smear.mag()>1.0);
                 smear = (1.-polish) * smear;
                 FacetNormal = Normal + smear;
              } while (Momentum * FacetNormal >= 0.0);
              FacetNormal = FacetNormal.unit();
           }
           else {
              FacetNormal = Normal;
           }

   }
   return FacetNormal;

}


inline
void TOFG4OpBoundaryProcess::ChooseReflection()
{
                 G4double rand = G4UniformRand();
                 if ( rand >= 0.0 && rand < prob_ss ) {
                    theStatus = SpikeReflection;
                    theFacetNormal = theGlobalNormal;
                 }
                 else if ( rand >= prob_ss &&
                           rand <= prob_ss+prob_sl) {
                    theStatus = LobeReflection;
                 }
                 else if ( rand > prob_ss+prob_sl &&
                           rand < prob_ss+prob_sl+prob_bs ) {
                    theStatus = BackScattering;
                 }
                 else {
                    theStatus = LambertianReflection;
                 }
}


inline
void TOFG4OpBoundaryProcess::DoReflection(){
   if (theStatus == LambertianReflection ) {
        NewMomentum = G4LambertianRand(theGlobalNormal);
        theFacetNormal = (NewMomentum - OldMomentum).unit();
        G4double EdotN = OldPolarization * theFacetNormal;
        NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;
    }
    else if  ( theStatus == BackScattering ) { 
	  NewMomentum = -OldMomentum;
	  NewPolarization = -OldPolarization;
     }
    else {//metal reflection
        G4double PdotN; 
        do {
            theFacetNormal=GetFacetNormal(OldMomentum,theGlobalNormal,1);
            PdotN = OldMomentum * theFacetNormal;
            NewMomentum = OldMomentum - (2.*PdotN)*theFacetNormal;
         } while (NewMomentum * theGlobalNormal < 0.0);
         G4double EdotN = OldPolarization * theFacetNormal;
         NewPolarization = -OldPolarization + (2.*EdotN)*theFacetNormal;   
    } 
    
}

inline
void TOFG4OpBoundaryProcess::DoAbsorption(){
              theStatus = Absorption;
              if ( G4BooleanRand(theEfficiency) ) {

                 // EnergyDeposited =/= 0 means: photon has been detected
                 theStatus = Detection;
                 aParticleChange.ProposeLocalEnergyDeposit(thePhotonMomentum);
              }
              else {
                 aParticleChange.ProposeLocalEnergyDeposit(0.0);
              }

              NewMomentum = OldMomentum;
              NewPolarization = OldPolarization;
//              aParticleChange.ProposeEnergy(0.0);
              aParticleChange.ProposeTrackStatus(fStopAndKill);
}

