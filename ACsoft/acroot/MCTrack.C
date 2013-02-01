#include "MCTrack.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(MCTrack)
  COL( "VolName",     const Char_t*, VolumeName)
  COL( "X0 [cm]",     Float_t,       X0)
  COL( "Y0 [cm]",     Float_t,       Y0)
  COL( "Z0 [cm]",     Float_t,       Z0)
END_DEBUG_TABLE

}

}
