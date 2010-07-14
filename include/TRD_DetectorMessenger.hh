
#ifndef TRD_DetectorMessenger_h
#define TRD_DetectorMessenger_h

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"

class TRD_DetectorConstruction;

class TRD_DetectorMessenger : public G4UImessenger
{

 public:
  explicit TRD_DetectorMessenger( TRD_DetectorConstruction* myDet);
  ~TRD_DetectorMessenger();

  void SetNewValue(G4UIcommand* command, G4String newValue);

 private:
  TRD_DetectorConstruction*  theDetectorConstruction;
  
  G4UIdirectory*             detDir;
  G4UIcmdWithADouble*        trdArgonFractionCmd;
  G4UIcmdWithADouble*        trdXenonFractionCmd;
  G4UIcmdWithADouble*        trdCO2FractionCmd;
  G4UIcmdWithADoubleAndUnit* trdGasPressCmd;
  G4UIcmdWithADoubleAndUnit* trdGasTempCmd;
  G4UIcmdWithAString*        trdUpdCmd; 

  G4UIcmdWithADouble*        trdTGradXaCmd;
  G4UIcmdWithADouble*        trdTGradYaCmd;
  G4UIcmdWithADouble*        trdTGradZaCmd;
 
  G4UIcmdWithADouble*        trdTGradXbCmd;
  G4UIcmdWithADouble*        trdTGradYbCmd;
  G4UIcmdWithADouble*        trdTGradZbCmd;
 
  G4UIcmdWithADouble*        trdTGradCentCmd;

  G4UIcmdWithAString*        theReadCommand;
  G4UIcmdWithAString*        theWriteCommand;
  G4UIcmdWithAString*        theStepCommand;

  G4UIcmdWithADoubleAndUnit* trdFleeceDiamCmd;
};

#endif
