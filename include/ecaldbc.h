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
//
// ECAL global constants definition
//
const integer ECFLSMX=10; // max. fiber-layers per S-layer
const integer ECFBCMX=12; // max. fibers per layer in PMcell
const integer ECSLMX=9; // max. S(uper)-layers
const integer ECPMSMX=36; // max. PMCell(PM's) per S-layer
const integer ECFBLMX=500;// max. fibers per layer
const integer ECROTN=10000; // geant numbering of ECAL rot. matr.(starting from...)
const integer ECJSTA=10; // max size of counter-array for job statistics
const integer ECHIST=2000;// Ecal histogram number(starting from...) 
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
  static geant _mev2adc; // MC: Emeasured-MeV->ADCchan conv.factor (adc/mev)
  static geant _mev2mev; // MC: dE/dX-MeV->Em-Mev conv.factor 
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
  static geant mev2adc();
  static geant mev2mev();
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
  inline static void addmc(int i){
    assert(i>=0 && i< ECJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< ECJSTA);
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
  integer _status;  // (!=0/0->problems/ok)
  geant _chgain;    // common HV-gain of 4 SubCells in PMCell (wrt ref)
  geant _scgain[4]; // relat. gain of SubCells
  geant _adc2mev;   // ADCchannel->Emeas(MeV) conv. factor (mev/adc)
public:
  ECcalib(){};
  ECcalib(integer sid, integer sta, geant chg, geant scg[4], geant conv):
  _softid(sid),_status(sta),_chgain(chg),_adc2mev(conv){
    for(int i=0;i<4;i++)_scgain[i]=scg[i];
  };
  integer getstat(){return _status;}
  geant getchg(){return _chgain;}
  geant getscg(int i){return _scgain[i];}
  geant adc2mev(){return _adc2mev;}
  static void build();
};
//
//===========================================================================
// --------------> Class for general "time-Variable"  parameters :
class ECALVarp {  
//
private:
// ---> ECAL DAQ-system thresholds :
  geant _daqthr[5];   // DAQ-system thresholds
          // (0) -> indiv.channel readout threshold(ADCch)
	  // (1) -> Tot.adc low cut for "MIP"-trigger(ADCch)  
	  // (2) -> Tot.adc high cut for "MIP"-trigger(ADCch)  
	  // (3) -> Tot.adc low cut for "High"-trigger(ADCch)
	  // (4) -> spare  
// ---> Run-Time Cuts :
  geant _cuts[5];                    
          //  (0)  -> cut for cluster search(mev)
          //  (1)  -> 
public:
  ECALVarp(){};
// member functions :
//
  void init(geant daqthr[5], geant cuts[5]);
//
  geant daqthr(int i){;
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>=0 && i<5);
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
//
#endif

