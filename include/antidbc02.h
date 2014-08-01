//  $Id: antidbc02.h,v 1.17 2010/12/11 18:30:38 choutko Exp $
// Author E.Choumilov 2.07.97
//
#ifndef __ANTI2DBC__
#define __ANTI2DBC__
//
#include "typedefs.h"
#include "tofanticonst.h"
//
class ANTI2DBc {  
//
private:
// geom. constants:
  static geant _scradi;   // internal radious of ANTI sc. cylinder 
  static geant _scinth;   // thickness of scintillator
  static geant _scleng;   // scintillator paddle length (glob. Z-dim)
  static geant _wrapth;   // wrapper thickness 
  static geant _groovr;   // groove radious (corresponds to bump on neigb. paddle)
  static geant _pdlgap;   // inter paddle gap 
  static geant _stradi;   // support tube radious (internal)
  static geant _stleng;   //              length
  static geant _stthic;   //              thickness
//MC/RECO const:
  static geant _fadcbw;   //  MC:Flash-ADC bin width
  static geant _htdcbw;   //  bin width (ns) for LT/FT-TDC.
  static integer _daqscf; //  DAQ charge-readout scaling factor 
  static geant _ftcoinw;  // not used
  static geant _dopwmn;   //  MC: LT-discr. output pulse min.duration 
  static geant _dipwmn;   //  MC: LT-discr. input pulse min.duration 
  static geant _tgpwid;   //  "trig.pattern" pulse fixed width(ns)
  static geant _tgdtim;   //  "trig.pattern" branch dead time(ns)
  static geant _didtim;   //  input dead time of generic discr(min dist. of prev_down/next_up edges)[ns]
 //                        (i neglect by fall-time of generic discr, so DT=(11+5)ns for output signals !!!)
  static geant _tdcdtm;   //  dead time of TDC-inputs, the same for LT-/FT-inputs[ns]
  static geant _daccur;   //  generic discr. internal accuracy[ns]
  static geant _pbgate;   //  TgBox FT-gate width to latch "pattern" pulses
  static geant _mev2pe;   // Average (per/pad/side) mev->pe conv.factor[pe/mev], mev->0.5Mip/side 
  static geant _perrms;   // Average relative(wrt m.p) 1pe-signal rms 
  static geant _pc2adc;   // ADC-chip pC->adc_ch conv.factor[adc/pC]
  static geant _t2qgainr; //empirical factor to normalize amplitude(mV) in T-channel 
public:  
//
//  Member functions :
//
  static geant scradi();
  static geant scinth();
  static geant scleng();
  static geant wrapth();
  static geant groovr();
  static geant pdlgap();
  static geant stradi();
  static geant stleng();
  static geant stthic();
  static void setgeom();
//
  static geant fadcbw();
  static geant htdcbw();
  static integer daqscf();
  static geant ftcoinw();
  static geant dopwmn();
  static geant dipwmn();
  static geant tgpwid();   
  static geant tgdtim();
  static geant didtim();
  static geant tdcdtm();
  static geant daccur();   
  static geant pbgate(); 
  static geant mev2pe(); 
  static geant pc2adc(); 
  static geant perrms();
  static geant t2qgainr(); 
  static geant inipulsh(int & nbn, geant arr[]);
  static void inipedisp(int & nbn, geant arr[]);
  static void displ_a(char comm[], int id, int f, const geant arr[]);
#ifdef __DB__
friend class AntiDBcD;
#endif

};
//=================================================================
// class to store individual ANTI-ReadoutPaddle Stable calib-parameters:
class ANTI2SPcal{
//
private:
  integer softid;   // readout paddle number (LogicSector:1-8)
  geant hdthr;      // Hist-channel discriminator threshold (adc-ch units)
  geant dqthr;      // DAQ-readout threshold(ADC-ch sigmas)
  geant mev2pe[4];  //Lab.measured mev2pe conv.factors (pe/mev)[2*iside+physsn]
  geant tzero[2];   // 2 PhysSector-signal delays(ns, side independ) due to fib.connection
  geant ftdel[2];   // spare
  
public:
  static ANTI2SPcal antispcal[ANTI2C::MAXANTI];
  ANTI2SPcal(){};
  ANTI2SPcal(integer sid, geant m2p[4], geant dthr, geant hthr, 
                                 geant tzer[2], geant ftd[2]):
                                          softid(sid),
                                          hdthr(hthr),dqthr(dthr){
    tzero[0]=tzer[0];
    tzero[1]=tzer[1];
    ftdel[0]=ftd[0];
    ftdel[1]=ftd[1];
    for(int i=0;i<4;i++)mev2pe[i]=m2p[i];
  }
  integer getid(){return softid;}
  geant getdqthr(){return dqthr;}
  geant gettzer(int iphc){return tzero[iphc];}
  geant gettzerc(){return 0.5*(tzero[0]+tzero[1]);}
  geant getmev2pe(int phs,int sd){ return mev2pe[2*sd+phs];}//phsect=0/1; sd=0/1
  geant getmev2pec(int sd){ return 0.5*(mev2pe[2*sd]+mev2pe[2*sd+1]);}
//
  geant gethdthr(){
    return(hdthr);
  }
  void getftd(geant ftd[2]){
    ftd[0]=ftdel[0];
    ftd[1]=ftdel[1];
  }
  static void build();
};
//=================================================================
// class to store individual ANTI-ReadoutPaddle Variable calib-parameters:
class ANTI2VPcal{
//
private:
  integer softid;   // readout paddle number (LogicSector:1-8)
  integer status[2][2];// PhysSectors status [physs][side] (PM+AMPL, 1/0->dead/alive)
  geant attlen[2][2];  // PhysSectors atten.length(cm)
  geant pe2adc[2][2];  // PhysSectors pe->adc conv.factor(adc/pe)
//    (incl.indiv. abs. PM-amplification and all deviations from LabMeasured Mev2pe's)
  geant powp[2][2];          // PhysSectors power parameter for edge-effect
  geant snorp[2][2];         // PhysSectors normaliz.param for edge-effect
    
public:
  static ANTI2VPcal antivpcal[ANTI2C::MAXANTI];
  ANTI2VPcal(){};
  ANTI2VPcal(integer sid, integer sta[2][2], geant pe2a[2][2], geant atl[2][2], 
             geant pow[2][2], geant snor[2][2]):softid(sid){
                                          
    for(int i=0;i<2;i++){
    for(int j=0;j<2;j++){
      status[i][j]=sta[i][j];
      attlen[i][j]=atl[i][j];
      pe2adc[i][j]=pe2a[i][j];
      powp[i][j]=pow[i][j];
      snorp[i][j]=snor[i][j];
    }
    }
  }
  integer getid(){return softid;}
  geant getpe2adc1(int is){return pe2adc[0][is];}
  geant getpe2adc2(int is){return pe2adc[1][is];}
  geant getpe2adcc(int is){
    if(status[0][is]==0 && status[1][is]==0)return 0.5*(pe2adc[0][is]+pe2adc[1][is]);
    else if(status[0][is]==0)return pe2adc[0][is];
    else if(status[1][is]==0)return pe2adc[1][is];
    else return -1;
  }
  geant getatlen(int iphc, int isd){return attlen[iphc][isd];}
  geant getatlenc(int isd);
  geant SignalAtt(int iphs, int isd, geant z);
  geant SignalAttc(int isd, geant z);
//
  bool CalStOK(int isd){return (status[0][isd]==0 || status[1][isd]==0);}
  integer getstat1(int isd){
    return status[0][isd];
  }
  int NPhysSecOK();
  integer getstat2(int isd){
    return status[1][isd];
  }
  static void build();
};
//=================================================================
// class to store ANTI-ReadoutPaddle peds/sigmas  :
class ANTIPeds{
//
private:
  integer softid;  // S(readout-sector number 1-8)
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
//
public:
  static ANTIPeds anscped[ANTI2C::MAXANTI];
  ANTIPeds(){};
  ANTIPeds(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2]):softid(_sid)
  {
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
    }
  };
  bool PedStOK(int isd){return stata[isd]==0;}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd){return stata[isd];}
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  static void build();
  static void mcbuild();
};
//=================================================================
// class to store ANTI-ReadoutPaddle MC-seed peds/sigmas  :
class ANTIPedsMS{
//
private:
  integer softid;  // S(readout-sector number 1-8)
  integer stata[2];//status for side1/2 anodes =0/1->ok/bad
  geant peda[2]; // anode peds for side1/2
  geant siga[2]; // anode ped.sigmas .............................
//
public:
  static ANTIPedsMS anscped[ANTI2C::MAXANTI];
  ANTIPedsMS(){};
  ANTIPedsMS(integer _sid,
           integer _stata[2],
           geant _peda[2], geant _siga[2]):softid(_sid)
  {
    for(int i=0;i<2;i++){
      stata[i]=_stata[i];
      peda[i]=_peda[i];
      siga[i]=_siga[i];
    }
  };
  bool PedStOK(int isd){return stata[isd]==0;}
  geant &apeda(int isd)  {return peda[isd];}  
  geant &asiga(int isd)  {return siga[isd];}
  integer &astaa(int isd){return stata[isd];}
  
