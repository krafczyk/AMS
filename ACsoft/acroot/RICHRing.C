#include "RICHRing.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(RICHRing)
  COL( "Status Bitset\t\t\t\t\t",  std::bitset<32>,   StatusBitset)
  COL( "Hits",           UShort_t, NumberOfHits)
  COL( "UsedHits",       UShort_t, NumberOfUsedHits)
  COLE("Beta\t\t\t\t\t", Float_t,  Beta)
  COL( "BetaConsist",    Float_t,  BetaConsistency)
  COL( "X [cm]\t",       Float_t,  X)
  COL( "Y [cm]\t",       Float_t,  Y)
  COL( "Z [cm]\t",       Float_t,  Z)
  COL( "Theta [deg]",    Float_t,  Theta)
  COL( "Phi [deg]",      Float_t,  Phi)
  NEWTABLE
  COL( "Prob\t\t",       Float_t,  Probability)
  COL( "CEstim\t",       Float_t,  ChargeEstimate)
  COL( "NPhotoE",        Float_t,  NPhotoElectrons)
  COL( "NExpPhotoE",     Float_t,  NExpectedPhotoElectrons)
  COL( "NCollPhotoE",    Float_t,  NCollectedPhotoElectrons)
  COL( "IsNaF",          bool,     IsNaF)
  COL( "Width\t",        Float_t,  Width)
  COL( "WidthUnused",    Float_t,  WidthUnusedHits)
  COL( "Dist2TileBorder",Float_t, DistanceToTileBorder)
END_DEBUG_TABLE

}

}
