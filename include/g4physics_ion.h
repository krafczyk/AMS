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

class G4BinaryLightIonReaction;
class G4VCrossSectionDataSet;
#ifdef G4_USE_DPMJET
class G4DPMJET2_5Model;
class G4DPMJET2_5CrossSection;
class G4IonsHEAOCrossSection;
#endif
class IonDPMJETPhysics : public G4VHadronPhysics
{
 public:
    IonDPMJETPhysics(G4bool val=1);
    virtual ~IonDPMJETPhysics();
    void ConstructProcess();
   
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
  G4bool useDPMJETXS;
  G4BinaryLightIonReaction*  theIonBC;
  G4BinaryLightIonReaction*  theIonBC1;
};

#endif
