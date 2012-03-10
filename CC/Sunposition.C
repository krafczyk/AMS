/** Class to obtain the Sun Position in different coordinates Frames
 *  Author Stefano Della Torre
 *  Mail: stefano.dellatorre@mib.infn.it
 *  Date: Feb. 2012
 */
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>

#include <Sunposition.h>

/************************************** in predict **************************/
#define twopi		6.28318530717958623	/* 2*Pi  */
#define pi		3.14159265358979323846	/* Pi */
#define pio2		1.57079632679489656	/* Pi/2 */
#define x3pio2		4.71238898038468967	/* 3*Pi/2 */
#define twopi		6.28318530717958623	/* 2*Pi  */
//#define AU		1.49597870691E8	/* Astronomical unit - km (IAU 76) */
#define AU		14959787070030.  /*Astronomical unit - cm - Particle Physics booklet 2010*/

/** Constructor */
SunPosition::SunPosition()
{
  ISSdef=0;
  time=0;
}

/** Destructor */
SunPosition::~SunPosition()
{
}


int SunPosition::Sign(double arg)
{
	/* Returns sign of a double */
	if (arg>0)
		return 1;
	else if (arg<0)
		return -1;
	else
		return 0;
}

double SunPosition::AcTan(double sinx, double cosx)
{
	/* Four-quadrant arctan function */
	if (cosx==0.0)
	{
		if (sinx>0.0)
			return (pio2);
		else
			return (x3pio2);
	}
	else
	{
		if (cosx>0.0)
		{
			if (sinx>0.0)
				return (atan(sinx/cosx));
			else
				return (twopi+atan(sinx/cosx));
		}
		else
			return (pi+atan(sinx/cosx));
	}
}
double SunPosition::ArcSin(double arg)
{
	/* Returns the arcsine of the argument */
	if (fabs(arg)>=1.0)
		return(Sign(arg)*pio2);
	else
	return(atan(arg/sqrt(1.0-arg*arg)));
}
double SunPosition::ArcCos(double arg)
{
	/* Returns arccosine of argument */
	return(pio2-ArcSin(arg));
}

double SunPosition::Modulus(double arg1, double arg2)
{
	/* Returns arg1 mod arg2 */

	int i;
	double ret_val;

	ret_val=arg1;
	i=ret_val/arg2;
	ret_val-=i*arg2;

	if (ret_val<0.0)
		ret_val+=arg2;

	return ret_val;
}




void	SunPosition::Cart2Angular(double x, double y, double z, double& r, double& theta, double& phi){
  /* convert cartesian coordinates into spherical coordinates 
     theta is the latitude (i.e. the angle with the equator plane
     phi is the aziutal angle*/
  r=sqrt(x*x+y*y+z*z);
  phi=AcTan(y,x);
  theta=pi/2.-acos(z/r);
}

void	SunPosition::Angular2Cart(double r, double theta, double phi, double& x, double& y, double& z ){
  /* convert spherical coordinates into cartesian coordinates 
     theta is the latitude (i.e. the angle with the equator plane
     phi is the aziutal angle*/
  x=r*sin(pi/2.-theta)*cos(phi);
  y=r*sin(pi/2.-theta)*sin(phi);
  z=r*cos(pi/2.-theta);
}

double	SunPosition::getTime(){
  /* get Time in Julian Day*/
  return time;
}
void	SunPosition::setJDTime(double itime){
  /* Set time in Julian Day*/
  time=itime;
 }
 
void     SunPosition::setGPSTime(double itime){
  /* Set GPS unix time (s)*/
  time=GPS_JD(itime);
 }
 
