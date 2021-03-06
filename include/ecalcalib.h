#ifndef __AMSECCALIB__
#define __AMSECCALIB__
#include "typedefs.h"
#include "ecaldbc.h"

//  
// v1.0 E.Choumilov 08.12.2000
// class to manipulate with REUN-calib data:
//                  (REspons UNiformity)
//
// glob. consts for calibration :
//
const integer ECCLBMX=10;// max. long. bins for uniformity study(should be even)
//const integer ECLBMID=2;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECLBMID=3;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECCHBMX=100;// max. bins in hg-channel for h2lr study
const integer ECCADCR=1500;//max. HGainADC range to use ..............
const integer ECEVBUF=500;//size of buf. to store ev-by-ev PM-resp vs Lbin (FIAT-calib)
const integer ECEVEMX=1000;//max events/pm(pix,lbin)
const integer ECAHBUF=100;//size of highest amplitudes buffer(1/10 of ECEVEMX) 
//
class ECREUNcalib {


  private:
    static integer pxstat[ecalconst::ECPMSL][4];// PM-subcell status(-1/0/1...->unusable/ok/limited
    static integer dystat[ecalconst::ECPMSL];// PM-dynodes status(-1/0/1...->unusable/ok/limited
    static number pxrgain[ecalconst::ECPMSL][4];// PM-subcell(pixel) gain(4*(relat.to averaged over PM)) 
    static number pxrgainc[ecalconst::ECPMSL][4];// as above but G-corrected (to check calib) 
    static number pmrgain[ecalconst::ECPMSL];   // PM gain(relative to aver PM) 
    static number pmrgainc[ecalconst::ECPMSL];  // PM gain(relative to aver PM, after G-corr)) 
    static number pmlres[ecalconst::ECPMSL][ECCLBMX][ECEVBUF];//to store ev-by-ev PM vs Longit.bin responce    
//    static number pmlres[ecalconst::ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce    
//    static number pmlres2[ecalconst::ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce err.
//    
    static number pmcresp[ecalconst::ECPMSL];//for PM rel.gains(central bins aver.responce)    
    static number pmcrahb[ecalconst::ECPMSL][ECAHBUF+1];//Ahighest buff .....................    
    static number pmcrespc[ecalconst::ECPMSL];//for PM rel.gains(central bins aver.responce, G-corr)    
    static integer pmccros[ecalconst::ECPMSL];//............................................
    static integer pmcrahm[ecalconst::ECPMSL];//Ahighest buff members    
//    
    static number sbcres[ecalconst::ECPMSL][4];//to store SubCell responce    
    static number sbcresc[ecalconst::ECPMSL][4];//to store SubCell responce, Gain-corrected    
    static number hi2lowr[ecalconst::ECPMSL][4];// h/l-gain ratios for each PM-subcell(pixel)
    static number hi2lowo[ecalconst::ECPMSL][4];// h/l-gain offsets for each PM-subcell(pixel)
    static number an2dynr[ecalconst::ECPMSL];// Anode/Dynode ratios for each PM
    static number a2dnevs[ecalconst::ECPMSL];// .............Nevents...........
    static integer tevpml[ecalconst::ECPMSL][ECCLBMX];// tot.events/Lbin/pm_fired (FIAT-calib)
    static number tevsbf[ecalconst::ECPMSL][4];// tot.events/subcell_fired 
    static number tevsbc[ecalconst::ECPMSL][4];// tot.events/subcell_crossed 
    static number tevpmc[ecalconst::ECPMSL];// tot.events/pm_crossed 
    static number tevpmf[ecalconst::ECPMSL];// tot.events/pm_fired 
    static number tevpmm[ecalconst::ECPMSL];// tot.events/pm_crossed_at_2_central_long_bins 
    static number tevhlc[ecalconst::ECPMSL*4][ECCHBMX];// tot.events/cell/bin for Hb/Lg calibr 
    static number sbchlc[ecalconst::ECPMSL*4][ECCHBMX];// cell-resp.(low-gain) for Hg/Lg calibr 
    static number sbchlc2[ecalconst::ECPMSL*4][ECCHBMX];// cell-resp.**2(low-gain) for Hgain/Lgain calibr 
    static number sbchlc3[ecalconst::ECPMSL*4][ECCHBMX];// cell-resp.(hi-gain) for Hgain/Lgain calibr 
    static number values[ECCLBMX];//working arrays for profile-fit 
    static number errors[ECCLBMX]; 
    static number coords[ECCLBMX];
    static integer nbins; 
    static number slslow;//averaged light att. slopes/%
    static number slfast;
    static number fastfr;
    static integer nfits;// counter for averaging
    static number et2momr;//aver. of Etot/Mom ratios
    static integer net2mom;// number of Etot/Mom measurements
  public:
    static void init();
    static void fill_1(integer,integer,integer,integer, number );
    static void fill_2(integer,integer,integer,number a[2]);
    static void fill_3(integer,number,number);
    static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
    static void mfit();
    static void makeToyRLGAfile();
    static void select();
    static void selecte();
    static void mfite();
}; 
//---------------------------------------------------------------------

