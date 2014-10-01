//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  PAMELAFlux
///\data  2014/09/30 Q.Yan Created
///\data  2014/10/01 SH proton flux (Rig/Ekin) and He flux (Ekin) added
///
//////////////////////////////////////////////////////////////////////////
#ifndef _PAMELAFlux_
#define _PAMELAFlux_

/// TGraphErrors*  PRFLUXRPAMELA   : PAMELA proton flux VS Rigidity
/// TGraphErrors*  PRENFLUXRPAMELA : PAMELA proton flux VS Ekin
/// TGraphErrors*  HEFLUXRPAMELA   : PAMELA helium flux VS Rigidity
/// TGraphErrors*  HEENFLUXRPAMELA : PAMELA helium flux VS Ekin
/// TGraphErrors*  PHRATIOPAMELA   : PAMELA proton/helium ratio VS Rigidity

//#   proton Flux in Rigidity[GV] PAMELA(2006/07-2008/12)
//#   Adriani et al., Science 132, 69 (2011)

const int pamelaprnb=83;

double PAMELAPRX[pamelaprnb]={
  1.01,1.06,1.11,1.16,1.22,1.28,1.34,1.4,1.47,1.54,1.61,1.68,1.76,1.85,1.93,2.02,2.12,2.22,2.32,2.43,2.55,2.67,2.79,2.92,3.06,3.21,3.36,3.52,3.68,3.86,4.04,4.23,4.43,4.74,5.2,5.7,6.25,6.86,7.52,8.24,9.04,9.91,10.86,11.91,13.06,14.32,15.7,17.22,18.88,20.7,22.7,24.89,27.29,29.92,32.81,35.97,39.44,43.25,47.42,52,57.01,62.51,68.54,75.16,82.41,90.36,101.34,116.36,133.6,153.39,176.12,202.21,232.16,266.56,306.05,359.39,451.75,582.46,749.72,1010.34
};

double PAMELAPRXE[pamelaprnb]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double PAMELAPRY[pamelaprnb]={
  1161,1161,1167,1156,1144,1130,1104,1074,1038,999,961,919,879,832,783,738,692.9,647.2,605.1,561.5,521.3,481.1,447.4,413.5,379.1,347.8,319.3,292.9,267.5,243.1,221.1,200.9,183.2,157.7,128.1,103.9,83.9,67.35,54.06,43.05,34.22,27.16,21.53,16.95,13.32,10.48,8.26,6.46,5,3.9,3.047,2.365,1.827,1.419,1.107,0.849,0.653,0.509,0.397,0.301,0.233,0.178,0.141,0.1103,0.0831,0.0633,0.0459,0.0312,0.0209,0.0142,0.0095,0.0064,0.00417,0.00297,0.00202,0.00132,0.00068,0.00038,0.00019,8.1e-05
};

double PAMELAPRYE[pamelaprnb]={
  40.05,40.05,40.05,40.05,40.05,40.05,40.05,40.05,40.05,40.0125,40.0125,40.0125,30.0167,30.0167,30.0167,30.0167,30.0135,20.0202,20.016,20.016,20.0122,20.0122,20.009,20.009,10.0125,10.0125,10.0125,10.008,10.008,9.00888,8.00562,8.00562,7.00643,6.00333,5.004,4.00125,3.00167,3.00135,2.0016,2.00122,1.0018,1.00125,0.800999,0.60075,0.500899,0.4005,0.300666,0.200998,0.20025,0.20025,0.100404,0.0902718,0.0702567,0.0502494,0.0401995,0.0302655,0.0301496,0.0202237,0.0200998,0.010198,0.00921954,0.00707107,0.00509902,0.0041,0.00310484,0.00305941,0.00203961,0.00104403,0.0008544,0.000632456,0.000447214,0.000316228,0.000215407,0.000122066,0.000102956,6.7082e-05,4.47214e-05,3.60555e-05,2.23607e-05,1.22066e-05
};

TGraphErrors*  PRFLUXRPAMELA= new TGraphErrors(pamelaprnb,PAMELAPRX,PAMELAPRY,PAMELAPRXE,PAMELAPRYE);



//#   proton Flux in Ekin[GeV] PAMELA(2006/07-2008/12), 1H assumed
//#   Adriani et al., Science 132, 69 (2011)

