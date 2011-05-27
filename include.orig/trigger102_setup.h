#ifndef __AMS2TRIGGERSETUP__
#define __AMS2TRIGGERSETUP__
#include "tofanticonst.h"
 class ScalerMon{
  public:
#ifdef __ROOTINC__
    void * dum;
#endif
    number _FTtrig[5];//<Glob,FTC,FTZ,FTE,NonPhys
    number _LVL1trig[9];//LVL1, sub1:sub8
    number _DetMaxRate[5];//CPmx,BZmx,ACmx,ECFTmx,ECL1(angle)mx
    number _LiveTime[2];
    uinteger _TimeCalib;
    geant _tgatelt;//time-gate for LiveTime
    geant _tgatetr;//..........for trig.FPGA
    geant _tgatesc;//..........for scal.FPGA
/*
    number _SPtrig[5];//LA0,LA1,Ext,DSP,Internal
    number _TrigTimeT;// trigger time tap
    number _CPside1[4];//4 tof-layers
    number _CPside2[4];
    number _CTside1[4];
    number _CTside2[4];
    number _BZside1[4];//4 tof-layers
    number _BZside2[4];
    number _AntiBot[8];//8 anti-sectors(logical)
    number _AntiTop[8];
    number _ECftProj[2];//EC-ft rate in x,y-proj
    number _ECl1Proj[2];//EC-lev1(angle) rate in x,y-proj
    number _ScalTimeT;//scalers time tap
    number _LiveTime[2];
    uinteger _TrigFPGAid;
    uinteger _ScalFPGAid;
*/
  public:
    number &FTtrig(int i){return _FTtrig[i];}
    number &LVL1trig(int i){return _LVL1trig[i];}
    number &LiveTime(int i){return _LiveTime[i];}
    number &DetMaxRate(int i){return _DetMaxRate[i];}
    uinteger &TimeCalib(){return _TimeCalib;}
    geant FTrate(){return geant(_FTtrig[0]);}
    geant FTCrate(){return geant(_FTtrig[1]);}
    geant LVL1rate(){return geant(_LVL1trig[0]);}
    geant &TGateLT(){return _tgatelt;}
    geant &TGateTR(){return _tgatetr;}
    geant &TGateSC(){return _tgatesc;}
/*
    number &SPtrig(int i){return _SPtrig[i];}
    number &TrigTimeT(){return _TrigTimeT;}
    number &CPside1(int i){return _CPside1[i];}
    number &CPside2(int i){return _CPside2[i];}
    number &CTside1(int i){return _CTside1[i];}
    number &CTside2(int i){return _CTside2[i];}
    number &BZside1(int i){return _BZside1[i];}
    number &BZside2(int i){return _BZside2[i];}
    number &AntiTop(int i){return _AntiTop[i];}
    number &AntiBot(int i){return _AntiBot[i];}
    number &ECftProj(int i){return _ECftProj[i];}
    number &ECl1Proj(int i){return _ECl1Proj[i];}
    number &ScalTimeT(){return _ScalTimeT;}
    number &LiveTime(int i){return _LiveTime[i];}
    uinteger &TrigFPGAid(){return _TrigFPGAid;}
    uinteger &ScalFPGAid(){return _ScalFPGAid;}
    geant TOFrateMX(){return _CPside1[2]>_CPside2[2]?geant(_CPside1[2]):geant(_CPside2[2]);}//imply layer3 has max rate due to its biggest size
    geant ECftrateMX(){return _ECftProj[0]>_ECftProj[1]?geant(_ECftProj[0]):geant(_ECftProj[1]);}
    geant AntirateMX(){
      number mx=0;
      for(int i=0;i<8;i++){
        number mr=_AntiTop[i]>_AntiBot[i]?_AntiTop[i]:_AntiBot[i];
	if(mr>mx)mx=mr;
      }
      return geant(mx);
    }
*/
#ifndef __ROOTSHAREDLIBRARY__
    void setdefs();
#else
    void setdefs(){};
#endif
    friend class AMSSetupR;
#ifndef __ROOTINC__
ClassDef(ScalerMon,1)       //ScalerMon
#endif
 };
