/** Class to obtain the Sun Position in different coordinates Frames
 *  Author Stefano Della Torre
 *  Mail: stefano.dellatorre@mib.infn.it
 *  Date: Feb. 2012
 */

#include <iostream>



class SunPosition
{
    /* > theta is the  angle respect the equator (also elev or edec, )
       > phi is the azimut angle (alse eRA)
       > the ISS coordinated and attitude are given in the same form of root-file
       > GetSun.. function results 0 if some error occurs, 1 if succesfull
       > all angles are in radians
       > GetBetaAngle results is in degree, it get 999 if some error occurs,
     */
public:
  /** Constructor */
  SunPosition();

  /** Destructor  */
  ~SunPosition();
  double getTime(); /* get Time in Julian Day*/
  void   setJDTime(double itime); /* Set time in Julian Day*/
  void   setGPSTime(double itime); /* Set GPS unix time (s)*/
  void   setUTCTime(double itime); /* Set UTC unix time (s)*/
  void	 setISSGTOD( float ialtitude, float idec, float iasc, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll); /* set the ISS position in GTOD*/
  int	 GetSunFromAMS(double &elev, double &azimut); 	   /*Get Sun from AMS frame*/
  int	 GetSunFromLVLH(double &elev, double &azimut);	   /*Get Sun from LVLH frame*/
  int	 GetSunFromGTOD(double &lat, double &longi);       /*Get Sun from GTOD frame*/
  int	 GetSunFromEquat(double &edec, double &eRA);       /*Get Sun from ECI J2000 frame */
  void	 calculate_Sun(double &LongSolar, double  &distSolar); /*Get Sun in ecliptic coordinates*/
  double GetBetaAngle(); /* Evaluate the Beta Angle (in degree)*/
  int    ISSday_night(); /* Day(1)/night(0) flag, in spherical approximation, */
protected:



private:
  int 	 ISSdef;
  double time; 
  double ISSaltitude; /* in cm !! */
  double ISSdec;
  double ISSasc;
  double ISSVelPhi;
  double ISSVelTheta;
  double ISSyaw;
  double ISSpitch;
  double ISSroll;
  
  int Sign(double arg);/* Returns sign of a double */
  double AcTan(double sinx, double cosx);/* Four-quadrant arctan function */
  double ArcSin(double arg);/* Returns the arcsine of the argument */
  double ArcCos(double arg);/* Returns arccosine of argument */
  double Modulus(double arg1, double arg2);/* Returns arg1 mod arg2 */
  void	 Cart2Angular(double x, double y, double z, double &r, double &theta, double &phi);/* convert cartesian coordinates into spherical coordinates  */
  void	 Angular2Cart(double r, double theta, double phi, double& x, double& y, double& z );/* convert spherical coordinates into cartesian coordinates */ 
  double GPS_JD(double itime);/*convert GPS time (s) in JD day*/ 
  double UTC_JD(double itime);/* convert UTC time (given in unix time seconds) in JD day*/
  double RoutineR2(double M, double e);/* routine to calculate the solution of kepler equation*/
  double NutatObl_obliq(double time);/*nutation correction to MeanObliquityEclipt*/
  double NutatObl_long(double time);/*nutation correction to Ecliptic longitude*/  
  double MeanObliquityEclipt(double time);/* Mean Obliquity for the convertion from Ecliptic to ECI*/
  double GMST_rad(double time);/* Greenwich mean sidereal time  in radians */
  void	 Eclipt2Equat(double time, double EclipLong, double EclipLat, double &RA, double &dec);/* Conversion from Ecliptic to ECI coordinates */
  void	 Equat2GTOD(double &x, double &y, double &z);/* Conversion from ECI to GTOD coordinates */
  void	 GTOD2LVLH(double &x, double &y, double &z);/* convert the position from GTOD to LVLH*/
  void	 LVLH2Body(double &x, double &y, double &z);/* move from LVLH to ISS ref System*/
  void	 Body2AMS(double &x, double &y, double &z);/* convert from ISS-Body ref System to AMS-02 reference System*/
  
};
