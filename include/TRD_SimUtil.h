#ifndef _TRD_SimUtil_
#define _TRD_SimUtil_

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

extern AMSgmat mat;

class TrdSimUtil{
 private:
  G4Material *trdTubeGasMaterial;
  G4Material *trdFleeceMaterial;
  G4Material *trdFleeceGasMaterial;
  G4Material *trdRadiatorArtificial;

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
  TrdSimUtil():trdGasTemperature(290.),trdGasPressure(1.*atmosphere),trdFleeceFiberDiameter(12.0*um),trdRadiatorThickness(20.0*mm),fAlphaFiber(100.),fAlphaGas(100.),trdFleeceGasConf(0),fTrdNumberFractionXe(0.8),fTrdNumberFractionCO2(0.2),fTrdNumberFractionAr(0.){
    //    DefineRadiator();
    //    DefineTubeGas();
  };
  
  void UpdateGas( void );
  
  void SetTrdNumberFractionXenon( G4double val ) { fTrdNumberFractionXe = val; }
  void SetTrdNumberFractionArgon( G4double val ) { fTrdNumberFractionAr = val; }
  void SetTrdNumberFractionCO2( G4double val ) { fTrdNumberFractionCO2 = val; }

  void SetTrdRadiatorFleeceFiberDiameter( G4double val ) {trdFleeceFiberDiameter = val;}
  
  void SetTrdGasTemperature( G4double val ) {trdGasTemperature = val;}
  void SetTrdGasPressure( G4double val ) {trdGasPressure = val;}

  //  AMSgmat *GetRadiatorFleeceMaterial( void );
  AMSgmat *GetTubeGasMaterial( void );
  AMSgmat *GetRadiatorArtificialMaterial( void );

  G4Material *GetG4TubeGasMaterial( void ){return trdTubeGasMaterial;}
  G4Material *GetG4FleeceMaterial( void ){return trdFleeceMaterial;}
  G4Material *GetG4FleeceGasMaterial( void ){return trdFleeceGasMaterial;}
  //  AMSgmat *GetRadiatorGasMaterial( void );
  
  G4double GetTrdFoilThickness ( void ){return (trdFleeceFiberDiameter);}
  G4double GetTrdGasThickness ( void ){return (trdMeanGasGap);}
  G4double GetTrdFoilNumber ( void ){return (trdMeanFoilNumber);}
  G4double GetAlphaFiber ( void ){return fAlphaFiber;}
  G4double GetAlphaGas ( void ){return fAlphaGas;}
  //  TRD_TrdPhysics *DefineGammaXTR( void );
  
  void DefineTubeGas( void );
  void DefineRadiator( void );

};

extern TrdSimUtil trdSimUtil;
#endif