//
 class Lvl1TrigConfig {
  public:
#ifdef __ROOTINC__
    void * dum;
#endif
   integer _tofinmask[TOF2GC::SCLRS][TOF2GC::SCMXBR];//=0/1/2/3=>Not_in_trig/both/s1/s2, z>=1 trigger
   integer _tofinzmask[TOF2GC::SCLRS][TOF2GC::SCMXBR];//=0/1/2/3=>Not_in_trig/both/s1/s2, z>=2 trigger
   integer _tofoamask[TOF2GC::SCLRS];//=0/1=>And/Or of two Plane-sides, FTC(z>=1)
   integer _tofoazmask[TOF2GC::SCLRS];//=0/1=>And/Or of two Plane-sides, BZ(z>=2)
   integer _toflut1;//accepted FT tof-layers config-1 (LookUpTabel,z>=1)
   integer _toflut2;//accepted FT tof-layers config-2 (2nd LookUpTabel, z>=1)
   integer _toflutbz;//accepted LVL1 tof-layers config(z>=2 lvl1)
   integer _toflcsz;//MN, layer-logic for FTZ(slowZ>=2),M=0/1->and/or of top and bot logic, N->top(bot)logic
   integer _tofextwid;//5_lowsignbits/next5moresignbits=>widthExtentionCodes for TopTOF/BotTOF 
   integer _antinmask[ANTI2C::MAXANTI];//=0/1/2/3=>Not_in_trig/both/s1(bot)/s2(top), in_trigger mask
   integer _antoamask[ANTI2C::MAXANTI];//=0/1=>And/Or of two LogicSector-sides mask
   integer _antsectmx[2];//two maxlimits on acceptable numb. of fired ANTI-sectors(logical),equat/polar reg.
   integer _ecorand;//Ecal or/and(=1/2)->  of two projections requirements on min. numb. of fired layers
   integer _ecprjmask;//Ecal proj. dec-mask(lkji: ij=1/0->XYproj active/disabled in FT; kl=same for LVL1(angle)
//                      i don't need here ecinmask because already have dynode bad/good status in calib. 
   integer _globftmask;//global FT sub-triggers dec-mask(i|j|k->FTE|FTZ|FTC)
   integer _globl1mask;//global LVL1 trigger(phys.branches) bit-patt(bitON->BranckIsRequired, oppos.to Lin!)
   integer _physbrmemb[8];//Requir.Memb. bit-patt. for phys.branches(bitON->MemberIsRequired, oppos.to Lin!) 
   integer _phbrprescf[8];//Prescaling factors(1,2,...1000) for each phys.branch
  public:
   integer &tofinmask(int il, int ib){return _tofinmask[il][ib];}
   integer &tofinzmask(int il, int ib){return _tofinzmask[il][ib];}
   integer &tofoamask(int il){return _tofoamask[il];}
   integer &tofoazmask(int il){return _tofoazmask[il];}
   integer &toflut1(){return _toflut1;}
   integer &toflut2(){return _toflut2;}
   integer &toflutbz(){return _toflutbz;}
   integer &toflcsz(){return _toflcsz;}
   integer &tofextwid(){return _tofextwid;}
   integer &antinmask(int is){return _antinmask[is];}
   integer &antoamask(int is){return _antoamask[is];}
   integer &antsectmx(int i){return _antsectmx[i];}
   integer &ecorand(){return _ecorand;}
   integer &ecprjmask(){return _ecprjmask;}
   integer &globftmask(){return _globftmask;}
   integer &globl1mask(){return _globl1mask;}
   integer &physbrmemb(int br){return _physbrmemb[br];}
   integer &phbrprescf(int br){return _phbrprescf[br];}
#ifndef __ROOTSHAREDLIBRARY__
   void read();
   void saveRD(int flag);
   void redefbydc();//MC: redef. some config-pars in memory by DataCards
#else
   void read(){};
   void saveRD(int flag){};
   void redefbydc(){};
#endif
   friend class AMSSetupR;
#ifndef __ROOTINC__
ClassDef(Lvl1TrigConfig,1)       //Lvl1TrigConfig 
#endif
 };
#endif
