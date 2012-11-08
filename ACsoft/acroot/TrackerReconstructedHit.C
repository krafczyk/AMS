#include "TrackerReconstructedHit.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(TrackerReconstructedHit)
  COL( "Layer",      Char_t,  Layer)
  COL( "X [cm]\t",   Float_t, X)
  COL( "Y [cm]\t",   Float_t, Y)
  COL( "SigX [ADC]", Float_t, SignalX)
  COL( "SigY [ADC]", Float_t, SignalY)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TrackerReconstructedHit& object) {

  stream.writeRawData(&object.fLayer, 1);
  stream << object.fX << object.fY << object.fSignalX << object.fSignalY;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TrackerReconstructedHit& object) {

  stream.readRawData(&object.fLayer, 1);

  FloatArrayStream<4> floatStream(stream);
  object.fX = floatStream.read();
  object.fY = floatStream.read();
  object.fSignalX = floatStream.read();
  object.fSignalY = floatStream.read();
  return stream;
}

}
