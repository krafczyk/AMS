#include "TOFCluster.h"

namespace AC {

BEGIN_DEBUG_TABLE(TOFCluster)
  COL( "Layer",                Int_t,   Layer)
  COL( "Bar",                  Int_t,   Bar)
  COL( "X [cm]\t",             Float_t, X)
  COL( "Y [cm]\t",             Float_t, Y)
  COL( "Z [cm]\t",             Float_t, Z)
  COLE("Time [s]\t\t\t\t\t\t", Float_t, Time)
  COL( "Energy [MeV]",         Float_t, Energy)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TOFCluster& object) {

  stream << object.fStatus << object.fLayerBar << object.fX << object.fY << object.fZ << object.fTime << object.fTimeError << object.fEnergy;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TOFCluster& object) {

  stream >> object.fStatus >> object.fLayerBar;

  FloatArrayStream<6> floatStream(stream);
  object.fX = floatStream.read();
  object.fY = floatStream.read();
  object.fZ = floatStream.read();
  object.fTime = floatStream.read();
  object.fTimeError = floatStream.read();
  object.fEnergy = floatStream.read();
  return stream;
}

}
