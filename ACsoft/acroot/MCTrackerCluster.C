#include "MCTrackerCluster.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(MCTrackerCluster)
  COL( "ID",         Short_t, ID)
  COL( "Edep [MeV]", Float_t, DepositedEnergy)
  COL( "Mom [GeV]",  Float_t, MCParticleMomentum)
  COL( " X [cm]",    Float_t, X)
  COL( " Y [cm]",    Float_t, Y)
  COL( " Z [cm]",    Float_t, Z)
END_DEBUG_TABLE

}

}
