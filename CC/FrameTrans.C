/**
 *  this is library to convert from/to AMS reference using
 *  ground measurementrs or startracker.
 * 
 *  see FrameTrans.h for additiona comments and reference
 * 
 *   developers:
 *   SDT - Stefano Della Torre - INFN Milano Bicocca - stefano.dellatorre@mib.infn.it
 *   AL  - Alexey Lebedev - MIT - alexey.lebedev@cern.ch     
 *        
 *  
 */
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include "FrameTrans.h"

using namespace std;
//Pi value obtained with Wolfram-Mathematica 8
#define twopi  6.2831853071795864769252867665590057683943387987502    // 2*Pi
#define pi     3.1415926535897932384626433832795028841971693993751    // Pi
#define pio2   1.5707963267948966192313216916397514420985846996876    // Pi/2
#define x3pio2 4.7123889803846898576939650749192543262957540990627    // 3*Pi/2
//#define AU  1.49597870691E8  // Astronomical unit - km (IAU 76)
#define AU  14959787070030   // Astronomical unit - cm - Particle Physics booklet 2010

//~------------ TO BE USED BY USER ------------

//~============================================================================

int get_ams_ra_dec_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double &ra, double &dec, double PosISS[3], double VelISS[3], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[3] = Velocity of ISS in VelocityS(angular velocity), ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll  [rad]
// xtime     = UTC time (in unix format)

  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  
  //convert position  of ISS into J2000 frame
  double ISSx,ISSy,ISSz;
  double ISSvx,ISSvy,ISSvz;
  FT_Angular2Cart(PosISS[0],  PosISS[2],PosISS[1],ISSx, ISSy, ISSz);
  FT_Angular2Cart(VelISS[0]*PosISS[0],  VelISS[2],VelISS[1],ISSvx, ISSvy, ISSvz);
  FT_GTOD2Equat(ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz, xtime);
  //convert  
  FT_AMS2Body(x, y, z);
  FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);            // Parameters: ISSyaw,  ISSpitch,  ISSroll  (in rad)          
  FT_LVLH2ECI(x, y, z,ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz ); // Parameters: x,y,z,vx,vy,z
  FT_Cart2Angular(x, y, z,r, dec,  ra);
  dec=dec*180./pi;
  ra=ra*180./pi;
  return 0;
}



int get_ams_l_b_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double & l, double &b, double PosISS[3], double VelISS[3], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[3] = Velocity of ISS in VelocityS(angular velocity), ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll [in rad]
// xtime     = UTC time (in unix format)
  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  double ra,dec;
  //convert position  of ISS into J2000 frame
  double ISSx,ISSy,ISSz;
  double ISSvx,ISSvy,ISSvz;
  FT_Angular2Cart(PosISS[0],  PosISS[2],PosISS[1],ISSx, ISSy, ISSz);
  FT_Angular2Cart(VelISS[0]*PosISS[0],  VelISS[2],VelISS[1],ISSvx, ISSvy, ISSvz);
  FT_GTOD2Equat(ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz, xtime);
  //convert  
  FT_AMS2Body(x, y, z);
  FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);            // Parameters: ISSyaw,  ISSpitch,  ISSroll  (in rad)          
  FT_LVLH2ECI(x, y, z,ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz ); // Parameters: x,y,z,vx,vy,z
  FT_Cart2Angular(x, y, z,r, dec,  ra);
  FT_Equat2Gal(ra, dec);
   /* result */ 
  l=ra*180./pi;
  b=dec*180./pi;
  return 0;
}


int get_ams_gtod_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double & theta, double &phi, double PosISS[3], double VelISS[3], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// theta, phi          = GTOD latitude and longitude  of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[3] = Velocity of ISS in VelocityS(angular velocity), ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll [in rad]
// xtime     = UTC time (in unix format)
  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  //convert position  of ISS into J2000 frame
  double ISSx,ISSy,ISSz;
  double ISSvx,ISSvy,ISSvz;
  FT_Angular2Cart(PosISS[0],  PosISS[2],PosISS[1],ISSx, ISSy, ISSz);
  FT_Angular2Cart(VelISS[0]*PosISS[0],  VelISS[2],VelISS[1],ISSvx, ISSvy, ISSvz);
  FT_GTOD2Equat(ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz, xtime);
  //convert  
  FT_AMS2Body(x, y, z);
  FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);            // Parameters: ISSyaw,  ISSpitch,  ISSroll  (in rad)          
  FT_LVLH2ECI(x, y, z,ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz ); // Parameters: x,y,z,vx,vy,z
  FT_Equat2GTOD(x, y,z,xtime);
   /* result */

  FT_Cart2Angular(x, y, z,r, theta,  phi); 
  theta=theta*180./pi;
  phi=phi*180./pi;
  
  
  return 0;
}




int get_ams_ra_dec_from_StarTracker(double AMS_x, double AMS_y,double AMS_z, double & ra, double &dec, int CamID, double ST_RA, double ST_dec, double ST_Orient){
// SDT(sept2012) - trasformation from AMS frame to J2000 frame using the star Tracker data
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
// CamID               = Identifier of Camera, 1(Port Camera), 0(starBoard)
// ST_RA               = J2000 Right Ascension of Star Tracker pointing direction in degree;
// ST_dec              = J2000 Declination of Star Tracker pointing direction in degree; 
// ST_Orient           = orientation of top of the CCD wrt the celestial north in degree.
  double r;
  ST_RA=ST_RA/180.*pi;
  ST_dec=ST_dec/180.*pi; 
  ST_Orient=ST_Orient/180.*pi; 
  ST_AMS2ECI( AMS_x,  AMS_y,  AMS_z, CamID,  ST_RA,  ST_dec,  ST_Orient);
  FT_Cart2Angular( AMS_x,  AMS_y,  AMS_z,r,  dec,  ra);
  dec=dec*180./pi;
  ra=ra*180./pi;
  return 0;
}

int get_ams_l_b_from_StarTracker(double AMS_x, double AMS_y,double AMS_z, double & l, double &b, int CamID, double ST_RA, double ST_dec, double ST_Orient){
// SDT(sept2012) - trasformation from AMS frame to galactic frame using the star Tracker data
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
// CamID               = Identifier of Camera, 1(Port Camera), 0(starBoard)
// ST_RA               = J2000 Right Ascension of Star Tracker pointing direction in degree;
// ST_dec              = J2000 Declination of Star Tracker pointing direction in degree; 
// ST_Orient           = orientation of top of the CCD wrt the celestial north in degree.
  double r,ra,dec;
  ST_RA=ST_RA/180.*pi;
  ST_dec=ST_dec/180.*pi; 
  ST_Orient=ST_Orient/180.*pi; 
  ST_AMS2ECI( AMS_x,  AMS_y,  AMS_z, CamID,  ST_RA,  ST_dec,  ST_Orient);
  FT_Cart2Angular( AMS_x,  AMS_y,  AMS_z,r,  dec,  ra);
  FT_Equat2Gal(ra, dec);
  l=ra*180./pi;
  b=dec*180./pi;
  return 0;
}

