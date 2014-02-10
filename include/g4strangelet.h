#ifndef __G4STRANGELET__
#define __G4STRANGELET__
#include "globals.hh"
#include "G4ios.hh"
#include "G4Ions.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsTable.hh"
#include "G4Step.hh"
#include <string>

// ######################################################################
// ###                     strangelet                                 ###
// ######################################################################




#include "G4ParticleTable.hh"
#include "G4VDiscreteProcess.hh"
#include "G4VCrossSectionDataSet.hh"
#include "G4HadronicInteraction.hh"
#include "G4HadFinalState.hh"
#include "G4Track.hh"
#include "G4Nucleus.hh"
#include "G4HadProjectile.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Version.hh"
class TF1;
class G4StrangeletP : public G4VDiscreteProcess {
    public:
    G4StrangeletP(G4String name) : G4VDiscreteProcess(name) {};
    G4double GetMeanFreePath(const G4Track& track, G4double step, G4ForceCondition *);
    G4double PostStepGetPhysicalInteractionLength(const G4Track& track, G4double step, G4ForceCondition *);
    virtual G4bool IsApplicable(const G4ParticleDefinition& p);
    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& stepData);
};


class StrHP:   public G4HadronicInteraction{
  public:
  StrHP();
    G4bool IsApplicable (const G4HadProjectile &theTrack, G4Nucleus &theTarget);
    virtual G4HadFinalState *ApplyYourself(const G4HadProjectile &, G4Nucleus &);
    
  protected:
  int     g[25];
  int     IsStr[25];
  double n_S[25];
  double n_I3[25];
  double n_B[25];
  double mass[25];
  double number[25];
  std::string name[25];
  double total_S[25];
  double total_I3[25];
  double total_B[25];
  double total_E[25];
  int    GEANT_ID[25];
  int particle_NUM;
    static TF1 *f1;
    static TF1 *fE;
    G4ParticleTable         *theParticleTable;
    G4IonTable              *theIonTable;
    void Thermal(double m_s, double m_a, double targetA, double strZ, double strA, double strBetaIn[3], double &StrA_out , double &StrZ_out, double StrMom_out[4], int &n_out, int ID_out[1000], double mom_out[1000][4]);
};
class StrCS:  public G4VCrossSectionDataSet{
  public:
    StrCS();
    ~StrCS();
    virtual G4bool IsApplicable(const G4DynamicParticle* theProjectile,
      const G4Element* theTarget);

// >>> A. Oliva - compatibility with geant4.9.6
#if G4VERSION_NUMBER  > 945 
    virtual G4bool IsIsoApplicable(const G4DynamicParticle* theProjectile, 
                                   G4int ZZ, G4int AA, 
                                   const G4Element* elm = 0, const G4Material* mat = 0);
#else
    virtual G4bool IsIsoApplicable(const G4DynamicParticle* theProjectile, 
                                   G4int ZZ, G4int AA);
#endif

#if G4VERSION_NUMBER  > 945 
    virtual G4double GetIsoCrossSection(const G4DynamicParticle* theProjectile, 
                                        G4int ZZ, G4int AA, 
                                        const G4Isotope* iso = 0, const G4Element* elm = 0, const G4Material* mat = 0);
#endif
// <<< A. Oliva - compatibility with geant4.9.6

    virtual G4double GetCrossSection(const G4DynamicParticle* theProjectile,
      const G4Element* theTarget, G4double theTemperature);

    virtual
    G4double GetZandACrossSection(const G4DynamicParticle* theProjectile,
                                  G4int ZZ, G4int AA,
                                  G4double theTemperature);

    virtual void BuildPhysicsTable(const G4ParticleDefinition&)
    {}

    virtual void DumpPhysicsTable(const G4ParticleDefinition&)
    {G4cout << "StrCS: uses formula"<<G4endl;}
    void SetLowEnergyCheck(G4bool);

  private:
    G4bool         lowEnergyCheck;
};

#endif


