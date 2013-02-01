#include "TrackerTrackFit.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(TrackerTrackFit)
  COL( "Algo",             Int_t,   Algorithm)
  COL( "Patt",             Int_t,   Pattern)
  COL( "Refit",            Int_t,   Refit)
  COL( "Rigi [GV]",        Float_t, Rigidity)
  COL( "InvRigErr [1/GV]", Float_t, InverseRigidityError)
  COL( "Chi2NormX  ",      Float_t, ChiSquareNormalizedX)
  COL( "Chi2NormY  ",      Float_t, ChiSquareNormalizedY)
  COL( "L1Tht [deg]",      Float_t, ThetaLayer1)
  COL( "L1Phi [deg]",      Float_t, PhiLayer1)
  COL( "L1X [cm] ",        Float_t, XLayer1)
  COL( "L1Y [cm] ",        Float_t, YLayer1)
  NEWTABLE
  COL( "TRDUX [cm]",       Float_t, XTRDUpper)
  COL( "TRDUY [cm]",       Float_t, YTRDUpper)
  COL( "TRDCX [cm]",       Float_t, XTRDCenter)
  COL( "TRDCY [cm]",       Float_t, YTRDCenter)
  COL( "TRDLX [cm]",       Float_t, XTRDLower)
  COL( "TRDLY [cm]",       Float_t, YTRDLower)
  COL( "TOFUTht [deg]",    Float_t, ThetaTOFUpper)
  COL( "TOFUPhi [deg]",    Float_t, PhiTOFUpper)
  COL( "TOFUX [cm]",       Float_t, XTOFUpper)
  COL( "TOFUY [cm]",       Float_t, YTOFUpper)
  NEWTABLE
  COL( "L56X [cm] ",       Float_t, XLayer56)
  COL( "L56Y [cm] ",       Float_t, YLayer56)
  COL( "RICHTht [deg]",    Float_t, ThetaRICH)
  COL( "RICHPhi [deg]",    Float_t, PhiRICH)
  COL( "RICHX [cm]",       Float_t, XRICH)
  COL( "RICHY [cm]",       Float_t, YRICH)
  COL( "L9Tht [deg]",      Float_t, ThetaLayer9)
  COL( "L9Phi [deg]",      Float_t, PhiLayer9)
  COL( "L9X [cm] ",        Float_t, XLayer9)
  COL( "L9Y [cm] ",        Float_t, YLayer9)
END_DEBUG_TABLE

}

}
