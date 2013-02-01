#include "ACCCluster.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(ACCCluster)
  COL( "Phi [deg]",    Float_t, Phi)
  COL( "Z [cm]\t",     Float_t, Z)
  COL( "Time [ns]",    Float_t, Time)
  COL( "Energy [MeV]", Float_t, Energy)
END_DEBUG_TABLE

}

}