  void getpeda(geant _ped[2]){
    for(int i=0;i<2;i++)_ped[i]=peda[i];
  };
  void getsiga(geant _sig[2]){
    for(int i=0;i<2;i++)_sig[i]=siga[i];
  };
  
  static void build();
};
//=================================================================
//
class ANTI2JobStat{
//
private:
  static integer mccount[ANTI2C::ANJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 -> Flash-adc buffer ovfl
//          i=2 -> Hist-tdc stack ovfl
//          i=3 -> ADC range ovfl
//          i=4 -> Hist-hit is out of search window(wrt FT)
//          i=5 -> FT/Anti-trig Coinc
//          i=6 => Ghits->RawEvent OK
//          i=7 => Common FT exist
//          i=8 -> trig-tdc stack ovfl
  static integer recount[ANTI2C::ANJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> Lev-1 trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5 -> Using TOF in LVL1
//           =6 -> Using EC  in LVL1
//       ....=7-10 -> reco spare
//           =11-> AntiCalib entries
//           =12-> Nsectors OK
//           =13-> TRK-track found
//           =14-> Track-sector matching OK
//           =15-> TOF/TRK Z=1 
//           =16-> good matching/impact 
//           =17-> In MIP range 
//           =18-> CrossLength OK
//           =19-> PedCalib entries 
//           =20-> PedCalib-events found 
//
//------
  static integer chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];//channel statistics
//                              [0] -> h/w-status="ON" frequency 
//                              [1] -> "ADCA-ON"  (Nadch>0)
//                              [2] -> spare
//
//                              [5] -> "h/w-ok frequency (validate)"
//                              [6] -> "No ADC hit"
//                              [7] -> spare
//                              [8] -> "bad up/down sequence in TDC"
//------
  static integer brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];// bar statistics
//                               [0] -> h/w-status="ON" frequency
//                               [1] -> "multipl-OK"
//                               [2] -> "Good for calibr"
public:
  static void clear();
  static void addmc(int i);
  static void addre(int i);
  static void addch(int chnum, int i);
  static void addbr(int brnum, int i);
  static void bookh();
  static void bookmch();
  static void printstat();
  static void outpmc();
  static void outp();
};
//===================================================================
#endif
