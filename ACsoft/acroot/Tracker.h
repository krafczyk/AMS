#ifndef Tracker_h
#define Tracker_h

#include "TrackerTrack.h"
#include "Tracker-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Tracker data
  */
class Tracker {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of TrackerTrack objects */
  typedef Vector<TrackerTrack, 4> TracksVector;

  AC_Tracker_Variables

  /** Helper method dumping a %Tracker object to the console
    */
  void Dump() const;

  /** Clear Tracker to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Number of clusters
    */
  Short_t Clusters() const { return fClusters; }

  /** Accessors for the TrackerTrack objects.
    */
  const TracksVector& Tracks() const { return fTracks; }

private:
  REGISTER_CLASS(Tracker)
};

}

}

#endif
