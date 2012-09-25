/////////////////////
///   Library to convert from/to AMS reference 
///   it is supposed that the coordinate in AMS frame is of arrival direction.
//////////////////////
 /*  
 *  Authors :
 *           SDT - Stefano Della Torre - INFN Milano Bicocca - stefano.dellatorre@mib.infn.it
 *           AL  - Alexey Lebedev - MIT - alexey.lebedev@cern.ch     
 *  Date: Aug 2012 - first version (SDT)
 *  ---- report here main changes ----
 *  SDT 16 aug 2012 - add FT_modulus function
 *  SDT 11 sept 2012 - implements few correction from AL
 *  SDT 11 sept 2012 - correct output range of galactic to celestial (and Viceversa) longitude (and RA)
 *  SDT 18 sept 2012 - correct typos err in FT_Gal2Equat and FT_Equat2Gal
 *  SDT 22 sept 2012 - add starTracker rotations function
 *  SDT 22 sept 2012 - modify for user functions in order to align angle definition with those in AMSsoftware
 *  SDT 22 sept 2012 - add get_ams_ra_dec_from_ALTEC_INTL that use attitude angle in INTL frame from altec.
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <math.h>
using namespace std;



//~------------ TO BE USED BY USER ------------

//~============================================================================

///    generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
/// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
/// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
/// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
/// VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
/// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
/// xtime     = UTC time (in unix format)
int get_ams_ra_dec_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double &ra, double &dec, double PosISS[3], double VelISS[2], double ypr[3], double xtime);

///  generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
/// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
/// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
/// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
/// VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
/// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
/// xtime     = UTC time (in unix format)
int get_ams_l_b_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double & l, double &b, double PosISS[3], double VelISS[2], double ypr[3], double xtime);

/// SDT(sept2012) - trasformation from AMS frame to J2000 frame using the star Tracker data
/// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
/// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
/// CamID               = Identifier of Camera, 0(Port Camera), 1(starBoard)
/// ST_RA               = J2000 Right Ascension of Star Tracker pointing direction in degree;
/// ST_dec              = J2000 Declination of Star Tracker pointing direction in degree; 
/// ST_Orient           = orientation of top of the CCD wrt the celestial north in degree.
int get_ams_ra_dec_from_StarTracker(double AMS_x, double AMS_y,double AMS_z, double & ra, double &dec, int CamID, double ST_RA, double ST_dec, double ST_Orient);

/// trasformation from AMS frame to galactic frame using the star Tracker data
/// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
/// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
/// CamID               = Identifier of Camera, 0(Port Camera), 1(starBoard)
/// ST_RA               = J2000 Right Ascension of Star Tracker pointing direction in degree;
/// ST_dec              = J2000 Declination of Star Tracker pointing direction in degree; 
/// ST_Orient           = orientation of top of the CCD wrt the celestial north in degree.
int get_ams_l_b_from_StarTracker(double AMS_x, double AMS_y,double AMS_z, double & l, double &b, int CamID, double ST_RA, double ST_dec, double ST_Orient);

  
/// trasformation from AMS frame to J2000 frame using the star Tracker data
/// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
/// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
/// ISSyaw,ISSpitch,ISSroll= attitude Information from ALTEC INTL data, i.e. the attitude with respect the J2000 frame, in degree.
int get_ams_ra_dec_from_ALTEC_INTL(double AMS_x, double AMS_y,double AMS_z, double & ra, double &dec, double ISSyaw,double ISSpitch, double ISSroll);

// ** **************** COMMON *******************************/

void	FT_Cart2Angular(double x, double y, double z, double& r, double& theta, double& phi);
void	FT_Angular2Cart(double r, double theta, double phi, double& x, double& y, double& z );
double  FT_Modulus(double arg1, double arg2);



// ** **************** STAR TRACKER *************************/

 
/// transformation of a point in XYZ from AMS-02 frame into ECI-J2000 celestial frame
/// using information from StarTracker. 
/// x,y,z: as input are in AMS-02 frame,
///        as output are in J2000 frame.
/// CamID: Identifier of Camera, 0(Port Camera), 1(starBoard)
/// ST_RA: J2000 Right Ascension of Star Tracker pointing direction;
/// ST_dec: J2000 Declination of Star Tracker pointing direction; 
void ST_AMS2ECI(double& x, double& y, double& z,int CamID, double ST_RA, double ST_dec, double ST_Orient);
 


