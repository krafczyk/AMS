//create by  Qi Yan //in order to fix Geant4 unified-model finish-polishedbackpainted
#ifndef _G4TOF_
#define _G4TOF_

#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4Poisson.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleMomentum.hh"
#include "G4Step.hh"
#include "G4VRestDiscreteProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4PhysicsTable.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"

#include "G4EmSaturation.hh"

class TOFG4Scintillation : public G4VRestDiscreteProcess{

public:
    TOFG4Scintillation(const G4String& processName = "Scintillation",
                                 G4ProcessType type = fElectromagnetic);   
    ~TOFG4Scintillation();
public:
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
    G4double GetMeanFreePath(const G4Track& aTrack,
                                       G4double ,
                                       G4ForceCondition* );
    G4double GetMeanLifeTime(const G4Track& aTrack,
                                 G4ForceCondition* );
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                        const G4Step&  aStep);
    G4VParticleChange* AtRestDoIt (const G4Track& aTrack,
                                       const G4Step& aStep);
    void SetTrackSecondariesFirst(const G4bool state){fTrackSecondariesFirst = state;}
    void SetScintillationYieldFactor(const G4double yieldfactor){YieldFactor = yieldfactor;}
    void SetScintillationExcitationRatio(const G4double excitationratio){ExcitationRatio = excitationratio;}

    G4PhysicsTable* GetFastIntegralTable(){ return theFastIntegralTable;}
    G4PhysicsTable* GetSlowIntegralTable(){ return theSlowIntegralTable;}
    void AddSaturation(G4EmSaturation* sat) { emSaturation = sat; }
    G4EmSaturation* GetSaturation() const { return emSaturation; }
    void DumpPhysicsTable() const;
    G4double single_exp(G4double t, G4double tau2);
    G4double bi_exp(G4double t, G4double tau1, G4double tau2);
    G4double sample_time(G4double tau1, G4double tau2);
   
protected:
     void BuildThePhysicsTable();
     G4PhysicsTable* theSlowIntegralTable;
     G4PhysicsTable* theFastIntegralTable;
     G4bool fTrackSecondariesFirst;
     G4double YieldFactor;
     G4double ExcitationRatio;
     G4EmSaturation* emSaturation;

};
inline
G4bool TOFG4Scintillation::IsApplicable(const G4ParticleDefinition& aParticleType)
{
       if (aParticleType.GetParticleName() == "opticalphoton") return false;
       if (aParticleType.IsShortLived()) return false;

       return true;
}


inline
void TOFG4Scintillation::DumpPhysicsTable() const
{
        if (theFastIntegralTable) {
           G4int PhysicsTableSize = theFastIntegralTable->entries();
           G4PhysicsOrderedFreeVector *v;

           for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
           {
                v = (G4PhysicsOrderedFreeVector*)(*theFastIntegralTable)[i];
                v->DumpValues();
           }
         }

        if (theSlowIntegralTable) {
           G4int PhysicsTableSize = theSlowIntegralTable->entries();
           G4PhysicsOrderedFreeVector *v;

           for (G4int i = 0 ; i < PhysicsTableSize ; i++ )
           {
                v = (G4PhysicsOrderedFreeVector*)(*theSlowIntegralTable)[i];
                v->DumpValues();
           }
         }
}



#include "G4OpBoundaryProcess.hh"


class TOFG4OpBoundaryProcess: public G4OpBoundaryProcess{  
public:
    TOFG4OpBoundaryProcess(const G4String& processName = "OpBoundary",
                                     G4ProcessType type = fOptical);

    ~TOFG4OpBoundaryProcess(){};

public: 
    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                       const G4Step&  aStep);
    G4OpBoundaryProcessStatus GetTOFBoundStatus() const{return theStatus;}
    G4OpBoundaryProcessStatus GetStatus (){
      return theStatus==Undefined?G4OpBoundaryProcess::GetStatus():theStatus;
   }

protected:
    G4int verboseLevel1;

private:
    G4bool G4BooleanRand(const G4double prob){return (G4UniformRand() < prob);};
    G4ThreeVector GetFacetNormal(const G4ThreeVector& Momentum,
                                 const G4ThreeVector&  Normal,G4int mode) const;
    void ChooseReflection();
    void DoReflection();
    void DoAbsorption();
    void DielectricDielectric_backpainted();
    void DielectricMetal_backpainted(); 

private:
     G4double thePhotonMomentum;
     
     G4ThreeVector OldMomentum;
     G4ThreeVector OldPolarization;
     G4ThreeVector NewMomentum;
     G4ThreeVector NewPolarization;
     G4ThreeVector FakeNewMomentum;

     G4ThreeVector theGlobalNormal;
     G4ThreeVector theFacetNormal;

     G4Material* Material1;
     G4Material* Material2;
     G4OpticalSurface* OpticalSurface;
 
     G4MaterialPropertyVector* PropertyPointer;
     G4MaterialPropertiesTable* aMaterialPropertiesTable;
     G4MaterialPropertyVector* Rindex;
     G4double Rindex1,Rindex2;
     G4double cost1, cost2, sint1, sint2;
 
     G4OpBoundaryProcessStatus theStatus;
     G4SurfaceType type;
     G4OpticalSurfaceModel theModel;
     G4OpticalSurfaceFinish theFinish;

     G4double theAbsorptivity;
     G4double theReflectivity;
     G4double theEfficiency;
     G4double prob_sl, prob_ss, prob_bs;

     G4ThreeVector A_trans, A_paral, E1pp, E1pl;
     G4double E1_perp, E1_parl,s1;

     G4double E2_perp,E2_parl,E2_total,s2,TransCoeff;
     G4double sp,sP,t_perp,sl,sL,t_parl;
     G4double E2_abs, C_parl, C_perp;      

     G4double kCarTolerance;


};


#endif