double  SunPosition::GPS_JD(double itime){
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

void     SunPosition::setUTCTime(double itime){ 
  /* Set UTC unix time (s)*/
  time=UTC_JD(itime);

 }
double  SunPosition::UTC_JD(double itime){
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
 
void	SunPosition::setISSGTOD( float ialtitude, float idec, float iasc, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll){
  /* set the ISS position in GTOD
     the variables are defined as in AMS-root files*/
  ISSdef=1;
  ISSaltitude=(double)ialtitude;
  ISSdec=(double)idec;
  ISSasc=(double)iasc;
  ISSVelTheta=(double) iveltheta;
  ISSVelPhi=(double) ivelphi;
  ISSyaw=(double)iyaw;
  ISSpitch=(double)ipitch;
  ISSroll=(double)iroll;
}

double	SunPosition::RoutineR2(double M, double e){
  /*routine to calculate the solution of kepler equation*/
  double E; /*solutio of Kepler equation*/ 
  double delta=0;
  E=M;
  delta=E-e*sin(E)-M;
  while(fabs(delta)>1e-6){ /* accuracy of 1e-6 radians*/
    E=E-(delta/(1.-e*cos(E)));
    delta=E-e*sin(E)-M;
  }
  return E;
}
void SunPosition::calculate_Sun(double& LongSolar, double& distSolar){
 /*Get Sun in ecliptic coordinates*/ 
 /*from "Pratical Astronomy wuth your calculator and spreadsheet" - 4th edition - 2011 - pag. 107--110*/
 double T=(time-2415020.0)/36525.;/*MDJ respect 1900 Jan 0.5*/
 double A= 100.0021359 * T;
 int    Ai=(int)A;
 //double eps_g=Modulus(279.6966778+36000.76892*T+0.0003025*T*T,360); /*Sun's mean (false) ecliptic longitude at the epoch*/
 double eps_g=Modulus(279.6966778+360.*(A-(double)Ai)+0.0003025*T*T,360); /*Sun's mean (false) ecliptic longitude at the epoch*/
 double PhiOmega_g=Modulus(281.2208444+1.719175*T+0.000452778*T*T,360); /*longitude of the Sun at Perigee */
 double e=0.01675104-0.0000418*T-0.000000126*T*T;/*eccentricity*/
 double Msun=Modulus(eps_g-PhiOmega_g,360)*pi/180.; /*Mean anomaly (radian)*/
 double E=RoutineR2( Msun,e);  /*solution of kepler equation (radian)*/
 double nu=Modulus(2.*atan(sqrt((1.+e)/(1.-e))*tan(E/2.))* 180./pi,360) ;/*true anomaly (degree)*/
 LongSolar=Modulus(nu+PhiOmega_g,360)*pi/180. ;/*geocentri ecliptic longitude (rad)*/
 distSolar=(1.-(e*e))/(1.+e*cos(nu/180.*pi)); /*distance from the sun in AU*/
 distSolar*=AU;  /*distance from the sun in cm*/
}  

// /*nutation correction to MeanObliquityEclipt: Practical Astronomy*/
// double NutatObl(double time){
//  double T=(time-2415020.0)/36525.;/*MDJ respect 1900 Jan 0.5*/
//  double A=100.0021358 * T;
//  int    Ai=(int)A;
//  double L=Modulus( 279.6967+360.0*(A-(double)Ai)   ,360);/*sun mean Longitude*/
//  double B=5.372617*T;
//  int    Bi=(int)B;
//  double MonNode=Modulus( 259.1833- 360.0*(B-(double)Bi) ,360); /* Moon Node*/
//  double Nuta=(9.2*cos(MonNode/360.*twopi )+0.5*cos((2*L)/360.*twopi)  ) ;/*arcSec*/
//  return Nuta/(60*60);/*degree*/
// }

double  SunPosition::NutatObl_obliq(double time){/*nutation correction to MeanObliquityEclipt: Astrophysical Formulae*/
 double d=(time-2451545.0);/*JD respect J2000*/
 return (+0.0026*cos((125.0-0.05295*d)*pi/180.)+0.0002*cos((200.9-1.97129*d)*pi/180.)   ); 
}
double  SunPosition::NutatObl_long(double time){/*nutation correction to Ecliptic longitude: Astrophysical Formulae*/
 double d=(time-2451545.0);/*JD respect J2000*/
 return (-0.0048*cos((125.0-0.05295*d)*pi/180.)-0.0004*cos((200.9-1.97129*d)*pi/180.)   ); 
}

double SunPosition::MeanObliquityEclipt(double time){/* Mean Obliquity for the convertion from Ecliptic to ECI:  Practical Astronomy*/
  double T=time-2451545.0;/*JD for 2000 jan 1.5 */
  T=T/36525.0; /*julian centuries*/
  double DE=T*46.815+0.0006*T*T-0.00181*T*T*T;
  DE=DE/3600;
  double obl=23.43929167-DE + NutatObl_obliq(time);
  return obl;/*degree*/
 
}

void	SunPosition::Eclipt2Equat(double time, double EclipLong, double EclipLat, double &RA, double &dec){
  /* Conversion from Ecliptic to ECI coordinates */
  double Obliq=MeanObliquityEclipt(time)*pi/180.;/*mean Obliquity in rad- J2000 ref*/
   EclipLong+=NutatObl_long(time);
  dec=asin( sin(EclipLat)*cos(Obliq)+cos(EclipLat)*sin(Obliq)*sin(EclipLong)); /*dec in equatorial coordinates - degree*/
  double y=sin( EclipLong )*cos(Obliq)-tan(EclipLat)*sin(Obliq);
  double x=cos(EclipLong);
  RA=AcTan(y,x);/*RA is in rad!!!*/
}

double SunPosition::GMST_rad(double time){
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

void	SunPosition::Equat2GTOD(double &x, double &y, double &z){
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

void	SunPosition::GTOD2LVLH(double &x, double &y, double &z){
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
  SunPos_x=SunPos_x-ISSpos_x;
  SunPos_y=SunPos_y-ISSpos_y;
  SunPos_z=SunPos_z-ISSpos_z;
  
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

void	SunPosition::LVLH2Body(double &x, double &y, double &z){
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

void	SunPosition::Body2AMS(double &x, double &y, double &z){
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
   x=x+85.598;
   y=y-2136.390;
   z=z+388.112;
   
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


int	SunPosition::GetSunFromEquat(double &edec, double &eRA){
  /*Get Sun from ECI J2000 frame */
  double LongS,r;
  double beta=0.; /*Sun is on the ecpliciptica*/
  calculate_Sun(LongS,r);
  double RA=0.;
  double dec=0.;
  Eclipt2Equat(time, LongS, beta,RA, dec);
  eRA=RA;
  edec=dec;
  return 1.;
}

int	SunPosition::GetSunFromGTOD(double &lat, double &longi){
  /*Get Sun from GTOD frame*/
  double LongS,r;
  double beta=0.; /*Sun is on the ecpliciptica*/
  calculate_Sun(LongS,r);
  double RA=0.;
  double dec=0.;
  Eclipt2Equat(time, LongS, beta,RA, dec);
  double x,y,z;
  Angular2Cart(r,dec,RA,x,y,z);
  Equat2GTOD(x,y,z);
  double e_elev,e_azimut;
  Cart2Angular(x,y,z,r,e_elev,e_azimut);
  lat=e_elev;
  longi=e_azimut;
  return 1;
}

int	SunPosition::GetSunFromLVLH(double &elev, double &azimut){
  /*Get Sun from LVLH frame*/
  if (ISSdef==1){
   double LongS,r;
   double beta=0.; /*Sun is on the ecpliciptica*/
   calculate_Sun(LongS,r);
   double RA=0.;
   double dec=0.;
   Eclipt2Equat(time, LongS, beta,RA, dec);
   double x,y,z;
   Angular2Cart(r,dec,RA,x,y,z);
   Equat2GTOD(x,y,z);
   GTOD2LVLH(x,y,z);
   
   double e_elev,e_azimut;
   Cart2Angular(x,y,z,r,e_elev,e_azimut);
   elev=e_elev;
   azimut=e_azimut;
   return 1;
  }
  else return 0;
}
int	SunPosition::GetSunFromAMS(double &elev, double &azimut){
  /*Get Sun from AMS frame*/
  if (ISSdef==1){
   double LongS,r;
   double beta=0.; /*Sun is on the ecpliciptica*/
   calculate_Sun(LongS,r);
   double RA=0.;
   double dec=0.;
   Eclipt2Equat(time, LongS, beta,RA, dec);
   double x,y,z;
   Angular2Cart(r,dec,RA,x,y,z);
   Equat2GTOD(x,y,z);
   GTOD2LVLH(x,y,z);
   LVLH2Body(x,y,z);
   Body2AMS(x,y,z);
   double e_elev,e_azimut;
   Cart2Angular(x,y,z,r,e_elev,e_azimut);
   elev=e_elev;
   azimut=e_azimut;
   return 1;
  }
  else return 0;
}

double SunPosition::GetBetaAngle(){
 double elev,azim,beta;
 if ((GetSunFromLVLH(elev,azim)!=0)){
  beta=-asin(cos(elev)*sin(azim));
   return beta*180./pi;
 }else
 {
   return 999.;
 }
  
  
}


int SunPosition::ISSday_night(){
 /* Day night flag, in spherical approximation
  * 1=Day 0=night 
  * this function test if the Sun Elevation in LVLH is
  * inside the Earth circle viewed around +Z Axis  */
 double elev,azim,beta;
 double Rterr=637816000; // Earth radius in cm
 if ((GetSunFromLVLH(elev,azim)!=0)){
  if (elev>(acos(Rterr/ISSaltitude))){
      return 0; }  // night
   else
     {return 1;}  // day
  }else
 {
   return -1;
 }
}