double PAMELAPRENX[pamelaprnb]={
0.44,0.48,0.52,0.56,0.6,0.65,0.7,0.75,0.8,0.86,0.92,0.99,1.06,1.13,1.21,1.29,1.38,1.47,1.57,1.67,1.78,1.89,2.01,2.13,2.27,2.4,2.55,2.7,2.86,3.03,3.21,3.39,3.59,3.9,4.35,4.84,5.38,5.98,6.64,7.36,8.15,9.01,9.97,11.01,12.16,13.41,14.79,16.31,17.96,19.78,21.78,23.97,26.37,29,31.88,35.05,38.52,42.32,46.49,51.07,56.08,61.58,67.61,74.22,81.47,89.42,100.41,115.42,132.66,152.46,175.18,201.27,231.23,265.62,305.12,358.45,450.82,581.52,748.78,1009.4
};

double PAMELAPRENY[pamelaprnb]={
  1581,1549,1527,1484,1444,1402,1349,1293,1233,1171,1113,1053,996,933,871,813,758,703,652.6,601.8,555.5,510,472,434.3,396.5,362.4,331.5,303.2,276,250.2,227,205.8,187.2,160.7,130.2,105.3,84.8,68,54.48,43.33,34.4,27.28,21.62,17,13.35,10.51,8.28,6.47,5.01,3.9,3.05,2.367,1.828,1.42,1.107,0.849,0.653,0.509,0.397,0.301,0.233,0.178,0.141,0.1103,0.0831,0.0633,0.0459,0.0312,0.0209,0.0142,0.0095,0.0064,0.00417,0.00297,0.00202,0.00132,0.00068,0.00038,0.00019,8.1e-05
};

double PAMELAPRENYE[pamelaprnb]={
  60.075,60.075,60.075,60.075,60.0333,50.04,50.04,50.04,50.04,40.05,40.05,40.05,40.0125,40.0125,30.0167,30.0167,30.0167,30.0167,30.0135,20.016,20.016,20.0122,20.0122,20.009,20.009,10.0125,10.0125,10.008,10.008,9.00888,9.00888,8.00562,7.00643,6.00333,5.004,4.00125,3.00167,3.00167,2.0016,2.00122,1.0018,1.00125,0.800999,0.60075,0.500899,0.4005,0.300666,0.300666,0.20025,0.20025,0.100404,0.0902718,0.0702567,0.0502494,0.0401995,0.0302655,0.0301496,0.0202237,0.0200998,0.010198,0.00921954,0.00707107,0.00509902,0.0041,0.00310484,0.00305941,0.00203961,0.00104403,0.0008544,0.000632456,0.000447214,0.000316228,0.000215407,0.000122066,0.000102956,6.7082e-05,4.47214e-05,3.60555e-05,2.23607e-05,1.22066e-05
};

TGraphErrors*  PRENFLUXRPAMELA= new TGraphErrors(pamelaprnb,PAMELAPRENX,PAMELAPRENY,PAMELAPRXE,PAMELAPRENYE);


//#   He Flux in Rigidity[GV] PAMELA(2006/07-2008/12)
//#   Adriani et al., Science 132, 69 (2011)

const int pamelahenb=83;

double PAMELAHEX[pamelahenb]={
1.01,1.06,1.11,1.16,1.22,1.27,1.33,1.4,1.46,1.53,1.6,1.68,1.76,1.84,1.93,2.02,2.11,2.21,2.32,2.43,2.54,2.66,2.79,2.92,3.06,3.2,3.35,3.51,3.67,3.85,4.03,4.22,4.42,4.63,4.85,5.07,5.44,5.96,6.54,7.17,7.86,8.62,9.45,10.37,11.37,12.46,13.67,14.99,16.43,18.02,19.76,21.67,23.76,26.05,28.56,31.32,34.34,37.66,41.29,45.28,49.64,54.44,59.69,65.45,71.76,78.69,86.28,94.6,106.11,121.83,139.88,160.61,184.4,211.72,243.09,279.11,320.46,367.94,422.46,485.05,582.45,749.71,1010.33};

