// $Id: TrdMcDatacards.C,v 1.4 2013/11/07 14:44:17 traiha Exp $
#include "TrdMcDatacards.h"
#include <math.h>
#include <float.h>
#ifdef _PGTRACK_
ClassImp(TRDMCFFKEY_DEF);
#endif

//==============================
//  TRD simulation datacards
//==============================
void TRDMCFFKEY_DEF::init(){
  CreatePDF=0;
  mode=-1;
  g3trd=123654;
  cor=0.68;
  alpha=0;
  beta=0;
  gain=1;
  f2i=8;
  adcoverflow=4095;
  ped=355;
  pedsig=102;
  sigma=6;
  cmn=15;
  NoiseOn=1;
  //GeV2ADC=1.e8;
  // changed vc 22-dec-2009
  GeV2ADC=1.e8/3 * 1.39;
  Thr1R=3.5;
  sec[0]=0;
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=120;
  GenerateConst=0;
  NoiseLevel=0.01;
  DeadLevel=0.;
  multiples=0;
  Gdens=0.00446;
  Pecut=-0.65;
  ntrcor=1.3;
  etrcor=1.2;
  ndecor=1.0;
  edecor=1.0;
  Tmean=287.15;
  Pmean=927.0;
  FibreDiam=12.;
  RadThick=20.;
  AlphaFibre=5.;
  AlphaGas=0.66;
  FleeceGas=1;
  XenonFraction=0.89;
  ArgonFraction=0.;
  CO2Fraction=0.11;
  GasStep=6.;
  PAIModel=1;
  debug=0;
  TRphotonEnergyScalingFactor=0.80;
  ZShift=0.1; // TR 07-nov-2013: TRD shifted upwards 1 mm instead of previous 0.1 mm
}

TRDMCFFKEY_DEF TRDMCFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trdmcffkey_)
#endif
