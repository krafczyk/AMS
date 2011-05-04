#ifndef _TRD_SimUtil_
#define _TRD_SimUtil_

#include "commons.h"
#include "gmat.h"
#include "globals.hh"

#include "TRD_VXTenergyLoss.hh"
#include "TRD_GammaXTRadiator.hh"
#include "TRD_TrdPhysics.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Proton.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4VPersistencyManager.hh"
#include "G4ProcessManager.hh"
#include "G4PAIModel.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolumeStore.hh"
#include <G4VModularPhysicsList.hh>
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4MultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hIonisation.hh"

#include "G4ionIonisation.hh"


#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4Region.hh"
#include "G4RegionStore.hh"

#include "G4PAIModel.hh"

extern AMSgmat mat;

class TrdSimUtil
{
 private:
  G4Material *trdTubeGasMaterial;
  G4Material *trdFleeceMaterial;
  G4Material *trdFleeceGasMaterial;
  G4Material *trdRadiatorArtificial;
  G4Material *trdKaptonMaterial;

  G4double trdFleeceFiberDiameter;
  G4double trdRadiatorThickness;
  G4double trdGasTemperature;                  ///< temperature of TRD gas
  G4double trdGasPressure;                     ///< pressure of TRD gas
  G4double trdFoilThickness;
  G4double trdMeanFoilNumber;
  G4double trdMeanRadiatorFibreDensity;
  G4double trdMeanGasGap;

  G4double fTrdNumberFractionXe;
  G4double fTrdNumberFractionAr;
  G4double fTrdNumberFractionCO2;

  G4double fAlphaFiber;
  G4double fAlphaGas;
  
  G4int trdFleeceGasConf;///< gas in radiator 0=AIR,1=VACUUM

 public:
  TrdSimUtil();
  
  void UpdateGas( void );
  void EndOfRun( void );

  void SetTrdNumberFractionXenon( G4double val ) { fTrdNumberFractionXe = val; }
  void SetTrdNumberFractionArgon( G4double val ) { fTrdNumberFractionAr = val; }
  void SetTrdNumberFractionCO2( G4double val ) { fTrdNumberFractionCO2 = val; }

  void SetTrdRadiatorFleeceFiberDiameter( G4double val ) {trdFleeceFiberDiameter = val;}
  
  void SetTrdGasTemperature( G4double val ) {trdGasTemperature = val;}
  void SetTrdGasPressure( G4double val ) {trdGasPressure = val;}

  AMSgmat *GetTubeGasMaterial( void );
  AMSgmat *GetRadiatorArtificialMaterial( void );
  AMSgmat *GetKaptonMaterial( void );

  G4Material *GetG4TubeGasMaterial( void ){return trdTubeGasMaterial;}
  G4Material *GetG4FleeceMaterial( void ){return trdFleeceMaterial;}
  G4Material *GetG4FleeceGasMaterial( void ){return trdFleeceGasMaterial;}
  G4Material *GetG4KaptonMaterial( void ){return trdKaptonMaterial;}
  
  G4double GetTrdFoilThickness ( void ){return (trdFleeceFiberDiameter);}
  G4double GetTrdGasThickness ( void ){return (trdMeanGasGap);}
  G4double GetTrdFoilNumber ( void ){return (trdMeanFoilNumber);}
  G4double GetAlphaFiber ( void ){return fAlphaFiber;}
  G4double GetAlphaGas ( void ){return fAlphaGas;}
  
  void DefineTubeGas( void );
  void DefineRadiator( void );

  G4ProductionCuts *fTrdGasRegionCuts;
  G4ProductionCuts *fTrdRadRegionCuts;
  G4Region*        gasregion;
  G4Region*        radregion;
  G4LogicalVolume* radlv;
};


extern TrdSimUtil trdSimUtil;
#endif
