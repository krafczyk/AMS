// $Id: TrdMcDatacards.h,v 1.1 2013/10/14 16:01:12 traiha Exp $
#ifndef TrdMcDatacards_h
#define TrdMcDatacards_h

#include "typedefs.h"
#include "TObject.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

//########################################################
// WARNING do not touch the order of the variables unless 
//         you understand all the consequences
//         Never delete or move the first data meber of 
//         each class (ask PZ)
//########################################################

//==============================================================


//! TRD MC datacards that are stored in the AMS root file
class TRDMCFFKEY_DEF: public TObject{
public:
  integer mode;   // 0: saveliev 1: saveliev+Pai 2: garibyan + Pai
  integer g3trd;  // g3identifier for trd aware media
  float cor;      // saveliev parameter
  float alpha;    // garibyan pars
  float beta;     // garibyan par
  geant ped;     
  geant pedsig;     
  geant sigma;
  geant gain;
  geant cmn;
  geant f2i;
  integer adcoverflow;
  integer NoiseOn;
  float GeV2ADC;
  float Thr1R;
  integer GenerateConst;
  geant NoiseLevel;
  geant DeadLevel;
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
  integer multiples;
  geant Gdens;
  geant Pecut;
  geant ntrcor;
  geant etrcor;
  geant ndecor;
  geant edecor;

  geant Tmean;
  geant Pmean;
  geant FibreDiam;
  geant RadThick;
  geant AlphaFibre;
  geant AlphaGas;
  integer FleeceGas;
  geant XenonFraction;
  geant ArgonFraction;
  geant CO2Fraction;
  geant GasStep;
  integer PAIModel;
  integer debug;
  int CreatePDF;
  float TRphotonEnergyScalingFactor;
  void init();
  TRDMCFFKEY_DEF():TObject(){init();}
  ClassDef(TRDMCFFKEY_DEF,1);
};

#define TRDMCFFKEY COMMON_BLOCK(TRDMCFFKEY,trdmcffkey)
COMMON_BLOCK_DEF(TRDMCFFKEY_DEF,TRDMCFFKEY);

#endif
