// ----------------------------------------------------------
//    HEAO experiment Ion Cross-Section
// ----------------------------------------------------------
// History
//    Created:  06-Aug-2013 Q.Yan qyan@cern.ch
// ----------------------------------------------------------

#include "G4IonsHEAOCrossSection.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4HadTmpUtil.hh"
#include "G4DynamicParticle.hh"


///////////////////////////////////////////////////////////////////////////////
//
G4double G4IonsHEAOCrossSection::
GetZandACrossSection(const G4DynamicParticle* aParticle, G4int ZZ, 
                     G4int AA, G4double /*temperature*/)
{

   G4int Ap = aParticle->GetDefinition()->GetBaryonNumber();
   G4int Zp = G4int(aParticle->GetDefinition()->GetPDGCharge()/eplus + 0.5 ); 

   G4int At = AA;
   G4int Zt = ZZ;
 
   G4double one_third = 1.0 / 3.0;

   G4double cubicrAt = std::pow ( G4double(At) , G4double(one_third) );  
   G4double cubicrAp = std::pow ( G4double(Ap) , G4double(one_third) );  

   G4double R=cubicrAt+cubicrAp-0.83;

   G4double xsection= 57.3*R*R*millibarn; 

   return xsection; 
}

///////////////////////////////////////////////////////////////////////////////
//
G4double G4IonsHEAOCrossSection::
GetCrossSection(const G4DynamicParticle* aParticle, const G4Element* anElement,
                G4double temperature)
{
  G4int nIso = anElement->GetNumberOfIsotopes();
  G4double xsection = 0;
   
  if (nIso) {
    G4double sig;
    G4IsotopeVector* isoVector = anElement->GetIsotopeVector();
    G4double* abundVector = anElement->GetRelativeAbundanceVector();
    G4int ZZ;
    G4int AA;
   
    for (G4int i = 0; i < nIso; i++) {
      ZZ = (*isoVector)[i]->GetZ();
      AA = (*isoVector)[i]->GetN();
      sig = GetZandACrossSection(aParticle, ZZ, AA, temperature);
      xsection += sig*abundVector[i];
    }
 
  } else {
    G4int ZZ = G4lrint(anElement->GetZ());
    G4int AA = G4lrint(anElement->GetN());
    xsection = GetZandACrossSection(aParticle, ZZ, AA, temperature);
  }
  
  return xsection;
}


///////////////////////////////////////////////////////////////////////////////
//
G4bool G4IonsHEAOCrossSection::IsApplicable(const G4DynamicParticle* theProjectile, const G4Element*){

  return IsIsoApplicable(theProjectile, 0, 0); 
}

///////////////////////////////////////////////////////////////////////////////
//
G4bool G4IonsHEAOCrossSection::IsIsoApplicable(const G4DynamicParticle* theProjectile, G4int ZZ, G4int AA){

   G4int AP = theProjectile->GetDefinition()->GetBaryonNumber();
   G4double EPN= theProjectile->GetKineticEnergy()/AP;
   G4bool result  =(EPN > lowerLimit && EPN <= upperLimit);
   return result;
}
