#include <event.h>
#include <bcorr.h>
extern "C" void btempcor_(float& factor) {

// Default in absence of corrections
  factor = 1.;
  if(!MISCFFKEY.BTempCorrection)return;

// evaluate temperature
  float temp=MagnetVarp::getmeanmagnetmtemp();

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
