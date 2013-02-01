#include "TRDRawHit.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TRDRawHit)
  COL( "Lay",          Short_t,         Layer)
  COL( "Lad",          Short_t,         Ladder)
  COL( "Tub",          Short_t,         Tube)
  COL( "E_depo [ADC]", Float_t,         DepositedEnergy)
  COL( "Mod",          Short_t,         Module)
  COL( "GCir",         Short_t,         GasCircuit)
  COL( "GGrp",         Short_t,         GasGroup)
END_DEBUG_TABLE

bool operator<(const TRDRawHit& left, const TRDRawHit& right) {

  if (left.Layer() < right.Layer())
    return true;
  if (left.Layer() > right.Layer())
    return false;
  if (left.Ladder() < right.Ladder())
    return true;
  if (left.Ladder() > right.Ladder())
    return false;
  if (left.Tube() < right.Tube())
    return true;
  if (left.Tube() > right.Tube())
    return false;
  return left.DepositedEnergy() > right.DepositedEnergy();
}

}

}
