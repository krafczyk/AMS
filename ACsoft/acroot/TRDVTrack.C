#include "TRDVTrack.h"
#include "Statistics.hh"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TRDVTrack)
  COL( "ChiSquare",             Float_t,       ChiSquare)
  COL( "Theta [deg]",           Float_t,       Theta)
  COL( "Phi [deg]",             Float_t,       Phi)
  COL( "X [cm]\t",              Float_t,       X)
  COL( "Y [cm]\t",              Float_t,       Y)
  COL( "Z [cm]\t",              Float_t,       Z)
  COL( "Pattern",               UInt_t,        Pattern)
  NEWTABLE
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t",  ChargesNewVector, ChargesNew)
END_DEBUG_TABLE

ChargeAndError TRDVTrack::GetChargeAndErrorNew() const {

  Utilities::Moments moments = Utilities::CalculateMoments(fChargesNew);
  return ChargeAndError(moments.mean, moments.meanError);
}

}

}
