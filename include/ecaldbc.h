//  $Id: ecaldbc.h,v 1.10 2001/01/22 17:32:42 choutko Exp $
// Author E.Choumilov 14.07.99.
//
//
#ifndef __ECALDBC__
#define __ECALDBC__
#include <cern.h>
#include <commons.h>
#include <amsdbc.h>
#include <extC.h>
#include <string.h>
#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <job.h>
#include <math.h>
//
// ECAL global constants definition
//
const integer ECFLSMX=10; // max. fiber-layers per S-layer
const integer ECFBCMX=12; // max. fibers per layer in PMcell
const integer ECSLMX=9; // max. S(uper)-layers
const integer ECPMSMX=36; // max. PMCell(PM's) per S-layer
const integer ECPMSL=ECSLMX*ECPMSMX;// Max. total PM's in all S-layers
const integer ECFBLMX=500;// max. fibers per layer
const integer ECADCMX=4095;//max capacity of ADC(12bits)
const integer ECROTN=10000; // geant numbering of ECAL rot. matr.(starting from...)
const integer ECJSTA=10; // max size of counter-array for job statistics
const integer ECHIST=2000;// MCEcal histogram number(starting from...) 
const integer ECHISTR=2100;// REEcal histogram number(starting from...) 
//
//geometry :
//
//===========================================================================
// 
// --------------> Class for "time-stable("geom")"  parameters :
class ECALDBc {  
//
private:
// geom. constants:
  static geant _gendim[10]; // ECAL general dimensions
//                            1-3-> X,Y,Z-sizes of EC-radiator;
//                            4-> eff. X(Y)-thickness of PM+electronics volume
//                            5->X-pos; 6->Y-pos; 7->Z-pos(front face);
//                            8->top(bot) honeycomb thickness
  static geant _fpitch[3]; // fiber pitch:
//                           1-> in X(Y); 2/3->Z(inside of x(y) cell and between x-y)
  static geant _rdcell[10];// readout-PMcell parameters
//                          1-4->fiber att.length,%,diam; 5->size(dx=dz) of "1/4" of PM-cathode;
//                          6->abs(x(z)-position) of "1/4" in PMT coord.syst.(not used);
//                          7/8->X(Y)-pitch of PM's; fiber glue thickness(in radious)
//                          9/10-> spare
//
  static integer _scalef;  // MC/Data: scale factor used for digitization in DAQ-system
// 
  static integer _nfibpl[2];// num.of fibers per odd/even(1st/2nd) elementary layer in s-layer
  static integer _slstruc[6];//1->1st super-layer projection(0->X, 1->Y);
//                             2->num. of fib.layers per super-layer; 3->num.of super-layers (X+Y);
//                             4->num. of PM's per super-layer;
//                             5-6->readout dir. in X/Y-proj (=1/-1->+/-) for the 1st PM-cell. 
  static integer _fibcgr[ECFLSMX][ECFBCMX];// fiber grouping in readout PMcell:
  static integer _nfibpcl[ECFLSMX];// real fibers per layer in PMcell
//
public:  
//
//  Member functions :
//
// ---> function to read geomconf-file
  static void readgconf();
//
//
  static geant gendim(integer i);
  static geant fpitch(integer i);
  static geant rdcell(integer i);
  static integer scalef(){return _scalef;}
  static integer nfibpl(integer i);
  static integer slstruc(integer i);
  static integer fibcgr(integer i, integer j);
  static integer nfibpcl(integer i);
  static void fid2cidd(integer fid, integer cid[4], number w[4]);
  static void fid2cida(integer fid, integer cid[4], number w[4]);
  static void getscinfod(integer i, integer j, integer k,
         integer &pr, integer &pl, integer &ce, number &ct, number &cl, number &cz);
  static void getscinfoa(integer i, integer j, integer k,
         integer &pr, integer &pl, integer &ce, number &ct, number &cl, number &cz);
  static number segarea(number r, number ds);
	 
	 
//  
  static integer debug;
//
};
//===========================================================================
//
// ---------------> Class for JobControlStatistics  :
//
class EcalJobStat{
//
private:
  static integer mccount[ECJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => MCHits->RawEvent OK
  static integer recount[ECJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> 
public:
  static geant zprofa[2*ECSLMX];// average profile
  static void clear();
  static void addmc(int i){
    #ifdef __AMSDEBUG__
      assert(i>=0 && i< ECJSTA);
    #endif
    mccount[i]+=1;
  }
  static void addre(int i){
    #ifdef __AMSDEBUG__
      assert(i>=0 && i< ECJSTA);
    #endif
    recount[i]+=1;
  }
  static void printstat();
  static void bookhist();
  static void bookhistmc();
  static void outp();
  static void outpmc();
};
//
//===========================================================================
// ---------------> Class to store ECAL calibration constants :
//
class ECcalib{
//
private:
  integer _softid;  // SSPP (SS->S-layer number, PP->PMcell number)
  integer _status[4];  //4-SubCells each as HL(H->Hch,L->Lch) (H(L)=0/!=0->ok/problems)
  geant _pmrgain;    // PM relative(to ref.PM) gain (if A=(sum of 4 SubCells) pmrgain = Agiven/Aref)
  geant _scgain[4]; // relative(to averaged) gain of 4 SubCells(highGain chain)(average_of_four=1 !!!)
  geant _hi2lowr[4]; // ratio of gains of high- and low-chains (for each of 4 SubCells)
  geant _an2dyr;    // 4xAnode_pixel/dynode signal ratio
  geant _adc2mev;   // Global(hope) Signal(ADCchannel)->Emeas(MeV) conv. factor (MeV/ADCch)
public:
  static ECcalib ecpmcal[ECSLMX][ECPMSMX];
  ECcalib(){};
  ECcalib(integer sid, integer sta[4], geant pmg, geant scg[4], geant h2lr[4], geant a2dr,
       geant conv):_softid(sid),_pmrgain(pmg),_an2dyr(a2dr),_adc2mev(conv){
    for(int i=0;i<4;i++){
      _status[i]=sta[i];
      _scgain[i]=scg[i];
      _hi2lowr[i]=h2lr[i];
    }
  };
  integer getstat(int i){return _status[i];}
  geant pmrgain(){return _pmrgain;}
  geant pmscgain(int i){return _scgain[i];}
  geant hi2lowr(integer subc){return _hi2lowr[subc];}
  geant adc2mev(){return _adc2mev;}
  geant an2dyr(){return _an2dyr;}
  static void build();
};
//
//===========================================================================
// --------------> Class for general "time-Variable"  parameters :
class ECALVarp {  
//
private:
// ---> ECAL DAQ-system thresholds :
  geant _daqthr[10];   // DAQ-system thresholds
          // (0) -> indiv. high-channel readout threshold(ADCch)
	  // (1) -> Anode(high)-sum(~mV) low cut for "MIP"-trigger(mev tempor)  
	  // (2) ->                      high cut for "MIP"-trigger(mev tempor)  
	  // (3) ->                      low cut for "High"-trigger(mev tempor)
	  // (4) -> indiv. low-channel readout threshold(ADCch)  
	  // (5) -> indiv. dynode thresh. for trigger  
	  // (6) -> spare  
	  // (7) ->   
	  // (8) ->   
	  // (9) ->   
// ---> RECO Run-Time Cuts :
  geant _cuts[5];                    
          //  (0)  -> cut for cluster search(mev)
          //  (1)  -> 
public:
  static ECALVarp ecalvpar;
  ECALVarp(){};
// member functions :
//
  void init(geant daqthr[10], geant cuts[5]);
//
  geant daqthr(int i){;
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>=0 && i<10);
      }
    #endif
    return _daqthr[i];}
//
  geant rtcuts(int i){;
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>=0 && i<5);
      }
    #endif
    return _cuts[i];}
