#include "Utilities.hh"
#include <iostream>
#include <TMath.h>
#include <TMatrixDEigen.h>
namespace Utilities {

std::vector<double> GenerateLogBinning(int nBinLog, double Tmin, double Tmax) {

  std::vector<double> xMin;
  xMin.push_back(Tmin);
  for (int i=0; i<nBinLog; i++) {
    double xMax = exp(log(xMin[i])+(log(Tmax)-log(Tmin))/nBinLog);
    xMin.push_back(xMax);
  }
  return xMin;
}

TVector3 TransformAMS02toMAG(const double &yaw, const double &pitch, const double &roll, const double &altitude, const double &phi, const double &theta, const double &velPhi, const double &velTheta, const TVector3 &inputVector)
{
  TVector3 event = inputVector;
  // Particle direction is given as an input: inputVector
  // needed ISS information: position in LVLH (yaw, pitch, roll), position in GTOD (altitude, theta, phi), and velocity direction in GTOD (velocity theta, velocity phi)
  // all vectors are/should be normalised

  // The Space Station Coordinate system is a body-fixed right-handed Catesian system.
  // The X_ISS axis is parallel to the longitudinal axis of the module cluster. The forward direction (+X_ISS) is called Ram side. The backward direction (-X_ISS) is called Wake side.
  // The Y_ISS axis is parallel to the axis of the module cluster. The positive direction (+Y_ISS) is called Starboard, the negative direction(-Y_ISS) is called Port.
  // The Z_ISS axis points to the Nadir direction (+Z_ISS). The zenith is the negative side (-Z_ISS).

  // AMS-02 is located on the Starboard side of the ISS.
  // AMS-02 has its own Cartesian right-handed coordinate system.
  // The origin is located at the geometric center of the tracker detector.
  // The X_AMS axis points towards the port direction (=-Y_ISS).
  // The Y_AMS axis points towards the wake side (=-X_ISS).
  // The Z_AMS axis points towards the zenith (=-Z_ISS).
  // Initialise the AMS02 reference frame
  TMatrix AMS02toISS(3, 3);
  AMS02toISS(0, 0) = 0.;
  AMS02toISS(0, 1) = -1.;
  AMS02toISS(0, 2) = 0.;
  AMS02toISS(1, 0) = -1.;
  AMS02toISS(1, 1) = 0.;
  AMS02toISS(1, 2) = 0.;
  AMS02toISS(2, 0) = 0.;
  AMS02toISS(2, 1) = 0.;
  AMS02toISS(2, 2) = -1.;

  // Transform event direction from AMS to ISS system
  // AMS is tilted of about 12 degrees with respect to the axis perpendicular to the ISS structure
  event = AMS02toISS*inputVector;
  event.RotateX(-12.*TMath::DegToRad());

  // Local Vertical Local Horizontal (LVLH)
  // The LVLH frame is a quasi-inertial, right-handed Cartesian system with the origin located in the center of mass of the object (i.e., the ISS).
  // To define a relationship between the ISS reference frame and the flight orientation is is necessaR_Y to give three attitude angles called Yaw, Pich, and Roll, around the Z_ISS, Y_ISS, and X_ISS axes respectively.
  // To pass from the LVLH reference system to the ISS one, one has to apply the following sequence: Yaw, Pitch, Roll.

  // Apply ISS to LVLH transformation
  event.RotateX(roll);
  event.RotateY(pitch);
  event.RotateZ(yaw);

  // Greenwich True Of Date (GTOD)
  // The GTOD system is a Geocentric, Earth-fixed frame.
  // The X_GTOD axis points towards the GTOD meridian and it lies on the Earth equatorial plane.
  // The Y_GTOD axis lies on the equatorial plane.
  // The Z_GTOD axis overlaps with the Earth rotational axis pointing to the North pole.

  // The LVLH is defined by means of the ISS radial position vector R and its velocity vector V given in GTOD.
	// The Z_GTOD lies along the geocentric radius vector to the vehicle R and is positive towards the centre of Earth.
	// The X_GTOD lies on the vertical orbital plane to the Z_GTOD axis and is positive in the of the vehicle motion.
  // The Y_GTOD is the cross product of the X_GTOD and Z_GTOD axes (Y_GTOD = Z_GTOD x X_GTOD)

  // First construct GTOD to LVLH matrix and then invert it
  TMatrix LVLHtoGTOD(3, 3);
  // Calculate the components of the R and V vectors
  TVector3 R, V;
  R.SetMagThetaPhi(1., TMath::Pi()*.5 + theta, phi + TMath::Pi());
  V.SetMagThetaPhi(1., TMath::Pi()*.5 - velTheta, velPhi);

  // Calculate each axis
  // new Z-axis: R pointing to the centre of Eearth
  LVLHtoGTOD(2,0) = R(0);
  LVLHtoGTOD(2,1) = R(1);
  LVLHtoGTOD(2,2) = R(2);

  // new X-axis: V
  LVLHtoGTOD(0,0) = V(0);
  LVLHtoGTOD(0,1) = V(1);
  LVLHtoGTOD(0,2) = V(2);

  // new Y-axis: cross product of R and V
  TVector3 ZxX = R.Cross(V);
  LVLHtoGTOD(1, 0) = ZxX(0);
  LVLHtoGTOD(1, 1) = ZxX(1);
  LVLHtoGTOD(1, 2) = ZxX(2);

  // now invert
  LVLHtoGTOD.Invert();

  // LVLH to GTOD
  event = LVLHtoGTOD*event;

  // GTOD to MAG
  // The MAG frame is shifted (~570 km) and rotated (~11 degrees) with respect to the GTOD frame
  // We apply first the shift and then rotation

  // Needed information about the geomagnetic field, approximated by a dipole field
  const double magneticNpole_theta = -1.399310;       // magnetic centred north pole latitude 2012 (rad)
  const double magneticNpole_phi = 1.877269;          // magnetic centred north pole longitude 2012 (rad)
  const double magneticDpolecenter_theta = 0.395086;	// dipole centre latitude 2012 (rad)
  const double magneticDpolecenter_phi = 2.639650;	 	// dipole centre longitude 2012 (rad)
  const double magneticDshift = 569.779321;           // dipole centre shift respect earth's center (km) 2012

  // Calculate magnetic Dipole center in GTOD
  TVector3 Dipole;
  Dipole.SetMagThetaPhi(1., TMath::Pi()*0.5 - magneticDpolecenter_theta, magneticDpolecenter_phi);

  // shift the event position from Earth centre to Dipole centre in GTOD
  // take ISS altitude. The error introduced with the altitude difference between ISS and AMS should be negligeable.
  double relative_X = altitude*event(0) - magneticDshift*Dipole(0);
  double relative_Y = altitude*event(1) - magneticDshift*Dipole(1);
  double relative_Z = altitude*event(2) - magneticDshift*Dipole(2);

  // normalise vector
  double length = sqrt(relative_X*relative_X + relative_Y*relative_Y + relative_Z*relative_Z);
  TVector3 EventFromDipoleCentre(relative_X/length, relative_Y/length, relative_Z/length);
  
  // Calculate magnetic North Pole in GTOD (We have to take into account that the geographic latitude is pi/2 - the cylindrical theta)
  TVector3 Npole;
  Npole.SetMagThetaPhi(1., TMath::Pi()*0.5 - magneticNpole_theta, magneticNpole_phi);

  // The geomagnetic coordinate system (MAG) defined so that its Z axis is parallel to the magnetic dipole axis.
  // The Y-axis of this system is perpendicular to the geographic poles such that if D is the dipole position and S is the south pole Y = D x S.
  // The X-axis completes a right-handed orthogonal set.

  TMatrix GTODtoMAG(3, 3);
  // new Z axis: parallel to dipole axis
  GTODtoMAG(2, 0) = Npole(0);
  GTODtoMAG(2, 1) = Npole(1);
  GTODtoMAG(2, 2) = Npole(2);

  // new Y axis: cross product of new Z axis and position of the South Pole (0, 0, -1)
  TVector3 geographicSpole;
  geographicSpole.SetMagThetaPhi(1., 180., 0.);
  TVector3 DxS = Npole.Cross(geographicSpole);
  DxS.SetMag(1.);

  GTODtoMAG(1, 0) = DxS(0);
  GTODtoMAG(1, 1) = DxS(1);
  GTODtoMAG(1, 2) = DxS(2);

  // X axis: cross product of new axis Y and Z
  GTODtoMAG(0,0) = GTODtoMAG(1,1)*GTODtoMAG(2,2) - GTODtoMAG(1,2)*GTODtoMAG(2,1);
  GTODtoMAG(0,1) = GTODtoMAG(1,2)*GTODtoMAG(2,0) - GTODtoMAG(1,0)*GTODtoMAG(2,2);
  GTODtoMAG(0,2) = GTODtoMAG(1,0)*GTODtoMAG(2,1) - GTODtoMAG(1,1)*GTODtoMAG(2,0);

  // The event direction in the MAG frame is the scalar product of MAG unit vectors and the shifted event direction
  TVector3 eventInMAG = GTODtoMAG*EventFromDipoleCentre;

  return eventInMAG;
}

}
