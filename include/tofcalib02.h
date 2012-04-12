//  $Id: tofcalib02.h,v 1.22 2012/04/12 10:29:03 lquadran Exp $
#include "typedefs.h"
#include "tofdbc02.h"  
//  Some classes for calibrations. E.Choumilov
//     latest update 12.05.2008 by E.Choumilov
//     latest update 15.01.2009 by E.Choumilov
//-------------------------------
//  Joined class for TofTimeAmpl-params calibration:
class TofTmAmCalib {
private:
//common-part:
  //Tzslw-part:
  static geant slope;
  static geant tzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s0;
  static number s1;
  static number s3[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s30[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s4;
  static number s6[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s7[TOF2GC::SCMXBR];
  static number s8;
  static number s12[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s13[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s14[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s15[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s16[TOF2GC::SCMXBR];
  static number events;
  static number resol;
  static number resol1;
//Tdelv-part:
  static number _tdiff[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// side-times differences (ns)
  static number _tdif2[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// square of ...
  static number _clong[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// impact longit.coordinates(cm)
  static integer _nevnt[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// event counters
  static integer _nbins[TOF2GC::SCBTPN];//numb.of coord-bins vs bar-type(<=TOF2GC::SCTDBM)
//Ampl-part:
  static number ambin1[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s1-signals for each ref_bar/bin/event
  static integer nevenb1[TOF2GC::SCBTBN];// s1 events accum. per ref_bar/bin for ambin
  static number ambin2[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s2-signals for each ref_bar/bin/event
  static integer nevenb2[TOF2GC::SCBTBN];// s2 events accum. per ref_bar/bin for ambin
  static number amchan[TOF2GC::SCCHMX][TOF2GC::SCACMX];//centr.inc side-signals for each channel/event
  static integer nevenc[TOF2GC::SCCHMX];// numb.of events accum. per channel for amchan
  static geant SideMPA[TOF2GC::SCLRS][TOF2GC::SCMXBR][2];// side MP-signals ADCch(PM-equilization procedure)
  static geant gains[TOF2GC::SCCHMX];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
  static geant btamp[2][TOF2GC::SCBTBN];// MostProb bar-signals for each side/bin(ref.bars) 
  static geant ebtamp[2][TOF2GC::SCBTBN];// Error in MostProb  for each side/bin(ref.bars) 
  static integer rbls[TOF2GC::SCBTPN];// list of ref. bars (diff. by bar type)(LBB)
  static geant profb[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// profile bins width(counting from "-" side)
  static geant profp[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// bin middle-point values
  static integer nprbn[TOF2GC::SCBTPN];// real number of prof.bins according to bar-type
  
  static number a2dr[TOF2GC::SCCHMX];//sum of An/Sum(Dyn(pmt))-ratios for each channel
  static number a2dr2[TOF2GC::SCCHMX];//sum of ratios**2 for each channel
  static integer neva2d[TOF2GC::SCCHMX];//number of events/channel for above sum
  static number d2sdr[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of Dyn(pm)/Sum(Dyn(pm))-gains for LBBS
  static number d2sdr2[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of ratios**2 for each LBBS
  static integer nevdgn[TOF2GC::SCCHMX];//number of events/channel for above sum
  
  static number ammrfc[TOF2GC::SCBTPN];// mean charge (central incidence) for ref.counters
  static integer nevrfc[TOF2GC::SCBTPN];// number of accum. events for above sum
  static number arefb[TOF2GC::SCBTPN][TOF2GC::SCACMX];// tot-signals for each ref_bar/event
  static integer nrefb[TOF2GC::SCBTPN];// numb.of events accum. per ref_bar
  static geant fpar[10];// parameters to fit
  static integer nbinr;// working var. for A-profile fit
  static integer iside;// working var. for A-profile fit
  static geant fun2nb;// working var. for  fit
  static number mcharge[TOF2GC::SCPRBM]; 
  static number mcoord[TOF2GC::SCPRBM];
  static number emchar[TOF2GC::SCPRBM];
  static number aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP];// A-profile parameters(ref.bars)
  static number clent;//bar-length (to exchange with FCN)
  static integer cbtyp;//bar-type  (....................)
  static integer fitflg;//flag     (....................)
//
  static geant binsta[50];// bin-statistics arrays for dE/dX fit
  static geant bincoo[50];// bin-coordinate ...
  static integer elbt;//     tot.bins
  static geant elfitp[TOF2GC::SCELFT];// fit-parameters
  static char eltit[60];  // title for fit
  static geant TofTzSlope[TOF2GC::SCMXBR*TOF2GC::SCLRS]; // New Slewing parameters
//
public:
  static void initjob();
  static void endjob();
//  static uinteger & getJobFirstRunN(){return JobFirstRunN;}
//
  static void inittz();
  static void filltz(int ib[4],number tdi[3],number dum[3]);
  static void mfuntz(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void fittz();//Tzslw-fits, results->tzero,slope->file
  static void select();//Tdelv/Tzslw/Ampl-common select routine
  static geant getslop(){return slope;};
  static void gettzer(geant arr[]){
    int cnum;
    for(int il=0;il<TOF2GC::SCLRS;il++){
    for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
      cnum=il*TOF2GC::SCMXBR+ib;
      arr[cnum]=tzero[il][ib];
    }
    }
  };
//
  static void inittd();
  static void filltd(integer il, integer ib, number td, number coo);
  static void fittd();//Tdelv-fits, results->efflightvel,to->file
  
  static void initam();
  static void fillam(integer il, integer ib, geant ama[2], geant coo);
  static void fillabs(integer il, integer ib, geant ama[2], geant coo);
  static void filla2dg(int il, int ib, geant cin, geant ama[2], geant amd[2][TOF2GC::PMTSMX]);
  
  static void mfunam(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void melfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void fitam();
  static void endjam();
  static integer btyp2id(integer btyp){//id=LBB, btyp=1-11
    if(btyp>0 && btyp<=TOF2GC::SCBTPN)return rbls[btyp-1];
    else return(0); 
  }

};
//-----------------------------------------------------------------------
//===============> TOF PedCalib:
//
const integer TFPCSTMX=100;// PedCalib max.values's stack size (max)
const integer TFPCEVMN=100;//min ev/ch to calc.ped/sig(also for partial average calc)
const integer TFPCEVMX=1000;//max.statistics on events/channel
const geant TFPCSPIK=50.;//Anode ADC-value(adc-ch) to be considered as spike(~1mip)
//
//  class to manipulate with PedSig-calibration  data:
class TOFPedCalib {
private:
  static number adc[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store Anode/Dynode adc sum
  static number adc2[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store adc-squares sum
  static number adcm[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1][TFPCSTMX];//max. adc-values stack
  static number port2r[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//portion of hi-ampl to remove
  static integer nevt[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];// events in sum
  static geant peds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static geant dpeds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static geant thrs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static geant sigs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static uinteger stas[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
  static integer nstacksz;//really needed stack size (ev2rem*TFPCEVMX)
  static integer hiamap[TOF2GC::SCLRS][TOF2GC::SCMXBR];//high signal Paddles map (1 event) 
#pragma omp threadprivate (hiamap) 
  static time_t BeginTime;
  static uinteger BeginRun;
//
  class TOFPedCal_ntpl {
    public:
      integer Run;
      integer Layer;
      integer Paddle;
      geant   PedA[2];
      geant   SigA[2];
      geant   PedD[2][3];
      geant   SigD[2][3];
      integer StaA[2];
      integer StaD[2][3];
  };
  static TOFPedCal_ntpl TOFPedCalNT;
//
public:
  static void init();
  static void initb();
  static void resetb();
  static void fill(int il, int ib, int is, int pmt, geant adc);//pmt=0/1,2,3=>anode/dynode1,2,3
  static void filltb(int il,int ib,int is,int pmt,geant pd,geant dpd,geant thr,geant sg);//to store OnBoardPedTable elems
  static void outp(int flg);
  static void outptb(int flg);//to manipulation with final OnBoardPedTable
  static void hiamreset(){
    for(int i=0;i<TOF2GC::SCLRS;i++){
      for(int j=0;j<TOF2GC::SCMXBR;j++)hiamap[i][j]=0;
    }
  } 
  static time_t & BTime(){return BeginTime;}
  static uinteger & BRun(){return BeginRun;}
  static void ntuple_close();
};
//------------------------------------------------------------------
//==================> TOF-tdc calibration:
//  class to manipulate with PedSig-calibration  data:
class TOFTdcCalib{
private:
  static integer evpch[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2];//events/tdc_chan
  static number diflin[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//chann's diff.nonlin
  static geant intlin[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//chann's integr.nonlin
  static number avtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//average chip's temperature(use 1st ch entries only)
  static integer tempev[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//temper.statistics(use 1st ch. entries only)
  static geant mntemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//min ...................... 
  static geant mxtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//max ...................... 
  static geant iavtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//interm.aver ...................... 
  static integer istore[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//interm. storage
  static integer iupnum;//numb. of interm.storage file updates
  static time_t BeginTime;
  static uinteger BeginRun;
public:
  static void init();
  static void fill(int crt, int ssl, int ch, int tdc, geant temp);
  static void ifill(int crt, int ssl, int ch, int tdc, geant temp);
  static void outp(int flg);
};