int get_ams_ra_dec_from_ALTEC_INTL(double AMS_x, double AMS_y,double AMS_z, double & ra, double &dec, double ISSyaw,double ISSpitch, double ISSroll){
// SDT(sept2012) - trasformation from AMS frame to J2000 frame using the star Tracker data
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
// ISSyaw,ISSpitch,ISSroll= attitude Information from ALTEC INTL data, i.e. the attitude with respect the J2000 frame, in degree.
  double r;//dummy
  ISSyaw=ISSyaw/180.*pi;
  ISSpitch=ISSpitch/180.*pi;
  ISSroll=ISSroll/180.*pi;
  FT_AMS2Body(AMS_x, AMS_y, AMS_z);                  // move from AMS to ISS Body frame
  FT_Body_to_J2000(AMS_x, AMS_y, AMS_z,  ISSyaw, ISSpitch,  ISSroll ); // move from ISS Body frame to J2000
  FT_Cart2Angular( AMS_x,  AMS_y,  AMS_z, r, dec,  ra);
  dec=dec/pi*180.;
  ra=ra/pi*180.;
  
return 0; 
}


// from galactic longitude and latitude calculates ams coordinates
// INVERSE TO FRAMETRANS from galactic longitude,latitude to AMS coordinates
int get_ams_l_b_inverse_fromGTOD(double &AMS_x, double &AMS_y,double &AMS_z, double l, double b, double PosISS[3], double VelISS[3], double ypr[3], double xtime){
  // C.Pizzolotto(Nov2012) - generic trasformation from galactic frame to AMS frame passing via via GTOD (or ~ctrs)
  // l, b                = galactic longitude and latitude in J2000 frame in degree
  // AMS_x, AMS_y, AMS_z = Particle direction in AMS frame (cartesian): output 
  // PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
  // VelISS[3] = Velocity of ISS in VelocityS(angular velocity), ISSVelPhi, ISSVelTheta
  // ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll [in rad]
  // xtime     = UTC time (in unix format)
  
  double ra,dec;
  double x,y,z;
  double r=1.; 
  
  ra = l/180.*pi; //LONGITUDE radiants
  dec = b/180.*pi;//LATITUDE radiants
 
  //convert position  of ISS into J2000 frame
  double ISSx,ISSy,ISSz;
  double ISSvx,ISSvy,ISSvz;
  FT_Angular2Cart(PosISS[0],  PosISS[2],PosISS[1],ISSx, ISSy, ISSz);
  FT_Angular2Cart(VelISS[0]*PosISS[0],  VelISS[2],VelISS[1],ISSvx, ISSvy, ISSvz);
  FT_GTOD2Equat(ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz, xtime);
  
  FT_Gal2Equat(ra,dec);
  FT_Angular2Cart(r,  dec,  ra,  x,  y,  z ); 
  FT_ECI2LVLH(x, y, z,ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz ); // Parameters: x,y,z,vx,vy,z
  FT_LVLH2Body(x,y,z,ypr[0],ypr[1],ypr[2]);   // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_Body2AMS(x,y,z);
  AMS_x = x;
  AMS_y = y;
  AMS_z = z;

  return 0;
}




double degree_to_Rad(double angDeg){
// SDT (nov 2012) - convert angle in degree to radiant
return angDeg/180.*pi;
}  
double rad_to_degree(double angRad){
// SDT (nov 2012) - convert angle in radiant to degree
return angRad*180./pi;
}  

/** **************** COMMON *******************************/
void	FT_Cart2Angular(double x, double y, double z, double& r, double& theta, double& phi){
  /* convert cartesian coordinates into spherical coordinates 
     theta is the latitude (i.e. the angle with the equator plane
     phi is the aziutal angle*/
  r=sqrt(x*x+y*y+z*z);
  phi=atan2(y,x);
  theta=pi/2.-acos(z/r);
}

void	FT_Angular2Cart(double r, double theta, double phi, double& x, double& y, double& z ){
  /* convert spherical coordinates into cartesian coordinates 
     theta is the latitude (i.e. the angle with the equator plane
     phi is the aziutal angle*/
  x=r*sin(pi/2.-theta)*cos(phi);
  y=r*sin(pi/2.-theta)*sin(phi);
  z=r*cos(pi/2.-theta);
}

double FT_Modulus(double arg1, double arg2)
{
	/* Returns arg1 mod arg2 */
	int i;
	double ret_val;
	ret_val=arg1;
	i=(int)(ret_val/arg2);
	ret_val-=i*arg2;
	if (ret_val<0.0)
		ret_val+=arg2;
	return ret_val;
}


/** **************** STAR TRACKER *************************/

void ST_AMS2ECI(double& x, double& y, double& z,int CamID, double ST_RA, double ST_dec, double ST_Orient){
// SDT (Sept 2012) - transformation of a point in XYZ from AMS-02 frame into ECI-J2000 celestial frame
//                   using information from StarTracker
// x,y,z: as input are in AMS-02 frame,
//        as output are in J2000 frame.
// CamID: Identifier of Camera, 1(Port Camera), 0(starBoard)
// ST_RA: J2000 Right Ascension of Star Tracker pointing direction;
// ST_dec: J2000 Declination of Star Tracker pointing direction;  
  double CAM_Yaw,CAM_Roll;
  if (CamID==1){ /*Port Camera*/
    CAM_Yaw=235./180.*pi;   /* Nominal Value*/
    CAM_Roll=-40./180.*pi;  /* nominal Value*/
  }
  if (CamID==0){ /*StarBoard Camera*/
    CAM_Yaw=55./180.*pi;    /* Nominal Value*/
    CAM_Roll=-40./180.*pi;  /* nominal Value*/
  }
  ST_AMS2CAM(x, y, z, CAM_Yaw, CAM_Roll);  
  ST_CAM2tangPlane( x, y, z, -ST_Orient-pi);
  ST_tangPlane2ECI( x, y, z, ST_RA, ST_dec);
// the sequence is:
// i)  move from AMS-frame to CCD frame using rotations defined by Star Tracker Position
// ii) move from CCD to tangent plane using the info on image rotation wrt celestial north. this angle should correct by 180deg and negate. (under study)
// iii)move from Tangent plane to Celestial frame using the pointing direction of Star tracker.  
}

void ST_ECI2AMS(double& x, double& y, double& z,int CamID, double ST_RA, double ST_dec, double ST_Orient){
// SDT (Sept 2012) - transformation of a point in XYZ from  ECI-J2000 celestial  frame AMS-02 intoframe
//                   using information from StarTracker
// x,y,z: as input are in J2000 frame,
//        as output are in AMS-02 frame.
// CamID: Identifier of Camera, 1(Port Camera), 0(starBoard)
// ST_RA: J2000 Right Ascension of Star Tracker pointing direction;
// ST_dec: J2000 Declination of Star Tracker pointing direction;  
  double CAM_Yaw,CAM_Roll;
  if (CamID==1){ /*Port Camera*/
    CAM_Yaw=235./180.*pi;   /* Nominal Value*/
    CAM_Roll=-40./180.*pi;  /* nominal Value*/
  }
  if (CamID==0){ /*StarBoard Camera*/
    CAM_Yaw=55./180.*pi;    /* Nominal Value*/
    CAM_Roll=-40./180.*pi;  /* nominal Value*/
  }
  ST_ECI2tangPlane( x, y, z, ST_RA, ST_dec);
  ST_tangPlane2CAM( x, y, z, -ST_Orient-pi);
  ST_CAM2AMS(x, y, z, CAM_Yaw, CAM_Roll);  
 
 
}


