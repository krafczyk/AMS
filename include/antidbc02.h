// Author E.Choumilov 2.07.97
//
#ifndef __ANTI2DBC__
#define __ANTI2DBC__
//
#include <typedefs.h>
//
// Constants for Anti-Counters :
namespace ANTI2C{
 const integer MAXANTI=16; // number of sectors
 const integer ANCHMX=2*MAXANTI;// max. number of phys. channels 
 const integer ANAHMX=8; // max. number of TovT-pulse hits (pairs of edges) per chan.
 const integer ANTHMX=8; // max. number of FT-pulse hits (pairs of edges) per channel
 const integer ANJSTA=5;// size of Job-statistics array 
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
  static geant _tdcabw;    // bin width (ns) in TovT TDC.
  static geant _shprdt;    // shaper decay time (ns)
  static geant _ftpulw;    // FTrigger pulse width (ns) 
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
  static geant tdcabw();
  static geant shprdt();
  static geant ftpulw();
#ifdef __DB__
friend class AntiDBcD;
#endif

};
//=================================================================
// class to store individual ANTI_paddle calibr. constants:
class ANTI2Pcal{
//
private:
  integer softid;   // paddle number (BS -> Bar/Side)
  integer status[2];// each side  status (1/0->dead/alive)
  geant dtthr[2];   // trigger discr.thresh.(pe) (suppose here N.p.e. ~ Q(pC) ~ A(mV))
  geant dathr[2];   // discriminator threshold (pe) for TovT measurement
  geant ftdel[2];   // FTrig_up_edge delay wrt ADCA_up_edge (ns)
  geant mip2q;      // Total Anode-signal(pe) for MIP (pe/MeV)
  geant gain[2];    // Two side gains(PMT+Shpr) relative to some ref.counter
  geant q2pe;       // Q(pC)->Signal(pe) conv.factor (pe/pC)
  geant aipar[2][3];// anode integrator parameters for two sides
  
public:
  static ANTI2Pcal antisccal[ANTI2C::MAXANTI];
  ANTI2Pcal(){};
  ANTI2Pcal(integer sid, integer sta[2], geant tthr[2], geant athr[2], geant ftd[2],
                                 geant m2q, geant q2e,geant gn[2],  geant aip[2][3]):
                                   softid(sid),mip2q(m2q),q2pe(q2e){
    status[0]=sta[0];
    status[1]=sta[1];
    dtthr[0]=tthr[0];
    dtthr[1]=tthr[1];
    dathr[0]=athr[0];
    dathr[1]=athr[1];
    gain[0]=gn[0];
    gain[1]=gn[1];
    ftdel[0]=ftd[0];
    ftdel[1]=ftd[1];
    for(int i=0;i<3;i++){
      aipar[0][i]=aip[0][i];
      aipar[1][i]=aip[1][i];
    }
  }
  integer getid(){return softid;}
  geant getmip(){return mip2q;}
  geant getq2pe(){return q2pe;}
//
  void getathr(geant thr[2]){
    thr[0]=dathr[0];
    thr[1]=dathr[1];
  }
  void gettthr(geant thr[2]){
    thr[0]=dtthr[0];
    thr[1]=dtthr[1];
  }
  void getstat(integer sta[2]){
    sta[0]=status[0];
    sta[1]=status[1];
  }
  void getgain(geant gn[2]){
    gn[0]=gain[0];
    gn[1]=gain[1];
  }
  void getftd(geant ftd[2]){
    ftd[0]=ftdel[0];
    ftd[1]=ftdel[1];
  }
  void q2t2q(int cof, int sdf, number &t, number &q); // Q(pC)<->TovT(ns)
  static void build();
};
//=================================================================
//
class ANTI2JobStat{
//
private:
  static integer mccount[ANTI2C::ANJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => Ghits->RawEvent OK
  static integer recount[ANTI2C::ANJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> Lev-1 trigger OK
//          i=2 -> RawEv-validate OK
//           =3 -> RawEvent->RawCluster OK
//           =4 -> RawCluster->Cluster OK
//           =5- -> spare
//
//------
  static integer chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];//channel statistics
//                              [0] -> h/w-status="ON" frequency 
//                              [1] -> "TDCA-ON"  (Ntdca>0)
//                              [2] -> "TDCA-1hit"  (Ntdca=1)
//
//                             [5] -> "h/w-ok frequency (validate)"
//                             [6] -> "bad up/down sequence in TDCA"
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
