#include "CoordinateSystems.hh"

#include "EventHeader.h"
#include "FrameTrans.h"

namespace ACsoft {

Utilities::CoordinateSystems::CoordinateSystems()
{
}

Utilities::CoordinateSystems::~CoordinateSystems()
{
}

void Utilities::CoordinateSystems::GetGalacticCoordinates(const AC::EventHeader& header, double theta, double phi,  double& galacticLongitude, double& galacticLatitude) const
{
  double UTCtime = header.UTCTimeStamp().AsDouble();

  double yaw = header.ISSYaw()*TMath::DegToRad();
  double pitch = header.ISSPitch()*TMath::DegToRad();
  double roll = header.ISSRoll()*TMath::DegToRad();

  double radS = header.ISSDistanceFromEarthCenter()*1000*100; // km -> cm
  double thetaS = header.ISSLatitude()*TMath::DegToRad();
  double phiS = header.ISSLongitude()*TMath::DegToRad();

  double velS = 1.0;
  double velTheta = header.ISSVelocityLatitude()*TMath::DegToRad();
  double velPhi = header.ISSVelocityLongitude()*TMath::DegToRad();

  double ams_x = cos(phi)*sin(theta);
  double ams_y = sin(phi)*sin(theta);
  double ams_z = cos(theta);

  double ypr[3] = {yaw, pitch, roll};
  double PosISS[3] = {radS, phiS, thetaS};
  double VelISS[3] = {velS, velPhi, velTheta};

  get_ams_l_b_fromGTOD(ams_x, ams_y, ams_z, galacticLongitude, galacticLatitude, PosISS, VelISS, ypr, UTCtime);
}

void Utilities::CoordinateSystems::GetEquatorialCoordinates(const AC::EventHeader& header, double theta, double phi,  double& rightAscension, double& declination) const
{
  double UTCtime = header.UTCTimeStamp().AsDouble();

  double yaw = header.ISSYaw()*TMath::DegToRad();
  double pitch = header.ISSPitch()*TMath::DegToRad();
  double roll = header.ISSRoll()*TMath::DegToRad();

  double radS = header.ISSDistanceFromEarthCenter()*1000*100; // km -> cm
  double thetaS = header.ISSLatitude()*TMath::DegToRad();
  double phiS = header.ISSLongitude()*TMath::DegToRad();

  double velS = 1.0;
  double velTheta = header.ISSVelocityLatitude()*TMath::DegToRad();
  double velPhi = header.ISSVelocityLongitude()*TMath::DegToRad();

  double ams_x = cos(phi)*sin(theta);
  double ams_y = sin(phi)*sin(theta);
  double ams_z = cos(theta);

  double ypr[3] = {yaw, pitch, roll};
  double PosISS[3] = {radS, phiS, thetaS};
  double VelISS[3] = {velS, velPhi, velTheta};

  get_ams_ra_dec_fromGTOD(ams_x, ams_y, ams_z, rightAscension, declination, PosISS, VelISS, ypr, UTCtime);
}

void Utilities::CoordinateSystems::GalacticToEquatorial(double l, double b, double& rightAscension, double& declination) const
{
  l *= TMath::DegToRad();
  b *= TMath::DegToRad();

  FT_Gal2Equat(l,b);

  rightAscension = l * TMath::RadToDeg();
  declination = b * TMath::RadToDeg();
}

void Utilities::CoordinateSystems::EquatorialToGalactic(double rightAscension, double declination, double& l, double& b) const
{
  rightAscension *= TMath::RadToDeg();
  declination *= TMath::RadToDeg();

  FT_Equat2Gal(rightAscension, declination);

  l = rightAscension * TMath::DegToRad();
  b = declination * TMath::DegToRad();
}

}
