#include "MCEventGenerator.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(MCEventGenerator)
  COL( "ParticleID",  Short_t,   ParticleID)
  COL( "TrackID",     UShort_t,  TrackID)
  COL( "Parent",      UShort_t,  MotherParticle)
  COL( "Process",     MCProcess, Process)
  COL( "p [GeV/c]\t", Float_t,   Momentum)
  COL( "m [GeV/c]\t", Float_t,   Mass)
  COL( "Z\t",         Float_t,   Charge)
  COL( "X0 [cm]\t",   Float_t,   X0)
  COL( "Y0 [cm]\t",   Float_t,   Y0)
  COL( "Z0 [cm]\t",   Float_t,   Z0)
  COL( "Theta [deg]", Float_t,   Theta)
  COL( "Phi [deg]",   Float_t,   Phi)
END_DEBUG_TABLE

}

}
