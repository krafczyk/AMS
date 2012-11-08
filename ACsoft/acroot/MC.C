#include "MC.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_OUTPUT(MC)
  DUMP(EventGenerators)
  DUMP(Tracks)
  DUMP(TRDHits)
END_DEBUG_OUTPUT

void MC::Clear(Option_t *) {

  fEventGenerators.clear();
  fTracks.clear();
  fTRDHits.clear();
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const MC& object) {

  stream << object.fEventGenerators << object.fTracks << object.fTRDHits;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, MC& object) {

  stream >> object.fEventGenerators >> object.fTracks >> object.fTRDHits;
  return stream;
}

}
