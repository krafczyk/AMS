//  $Id: tofdbc02.h,v 1.7 2002/02/12 08:44:09 choumilo Exp $
// Author E.Choumilov 13.06.96.
//
// Last edit : Jan 21, 1997 ak. !!!! put back friend class TOFDBcD
// Last edit : june 27, 1997 EC, add some DAQ-constants
//
#ifndef __TOF2DBC__
#define __TOF2DBC__
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
// #include <daqblock.h>
//
//some general(tof version-independent) constants
namespace TOFGC{
  const integer AMSDISL=500;//max length of distributions in AMSDistr class
  const integer SCWORM=128;// Max.length (in 16-bits words) in bit-stream class
  const integer SCBITM=SCWORM*16;// same in bits
  const integer SCBADB1=128; // status bit to mark counter with bad "history"
  const integer SCBADB2=256; // status bit to mark counter with only one_side measurement
  const integer SCBADB3=512; // ...  with bad t-meas.(don't use for tzcalibr/beta-meas)
  const integer SCBADB4=1024; // missing side number(s1->n0_bit,s2->set_bit, IF B2 is set !)
  const integer SCBADB5=2048; // set if missing side was recovered (when B2 is set)
  const integer SCBADB6=4096; // set if no aTDC/sTDC matching on any alive side
  const integer SCBADB7=8192; // set if no dTDC/sTDC matching .................
  const integer MAXPLN=4;//  max.planes (max of ams01 and ams02)
  const integer MAXPAD=14;// max.paddles/plane (max of ams01 and ams02)
}
//-----
// TOF2 global constants definition
namespace TOF2GC{
//geometry :
const integer SCMXBR=12; // max nmb of bars/layer
const integer SCLRS=4; // max nmb of layers in TOF-systems
//const integer SCBRS[SCLRS]={12,12,12,12}; // nmb of bars per layer (top->bot)
const integer SCROTN=2; // start nmb of abs. numbering of TOF rot. matrixes
const integer SCBTPN=10;    //nmb of sc. bar types (different by length now)
const integer SCCHMX=SCLRS*SCMXBR*2; //MAX scint. channels
const integer SCBLMX=SCLRS*SCMXBR;   //MAX scint. bars*layers
//MC
const integer TOFPNMX=1000;// max integral-bins  in TOFTpoints class
const integer SCANPNT=20; //max scan points in MC/REC t/a-calibration
const integer SCANTDL=400;//scan time distribution max.length(should be SCANTDL<AMSDISL) 
const integer SCTBMX=5000;//length of MC "flash-ADC" buffer
const integer SCSBMX=1200;//length of MC "shaper" buffer
//                       (should be above SCTBMX*fadcb/shaptb+6.21*tfall/shaptb) 
//DAQ
const int16u SCPHBP=16384; // phase bit position in Reduced-format TDC word (15th bit)
const int16u SCPHBPA=32768;// phase bit position in Raw-format address word (16th bit)
const int16u SCADCMX=4095;// MAX. value in ADC (12bits-1)
const integer SCCRAT=8; // number of crates with TOF(+CTC/ANTI)-channels (S-crates)
const integer SCSFET=4; // SFETs per crate
const integer SCTOFC=4; // max. TOF-channels (each =4 TDC-channels) per SFET
const integer SCTDCC=SCTOFC*4; // max. TDC-channels per SFET (4 tdcchan/tofch)
const integer DAQSBLK=8;// total S-crates(blocks) in DAQ
const integer DAQSSLT=6;// max. available slots in S-crate (max. SFEx cards)
const integer DAQSFMX=2;// number of format types
const integer DAQSTSC=1;// number of slots with Temp-info per crate
const integer DAQSTCS=4;// number of temp-channel(tdcc) per slot(if present)
const integer DAQSTMX=DAQSBLK*DAQSTSC*DAQSTCS;// max. temp. channels
//RECO
const integer SCTHMX1=8;//max trigger hits( multpl.factor=2 for tr/ftdc/adc, =4 for stdc)
const integer SCTHMX2=8;//max fast TDC (history) hits  
const integer SCTHMX3=4;//max slow TDC (stretcher) hits
const integer SCTHMX4=8;//max adc TDC (time_over_threshold) hits  
const integer SCJSTA=35;   //size of Job-statistics array
const integer SCCSTA=16;   //size of Channel-statistics array
const integer SCIPAR=3;    // number of a(d)-integrator parameters
const integer SCPROFP=5;//max. parameters in A-profile fit
//
//      Calibration:
// TDIF
const integer SCTDBM=17; //max. number of coord. bins for TDIF-calibration
// AMPL 
const integer SCACMX=2000;// max. number of accum. events per channel(or bin)
const integer SCPRBM=17;//max.bins for Ampl-profile along the counter
const integer SCBTBN=SCBTPN*SCPRBM;// max. bar_types(i.e.ref.bars) * max.bins
const integer SCELFT=5;     // max. number of param. for dE/dX fit
const integer SCMCIEVM=2000;//max. events for MC A-integrator calibration 
// AVSD
const integer SCACHB=500;//max.bins in adch for "adcl vs adch" fit
const number SCACBW=10.;// bin width in adch binning (adc-channels)
// STRR
const integer SCSRCHB=1200;// max. bin for dtout (2000-6000ns)
const integer SCSRCLB=400;// min. bin for dtout
const integer SCSRCHBMC=1200;// max. bin for dtout(MC)(2000-6000ns)
const integer SCSRCLBMC=400;// min. bin for dtout (MC)
const number SCSRCTB=5.;// time binning for dtout (ns)
}
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
  static geant _lglen[TOF2GC::SCBTPN]; // light-guide lengthes for SCBTPN types
  static geant _supstr[10]; // supp. structure parameters
  static geant _plnstr[15]; // sc. plane structure param.