void ST_ECI2tangPlane(double& x, double& y, double& z, double ST_RA, double ST_dec){
// SDT (Sept 2012) - transformation of a point in XYZ from ECI frame to Plane tangent to celestial frame (i.e. plane of Star Tracker CCD)
//                   using the zenit direction (in ECI) of such plane (defind by ST_RA and ST_dec, pointing direction of starTracker camera)
// x,y,z: as input are in ECI frame,
//        as output are in Tangent Plane frame
// ST_RA: J2000 Right Ascension of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker
// ST_dec: J2000 Declination of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker
  double oldX=x;double oldY=y;double oldZ=z;
  x=-sin(ST_RA)*oldX             + cos(ST_RA)*oldY;
  y=-cos(ST_RA)*sin(ST_dec)*oldX - sin(ST_RA)*sin(ST_dec)*oldY   + (cos(ST_RA)*cos(ST_RA)* cos(ST_dec)+ cos(ST_dec) *sin(ST_RA)*sin(ST_RA))*oldZ;
  z=cos(ST_dec)*cos(ST_RA)*oldX  + cos(ST_dec)*sin(ST_RA)*oldY   + sin(ST_dec)*oldZ;
//explanation:
//The transformation matrix is 
//   | X |   | -sin(RA)          cos(RA)                           0                                  |
//   | Y | = |-cos(RA)sin(Dec)   -sin(RA)sin(dec)                + cos^2(RA)cos(dec)+cos(dec)sin^2(RA)|
//   | Z |   | cos(Dec)cos(RA)   +cos(dec)sin(RA)                + sin(dec)                           |
// Z= is the unity vector of pointing direction in J2000 (x_eci=cos(Dec)cos(RA),y_eci=cos(dec)sin(RA),z_eci=sin(dec))
// X= is a vector on ECI equatorial plane that is ortigonal to R, this is obtained by vector product between Z_eci axis (0,0,1) and
//    the projection of R on ECI equatorial plane.(0,0,1)x(cos(RA),sin(RA),0)
// Y= Z x Y  
}
void ST_tangPlane2ECI(double& x, double& y, double& z, double ST_RA, double ST_dec){
// SDT (Sept 2012) - transformation of a point in XYZ from "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD) into ECI frame
//                   using the zenit direction (in ECI) of such plane (defind by ST_RA and ST_dec, pointing direction of starTracker camera)
// x,y,z: as input are in Tangent Plane frame,
//        as output are in ECI frame
// ST_RA: J2000 Right Ascension of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker
// ST_dec: J2000 Declination of zenit direction of tangent plane. i.e. the pointing direction of Star Tracker
  double oldX=x;double oldY=y;double oldZ=z;
  x=-sin(ST_RA)*oldX             - cos(ST_RA)*sin(ST_dec)*oldY                                                 + cos(ST_dec)*cos(ST_RA)*oldZ;
  y= cos(ST_RA)*oldX             - sin(ST_RA)*sin(ST_dec)*oldY                                                 + cos(ST_dec)*sin(ST_RA)*oldZ;
  z=                             (cos(ST_RA)*cos(ST_RA)* cos(ST_dec)+ cos(ST_dec) *sin(ST_RA)*sin(ST_RA))*oldY + sin(ST_dec)*oldZ;
// explanation:
// transpose matrix defined in ST_ECI2tangPlane()  
}

void ST_tangPlane2CAM(double& x, double& y, double& z, double ST_Orient){
// SDT (Sept 2012) - transformation of a point in XYZ from "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD) into CCD frame
//                   using the Orientation direction (in ECI) of such plane (defind by ST_Orient, i.e. orientation of top of the CCD wrt the celestial north)
// x,y,z: as input are in Tangent Plane frame,
//        as output are in CCD frame
// ST_Orient: orientation of top of the CCD wrt the celestial north.
  double oldX=x;double oldY=y;double oldZ=z;
  x= cos(ST_Orient)*oldX   + sin(ST_Orient)*oldY;
  y=-sin(ST_Orient)*oldX   + cos(ST_Orient)*oldY;
  z=                                        oldZ;
// explanation:
// this matrix rotate the local frame on the camera (the "Plane tangent to celestial frame") to match the CCD frame definition.
// the relation between the to frame is a rotation around the pointing direction (z axis) of the angle defined by  ST_Orient.
// note that in this case ST_Orient is positive clockwise.
}
void ST_CAM2tangPlane(double& x, double& y, double& z, double ST_Orient){
// SDT (Sept 2012) - transformation of a point in XYZ from  CCD frame into "Plane tangent to celestial frame" (i.e. plane of Star Tracker CCD)
//                   using the Orientation direction (in ECI) of such plane (defind by ST_Orient, i.e. orientation of top of the CCD wrt the celestial north)
// x,y,z: as input are in CCD frame,
//        as output are in Tangent Plane frame
// ST_Orient: orientation of top of the CCD wrt the celestial north.  
  double oldX=x;double oldY=y;double oldZ=z; 
  x= cos(ST_Orient)*oldX   - sin(ST_Orient)*oldY;
  y= sin(ST_Orient)*oldX   + cos(ST_Orient)*oldY;
  z=                                        oldZ;
// explanation:
// transpose matrix defined in  ST_tangPlane2CAM(); 
}

void ST_AMS2CAM(double& x, double& y, double& z, double Y, double R){
// SDT (Sept 2012) - transformation of a point in XYZ from  AMS-02 frame int CCD
//                   using the orientation of the camera given by a yaw-pitch(=0)-roll rotations
// x,y,z: as input are in AMS-02 frame,
//        as output are in CCD frame
// Y: Yaw rotation angle.
// R: Roll rotation angle.
  double oldX=x;double oldY=y;double oldZ=z;
 
  x= cos(Y)*oldX        + sin(Y)*oldY;
  y=-cos(R)*sin(Y)*oldX + cos(R)*cos(Y)*oldY   + sin(R)*oldZ;
  z= sin(R)*sin(Y)*oldX - cos(Y)*sin(R)*oldY   + cos(R)*oldZ;
// explanation:
// this is the common YPR matrix, where P=0. the CCD frame was measured during assembly procedure.
}

void ST_CAM2AMS(double& x, double& y, double& z, double Y, double R){
// SDT (Sept 2012) - transformation of a point in XYZ from CCD frame int  AMS-02 frame
//                   using the orientation of the camera given by a yaw-pitch(=0)-roll rotations
// x,y,z: as input are in CCD frame,
//        as output are in AMS-02 frame
// Y: Yaw rotation angle.
// R: Roll rotation angle.  
  double oldX=x;double oldY=y;double oldZ=z;
  x= cos(Y)*oldX        - cos(R)*sin(Y)*oldY   + sin(R)*sin(Y)*oldZ; 
  y= sin(Y)*oldX        + cos(R)*cos(Y)*oldY   - cos(Y)*sin(R)*oldZ;
  z=                    + sin(R)*oldY          + cos(R)*oldZ;
// explanation:
// this is the transpose of YPR matrix, where P=0. the CCD frame was measured during assembly procedure.  
}