double PAMELAHEXE[pamelahenb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

double PAMELAHEY[pamelahenb]={
42.4,53.2,63.4,74,81.4,88.4,92.7,96,97.1,97.3,98.1,96.9,95,92.5,90.6,88.5,85,80.8,77,73.4,69.9,65.4,61.7,57,53.4,49.8,46,42.4,38.8,36.1,32.9,30.2,27.6,25.13,22.89,20.73,18.08,14.69,12.03,9.71,7.87,6.36,5.05,4.03,3.22,2.59,2.031,1.619,1.292,1.003,0.79,0.633,0.486,0.38,0.304,0.233,0.184,0.143,0.112,0.0896,0.0677,0.0519,0.0407,0.0318,0.0249,0.019,0.0147,0.0121,0.0086,0.0058,0.00398,0.00275,0.00186,0.00121,0.00078,0.00056,0.00041,0.00028,0.0002,0.00014,8.4e-05,5.1e-05,3e-05
};

double PAMELAHEYE[pamelahenb]={
2.03961,2.03961,2.03961,3.02655,3.02655,3.02655,3.02655,4.01995,4.01995,4.01995,4.01995,4.01995,4.01995,3.01496,3.01496,3.01496,3.01496,3.01496,3.01496,3.00666,3.00666,3.00666,2.00998,2.00998,2.00998,2.00998,2.00998,2.0025,2.0025,1.00499,1.00499,1.00499,1.00499,0.904489,0.903549,0.80399,0.701783,0.601332,0.500899,0.401123,0.301496,0.200998,0.200998,0.200998,0.100499,0.100499,0.080399,0.060407,0.0503587,0.0403113,0.0302655,0.0202237,0.0202237,0.010198,0.010198,0.00921954,0.00707107,0.00509902,0.00412311,0.00313209,0.00308058,0.00208806,0.00206155,0.00107703,0.00107703,0.000761577,0.00067082,0.000538516,0.000360555,0.000223607,0.000219317,0.000122066,9.43398e-05,6.40312e-05,4.24264e-05,3.60555e-05,2.82843e-05,2.23607e-05,1.41421e-05,1.28062e-05,8.48528e-06,7.07107e-06,8.60233e-06
};

TGraphErrors*  HEFLUXRPAMELA= new TGraphErrors(pamelahenb,PAMELAHEX,PAMELAHEY,PAMELAHEXE,PAMELAHEYE);


//#   He Flux in Ekin/N[GeV/n] PAMELA(2006/07-2008/12), 4He assumed
//#   Adriani et al., Science 132, 69 (2011)

double PAMELAHEENX[pamelahenb]={
  0.13,0.14,0.15,0.17,0.18,0.2,0.21,0.23,0.25,0.27,0.3,0.32,0.35,0.38,0.41,0.44,0.47,0.51,0.55,0.6,0.64,0.69,0.74,0.8,0.85,0.92,0.98,1.05,1.12,1.2,1.28,1.37,1.46,1.56,1.66,1.77,1.94,2.19,2.46,2.77,3.1,3.47,3.88,4.33,4.82,5.36,5.96,6.61,7.33,8.12,8.98,9.93,10.98,12.12,13.37,14.75,16.26,17.91,19.73,21.72,23.9,26.29,28.92,31.8,34.95,38.42,42.21,46.37,52.12,59.98,69.01,79.37,91.27,104.93,120.61,138.62,159.3,183.03,210.29,241.59,290.29,373.92,504.23};

double PAMELAHEENY[pamelahenb]={
  179,217,249,281,300,315,320,322,316,308,302,291,278,264,252.9,241.7,227.4,212,198.1,185.6,173.7,160.1,148.8,135.5,125.4,115.4,105.4,96.2,87.1,80.3,72.7,66.2,60,54.2,49.1,44.2,38.3,30.8,25.03,20.08,16.18,13.02,10.29,8.19,6.54,5.24,4.1,3.26,2.6,2.016,1.588,1.27,0.976,0.762,0.61,0.466,0.368,0.286,0.225,0.179,0.136,0.104,0.081,0.0637,0.0499,0.038,0.0294,0.0241,0.0171,0.0117,0.008,0.0055,0.0037,0.00242,0.00155,0.00111,0.00081,0.00055,0.00039,0.00029,0.00017,0.0001,6e-05};

double PAMELAHEENYE[pamelahenb]={
  7.28011,8.24621,9.21954,10.198,10.198,10.198,10.198,10.0499,10.0499,10.0499,10.0499,10.0499,10.0499,10.0499,9.04489,9.03549,9.03549,8.03057,7.03491,7.02567,7.02567,6.0208,6.0208,5.01597,5.01597,4.01995,4.01123,4.01123,3.01496,3.01496,3.01496,3.00666,2.00998,2.00998,2.00998,2.00998,1.00499,1.00404,0.902718,0.802247,0.60208,0.501597,0.401995,0.301496,0.202237,0.200998,0.200998,0.100499,0.100499,0.0805047,0.060531,0.0504876,0.0404475,0.0304138,0.0203961,0.0202237,0.0104403,0.010198,0.00921954,0.00728011,0.00538517,0.00412311,0.00316228,0.00219317,0.00211896,0.00208806,0.00111803,0.000984886,0.000761577,0.000538516,0.000360555,0.000223607,0.000223607,0.000128062,9.21954e-05,7.07107e-05,5.65685e-05,4.24264e-05,2.82843e-05,2.82843e-05,1.41421e-05,1.41421e-05,1.34536e-05};

TGraphErrors*  HEENFLUXRPAMELA= new TGraphErrors(pamelahenb,PAMELAHEENX,PAMELAHEENY,PAMELAHEXE,PAMELAHEENYE);


//#   proton/He ratio in Rigidity[GV] PAMELA(2006/07-2008/12)
//#   Adriani et al., Science 132, 69 (2011)

const int pamelaprnb=83;

double PAMELAPHRX[pamelaprnb]={
  1.01,1.06,1.11,1.16,1.22,1.28,1.34,1.4,1.47,1.54,1.61,1.68,1.76,1.85,1.93,2.02,2.12,2.22,2.32,2.43,2.55,2.67,2.79,2.92,3.06,3.21,3.36,3.52,3.68,3.86,4.04,4.23,4.43,4.64,4.85,5.08,5.45,5.97,6.55,7.18,7.87,8.63,9.46,10.38,11.38,12.47,13.68,15,16.44,18.03,19.77,21.68,23.77,26.06,28.57,31.33,34.35,37.67,41.3,45.29,49.66,54.45,59.7,65.46,71.77,78.7,86.29,94.62,106.12,121.84,139.89,160.62,184.42,211.74,243.11,279.12,320.47,367.95,422.47,485.06,582.46,749.72,1010.34};

double PAMELAPHRXE[pamelaprnb]={
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

double PAMELAPHRY[pamelaprnb]={
  27.2,21.7,18.3,15.53,13.98,12.73,11.85,11.14,10.64,10.23,9.76,9.45,9.22,8.96,8.61,8.31,8.12,7.98,7.84,7.63,7.44,7.33,7.23,7.24,7.08,6.97,6.92,6.89,6.88,6.72,6.7,6.63,6.62,6.58,6.54,6.49,6.38,6.34,6.26,6.19,6.13,6.03,6.03,5.99,5.92,5.79,5.81,5.74,5.64,5.63,5.6,5.41,5.52,5.47,5.27,5.4,5.27,5.2,5.12,4.99,5.11,5.09,5.04,4.95,4.96,5.09,4.9,4.6,4.77,4.6,4.7,4.5,4.4,4.5,4.8,4.7,4.6,4.3,4.1,3.9,4.6,3.7,2.7};

double PAMELAPHRYE[pamelaprnb]={
  1.0198,1.0198,0.806226,0.705762,0.60531,0.60407,0.503587,0.502494,0.502494,0.501597,0.401995,0.401995,0.401995,0.401123,0.401123,0.401123,0.401123,0.401123,0.401123,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.300666,0.300666,0.300666,0.300666,0.300666,0.300666,0.300666,0.300666,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.301496,0.202237,0.302655,0.302655,0.203961,0.203961,0.206155,0.206155,0.206155,0.208806,0.208806,0.211896,0.211896,0.215407,0.215407,0.219317,0.223607,0.223607,0.219317,0.223607,0.223607,0.223607,0.223607,0.282843,0.282843,0.282843,0.360555,0.360555,0.360555,0.360555,0.447214,0.538516,0.728011};

TGraphErrors*  PHRATIOPAMELA= new TGraphErrors(pamelaprnb,PAMELAPHRX,PAMELAPHRY,PAMELAPHRXE,PAMELAPHRYE);



//#   He Flux in Rigidity[GV] PAMELA-CALO(2006/06-2010/01)
const int pamelacalohenb=6;

double PAMELACALOHEX[pamelacalohenb]={146.186,288.16,571.551,1113.69,2214,4464.14};

double PAMELACALOHEXE[pamelacalohenb]={0,0,0,0,0,0};

double PAMELACALOHEY[pamelacalohenb]={0.00316974,0.000564988,0.0001035,2.44e-05,4.71e-06,5.65e-07};

double PAMELACALOHEYE[pamelacalohenb]={0.000509418,0.00013437,2.07963e-05,4.11398e-06,6.8738e-07,7.8924e-08};

TGraphErrors*  HEFLUXRPAMELACALO= new TGraphErrors(pamelacalohenb,PAMELACALOHEX,PAMELACALOHEY,PAMELACALOHEXE,PAMELACALOHEYE);


#endif
