//  $Id: tofdbc02.h,v 1.21 2005/03/11 11:16:28 choumilo Exp $
// Author E.Choumilov 13.06.96.
//
// Last edit : Jan 21, 1997 ak. !!!! put back friend class TOFDBcD
// Last edit : june 27, 1997 EC, add some DAQ-constants
// Add TOF-charge classes, E.Choumilov 19.01.2005
//
#ifndef __TOF2DBC__
#define __TOF2DBC__
#include <typedefs.h>
#include <cern.h>
#include <commons.h>
#include <amsdbc.h>
#include <extC.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <job.h>
// #include <daqblock.h>
//
//
//some general(tof version-independent) constants
namespace TOFGC{
  const integer AMSDISL=500;//max length of distributions in AMSDistr class
  const integer SCWORM=256;// Max.length (in 16-bits words) in bit-stream class
  const integer SCBITM=SCWORM*16;// same in bits(-> 2.048 mks with 0.5 ns binning)
  const integer SCBADB1=128; // status bit to mark counter with bad "history"
  const integer SCBADB2=256; // status bit to mark counter with only one_side measurement
  const integer SCBADB3=512; // ... bad for t-meas.based on DB(don't use for tzcalibr/beta-meas)
  const integer SCBADB4=1024; // missing side number(s1->no_bit,s2->set_bit, IF B2 is set !)
  const integer SCBADB5=2048; // set if missing side was recovered (when B2 is set)
  const integer SCBADB6=4096; // set if no aTDC/sTDC matching on any alive side
  const integer SCBADB7=8192; // set if no dTDC/sTDC matching .................
}
//-----
// TOF2 global constants definition
namespace TOF2GC{
//geometry :
const integer SCMXBR=10; // max nmb of bars/layer
const integer SCLRS=4; // max nmb of layers in TOF-systems
const integer SCROTN=2; // start nmb of abs. numbering of TOF rot. matrixes
const integer SCBTPN=11; //Real nmb of sc. bar types (different by length now)
const integer SCCHMX=SCLRS*SCMXBR*2; //MAX scint. channels
const integer SCBLMX=SCLRS*SCMXBR;   //MAX scint. bars*layers
const integer PMTSMX=3; // MAX number of PMTs per side
//MC
const integer TOFPNMX=1000;// max integral-bins  in TOFTpoints class
const integer SCANPNT=15; //max scan points in MC/REC t/a-calibration
const integer SCANTDL=400;//scan time distribution max.length(should be SCANTDL<AMSDISL) 
const integer SCANWDS=4; //max. width-divisions in the sc.paddle scan
const integer SCTBMX=10000;//length of MC "flash-ADC" buffer(-> 1mks with 0.1ns binning)
// 
//DAQ
const int16u SCPHBP=16384; // phase bit position in Reduced-format TDC word (15th bit)
const int16u SCPHBPA=32768;// phase bit position in Raw-format address word (16th bit)
const int16u SCADCMX=4095;// MAX. value in ADC (12bits-1)

const integer SCCRAT=4; // number of crates with TOF(+ANTI)-channels (S-crates)
const integer SCSLTM=9;// number of slots(all types) per crate(max)
const integer SCSLTY=6;// slot(card) types(real:sdr,spt,sfet,sfea1,sfea2,sfec) 
const integer SCRCHM=40;// number of readout channels (outputs) per slot(card)(max)
const integer SCRCMX=SCCRAT*SCSLTM*SCRCHM;//total readout-channels(max)
const integer SCPMOU=4;//  number pm-outputs per side (max)(anode+3dynodes)
const integer SCMTYP=4;// number of measurement types(max)(fTDC,sTDC,amplH,amplL)
const integer SCAMTS=4;// number of Anode measurements types(actual)(t,h,qh,ql)
const integer SCDMTS=2;// number of Dynode measurements types(actual)(dh,ql)

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
const integer SCTHMX1=8;//max trigger hits( multpl.factor=2 for trig/ftdc, =4 for stdc)
const integer SCTHMX2=8;//max fast TDC (history) hits  
const integer SCTHMX3=4;//max slow TDC (stretcher) hits
const integer SCTHMX4=1;//max adca(anode)/adcd/adcdl hits  
const integer SCJSTA=35;   //size of Job-statistics array
const integer SCCSTA=25;   //size of Channel-statistics array
const integer SCPROFP=6;//max. parameters/side in A-profile(Apm<->Yloc) fit
const integer SCPDFBM=100;//max bins in TOF-eloss Prob Density Functions(need additional 2 for ovfls)
//
//      Calibration:
// TDIF
const integer SCTDBM=17; //max. number of coord. bins for TDIF-calibration
// AMPL 
const integer SCACMX=2000;// max. number of accum. events per channel(or bin)
const integer SCPRBM=17;//max.bins for Ampl-profile along the counter
const integer SCBTBN=SCBTPN*SCPRBM;// max. bar_types(i.e.ref.bars) * max.bins
const integer SCELFT=4;     // max. number of param. for dE/dX fit
const integer SCMCIEVM=2000;//max. events for MC A-integrator calibration 
// AVSD
const integer SCACHB=500;//max.bins in A(h) for "D(h) vs A(h)" fit
const number SCACBW=10.;// bin width in A(h) binning (adc-channels)
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
  static geant _lglen[TOF2GC::SCBTPN]; // eff.light-guide lengthes for SCBTPN types
  static geant _outcp[TOF2GC::SCLRS][4]; // outer counters param(wid/xc/yc/lgw) vs layer
  static geant _supstr[12]; // supporting structure parameters
  static geant _plnstr[20]; // sc. plane structure parameters
// MC/RECO constants:
  static geant _edep2ph;     // MC edep(Mev)-to-Photons convertion
  static geant _seresp;      // PMT single elec.responce (mV on 50 Ohm, MP=MEAN)
  static geant _seresv;      // ........................ variations (relat to MP)
  static geant _adc2q;       // not used (now taken from TFCA card #21) 
  static geant _fladctb;     // MC flash-ADC time-binning (ns)
  static geant _shaptb;      // not used
  static geant _shrtim;      // not used
  static geant _shftim;      // not used
  static geant _shctim;      // not used
  static geant _tdcbin[4];   // TDC binning for ftdc/stdc, supl.DAQ binning for ADC
  static geant _trigtb;      // MC time-binning in logic(trig) pulse handling
  static geant _strflu;      // Stretcher "end-mark" time fluctuations (ns)
  static geant _daqpwd[15];  // MC DAQ-system pulse_widths/dead_times/...
  static geant _di2anr;      // not used
  static geant _strrat;      // not used
  static geant _strjit1;     // "start" signal jitter at stretcher input(ns) 
  static geant _strjit2;     // "stop"(FT) signal jitter at stretcher input(ns)
  static geant _ftdelf;      // FastTrigger (FT) fixed (by h/w) delay (ns)
  static geant _ftdelm;      // FT max delay (allowed by stretcher logic) (ns)
  static geant _accdel;      // "Lev-1"(Common stop) signal delay wrt FT (ns)
  static geant _fstdcd;      // Same hit(up-edge) relative delay of slow- wrt hist-TDC
  static geant _fatdcd;      // not used
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
//          i=3 => Out-of-volume hit
//          i=4 => Fired TOF-bars
//          i=5 => MC flash-ADC overflows
//          i=9 => MC fTDC stack overflows
//          i=6 => MC stretch-TDC overflows
//          i=7 => MC anodeH-ADC overflows
//          i=8 => MC dynodeH-ADC overflows
//          i=10=> MC dynodeL-ADC overflows
//          i=11=> MC GHitT out of FADC
//          i=12=> MC GHitT total
//          i=13=> MC anodeL-ADC overflows
//          i=14=> OK in MCgen fast selection
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
//                              [0] -> RawEvent channel entries  
//                              [1] -> "FTDC-ON"  (Nftdc>0)
//                              [2] -> "STDC-ON"  (Nstdc>0)
//                              [3] -> "ADCA-ON"  (Nadca>0)
//                             [18] -> "ADCAL-ON"  (Nadcal>0)
//                              [4] -> "ADCD-ON"  (Nadcd>0)
//                              [5] -> "ADCDL-ON"  (Nadcdl>0)
//                              [6] -> "FTDC-1hit"  (Nftdc=1)
//                              [7] -> "STDC-1hit"  (Nstdc=1)
//                              [8] -> "(F&S&A)-ON "
//                              [9] -> "AnodeH-adc overflow "
//                             [19] -> "AnodeL-adc overflow "
//                              [10] -> "DynodeH-adc overflow "
//                              [11] -> "DynodeL-adc overflow "
//
//                             [12] -> "Validate entries/channel"
//                             [13] -> "bad up/down sequence in hist-TDC"
//                             [14] -> "bad up/down sequence in slow-TDC"
//                             [15] -> "problems in AnodeADC(h)"
//                             [20] -> "problems in AnodeADC(l)"
//                             [16] -> "problems in DynodeADC(h)"
//                             [17] -> "problems in DynodeADC(l)"
//------
  static integer brcount[TOF2GC::SCBLMX][TOF2GC::SCCSTA];// bar statistics
//                               [0] -> RawEvent Bar(at least 1side) entries
//                               [1] -> "multipl-OK"
//                               [2] -> "history-OK"
//                               [3] -> "2-sided history-OK"
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
  integer npmts;   // Npmts per side
  integer status[2]; //2-sides cal.status F|S|Ah|Al|Dh|Dl -> Anode(FastTDC/SlowTDC/
//                      AnodeH/AnodeL/DynodeH/DynodeL ADC channels, =0/1->OK/Bad)
  geant gaina[2]; // Ah-gain(PMT mainly)(S1/2; relative to some ref. bar of given type)
  geant gaind[2][TOF2GC::PMTSMX];//Dh rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // Ah to Dh(equilized sum) ratio (in unsatur. region)
  geant asatl;  // anode_chain saturation limit(mev)(incl. PM,Shaper,...)
//                  (i.e. below use anode data, above  - dinode data)
  geant tthr;   // Time-discr. threshold (mV)
  geant strat[2][2];  // Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant fstrd;  // same hit time difference between fast/slow TDC (ns)
  geant tzero;  // T0 (ns)
  geant slope; // slope for T vs (1/sqrt(Qa1)+1/sqrt(Qa2))
  geant slops[2];// indiv.slopes for Ts vs 1/Qs
  geant yctdif;// two ends time difference at counter center (ns)
  geant td2pos[2];// t_diff->position conv. factors(=Vlight,cm/ns) and coo-error(cm))
// for (at least) reference bar (in each bar type group) :
  geant mip2q;// 2-sides A-signal(pC/Mev) (at long(Y) coo=0(center))
  integer nscanp;// real number of scant points(long wdiv)
  geant yscanp[TOF2GC::SCANPNT]; // Y-positions of the scan points(bar type depend)
  geant relout[TOF2GC::SCANPNT]; // Relative(to Y=0) 2-Sides(sum) Light output
//(if some PMTs in some bar are dead, curve should be MC-calc. or measured)  
//
  geant ah2lr[2]; // Anode high/low channel ratio (side-1,2)
  geant dh2lr[2][TOF2GC::PMTSMX]; //  Dh(pm) to Dl(pm) gains ratio (side-1,2)
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr or dh2lr factors)
//
  geant unipar[2*TOF2GC::SCPROFP];// responce uniformity parameters (for ref.bars)