/****************
 * FSpada nov 2011 - Give extrapolated pos (ICRS) after dt
 ****************/
AMSPoint getOrbitPoint(double dt, double x0, double y0, double z0, double vx0, double vy0, double vz0, int is_circ)
{

  const double GM = 398600.4418;     // GM = 398600.4418 +- 0.0008 km^3 s^(-2) 
  const double toll = 1E-7;

  double r0 = sqrt(x0*x0+y0*y0+z0*z0);
  double v0 = sqrt(vx0*vx0+vy0*vy0+vz0*vz0);
  cout<<fixed;
  double m[3], A[3];
  m[0] = y0*vz0-z0*vy0;
  m[1] = z0*vx0-x0*vz0;
  m[2] = x0*vy0-y0*vx0;
  double mm = sqrt(m[0]*m[0]+m[1]*m[1]+m[2]*m[2]);
  A[0] =  x0*vy0*vy0 - y0*vx0*vy0 - z0*vx0*vz0 + x0*vz0*vz0 - GM*x0/r0;
  A[1] = -x0*vx0*vy0 + y0*vx0*vx0 + y0*vz0*vz0 - z0*vy0*vz0 - GM*y0/r0;
  A[2] =  z0*vx0*vx0 - x0*vx0*vz0 - y0*vy0*vz0 + z0*vy0*vy0 - GM*z0/r0;
  double a = (is_circ)? r0 : GM*r0/(2*GM-v0*v0*r0);
  double p =  m[0]/(mm+m[2]);
  double q = -m[1]/(mm+m[2]);
  double k = (is_circ)? 0 : (A[0]*(1-p*p+q*q) + 2*A[1]*p*q - 2*A[2]*p) / (GM*(1+p*p+q*q));
  double h = (is_circ)? 0 : (2*A[0]*p*q + A[1]*(1+p*p-q*q) + 2*A[2]*q) / (GM*(1+p*p+q*q));
  double X1 = (x0*(1-p*p+q*q) + 2*y0*p*q - 2*z0*p) / (1+p*p+q*q);
  double Y1 = (2*x0*p*q + y0*(1+p*p-q*q) + 2*z0*q) / (1+p*p+q*q);
  double b = 1/(1+sqrt(1-h*h-k*k));
  double F = atan3( k + ((1-k*k*b)*X1 - h*k*b*Y1)/(a*sqrt(1-h*h-k*k)), h + ((1-h*h*b)*Y1 - h*k*b*X1)/(a*sqrt(1-h*h-k*k)) );
  double l = F - k*sin(F) + h*cos(F);
  double F0 = l + dt*sqrt(GM/pow(a,3)), Fi, Fii, Ft;
  int i = 0;
  double dif;
  do {                               // if is_circ will iterate just once and Ft = F0
    if (i==0) Fi = F0;
    Fii = Fi - ( Fi - k*sin(Fi) + h*cos(Fi) - F0 )/( 1 - k*cos(Fi) - h*sin(Fi));
    dif=sqrt((Fii-Fi)*(Fii-Fi));
    //cout<<"       i="<<i<<" Fi "<<Fi<<" Fii "<<Fii<<"  dF "<<dif<<endl;
    i++; 
    Fi = Fii;
  } while (dif>toll);
  Ft = Fi;
  //cout<<"  getOrbitPoint:: F0 "<<F0<<" Ft "<<Ft<<endl;
  double Xt = a*( (1-h*h*b)*cos(Ft) + h*k*b*sin(Ft) - k );
  double Yt = a*( (1-k*k*b)*sin(Ft) + h*k*b*cos(Ft) - h );
  //cout<<"  getOrbitPoint:: Xt "<<Xt<<" Yt "<<Yt<<endl;
  double x = (Xt*(1-p*p+q*q) + 2*Yt*p*q) / (1+p*p+q*q);
  double y = (Yt*(1+p*p-q*q) + 2*Xt*p*q) / (1+p*p+q*q);
  double z = (-2*Xt*p + 2*Yt*q) / (1+p*p+q*q);
  //double x1 = (X1*(1-p*p+q*q) + 2*Y1*p*q) / (1+p*p+q*q);
  //double y1 = (Y1*(1+p*p-q*q) + 2*X1*p*q) / (1+p*p+q*q);
  // double z1 = (-2*X1*p + 2*Y1*q) / (1+p*p+q*q);
  //cout<<"getOrbitPoint:: x "<<x<<" y "<<y<<" z "<<z<<endl;
  //cout<<"getOrbitPoint:: x1 "<<x1<<" y1 "<<y1<<" z1 "<<z1<<endl;
  AMSPoint _pos(x,y,z);
  return _pos;
}

/************************************
 * atan with range -PI/2:3PI/2
 ************************************/
double atan3(double x, double y)
{
  const double PI = 3.141599265;
  double t = (x>0)? atan(y/x) : atan(y/x)+PI;
  return t;
}




/** **************** GROUND SEGMENT   ************************/



/*****************************************************************************/
void	FT_AMS2Body(double &x, double &y, double &z){
    /* convert from AMS-02 Ref System to ISS-Body ref System */
   double oldX,oldY,oldZ;
     /*rotate the frame of -12.0001 degree along y azis*/
   double a=12.0001*pi/180.; /*inclination of ams-02 in radian*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX*cos(a)+oldZ*sin(a);
   y= oldY;
   z= -oldX*sin(a)+oldZ*cos(a);
   
   /*change orientation of axis*/

   oldX=x; oldY=y; oldZ=z;
   x=-oldY;
   y=-oldX;
   z=-oldZ;
     
     /*move from AMS-02 position to centre of body system */
  // x=x-85.598;
  // y=y+2136.390;
  // z=z-388.112;
}
  
