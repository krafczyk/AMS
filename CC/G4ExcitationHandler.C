#include "G4Version.hh"
#if G4VERSION_NUMBER  > 945 
#define __G4PROTONBUG__
#endif
#ifdef __G4PROTONBUG__


#include "G4BGGNucleonInelasticXS.hh"
#include "G4SystemOfUnits.hh"
#include "G4GlauberGribovCrossSection.hh"
#include "G4NucleonNuclearCrossSection.hh"
#include "G4HadronNucleonXsc.hh"
//#include "G4HadronInelasticDataSet.hh"
#include "G4ComponentSAIDTotalXS.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"

#include "G4CrossSectionDataSetRegistry.hh"



G4double
G4BGGNucleonInelasticXS::GetIsoCrossSection(const G4DynamicParticle* dp,
                                            G4int Z, G4int A,
                                            const G4Isotope*,
                                            const G4Element*,
                                            const G4Material*)
{
  // this method should be called only for Z = 1

  G4double cross = 0.0;
  G4double ekin = dp->GetKineticEnergy();

  if(ekin <= fSAIDHighEnergyLimit) {
    cross = fSAID->GetInelasticIsotopeCrossSection(particle, ekin, 1, 1);
  } else if(ekin < fHighEnergy) {
    fHadron->GetHadronNucleonXscNS(dp, theProton);
    cross = (theCoulombFac[0]/ekin + 1)*fHadron->GetInelasticHadronNucleonXsc();
  } else {
    fHadron->GetHadronNucleonXscPDG(dp, theProton);
    cross = (theCoulombFac[1]/ekin + 1)*fHadron->GetInelasticHadronNucleonXsc();
  }
  cross *= A;

  if(verboseLevel > 1) {
    G4cout << "G4BGGNucleonInelasticXS::GetCrossSection  for "
           << dp->GetDefinition()->GetParticleName()
           << "  Ekin(GeV)= " << dp->GetKineticEnergy()/CLHEP::GeV
           << " in nucleus Z= " << Z << "  A= " << A
           << " XS(b)= " << cross/barn
           << G4endl;
  }
 //VC
  return cross*1.09;
}



#include "G4SubtractionSolid.hh"
G4double  G4SubtractionSolid::DistanceToIn( const G4ThreeVector& p ) const 
{
  G4double dist=0.0;

  if( Inside(p) == kInside )
  {
    G4cout << "WARNING - Invalid call in "
           << "G4SubtractionSolid::DistanceToIn(p)" << G4endl
           << "  Point p is inside !" << G4endl;
    G4cout << "          p = " << p << G4endl;
    G4cerr << "WARNING - Invalid call in "
           << "G4SubtractionSolid::DistanceToIn(p)" << G4endl
           << "  Point p is inside !" << G4endl;
    G4cerr << "          p = " << p << G4endl;
    G4cerr<< fPtrSolidA->GetName()<<" "<< fPtrSolidB->GetName()<<std::endl;
  }

  if( ( fPtrSolidA->Inside(p) != kOutside) &&   // case 1
      ( fPtrSolidB->Inside(p) != kOutside)    )
  {
      dist= fPtrSolidB->DistanceToOut(p)  ;
  }
  else
  {
      dist= fPtrSolidA->DistanceToIn(p) ; 
  }
  
  return dist; 
}


#endif
#if G4VERSION_NUMBER  < 946 

#include "G4EMDissociation.hh"
G4EMDissociation::~G4EMDissociation ()
{
  if (handlerDefinedInternally) delete theExcitationHandler;
  delete thePhotonSpectrum;
}
/*
  #include "G4IonProtonCrossSection.hh"

G4bool G4IonProtonCrossSection::IsIsoApplicable(const G4DynamicParticle* dp,
                                         G4int Z, G4int A)
{
  G4bool result = false;
  if(Z < 2 && A < 2 && dp->GetDefinition()->GetPDGCharge()/eplus > 1.5)
    { result = true;}
  return result;
}

*/
#endif