//
//
public:
  static TOF2Brcal scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOF2Brcal(){};
  TOF2Brcal(integer sid, integer npm, integer sta[2], geant gna[2],
           geant gnd[2][TOF2GC::PMTSMX], 
           geant a2d[2], geant asl, geant tth, geant stra[2][2], geant fstd, 
           geant t0, geant sl, geant sls[2], geant tdiff, geant td2p[2],
           geant mip, integer nscp, geant ysc[], geant relo[], geant upar[], 
           geant ah2l[2], geant dh2l[2][TOF2GC::PMTSMX], geant a2q);
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
// status(is)=F*1000000000+S*100000000+Ah*10000000+Al*1000000+Dh(nml*1000)+Dl(kji)
//                              l/m/n=1->PmHch1/2/3 bad;i/j/k=1->PmLch1/2/3 bad
  bool FchOK(int isd){
    return(status[isd]/1000000000==0);// means good FastTDC chan(anode)
  }
  bool SchOK(int isd){
    return((status[isd]%1000000000)/100000000==0);// means good SlowTDC chan(anode)
  }
  bool AHchOK(int isd){
    return((status[isd]%100000000)/10000000==0);// means good Anode Hi-gain chan
  }
  bool ALchOK(int isd){
    return((status[isd]%10000000)/1000000==0);// means good Anode Low-gain chan
  }
  bool DHchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000000)/1000;
    if(ipm==0)return(nml%10==0);// means good Dynode Hi-gain ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode Hi-gain ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode Hi-gain ipm-chan(3)
    return(0);
  }
  bool DLchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode Lo-gain ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode Lo-gain ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode Lo-gain ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){return(
                     ((status[isd]/1000000000)==0 || TFREFFKEY.relogic[1]>=1)
                   && (status[isd]%1000000000)/100000000==0
		   && (status[isd]%100000000)/10000000==0
		   && (status[isd]%10000000)/1000000==0
		   && (status[isd]%1000000)/1000==0
		   && (status[isd]%1000)==0
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
  geant getdh2l(int isd, int ipm){
    return dh2lr[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  void getupar(geant upar[]){
    for(int i=0;i<2*TOF2GC::SCPROFP;i++){upar[i]=unipar[i];}
  }
  void getah2lr(geant hlr[2]){
    for(int i=0;i<2;i++){hlr[i]=ah2lr[i];}
  }
  
  void q2a2q(int cof, int sdf,int hlf, number &adc, number &q); //Q(pC)<->ADC(chan)
  geant adc2mip(int hlf, number am[2]); // A-measured s1+s2 ampl.(ADCch) -> (mev)
  geant adcd2mip(int hlf, number am[2][TOF2GC::PMTSMX]); // D-measured s1+s2 ampl.(ADCch) -> (mev)
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
  geant gaind[2][TOF2GC::PMTSMX];//Dh(pm) rel.gain(S1/2; for each PMT wrt aver. side-signal)       
  geant a2dr[2];  // anode-to-dynode(high-gain chan) signal ratio (in unsatur. region)
  geant strat[2][2];  // Stretcher param.[side 1/2][par.1(ratio)/2(offs)]
  geant ah2lr[2]; // Anode high/low channel ratio (side-1,2)
  geant dh2lr[2][TOF2GC::PMTSMX];// Dh(pm) to Dl(pm) gain ratio (side-1,2)
  geant adc2qf; // Average(tempor) Anode ADCch->Q conv.factor(pC/ch)
//  (for dynodes this factor is not required because included into a2dr or dh2lr factors)
//
//
public:
  static TOFBrcalMS scbrcal[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBrcalMS(){};
  TOFBrcalMS(integer sid, integer sta[2], geant gna[2], 
           geant gnd[2][TOF2GC::PMTSMX],
           geant a2d[2], geant stra[2][2], geant ah2l[2], 
	   geant dh2l[2][TOF2GC::PMTSMX], geant a2q);
//
  void getbstat(int sta[]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
//
// status(is)=F*1000000000+S*100000000+Ah*10000000+Al*1000000+Dh(nml*1000)+Dl(kji)
//                              l/m/n=1->PmHch1/2/3 bad;i/j/k=1->PmLch1/2/3 bad
  bool FchOK(int isd){
    return(status[isd]/1000000000==0);// means good FastTDC chan(anode)
  }
  bool SchOK(int isd){
    return((status[isd]%1000000000)/100000000==0);// means good SlowTDC chan(anode)
  }
  bool AHchOK(int isd){
    return((status[isd]%100000000)/10000000==0);// means good Anode Hi-gain chan
  }
  bool ALchOK(int isd){
    return((status[isd]%10000000)/1000000==0);// means good Anode Low-gain chan
  }
  bool DHchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000000)/1000;
    if(ipm==0)return(nml%10==0);// means good Dynode Hi-gain ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode Hi-gain ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode Hi-gain ipm-chan(3)
    return(0);
  }
  bool DLchOK(int isd, int ipm){//ipm=0-2
    int nml=(status[isd]%1000);
    if(ipm==0)return(nml%10==0);// means good Dynode Lo-gain ipm-chan(1)
    if(ipm==1)return((nml%100)/10==0);// means good Dynode Lo-gain ipm-chan(2)
    if(ipm==2)return(nml/1000==0);// means good Dynode Lo-gain ipm-chan(3)
    return(0);
  }
  bool SideOK(int isd){return(
                     ((status[isd]/1000000000)==0 || TFREFFKEY.relogic[1]>=1)
                   && (status[isd]%1000000000)/100000000==0
		   && (status[isd]%100000000)/10000000==0
		   && (status[isd]%10000000)/1000000==0
		   && (status[isd]%1000000)/10000==0
		   && (status[isd]%1000)==0
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
  geant getdh2l(int isd, int ipm){
    return dh2lr[isd][ipm];
  }
  void geta2dr(geant a2d[2]){
    a2d[0]=a2dr[0];
    a2d[1]=a2dr[1];
  }
  void getah2lr(geant hlr[2]){
    for(int i=0;i<2;i++){hlr[i]=ah2lr[i];}
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
  integer stata[2];//status for side1/2 anodes(high gain ch) =0/1->ok/bad
  integer statal[2];//status for side1/2 anodes(low gain ch) =0/1->ok/bad
  integer statd[2][TOF2GC::PMTSMX];//Dh-pmts statuses( =0/1->ok/bad) 
  integer statdl[2][TOF2GC::PMTSMX];//Dl-pmts statuses( =0/1->ok/bad)
  geant peda[2]; // anode(hi) peds for side1/2
  geant siga[2]; // anode(hi) ped.sigmas .............................
  geant pedal[2]; // anode(low) peds for side1/2
  geant sigal[2]; // anode(low) ped.sigmas .............................
  geant pedd[2][TOF2GC::PMTSMX]; // Dh-pmts peds for side1/2
  geant sigd[2][TOF2GC::PMTSMX]; // Dh-pmts ped.sigmas .............................
  geant peddl[2][TOF2GC::PMTSMX]; // Dl-pmts peds for side1/2
  geant sigdl[2][TOF2GC::PMTSMX]; // Dl-pmts ped.sigmas .............................
//
public:
  static TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];
  TOFBPeds(){};
  TOFBPeds(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2],
           integer _statal[2],
           geant _pedal[2], geant _sigal[2],
           integer _statd[2][TOF2GC::PMTSMX],
           geant _pedd[2][TOF2GC::PMTSMX], geant _sigd[2][TOF2GC::PMTSMX],
	   integer _statdl[2][TOF2GC::PMTSMX],
           geant _peddl[2][TOF2GC::PMTSMX], geant _sigdl[2][TOF2GC::PMTSMX]
  ):softid(_sid){
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      statal[i]=_statal[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
      pedal[i]=_pedal[i];
      sigal[i]=_sigal[i];
      for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
        statd[i][ipm]=_statd[i][ipm];
        statdl[i][ipm]=_statdl[i][ipm];
        pedd[i][ipm]=_pedd[i][ipm];
        sigd[i][ipm]=_sigd[i][ipm];
        peddl[i][ipm]=_peddl[i][ipm];
        sigdl[i][ipm]=_sigdl[i][ipm];
      }
    }
  };
  bool PedAHchOK(int isd){return stata[isd]==0;}
  bool PedALchOK(int isd){return statal[isd]==0;}
  bool PedDHchOK(int isd, int ipm){return (statd[isd][ipm]==0);}
  bool PedDHchOK(int isd){return (statd[isd][0]==0 && statd[isd][1]==0 && statd[isd][2]==0);}
  bool PedDLchOK(int isd, int ipm){return (statdl[isd][ipm]==0);}
  bool PedDLchOK(int isd){return (statdl[isd][0]==0 && statdl[isd][1]==0 && statdl[isd][2]==0);}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  geant &apedal(int isd)  {return pedal[isd];}  
  geant &asigal(int isd)  {return sigal[isd];}
  integer &astad(int isd, int ipm)  {return statd[isd][ipm];}  
  geant &apedd(int isd, int ipm)  {return pedd[isd][ipm];}  
  geant &asigd(int isd, int ipm)  {return sigd[isd][ipm];}
  integer &astadl(int isd, int ipm)  {return statdl[isd][ipm];}  
  geant &apeddl(int isd, int ipm)  {return peddl[isd][ipm];}  
  geant &asigdl(int isd, int ipm)  {return sigdl[isd][ipm];}
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  void getpedal(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=pedal[i];
  };
  void getsigal(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=sigal[i];
  };
  
  void getpedd(int ipm, geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=pedd[i][ipm];
  };
  void getsigd(int ipm, geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=sigd[i][ipm];
  };
  
  void getpeddl(int ipm, geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peddl[i][ipm];
  };
  void getsigdl(int ipm, geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=sigdl[i][ipm];
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
  number stpx;//bin width(MeV)
  number xmin;//1st bin low edge(MeV)
  number slope;//exp.slope to calc. pdf in ovfl-region
  number unpdf;//const.level of pdf in undf-region 
  number elpdf[TOF2GC::SCPDFBM];//PDF-array 
public:
  static TofElosPDF TofEPDFs[AMSChargConst::MaxZTypes];
  TofElosPDF(){};
  TofElosPDF(int ich, int ch, int nb, geant stp, geant bnl, geant undf, geant ovfl, geant distr[]);
  int getnbins(){return nbins;}
  int getcharge(){return charge;}
  number getstep(){return stpx;}
  number getlkhd(int nhits, int hstat[], number ehit[], number beta);
  static void build();  
}; 
#endif

