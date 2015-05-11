#ifndef G4Physics_Ion_h
#define G4Physics_Ion_h 1
// ------------------------------------------------------------
//      AMS GEANT4 new Ion Physics 
//      Ion Eleastic for high Energy->DPM-JET
// ------------------------------------------------------------
//      History
//        Created:       2012-Apr-09  Q.Yan
//        Modified:
// -----------------------------------------------------------
#include "G4VHadronPhysics.hh"
#include "globals.hh"
class G4StrangeletP;
class G4BinaryLightIonReaction;
class G4VCrossSectionDataSet;
#ifdef G4_USE_DPMJET
class G4DPMJET2_5Model;
class G4DPMJET2_5CrossSection;
class G4IonsHEAOCrossSection;
#endif
#include "G4INCLXXInterface.hh"
#include "G4FTFBuilder.hh"
class IonDPMJETPhysics : public G4VHadronPhysics
{
 public:
    IonDPMJETPhysics();
    virtual ~IonDPMJETPhysics();
    void ConstructProcess();
    int UseInclXX;   
    IonDPMJETPhysics(int useinclxx):UseInclXX(useinclxx),G4VHadronPhysics(){};
 private:

  void AddProcess(const G4String& name, G4ParticleDefinition* part,
                  G4bool isIon);

  G4VCrossSectionDataSet* fTripathi;
  G4VCrossSectionDataSet* fTripathiLight;
  G4VCrossSectionDataSet* fShen;
  G4VCrossSectionDataSet* fIonH;
#ifdef G4_USE_DPMJET
  G4DPMJET2_5CrossSection* dpmXS;
  G4IonsHEAOCrossSection * HEAOXS;
  G4DPMJET2_5Model*        theDPM;
#endif
  G4BinaryLightIonReaction*  theIonBC;
   G4INCLXXInterface*  theINCLXX;
   G4HadronicInteraction*  theFTF;
  G4BinaryLightIonReaction*  theIonBC1;
};

#endif
