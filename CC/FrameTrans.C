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

int get_ams_ra_dec_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double &ra, double &dec, double PosISS[3], double VelISS[2], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// ra, dec             = Right Ascension and Declination of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// xtime     = UTC time (in unix format)

  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  

  FT_AMS2Body(x, y, z);
  FT_Body2LVLH(x, y, z, ypr[0], ypr[1], ypr[2]);                                  // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_LVLH2GTOD(x, y, z, PosISS[0], PosISS[1], PosISS[2] , VelISS[0], VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
  FT_GTOD2Equat(x, y, z, xtime);                                                  // Parameters: xtime is GPS
  FT_Cart2Angular(x, y, z,r, dec,  ra);
  dec=dec*180./pi;
  ra=ra*180./pi;
  return 0;
}

int get_ams_l_b_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double & l, double &b, double PosISS[3], double VelISS[2], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// xtime     = UTC time (in unix format)
  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  double ra,dec;

  FT_AMS2Body(x,y,z);
  FT_Body2LVLH(x,y,z,ypr[0],ypr[1],ypr[2]);                                // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_LVLH2GTOD(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
  FT_GTOD2Equat(x,y,z,xtime);                                              // Parameters: xtime is GPS
  FT_Cart2Angular( x,  y,  z,r, dec,  ra);
  FT_Equat2Gal(ra, dec);
   /* result */ 
  l=ra*180./pi;
  b=dec*180./pi;
  return 0;
}


int get_ams_gtod_fromGTOD(double AMS_x, double AMS_y,double AMS_z, double & theta, double &phi, double PosISS[3], double VelISS[2], double ypr[3], double xtime){
// SDT(sept2012) - generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
// AMS_x, AMS_y, AMS_z = Particle/Photon arrival direction in AMS frame (cartesian) 
// l, b                = galactic longitude and latitude of Particle/Photon arrival direction in J2000 frame in degree
// PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
// VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
// ypr[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
// xtime     = UTC time (in unix format)
  
  double x=AMS_x; double y=AMS_y; double z=AMS_z;
  double r=1.;//dummy for check
  double ra,dec;

  FT_AMS2Body(x,y,z);
  FT_Body2LVLH(x,y,z,ypr[0],ypr[1],ypr[2]);                                // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_LVLH2GTOD(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
   /* result */ 
  theta=acos(z)*180./pi;
  phi=atan2(y,x)*180./pi;
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
  ST_CAM2AMS(x, y, z, CAM_Yaw, CAM_Roll);  
  ST_tangPlane2CAM( x, y, z, -ST_Orient-pi);
  ST_ECI2tangPlane( x, y, z, ST_RA, ST_dec);
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
void	FT_GTOD2LVLH(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta ){
  /** convert the position from GTOD to LVLH
      note: for the purpose of this procedure ISS 
      and SUN MUST be given in the same ref sistem
      - in this case GTOD -*/
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
  //const double Re=6378.137; /*Earth Equatorial radius in km*/
  FT_Angular2Cart(ISSaltitude,ISSdec,ISSasc,ISSpos_x,ISSpos_y,ISSpos_z);
  ISSpos_mag=sqrt(ISSpos_x*ISSpos_x+ISSpos_y*ISSpos_y+ISSpos_z*ISSpos_z);
   /* unity vectors */
  double R_x = cos(ISSasc ) * sin( pi/2.- ISSdec );
  double R_y = sin( ISSasc ) * sin( pi/2.-ISSdec );
  double R_z = cos( pi/2.-ISSdec );  /* %%%%%%%%%%%%% corretto -> controlla bastian*/
//std::cout<<ISSpos_y/ISSpos_mag<<" "<<R_y<<std::endl;
  double V_x = cos( ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
  double V_y = sin(ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
  double V_z = cos( pi/2.-ISSVelTheta );
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


void	FT_LVLH2GTOD(double &x, double &y, double &z, double ISSaltitude,double ISSasc,double ISSdec,double ISSVelPhi, double ISSVelTheta ){
   double ISSpos_x,ISSpos_y, ISSpos_z, ISSpos_mag;
   FT_Angular2Cart(ISSaltitude,ISSdec,ISSasc,ISSpos_x,ISSpos_y,ISSpos_z);
   ISSpos_mag=sqrt(ISSpos_x*ISSpos_x+ISSpos_y*ISSpos_y+ISSpos_z*ISSpos_z);
   /* define the ISS coordinates and velocity*/
   double R_x = cos(ISSasc ) * sin( pi/2.- ISSdec );
   double R_y = sin( ISSasc ) * sin( pi/2.-ISSdec );
   double R_z = cos( pi/2.-ISSdec );  
   double V_x = cos( ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
   double V_y = sin(ISSVelPhi ) * sin(pi/2.- ISSVelTheta );
   double V_z = cos( pi/2.-ISSVelTheta );
   
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
  double JDtime;
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

int  FT_Equat2Gal(double &azimut, double &elev){
// SDT(Aug 2012) - converts from J2000 frame to Galactic:
// from Duffett-Smith & Zwart, Practical Astronomy, 4th edition 2011, pp56-59  */
 double RA=azimut;
 double Dec=elev;
 double b;/* galactic latitude  */
 double l;/* galactic longitude */
 double GalCen_RA =(192.+15./60.)/180.*pi; /* 192deg 15' */
 double GalCen_Dec=( 27.+24./60.)/180.*pi; /*  27deg 24' */
 double GalCen_AscendingNode=33./180.*pi;  /*  33deg  0' */
 b=asin(cos(Dec)*cos(GalCen_Dec)*cos(RA-GalCen_RA) + sin(Dec)*sin(GalCen_Dec));
 l=atan2( (sin(Dec)-sin(b)*sin(GalCen_Dec)),(cos(Dec)*sin(RA-GalCen_RA)*cos(GalCen_Dec) )  )+GalCen_AscendingNode;
 

// SDT(sept2012) check if  galactic longitude is in the range [-180:180]
 if (l>pi){ //if longitude is >180deg then substract 360deg
 l+=-twopi;
 }
 // SDT(oct2012) - correct output
 azimut=l;
 elev=b;
 return 1;
 
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
//   FT_LVLH2GTOD(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
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
//   FT_LVLH2GTOD(x, y, z, PosISS[0], PosISS[1], PosISS[2] , VelISS[0], VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
//   FT_GTOD2Equat(x, y, z, xtime);                                                  // Parameters: xtime is GPS
//   FT_Cart2Angular(x, y, z,r, dec,  ra);
//   return 0;
// }
