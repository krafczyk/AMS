#include "RICH.h"

namespace AC {

BEGIN_DEBUG_OUTPUT(RICH)
  DUMP(Rings)
END_DEBUG_OUTPUT

void RICH::Clear(Option_t *) {

  fRings.clear();
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const RICH& object) {

  stream << object.fRings;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, RICH& object) {

  stream >> object.fRings;
  return stream;
}

}
