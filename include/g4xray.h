//  $Id: g4xray.h,v 1.4 2002/09/20 09:30:41 choutko Exp $
#ifndef __G4XRAY__
#define __G4XRAY__
#include "globals.hh"
#include "G4ios.hh"
#include "G4VBoson.hh"
#include "G4Step.hh"
// ######################################################################
// ###                           xray                                 ###
// ######################################################################

class G4XRay : public G4VBoson
{
 private:
   static G4XRay theXRay;
   static G4double  theXRayLengthCut;
   static G4double* theXRayKineticEnergyCuts;
  
   //-------- the followings are used privately in G4XRay ----
 private:
    typedef G4LossTable G4CrossSectionTable;
    void BuildAbsorptionLengthVector(
                              const G4Material* aMaterial,
                              const G4CrossSectionTable* aCrossSectionTable,
                              G4double       maxEnergy,
                              G4double       aMass,
                              G4RangeVector* absorptionLengthVector
                             );
    G4double ComputeCrossSection(
                              G4double AtomicNumber,
                              G4double KineticEnergy
                             ) const;

 //--------------for SetCuts----------------------------------------------
 protected:
    G4double ComputeLoss(G4double AtomicNumber,G4double KineticEnergy) const
    {
      return ComputeCrossSection(AtomicNumber,KineticEnergy);
    };

    void BuildRangeVector(
                                  const G4Material* aMaterial,
                                  const G4LossTable* aLossTable,
                                  G4double       maxEnergy,
                                  G4double       aMass,
                                  G4RangeVector* rangeVector)
    {
        BuildAbsorptionLengthVector(aMaterial,
                                    (const G4CrossSectionTable*)aLossTable,
                                    maxEnergy, aMass, rangeVector);
    }

    G4double ConvertCutToKineticEnergy(
                                 G4RangeVector* absorptionLengthVector,
 size_t materialIndex
                                ) const;

    G4double ConvertCutToKineticEnergy(
                                 G4RangeVector* absorptionLengthVector
                                ) const;


 private: // hide conxtructor as private
   G4XRay(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,
       G4int               iSpin,        G4int               iParity,
       G4int               iConjugation, G4int               iIsospin,
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable
   );

 public:
   virtual ~G4XRay(){}
   static G4XRay* XRayDefinition();
   static G4XRay* XRay();
/*
   static G4double  GetCuts() {return theXRayLengthCut;}
   static G4double* GetCutsInEnergy() {return theXRayKineticEnergyCuts;};

   virtual void SetCuts(G4double aCut);
*/
};

/*
inline void G4XRay::SetCuts(G4double aCut)
{
  CalcEnergyCuts(aCut);
  theXRayLengthCut = theCutInMaxInteractionLength;
  theXRayKineticEnergyCuts = theKineticEnergyCuts;

}
*/

inline G4XRay* G4XRay::XRay()
{ return &theXRay; }

#include "G4VDiscreteProcess.hh"
#include "G4VContinuousProcess.hh"
class G4XRayTRDP : public G4VDiscreteProcess{
public:
G4XRayTRDP(G4String name):G4VDiscreteProcess(name){};
G4double GetMeanFreePath(const  G4Track & track, G4double step, G4ForceCondition *){return DBL_MAX;}
G4double PostStepGetPhysicalInteractionLength(const  G4Track & track, G4double step, G4ForceCondition *);
virtual G4bool IsApplicable(const G4ParticleDefinition& p){
return (p.GetPDGCharge() !=0 && !p.IsShortLived()) ;
}
G4VParticleChange* PostStepDoIt( const G4Track& track, const G4Step& stepData );
};
class G4XRayTRCP : public G4VContinuousProcess{
public:
G4XRayTRCP(G4String name):G4VContinuousProcess(name){};
virtual G4bool IsApplicable(const G4ParticleDefinition& p){
return (p.GetPDGCharge() !=0 && !p.IsShortLived()) ;
}
G4VParticleChange* AlongStepDoIt( const G4Track& track, const G4Step& stepData );
        G4double GetContinuousStepLimit(const G4Track& aTrack,
                                        G4double  ,
                                        G4double  ,
                                        G4double& ){return DBL_MAX;}

};
#endif
