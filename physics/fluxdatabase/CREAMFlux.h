//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  CREAMFlux
///\data  2014/09/30 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _CREAMFlux_
#define _CREAMFlux_

/// TGraphErrors*  HEFLUXRCREAM  : CREAM-I helium flux VS Rigidity

//#   He Flux in Rigidity[GV] CREAM-I(2004/12-2005/01)
const int creamhenb=10;

double CREAMHEX[creamhenb]={
1589.31,2531.68,4001.86,6326.42,10041.8,15876.4,25300.1,40001.9,63247.5,100401
};

double CREAMHEXE[creamhenb]={
0,0,0,0,0,0,0,0,0,0,
};

double CREAMHEY[creamhenb]={
7.1e-06,2.175e-06,6.55e-07,1.915e-07,6.35e-08,2.095e-08,5.75e-09,1.7e-09,4.1e-10,1.45e-10,
};

double CREAMHEYE[creamhenb]={
2e-07,8e-08,3.5e-08,1.55e-08,7e-09,3.2e-09,1.35e-09,5.5e-10,2.45e-10,1.2e-10,
};

TGraphErrors*  HEFLUXRCREAM= new TGraphErrors(creamhenb,CREAMHEX,CREAMHEY,CREAMHEXE,CREAMHEYE);

#endif
