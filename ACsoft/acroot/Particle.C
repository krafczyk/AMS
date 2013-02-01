#include "Particle.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(Particle)
  COL( "SA-Shadow",  bool, IsInSolarArrayShadow)
  COL( "BH-TRKT",    bool, TrackerTrackIndexFromBetaH)
  COL( "BH-TRDV",    bool, TRDVTrackIndexFromBetaH)
  COL( "BH-ESHWR",   bool, ECALShowerIndexFromBetaH)
  COL( "GC-BT",      bool, GalacticCoordinatesFromBacktracing)
  COL( "GC-Orb",     bool, GalacticCoordinatesFromOrbit)
  NEWTABLE
  COL( "StoermerCutoffNegative", Float_t, StoermerCutoffNegative)
  COL( "StoermerCutoffPositive", Float_t, StoermerCutoffPositive)
  COL( "   GalLongBT",           Float_t, GalacticLongitudeFromBacktracing)
  COL( "   GalLatiBT",           Float_t, GalacticLatitudeFromBacktracing)
  COL( "  GalLongOrb",           Float_t, GalacticLongitudeFromOrbit)
  COL( "  GalLatiOrb",           Float_t, GalacticLatitudeFromOrbit)
  COL( "BTNegRigStat",           Int_t,   BacktracingStatusNegativeRigidity)
  COL( "BTPosRigStat",           Int_t,   BacktracingStatusPositiveRigidity)
  COL( "BTNegEneStat",           Int_t,   BacktracingStatusNegativeEnergy)
  COL( "BTPosEneStat",           Int_t,   BacktracingStatusPositiveEnergy)
  NEWTABLE
  COL( "TOFB",  Short_t, TOFBetaIndex)
  COL( "TRDV",  Short_t, TRDVTrackIndex)
  COL( "TRDH",  Short_t, TRDHTrackIndex)
  COL( "TRKT",  Short_t, TrackerTrackIndex)
  COL( "RIRI",  Short_t, RICHRingIndex)
  COL( "ESHWR", Short_t, ECALShowerIndex)
END_DEBUG_TABLE

}

}
