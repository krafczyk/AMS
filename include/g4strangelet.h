#ifndef __G4STRANGELET__
#define __G4STRANGELET__
#include "globals.hh"
#include "G4ios.hh"
#include "G4Ions.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsTable.hh"
#include "G4Step.hh"
// ######################################################################
// ###                     strangelet                                 ###
// ######################################################################



class G4Strangelet : public G4Ions
{
 private:
   static G4Strangelet* theInstance;

 private: // hide constructor as private
   G4Strangelet(){}

 public:
   ~G4Strangelet(){}

   static G4Strangelet* Definition(G4double mass, G4double charge);
   static G4Strangelet* StrangeletDefinition(G4double mass, G4double charge);
   static G4Strangelet* Strangelet(G4double mass, G4double charge);
};

#include "G4VDiscreteProcess.hh"
class G4StrangeletTRDP : public G4VDiscreteProcess {
    public:
    G4StrangeletTRDP(G4String name) : G4VDiscreteProcess(name) {};
    G4double GetMeanFreePath(const G4Track& track, G4double step, G4ForceCondition *) {return DBL_MAX;}
    G4double PostStepGetPhysicalInteractionLength(const G4Track& track, G4double step, G4ForceCondition *);
    virtual G4bool IsApplicable(const G4ParticleDefinition& p) {
        return (p.GetPDGCharge() != 0 && ! p.IsShortLived());
    }
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& stepData);
};

#endif
