//  $Id: trigger302.h,v 1.1 2001/08/08 14:34:21 choutko Exp $
#ifndef __AMSTRIGGER3__
#define __AMSTRIGGER3__
#include <link.h>
#include <tofdbc02.h>
#include <tofdbc.h>
#include <tkdbc.h>
#include <amsdbc.h>
#include <amsstl.h>
#include <trid.h>
#include <commons.h>
//
const integer NTRHDRP=trid::ntdr;
const integer NTRHDRP2=trid::ntdr*trid::ncrt;
const integer maxtr=10000;
const integer maxtrpl=10;
const integer NDSTR=3;
//
class TriggerAuxLVL3{
 protected:
  integer _ltr;
  integer _ctr;
  int16 _ptr[maxtr];
 public:

  TriggerAuxLVL3();
  void addnoise();
  void fill(integer crate);
  int16 * readtracker(integer begin=0);
};




class TriggerLVL3: public AMSlink{
protected:
 integer _TOFTrigger;
 integer _AntiTrigger;
 integer _TrackerTrigger;   // 0  - initial state
                            // 1  - reject 
                            // 2  - Too many hits  ( no fit  done )
                            // 3  - No comb found
                            // 4  - >=2 comb found
                            // 5  - Reserved
                            // 6  - Reserved
                            // 7  - Accept 
                            // + 8   // Heavy Ion 
 integer _NTrHits;      // Total number of "good" hits
 integer _NPatFound;    // Number of "tracks" found
 integer _Pattern[2];
 number _Residual[2];      // Discriminators
 number _Time;          // Time to process event
 number _TrEnergyLoss;

// Aux Tr

 integer _nhits[trconst::maxlay];
 integer _drp[trconst::maxlay][maxtrpl];
 geant _coo[trconst::maxlay][maxtrpl];
 geant _eloss[trconst::maxlay][maxtrpl];
 geant _lowlimitY[trconst::maxlay];
 geant _upperlimitY[trconst::maxlay];
 geant _lowlimitX[trconst::maxlay];
 geant _upperlimitX[trconst::maxlay];
 geant coou, dscr,cood,a,b,s,r;
 geant resid[trconst::maxlay-2],zmean,factor,amp[trconst::maxlay];

 void _copyEl(){}
 void _printEl(ostream & stream);
 void _writeEl();
 integer _UpdateOK(geant s, geant res[], geant amp[],integer pat);
 integer _Level3Searcher(int call, int pat);
 static integer _TOFPattern[TOFGC::MAXPAD][TOFGC::MAXPAD];
 static integer _TOFStatus[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _TOFOr[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _TrackerStatus[NTRHDRP2];
 static integer _TrackerAux[NTRHDRP][trid::ncrt];
 static integer _TOFAux[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _NTOF[TOFGC::MAXPLN];
 static geant _TOFCoo[TOFGC::MAXPLN][TOFGC::MAXPAD][3];
 static geant _TrackerCoo[NTRHDRP][trid::ncrt][3];
 static geant _TrackerDir[NTRHDRP][trid::ncrt];
 static geant _TrackerCooZ[trconst::maxlay];
 static integer _TrackerDRP2Layer[NTRHDRP][trid::ncrt];
 static number _stripsize;
 static integer _TrackerOtherTDR[NTRHDRP][trid::ncrt];
public:
 TriggerLVL3();
 TriggerLVL3(integer tra, integer tof, integer anti,integer ntr, integer npat,
 integer pat[], number res[], number time, number eloss);
 integer tofok();
 integer TOFOK(){return _TOFTrigger==1;}
 integer & TrackerTrigger(){ return _TrackerTrigger;}
 integer TrackerTriggerS(); 
 integer LVL3OK();
 integer LVL3HeavyIon(){return (_TrackerTrigger & 8) || (_TrEnergyLoss>600);}
 static integer TOFOr(uinteger paddle,uinteger plane);
 static integer TOFInFastTrigger(uinteger paddle, uinteger plane);
 void preparetracker();
 void addtof(int16 plane, int16 paddle);
 geant getlowlimitY(integer layer){return _lowlimitY[layer];} 
 geant getupperlimitY(integer layer){return _upperlimitY[layer];}
 geant getlowlimitX(integer layer){return _lowlimitX[layer];} 
 geant getupperlimitX(integer layer){return _upperlimitX[layer];}
 void settime(number time);
 void fit(integer idum);
 integer addnewhit(geant coo, geant amp, integer layer, integer drp);
 static void build();
 static void build2();
 static void init();
 static geant Discriminator(integer nht);
//
#ifdef __DB__
   friend class Triggerlvl3D;
#endif
 // Interface with DAQ

       static integer checkdaqid(int16u id);
       static integer calcdaqlength(integer i);
       static integer getmaxblocks(){return 1;}
       static int16u getdaqid(int i){return (i>=0 && i<getmaxblocks())?
       ( 9<<9 | i<<6 ):0;}
       static void builddaq(integer i, integer n, int16u *p);
       static void buildraw(integer n, int16u *p);


    friend class TriggerExpertLVL3;

 integer skip(){
   if ( _TrackerTrigger==0 )return 1;
   else return 0;
 }

};


class TriggerExpertLVL3{
protected:
number _Mean[NDSTR];
number _Sigma[NDSTR];
number _XCount[NDSTR];
integer _Counter;              // current event in seq
integer _CounterMax;           // max no events in seq
integer _ToBadSwitch;          // max seq to switch off
integer _TryAgainSwitch;       // max seq to switch on
  // first index:
  //  0 - Reject
  //  1 - Fails if too many hits/comb
  //  2 - Accept
  //  3 - Sum
geant  _Distributions[NDSTR+1][NTRHDRP2]; 
integer  _Relative[NDSTR];      // 0 - take abs 1 - take relative    
integer _DRPBad[NTRHDRP2];       // how man sequences the drp is bad
integer _DRPOff[NTRHDRP2];       // how many seq the drp was off
geant _BadRange[NDSTR][2];
public:
TriggerExpertLVL3(integer countermax, integer tobadswitch, integer tryagain,
                  geant badrange[NDSTR][2]);

void update(const TriggerLVL3  * plvl3);
void analyse(const TriggerLVL3  * plvl3);
void print();
 static TriggerExpertLVL3 * pExpert;
};

class AMSLVL3Error{
private:
 char msg[256];
public:
 AMSLVL3Error(char * name);
 char * getmessage();
};

#endif

