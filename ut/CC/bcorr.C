//  $Id: bcorr.C,v 1.4 2001/01/22 17:32:18 choutko Exp $
#include <event.h>
#include <bcorr.h>
#include <commons.h>
extern "C" void bzcorr_(float& factor){
 factor=MISCFFKEY.BZCorr;
}
extern "C" void btempcor_(float& factor) {

// Default in absence of corrections
  factor = 1.;
  if(!MISCFFKEY.BTempCorrection)return;

// evaluate temperature
  float temp=MagnetVarp::getmeanmagnetmtemp();
  if (temp<=0. || temp>=50.) return;

// apply the correction
  const float dBdT=0.13e-2;
  const float TETH=18.5;
  factor = 1.-dBdT*(temp-TETH);

//  static int oldevent = -1;
//  if (AMSEvent::gethead()->getEvent() != oldevent) {
//    oldevent = AMSEvent::gethead()->getEvent();
//    time_t utime = AMSEvent::gethead()->gettime();
//    cout << "Event= " << oldevent << " Time= " << utime;
//    cout << " MagTemp= " << temp << " BtempCor= " << factor << endl;
//  }

}

MagnetVarp::MagnetTemperature MagnetVarp::mgtt;
