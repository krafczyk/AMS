#include "Tracker.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(Tracker)
  DUMP_POD("clusters", Clusters)
  DUMP(Tracks)
END_DEBUG_OUTPUT

void Tracker::Clear() {

  fClusters = 0;
  fTracks.clear();
}

}

}