// MC/RECO constants:
  static geant _edep2ph;     // MC edep(Mev)-to-Photons convertion
  static geant _seresp;      // PMT single elec.responce (mV on 50 Ohm, MP=MEAN)
  static geant _seresv;      // ........................ variations (relat to MP)
  static geant _adc2q;       // generic ADCch->Q conv. factor (pC/chan) 
  static geant _fladctb;     // MC flash-ADC time-binning (ns)
  static geant _shaptb;      // MC shaper pulse time-binning
  static geant _shrtim;      // MC shaper pulse rise-time (ns)
  static geant _shftim;      // MC shaper pulse fall-time (ns)
  static geant _shctim;      // MC shaper pulse cut-time (gate width)(ns)
  static geant _tdcbin[4];   // TDC binning for ftdc/stdc, supl.DAQ binning for ADC
  static geant _trigtb;      // MC time-binning in logic(trig) pulse handling
  static geant _strflu;      // Stretcher "end-mark" time fluctuations (ns)
  static geant _daqpwd[15];  // MC DAQ-system pulse_widths/dead_times/...
  static geant _di2anr;      // default(MC-data) dinode-to-anode signal ratio
  static geant _strrat;      // default(-------) stretcher ratio
  static geant _strjit1;     // "start" signal jitter at stretcher input(ns) 
  static geant _strjit2;     // "stop"(FT) signal jitter at stretcher input(ns)
  static geant _ftdelf;      // FastTrigger (FT) fixed (by h/w) delay (ns)
  static geant _ftdelm;      // FT max delay (allowed by stretcher logic) (ns)
  static geant _accdel;      // Lev-1 signal delay with respect to FT (ns)
  static geant _fstdcd;      // Same hit(up-edge) relative delay of slow- wrt hist-TDC
  static geant _fatdcd;      // Same hit(up-edge) rel.delay of A(d)- wrt hist-TDC
  static integer _pbonup;    // set phase bit "on" for leading(up) edge (yes/no->1/0)
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
  static geant supstr(integer i);
//
  static geant plnstr(integer i);
//   function to get Z-position of scint. bar=ib in layer=il
  static geant getzsc(integer il, integer ib);
// function to get transv. position of scint. bar=ib in layer=il
  static geant gettsc(integer il, integer ib);
// functions to get MC/RECO parameters:
//
  static geant edep2ph();
  static geant fladctb();
  static geant shaptb();
  static geant shrtim();
  static geant shftim();
  static geant shctim();
//
  static geant accdel();
  static geant ftdelf();
  static geant ftdelm();
  static geant fstdcd();
  static geant fatdcd();
  static geant seresp();
  static geant seresv();
  static geant adc2q();
  static integer pbonup();
//
  static geant strrat();
//
  static geant strjit1();
//
  static geant strjit2();
//
  static geant strflu();
