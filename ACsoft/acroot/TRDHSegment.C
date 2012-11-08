#include "TRDHSegment.h"

namespace AC {

BEGIN_DEBUG_TABLE(TRDHSegment)
  COL( "RawHitIndex\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", RawHitIndexVector, RawHitIndex)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TRDHSegment& object) {

  stream << object.fRawHitIndex;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TRDHSegment& object) {

  stream >> object.fRawHitIndex;
  return stream;
}

}