/// transformation of a point in XYZ from  ECI-J2000 celestial  frame AMS-02 intoframe
/// using information from StarTracker
/// x,y,z: as input are in J2000 frame,
///        as output are in AMS-02 frame.
/// CamID: Identifier of Camera, 0(Port Camera), 1(starBoard)
/// ST_RA: J2000 Right Ascension of Star Tracker pointing direction;
/// ST_dec: J2000 Declination of Star Tracker pointing direction; 
void ST_ECI2AMS(double& x, double& y, double& z,int CamID, double ST_RA, double ST_dec, double ST_Orient);
 
/// transformation of a point in XYZ from ECI frame to Plane tangent to celestial frame (i.e. plane of Star Tracker CCD)
/// using the zenit direction (in ECI) of such plane (defind by ST_RA and ST_dec, pointing direction of starTracker camera)
/// x,y,z: as input are in ECI frame,
///        as output are in Tangent Plane frame
/// ST_RA: J2000 Right Ascension of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker in radiant
/// ST_dec: J2000 Declination of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker in radiant
void ST_ECI2tangPlane(double& x, double& y, double& z, double ST_RA, double ST_dec);

/// transformation of a point in XYZ from "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD) into ECI frame
/// using the zenit direction (in ECI) of such plane (defind by ST_RA and ST_dec, pointing direction of starTracker camera)
/// x,y,z: as input are in Tangent Plane frame,
///        as output are in ECI frame
/// ST_RA: J2000 Right Ascension of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker in radiant
/// ST_dec: J2000 Declination of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker in radiant
void ST_tangPlane2ECI(double& x, double& y, double& z, double ST_RA, double ST_dec);

/// transformation of a point in XYZ from "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD) into CCD frame
/// using the Orientation direction (in ECI) of such plane (defind by ST_Orient, i.e. orientation of top of the CCD wrt the celestial north)
/// x,y,z: as input are in Tangent Plane frame,
///        as output are in CCD frame
/// ST_Orient: orientation of top of the CCD wrt the celestial north in radiant.
void ST_tangPlane2CAM(double& x, double& y, double& z, double ST_Orient);

/// transformation of a point in XYZ from  CCD frame into "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD)
/// using the Orientation direction (in ECI) of such plane (defind by ST_Orient, i.e. orientation of top of the CCD wrt the celestial north)
/// x,y,z: as input are in CCD frame,
///        as output are in Tangent Plane frame
/// ST_Orient: orientation of top of the CCD wrt the celestial north in radiant.  
void ST_CAM2tangPlane(double& x, double& y, double& z, double ST_Orient);

/// transformation of a point in XYZ from  AMS-02 frame int CCD
/// using the orientation of the camera given by a yaw-pitch(=0)-roll rotations
/// x,y,z: as input are in AMS-02 frame,
///        as output are in CCD frame
/// Y: Yaw rotation angle in radiant.
/// R: Roll rotation angle in radiant.
void ST_AMS2CAM(double& x, double& y, double& z, double Y, double R);

/// transformation of a point in XYZ from CCD frame int  AMS-02 frame
/// using the orientation of the camera given by a yaw-pitch(=0)-roll rotations
/// x,y,z: as input are in CCD frame,
///        as output are in AMS-02 frame
/// Y: Yaw rotation angle in radiant.
/// R: Roll rotation angle in radiant. 
void ST_CAM2AMS(double& x, double& y, double& z, double Y, double R) ;
  
// ** **************** GROUND SEGMENT   ************************/

void	FT_AMS2Body(double &x, double &y, double &z);
void	FT_Body2AMS(double &x, double &y, double &z);
/* ****************************************************************************/
void	FT_LVLH2Body(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll);
void	FT_Body2LVLH(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll);
/* ****************************************************************************/
void	FT_GTOD2LVLH(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta );
void	FT_LVLH2GTOD(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta );
/* ****************************************************************************/
double  FT_GMST_rad(double timeUnix);
void	FT_GTOD2Equat(double &x, double &y, double &z, double time);
void	FT_Equat2GTOD(double &x, double &y, double &z, double time);
/* *************************************************************/
double  FT_GPS_JD(double itime);
double  FT_UTC_JD(double itime);
/* *************************************************************/
int  FT_Equat2Gal(double &azimut, double &elev);
int  FT_Gal2Equat(double &azimut, double &elev);
/* *************************************************************/
void FT_Body_to_J2000(double &x, double &y, double &z, double ISSyaw,double ISSpitch, double ISSroll );


