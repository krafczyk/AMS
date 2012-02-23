//  $Id: trigger102.h,v 1.37 2012/02/23 09:22:35 choutko Exp $
#ifndef __AMS2TRIGGER__
#define __AMS2TRIGGER__
#define __ROOTINC__
#include "link.h"
#include "antidbc02.h"
#include "antidbc02.h"
#include "trigger102_setup.h"
//
class Trigger2LVL1: public AMSlink{
protected:
//
 class Scalers{//old ones !
  protected:
   uinteger _Nentries;
   uinteger _Tls[3][32];
   uinteger _GetIndex(time_t time);
  public:
   geant getsum(time_t time) {return _Tls[2][_GetIndex(time)]/96.;}
   geant getlifetime(time_t time)   {return _Tls[1][_GetIndex(time)]/16384.;}
 };
// 
 unsigned int _PhysBPatt;//LVL1 Phys. Branches Pattern(bits 0-7)(unbiased,p,Ion,SlowIon,e,gamma, etc)
 unsigned int _JMembPatt;//LVL1 Joined Members Pattern(bits 1-16)(FTC,FTZ,FTE,ACC0,ACC1,BZ,ECAL-F_and,...)
 unsigned int _AuxTrigPatt;//Aux.Trig. members pattern (LA1/2,DSP,Int)
 integer _tofflag1;//Tof-layers contrib. in FTC(z>=1) (<0/0/1/2...=>none/all4/miss.layer#)  
 integer _tofflag2;//Tof-layers contrib. in BZ(z>=2) (<0/0/1/2...=>none/all4/miss.layer#)  
 integer _tofpatt1[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern for z>=1(when globFT)
 integer _tofpatt2[TOF2GC::SCLRS];// TOF:  triggered paddles/layer pattern for z>=2(when globFT)
 integer _antipatt; //ANTI: pattern of FT-coincided sectors(logical)
 integer _ecalflag; //ECAL: =MN
// M=0/1/2/3->FTE(Ener)Flag=No/FTE_when_no_prj_bits(?)/FTE&1proj(or)/FTE&2proj(and),
// N=0/1/2/3->LVL1(Angle)Flag=Undef(noFTE)/0AnglePrj@FTEfound/OrAngPrj/AndAngPrj;
 int16u _ectrpatt[6][3];//PM(dyn) trig.patt for 6"trigger"-SupLayers(36of3x16bits for 36 dynodes) 
 geant   _ectrsum;//"EC tot.energy"(total sum of all dynode channels used for trigger,gev)
 geant _LiveTime;//Live time fraction
 geant _TrigRates[19];//TrigComponentsRates(Hz):FT,FTC,FTZ,FTE,NonPH,LVL1,LVL1m1-m8,CPmx,BZmx,ACmx,EFTmx,EANmx
 uinteger _TrigTime[5];//trig.times:[0]->calibr;[1]->reset 24bits counter;[2]-[3]->40bits(32+8)
//                       of 0.64mks counter; [4]->DeltaTrigTime(mksec, 2 consecutive events)
 static Scalers _scaler;
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " LiveTime " << float(_LiveTime)/1000.<<endl;}
 void _writeEl();
public:
 static Lvl1TrigConfig l1trigconf;//current TrigSystemConfiguration
 static ScalerMon scalmon;//current scalers values
 static bool SetupIsChanged;
 static bool ScalerIsChanged;
#pragma omp threadprivate(SetupIsChanged,ScalerIsChanged)
 static integer PhysBranchCount[8];
 static int16u nodeids[2];//LVL1 node IDs(side_a/_b)
 
