#include "ECALShower.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(ECALShower)
  COL( "Hits",                 UShort_t, NumberOfHits)
  COL( "E_depo [MeV]",         Float_t,  DepositedEnergy)
  COL( "Energy3CMRatio",       Float_t,  EnergyAt3CMRatio)
  COLE("E_reco [GeV]\t\t\t\t", Float_t,  ReconstructedEnergy)
  COL( "Rear leak",            Float_t,  RelativeRearLeak)
  COL( "Shower max [cm]",      Float_t,  ShowerMaximum)
  COL( "Chi2\t\t",             Float_t,  ChiSquareProfile)
  NEWTABLE
  COL( "Theta [deg]", Float_t, Theta)
  COL( "Phi [deg]",   Float_t, Phi)
  COL( "X [cm]\t",    Float_t, X)
  COL( "Y [cm]\t",    Float_t, Y)
  COL( "Z [cm]\t",    Float_t, Z)
  COL( "EnX [cm]\t",  Float_t, EntryX)
  COL( "EnY [cm]\t",  Float_t, EntryY)
  COL( "EnZ [cm]\t",  Float_t, EntryZ)
  NEWTABLE
  COL( "ExX [cm]\t",         Float_t,          ExitX)
  COL( "ExY [cm]\t",         Float_t,          ExitY)
  COL( "ExZ [cm]\t",         Float_t,          ExitZ)
  COL( "Estimators\t\t\t\t", EstimatorsVector, Estimators)
END_DEBUG_TABLE

}

}
