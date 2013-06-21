#include "TrackerReconstructedHit.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TrackerReconstructedHit)
  COL( "Layer",      Char_t,  Layer)
  COL( "X [cm]\t",   Float_t, X)
  COL( "Y [cm]\t",   Float_t, Y)
  COL( "QLayJ [e]",  Float_t, QLayerJ)
  COL( "ResY [mu]",  Short_t, ResidualY)
  COL( "IsXYhit",    bool,    IsXAndYSignal)
END_DEBUG_TABLE

}

}
