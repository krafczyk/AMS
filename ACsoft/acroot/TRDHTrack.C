#include "TRDHTrack.h"

namespace AC {

BEGIN_DEBUG_TABLE(TRDHTrack)
  COL( "ChargesNew\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", ChargesNewVector, ChargesNew)
  NEWTABLE
  COL( "HSegIndex\t\t\t\t\t", HSegmentIndexVector, HSegmentIndex)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TRDHTrack& object) {

  stream << object.fStatus << object.fChargesNew << object.fHSegmentIndex;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TRDHTrack& object) {

  stream >> object.fStatus >> object.fChargesNew >> object.fHSegmentIndex;
  return stream;
}

}