//
  static geant di2anr();
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
// --------------> Class for general "time-Variable"  parameters :
class TOF2Varp {  
//
private:
//
class TOF2Temperature{
 public:
  uinteger tofav[8];
  uinteger cr01[4];
  uinteger cr31[4];
  uinteger cr41[4];
  uinteger cr71[4];
  uinteger cr03[2];
  uinteger cr33[3];
  uinteger cr43[3];
  uinteger cr73[3];
};
// ---> TOF DAQ-system thresholds :
  geant _daqthr[5];   // DAQ-system thresholds (defaults)
// ---> Cuts :
  geant _cuts[10];                    
          //  (0) fstdw   -> t-window to identify same hit in fast/slow_tdc
          //  (1) hiscutb -> "befor"-cut in time history (ns)
          //  (2) hiscuta -> "after"-cut in time history (ns)
          //  (3) lcoerr  -> "err. in longit.coord. measurement
          //  (4) ftdelf  -> FT fixed delay
          //  (5) sftdcd  -> s/f-TDC delay (s- wrt f-TDC)
          //  (6) eclass  -> assim.cut for TOFCluster energy calc.
          //  (7) eclmat  -> internal long.coo matching cut ....
          //  (8) satdcg  -> 
          //  (9) sdtdcg  -> 
   static TOF2Temperature  tftt;
//
public:
  static TOF2Varp tofvpar;
  TOF2Varp(){};
// member functions :
//
  static TOF2Temperature * gettoftp(){return &tftt;}
  static integer gettoftsize(){return sizeof(tftt);}

  static geant getmeantoftemp(int crate);
  void init(geant daqthr[5], geant cuts[10]);
//
  geant fstdw(){return _cuts[0];}
  geant hiscutb(){return _cuts[1];}
  geant hiscuta(){return _cuts[2];}
  geant lcoerr(){return _cuts[3];}
  geant ftdelf(){return _cuts[4];}
  geant sftdcd(){return _cuts[5];}
  geant eclass(){return _cuts[6];}
  geant eclmat(){return _cuts[7];}
  geant satdcg(){return _cuts[8];}
  geant sdtdcg(){return _cuts[9];}
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
//           21-25 TOF-user event-counters
//          =33 -> TOF reco with TOF in LVL1
//          =34 -> TOF reco with EC in LVL1
//------
  static integer chcount[TOF2GC::SCCHMX][TOF2GC::SCCSTA];//channel statistics
//                              [0] -> h/w-status="ON" frequency 
//                              [1] -> "FTDC-ON"  (Nftdc>0)
//                              [2] -> "STDC-ON"  (Nstdc>0)
//                              [3] -> "ADCA-ON"  (Nadca>0)
//                              [4] -> "ADCD-ON"  (Nadcd>0)
//                              [5] -> "FTDC-1hit"  (Nftdc=1)
//                              [6] -> "STDC-1hit"  (Nstdc=1)
//                              [7] -> "ADCA-1hit"  (Nadca=1)
//                              [8] -> "ADCD-1hit"  (Nadcd=1)
//                              [9] -> "3-measurements"
//
//                             [11] -> "h/w-ok frequency (validate)"
//                             [12] -> "bad up/down sequence in hist-TDC"
//                             [13] -> "bad up/down sequence in slow-TDC"
//                             [14] -> "bad up/down sequence in A-TDC"
//                             [15] -> "bad up/down sequence in D-TDC"
//------
  static integer brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];// bar statistics
//                               [0] -> h/w-status="ON" frequency
//                               [1] -> "multipl-OK"
//                               [2] -> "history-OK"
//                               [3] -> "2-sided history-OK"
//                               [4] -> "a/s-TDC matching OK"
//                               [5] -> "d/s-TDC matching OK"
//------
  static integer scdaqbc1[TOF2GC::SCCRAT][2];// scDAQ-block appearence frequecy  (for fmt=0/1->raw/red)
  static integer scdaqbc2[TOF2GC::SCCRAT][2];// scDAQ-block appearence frequecy(non-empty)
  static integer scdaqbc3[TOF2GC::SCCRAT][2];// scDAQ-block "length mismatch" appear.frequency
  static integer scdaqbc4[TOF2GC::SCCRAT][2];// scDAQ-block "cr/sf/tofc in contradiction with map" appear.frequency
//
public:
  static void clear();
  inline static void addmc(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< TOF2GC::SCJSTA);
    recount[i]+=1;
  }
  inline static void addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < TOF2GC::SCCHMX && i < TOF2GC::SCCSTA);
    chcount[chnum][i]+=1;
  }
  inline static void addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < TOF2GC::SCBLMX && i < TOF2GC::SCCSTA);
    brcount[brnum][i]+=1;
  }
  inline static void addaq1(int16u blnum, int16u fmt){
    assert(blnum<TOF2GC::SCCRAT && fmt<2);
    scdaqbc1[blnum][fmt]+=1;
  }
  inline static void addaq2(int16u blnum, int16u fmt){
    assert(blnum<TOF2GC::SCCRAT && fmt<2);
    scdaqbc2[blnum][fmt]+=1;
  }
  inline static void addaq3(int16u blnum, int16u fmt){
    assert(blnum<TOF2GC::SCCRAT && fmt<2);
    scdaqbc3[blnum][fmt]+=1;
  }
  inline static void addaq4(int16u blnum, int16u fmt){
    assert(blnum<TOF2GC::SCCRAT && fmt<2);
    scdaqbc4[blnum][fmt]+=1;
  }
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
  integer softid;  // LBB
  integer status[2]; // 2-sides status,each number as LR(L->PM1,R->PM2)(!=0/0->problems/ok)
  geant gaina[2]; // A-chain gain(PMT mainly)| (side-1/2; relative to some 
  geant gaind[2]; // D-chain gain(PMT mainly)|       reference bar of given type)
  geant asatl;  // anode_chain saturation limit(mev)(incl. PM,Shaper,...)
