#include <typedefs.h>
#include <tofdbc02.h>  
//  Some classes for calibrations. E.Choumilov
//
//  class to manipulate with TZSL-calibration data :
class TOF2TZSLcalib {
private:
  static geant slope;
  static geant tzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s1;
  static number s2;
  static number s3[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static number s4;
  static number s5;
  static number s6[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s7[TOF2GC::SCMXBR];
  static number s8;
  static number s9[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s10[TOF2GC::SCMXBR];
  static number s11;
  static number s12[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s13[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s14[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
  static number s15[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
  static number s16[TOF2GC::SCMXBR];
  static number events;
  static number resol;
public:
  static void init(){
    int i,j,il,ib;
    slope=0.;
    s1=0.;
    s2=0.;
    s4=0.;
    s5=0.;
    s8=0.;
    s11=0.;
    events=0.;
    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
      for(il=0;il<TOF2GC::SCLRS;il++){
        tzero[il][ib]=0.;
        s3[il][ib]=0.;
      }
      for(i=0;i<(TOF2GC::SCLRS-1);i++){
        s6[i][ib]=0.;
        s9[i][ib]=0.;
        s15[i][ib]=0.;
      }
      s7[ib]=0.;
      s10[ib]=0.;
      s16[ib]=0.;
      for(j=0;j<TOF2GC::SCMXBR;j++){
        s12[ib][j]=0.;
        s13[ib][j]=0.;
        s14[ib][j]=0.;
      }
    }
  };
  static void fill(number beta,int ib[4],number tld[3],number tdi[3],number dum[3]);
  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void mfit();
  static void select();
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
};
//------------------------------------------------------------------------
//
//  class to manipulate with TDIF-calibration data :
class TOF2TDIFcalib {
private:
  static integer nbins[TOF2GC::SCBTPN];//numb.of coord-bins vs bar-type(<=TOF2GC::SCTDBM)
  static number tdiff[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// side-times differences (ns)
  static number tdif2[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// square of ...
  static number clong[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// impact longit.coordinates(cm)
  static integer nevnt[TOF2GC::SCBLMX][TOF2GC::SCTDBM];// event counters
public:
  static void init();
  static void fill(integer il, integer ib, number td, number coo);
  static void select();
  static void fit();
};
//-----------------------------------------------------------------------
//  class to manipulate with AMPL-calibration data :
class TOF2AMPLcalib {
private:
  static number ambin[TOF2GC::SCBTBN][TOF2GC::SCACMX];// 2_sides-signals for each ref_bar/bin/event
  static integer nevenb[TOF2GC::SCBTBN];// numb.of events accum. per ref_bar/bin for ambin
  static number amchan[TOF2GC::SCCHMX][TOF2GC::SCACMX];// side-signals for each channel/event
  static integer nevenc[TOF2GC::SCCHMX];// numb.of events accum. per channel for amchan
  static geant gains[TOF2GC::SCCHMX];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
  static geant btamp[TOF2GC::SCBTBN];// trunc. mean. bar-signals for each bin(ref.bars) 
  static integer rbls[TOF2GC::SCBTPN];// list of ref. bars (diff. by bar type)(LBB)
  static geant profb[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// profile bins width(counting from "-" side)
  static geant profp[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// bin middle-point values
  static integer nprbn[TOF2GC::SCBTPN];// real number of prof.bins according to bar-type
  static number a2dr[TOF2GC::SCCHMX];// sum of anode/dinode signal-ratios for each channel
  static number a2dr2[TOF2GC::SCCHMX];// sum of anode/dinode (signal-ratios)**2
  static integer nevenr[TOF2GC::SCCHMX];// number of events/channel for above sum
  static number ammrfc[TOF2GC::SCBTPN];// mean charge (central incidence) for ref.counters
  static integer nevrfc[TOF2GC::SCBTPN];// number of accum. events for above sum
  static number arefb[TOF2GC::SCBTPN][TOF2GC::SCACMX];// tot-signals for each ref_bar/event
  static integer nrefb[TOF2GC::SCBTPN];// numb.of events accum. per ref_bar
  static geant fpar[10];// parameters to fit
  static integer nbinr;// working arrays for A-profile fit
  static number mcharge[TOF2GC::SCPRBM]; 
  static number mcoord[TOF2GC::SCPRBM];
  static number aprofp[TOF2GC::SCBTPN][TOF2GC::SCPROFP];// A-profile parameters(ref.bars)
  static number clent;//bar-length (to exchange with FCN)
  static integer cbtyp;//bar-type  (....................)
//
  static geant binsta[50];// bin-statistics arrays for dE/dX fit
  static geant bincoo[50];// bin-coordinate ...
  static integer elbt;//     tot.bins
  static geant elfitp[TOF2GC::SCELFT];// fit-parameters
  static char eltit[60];  // title for fit
public:
  static void init();
  static void fill(integer il, integer ib, geant ama[2], geant coo);
  static void fillabs(integer il, integer ib, geant ama[2], geant coo, number mom,
                                                                     number btof);
  static void filla2d(integer il, integer ib, geant ama[2], geant amd[2]);
  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void melfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void select();
  static void fit();
};
//-----------------------------------------------------------------------
//
//  class to manipulate with AvsD-calibration data :
class TOF2AVSDcalib {
private:
  static number dtdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean adc-low per chan/bin 
  static number dtdyn2[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean (adcl)**2 per chan/bin
  static integer nevdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// events in above sums per chan/bin
  static integer nevdynt[TOF2GC::SCCHMX];// events in above sums per chan
public:
  static void init();
  static void filltovt(integer chan, geant tovta, geant tovtd);
  static void fit(number a2d[], number a2do[], number &av, number &avo);
};
//-----------------------------------------------------------------------
//  class to manipulate with STRR-calibration (StretcherRatio) data:
class TOF2STRRcalib {
private:
  static number dtin[TOF2GC::SCCHMX][TOF2GC::SCSRCHB]; // to calc. mean dtin per chan/bin (150 bins x 1ns)
  static number dtinq[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];//to calc. mean square dtin per chan/bin
  static integer nevnt[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];// events per chan/bin 
  static integer nevtot[TOF2GC::SCCHMX];    // total number of analized events/chan
  static integer nevnt2[TOF2GC::SCCHMX];// event counters for f/s-tdc time difference
  static number fstdif[TOF2GC::SCCHMX];// f/s-tdc time difference
  static number fstdif2[TOF2GC::SCCHMX];// squares ....
  static number sbins[TOF2GC::SCCHMX];// for "points-fit" method
  static number ssumc[TOF2GC::SCCHMX];
  static number ssumt[TOF2GC::SCCHMX];
  static number ssumid[TOF2GC::SCCHMX];
  static number ssumct[TOF2GC::SCCHMX];
  static number ssumc2[TOF2GC::SCCHMX];
  static number ssumt2[TOF2GC::SCCHMX];
public:
  static void init();
  static void fill(integer ichan, number tm[3]);
  static void fill2(integer ichan, number tdif);
  static void outp(); 
};
//--------------------------
