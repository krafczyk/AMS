//  $Id: trigger302.h,v 1.3 2001/11/19 14:39:23 choutko Exp $
#ifndef __AMSTRIGGER302__
#define __AMSTRIGGER302__
#include <link.h>
#include <tofdbc02.h>
#include <tofdbc.h>
#include <tkdbc.h>
#include <trddbc.h>
#include <amsdbc.h>
#include <amsstl.h>
#include <trid.h>
#include <trdid.h>
#include <commons.h>
//
namespace trigger302const{
const integer NTRHDRP=trid::ntdr;
const integer NTRHDRP2=trid::ntdr*trid::ncrt;
const integer maxtr=10000;
const integer maxtrd=1000;
const integer maxtof=1000;
const integer maxtrpl=10;
};
//
class TriggerAuxLVL302{
 protected:
  integer _ltr;   // recorded length  
  integer _ctr;   // current pointer position while read
  int16 _ptr[trigger302const::maxtr];
 public:

  TriggerAuxLVL302();
  void sitknoise();
  void addnoisetk(integer crate);
  void filltk(integer crate);
  void filltrd(integer crate);
  void filltof(integer crate);
  int16 * readtracker(integer begin=0);
  int16 * readtrd(integer begin=0);
  int16 * readtof(integer begin=0);
};




class TriggerLVL302: public AMSlink{

// Put everything in one class due to necessity to be as close to C as possible

protected:
 uinteger _TriggerInputs;   //   0 Default
                            //   1  Doesnot Require TRD      
                            //   2  Doesnot Require TOF Timing      
 integer _TOFTrigger;
 uinteger _AntiTrigger;
 uinteger _TRDTrigger;
 uinteger _TrackerTrigger;
 uinteger _MainTrigger;     // 0  - initial state
                            // 1  - No Tr Tracks found
                            // 2  - No TRD Tracks found
                            // 3  - Too Many Hits in Tracker/TRD/TOF
                            // 4  - No TOF Time Information found
                            // 5  - Positive Rigidity(Momentum) found
                            // 6  - Ambigious Comb found 
                            // 7  - Negative Rigidity(Momentum) found
                            // + 8   // High Gamma (TRD)  
                            // + 16   // High Ion (Tracker)
                            // + 32  //  Prescaled event  

// TOF Time Part
//
// EC to define
//

// Trd PART
integer _TRDHits; // number of good hits
integer _TRDTracks; // number of trd tracks
integer _TRDHMult;   // number of clusters with energy > thr 

// Tracker Part
 integer _NTrHits;      // Total number of "good" hits
 integer _NPatFound;    // Number of "tracks" found
 integer _Pattern[2];
 number _Residual[2];      // Discriminators
 number _TrEnergyLoss;


// Tracker Aux Part

 integer _nhits[trconst::maxlay];
 integer _drp[trconst::maxlay][trigger302const::maxtrpl];
 geant _coo[trconst::maxlay][trigger302const::maxtrpl];
 geant _eloss[trconst::maxlay][trigger302const::maxtrpl];
 geant _lowlimitY[trconst::maxlay];
 geant _upperlimitY[trconst::maxlay];
 geant _lowlimitX[trconst::maxlay];
 geant _upperlimitX[trconst::maxlay];
 geant coou, dscr,cood,a,b,s,r;
 geant resid[trconst::maxlay-2],zmean,factor,amp[trconst::maxlay];
 static integer _TOFPattern[TOFGC::MAXPAD][TOFGC::MAXPAD];
 static integer _TOFStatus[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _TOFOr[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _TrackerStatus[trigger302const::NTRHDRP2];
 static integer _TrackerAux[trigger302const::NTRHDRP][trid::ncrt];
 static integer _TOFAux[TOFGC::MAXPLN][TOFGC::MAXPAD];
 static integer _NTOF[TOFGC::MAXPLN];
 static geant _TOFCoo[TOFGC::MAXPLN][TOFGC::MAXPAD][3];
 static geant _TrackerCoo[trigger302const::NTRHDRP][trid::ncrt][3];
 static geant _TrackerDir[trigger302const::NTRHDRP][trid::ncrt];
 static geant _TrackerCooZ[trconst::maxlay];
 static integer _TrackerDRP2Layer[trigger302const::NTRHDRP][trid::ncrt];
 static number _stripsize;
 static integer _TrackerOtherTDR[trigger302const::NTRHDRP][trid::ncrt];

 integer _UpdateOK(geant s, geant res[], geant amp[],integer pat);
 integer _Level3Searcher(int call, int pat);


// Trd Aux Part


//  Tof Aux Part





 void _copyEl(){}
 void _printEl(ostream & stream);
 void _writeEl();
 number _Time;          // Time to process event


public:
 TriggerLVL302();
 TriggerLVL302(integer tra, integer tof, integer anti,integer ntr, integer npat,
 integer pat[], number res[], number time, number eloss);
 integer tofok();
 integer TOFOK(){return _TOFTrigger==1;}
 uinteger & TrackerTrigger(){ return _TrackerTrigger;}
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



#endif

