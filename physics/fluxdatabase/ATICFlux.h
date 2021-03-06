//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  ATICFlux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _ATICFlux_
#define _ATICFlux_
#include "TGraphErrors.h"
/// TGraphErrors*  HEFLUXRATIC02  : ATIC02 helium flux VS Rigidity


//#   He Flux in Rigidity[GV] ATIC02(2003/01)
const int atic02henb=15;

double ATIC02HEX[atic02henb]={
26.3985,40.7716,63.3368,99.8468,156.853,247.857,391.36,616.861,981.862,1551.86,2461.86,3896.86,6151.86,9801.86,15501.9,
};

double ATIC02HEXE[atic02henb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

double ATIC02HEY[atic02henb]={
0.345141,0.102693,0.0303868,0.0091984,0.0028198,0.000867975,0.000263997,7.77996e-05,2.4e-05,7.83999e-06,2.5e-06,8.28e-07,2.7e-07,6.56e-08,1.812e-08,
};

double ATIC02HEYE[atic02henb]={
0.00399006,0.000599374,0.000199913,5.99895e-05,1.99986e-05,9.99972e-06,3.99996e-06,1.99999e-07,7.99999e-07,3.2e-07,1.8e-07,6.4e-08,3.2e-08,1.22e-08,5.18e-09,
};

TGraphErrors*  HEFLUXRATIC02= new TGraphErrors(atic02henb,ATIC02HEX,ATIC02HEY,ATIC02HEXE,ATIC02HEYE);

#endif

