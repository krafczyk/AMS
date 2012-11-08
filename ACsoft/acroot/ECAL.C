#include "ECAL.h"

namespace AC {

BEGIN_DEBUG_OUTPUT(ECAL)
  DUMP(Showers)
END_DEBUG_OUTPUT

void ECAL::Clear(Option_t *) {

  fShowers.clear();
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const ECAL& object) {

  stream << object.fShowers;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, ECAL& object) {

  stream >> object.fShowers;
  return stream;
}

}
