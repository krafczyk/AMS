#include "MCEventGenerator.h"
#include "Tools.h"

namespace AC {

BEGIN_DEBUG_TABLE(MCEventGenerator)
  COL( "ID\t",        UShort_t, ID)
  COL( "Parent",      UShort_t, MotherParticle)
  COL( "Process",   	MCProcess,  Process)
  COL( "p [GeV/c]\t",   Float_t,  Momentum)
  COL( "m [GeV/c]\t",   Float_t,  Mass)
  COL( "Z\t",           Float_t,  Charge)
  COL( "X0 [cm]\t",     Float_t,  X0)
  COL( "Y0 [cm]\t",     Float_t,  Y0)
  COL( "Z0 [cm]\t",     Float_t,  Z0)
  COL( "Theta [deg]", Float_t,  Theta)
  COL( "Phi [deg]",   Float_t,  Phi)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const MCEventGenerator& object) {

 stream << object.fID << object.fMotherParticle << (UChar_t)object.fProcess
        << object.fMomentum << object.fMass << object.fCharge
        << object.fX0 << object.fY0 << object.fZ0 << object.fTheta << object.fPhi;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, MCEventGenerator& object) {

  ShortArrayStream<2> shortStream(stream);
  object.fID = shortStream.read();
  object.fMotherParticle = shortStream.read();

  UChar_t Process;
  stream >> Process;
  object.fProcess = (MCProcess) Process;

  FloatArrayStream<8> floatStream(stream);
  object.fMomentum = floatStream.read();
  object.fMass = floatStream.read();
  object.fCharge = floatStream.read();
  object.fX0 = floatStream.read();
  object.fY0 = floatStream.read();
  object.fZ0 = floatStream.read();
  object.fTheta = floatStream.read();
  object.fPhi = floatStream.read();

  return stream;
}

}
