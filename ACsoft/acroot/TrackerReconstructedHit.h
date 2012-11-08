#ifndef TrackerReconstructedHit_h
#define TrackerReconstructedHit_h

#include "Tools.h"

namespace AC {

/** %Tracker reconstructed hit data
  */
class TrackerReconstructedHit {
  WTF_MAKE_FAST_ALLOCATED;
public:
  TrackerReconstructedHit()
    : fLayer(0)
    , fX(0)
    , fY(0)
    , fSignalX(0)
    , fSignalY(0) {

  }

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

  /** Cluster X signal [ADC counts].
    * Returns the signal of X.
    */
  Float_t SignalX() const { return fSignalX; }

  /** Cluster Y signal [ADC counts].
    * Returns the signal of Y.
    */
  Float_t SignalY() const { return fSignalY; }

  /** Signal in X and Y?
    */
  bool IsXAndYSignal() const { return fSignalX>0. && fSignalY>0.; }

private:
  Char_t fLayer;    // TrRecHHit->GetLayerJ()    <0 if mult. unresolved
  Float_t fX;       // TrRecHHit->GetCoord().x()
  Float_t fY;       // TrRecHHit->GetCoord().y()
  Float_t fSignalX; // TrRecHHit->GetSignalCombination(0)
  Float_t fSignalY; // TrRecHHit->GetSignalCombination(1)

  REGISTER_CLASS_WITH_TABLE(TrackerReconstructedHit)
};

}

#endif
