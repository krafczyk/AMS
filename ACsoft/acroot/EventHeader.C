#include "EventHeader.h"

namespace AC {

BEGIN_DEBUG_TABLE(EventHeader)
  COL( "Rnd",               UChar_t,    Random)
  COL( "Event\t",           Int_t,      Event)
  COL( "Timestamp\t\t\t\t", TTimeStamp, TimeStamp)
  COL( "GLat [deg]",        Float_t,    GalacticLatitude)
  COL( "GLon [deg]",        Float_t,    GalacticLongitude)
  COL( "MAGLat [deg]",      Float_t,    MagneticLatitude)
  COL( "MAGLon [deg]",      Float_t,    MagneticLongitude)
  NEWTABLE
  COL( "ISSLat [deg]",    Float_t, ISSLatitude)
  COL( "ISSLon [deg]",    Float_t, ISSLongitude)
  COL( "ISSAlt [km]",     Float_t, ISSAltitude)
  COL( "ISSAltGrd [km]",  Float_t, ISSAltitudeAboveGround)
  COL( "ISSRol [deg]",    Float_t, ISSRoll)
  COL( "ISSYaw [deg]",    Float_t, ISSYaw)
  COL( "ISSPit [deg]",    Float_t, ISSPitch)
  COL( "ISSVelLat [deg]", Float_t, ISSVelocityLatitude)
  COL( "ISSVelLon [deg]", Float_t, ISSVelocityLongitude)
END_DEBUG_TABLE

void EventHeader::Clear() {

  fStatus = 0;
  fRandom = 0;
  fMCRandomSeed1 = 0;
  fMCRandomSeed2 = 0;
  fEvent = 0;
  fTimeStamp = TTimeStamp();
  fGalacticLatitude = 0;
  fGalacticLongitude = 0;
  fMagneticLatitude = 0;
  fMagneticLongitude = 0;
  fISSLatitude = 0;
  fISSLongitude = 0;
  fISSAltitude = 0;
  fISSRoll = 0;
  fISSPitch = 0;
  fISSYaw = 0;
  fISSVelocityLatitude = 0;
  fISSVelocityLongitude = 0;
}

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const EventHeader& object) {

  stream << (qint64) object.fStatus << object.fRandom << object.fMCRandomSeed1
         << object.fMCRandomSeed2 << object.fEvent << object.fTimeStamp
         << object.fGalacticLatitude << object.fGalacticLongitude << object.fMagneticLatitude << object.fMagneticLongitude
         << object.fISSLatitude << object.fISSLongitude << object.fISSAltitude << object.fISSRoll
         << object.fISSPitch << object.fISSYaw << object.fISSVelocityLatitude << object.fISSVelocityLongitude;

  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, EventHeader& object) {

  qint64 status;
  stream >> status >> object.fRandom >> object.fMCRandomSeed1 >> object.fMCRandomSeed2
         >> object.fEvent >> object.fTimeStamp;

  object.fStatus = status;

  FloatArrayStream<12> floatStream(stream);
  object.fGalacticLatitude = floatStream.read();
  object.fGalacticLongitude = floatStream.read();
  object.fMagneticLatitude = floatStream.read();
  object.fMagneticLongitude = floatStream.read();
  object.fISSLatitude = floatStream.read();
  object.fISSLongitude = floatStream.read();
  object.fISSAltitude = floatStream.read();
  object.fISSRoll = floatStream.read();
  object.fISSPitch = floatStream.read();
  object.fISSYaw = floatStream.read();
  object.fISSVelocityLatitude = floatStream.read();
  object.fISSVelocityLongitude = floatStream.read();

  return stream;
}

}
