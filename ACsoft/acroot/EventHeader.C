#include "EventHeader.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(EventHeader)
  COL( "Rnd",               UChar_t,    Random)
  COL( "Event\t",           Int_t,      Event)
  COL( "Timestamp\t\t\t\t", TTimeStamp, TimeStamp)
  COL( "UTCTime\t\t\t\t",   Double_t,   UTCTime)
  COL( "MAGLat [deg]",      Float_t,    MagneticLatitude)
  COL( "MAGLon [deg]",      Float_t,    MagneticLongitude)
  COL( "MaxCutConeNeg",     Float_t,    MaxCutOffConeNegative)
  COL( "MaxCutConePos",     Float_t,    MaxCutOffConePositive)
  NEWTABLE
  COL( "ISSLat [deg]",    Float_t, ISSLatitude)
  COL( "ISSLon [deg]",    Float_t, ISSLongitude)
  COL( "ISSDst  [km]",    Float_t, ISSDistanceFromEarthCenter)
  COL( "ISSAltGrd [km]",  Float_t, ISSAltitude)
  COL( "ISSRol [deg]",    Float_t, ISSRoll)
  NEWTABLE
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
  fUTCTime = 0;
  fMagneticLatitude = 0;
  fMagneticLongitude = 0;
  fISSLatitude = 0;
  fISSLongitude = 0;
  fISSDistanceFromEarthCenter = 0;
  fISSRoll = 0;
  fISSPitch = 0;
  fISSYaw = 0;
  fISSVelocityLatitude = 0;
  fISSVelocityLongitude = 0;
}

/** calculates the distance between ISS and the center of earth's magentic dipole in km using the exccentric
  * dipole approximation with location values of the dipole obtained from IGRF2000.
  *
  * FIXME:  Move me somewhere else if i do not belong here. A copy is prepared in the CutoffCalculator class.
  *         If this place is fine just remove the FIXME and the copy.
  *
  * FIXME:  Test this some more, before implementing into the vertical stoermer cutoff.
  */
float AC::EventHeader::ISSDistanceFromDipoleCenter() const
{
  //location of the dipole seen from earth's center
  float xDipole = -401.86; // km
  float yDipole = 300.25; // km
  float zDipole = 200.61; // km

  float ISStoEarthCenter = AC::EventHeader::fISSDistanceFromEarthCenter / 100000;

  // calculate magnetic position of ISS in cartesian coordinates.
  float xISS = ISStoEarthCenter*sin(AC::EventHeader::fMagneticLatitude)*cos(AC::EventHeader::fMagneticLongitude);
  float yISS = ISStoEarthCenter*sin(AC::EventHeader::fMagneticLatitude)*sin(AC::EventHeader::fMagneticLongitude);
  float zISS = ISStoEarthCenter*cos(AC::EventHeader::fMagneticLatitude);

  // calculate the distance between the two points.
  float DistanceFromDipoleCenter = sqrt( (xDipole-xISS)*(xDipole -xISS) + (yDipole-yISS)*(yDipole-yISS) + (zDipole-zISS)*(zDipole-zISS) );

  return DistanceFromDipoleCenter;
}

}

}
