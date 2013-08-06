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

class G4IonsHEAOCrossSection : public G4VCrossSectionDataSet
{
  public:
    G4IonsHEAOCrossSection()
     : G4VCrossSectionDataSet("G4IonsHEAOCrossSection"),upperLimit( 1000*TeV ), lowerLimit( 10*MeV ){}
   
    virtual
    G4bool IsApplicable(const G4DynamicParticle* theProjectile, const G4Element*);

    virtual
    G4bool IsIsoApplicable(const G4DynamicParticle* theProjectile,
                           G4int /*ZZ*/, G4int /*AA*/);

    virtual
    G4double GetCrossSection(const G4DynamicParticle*, 
                             const G4Element*, G4double aTemperature);

    virtual
    G4double GetZandACrossSection(const G4DynamicParticle*, G4int ZZ, 
                                  G4int AA, G4double aTemperature);

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