 Trigger2LVL1(integer PhysBPatt, integer JMembPatt, integer auxtrpat, integer toffl1,integer toffl2, 
              integer tofpatt1[],integer tofpatt2[], integer antipatt, integer ecflg,
                int16u ectrpatt[6][3], geant ectrsum, geant LiveTime, geant rates[],uinteger trt[]):
      _PhysBPatt(PhysBPatt),_JMembPatt(JMembPatt),_AuxTrigPatt(auxtrpat),_tofflag1(toffl1),
      _tofflag2(toffl2),_antipatt(antipatt),_ecalflag(ecflg),_ectrsum(ectrsum),_LiveTime(LiveTime)
 {
   int i,j;
   for(i=0;i<TOF2GC::SCLRS;i++)_tofpatt1[i]=tofpatt1[i];
   for( i=0;i<TOF2GC::SCLRS;i++)_tofpatt2[i]=tofpatt2[i];
   for(i=0;i<6;i++)for(j=0;j<3;j++)_ectrpatt[i][j]=ectrpatt[i][j];
   for(i=0;i<19;i++)_TrigRates[i]=rates[i];
   for(i=0;i<5;i++)_TrigTime[i]=trt[i];
 }
 bool IsECHighEnergy()const {return _ecalflag/10>=2;}
 bool IsECEMagEnergy()const {return _ecalflag%10>=2;}
 bool GlobFasTrigOK(){return ((_JMembPatt&1)>0 || (_JMembPatt&(1<<5))>0 || (_JMembPatt&(1<<6))>0);}
 bool TofFasTrigOK(){return ((_JMembPatt&1)>0 || (_JMembPatt&(1<<5))>0);}
 bool EcalFasTrigOK(){return ((_JMembPatt&(1<<6))>0);}
 bool ExternTrigOK(){return ((_JMembPatt&(1<<14))>0);}
 bool AuxTrigOK(int i){return ((_AuxTrigPatt&(1<<i))>0);}
 bool JMembPattBitSet(int i){return ((_JMembPatt&(1<<i))>0);}
  static Scalers * getscalersp(){return &_scaler;}
  static integer getscalerssize(){return sizeof(_scaler);}
  geant getlivetime () const {return _LiveTime;}
 number gettrrates(int i){return _TrigRates[i];}
 uinteger gettrtime(int i){return _TrigTime[i];}
 integer gettoflag1() {return _tofflag1;}
 integer gettoflag2() {return _tofflag2;}
 integer getecflag() {return _ecalflag;}
 void getectrpatt(int16u patt[6][3]){for(int i=0;i<6;i++)for(int j=0;j<3;j++)patt[i][j]=_ectrpatt[i][j];}
 bool EcalDynON(int sl, int pm){//sl=0-5, pm=0-35
   int word,bit;
   word=pm/16;
   bit=pm%16;
   return((_ectrpatt[sl][word]&(1<<bit))>0);
 }
 void settofpat1(int patt[]){ for(int i=0;i<TOF2GC::SCLRS;i++)_tofpatt1[i]=patt[i];}
 void settofpat2(int patt[]){ for(int i=0;i<TOF2GC::SCLRS;i++)_tofpatt2[i]=patt[i];}
 void setecpat(int16u patt[6][3]){for(int i=0;i<6;i++)for(int j=0;j<3;j++)_ectrpatt[i][j]=patt[i][j];}
 void setectrs(geant trs){_ectrsum=trs;}
 integer getl1strpatt(){return _PhysBPatt;}
 integer checktofpattor(integer tofc, integer paddle);
 integer checktofpattand(integer tofc, integer paddle);
 integer checkantipatt(integer counter){return _antipatt & (1<<counter);}
 integer getantipatt(){return _antipatt;}
 static void build();
 static void init();
 static integer prescfcode2val(int16u code){return (1024-(code&0x3FF));}
 static int16u  prescfval2code(integer val){
          if(val<1024 && val>0)return int16u(1024-val);
          else return 0;
        }

 // Interface with DAQ
      static int16u getdaqid(int sid){return(nodeids[sid]);}      
      static integer checkdaqid(int16u id);
      static void node2side(int16u nodeid, int16u &sid);
      static integer calcdaqlength(int i);
      static integer getmaxblocks();
      static void builddaq(integer i, integer n, int16u *p);
      static void buildraw(integer n, int16u *p);
      static int buildrawearly(integer n, int16u *p);
      static void EventBitDump(integer leng, int16u *p, char * message);
      static int16u lutbswap(int16u lut);//RD L3<->L4 problem correction for LUTs
#ifdef __WRITEROOT__
      friend class Level1R;
#endif
friend class AMSSetupR;
};
//-----------------------------
class TGL1JobStat{
//
private:
  static integer countev[20];
//          i=0 -> entries(MC/RD)
//          i=1 -> MC: Common FT OK
//          i=2 -> 
//          i=3 -> 
//          i=4 -> 
//          i=5 -> 
//          i=6 => 
//          i=7 =>
//         i=15 => HW-created LVL1 found
// 
  static integer daqc1[80];//daq-decoding counters
//            i=0 -> LVL1-segment entries
//             =1 -> ............ non empty
//             =2 -> ............ with a-side 
//             =3 -> ............ with b-side
//             =4 -> ............ no assembl_errors
//             =5 -> TrigPattBlock entries 
//             =6 -> ............. length OK 
//             =7 -> LiveTimeBlock entries 
//             =8 -> ............. length OK 
//             =9 -> TrigSetupBlock entries 
//             =10-> .............. length OK
//             =11-> ScalersBlock entries 
//             =12-> ............ length OK
//             =13-> total LVL1-segment errors
//             =14-> total good LiveTime/Rates LVL1-obj
//             =15-> total bad LiveTime/Rates cases
//             =20/23-> format-types entries
//             =24-> nonDATA segments
//             =25/31-> reply status bits
//             =32/33-> empty trig.patt
//             =34-> non-empty AuxTrigPatt
//             =35-39
public:
  static void resetstat();
  static void addev(int i);
  static void daqs1(integer info);
  static void printstat();
};
//---------------------
//
#endif
