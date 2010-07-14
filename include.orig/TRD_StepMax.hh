#ifndef TRD_STEPMAX_H
#define TRD_STEPMAX_H

#include "G4ios.hh"
#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4Step.hh"

/// \brief Process to limit step length in a given volume.
///
/// Based on G4's TestEm10 example.
class TRD_StepMax : public G4VDiscreteProcess
{
 public:     

  TRD_StepMax( const G4String& processName, const G4String& logicalVolumeName );
  TRD_StepMax(TRD_StepMax &);

  ~TRD_StepMax();

  G4double PostStepGetPhysicalInteractionLength( const G4Track& track,
						 G4double   previousStepSize,
						 G4ForceCondition* condition  );

  G4VParticleChange* PostStepDoIt( const G4Track& ,
				   const G4Step& );

  void SetMaxStep(G4double);

 protected:

  // it is not needed here !
  G4double GetMeanFreePath( const G4Track& aTrack,
			    G4double   previousStepSize,
			    G4ForceCondition* condition ) { return( 0.0 ); }

 private:
  
  // hide assignment operator as private 
  TRD_StepMax & operator=(const TRD_StepMax &right);

 private:

  G4double MaxChargedStep;
  G4String LogicalVolumeName;

};

#endif
