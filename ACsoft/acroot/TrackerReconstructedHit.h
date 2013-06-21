#ifndef TrackerReconstructedHit_h
#define TrackerReconstructedHit_h

#include "Tools.h"
#include "TrackerReconstructedHit-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Tracker reconstructed hit data
  */
class TrackerReconstructedHit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_TrackerReconstructedHit_Variables

  /** Helper method dumping an TrackerReconstructedHit object to the console
    */
  void Dump() const;

  /** Ladder layer J Scheme number (1-9).
    */
  Char_t Layer() const { return fLayer; }
 
  /** X coordinate [cm].
    */ 
  Float_t X() const { return fX; }
  
  /** Y coordinate [cm].
    */
  Float_t Y() const { return fY; }

  /** TrackerTrack Charge in Layer of hit:  .
    */
  Float_t QLayerJ() const { return fabs(fQLayerJ); }

  /** Cluster Y residal [microns] wrt to std fit iTrTrackPar(0)
    */
  Short_t ResidualY() const { return fResidualY; }

  /** Signal in X and Y - stored in positive sign of fQLayerJ ?
    */
  bool IsXAndYSignal() const { return fQLayerJ > 0.; }

private:
  REGISTER_CLASS_WITH_TABLE(TrackerReconstructedHit)
};

}

}

#endif
