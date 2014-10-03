//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  BESSFlux
///\data  2014/09/30 Q.Yan Created
///\data  2014/10/01 SH proton flux (Rig/Ekin) and He flux (Ekin) added
///
//////////////////////////////////////////////////////////////////////////
#ifndef _BESSFlux_
#define _BESSFlux_

/// TGraphErrors*  PRFLUXEBESSTEV  : BESS-TeV proton flux VS Ekin
/// TGraphErrors*  HEFLUXEBESSTEV  : BESS-TeV helium flux VS Ekin

//#   proton Flux in Ekin[GeV] BESS-TeV(2002/08), 1H assumed
//#   Haino et al., PRB 594, 35 (2004)

const int besstevprnb=37;
double BESSTEVPRENX[besstevprnb]={
  1.08,1.26,1.47,1.71,2.00,2.33,2.71,3.16,3.69,4.30,5.01,5.84,6.81,7.93,9.25,10.8,12.6,14.7,17.1,19.9,23.2,27.1,31.6,36.8,42.9,50.0,58.3,68.0,79.2,92.3,112.,140.,177.,222.,281.,352.,463.};

double BESSTEVPRENXE[besstevprnb]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double BESSTEVPRENY[besstevprnb]={
  3.50e2,3.22e2,2.98e2,2.71e2,2.41e2,2.08e2,1.74e2,1.45e2,1.19e2,9.52e1,7.35e1,5.63e1,4.34e1,3.15e1,2.25e1,1.59e1,1.12e1,7.71,5.33,3.63,2.48,1.62,1.09,7.17e-1,4.84e-1,3.15e-1,2.07e-1,1.34e-1,9.09e-2,5.75e-2,3.43e-2,1.98e-2,1.00e-2,5.42e-3,2.46e-3,1.62e-3,7.47e-4};

double BESSTEVPRENYE[besstevprnb]={
  0.114018e2,0.104403e2,0.0921954e2,0.0824621e2,0.0728011e2,0.0632456e2,0.0509902e2,0.0412311e2,0.0316228e2,0.281603e1,0.221359e1,0.167631e1,0.126491e1,0.0948683e1,0.067082e1,0.0509902e1,0.0412311e1,0.282843,0.202237,0.141421,0.10198,0.0608276,0.0412311,0.310483e-1,0.218403e-1,0.148661e-1,0.0948683e-1,0.067082e-2,0.470106e-2,0.31305e-2,0.21095e-2,0.130384e-2,0.0781025e-2,0.460109e-3,0.254951e-3,0.19799e-3,1.04139e-4};

TGraphErrors*  PRFLUXEBESSTEV= new TGraphErrors(besstevprnb,BESSTEVPRENX,BESSTEVPRENY,BESSTEVPRENXE,BESSTEVPRENYE);


//#   He Flux in Ekin/n[GeV/n] BESS-TeV(2002/08), 4He assumed
//#   Haino et al., PRB 594, 35 (2004)
const int besstevhenb=30;

double BESSTEVHEENX[besstevhenb]={
  1.08,1.26,1.47,1.71,2.00,2.33,2.71,3.16,3.69,4.29,4.98,5.84,6.80,7.94,9.24,10.8,12.6,14.7,17.0,19.9,23.2,27.1,31.4,36.7,42.9,49.9,62.5,86.1,116.,175};

double BESSTEVHEENXE[besstevhenb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double BESSTEVHEENY[besstevhenb]={
  5.22e1,4.78e1,4.02e1,3.21e1,2.62e1,2.17e1,1.81e1,1.37e1,9.77,7.67,5.71,3.98,2.83,2.07,1.48,1.02,6.76e-1,4.71e-1,3.27e-1,2.13e-1,1.46e-1,9.67e-2,5.89e-2,4.23e-2,2.85e-2,1.84e-2,9.40e-3,4.14e-3,2.16e-3,5.53e-4};

double BESSTEVHEENYE[besstevhenb]={
  0.386264e1,0.354683e1,0.300832e1,0.237697e1,0.196977e1,0.161555e1,0.13e1,0.0984886e1,0.75591,0.599416,0.452769,0.325576,0.223607,0.162788,0.123693,0.0824621,0.563205e-1,0.398497e-1,0.278568e-1,0.183848e-1,0.13e-1,0.890056e-2,0.566039e-2,0.424382e-2,0.3e-2,0.198494e-2,1.28701e-3,0.670075e-3,0.396611e-3,1.12071e-4};

TGraphErrors*  HEFLUXEBESSTEV= new TGraphErrors(besstevhenb,BESSTEVHEENX,BESSTEVHEENY,BESSTEVHEENXE,BESSTEVHEENYE);

#endif
