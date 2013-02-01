#include "TRDHTrack.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TRDHTrack)
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", ChargesNewVector, ChargesNew)
  NEWTABLE
  COL( "HSegIndex\t\t\t\t\t", HSegmentIndexVector, HSegmentIndex)
END_DEBUG_TABLE

}

}
