/**
   this is library to convert from/to AMS reference using
   ground measurementrs or startracker.
   
 *  Author Stefano Della Torre - INFN Milano Bicocca
 *  Mail: stefano.dellatorre@mib.infn.it
 *  Date: Aug 2012
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <math.h>
using namespace std;

#include <FrameTrans.h>

/************************************** Constant **************************/
#define twopi		6.28318530717958623	/* 2*Pi  */
#define pi		3.14159265358979323846	/* Pi */
#define pio2		1.57079632679489656	/* Pi/2 */
#define x3pio2		4.71238898038468967	/* 3*Pi/2 */

//#define AU		1.49597870691E8	/* Astronomical unit - km (IAU 76) */
#define AU		14959787070030  /*Astronomical unit - cm - Particle Physics booklet 2010*/

/** *************** TO BE USED BY USER ********************/




int get_ams_ra_dec_fromGTOD(double PosISS[3], double VelISS[2], double rpy[3], double & ra, double &dec, double xtime){
  /* generic trasformation from AMS frame to J2000 frame passing via GTOD (or ~ctrs)
   * PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
   * VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
   * rpy[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
   * ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
   * xtime     = UTC time (in unix format)*/
  
  double x=0.; double y=0.; double z=1.;
  double r=1.;//dummy for check
  
  FT_Angular2Cart( r,  dec,  ra,  x,  y,  z );
  FT_AMS2Body(x,y,z);
  FT_Body2LVLH(x,y,z,rpy[0],rpy[1],rpy[2]);                                // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_LVLH2GTOD(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
  FT_GTOD2Equat(x,y,z,xtime);                                              // Parameters: xtime is GPS
  FT_Cart2Angular( x,  y,  z,r, dec,  ra);
  
}

int get_ams_l_b_fromGTOD(double PosISS[3], double VelISS[2], double rpy[3], double & Azim, double &Elev, double xtime){
  /* generic trasformation from AMS frame to galactic frame passing via GTOD (or ~ctrs)
   * PosISS[3] = posizion of ISS in r,azimut[i.e. longitude],elev[i.e. latitude] 
   * VelISS[2] = Velocity of ISS in  ISSVelPhi, ISSVelTheta
   * rpy[3]    = attitude wtr LVLH in ISSyaw,  ISSpitch,  ISSroll 
   * ra, dec   = azimut and elevation direction of arrival particle -> RA and DEC direction of arrival particle
   * xtime     = UTC time (in unix format)*/
  
  double x=0.; double y=0.; double z=1.;
  double r=1.;//dummy for check
  double ra,dec;
  double l,b;
  
  FT_Angular2Cart( r,  Elev,  Azim,  x,  y,  z );
  FT_AMS2Body(x,y,z);
  FT_Body2LVLH(x,y,z,rpy[0],rpy[1],rpy[2]);                                // Parameters: ISSyaw,  ISSpitch,  ISSroll
  FT_LVLH2GTOD(x,y,z,PosISS[0],PosISS[1],PosISS[2] ,VelISS[0],VelISS[1] ); // Parameters: r,azimut,elev,ISSVelPhi,ISSVelTheta
  FT_GTOD2Equat(x,y,z,xtime);                                              // Parameters: xtime is GPS
  FT_Cart2Angular( x,  y,  z,r, dec,  ra);
  FT_Equat2Gal(ra, dec);
  l=ra;
  b=dec;
  /* result */
  Azim=l;
  Elev=b;
  
  
}
/** **************** STAR TRACKER *************************/

/* To Be Check */

