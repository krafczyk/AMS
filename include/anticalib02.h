#include "typedefs.h"
#include "antidbc02.h"  
//  Some classes for calibrations. E.Choumilov
//-----------------------------------------------------------------------
// Gain/AttLen calibration:
//
//  class to manipulate with ANTI-calibration data :
class AntiCalib {
private:
  static integer evs2bin[ANTI2C::CalChans][ANTI2C::LongBins];
  static number adccount[ANTI2C::CalChans][ANTI2C::LongBins][ANTI2C::BinEvsMX];//indiv.adc for each bin
  static number fitpars[5];//working arrays for profile-fit
  static integer fitbins;
  static number values[ANTI2C::LongBins]; 
  static number errors[ANTI2C::LongBins]; 
  static number coords[ANTI2C::LongBins];
//
public:
  static void init();
  static void fill(integer secn, geant ama[2], geant coo);
  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void select();
  static void fit();
};
//----------------------------------------------------------------------------------
//
const integer ATPCSTMX=100;// PedCalib max.values's stack size (max)
const integer ATPCEVMN=100;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer ATPCEVMX=1000;//max.statistics on events/channel
const geant ATPCSPIK=20.;//Anode ADC-value(adc-ch) to be considered as spike(~1mip)
//
//  class to manipulate with PedSig-calibration  data:
class ANTPedCalib {
private:
  static number adc[ANTI2C::MAXANTI][2];//store Anode/Dynode adc sum
  static number adc2[ANTI2C::MAXANTI][2];//store adc-squares sum
  static number adcm[ANTI2C::MAXANTI][2][ATPCSTMX];//max. adc-values stack
  static number port2r[ANTI2C::MAXANTI][2];//portion of hi-ampl to remove
  static integer nevt[ANTI2C::MAXANTI][2];// events in sum
  static geant peds[ANTI2C::MAXANTI][2];
  static geant dpeds[ANTI2C::MAXANTI][2];
  static geant thrs[ANTI2C::MAXANTI][2];
  static geant sigs[ANTI2C::MAXANTI][2];
  static uinteger stas[ANTI2C::MAXANTI][2];
  static integer nstacksz;//really needed stack size (ev2rem*ANPCEVMX)
  static integer hiamap[ANTI2C::MAXANTI];//high signal Paddles map (1 event)
#pragma omp threadprivate (hiamap) 
  static time_t BeginTime;
  static uinteger BeginRun;
//
  class ANTPedCal_ntpl {
    public:
      integer Run;
      integer Sector;
      geant   PedA[2];
      geant   SigA[2];
      integer StaA[2];
  };
  static ANTPedCal_ntpl ANTPedCalNT;
//
public:
  static void init();
  static void initb();
  static void resetb();
  static void fill(int ls, int is, geant adc);//ls->logical sector(1of8),is->side(bot/top)
  static void filltb(int ls,int is,geant pd,geant dpd,geant thr,geant sg);//to use OnBoardTable
  static void outp(int flg);
  static void outptb(int flg);
  static void hiamreset(){
    for(int i=0;i<ANTI2C::MAXANTI;i++)hiamap[i]=0;
  } 
  static time_t & BTime(){return BeginTime;}
  static uinteger & BRun(){return BeginRun;}
  static void ntuple_close();
};
