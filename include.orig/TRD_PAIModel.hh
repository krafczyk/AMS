#ifndef TRD_PAIModel_h
#define TRD_PAIModel_h 1

#include <vector>
#include "G4VEmModel.hh"
#include "globals.hh"
#include "G4VEmFluctuationModel.hh"
#include "G4PAIySection.hh"

class G4PhysicsLogVector;
class G4PhysicsTable;
class G4Region;
class G4MaterialCutsCouple;
class G4ParticleChangeForLoss;

class TRD_PAIModel : public G4VEmModel, public G4VEmFluctuationModel
{

public:

  TRD_PAIModel(const G4ParticleDefinition* p = 0, const G4String& nam = "PAI");

  virtual ~TRD_PAIModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual void InitialiseMe(const G4ParticleDefinition*) {};

  virtual G4double ComputeDEDX(const G4MaterialCutsCouple*,
			       const G4ParticleDefinition*,
			       G4double kineticEnergy,
			       G4double cutEnergy);

  virtual G4double CrossSection(const G4MaterialCutsCouple*,
				const G4ParticleDefinition*,
				G4double kineticEnergy,
				G4double cutEnergy,
				G4double maxEnergy);

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

  virtual G4double SampleFluctuations(const G4Material*,
				      const G4DynamicParticle*,
				      G4double&,
				      G4double&,
				      G4double&);

  virtual G4double Dispersion(    const G4Material*,
				  const G4DynamicParticle*,
				  G4double&,
				  G4double&);

  void     DefineForRegion(const G4Region* r) ;
  void     ComputeSandiaPhotoAbsCof();
  void     BuildPAIonisationTable();
  void     BuildLambdaVector();

  G4double GetdNdxCut( G4int iPlace, G4double transferCut);
  G4double GetdEdxCut( G4int iPlace, G4double transferCut);
  G4double GetPostStepTransfer( G4double scaledTkin );
  G4double GetEnergyTransfer( G4int iPlace,
			      G4double position,
			      G4int iTransfer );

  void SetVerboseLevel(G4int verbose){fVerbose=verbose;};



protected:

  G4double MaxSecondaryEnergy(const G4ParticleDefinition*,
                                    G4double kinEnergy);

private:

  void SetParticle(const G4ParticleDefinition* p);

  // hide assignment operator 
  TRD_PAIModel & operator=(const  TRD_PAIModel &right);
  TRD_PAIModel(const  TRD_PAIModel&);

  // The vector over proton kinetic energies: the range of gammas
  G4int                fVerbose; 
  G4double             fLowestGamma;
  G4double             fHighestGamma;
  G4double             fLowestKineticEnergy;
  G4double             fHighestKineticEnergy;
  G4int                fTotBin;
  G4int                fMeanNumber;
  G4PhysicsLogVector*  fParticleEnergyVector ;
  G4PAIySection        fPAIySection;

  // vectors

  G4PhysicsTable*                    fPAItransferTable;
  std::vector<G4PhysicsTable*>       fPAIxscBank;

  G4PhysicsTable*                    fPAIdEdxTable;
  std::vector<G4PhysicsTable*>       fPAIdEdxBank;

  std::vector<const G4MaterialCutsCouple*> fMaterialCutsCoupleVector;
  std::vector<const G4Region*>       fPAIRegionVector;

  const G4MaterialCutsCouple*        fCutCouple;
  const G4Material*                  fMaterial;
  G4double                           fDeltaCutInKinEnergy; 

  size_t                             fMatIndex ;  
  G4double**                         fSandiaPhotoAbsCof ;
  G4int                              fSandiaIntervalNumber ;

  G4PhysicsLogVector*                fdEdxVector ;
  std::vector<G4PhysicsLogVector*>   fdEdxTable ;

  G4PhysicsLogVector*                fLambdaVector ;
  std::vector<G4PhysicsLogVector*>   fLambdaTable ;

  G4PhysicsLogVector*                fdNdxCutVector ;
  std::vector<G4PhysicsLogVector*>   fdNdxCutTable ;

  const G4ParticleDefinition* fParticle;
  const G4ParticleDefinition* fElectron;
  const G4ParticleDefinition* fPositron;
  G4ParticleChangeForLoss*    fParticleChange;

  G4double fMass;
  G4double fSpin;
  G4double fChargeSquare;
  G4double fRatio;
  G4double fHighKinEnergy;
  G4double fLowKinEnergy;
  G4double fTwoln10;
  G4double fBg2lim; 
  G4double fTaulim;
  G4double fQc;

  G4bool   isInitialised;
};

/////////////////////////////////////////////////////////////////////

inline G4double TRD_PAIModel::MaxSecondaryEnergy( const G4ParticleDefinition* p,
                                                      G4double kinEnergy) 
{
  G4double tmax = kinEnergy;
  if(p == fElectron) tmax *= 0.5;
  else if(p != fPositron) { 
    G4double mass = p->GetPDGMass();
    G4double ratio= electron_mass_c2/mass;
    G4double gamma= kinEnergy/mass + 1.0;
    tmax = 2.0*electron_mass_c2*(gamma*gamma - 1.) /
                  (1. + 2.0*gamma*ratio + ratio*ratio);
  }
  return tmax;
}

///////////////////////////////////////////////////////////////

inline  void TRD_PAIModel::DefineForRegion(const G4Region* r) 
{
  fPAIRegionVector.push_back(r);
}

#endif







