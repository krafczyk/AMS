#include "TRD.h"

#include <algorithm>

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(TRD)
  DUMP_POD("hits", NumberOfHits)
  DUMP(VTracks)
  DUMP(HTracks)
  DUMP(HSegments)
  DUMP(SortedRawHits)
END_DEBUG_OUTPUT

void TRD::Clear() {

  fNumberOfHits = 0;
  fVTracks.clear();
  fHTracks.clear();
  fHSegments.clear();
  fRawHits.clear();
}

TRD::RawHitsVector TRD::SortedRawHits() const {

  // This is slow, but it should be no issue, as it's only used for dumping events.
  RawHitsVector copy = RawHits();
  std::sort(copy.begin(), copy.end());
  return copy;
}

}

}