/** **************** GROUND SEGMENT   ************************/

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
   *  AMS in inclined along the YZ plane of a=12.0001Â° 
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
  Angular2Cart(ISSaltitude,ISSdec,ISSasc,ISSpos_x,ISSpos_y,ISSpos_z);
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
   Angular2Cart(ISSaltitude,ISSdec,ISSasc,ISSpos_x,ISSpos_y,ISSpos_z);
   ISSpos_mag=sqrt(ISSpos_x*ISSpos_x+ISSpos_y*ISSpos_y+ISSpos_z*ISSpos_z);
   /* define the ISS coordinates and velocity*/
   double R_x = cos(ISSasc ) * sin( pi/2.- ISSdec );
   double R_y = sin( ISSasc ) * sin( pi/2.-ISSdec );
   double R_z = cos( pi/2.-ISSdec );  /* %%%%%%%%%%%%% corretto -> controlla bastian*/
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
 time=  FT_UTC_JD(double timeUnix); //convert from unixtime (UTC) to Julian days
 //time=  FT_GPS_JD(double timeUnix); //convert from unixtime (GPS) to Julian days
 /* Greenwich mean sidereal time  in radians */
 double T=(time-2451545.0)/36525.0 ;/*Interval of time, measured in Julian centuries of 36525 days of UT (mean solar day), elapsed since the epoch 2000 Jan 1d12hUT*/
 double d=(time-2451545.0);
 double GMST_deg;/*greenwich mean sidereal time in degree (i.e. the Greenwich hour angle of the mean equinox of date)*/
 /* GMST from Meeus, J., 2000. Astronomical Algorithms. Willman-Bell, Richmond,VA, 2nd ed.  p 84 (eq.11-4)
    adapdet from IDL procedure http://idlastro.gsfc.nasa.gov/ftp/pro/astro/ct2lst.pro */
 GMST_deg=Modulus(( 280.46061837 + 360.98564736629*d +T*T*(0.000387933-T/38710000.0)),360);
 double GMSTrad= GMST_deg*pi/180.; /* greenwich mean sidereal time  in radians*/
 return GMSTrad;
}
void	FT_GTOD2Equat(double &x, double &y, double &z, double time){
  /* Conversion from ECI to GTOD coordinates */
  double oldX=x;
  double oldY=y;
  double oldZ=z;
  double alpha_g=0;
  
  alpha_g=GMST_rad(time);
  
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
  
  alpha_g=GMST_rad(time);
  
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


double  FT_UTC_JD(double itime){
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


/***********************************************************************/

int  FT_Equat2Gal(double &azimut, double &elev){
 /* converts from J2000 frame to Galactic:
  * from Duffett-Smith & Zwart, Practical Astronomy, 4th edition 2011, pp56-59  */
 double RA=azimut;
 double Dec=elev;
 double b;/* galactic latitude  */
 double l;/* galactic longitude */
 double GalCen_RA =(192.+15./60.)/180.*pi; /* 192° 15' */
 double GalCen_Dec=( 27.+24./60.)/180.*pi; /*  27° 24' */
 double GalCen_AscendingNode=33./180.*pi;  /*  33°  0' */
 b=asin(cos(Dec)*cos(GalCen_Dec)*cos(RA-GalCen_RA) + sin(Dec)*sin(GalCen_Dec));
 l=atan2( (sin(Dec)-sin(b)*sin(GalCen_Dec)),(cos(Dec)*sin(RA-GalCen_RA)*cos(GalCen_Dec) )  )+GalCen_AscendingNode;
 
 azimut=l;
 elev=b;
 return 1;
 
}

int  FT_Gal2Equat(double &azimut, double &elev){
 /* converts from Galactic to J2000 frame :
  * from Duffett-Smith & Zwart, Practical Astronomy, 4th edition 2011, pp56-59  */
 double RA;
 double Dec;
 double b=azimut;/* galactic latitude  */
 double l=elev;  /* galactic longitude */
 
 double GalCen_RA =(192.+15./60.)/180.*pi; /* 192° 15' */
 double GalCen_Dec=( 27.+24./60.)/180.*pi; /*  27° 24' */
 double GalCen_AscendingNode=33./180.*pi;  /*  33°  0' */
 
 
 Dec=asin(cos(b)*cos(GalCen_Dec)*sin(l-GalCen_AscendingNode) + sin(b)*sin(GalCen_Dec));
 RA=atan2( (cos(b)*cos(l-GalCen_AscendingNode)),(sin(b)cos(GalCen_Dec)-cos(b)*sin(GalCen_Dec)*sin(l-GalCen_AscendingNode) )  )+GalCen_RA;
 
 azimut=RA;
 elev=Dec;
 return 1;
 
}
