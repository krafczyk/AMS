#include "TOFCluster.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TOFCluster)
  COL( "Layer",                Int_t,   Layer)
  COL( "Bar",                  Int_t,   Bar)
  COL( "X [cm]\t",             Float_t, X)
  COL( "Y [cm]\t",             Float_t, Y)
  COL( "Z [cm]\t",             Float_t, Z)
  COLE("Time [s]\t\t\t\t\t\t", Float_t, Time)
  COL( "Energy [MeV]",         Float_t, Energy)
  COL( "Charge",               Float_t, Charge)
END_DEBUG_TABLE

}

}
