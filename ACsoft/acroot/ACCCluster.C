#include "ACCCluster.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(ACCCluster)
  COL( "Phi [deg]",    Float_t, Phi)
  COL( "Z [cm]\t",     Float_t, Z)
  COL( "Time [ns]",    Float_t, Time)
  COL( "Energy [MeV]", Float_t, Energy)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const ACCCluster& object) {

  stream << object.fStatus << object.fPhi << object.fZ << object.fTime << object.fEnergy;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, ACCCluster& object) {

  stream >> object.fStatus;

  FloatArrayStream<4> floatStream(stream);
  object.fPhi = floatStream.read();
  object.fZ = floatStream.read();
  object.fTime = floatStream.read();
  object.fEnergy = floatStream.read();
  return stream;
}

}
