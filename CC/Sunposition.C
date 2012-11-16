/** Class to obtain the Sun Position in different coordinates Frames
 *  Author Stefano Della Torre
 *  Mail: stefano.dellatorre@mib.infn.it
 *  Date: Feb. 2012 - rev. Nov. 2012
 */
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdio.h>

#include <Sunposition.h>
#include <FrameTrans.h>

/************************************** in predict **************************/
#define twopi		6.28318530717958623	/* 2*Pi  */
#define pi		3.14159265358979323846	/* Pi */
#define pio2		1.57079632679489656	/* Pi/2 */
#define x3pio2		4.71238898038468967	/* 3*Pi/2 */
#define twopi		6.28318530717958623	/* 2*Pi  */
//#define AU		1.49597870691E8	/* Astronomical unit - km (IAU 76) */
#define AU		14959787070030.  /*Astronomical unit - cm - Particle Physics booklet 2010*/
//---------------------- Public ------------------
/** Constructor */
SunPosition::SunPosition()
{
  ISSdef=0;
  time=0;
  timeUTC=0;
}


SunPosition::SunPosition(double itime, float ialtitude, float ilatitude, float ilongitude, float ivelAng, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll)
{
  ISSdef=1;
  timeUTC=itime;
  setUTCTime( itime);
  setISSGTOD( ialtitude,  ilatitude,  ilongitude,  ivelAng,  iveltheta,  ivelphi,  iyaw,  ipitch,  iroll);
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
   FT_Angular2Cart(r,dec,RA,x,y,z);
   FT_ECI2LVLH(x, y, z,  ISSx, ISSy, ISSz, ISSvx,  ISSvy, ISSvz );
   FT_LVLH2Body(x,y,z,ISSyaw,  ISSpitch,  ISSroll);
   FT_Body2AMS(x,y,z);
   double e_elev,e_azimut;
   FT_Cart2Angular(x,y,z,r,e_elev,e_azimut);
   elev=e_elev;
   azimut=e_azimut;
   return 1;
  }
  else return 0;
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
   FT_Angular2Cart(r,dec,RA,x,y,z);
   FT_ECI2LVLH(x, y, z,  ISSx, ISSy, ISSz, ISSvx,  ISSvy, ISSvz );
   double e_elev,e_azimut;
   FT_Cart2Angular(x,y,z,r,e_elev,e_azimut);
   elev=e_elev;
   azimut=e_azimut;
   return 1;
  }
  else return 0;
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
  FT_Angular2Cart(r,dec,RA,x,y,z);
  FT_Equat2GTOD(x,y,z,timeUTC);
  double e_elev,e_azimut;
  FT_Cart2Angular(x,y,z,r,e_elev,e_azimut);
  lat=e_elev;
  longi=e_azimut;
  return 1;
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
  if (elev>(acos(Rterr/sqrt(ISSx*ISSx+ISSy*ISSy+ISSz*ISSz)))){
      return 0; }  // night
   else
     {return 1;}  // day
  }else
 {
   return -1;
 }
}

/** Destructor */
SunPosition::~SunPosition()
{
}
//--------------------------- Set up and inizialize --------------
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
  timeUTC=itime+16;
  time=GPS_JD(itime);
 }
 
void     SunPosition::setUTCTime(double itime){ 
  /* Set UTC unix time (s)*/
  timeUTC=itime;
  time=UTC_JD(itime);
 }
 
void	SunPosition::setISSGTOD( float ialtitude, float ilatitude, float ilongitude, float ivelAng, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll){
  /* set the ISS position in GTOD and convert it in Inertial Frame */
  ISSdef=1;

  ISSyaw=(double)iyaw;
  ISSpitch=(double)ipitch;
  ISSroll=(double)iroll;
  
  FT_Angular2Cart(ialtitude,  ilatitude,ilongitude,ISSx, ISSy, ISSz);
  FT_Angular2Cart(ivelAng*ialtitude, iveltheta,ivelphi,ISSvx, ISSvy, ISSvz);
  FT_GTOD2Equat(ISSx, ISSy, ISSz,ISSvx,ISSvy,ISSvz, timeUTC);
 
} 


//------------------- Evaluating Sun position -------------

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
 double eps_g=FT_Modulus(279.6966778+360.*(A-(double)Ai)+0.0003025*T*T,360); /*Sun's mean (false) ecliptic longitude at the epoch*/
 double PhiOmega_g=FT_Modulus(281.2208444+1.719175*T+0.000452778*T*T,360); /*longitude of the Sun at Perigee */
 double e=0.01675104-0.0000418*T-0.000000126*T*T;/*eccentricity*/
 double Msun=FT_Modulus(eps_g-PhiOmega_g,360)*pi/180.; /*Mean anomaly (radian)*/
 double E=RoutineR2( Msun,e);  /*solution of kepler equation (radian)*/
 double nu=FT_Modulus(2.*atan(sqrt((1.+e)/(1.-e))*tan(E/2.))* 180./pi,360) ;/*true anomaly (degree)*/
 LongSolar=FT_Modulus(nu+PhiOmega_g,360)*pi/180. ;/*geocentri ecliptic longitude (rad)*/
 distSolar=(1.-(e*e))/(1.+e*cos(nu/180.*pi)); /*distance from the sun in AU*/
 distSolar*=AU;  /*distance from the sun in cm*/
}  

//--------------------- Frame transformation in ecliptic Frame ----------------
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
  //RA=AcTan(y,x);/*RA is in rad!!!*/
  RA=atan2(y,x);/*RA is in rad!!!*/
}

//--------------------------- Time Conversion ---------------

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

