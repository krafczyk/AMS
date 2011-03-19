//  $Id: tofdbc02.h,v 1.50 2011/03/19 16:25:17 choumilo Exp $
// Author E.Choumilov 13.06.96.
//
// Last edit : Jan 21, 1997 ak. !!!! put back friend class TOFDBcD
// Last edit : june 27, 1997 EC, add some DAQ-constants
// Add TOF-charge classes, E.Choumilov 19.01.2005
// Removed all TOF gain5 logic, E.Choumilov 22.08.2005
//
#ifndef __TOF2DBC__
#define __TOF2DBC__
#include "typedefs.h"
#include "cern.h"
#include "commons.h"
#include "amsdbc.h"
#include "extC.h"
#include <string.h>
#include <stdlib.h>
//#include "job.h"
// #include <daqblock.h>
//
//
#include "tofanticonst.h"
//===========================================================================
// 
// --------------> Class for "time-stable"  parameters :
class TOF2DBc {  
//
private:
// geom. constants:
  static integer _brtype[TOF2GC::SCBLMX];  // layer-map of bar types (diff. by length)
  static integer _plrotm[TOF2GC::SCLRS]; // TOF planes rotation mask
  static integer _planes;                // real TOF planes
  static integer _bperpl[TOF2GC::SCLRS]; // real bars per plane
  static geant _brlen[TOF2GC::SCBTPN]; // bar lengthes for SCBTPN types
  static geant _lglen[TOF2GC::SCBTPN]; // eff.light-guide lengthes for SCBTPN types
  static geant _outcp[TOF2GC::SCLRS][4]; // outer counters param(wid/xc/yc/lgw) vs layer
  static geant _supstr[12]; // supporting structure parameters
  static geant _plnstr[20]; // sc. plane structure parameters
// MC/RECO constants:
  static geant _edep2ph;     // MC edep(Mev)-to-Photons convertion
  static geant _adc2q;       // not used (now taken from TFCA card #21) 
  static geant _fladctb;     // MC flash-ADC time-binning (ns)
  static geant _tdcscl;      // max TDC-scale
  static geant _tdcbin[4];   // TDC binning for Time(FT)TDC, supl.DAQ binning for ADC, etc
  static geant _trigtb;      // MC time-binning in logic(trig) pulse handling
  static geant _strflu;      // Stretcher "end-mark" time fluctuations (ns)
  static geant _daqpwd[15];  // DAQ-system pulse_widths/dead_times/...
  static geant _tdctrdel;    // TDC Trig(Lvl1)suppl.delay on SFET(A) board
  static geant _tdctrlat;    // TDC trig.latency
  static geant _tdcmatw;     // TDC matching window
  static geant _lev1del;     // "Lev-1" signal delay wrt FT (JLV1 decision, ns)
  static geant _ftc2cj;      // FT signal crate-to-crate jitter(ns)  
  static geant _fts2sj;      // .......... slot-to-slot  jitter(ns)
  static geant _ltagew[2];   // true LTtime-hit wrt FT age-window(ns) 
  static geant _ftdelm;      // FT max delay (allowed by stretcher logic) (ns)
  static geant _clkperJLV;      // JLVTrig.electronics(JLV1-crate) clock period(ns)
  static geant _clkperSPT;      // SPTpreTrig.electronics(S-crates) clock period(ns)
  static integer _pbonup;    // set phase bit "on" for leading(up) edge (yes/no->1/0)
  static integer _nsespar;   // number of PMT SingleElectrSpectrum(SES) parameters
  static geant _sespar[TOF2GC::SCBTPN][TOF2GC::SESPMX];//SES params for each btyp
//                       (for now: 1st par->(MostProb,mV on 50 Ohm);2nd->rms(relat to MP))
  static geant _tofareftem[3];//RefTemperatures for SFET(A),PMT,SFEC
//
public:  

#ifdef __DB__
   friend class TOFDBcD;
#endif
//
//  Member functions :
//
// ---> function to read geomconf-file  :
  static void readgconf();

//---
  static integer brtype(integer ilay, integer ibar);
//
  static integer npmtps(integer ilay, integer ibar);
//
  static integer barseqn(integer ilay, integer ibar);
//
  static geant brlen(integer ilay, integer ibar);
//
  static geant lglen(integer ilay, integer ibar);
//
  static integer plrotm(integer ilay);
//
  static integer getbppl(integer ilay);
//
  static integer getnplns(){
    return _planes;
  }
//
  static geant supstr(int i);
//
  static geant plnstr(int i);
//
  static geant outcp(int i, int j);
//
//
//   function to get Z-position of scint. bar=ib in layer=il
  static geant getzsc(integer il, integer ib);
// function to get transv. position of scint. bar=ib in layer=il
  static geant gettsc(integer il, integer ib);
// functions to get MC/RECO parameters:
//
  static geant edep2ph();
  static geant fladctb();
  static geant tdcscl();
//
  static geant lev1del();
  static geant ltagew(int i);
  static geant ftdelm();
  static geant clkperJLV();
  static geant clkperSPT();
  static geant sespar(int ibt, int ipar);
  static geant tofareftem(int i){return _tofareftem[i];}
  static int nsespar();
  static geant adc2q();
  static integer pbonup();
//
  static geant ftc2cj();
  static geant fts2sj();
//
  static geant strjit2();
//
  static geant strflu();
//
  static geant tdctrdel();
  static geant tdctrlat();
  static geant tdcmatw();
//
  static geant tdcbin(int i);
//
  static geant daqpwd(int i);
//
  static geant trigtb();
//  
  static integer debug;
};
//===================================================================
// 
// --------------> Class for TOF2 slow temperature data :
class TofSlowTemp {
  private:
    geant _stemp[TOF2GC::SCLRS][16];//2*8(chain A->B) sensors temperatures
    uinteger _sta[TOF2GC::SCLRS][16];//2*8 sensors status (0/1->bad/ok)
  public:
    static TofSlowTemp tofstemp; 
  TofSlowTemp(){};
// member functions :
//lay=0,3;side=0,1;sen=0,7;chain=0,1->A,B.
  geant gettemp(int lay, int sen, int chain){return _stemp[lay][sen+8*chain];}
  uinteger getsta(int lay, int sen, int chain){return _sta[lay][sen+8*chain];}
  void settemp(int lay, int sen, int chain, geant temp){_stemp[lay][sen+8*chain]=temp;}
  void setsta(int lay, int sen, int chain, uinteger sta){_sta[lay][sen+8*chain]=sta;}
  int gettempC(int crat, int slot, geant & atemp);//average(over 2 sensors) SFEC-card temper
// (return 1/0->ok/fail)
  int gettempP(int lay, int side, geant & atemp);//average(over  
//"PM"-sensors(2*3) in layer/side of TOF-plane envelop) temper, return 1/0->ok/fail
  void init(); 
};  
//===================================================================
// 
// --------------> Class for general "time-Variable"  parameters :
class TOF2Varp {  
//
private:
// ---> TOF DAQ-system thresholds :
  geant _daqthr[5];   // DAQ-system thresholds (defaults)
// ---> Cuts :
  geant _cuts[10];                    
          //  (0) lhtdw   -> t-window(+-) around (LT-sHT)-mp for pairing LT/sumHT-hits
          //  (1) hiscutb -> "befor"-cut in time history(sumHT-ch) (ns)
          //  (2) hiscuta -> "after"-cut in time history(sumHT-ch) (ns)
          //  (3) lcoerr  -> "err. in longit.coord. measurement
          //  (4) ftdelf  -> FT decision_time+fixed delay
          //  (5) lhtdmp ->  MostProb val of LT-SumHT true time-hits 
          //  (6) eclass  -> assim.cut for TOFCluster energy calc.
          //  (7) Tdtemp  -> T-type def.temper (SFET(A))
          //  (8) Pdtemp  -> P-type def.temper (PMTs)
          //  (9) Cdtemp  -> C-type def.temper (SFEC)
//
public:
  static TOF2Varp tofvpar;
  TOF2Varp(){};
// member functions :
//
  void init(geant daqthr[5], geant cuts[10]);
//
  geant lhtdw(){return _cuts[0];}
  geant hiscutb(){return _cuts[1];}
  geant hiscuta(){return _cuts[2];}
  geant lcoerr(){return _cuts[3];}
  geant ftdelf(){return _cuts[4];}
  geant lhtdmp(){return _cuts[5];}
  geant eclass(){return _cuts[6];}
  geant Tdtemp(){return _cuts[7];}
  geant Pdtemp(){return _cuts[8];}
  geant Cdtemp(){return _cuts[9];}
// 
  geant daqthr(int i){;
#ifdef __AMSDEBUG__
      if(TOF2DBc::debug){
        assert(i>=0 && i<5);
      }
#endif
    return _daqthr[i];}
//
};
//===================================================================
//
class TOF2JobStat{
//
private:
  static integer mccount[TOF2GC::SCJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => TovT->RawEvent OK
//          i=2 => Ghits->RawCluster OK
//          i=3 => Out-of-volume hit
//          i=4 => Fired TOF-bars
//          i=5 => MC flash-ADC overflows
//          i=9 => MC fTDC stack overflows
//          i=6 => MC stretch-TDC overflows
//          i=7 => MC anode-ADC overflows
//          i=8 => MC dynode-ADC overflows
//          i=10=> spare
//          i=11=> MC GHitT out of FADC
//          i=12=> MC GHitT total
//          i=13=> spare
//          i=14=> OK in MCgen fast selection
//          i=15-22=> FT-trig.finder statistics
  static integer recount[TOF2GC::SCJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> H/W TOF-trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5 -> spare
//          i=6 -> entries to TZSl-calibration 
//           =7 -> TZSl: multiplicity OK
//           =8 -> TZSl: no interaction
//           =9 -> TZSl: Tracker mom. OK
//          =10 -> TZSl: TOF-Tracker matching OK
//          =20 -> TZSL: TOF self-matching OK
//
//          =11 -> entries to AMPL-calibration
//          =12 -> AMPL: multiplicity OK
//          =13 -> AMPL: no interaction 
//          =14 -> AMPL: Track mom. OK
//          =15 -> AMPL: TOF-TRK match OK
//          =30 -> AMPL: TOF beta-fit OK
//          =31 -> AMPL: Final check OK
//
//          =16 -> entries to STRR-calibration
//           
//          =17 -> entries to TDIF-calibration
//          =18 -> TDIF: multiplicity OK
//          =19 -> TDIF: tracker OK
//           21-29,40 TOF-user event-counters
//          =33 -> TOF reco with TOF in LVL1
//          =34 -> TOF reco with EC in LVL1
//
//          =38 -> entries to PEDS-calibration
//          =39 -> entries to TDCL-calibration
//------
  static integer chcount[TOF2GC::SCCHMX][TOF2GC::SCCSTA];//channel statistics
//                              [0] -> RawSide channel entries(with FT-time!)  
//                              [1] -> "LTtdc-ON"     (Nstdc>0)
//                              [2] -> "SumHTtdc-ON"  (Nhtdc>0)
//                              [3] -> "SumSHTtdc-ON" (Nshtdc>0)
//                              [4] -> "AnodeADC-ON"  (Nadca>0)
//                              [5] -> "DynodeADC-ON" (Nadcd>0)
//                              [6] -> "FTtdc-1hit"   (Nftdc=1)
//                              [7] -> "LTtdc-1hit"   (Nstdc=1)
//                              [8] -> "(LTtdc&(Aadc|Dadc)-ON "
//                              [9] -> "Anode-adc overflow "
//                              [10]-> "Dynode-adc overflow "
//                              [11]-> "Miss.SumHT when LT and Aadc"
//
//                             [12] -> "Validate entries/channel"
//                             [13] -> "Missing FTtdc(unrecoverable)"
//                             [14] -> "Multy FTtdc-hits case"
//                             [15] -> "Missing LTtdc"
//                             [16] -> "Missing Anode"
//                             [17] -> "Missing Dynode when Anode-ovfl"
//                             [18] -> "Missing SumHT info"
//                             [19] -> "Missing LT @ Qa>0"
//                             [20] -> "Missing Qa @ LT>0"
//                             [21] -> "LT & Qa present"
//                             [22] -> "Missing Matched SumHT @ LT>0"
//------
  static integer brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];// bar statistics
//                               [0] -> RawEvent Bar(at least 1side) entries
//                               [1] -> "multipl-OK"
//                               [2] -> "history-OK"
//                               [3] -> "2-sided history-OK"
//                               [4] -> "Entries/bar for match-test"
//                               [5] -> "Tof/Trk transv.MatchOK"
//                               [6] -> "Tof/Trk transv+long.MatchOK"
//------
  static integer daqsf[160];//fragment statistics
  static integer cratr[TOF2GC::SCCRAT][30];//raw fmt
  static integer cratp[TOF2GC::SCCRAT][30];//processed fmt
  static integer cratc[TOF2GC::SCCRAT][30];//on-board-pedcal fmt
  static integer cratm[TOF2GC::SCCRAT][30];//mixFMT raw/comp blocks comparison
  static integer sltr[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
  static integer sltp[TOF2GC::SCCRAT][TOF2GC::SCSLTM][20];
  static integer rdcr1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcr4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp1[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp2[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp3[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static integer rdcp4[TOF2GC::SCSLTM][TOF2GC::SCRCHM][20];
  static geant cquality[4][5];//cal.quality 4 types X 5 params
  static integer tofstc[40];
//
  static geant tofantemp[TOF2GC::SCCRAT][TOF2GC::SCFETA];//temperatures(TempT) from SFET(A)-cards reading
#pragma omp threadprivate(tofantemp) 
//
public:
  static void clear();
  static void resettemp();
  static void puttemp(int16u crt, int16u sen, geant t);
  static geant gettemp(int16u crt, int16u sen);
  static void addmc(int i);
  static void addre(int i);
  static integer getre(int i);
  static geant &cqual(int i, int j){return cquality[i][j];} 
  static void addch(int chnum, int i);
  inline static int getch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < TOF2GC::SCCHMX && i < TOF2GC::SCCSTA);
    return chcount[chnum][i];
  }
  static void addbr(int brnum, int i);
  static void daqsfr(int16u ie);
  static void daqscr(int16u df, int16u crat, int16u ie);
  static void daqssl(int16u df, int16u crat, int16u slot, int16u ie);
  static void daqsch(int16u df, int16u crat, int16u slot, int16u rdch, int16u ie);
  static void addtofst(int ie);
  static void printstat();
  static void bookhist();
  static void bookhistmc();
  static void outp();
  static void outpmc();
};
//===================================================================
// class to store TOF2_bar calibration constants :
class TOF2Brcal{
//
private:
  integer softid;  // LBB(Layer|Bar(=paddle))
  integer npmts;   // Npmts per side
  integer status[2]; //2-sides calib.status F|S|A|D -> Anode(sumHT/LTchan)_TDC/
//                                                  Anode/Dynode_ADC, dec.bit=0/1->OK/Bad
  geant gaina[2]; // Anode-gain(PMT mainly)(S1/2; relative to some ref. bar of given type)
  geant gaind[2][TOF2GC::PMTSMX];//Dynode rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // A to D(equilized sum) ratio (in unsatur. region)
  geant asatl;  // anode_chain saturation limit(mev)(incl. PM,Shaper,...)
//                  (i.e. below use anode data, above  - dinode data)
  geant tthr;   // Time-discr. threshold (mV)
  geant strat[2][2];//<===Don't need now ! Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant fstrd;      // <===Don't need now ! same hit time difference between fast/slow TDC (ns)
  geant tzero;  // T0 (ns)
  geant slope; // slope for T vs (1/sqrt(Qa1)+1/sqrt(Qa2))
  geant slops[2];// indiv.slopes for Ts vs 1/Qs
  geant yctdif;// two ends time difference at counter center (ns)
  geant td2pos[2];// t_diff->position conv. factors(=Vlight,cm/ns) and coo-error(cm))
// for (at least) reference bar (in each bar type group) :
  geant mip2q;// 2-sides A-signal(pC/Mev) (at longitudinal(Y) coo=0(center))
  integer nscanp;// real number of scant points(longitudinal wdiv)
  geant yscanp[TOF2GC::SCANPNT]; // Y-positions of the scan points(bar type depend)
  geant relout[TOF2GC::SCANPNT]; // Relative(to Y=0) 2-Sides(sum) Light output
//(if some PMTs in some bar are dead, curve should be MC-calc. or measured)  
//
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr)
//
  geant unipar[2*TOF2GC::SCPROFP];// responce uniformity parameters (for ref.bars)
//
//
public:
  static TOF2Brcal scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  static uinteger CFlistC[11];//Cflist-file content
  TOF2Brcal(){};
  TOF2Brcal(integer sid, integer npm, integer sta[2], geant gna[2],
           geant gnd[2][TOF2GC::PMTSMX], 
           geant a2d[2], geant asl, geant tth, geant stra[2][2], geant fstd, 
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2],
           geant mip, integer nscp, geant ysc[], geant relo[], geant upar[], 
           geant a2q);
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
// status(is)=BA*1000000+F*100000+S*10000+A*1000+D(kji); i/j/k=1->PMDyn1/2/3 bad
//
  bool BarTimingDBOK(){
    integer sta;
    sta=status[0]/1000000;
    return(sta==0);// means Bar(both side) OK in DB for timing (according to Tzslw-calib)
  }                              
  bool FchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return(sta/100000==0);// means good sumHT-chan(anode)
  }
  bool SchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return((sta%100000)/10000==0);// means good LTtime-chan(anode)
  }
  bool AchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return((sta%10000)/1000==0);// means good Anode-ampl channel
  }
  bool DchOK(int isd, int ipm){//ipm=0-2
    integer sta;
    sta=status[isd]%1000000;
    int nml=(sta%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode-ampl ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode-ampl ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode-ampl ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return(
                     ((sta/100000)==0 || TFREFFKEY.relogic[1]>=1)//sumHT
                   && (sta%100000)/10000==0 //LT
		   && (sta%10000)/1000==0   //Anode
//		   && (sta%1000)==0         //Dynodes        
		                         );}
//
  void gtstrat(geant str[2][2]){
    str[0][0]=strat[0][0];
    str[0][1]=strat[0][1];
    str[1][0]=strat[1][0];
    str[1][1]=strat[1][1];
  }
  integer getnpm(){ return npmts;}
  geant gtfstrd(){return fstrd;}
  geant getasatl(){return asatl;}
  geant gettthr(){return tthr;}
  geant gettdif(){return yctdif;}
  geant getslope(){return slope;}
  geant gettzero(){return tzero;}
  geant getadc2q(){return adc2qf;}
  void getslops(geant sls[2]){
    sls[0]=slops[0];
    sls[1]=slops[1];
  }
  void getgaina(geant gn[2]){
    gn[0]=gaina[0];
    gn[1]=gaina[1];
  }
  geant getagain(int isd){
    return gaina[isd];
  }
  geant getgnd(int isd, int ipm){
    return gaind[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  void getupar(geant upar[]){
    for(int i=0;i<2*TOF2GC::SCPROFP;i++){upar[i]=unipar[i];}
  }
  
  void q2a2q(int cof, int sdf, int hlf, number &adc, number &q); //Q(pC)<->ADC(chan)
  geant adc2mip(int hlf, number am[2]); // A-measured s1+s2 ampl.(ADCch) -> (mev)
  geant adcd2mip(number am[2][TOF2GC::PMTSMX]); // D-measured s1+s2 ampl.(ADCch) -> (mev)
  void  adc2q(int hlf, number am[2],number qs[2]);// meas. side ampl.(ADCch)->Q(pC)
  geant poscor(geant point); // position correction 
  geant tm2t(number tm[2], number am[2], int hlf); // raw times(ns)/ampl(ns) -> Time (ns)
  geant tm2tr(number tm[2]); // tempor(lv3-games), raw times(ns) -> Non_ampl_corrected_Time (ns)
  void tmd2p(number tm[2], number am[2], int hlf,
       geant &pos, geant &epos); // raw times/ampl -> Position/err(cm) along counter
  void td2ctd(number tdo, number am[2], int hlf, number &tdn);// time_diff(ns)->corrected(by ampl) one 
  void getd2p(geant &td2p, geant &etd2p){
    td2p=td2pos[0];
    etd2p=td2pos[1];
  }
  static void build();
  static void setpars(integer cfvers);
};
//===================================================================
// class to store another set of MC TOF2_bar calibration constants
//  to use them as "MC Seeds" for simulation   :
class TOFBrcalMS{
//
private:
  integer softid;  // LBB
  integer status[2]; //2-sides cal.status as befor
  geant gaina[2]; // A-chain gain(PMT mainly)| (side-1/2; relative to some 
//                                                     reference bar of given type)
  geant gaind[2][TOF2GC::PMTSMX];//D(pm) rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // anode-to-dynode signal ratio (in unsatur. region)
  geant strat[2][2];  // Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr)
//
//
public:
  static TOFBrcalMS scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBrcalMS(){};
  TOFBrcalMS(integer sid, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX],
           geant a2d[2], geant stra[2][2],  geant a2q);
//
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
//
// status(is)=BA*1000000+F*100000+S*10000+A*1000+D(kji); i/j/k=1->PMDyn1/2/3 bad
//                              
  bool BarTimingDBOk(){
    return(status[0]/1000000==0);// means Bar(both side) OK in DB for timing (according to Tzslw-calib)
  }                              
  bool FchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return(sta/100000==0);// means good sumHT-chan(anode)
  }
  bool SchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return((sta%100000)/10000==0);// means good LTtime-chan(anode)
  }
  bool AchOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return((sta%10000)/1000==0);// means good Anode-ampl channel
  }
  bool DchOK(int isd, int ipm){//ipm=0-2
    integer sta;
    sta=status[isd]%1000000;
    int nml=(sta%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode-ampl ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode-ampl ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode-ampl ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){
    integer sta;
    sta=status[isd]%1000000;
    return(
                     ((sta/100000)==0 || TFREFFKEY.relogic[1]>=1)//sumHT
                   && (sta%100000)/10000==0 //LT
		   && (sta%10000)/1000==0   //Anode
//		   && (sta%1000)==0         //Dynodes        
		                         );}
//
  void gtstrat(geant str[2][2]){
    str[0][0]=strat[0][0];
    str[0][1]=strat[0][1];
    str[1][0]=strat[1][0];
    str[1][1]=strat[1][1];
  }
  geant getadc2q(){return adc2qf;}
  void getgaina(geant gn[2]){
    gn[0]=gaina[0];
    gn[1]=gaina[1];
  }
//  void getgaind(int ipm, geant gn[2]){
//    gn[0]=gaind[0][ipm];
//    gn[1]=gaind[1][ipm];
//  }
  geant getagain(int isd){
    return gaina[isd];
  }
  geant getgnd(int isd, int ipm){
    return gaind[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  
  void q2a2q(int cof, int sdf,int hlf, number &adc, number &q); //Q(pC)<->ADC(chan)
  static void build();
};
//===================================================================
// class to store TOF_bar anode/dynode peds/sigmas  :
class TOFBPeds{
//
private:
  integer softid;  // LBB
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  integer statd[2][TOF2GC::PMTSMX];//Dh-pmts statuses( =0/1->ok/bad) 
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
  geant pedd[2][TOF2GC::PMTSMX]; // D-pmts peds for side1/2
  geant sigd[2][TOF2GC::PMTSMX]; // D-pmts ped.sigmas .............................
//
public:
  static TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBPeds(){};
  TOFBPeds(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2],
           integer _statd[2][TOF2GC::PMTSMX],
           geant _pedd[2][TOF2GC::PMTSMX], geant _sigd[2][TOF2GC::PMTSMX]
  ):softid(_sid){
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
      for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
        statd[i][ipm]=_statd[i][ipm];
        pedd[i][ipm]=_pedd[i][ipm];
        sigd[i][ipm]=_sigd[i][ipm];
      }
    }
  };
  bool PedAchOK(int isd){return stata[isd]==0;}
  bool PedDchOK(int isd, int ipm){return (statd[isd][ipm]==0);}
  bool PedDchOK(int isd){return (statd[isd][0]==0 && statd[isd][1]==0 && statd[isd][2]==0);}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd)  {return stata[isd];}
  geant &apedd(int isd, int ipm)  {return pedd[isd][ipm];}  
  geant &asigd(int isd, int ipm)  {return sigd[isd][ipm];}
  integer &astad(int isd, int ipm)  {return statd[isd][ipm];}  
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  
  void getpedd(int ipm, geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=pedd[i][ipm];
  };
  void getsigd(int ipm, geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=sigd[i][ipm];
  };
  
  static void build();
  static void mcbuild();
};
//-------------------------------------------------
class TofElosPDF{
//TOF-eloss prob.density function for particular charge 
private:
  int ichar;//0,1,...index of particle(e,p,he,..)
  int charge;//charge(1,1,2,...)
  int nbins;//distribution_length(without 2ovfl)
  geant stpx;//bin width(MeV)
  geant xmin;//1st bin low edge(MeV)
  geant slope;//exp.slope to calc. pdf in ovfl-region
  geant unpdf;//const.level of pdf in undf-region 
  geant elpdf[TOF2GC::SCPDFBM];//PDF-array
  geant bpow;//beta-dependence power 
public:
  static TofElosPDF TofEPDFs[AMSChargConst::MaxZTypes];
  TofElosPDF(){};
  TofElosPDF(int ich, int ch, geant bp, int nb, geant stp, geant bnl, geant undf, geant ovfl, geant distr[]);
  int getnbins(){return nbins;}
  int getcharge(){return charge;}
  geant getstep(){return stpx;}
  geant getbpow(){return bpow;}
  number getlkhd(int nhits, int hstat[], number ehit[], number beta);
  static void build();  
}; 
//-------------------------------------------------
class TofTdcCor{
//To keep TofTdc Integral nonlinearity correction and other data
private:
  int _bmap;//bit-map of abs.chan.numbers presented in calib.data ( lsb -> 1st ch.)
  geant _icor[TOF2GC::SCTDCCH-2][1024];//integr.nonlin. corrections, based on 10 lsb of TDC-count(bin#)
public:
  TofTdcCor(){};
  TofTdcCor(int bmap, geant icor[TOF2GC::SCTDCCH-2][1024]):_bmap(bmap){
    for(int ch=0;ch<TOF2GC::SCTDCCH-2;ch++){
      for(int bn=0;bn<1024;bn++)_icor[ch][bn]=icor[ch][bn];
    }
  };
  geant getcor(int time, int ch);
//
  bool truech(int ch){
#ifdef __AMSDEBUG__
    if(TOF2DBc::debug){
      //assert(ch>=0 && ch < TOF2GC::SCTDCCH-2);
      if(!(ch>=0 && ch < TOF2GC::SCTDCCH-2)){
         cerr<<"/assert(ch>=0 && ch < TOF2GC::SCTDCCH-2) "<<ch<<endl;
      }
    }
#endif
    return ((_bmap & (1<<ch))!=0);
  }
  static void build();
  static TofTdcCor tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
};
//---------------
class TofTdcCorMS{
//To keep TofTdc Integral nonlinearity correction and other data
private:
  int _bmap;//bit-map of abs.ch# presented in calib.data ( lsb -> 1st ch.)
  geant _icor[TOF2GC::SCTDCCH-2][1024];//integr.nonlin. corrections, based on 10 lsb of TDC-count(bin#)
public:
  TofTdcCorMS(){};
  TofTdcCorMS(int bmap, geant icor[TOF2GC::SCTDCCH-2][1024]):_bmap(bmap){
    for(int ch=0;ch<TOF2GC::SCTDCCH-2;ch++){
      for(int bn=0;bn<1024;bn++)_icor[ch][bn]=icor[ch][bn];
    }
  };
  int getbin(number htim, number ttim, int ch);//ch=0,1,...
  geant getinval(int ch, int bn){return _icor[ch][bn];}
//
  bool truech(int ch){
#ifdef __AMSDEBUG__
    if(TOF2DBc::debug){
      assert(ch>=0 && ch < TOF2GC::SCTDCCH-2);
    }
#endif
    return ((_bmap & (1<<ch))!=0);
  }
  static void build();
  static int getbins(number htim, number ttim);//no lin-corr, suitable for ACC also !!!
  static TofTdcCorMS tdccor[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];
};
//-------------------------------------------------
#endif

