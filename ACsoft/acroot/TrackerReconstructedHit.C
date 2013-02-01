#include "TrackerReconstructedHit.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TrackerReconstructedHit)
  COL( "Layer",      Char_t,  Layer)
  COL( "X [cm]\t",   Float_t, X)
  COL( "Y [cm]\t",   Float_t, Y)
  COL( "SigX [ADC]", Float_t, SignalX)
  COL( "SigY [ADC]", Float_t, SignalY)
  COL( "ResY [mu]",  Short_t, ResidualY)
END_DEBUG_TABLE

}

}
