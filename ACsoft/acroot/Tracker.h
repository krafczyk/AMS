#ifndef Tracker_h
#define Tracker_h

#include "TrackerTrack.h"

namespace AC {

/** %Tracker data
  */
class Tracker {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of TrackerTrack objects */
  typedef Vector<TrackerTrack, 4> TracksVector;

  Tracker()
    : fClusters(0) {

  }

  /** Helper method dumping a %Tracker object to the console
    */
  void Dump() const;

  /** Clear Tracker to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

  /** Number of clusters
    */
  Short_t Clusters() const { return fClusters; }

  /** Accessors for the TrackerTrack objects.
    */
  const TracksVector& Tracks() const { return fTracks; }

private:
  Short_t fClusters;
  TracksVector fTracks;

  REGISTER_CLASS(Tracker)
};

}

#endif