//==============================> "On-Data" Pedestal/sigma Calibration:
//
// this logic use REAL events when both (raw and compressed) formats are
// red out. The peds/sigmas are calculated at the end of the job
// and can be saved into DB and/or ped-file.
//
const integer ECPCSTMX=500;// PedCalib max.values's stack size (max)
const integer ECPCEVMN=50;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer ECPCEVMX=1000;//max.statistics on events/channel
const geant ECPCSPIK=10.;//signal threshold to be the "Spike"(~>1mips in Hgain chan)
//
class ECPedCalib {
  private:
    static time_t BeginTime;
    static uinteger BeginRun;
    static number adc[ecalconst::ECPMSL][5][2];
    static number adc2[ecalconst::ECPMSL][5][2];//square
    static number adcm[ecalconst::ECPMSL][5][2][ECPCSTMX];//max's  stack
    static number port2r[ecalconst::ECPMSL][5][2];//portion of hi-ampl to remove
    static integer nevt[ecalconst::ECPMSL][5][2];
    static integer hiamap[ecalconst::ECSLMX][ecalconst::ECPMSMX];//high signal PMTs map (1 event)
#pragma omp threadprivate (hiamap) 
    static geant peds[ecalconst::ECPMSL][5][2];
    static geant dpeds[ecalconst::ECPMSL][5][2];
    static geant thrs[ecalconst::ECPMSL][5][2];
    static geant sigs[ecalconst::ECPMSL][5][2];
    static uinteger stas[ecalconst::ECPMSL][5][2];
//    static geant rpeds[ecalconst::ECPMSL][5][2];
//    static geant rsigs[ecalconst::ECPMSL][5][2];
//    static uinteger rstas[ecalconst::ECPMSL][5][2];
    static integer nstacksz;//really needed stack size (ev2rem*ECPCEVMX)
//
    class ECPedCalib_ntpl {//to store PedTable-event(i.e.particular Run,SL,PM) in ntuple
      public:
        integer Run;
        integer SLayer;
        integer PmtNo;
        geant Pedh[5];
        geant Sigh[5];
        geant Pedl[5];
        geant Sigl[5];
        integer Stah[5];
        integer Stal[5];
    };
    static ECPedCalib_ntpl ECPedCalNT;
//
  public:
    static void init();
    static void resetb();
    static void initb();
    static void hiamreset(){for(int i=0;i<ecalconst::ECSLMX;i++)
                                          for(int j=0;j<ecalconst::ECPMSMX;j++)hiamap[i][j]=0;
                           }
    static void fill(integer swid, geant val);
    static void filltb(integer swid, geant ped, geant dped, geant thr, geant sig);
    static void outp(int flg);
    static void outptb(int flg);
    static time_t & BTime(){return BeginTime;}
    static uinteger & BRun(){return BeginRun;}
    static void ntuple_close();
};
//-----------------------------------------------------------------

/*

#include "ecid.h"


class AMSECIdCalib: public AMSECIds{
protected:

static integer  _Count[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static integer  _BadCh[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number   _ADC[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number   _ADCMax[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static number  _ADC2[ecalconst::ECPMSMX][ecalconst::ECSLMX][4][3];
static time_t _BeginTime;
static uinteger _CurRun;

class ECCalib_def {
public:
integer Run;
integer SLayer;
integer PmtNo;
integer Channel;
integer Gain;  //high low dynode
geant   Ped;
geant   ADCMax;
geant   Sigma;
integer BadCh;
};

static ECCalib_def ECCALIB;

public:
AMSECIdCalib():AMSECIds(){};
AMSECIdCalib(const AMSECIds &o):AMSECIds(o){};
static void getaverage();
static void write();
static void clear();
static void init();
void updADC(uinteger adc,uinteger gain);
static uinteger & Run(){return _CurRun;}
static time_t & Time(){return _BeginTime;}
static void buildSigmaPed(integer n, int16u *p);
static void buildPedDiff(integer n, int16u *p);
number getADC(uinteger gain) const{return gain<3?_ADC[_pmt][_slay][_pixel][gain]:0;}
number getADCMax(uinteger gain) const{return gain<3?_ADCMax[_pmt][_slay][_pixel][gain]:0;}
number getADC2(uinteger gain) const{return gain<3?_ADC2[_pmt][_slay][_pixel][gain]:0;}
number & setADC(uinteger gain) {return _ADC[_pmt][_slay][_pixel][gain<3?gain:0];}
number & setADC2(uinteger gain) {return _ADC2[_pmt][_slay][_pixel][gain<3?gain:0];}
uinteger getcount(uinteger gain) const{return gain<3?_Count[_pmt][_slay][_pixel][gain]:0;}






};
*/

#endif
