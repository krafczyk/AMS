
// $Id: TRD_StepMax.C,v 1.1 2010/07/14 14:23:01 zweng Exp $

#include "TRD_StepMax.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4EnergyLossTables.hh"

TRD_StepMax::TRD_StepMax( const G4String& processName, const G4String& logicalVolumeName )
  : G4VDiscreteProcess(processName), MaxChargedStep(DBL_MAX), LogicalVolumeName( logicalVolumeName )
{
  if (verboseLevel>0) {
    G4cout << GetProcessName() << " is created "<< G4endl;
  }
}

TRD_StepMax::~TRD_StepMax()
{
}

TRD_StepMax::TRD_StepMax(TRD_StepMax& right)
  : G4VDiscreteProcess(right)
{}

void TRD_StepMax::SetMaxStep(G4double step)
{
  G4cout << "VMCTest: TRD_StepMax::SetMaxStep  is Called "<< G4endl;
  MaxChargedStep = step ;
}


G4double TRD_StepMax::PostStepGetPhysicalInteractionLength( const G4Track& aTrack,
							    G4double,
							    G4ForceCondition* condition ){
  // condition is set to "Not Forced"
  *condition = NotForced;

  G4double ProposedStep = DBL_MAX;

  if((MaxChargedStep > 0.) &&
     (aTrack.GetVolume() != NULL) &&
     (aTrack.GetVolume()->GetLogicalVolume()->GetName() == LogicalVolumeName ) &&
     (aTrack.GetDynamicParticle()->GetDefinition()->GetPDGCharge() != 0.)){
    ProposedStep = MaxChargedStep ;
     G4cout << "[TRD_StepMax::PostStepGetPhysicalInteractionLength] Volume " << aTrack.GetVolume()->GetName() << ": step= " << ProposedStep/mm << " mm !" << G4endl;
  }

  return ProposedStep;
}

G4VParticleChange* TRD_StepMax::PostStepDoIt( const G4Track& aTrack,
					      const G4Step& ){
  // do nothing
  aParticleChange.Initialize(aTrack);
  return &aParticleChange;
}

