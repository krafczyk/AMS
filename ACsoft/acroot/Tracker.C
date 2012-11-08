#include "Tracker.h"

namespace AC {

BEGIN_DEBUG_OUTPUT(Tracker)
  DUMP_POD("clusters", Clusters)
  DUMP(Tracks)
END_DEBUG_OUTPUT

void Tracker::Clear(Option_t *) {

  fClusters = 0;
  fTracks.clear();
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const Tracker& object) {

  stream << object.fClusters << object.fTracks;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, Tracker& object) {

  stream >> object.fClusters >> object.fTracks;
  return stream;
}

}
