//  $Id: trigger302.h,v 1.5 2001/12/03 13:33:21 choutko Exp $
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
const integer maxtof=1000;
const integer maxtrpl=10;
const integer TRDIN=1;
const integer TOFIN=2;
const integer maxufe=20;
const integer maxhitstrd=12;
const integer maxtrd=maxufe*maxhitstrd;
const integer matrixsize=60;
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

 integer _TOFTrigger;       //  -1 No Matrix
                            //   0 Too Many Hits
                            //   1  tof 0  1 cluster or 2 cluster
                            //   2  tof 1  -----
                            //   4  tof 2  -----
                            //   8  tof 3  -----
                            //   16  tof 0  2 cluster
                            //   32  tof 1  -----
                            //   64  tof 2  -----
                            //   128  tof 3  -----

 uinteger _TRDTrigger;      //   0  Nothing found
                            //   bit 0:  Segment x found
                            //   bit 1:  segment y found
                            //   bit 2: too many hits found  
                            //   bit 3: high gamma event found 

 uinteger _TrackerTrigger;  // 0  - initial state
                            // 1  - No Tr Tracks found
                            // 2  - Too Many Hits in Tracker
                            // 3  - Positive Rigidity found
                            // 4  - Ambigious Comb (A) found 
                            // 5  - Ambigious Comb (B) found 
                            // 6  - Negative Rigidity(Momentum) found
                            // + 8   // Heavy Ion (Tracker)

 uinteger _MainTrigger;     //  0  - initial state
                            // bit  0 No Tr Tracks found
                            // bit  1 No TRD Tracks found
                            // bit  2 Too Many Hits in Tracker
                            // bit  3 Too Many Hits in TRD
                            // bit  4 Too Many Hits in TOF
                            // bit  5 No TOF Time Information found
                            // bit  6 Upgoing event found
                            // bit  7 Positive Rigidity(Momentum) found
                            // bit  8 Ambigious Comb (A) found 
                            // bit  9 Ambigious Comb (B) found 
                            // bit  10  Negative Rigidity(Momentum) found
                            // bit  11   Heavy Ion (Tracker)
                            // bit  12  High Gamma (TRD)  
                            // bit  13 Prescaled event  

// TOF Time Part
//
// EC to define
//



 
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
 geant _upperlimitTOF[2][trconst::maxlay];
 geant _lowlimitTOF[2][trconst::maxlay];
 geant _upperlimitTRD[2][trconst::maxlay];
 geant _lowlimitTRD[2][trconst::maxlay];
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
  class TRDAux{
   public:
    number _Par[2][6];    // fit parameters: t=par[0]*(z-par[2])+par[1]
                          //                   par[3] == error(par[0])
   bool _SegmentFound[2];
   integer _NHits[2]; // number of good hits
   integer _HMult;   // number of clusters with energy > thr 

    integer _nufe;
    integer  _lasthaddr;
    uinteger _haddrs[trigger302const::maxufe];
    uinteger _nhits[trigger302const::maxufe];
    int16u _coo[trigger302const::maxufe][trigger302const::maxhitstrd][3];  // ute,tube,amp  ( X 8 ?)
    geant   _DistrMatrix[2][trigger302const::matrixsize][trigger302const::matrixsize];
static    geant _CooLimits[2];
static    geant _CooBinSize;
static    geant _TanLimits[2];
static    geant _TanBinSize;
static uinteger _Dir[trdid::nufe][trdid::nudr][trdid::ncrt];
static geant _Coo[trdid::nufe][trdid::nudr][trdid::ncrt][3];
static geant _CooZ[trdid::nute][trdconst::maxtube];
static geant _CooT[trdid::nute][trdconst::maxtube];

static geant _IncMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];
static geant _CooMatrix[trdid::nute][trdconst::maxtube][trdid::nute-1][trdconst::maxtube];
   TRDAux():_HMult(0),_nufe(-1),_lasthaddr(-1){
    for(int i=0;i<2;i++){_SegmentFound[i]=false;_NHits[i]=0;}
    for(int i=0;i<trigger302const::matrixsize;i++){
     for(int j=0;j<trigger302const::matrixsize;j++){
     _DistrMatrix[0][i][j]=0;
     _DistrMatrix[1][i][j]=0;
    }
   }
}
  integer addnewhit(uinteger crate, uinteger udr, uinteger ufe,uinteger ute, uinteger tube, int16u amp);
  void build();
  };  
  TRDAux TRDAux;


 //TOFTime Part
   integer _Direction;            //  -1 Up
                                  //  0  Unknwn
                                  //  1 Down

//  Tof Aux Part





 void _copyEl(){}
 void _printEl(ostream & stream);
 void _writeEl();
 number _Time;          // Time to process event

public:
 TriggerLVL302(bool tofin=true, bool trdin=true);

 void Finalize();   // mke the MainTriggerOutput

// Tracker Part


 integer toftrdok();
 integer TOFOK(){return _TOFTrigger==1;}
 uinteger & TrackerTrigger(){ return _TrackerTrigger;}
 uinteger & TRDTrigger(){ return _TRDTrigger;}
 integer MainTrigger(); 
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

 //TRD Part

 bool UseTRD(){return (_TriggerInputs&trigger302const::TRDIN)==0;}


 //TOFTime Part
   
 bool UseTOFTime(){return (_TriggerInputs&trigger302const::TOFIN) ==0;}




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

