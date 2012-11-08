#include "MCTrack.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(MCTrack)
  COL( "VolName",     const Char_t*, VolumeName)
  COL( "X0 [cm]",     Float_t,   X0)
  COL( "Y0 [cm]",     Float_t,   Y0)
  COL( "Z0 [cm]",     Float_t,   Z0)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const MCTrack& object) {

  stream.writeRawData(object.fVolumeName, 5);
  stream << object.fX0 << object.fY0 << object.fZ0;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, MCTrack& object) {

  stream.readRawData(object.fVolumeName, 5);

  FloatArrayStream<3> floatStream(stream);
  object.fX0 = floatStream.read();
  object.fY0 = floatStream.read();
  object.fZ0 = floatStream.read();
  return stream;
}

}
