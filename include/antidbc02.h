//  $Id: antidbc02.h,v 1.5 2003/03/18 09:04:17 choumilo Exp $
// Author E.Choumilov 2.07.97
//
#ifndef __ANTI2DBC__
#define __ANTI2DBC__
//
#include <typedefs.h>
//
// Constants for Anti-Counters :
namespace ANTI2C{
 const integer MAXANTI=8; // number of sectors
 const integer ANCHMX=2*MAXANTI;// max. number of phys. channels 
 const integer ANAHMX=1; // max. number of anode-charge hits  per chan.
 const integer ANTHMX=8; // max. number of time-history hits (pairs of edges) per channel
 const integer ANFADC=100;//Flash-ADC channels
 const integer ANJSTA=10;// size of Job-statistics array 
 const integer ANCSTA=10;// size of Channel-statistics array
 const integer ANCRAT=4; // number of crates with SFEA cards
 const integer ANSFEA=1; // number of SFEA cards per crate
 const integer ANCHSF=16;// max. number of TDC-channels per SFEA (incl. FT-channel)
 const integer ANCHCH=4;// real number of Anti-channels per TDC-chip (impl. 2chip/SFEA)
 const integer ANSLOT=5; // SFEA card position in crate (sequential slot number(1-6))
}
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
  static geant _fadcbw;   // MC:Flash-ADC bin width
  static geant _htdcdr;   // MC:History-TDC double pulse resolution
  static geant _htdcbw;   // bin width (ns) in Hist-TDC.
  static integer _daqscf; // DAQ charge-readout scaling factor 
  static geant _ftcoinw;  // not used
  static geant _hdpdmn;   // MC: Hist-discr. min pulse duration 
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
  static geant htdcdr();
  static geant htdcbw();
  static integer daqscf();
  static geant ftcoinw();
  static geant hdpdmn();
#ifdef __DB__
friend class AntiDBcD;
#endif

};
//=================================================================
// class to store individual ANTI_paddle calibr. constants:
class ANTI2Pcal{
//
private:
  integer softid;   // paddle number (Sector:1-8)
  integer status[2];// each side  status (1/0->dead/alive)
  geant dathr;      // discriminator threshold (p.e) for hist-channel
  geant ftdel[2];   // spare
  geant mev2pe[2];  // Edep->(pe's)conv.factor(1SideSignal(pe)/0.5MIP(mev) at counter center)
//                     (incl.indiv. abs. PM-amplification and clear-fiber atten)
  geant gain[2];    // Instant gains relative to some ref.gain(eg.during beam-test calibr)
//                     (to correct for gain variations over time)
  geant adc2pe;     // Signal(adc-ch)->Signal(pe's) conv.factor (pe/ADCch)
  geant dqthr;      // DAQ-readout threshold(ADC-ch sigmas)
  geant tzero;      // Signal delay(ns, imply = for both sides) due to fib.connection
  
public:
  static ANTI2Pcal antisccal[ANTI2C::MAXANTI];
  ANTI2Pcal(){};
  ANTI2Pcal(integer sid, integer sta[2], geant athr, geant ftd[2],
                             geant m2pe[2], geant gn[2],geant ad2e,
                                            geant dthr,geant tzer):
                                          softid(sid),dathr(athr),
                              adc2pe(ad2e),dqthr(dthr),tzero(tzer){
    status[0]=sta[0];
    status[1]=sta[1];
    mev2pe[0]=m2pe[0];
    mev2pe[1]=m2pe[1];
    gain[0]=gn[0];
    gain[1]=gn[1];
    ftdel[0]=ftd[0];
    ftdel[1]=ftd[1];
  }
  integer getid(){return softid;}
  geant getmev2pe(int is){return mev2pe[is];}
  geant getadc2pe(){return adc2pe;}
  geant getdqthr(){return dqthr;}
  geant gettzer(){return tzero;}
//
  geant getathr(){
    return(dathr);
  }
  bool CalStOK(int isd){return status[isd]==0;}
  void getstat(integer sta[2]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
  void getgain(geant gn[2]){
    gn[0]=gain[0];
    gn[1]=gain[1];
  }
  geant sgain(int is){
    return gain[is];
  }
  void getftd(geant ftd[2]){
    ftd[0]=ftdel[0];
    ftd[1]=ftdel[1];
  }
  static void build();
};
//=================================================================
// class to store ANTI_paddle anode-peds/sigmas  :
class ANTIPeds{
//
private:
  integer softid;  // S(sector number 1-8)
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
//
class ANTI2JobStat{
//
private:
  static integer mccount[ANTI2C::ANJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 -> F-adc buffer ovfl
//          i=2 -> H-tdc stack ovfl
//          i=3 -> ADC range ovfl
//          i=4 -> H-TDC range ovfl
//          i=5 -> FT/Anti-trig Coinc
//          i=6 => Ghits->RawEvent OK
  static integer recount[ANTI2C::ANJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> Lev-1 trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5 -> Using TOF in LVL1
//           =6 -> Using EC  in LVL1
//
//------
  static integer chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];//channel statistics
//                              [0] -> h/w-status="ON" frequency 
//                              [1] -> "ADCA-ON"  (Nadch>0)
//                              [2] -> "ADCA-1hit"  (Nadch=1)
//
//                             [5] -> "h/w-ok frequency (validate)"
//                             [6] -> "No ADC hits"
//                             [7] -> "Multiple ADC Hits"
//                             [8] -> "bad up/down sequence in TDC"
//------
  static integer brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];// bar statistics
//                               [0] -> h/w-status="ON" frequency
//                               [1] -> "multipl-OK"
public:
  inline static void clear(){
    int i,j;
    for(i=0;i<ANTI2C::ANJSTA;i++)mccount[i]=0;
    for(i=0;i<ANTI2C::ANJSTA;i++)recount[i]=0;
    for (i=0;i<ANTI2C::ANCHMX;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       chcount[i][j]=0;
    for (i=0;i<ANTI2C::MAXANTI;i++)
                  for(j=0;j<ANTI2C::ANCSTA;j++)
                                       brcount[i][j]=0;
  }
  inline static void addmc(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< ANTI2C::ANJSTA);
    recount[i]+=1;
  }
  inline static void addch(int chnum, int i){
    assert(chnum>=0 && i>=0);
    assert(chnum < ANTI2C::ANCHMX && i < ANTI2C::ANCSTA);
    chcount[chnum][i]+=1;
  }
  inline static void addbr(int brnum, int i){
    assert(brnum>=0 && i>=0);
    assert(brnum < ANTI2C::MAXANTI && i < ANTI2C::ANCSTA);
    brcount[brnum][i]+=1;
  }
  static void print();
};
//===================================================================
#endif