//
};
//===================================================================
// class to store ECAL PM-peds/sigmas  :
class ECPMPeds{
//
private:
  integer _softid;  // SSPP (SS->S-layer number, PP->PMcell number)
  geant _pedh[4]; // ped for high-channel of 4 SubCells(pixels)(in ADCchannels)
  geant _pedl[4]; // ped for low-channel of 4 SubCells(pixels)
  geant _sigh[4]; // sigma for high-channel of 4 SubCells(pixels)
  geant _sigl[4]; // sigma for high-channel of 4 SubCells(pixels)
//
public:
  static ECPMPeds pmpeds[ECSLMX][ECPMSMX];
  ECPMPeds(){};
  ECPMPeds(integer sid, geant pedh[4], geant sigh[4],
                         geant pedl[4], geant sigl[4]):_softid(sid){
    for(int i=0;i<4;i++){
      _pedh[i]=pedh[i];
      _sigh[i]=sigh[i];
      _pedl[i]=pedl[i];
      _sigl[i]=sigl[i];
    }
  };
  void getpedh(geant pedh[4]){
    for(int i=0;i<4;i++)pedh[i]=_pedh[i];
  }
  void getpedl(geant pedl[4]){
    for(int i=0;i<4;i++)pedl[i]=_pedl[i];
  }
  void getsigh(geant sigh[4]){
    for(int i=0;i<4;i++)sigh[i]=_sigh[i];
  }
  void getsigl(geant sigl[4]){
    for(int i=0;i<4;i++)sigl[i]=_sigl[i];
  }
  integer getsid(){return _softid;}
  static void build();
};
//
#endif

