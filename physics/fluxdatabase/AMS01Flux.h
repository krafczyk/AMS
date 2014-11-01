//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  AMS01Flux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _AMS01Flux_
#define _AMS01Flux_
#include "TGraphErrors.h"
/// TGraphErrors*  HEFLUXRAMS01  : AMS-01 helium flux VS Rigidity

//#   Pr Flux AMS01

//#   He Flux in Rigidity[GV] AMS01(1998/06)
const int ams01henb=31;

double AMS01HEX[ams01henb]={
0.831357,1.00003,1.20432,1.44319,1.73565,2.08947,2.50696,3.01381,3.62575,4.36181,5.24322,6.30243,7.58192,9.11522,10.9571,13.1724,15.8405,19.0473,22.9002,27.5358,33.107,39.8036,47.8541,57.5378,69.1789,83.1695,99.9926,120.222,144.539,173.774,208.926,
};

double AMS01HEXE[ams01henb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

double AMS01HEY[ams01henb]={
32,48.9,58.4001,62.8001,63.9,58.1999,49.3999,39.6001,30.8,22.6,15.9,11,7.28003,4.71,2.99,1.89,1.19,0.737004,0.470001,0.289,0.172,0.101,0.0632001,0.038,0.0222,0.0137,0.00829001,0.00491,0.00278,0.00165,0.00118,
};

double AMS01HEYE[ams01henb]={
16,2.9,3.19999,3.39999,3.49999,3.2,2.7,2.1,1.7,1.2,0.86,0.590001,0.39,0.25,0.16,0.1,0.0639998,0.0400001,0.026,0.016,0.00940001,0.00560001,0.00350001,0.0021,0.0012,0.000799999,0.000500003,0.00033,0.00019,0.00014,7.99998e-05,
};

TGraphErrors*  HEFLUXRAMS01= new TGraphErrors(ams01henb,AMS01HEX,AMS01HEY,AMS01HEXE,AMS01HEYE);

#endif
