/** Class to obtain the Sun Position in different coordinates Frames
 *  Author Stefano Della Torre
 *  Mail: stefano.dellatorre@mib.infn.it
 *  Date: Feb. 2012 - rev. Nov 2012
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
  /// general Constructor 
  SunPosition();
  /// Constructor that inizialize the object
  SunPosition(double itime, float ialtitude, float ilatitude, float ilongitude, float ivelAng, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll);
  /// Get Sun from AMS frame. it return 0 if ISS attitude is not defined
  int	 GetSunFromAMS(double &elev, double &azimut); 	 
  /// Get Sun position from LVLH frame. it return 0 if ISS attitude is not defined
  int	 GetSunFromLVLH(double &elev, double &azimut);	   /*Get Sun from LVLH frame*/
  /// Get Sun from GTOD frame
  int	 GetSunFromGTOD(double &lat, double &longi);       /*Get Sun from GTOD frame*/
  /// Get Sun from ECI J2000 frame
  int	 GetSunFromEquat(double &edec, double &eRA);       /*Get Sun from ECI J2000 frame */
  /// Evaluate the Beta Angle (in degree). if ISS position is not defined return 999.
  double GetBetaAngle();
  ///  Day(1)/night(0) flag, in spherical approximation. if ISS position is not defined return -1.
  int    ISSday_night(); 
  /// Destructor 
  ~SunPosition();
  /// get Time converted in in Julian Day 
  double getTime(); 
  /// Set time in Julian Day
  void   setJDTime(double itime); 
  /// Set GPS unix time (s)
  void   setGPSTime(double itime);
  /// Set UTC unix time (s)
  void   setUTCTime(double itime); 
  /// set the ISS position, velocity and attitude in GTOD
  void	 setISSGTOD( float ialtitude, float ilatitude, float ilongitude, float ivelAng, float iveltheta, float ivelphi, float iyaw, float ipitch, float iroll); 
  /// Get Sun in ecliptic coordinates (solar latitude is 0 by definition, LongSolar= Solar Longitude, distSolar= Distance Earth-Sun) at the time setted in the object
  void calculate_Sun(double& LongSolar, double& distSolar);
  
  


protected:



private:
  /// control Flag set "1" if ISS position is stored into the object
  int 	 ISSdef; 
  /// Time in second as Julian Day definition
  double time;
  /// Time in UTC Seconds
  double timeUTC;
  /// Iss x position in inertial J2000 Frame
  double ISSx;
  /// Iss y position in inertial J2000 Frame
  double ISSy;
  /// Iss z position in inertial J2000 Frame
  double ISSz;
  /// Iss Vx velocity in inertial J2000 Frame
  double ISSvx;
  /// Iss Vy velocity in inertial J2000 Frame
  double ISSvy;
  /// Iss Vz velocity in inertial J2000 Frame
  double ISSvz; 
  /// ISS attitude in LVLH: yaw angle
  double ISSyaw;
  /// ISS attitude in LVLH: pitch angle
  double ISSpitch;
  /// ISS attitude in LVLH: roll angle
  double ISSroll; 
  /// routine that calculate the solution of kepler equation
  double RoutineR2(double M, double e);
  /// nutation correction to MeanObliquityEclipt
  double  NutatObl_obliq(double time);
  /// nutation correction to Ecliptic longitude
  double NutatObl_long(double time);
  /// Mean Obliquity for the convertion from Ecliptic to ECI
  double MeanObliquityEclipt(double time);
  /// Conversion from Ecliptic to ECI coordinates
  void	Eclipt2Equat(double time, double EclipLong, double EclipLat, double &RA, double &dec);
  /// convert GPS time (given in unix time seconds) in Julian seconds
  double  GPS_JD(double itime);
  /// convert UTC time (given in unix time seconds) in Julian seconds
  double  UTC_JD(double itime);
  /// 
  
   
};
