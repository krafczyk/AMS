#ifndef AMSPhysicsList_HadronIon_h 
#define AMSPhysicsList_HadronIon_h 1
#include "G4VPhysicsConstructor.hh" 
#include "globals.hh" 
#include "G4DeuteronInelasticProcess.hh" 
#include "G4TritonInelasticProcess.hh" 
#include "G4AlphaInelasticProcess.hh"
#include "G4LElastic.hh"
#include "stdio.h"
#include "commons.h"
#include "G4ParticleDefinition.hh" 
#include "G4ProcessManager.hh" 
#include "G4LFission.hh" 
#include "G4LCapture.hh"
#include "G4Deuteron.hh" 
#include "G4Triton.hh" 
#include "G4He3.hh" 
#include "G4Alpha.hh" 
#include "G4GenericIon.hh"
#include "G4ExcitationHandler.hh" 
#include "G4Evaporation.hh" 
#include "G4FermiBreakUp.hh" 
#include "G4StatMF.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4HadronInelasticProcess.hh" 
#include "G4IonInelasticProcess.hh" 
#include "G4BinaryLightIonReaction.hh" 
#include "G4WilsonAbrasionModel.hh" 
#include "G4GeneralSpaceNNCrossSection.hh" 
#include "G4LEDeuteronInelastic.hh" 
#include "G4LETritonInelastic.hh" 
#include "G4LEAlphaInelastic.hh"
#include "G4EMDissociation.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "AMSPhysicsList_HadronIon.h" 
#include "G4LElastic.hh"
#include "G4HadronElasticProcess.hh"
class G4HadronInelasticProcess; 
//////////////////////////////////////////////////////////////////////////////// 
// 
class AMSPhysicsList_HadronIon : public G4VPhysicsConstructor {
public: 
AMSPhysicsList_HadronIon(const G4String& name = "abrasion_ion"); 
virtual ~AMSPhysicsList_HadronIon();
public: 
void ConstructParticle(){}
void ConstructProcess();
private:





}; //////////////////////////////////////////////////////////////////////////////// //
#endif
