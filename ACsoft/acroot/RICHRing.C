#include "RICHRing.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(RICHRing)
  COL( "Hits",           UShort_t, NumberOfHits)
  COLE("Beta\t\t\t\t\t", Float_t,  Beta)
  COL( "X [cm]\t",       Float_t,  X)
  COL( "Y [cm]\t",       Float_t,  Y)
  COL( "Z [cm]\t",       Float_t,  Z)
  NEWTABLE
  COL( "Theta [deg]",    Float_t,  Theta)
  COL( "Phi [deg]",      Float_t,  Phi)
  COL( "Prob\t\t",       Float_t,  Probability)
  COL( "CEstim\t",       Float_t,  ChargeEstimate)
END_DEBUG_TABLE

}

}
