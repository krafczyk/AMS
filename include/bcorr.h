//  $Id: bcorr.h,v 1.2 2001/01/22 17:32:42 choutko Exp $
#ifndef __AMSMFIELD__
#define __AMSMFIELD__
#include <typedefs.h>

extern "C" void btempcor();
#define BTEMPCOR btempcor_

class MagnetVarp {
//
private:
 class MagnetTemperature{
  public:
  uinteger mtemp;
  uinteger nmeas;
  uinteger nsens;
  uinteger stemp[16];
 };
 static MagnetTemperature mgtt;
//
public:
  MagnetVarp(){};
// member functions :
//
  static MagnetTemperature * getmagnettp(){return &mgtt;}
  static integer getmagnettsize(){return sizeof(mgtt);}
  static geant getmeanmagnetmtemp(){return (float)mgtt.mtemp/10.;}
  static geant getmeanmagnetstemp(int sensor){return (float)mgtt.stemp[sensor]/10.;}
};

#endif
