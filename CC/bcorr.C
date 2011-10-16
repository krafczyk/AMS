//  $Id: bcorr.C,v 1.7 2011/10/16 09:10:07 choutko Exp $
#ifdef __ROOTSHAREDLIBRARY__
#else
#include "event.h"
#include "bcorr.h"
#include "commons.h"
#endif
extern "C" void bzcorr_(float& factor){
#ifdef __ROOTSHAREDLIBRARY__
#else
 factor=MISCFFKEY.BZCorr;
#endif
}
extern "C" void btempcor_(float& factor) {
#ifdef __ROOTSHAREDLIBRARY__
#else
// Default in absence of corrections
  factor = 1.;
  if(!MISCFFKEY.BTempCorrection)return;

// evaluate temperature
  float temp=MagnetVarp::getmeanmagnetmtemp();
  if (temp<=0. || temp>=50.) return;

// apply the correction
  const float dBdT=0.13e-3;
  const float TETH=185;
  factor = 1.-dBdT*(temp-TETH);

//  static int oldevent = -1;
//  if (AMSEvent::gethead()->getEvent() != oldevent) {
//    oldevent = AMSEvent::gethead()->getEvent();
//    time_t utime = AMSEvent::gethead()->gettime();
//    cout << "Event= " << oldevent << " Time= " << utime;
//    cout << " MagTemp= " << temp << " BtempCor= " << factor << endl;
//  }
#endif
}

#ifdef __ROOTSHAREDLIBRARY__
#else
MagnetVarp::MagnetTemperature MagnetVarp::mgtt;
#endif