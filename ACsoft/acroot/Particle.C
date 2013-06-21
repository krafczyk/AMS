#include "Particle.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(Particle)
  COL( "SA-Shadow",  bool, IsInSolarArrayShadow)
  COL( "BH-TRKT",    bool, TrackerTrackIndexFromBetaH)
  COL( "BH-TRDV",    bool, TRDVTrackIndexFromBetaH)
  COL( "BH-ESHWR",   bool, ECALShowerIndexFromBetaH)
  COL( "GC-BT-Rig",  bool, GalacticCoordinatesFromRigidityBacktracing)
  COL( "GC-BT-Ene",  bool, GalacticCoordinatesFromEnergyBacktracing)
  COL( "GC-Orb",     bool, GalacticCoordinatesFromOrbit)
  NEWTABLE
  COL( "GalLongBTRig",           Float_t, GalacticLongitudeFromRigidityBacktracing)
  COL( "GalLatiBTRig",           Float_t, GalacticLatitudeFromRigidityBacktracing)
  COL( "GalLongBTEne",           Float_t, GalacticLongitudeFromEnergyBacktracing)
  COL( "GalLatiBTEne",           Float_t, GalacticLatitudeFromEnergyBacktracing)
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
