//  $Id: bcorr.h,v 1.3 2002/01/11 16:09:45 choutko Exp $
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
  static geant getmeanmagnetmtemp(){return (float)mgtt.mtemp;}
  static geant getmeanmagnetstemp(int sensor){return (float)mgtt.stemp[sensor]/10.;}
};

#endif
