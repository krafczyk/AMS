#include "TRD.h"

#include <algorithm>

namespace AC {

BEGIN_DEBUG_OUTPUT(TRD)
  DUMP_POD("hits", NumberOfHits)
  DUMP(VTracks)
  DUMP(HTracks)
  DUMP(HSegments)
  DUMP(SortedRawHits)
END_DEBUG_OUTPUT

void TRD::Clear(Option_t *) {

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

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TRD& object) {

  stream << object.fNumberOfHits << object.fVTracks << object.fHTracks << object.fHSegments << object.fRawHits;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TRD& object) {

  stream >> object.fNumberOfHits >> object.fVTracks >> object.fHTracks >> object.fHSegments >> object.fRawHits;
  return stream;
}

}
