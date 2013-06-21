#include "MCTRDCluster.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(MCTRDCluster)
  COL( "ID",            Short_t,          ID)
  COL( "LLT\t",         UShort_t,         LLT)
  COL( "Loss",          EnergyLossVector, EnergyLoss)
  COL( "Ekin [GeV]\t",  Float_t,          KineticEnergy)
  COL( "Edep [GeV]\t",  Float_t,          DepositedEnergy)
  COL( "Step [cm]\t",   Float_t,          StepSize)
END_DEBUG_TABLE

}

}
