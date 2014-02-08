#ifndef G4IonsHEAOCrossSection_h
#define G4IonsHEAOCrossSection_h

// ----------------------------------------------------------
//    HEAO experiment Ion Cross-Section
// ----------------------------------------------------------
// History
//    Created:  06-Aug-2013 Q.Yan qyan@cern.ch
// ----------------------------------------------------------

#include "globals.hh"
#include "G4VCrossSectionDataSet.hh"
#include "G4Element.hh" 
#include "G4Material.hh"

class G4IonsHEAOCrossSection : public G4VCrossSectionDataSet
{
  public:
    G4IonsHEAOCrossSection()
     : G4VCrossSectionDataSet("G4IonsHEAOCrossSection"),upperLimit( 1000*TeV ), lowerLimit( 10*MeV ){}
  
    virtual
    G4bool IsApplicable(const G4DynamicParticle* theProjectile, const G4Element*);

    virtual
    G4double GetCrossSection(const G4DynamicParticle*, 
                             const G4Element*, G4double aTemperature);

    virtual
    G4double GetZandACrossSection(const G4DynamicParticle*, G4int ZZ, 
                                  G4int AA, G4double aTemperature);


// >>> A. Oliva - compatibility with geant4.9.6
#if G4VERSION_NUMBER  > 945 
    virtual
    G4bool IsIsoApplicable(const G4DynamicParticle* theProjectile, G4int ZZ, G4int AA, 
                           const G4Element* elm = 0, const G4Material* mat = 0); 
#else
    virtual
    G4bool IsIsoApplicable(const G4DynamicParticle* theProjectile, G4int ZZ, G4int AA);
#endif

#if G4VERSION_NUMBER  > 945 
    virtual 
    G4double GetIsoCrossSection(const G4DynamicParticle* theProjectile, G4int ZZ, 
                                G4int AA, const G4Isotope* iso = 0, const G4Element* elm = 0, const G4Material* mat = 0); 
#endif
// <<< A. Oliva - compatibility with geant4.9.6

    virtual
    void BuildPhysicsTable(const G4ParticleDefinition&)
    {}

    virtual
    void DumpPhysicsTable(const G4ParticleDefinition&) 
    {G4cout << "G4HEAOCrossSection: uses"<<G4endl;}

  private:
    const G4double upperLimit;
    const G4double lowerLimit; 

};

#endif
