#ifndef TRD_h
#define TRD_h

#include "TRDHSegment.h"
#include "TRDHTrack.h"
#include "TRDRawHit.h"
#include "TRDVTrack.h"
#include "TRD-Streamer.h"

namespace ACsoft {

namespace AC {

/** Transition Radiation %Detector event data
  */
class TRD {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of TRDVTrack objects */
  typedef Vector<TRDVTrack, 30> VTracksVector;

  /** A vector of TRDHTrack objects */
  typedef Vector<TRDHTrack, 10> HTracksVector;

  /** A vector of TRDHSegment objects */
  typedef Vector<TRDHSegment, 40> HSegmentsVector;

  /** A vector of TRDRawHit objects */
  typedef Vector<TRDRawHit, 5250> RawHitsVector;

  AC_TRD_Variables

  /** Helper method dumping a %TRD object to the console
    */
  void Dump() const;

  /** Clear TRD to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Number of hits in %TRD.
    */
  Short_t NumberOfHits() const { return fNumberOfHits; }

  /** Accessors for the TRDVTrack objects.
    */
  const VTracksVector& VTracks() const { return fVTracks; }

  /** Accessors for the TRDHTrack objects.
    */
  const HTracksVector& HTracks() const { return fHTracks; }

  /** Accessors for the TRDHSegment objects.
    */
  const HSegmentsVector& HSegments() const { return fHSegments; }

  /** Accessors for the TRDRawHit objects.
    */
  const RawHitsVector& RawHits() const { return fRawHits; }

  /** Returns a sorted list of TRDRawHits.
    * The list is sorted by layer number, then ladder number, then straw number, and finally descending amplitude.
    * Note: DO NOT use this in performance sensitive areas, it copies the whole vector of TRDRawHits.
    * It should only be used for debugging and/or dumping.
    */
  RawHitsVector SortedRawHits() const;

private:
  REGISTER_CLASS(TRD)
};

}

}

#endif
