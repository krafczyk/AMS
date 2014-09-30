//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  PAMELAFlux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _PAMELAFlux_
#define _PAMELAFlux_

//#   He Flux in Rigidity[GV] PAMELA(2006/07-2008/12)
const int pamelahenb=84;

double PAMELAHEX[pamelahenb]={
1.01,1.06,1.11,1.16,1.22,1.27,1.33,1.4,1.46,1.53,1.6,1.68,1.76,1.84,1.93,2.02,2.11,2.21,2.32,2.43,2.54,2.66,2.79,2.92,3.06,3.2,3.35,3.51,3.67,3.85,4.03,4.22,4.42,4.63,4.85,5.07,5.44,5.96,6.54,7.17,7.86,8.62,9.45,10.37,11.37,12.46,13.67,14.99,16.43,18.02,19.76,21.67,23.76,26.05,28.56,31.32,34.34,37.66,41.29,45.28,49.64,54.44,59.69,65.45,71.76,78.69,86.28,94.6,106.11,121.83,139.88,160.61,184.4,211.72,243.09,279.11,320.46,367.94,422.46,485.05,582.45,749.71,1010.33,1010.33,
};

double PAMELAHEXE[pamelahenb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

double PAMELAHEY[pamelahenb]={
42.4,53.2,63.4,74,81.4,88.4,92.7,96,97.1,97.3,98.1,96.9,95,92.5,90.6,88.5,85,80.8,77,73.4,69.9,65.4,61.7,57,53.4,49.8,46,42.4,38.8,36.1,32.9,30.2,27.6,25.13,22.89,20.73,18.08,14.69,12.03,9.71,7.87,6.36,5.05,4.03,3.22,2.59,2.031,1.619,1.292,1.003,0.79,0.633,0.486,0.38,0.304,0.233,0.184,0.143,0.112,0.0896,0.0677,0.0519,0.0407,0.0318,0.0249,0.019,0.0147,0.0121,0.0086,0.0058,0.00398,0.00275,0.00186,0.00121,0.00078,0.00056,0.00041,0.00028,0.0002,0.00014,8.4e-05,5.1e-05,3e-05,3e-05,
};

double PAMELAHEYE[pamelahenb]={
2.03961,2.03961,2.03961,3.02655,3.02655,3.02655,3.02655,4.01995,4.01995,4.01995,4.01995,4.01995,4.01995,3.01496,3.01496,3.01496,3.01496,3.01496,3.01496,3.00666,3.00666,3.00666,2.00997,2.00997,2.00997,2.00997,2.00997,2.0025,2.0025,1.00499,1.00499,1.00499,1.00499,0.904489,0.903549,0.80399,0.701783,0.601332,0.500899,0.401123,0.301496,0.200998,0.200998,0.200998,0.100499,0.100499,0.080399,0.0604069,0.0503587,0.0403113,0.0302655,0.0202237,0.0202237,0.010198,0.010198,0.00921954,0.00707107,0.00509902,0.00412311,0.00313209,0.00308058,0.00208806,0.00206155,0.00107703,0.00107703,0.000761577,0.00067082,0.000538516,0.000360555,0.000223607,0.000219317,0.000122066,9.43398e-05,6.40312e-05,4.24264e-05,3.60555e-05,2.82843e-05,2.23607e-05,1e-05,1.28063e-05,8.48528e-06,7.07107e-06,8.60232e-06,8.60232e-06,
};

TGraphErrors*  HEFLUXRPAMELA= new TGraphErrors(pamelahenb,PAMELAHEX,PAMELAHEY,PAMELAHEXE,PAMELAHEYE);

//#   He Flux in Rigidity[GV] PAMELA-CALO(2006/06-2010/01)
const int pamelacalohenb=6;

double PAMELACALOHEX[pamelacalohenb]={146.186,288.16,571.551,1113.69,2214,4464.14};

double PAMELACALOHEXE[pamelacalohenb]={0,0,0,0,0,0};

double PAMELACALOHEY[pamelacalohenb]={0.00316974,0.000564988,0.0001035,2.44e-05,4.71e-06,5.65e-07};

double PAMELACALOHEYE[pamelacalohenb]={0.000509418,0.00013437,2.07963e-05,4.11398e-06,6.8738e-07,7.8924e-08};

TGraphErrors*  HEFLUXRPAMELACALO= new TGraphErrors(pamelacalohenb,PAMELACALOHEX,PAMELACALOHEY,PAMELACALOHEXE,PAMELACALOHEYE);


#endif