//                  (i.e. below use anode data, above  - dinode data)
  geant tthr;   // Time-discr. threshold (mV)
  geant strat[2][2];  // Stratcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant fstrd;  // same hit time difference between fast/slow TDC (ns)
  geant tzero;  // T0 (ns)
  geant slope; // slope for T vs (1/sqrt(Qa1)+1/sqrt(Qa2))
  geant slops[2];// indiv.slopes for Ts vs 1/Qs
  geant yctdif;// two ends time difference at counter center (ns)
  geant td2pos[2];// t_diff->position conv. factors(=Vlight,cm/ns) and coo-error(cm))
// for (at least) reference bar (in each bar type group) :
  geant mip2q;// 2-sides A-signal(pC/Mev) (at Y=0.)
  integer nscanp;// real number of scant points
  geant yscanp[TOF2GC::SCANPNT]; // Y-positions of the scan points(bar type depend)
  geant relout[TOF2GC::SCANPNT]; // Relative(to Y=0) 2-Sides(sum) Light output
//(if some PMTs in some bar are dead, curve should be MC-calc. or measured)  
//
  geant ahlcr[2]; // Amplitude high/low channel ratio (side-1,2)
  geant adccf; // ADCch->Q generic conv.factor(pC/ch)
//
  geant unipar[5];// responce uniformity parameters (along the ref.bars)
//
//
public:
  static TOF2Brcal scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOF2Brcal(){};
  TOF2Brcal(integer sid, integer sta[2], geant gna[2], geant gnd[2],
           geant asl, geant tth, geant stra[2][2], geant fstd, 
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2],
           geant mip, integer nscp, geant ysc[], geant relo[], geant upar[5], 
           geant ahlr[2],geant acf);
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
  void gtstrat(geant str[2][2]){
    str[0][0]=strat[0][0];
    str[0][1]=strat[0][1];
    str[1][0]=strat[1][0];
    str[1][1]=strat[1][1];
  }
  geant gtfstrd(){return fstrd;}
  geant getasatl(){return asatl;}
  geant gettthr(){return tthr;}
  geant gettdif(){return yctdif;}
  geant getslope(){return slope;}
  geant gettzero(){return tzero;}
  geant getadccf(){return adccf;}
  void getslops(geant sls[2]){
    sls[0]=slops[0];
    sls[1]=slops[1];
  }
  void getgaina(geant gn[2]){
    gn[0]=gaina[0];
    gn[1]=gaina[1];
  }
  void getgaind(geant gn[2]){
    gn[0]=gaind[0];
    gn[1]=gaind[1];
  }
  void getupar(geant upar[5]){
    for(int i=0;i<5;i++){upar[i]=unipar[i];}
  }
  void getahlr(geant hlr[2]){
    for(int i=0;i<2;i++){hlr[i]=ahlcr[i];}
  }
  void q2t2q(int cof, int sdf,int adf, number &t, number &q); //Q(pC)<->ADC(chan)
  geant adc2mip(int hlf, number am[2]); // measured ampl.(ADCch) -> (mev)
  void  adc2q(int hlf, number am[2],number qs[2]);// meas. ampl.(ADCch)->Q(pC)
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
};
//===================================================================
// class to store TOF_bar peds/sigmas  :
class TOFBPeds{
//
private:
  integer softid;  // LBB
  geant peds[4]; // peds for side1(high/low)/side2(high/low)
  geant sigs[4]; // ped.sigmas .............................
//
public:
  static TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBPeds(){};
  TOFBPeds(integer _sid, geant _peds[4], geant _sigs[4]):softid(_sid){
    for(int i=0;i<4;i++){
      peds[i]=_peds[i];
      sigs[i]=_sigs[i];
    }
  };
  void getped(geant _peds[4]){
    for(int i=0;i<4;i++)_peds[i]=peds[i];
  };
  void getsig(geant _sigs[4]){
    for(int i=0;i<4;i++)_sigs[i]=sigs[i];
  };
  static void build();
};
#endif

