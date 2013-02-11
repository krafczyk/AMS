//  $Id: bcorr.h,v 1.6 2013/02/11 19:34:03 choutko Exp $
#ifndef __AMSMFIELD__
#define __AMSMFIELD__
#include "typedefs.h"


class MagnetVarp {
//
public:
 class MagnetTemperature{
  public:
  uinteger nsens;
  float stemp[12]; ///<  measurements Port Ram Starboard Wake 31:UCF3     32:UCF4    35:UCF7     36:UCF8 39:LCF3      40:LCF4      43:LCF7      44:LCF8
                
  uinteger ncceb;
  float cceb[4][4]; ///<  B-Field P0 Component X,Y,Z,SensorT (P1,P2,P3)
  uinteger time; ///< last measurmeent time;
public:
  void loadValues(unsigned int time,bool cceb=false);
  int getmeanmagnettemp(float &temp,int method=1); ///< get meanmagnet temp method 0 all sensors method 1 0...3 method 2 4...11 ; return 0 if ok
  MagnetTemperature():ncceb(0),nsens(0),time(0){};
 ClassDef(MagnetTemperature,1)       //MagnetTemperature;
 };
 static MagnetTemperature mgtt;
#pragma omp threadprivate(mgtt)
//
public:
// member functions :
//
  static MagnetTemperature * getmagnettp(){return &mgtt;}
  static integer getmagnettsize(){return sizeof(mgtt);}
  /*!
  \brief Get "magnet" sensor temperature

  \param[out] factor      Magnetic field correction to be multiplied to magnetic filed and/or rigidity
  \param[in]  time        JMDC time (AMSEventR::UTime() will be used if time==0)
  \param[in]  method      0..11  get sensor #0

  \retval     0 success
  \retval     1 time==0 and no AMSEventR found
  \retval     2 unable to load temperature from slow control
  \retval     3 wrong sensor number
  \retval     4 no measurments available for given sensor
  */
  static int getmagnettemp(float &temp,unsigned int time=0,unsigned int sensor=0); ///< getmagnettemp
  static int getmagfiled(float &b,unsigned int time=0,unsigned int sensor=0,unsigned int comp=0); ///< getmagfield sensor;
  /*!
  \brief Get Magnetic field correction to be applied due to magnet temperarure change

  \param[out] factor      Magnetic field correction to be multiplied to magnetic filed and/or rigidity
  \param[in]  time        JMDC time (AMSEventR::UTime() will be used if time==0)
  \param[in]  method      0:  use all 12 sensors
                          1:  (default) use 4 sensors ("Port" "Ram" "Starboard" "Wake" only)
                          2:  use other 8 sensors
                          3:  use magnetic filed measurement (not implemented)
                          -1 to -12  use only sensor 1 to 12 for correction

  \retval     0 success
  \retval     1 time==0 and no AMSEventR found
  \retval     2 unable to load temperature from slow control
  \retval     3 only one measurment sensor available when method 0..2
  \retval     4 no measurments available for sensors in range
  */
  static int btempcor(float &factor,unsigned int time=0,int method=1); ///< method 0..2 like in getmeanmagnettemp method 3 direct magfield estimation ; return 0 if ok

 ClassDef(MagnetVarp,1)       //MagnetVarp;
};


#endif