void	FT_Body2AMS(double &x, double &y, double &z){
  /* convert from ISS-Body ref System to AMS-02 Ref System*/
  /** AMS is located on starboard side of ISS 
   *  Y axis of AMS is pointing to the wake side (opposite of the motion), 
   *  and the X axis is pointing to port side (toward the habitative modules).
   *  the Y azis is opposite to the direction of Earth
   *  AMS in inclined along the YZ plane of a=12.0001° 
   * 
   * AMS 02 System ref center is located 
   * X = -33.7 inches = -0.85598 m =  -85.598 cm
   * Y = 841.1 inches =  21.3639 m = 2136.390 cm
   * Z = -152.8 inches= -3.88112 m = -388.112 cm
   * From the Station Analysis Coordinate System 0,0,0 
   * (X along ram side, Y along starboard direction and Z toward Earth)
   * the transformation matrixs are
   *  	   | cos(a) 0 -sin(a)|   | 0  -1  0 |   
   * P_ams=|    0   1    0   | * | -1  0  0 | *  P_body
   * 	   | sin(a) 0  cos(a)|   | 0   0 -1 |  
   * */
    
   /*move from centre of body system to AMS-02 position*/
  // x=x+85.598;
  // y=y-2136.390;
  // z=z+388.112;
   
   /*change orientation of axis*/
   double oldX,oldY,oldZ;
   oldX=x; oldY=y; oldZ=z;
   x=-oldY;
   y=-oldX;
   z=-oldZ;
   
   /*rotate the frame of 12.0001 degree along y azis*/
   double a=12.0001*pi/180.; /*inclination of ams-02 in radian*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX*cos(a)-oldZ*sin(a);
   y= oldY;
   z= oldX*sin(a)+oldZ*cos(a);
  
}
/*****************************************************************************/



void	FT_LVLH2Body(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll){
  /** move from LVLH to ISS-body ref System, this could be done by appling the ISS attitude, 
   *  the reference sistem is simply rotate along Z,Y,X axes 
   *  using respectly the yaw,pitch and roll angle*/
  /** Ref: H. Suter (1998) AMS internal Report "Space shuttle flight parameters for mission STS-91" sec 4.1.2*/
  /** ref: Lectures notes on modeling of a spacecraft, Rafal Wisniewski, internal note, sec 3.5 */
  double oldX,oldY,oldZ;
  /* rotation along Z axis*/
  oldX=x; oldY=y; oldZ=z;
  x= oldX*cos(ISSyaw)+oldY*sin(ISSyaw);
  y=-oldX*sin(ISSyaw)+oldY*cos(ISSyaw);
  z= oldZ;
  /* rotation along Y axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX*cos(ISSpitch)-oldZ*sin(ISSpitch);
   y= oldY;
   z= oldX*sin(ISSpitch)+oldZ*cos(ISSpitch);
   /* rotation along X axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX;
   y= oldY*cos(ISSroll)+oldZ*sin(ISSroll);
   z=-oldY*sin(ISSroll)+oldZ*cos(ISSroll);
}

void	FT_Body2LVLH(double &x, double &y, double &z, double ISSyaw, double ISSpitch, double ISSroll){
    double oldX,oldY,oldZ;
       /* rotation along X axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX;
   y= oldY*cos(ISSroll)-oldZ*sin(ISSroll);
   z=+oldY*sin(ISSroll)+oldZ*cos(ISSroll);
  /* rotation along Y axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX*cos(ISSpitch)+oldZ*sin(ISSpitch);
   y= oldY;
   z=-oldX*sin(ISSpitch)+oldZ*cos(ISSpitch);
  /* rotation along Z axis*/
  oldX=x; oldY=y; oldZ=z;
  x= oldX*cos(ISSyaw)-oldY*sin(ISSyaw);
  y= oldX*sin(ISSyaw)+oldY*cos(ISSyaw);
  z= oldZ;
    
}
/*****************************************************************************/
                   
void	FT_ECI2LVLH(double &x, double &y, double &z, double ISSx,double ISSy,double ISSz,double ISSVelx, double ISSVely,double ISSVelz ){
  /** convert the position from ECI to LVLH
      note: for the purpose of this procedure ISS 
      and SUN MUST be given in the same ref sistem
      - in this case ECI -*/
  /** Ref: H. Suter (1998) AMS internal Report "Space shuttle flight parameters for mission STS-91"*/
  /*  let define: 
   *  V_v = unity vector along the direction of velocity vector of ISS , 
   *  R_v = unity vector along the direction position vector of ISS
   *  the matrix of transformation is given by three lines X_v, Y_v, Z_v  of 3 elements each
   * | X_v |   |  Y_v x Z_v |
   * | Y_v | = | V_v x R_v  | =R_m
   * | Z_v |   | -R_v       |
   *
   * then if SunPos_v is the position of Sun, ISSpos is the vector of ISS seen from Earth, 
   * the position in the ISS LVLH frame is P1_v
   * P1_v= R_m x (SunPos_v - ISSpos)
   */
  /* define the ISS coordinates and velocity*/

   double ISSpos_x,ISSpos_y, ISSpos_z, ISSpos_mag;
   ISSpos_x=ISSx; ISSpos_y=ISSy;ISSpos_z=ISSz;
   ISSpos_mag=sqrt(ISSpos_x*ISSpos_x+ISSpos_y*ISSpos_y+ISSpos_z*ISSpos_z);
   double ISSVel_mag=sqrt(ISSVelx*ISSVelx+ISSVely*ISSVely+ISSVelz*ISSVelz);
   /* define the ISS coordinates and velocity*/
//    double R_x = cos(ISSasc ) * sin( pi/2.- ISSdec );
//    double R_y = sin( ISSasc ) * sin( pi/2.-ISSdec );
//    double R_z = cos( pi/2.-ISSdec );  
//    double V_x = cos( ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
//    double V_y = sin(ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
//    double V_z = cos( pi/2.-ISSVelTheta );
   double R_x = ISSx/ISSpos_mag;
   double R_y = ISSy/ISSpos_mag;
   double R_z = ISSz/ISSpos_mag;  
   double V_x = ISSVelx/ISSVel_mag;
   double V_y = ISSVely/ISSVel_mag;
   double V_z = ISSVelz/ISSVel_mag;
  /* define the solar vertor*/
  double SunPos_x,SunPos_y,SunPos_z;
  SunPos_x=x;
  SunPos_y=y;
  SunPos_z=z;
  
  /* 1st step: translation ref to ISS posizione (SunPos_v - ISSpos)*/
//  SunPos_x=SunPos_x-ISSpos_x;
//  SunPos_y=SunPos_y-ISSpos_y;
//  SunPos_z=SunPos_z-ISSpos_z;
  
  /* 2nd step: create the martix R_m elements*/
  /* X_v, Y_v, Z_v vectors must be unitary */
  double X_x,X_y,X_z,Y_x,Y_y,Y_z,Z_x,Z_y,Z_z;
  double X_w,Y_w,Z_w; /* vector magnitude*/
   /* Y_v */
  Y_x=V_y*R_z-V_z*R_y;  
  Y_y=-(V_x*R_z-V_z*R_x);
  Y_z=V_x*R_y-V_y*R_x;  
  Y_w=sqrt(Y_x*Y_x+Y_y*Y_y +Y_z*Y_z);
  Y_x*=1./Y_w;
  Y_y*=1./Y_w;
  Y_z*=1./Y_w;
   /* Z_v */
  Z_x=-R_x;
  Z_y=-R_y;
  Z_z=-R_z;
  Z_w=sqrt(Z_x*Z_x+Z_y*Z_y +Z_z*Z_z);
   /* X_v */
  X_x=V_x*R_z*R_z +V_x*R_y*R_y -V_z*R_x*R_z -V_y*R_x*R_y;  
  X_y=V_y*R_x*R_x +V_y*R_z*R_z -V_x*R_x*R_y -V_z*R_y*R_z;
  X_z=V_z*R_y*R_y +V_z*R_x*R_x -V_y*R_y*R_z -V_x*R_z*R_x;
  X_w=sqrt(X_x*X_x+X_y*X_y +X_z*X_z);
  X_x*=1./X_w;
  X_y*=1./X_w;
  X_z*=1./X_w;
  
  /* 3rd step: rotate the frame to move in LVLH frame R_m x (SunPos_v - ISSpos)*/
  x = X_x*SunPos_x+X_y*SunPos_y+X_z*SunPos_z;
  y = Y_x*SunPos_x+Y_y*SunPos_y+Y_z*SunPos_z;
  z = Z_x*SunPos_x+Z_y*SunPos_y+Z_z*SunPos_z;
  
  //std::cout<<sqrt(x*x+y*y+z*z)<<std::endl;
  
}


void	FT_LVLH2ECI(double &x, double &y, double &z, double ISSx,double ISSy,double ISSz,double ISSVelx, double ISSVely,double ISSVelz ){
   double ISSpos_x,ISSpos_y, ISSpos_z, ISSpos_mag;
   ISSpos_x=ISSx; ISSpos_y=ISSy;ISSpos_z=ISSz;
   ISSpos_mag=sqrt(ISSpos_x*ISSpos_x+ISSpos_y*ISSpos_y+ISSpos_z*ISSpos_z);
   double ISSVel_mag=sqrt(ISSVelx*ISSVelx+ISSVely*ISSVely+ISSVelz*ISSVelz);
   /* define the ISS coordinates and velocity*/
//    double R_x = cos(ISSasc ) * sin( pi/2.- ISSdec );
//    double R_y = sin( ISSasc ) * sin( pi/2.-ISSdec );
//    double R_z = cos( pi/2.-ISSdec );  
//    double V_x = cos( ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
//    double V_y = sin(ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
//    double V_z = cos( pi/2.-ISSVelTheta );
   double R_x = ISSx/ISSpos_mag;
   double R_y = ISSy/ISSpos_mag;
   double R_z = ISSz/ISSpos_mag;  
   double V_x = ISSVelx/ISSVel_mag;
   double V_y = ISSVely/ISSVel_mag;
   double V_z = ISSVelz/ISSVel_mag;

   
   /* define the pointing object vertor*/
   double Pos_x,Pos_y,Pos_z;
   Pos_x=x;  Pos_y=y;  Pos_z=z;
  
   
     /* 2nd step: create the martix R_m elements*/
  /* X_v, Y_v, Z_v vectors must be unitary */
  double X_x,X_y,X_z,Y_x,Y_y,Y_z,Z_x,Z_y,Z_z;
  double X_w,Y_w,Z_w; /* vector magnitude*/
   /* Y_v */
  Y_x=V_y*R_z-V_z*R_y;  
  Y_y=-(V_x*R_z-V_z*R_x);
  Y_z=V_x*R_y-V_y*R_x;  
  Y_w=sqrt(Y_x*Y_x+Y_y*Y_y +Y_z*Y_z);
  Y_x*=1./Y_w;
  Y_y*=1./Y_w;
  Y_z*=1./Y_w;
   /* Z_v */
  Z_x=-R_x;
  Z_y=-R_y;
  Z_z=-R_z;
  Z_w=sqrt(Z_x*Z_x+Z_y*Z_y +Z_z*Z_z);
   /* X_v */
  X_x=V_x*R_z*R_z +V_x*R_y*R_y -V_z*R_x*R_z -V_y*R_x*R_y;  
  X_y=V_y*R_x*R_x +V_y*R_z*R_z -V_x*R_x*R_y -V_z*R_y*R_z;
  X_z=V_z*R_y*R_y +V_z*R_x*R_x -V_y*R_y*R_z -V_x*R_z*R_x;
  X_w=sqrt(X_x*X_x+X_y*X_y +X_z*X_z);
  X_x*=1./X_w;
  X_y*=1./X_w;
  X_z*=1./X_w;
  
    /* 3rd step: rotate the frame to move in LVLH frame R_m^T x (SunPos_v )*/
  x = X_x*Pos_x+Y_x*Pos_y+Z_x*Pos_z;
  y = X_y*Pos_x+Y_y*Pos_y+Z_y*Pos_z;
  z = X_z*Pos_x+Y_z*Pos_y+Z_z*Pos_z;
  
  
}
/*****************************************************************************/

double FT_GMST_rad(double timeUnix){
 double time=  FT_UTC_JD((double) timeUnix); //convert from unixtime (UTC) to Julian days
 //double time=  FT_GPS_JD((double) timeUnix); //convert from unixtime (GPS) to Julian days
 /* Greenwich mean sidereal time  in radians */
 double T=(time-2451545.0)/36525.0 ;/*Interval of time, measured in Julian centuries of 36525 days of UT (mean solar day), elapsed since the epoch 2000 Jan 1d12hUT*/
 double d=(time-2451545.0);
 double GMST_deg;/*greenwich mean sidereal time in degree (i.e. the Greenwich hour angle of the mean equinox of date)*/
 /* GMST from Meeus, J., 2000. Astronomical Algorithms. Willman-Bell, Richmond,VA, 2nd ed.  p 84 (eq.11-4)
    adapdet from IDL procedure http://idlastro.gsfc.nasa.gov/ftp/pro/astro/ct2lst.pro */
 GMST_deg=FT_Modulus(( 280.46061837 + 360.98564736629*d +T*T*(0.000387933-T/38710000.0)),360);
 //GMST_deg = (int)(280.46061837 + 360.98564736629 * d + T * T * (0.000387933 -T /38710000.0)) % 360;  // AL - possible variant
 double GMSTrad= GMST_deg*pi/180.; /* greenwich mean sidereal time  in radians*/
 return GMSTrad;
}
void	FT_GTOD2Equat(double &x, double &y, double &z, double time){
  /* Conversion from ECI to GTOD coordinates */
  double oldX=x;
  double oldY=y;
  double oldZ=z;
  double alpha_g=0;
  
  alpha_g=FT_GMST_rad(time);
  
  x=oldX*cos(alpha_g)-oldY*sin(alpha_g);
  y=oldX*sin(alpha_g)+oldY*cos(alpha_g);
  z=oldZ;
  
}


void	FT_Equat2GTOD(double &x, double &y, double &z, double time){
  /* Conversion from ECI to GTOD coordinates */
  double oldX=x;
  double oldY=y;
  double oldZ=z;
  double alpha_g=0;
  
  alpha_g=FT_GMST_rad(time);
  
  x=oldX*cos(alpha_g)+oldY*sin(alpha_g);
  y=-oldX*sin(alpha_g)+oldY*cos(alpha_g);
  z=oldZ;
  
}

void	FT_GTOD2Equat(double &x, double &y, double &z, double &vx, double &vy, double &vz, double time){
  /* Conversion from ECI to GTOD coordinates */
  double oldX=x;
  double oldY=y;
  double oldZ=z;
  double alpha_g=0;
  
  alpha_g=FT_GMST_rad(time);
  
  x=oldX*cos(alpha_g)-oldY*sin(alpha_g);
  y=oldX*sin(alpha_g)+oldY*cos(alpha_g);
  z=oldZ;
  
  
  double oldVX=vx;
  double oldVY=vy;
  double oldVZ=vz;
  double Omega= 2*pi /86400.; /* a complete round during a mean solar day*/
  
  vx= oldVX*cos(alpha_g)-oldVY*sin(alpha_g)   + Omega*( -oldY*cos(alpha_g)-oldX*sin(alpha_g) );
  vy= oldVX*sin(alpha_g)+oldVY*cos(alpha_g)   + Omega*( oldX*cos(alpha_g)-oldY*sin(alpha_g));
  vz= oldVZ;
  
  
  
}


void	FT_Equat2GTOD(double &x, double &y, double &z, double &vx, double &vy, double &vz, double time){
  /* Conversion from ECI to GTOD coordinates */
  double oldX=x;
  double oldY=y;
  double oldZ=z;
  double alpha_g=0;
  
  alpha_g=FT_GMST_rad(time);
  
  x=oldX*cos(alpha_g)+oldY*sin(alpha_g);
  y=-oldX*sin(alpha_g)+oldY*cos(alpha_g);
  z=oldZ;
  
  /* to transform the velocity
   * 
   *        |cos(alpha_g)   sin(alpha_g)   0  |    |vx|           | 0  1  0 | |cos(alpha_g)   sin(alpha_g)   0  | |x|
   * V_GTOD=|-sin(alpha_g)  cos(alpha_g)   0  |  * |vy| + Omega * |-1  0  0 |*|-sin(alpha_g)  cos(alpha_g)   0  |*|y|
   *        |    0              0          1  |    |vz|           | 0  0  0 | |    0              0          1  | |z|
   *                                                           
   *                                                              |y*cos(alpha_g)-x*sin(alpha_g) |
   *                   ....                             + Omega * |-x*cos(alpha_g)-y*sin(alpha_g)|
   *                                                              |             0                | 
   */
  double oldVX=vx;
  double oldVY=vy;
  double oldVZ=vz;
  double Omega= 2*pi /86400.; /* a complete round during a mean solar day*/
  
  vx= oldVX*cos(alpha_g)+oldVY*sin(alpha_g)   + Omega*( oldY*cos(alpha_g)-oldX*sin(alpha_g) );
  vy=-oldVX*sin(alpha_g)+oldVY*cos(alpha_g)   + Omega*( -oldX*cos(alpha_g)-oldY*sin(alpha_g));
  vz= oldVZ;
  
  
  
  
}




/**************************************************************/

double  FT_GPS_JD(double itime){
 /* convert GPS time (given in unix time seconds) in JD day*/ 
 /** The GPS time is an atomic time scale like TAI (internationa Atomic Time), 
  *  but differe from this of 19s.
  *  the Julian Day is defined as number 
  *  of days from  noon of 1st Jan 4713 B.C.. 
  *  one JD, by definition is 86400 sec of TAI.
  *  For convenient transformation, 
  *  0h  UT Jan 2000 = 2451543.5 JD
  *  0.5 UT jan 2000 = 2451544.0 JD
  *  0h  UTC jan 1970 = 2440587.5 JD (check on book)
  * 
  *  since both GPS time and JD is based on the same clock,
  *  to convert one to each other is sufficient to offset the 
  *  GPS time origin to JD 0 and apply the clock offset
  * 
  *  note that GPS time is given in ns
  * 
  * GPS time is given in Unix Time, defined as the number of seconds 
  * elapsed since midnight Coordinated Universal Time (UTC) of Thursday, January 1, 1970 (JD=2440587.5)
  * 
  */ 
  double TAItime=itime-19; /* convert GPS Time in TAI time, this Difference is constant in time*/
  double JDsec=TAItime+(2440587.5*86400);/* JD(secconds)=TAI(seconds)+JD_1970*SecondsInADay*/
  double JD=JDsec/86400; /* convert seconds in days*/
  return JD;
}


double FT_UTC_JD(double itime){
 /* convert UTC time (given in unix time seconds) in JD day*/
 /** the difference between TAI and UTC is 
  *  [INTERNATIONAL EARTH ROTATION AND REFERENCE SYSTEMS SERVICE (IERS) - Bulletin C 43]
  *  from 2009 January 1, 0h UTC, to 2012 July 1  0h UTC  : UTC-TAI = - 34s
  *  from 2012 July 1,    0h UTC, until further notice    : UTC-TAI = - 35s 
  */
 const double UTC1Ju2012= 1341100800; /* unix time of 2012 July 1,    0h UTC*/
 double TAItime;
 /* convert UTC Time in TAI time, this Difference is a function of the time*/
 if (itime>=UTC1Ju2012) TAItime=itime-35;
  else TAItime=itime-34;
 double JDsec=TAItime+(2440587.5*86400);/* JD(secconds)=TAI(seconds)+JD_1970*SecondsInADay*/
 double JD=JDsec/86400; /* convert seconds in days*/
 return JD;  
}


//~============================================================================


int  FT_Equat2Gal(double &azimut, double &elev, int alg){
 double RA=azimut;
 double Dec=elev;
 double b;/* galactic latitude  */
 double l;/* galactic longitude */
 double GalCen_RA =(192.+15./60.)/180.*pi; /* 192deg 15' */
 double GalCen_Dec=( 27.+24./60.)/180.*pi; /*  27deg 24' */
 double GalCen_AscendingNode=33./180.*pi;  /*  33deg  0' */
 
 switch (alg){
   case 0:
      // SDT(Aug 2012) - converts from J2000 frame to Galactic:
      // from Duffett-Smith & Zwart, Practical Astronomy, 4th edition 2011, pp56-59  */

      b=asin(cos(Dec)*cos(GalCen_Dec)*cos(RA-GalCen_RA) + sin(Dec)*sin(GalCen_Dec));
      l=atan2( (sin(Dec)-sin(b)*sin(GalCen_Dec)),(cos(Dec)*sin(RA-GalCen_RA)*cos(GalCen_Dec) )  )+GalCen_AscendingNode;
     break;
   case 1:
      // SDT(Feb 2012) -  Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3
      // http://aa.usno.navy.mil/software/novas/novas_c/novasc_info.php
      Nova_equ2gal ( RA, Dec, l, b);
     break;
     
   
   default: return 0;
 }
 
 // SDT(sept2012) check if  galactic longitude is in the range [-180:180]
 if (l>pi){ //if longitude is >180deg then substract 360deg
    l+=-twopi;
    }
 // SDT(oct2012) - correct output    
 azimut=l;
 elev=b;
 return 1;
}


void Nova_equ2gal ( double rai, double deci, double &glon, double &glat)
/*
SDT feb 2013 - adapted from Novas.c

------------------------------------------------------------------------

   PURPOSE:
      To convert ICRS right ascension and declination to galactic
      longitude and latitude.

   REFERENCES:
      Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.

   INPUT
   ARGUMENTS:
      rai (double)
         ICRS right ascension radiant.
      deci (double)
         ICRS declination in radiant.

   OUTPUT
   ARGUMENTS:
      *glon (double)
         Galactic longitude in degrees.
      *glat (double)
         Galactic latitude in degrees.

   RETURNED
   VALUE:
      None.

   GLOBALS
   USED:
      DEG2RAD, RAD2DEG   novascon.c

   FUNCTIONS
   CALLED:
      sin                math.h
      cos                math.h
      sqrt               math.h
      atan2              math.h

   VER./DATE/
   PROGRAMMER:
      V1.0/11-03/JAB (USNO/AA)
      V1.1/03-06/JAB (USNO/AA): Fixed initialization of 'ag'.
      V1.2/03-11/WKP (USNO/AA): Added braces to 2-D array initialization
                                to quiet gcc warnings.

   NOTES:
      1. This function uses the coordinate transformation specified
      in the reference.
      2. This function is the C version of NOVAS Fortran routine
      'eqgal'.

------------------------------------------------------------------------
*/
{
   double r, d, pos1[3], pos2[3], xyproj, g;

/*
   Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
*/

   double ag[3][3] = {
      {-0.0548755604, +0.4941094279, -0.8676661490},
      {-0.8734370902, -0.4448296300, -0.1980763734},
      {-0.4838350155, +0.7469822445, +0.4559837762}};

/*
   Form position vector in equatorial system from input coordinates.
*/
   r = rai;
   d = deci;
   pos1[0] = cos (d) * cos (r);
   pos1[1] = cos (d) * sin (r);
   pos1[2] = sin (d);

/*
   Rotate position vector to galactic system, using Hipparcos
   documentation eq. 1.5.13.
*/

   pos2[0] = ag[0][0] * pos1[0] + ag[1][0] * pos1[1] +

             ag[2][0] * pos1[2];

   pos2[1] = ag[0][1] * pos1[0] + ag[1][1] * pos1[1] +

             ag[2][1] * pos1[2];

   pos2[2] = ag[0][2] * pos1[0] + ag[1][2] * pos1[1] +

             ag[2][2] * pos1[2];

/*
   Decompose galactic vector into longitude and latitude.
*/

   xyproj = sqrt (pos2[0] * pos2[0] + pos2[1] * pos2[1]);

   if (xyproj > 0.0)
      g = atan2 (pos2[1], pos2[0]);
    else
      g = 0.0;
   glon = g ;

   g = atan2 (pos2[2], xyproj);
   glat = g ;

   return;
}






//~----------------------------------------------------------------------------

int  FT_Gal2Equat(double &azimut, double &elev){
 /* converts from Galactic to J2000 frame :
  * from Duffett-Smith & Zwart, Practical Astronomy, 4th edition 2011, pp56-59  */
 double RA;
 double Dec;
 double b=elev;/* galactic latitude  */
 double l=azimut;  /* galactic longitude */
 
 double GalCen_RA =(192.+15./60.)/180.*pi; /* 192deg 15' */
 double GalCen_Dec=( 27.+24./60.)/180.*pi; /*  27deg 24' */
 double GalCen_AscendingNode=33./180.*pi;  /*  33deg  0' */
 
 
 Dec=asin(cos(b)*cos(GalCen_Dec)*sin(l-GalCen_AscendingNode) + sin(b)*sin(GalCen_Dec) );
 RA=atan2( (cos(b)*cos(l-GalCen_AscendingNode) ),  ( sin(b)*cos(GalCen_Dec)-cos(b)*sin(GalCen_Dec)*sin(l-GalCen_AscendingNode) )  )+GalCen_RA;
 

 // SDT(sept2012) check if  right ascension is in the range [-180:180]
 if (RA>pi){ //if RA is >180deg then substract 360deg
 RA+=-twopi;
 }
 // SDT(oct2012) - correct output
 azimut=RA;
 elev=Dec; 
 return 1;
 
}




/* *********************************************************************************/
void	FT_Body_to_J2000(double &x, double &y, double &z, double ISSyaw,double ISSpitch, double ISSroll )
{
  /* convert from body to J2000, 
   * it require the euler angle for the attitude respect j2000
   * to move from Body to j2000  is a simple roll pitch yaw rotation (YPR to move from J2000 to Body )*/
    double oldX,oldY,oldZ;


  /* rotation along X axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX;
   y= oldY*cos(ISSroll)-oldZ*sin(ISSroll);
   z=+oldY*sin(ISSroll)+oldZ*cos(ISSroll);
  /* rotation along Y axis*/
   oldX=x; oldY=y; oldZ=z;
   x= oldX*cos(ISSpitch)+oldZ*sin(ISSpitch);
   y= oldY;
   z=-oldX*sin(ISSpitch)+oldZ*cos(ISSpitch);
  /* rotation along Z axis*/
  oldX=x; oldY=y; oldZ=z;
  x= oldX*cos(ISSyaw)-oldY*sin(ISSyaw);
  y= oldX*sin(ISSyaw)+oldY*cos(ISSyaw);
  z= oldZ;
}

/* ********************************************************************/



// -------------------- old ----------------------
// int get_ams_l_b_fromGTOD(double PosISS[3], double VelISS[2], double ypr[3], double & Azim, double &Elev, double xtime){
// // SDT(aug2012) - generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
// // PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// // VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// // ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// // ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
// // xtime     = UTC time (in unix format)
//   
//   double x=0.; double y=0.; double z=1.;
//   double r=1.;//dummy for check
//   double ra,dec;
//   double l,b;
//   
//   FT_Angular2Cart( r,  Elev,  Azim,  x,  y,  z );
//   FT_AMS2Body(x,y,z);
//   FT_Body2LVLH(x,y,z,ypr[0],ypr[1],ypr[2]);                                // Parameters: ISSyaw,  ISSpitch,  ISSroll
//   FT_LVLH2ECI(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
//   FT_GTOD2Equat(x,y,z,xtime);                                              // Parameters: xtime is GPS
//   FT_Cart2Angular( x,  y,  z,r, dec,  ra);
//   FT_Equat2Gal(ra, dec);
//   l=ra;
//   b=dec;
//   /* result */
//   Azim=l;
//   Elev=b;
//   
//   return 0;
// }


// int get_ams_ra_dec_fromGTOD(double PosISS[3], double VelISS[2], double ypr[3], double & ra, double &dec, double xtime){
// // SDT(aug2012) - generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
// // PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// // VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// // ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// // ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
// // xtime     = UTC time (in unix format)
// 
//   
//   double x=0.; double y=0.; double z=1.;
//   double r=1.;//dummy for check
//   
//   FT_Angular2Cart(r,  dec,  ra,  x,  y,  z );
//   FT_AMS2Body(x, y, z);
//   FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);                                  // Parameters: ISSyaw,  ISSpitch,  ISSroll
//   FT_LVLH2ECI(x, y, z, PosISS[0], PosISS[1], PosISS[2] , VelISS[0], VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
//   FT_GTOD2Equat(x, y, z, xtime);                                                  // Parameters: xtime is GPS
//   FT_Cart2Angular(x, y, z,r, dec,  ra);
//   return 0;
// }


// int get_ams_ra_dec_fromGTOD_ref(double AMS_x, double AMS_y,double AMS_z, double &ra, double &dec, double PosISS[3], double VelISS[2], double ypr[3], double xtime){
// // SDT(sept2012) - generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
// // AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// // ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
// // PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// // VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// // ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// // xtime     = UTC time (in unix format)
// 
//   
//   double x=AMS_x; double y=AMS_y; double z=AMS_z;
//   double r=1.;//dummy for check
//   
// 
//   FT_AMS2Body(x, y, z);
//   FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);                                  // Parameters: ISSyaw,  ISSpitch,  ISSroll
//   FT_LVLH2ECI(x, y, z, PosISS[0], PosISS[1], PosISS[2] , VelISS[0], VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
//   FT_GTOD2Equat(x, y, z, xtime);                                                  // Parameters: xtime is GPS
//   FT_Cart2Angular(x, y, z,r, dec,  ra);
//   dec=dec*180./pi;
//   ra=ra*180./pi;
//   return 0;
// }

// int main(){
//  return 0;}
