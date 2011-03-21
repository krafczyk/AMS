// $Id: job.C,v 1.782 2011/03/21 22:53:31 choutko Exp $
// Author V. Choutko 24-may-1996
// TOF,CTC codes added 29-sep-1996 by E.Choumilov 
// ANTI codes added 5.08.97 E.Choumilov
// Sep 17, 1997. ak. don't write timeid to files in dbase version
// Oct  1, 1997. ak. add FindTheBestTDV, FillTDVTable functions
// Nov   , 1997. ak. FindTheBestTDV, check id's
// Dec   , 1997. ak. dbendjob is modified
//
//misc/ Last Edit : Dec 27, 1997. ak.
// Dec 27, 2005 and befor: lots of changes for TOF,ANTI,ECAL,TrigLVL1,...by E.Choumilov. 
//

#include "tofdbc02.h"
#include "ecaldbc.h"
#include "daqecblock.h"
#include "ecalcalib.h"
#include "antidbc02.h"
#include "amsgobj.h"
#include "astring.h"
#include "cern.h"
#include <math.h>
#include "amsdbc.h"
#include "commons.h"
#include "extC.h"
#include <fenv.h>
#ifdef _PGTRACK_
#include "patt.h"
#include "TrRecon.h"
#include "TrTasDB.h"

#include "trrec.h"
#include "tkdcards.h"
#include "trdaq.h"
#include "MagField.h"

#else

#include "trmccluster.h"
#include "trcalib.h"
#include "trid.h"
#include "trrawcluster.h"
#include "tralig.h"

#endif

#include "mccluster.h"
#include "job.h"
#include "event.h"
#include "charge.h"
#include "timeid.h"
#include "mceventg.h"
#include "trdcalib.h"
#include "antidbc02.h"
#include "tofsim02.h"
#include "tofrec02.h"
#include "tofcalib02.h"
#include "anticalib02.h"
#include "trigger102.h"
//#include <trigger3.h>
#include "trigger302.h"
//#include "bcorr.h"

#include "ecalrec.h"
#include "daqevt.h"
#include "daqs2block.h"
#include "ntuple.h"
#include "user.h"
#include "status.h"
#include "richdbc.h"
#include "richid.h"
#include "richcal.h"
#include "richradid.h"
#include "richdaq.h"
#include "richlip.h"
#include "mccluster.h"
#include <sys/stat.h>
#include "producer.h"
#include "trdid.h"
#include "ecid.h"
#include "daqecblock.h"
#include "tofid.h"
#include "charge.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef __DB__
//+
 integer        ntdvNames;               // number of TDV's types
 char*          tdvNameTab[maxtdv];      // TDV's nomenclature
 int            tdvIdTab[maxtdv];
//

#include <dbS.h>
integer*   AMSJob::_ptr_start;
integer*   AMSJob::_ptr_end;
tdv_time*  AMSJob::_tdv;

extern LMS* lms;

#endif
extern "C" void rkmsinit_(float a[]);
//-
//
using namespace AMSChargConst;
//
char AMSJob::_ntuplefilename[256]="";
char AMSJob::_rootfilename[256]="";

long long AMSJob::GetNtupleFileSize(){
 struct stat64 buffer;
 long long size_n=stat64(_ntuplefilename,&buffer)? 0:buffer.st_size;
 long long size_r=stat64(_rootfilename,&buffer)? 0:buffer.st_size;
 return size_n>size_r?size_n/1024:size_r/1024;
}
time_t AMSJob::GetNtupleFileTime(){
 struct stat64 buffer;
  time_t timenow=0;
  time(&timenow);
 time_t size_n=stat64(_ntuplefilename,&buffer)? 0:buffer.st_atime;
 time_t size_r=stat64(_rootfilename,&buffer)? 0:buffer.st_atime;
 return size_n>size_r?timenow-size_n:timenow-size_r;
}

AMSNtuple* AMSJob::_pntuple=0;
AMSJob* AMSJob::_Head=0;
AMSNodeMap AMSJob::JobMap;
integer AMSJob::debug=0;
extern "C" void npq_();
const uinteger AMSJob::Reconstruction=1;
const uinteger AMSJob::RealData=2;
const uinteger AMSJob::CTracker=4;
const uinteger AMSJob::CTOF=8;
const uinteger AMSJob::CAnti=16;
const uinteger AMSJob::CCerenkov=32;
const uinteger AMSJob::CMagnet=64;
const uinteger AMSJob::CRICH=128;
const uinteger AMSJob::CTRD=256;
const uinteger AMSJob::CSRD=32;       // Same as cerenkov 
const uinteger AMSJob::CAMS=512;
const uinteger AMSJob::CEcal=1024;
const uinteger AMSJob::MTracker=2048;
const uinteger AMSJob::MTOF=4096;
const uinteger AMSJob::MAnti=4096*2;
const uinteger AMSJob::MCerenkov=4096*4;
const uinteger AMSJob::MLVL1=4096*8;
const uinteger AMSJob::MLVL3=4096*16;
const uinteger AMSJob::MAxAMS=4096*32;
const uinteger AMSJob::MAll=4096*64;
const uinteger AMSJob::MEcal=4096*128;
const uinteger AMSJob::Production=4096*256;
const uinteger AMSJob::Calibration=AMSJob::CTracker+
                                   AMSJob::CTOF+
                                   AMSJob::CAnti+
                                   AMSJob::CCerenkov+
                                   AMSJob::CMagnet+
                                   AMSJob::CRICH+
                                   AMSJob::CTRD+
                                   AMSJob::CEcal+
                                   AMSJob::CAMS;

const uinteger AMSJob::Monitoring=(AMSJob::MTracker)+
                                  (AMSJob::MTOF)+
                                  (AMSJob::MAnti)+
                                  (AMSJob::MEcal)+
                                  (AMSJob::MCerenkov)+
                                  (AMSJob::MLVL1)+
                                  (AMSJob::MLVL3)+
                                  (AMSJob::MAxAMS)+
                                  (AMSJob::MAll);
//


//
void AMSJob::data(){
#ifdef __HPUX__
  AMSgObj::GTrMedMap=*(new  AMSNodeMap() );
  AMSgObj::GVolMap= *(new  AMSNodeMap() );
  AMSgObj::BookTimer= *(new AMSStat() );
#endif  

  GCSTAK.NJTMIN=1;
  GCTRAK.upwght=0;
  GCTIME.TIMINT=10000000;
  GCTIME.TIMEND=10;
  GCTIME.ITIME=0;
  SELECTFFKEY.Run=0;
  SELECTFFKEY.Event=0;
  SELECTFFKEY.RunE=0;
  SELECTFFKEY.EventE=0;
  VBLANK(SELECTFFKEY.File,40);
  char * SELECT="SELECT";
  char * MIXED="MIXED";
  FFKEY(SELECT,(float*)&SELECTFFKEY,sizeof(SELECTFFKEY_DEF)/sizeof(integer),MIXED);

  { 
    for(int i=0;i<33;i++){
      STATUSFFKEY.status[i]=0;
    }
        STATUSFFKEY.status[33]=10;
  }
  FFKEY("ESTATUS",(float*)&STATUSFFKEY,sizeof(STATUSFFKEY_DEF)/sizeof(integer),"MIXED");

  AMSFFKEY.Simulation=0; // Simulation
  AMSFFKEY.Reconstruction=1; // Reconstruction
  AMSFFKEY.Jobtype=0; // Default
  AMSFFKEY.Debug=1;//2
  AMSFFKEY.CpuLimit=15;//3
  AMSFFKEY.Read=0;//4
  AMSFFKEY.Write=1;//5
  AMSFFKEY.Update=0;
  VBLANK(AMSFFKEY.Jobname,40);//6
  VBLANK(AMSFFKEY.Setupname,40);//46
  char amsetup[16]="AMS02P";
  UCTOH(amsetup,AMSFFKEY.Setupname,4,16);
  AMSFFKEY.ZeroSetupOk=0;
  // Set Defaults
FFKEY("AMSJOB",(float*)&AMSFFKEY+2,sizeof(AMSFFKEY_DEF)/sizeof(integer),"MIXED");
_siamsdata();
_reamsdata();

}

void AMSJob::_siamsdata(){
IOPA.hlun=0;//1
VBLANK(IOPA.hfile,40);//2
IOPA.ntuple=1;//42
IOPA.WriteAll=102;//43
IOPA.Portion=.2;//44
VBLANK(IOPA.TriggerC,40);
VBLANK(AMSFFKEY.TDVC,400);
char amsp[12]="AMSParticle";
UCTOH(amsp,IOPA.TriggerC,4,12);//45
IOPA.mode=0;//85
VBLANK(IOPA.ffile,40);//86
IOPA.MaxNtupleEntries=1000000000;//126
IOPA.MaxFileSize=100000000;
IOPA.MaxFileTime=86400*3;
IOPA.BuildMin=-1;//170
IOPA.histoman=1;//171
IOPA.skip=0; //172
IOPA.WriteRoot=0;//127
IOPA.WriteTGeometry=0;//173
IOPA.WriteTDVDataInRoot=0;
IOPA.ReadAMI=0;
VBLANK(IOPA.TGeometryFileName,40);//174
char tgeofilename[16]="ams02.root";
UCTOH(tgeofilename,IOPA.TGeometryFileName,4,16);
VBLANK(IOPA.rfile,40);//128
FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");
TRMFFKEY.OKAY=0;
TRMFFKEY.TIME=0;
FFKEY("TERM",(float*)&TRMFFKEY,sizeof(TRMFFKEY_DEF)/sizeof(integer),"MIXED");
MISCFFKEY.BTempCorrection=0;//(1)
MISCFFKEY.BeamTest=0;//(2)
MISCFFKEY.BZCorr=1;//(3)
MISCFFKEY.G3On=1;//(4)
MISCFFKEY.G4On=0;//(5)
MISCFFKEY.dbwrbeg=0;//(6)DBwriter UTC-time begin 
MISCFFKEY.dbwrend=0;//(7)DBwriter UTC-time end
MISCFFKEY.NumThreads=1;
MISCFFKEY.DynThreads=0;
MISCFFKEY.DivideBy=1;
MISCFFKEY.ChunkThreads=8192;
MISCFFKEY.RaiseFPE=0;
FFKEY("MISC",(float*)&MISCFFKEY,sizeof(MISCFFKEY_DEF)/sizeof(integer),"MIXED");



G4FFKEY.BFOrder=4;
G4FFKEY.Delta=0.001;
G4FFKEY.UniformMagField=0;
G4FFKEY.Geant3CutsOn=1;
G4FFKEY.PhysicsListUsed=1;
G4FFKEY.LowEMagProcUsed=0;
G4FFKEY.IonPhysicsModel=1;
G4FFKEY.UseEMDModel=0;
FFKEY("G4FF",(float*)&G4FFKEY,sizeof(G4FFKEY_DEF)/sizeof(integer),"MIXED");

PRODFFKEY.Debug=0;
PRODFFKEY.Job=-1;
FFKEY("PROD",(float*)&PRODFFKEY,sizeof(PRODFFKEY_DEF)/sizeof(integer),"MIXED");





 HVSEC.ihvse[0]=2;
 HVSEC.ihvse[1]=2;
FFKEY("HISE",(float*)&HVSEC,sizeof(HVSEC_DEF)/sizeof(integer),"MIXED");







_sitkdata();
_signdata();

_sitof2data();
_sianti2data();
_sitrig2data();
  
_sirichdata();
_siecaldata();
_sitrddata();
_sisrddata();
_simag2data();

}
//
//========================================================================
//
void AMSJob::_sirichdata(){

// Geometry defaults: unused
  /*************************************************
  RICGEOM.top_radius=RICHDB::top_radius;
  RICGEOM.bottom_radius=RICHDB::bottom_radius;
  RICGEOM.hole_radius=RICHDB::hole_radius;
  RICGEOM.height=RICHDB::rich_height;
  RICGEOM.radiator_radius=RICHDB::rad_radius;
  RICGEOM.radiator_height=RICHDB::rad_height;
  RICGEOM.radiator_box_length=RICHDB::rad_length;
  RICGEOM.light_guides_height=RICHDB::lg_height;
  RICGEOM.light_guides_length=RICHDB::lg_length;
  *******************************************************/

// Move information to fortran commons
  RICGTKOV.usrcla=RICHDB::rad_clarity;
  RICGTKOV.scatprob=RICHDB::scatprob;
  RICGTKOV.scatang=RICHDB::scatang;

  RICCONTROLFFKEY.iflgk_flag=0;  // This should be always zero
  RICCONTROLFFKEY.tsplit=0;      // Bo time splitting at the end 
  VBLANK(RICCONTROLFFKEY.pmttables,50);
  VBLANK(RICCONTROLFFKEY.pmttables_out,50);

//  FFKEY("RICGEOM",(float *)&RICGEOM,sizeof(RICGEOM_DEF)/sizeof(integer),"REAL");
  FFKEY("RICCONT",(float *)&RICCONTROLFFKEY,sizeof(RICCONTROLFFKEY_DEF)/sizeof(integer),"MIXED");


  RICRADSETUPFFKEY.setup=1;
  VBLANK(RICRADSETUPFFKEY.tables_in,50);
  VBLANK(RICRADSETUPFFKEY.tables_out,50);
  VBLANK(RICRADSETUPFFKEY.finemesh_in,50);
  VBLANK(RICRADSETUPFFKEY.alignment_in,50);
  FFKEY("RICSET",(float *)&RICRADSETUPFFKEY,sizeof(RICRADSETUPFFKEY_DEF)/sizeof(integer),"MIXED");

  RICRECFFKEY.recon[0]=11;   //beta reconstruction
  RICRECFFKEY.recon[0]=1;   //beta reconstruction  //no lip by default
  RICRECFFKEY.recon[1]=1;   //Z reconstruction

  RICRECFFKEY.liprflag[0]=1;  // LIP beta reconstructions to be performed
  RICRECFFKEY.liprflag[1]=1;  // LIP charge reconstructions to be performed

  RICRECFFKEY.lipsigma[0]=RICRECFFKEY.lipsigma[1]=     // LIP residuals sigmas
    RICRECFFKEY.lipsigma[2]=RICRECFFKEY.lipsigma[3]=0.; //(not yet implemented)

  RICRECFFKEY.lipdcut[0]=RICRECFFKEY.lipdcut[1]=0.; // LIP distance cut
                                                      //(not yet implemented) 

  RICRECFFKEY.lipznorm[0]=RICRECFFKEY.lipznorm[1]=0.; // LIP chg overall fact
                                                      //(not yet implemented) 

  RICRECFFKEY.lipdisp=0;  // LIP RICH display

  RICRECFFKEY.store_rec_hits=0;  // LIP RICH display

  FFKEY("RICREC",(float *)&RICRECFFKEY,sizeof(RICRECFFKEY_DEF)/sizeof(integer),"MIXED");

  RICFFKEY.ReadFile=0;
  RICFFKEY.sec[0]=0;
  RICFFKEY.sec[1]=0;
  RICFFKEY.min[0]=0;
  RICFFKEY.min[1]=0;
  RICFFKEY.hour[0]=0;
  RICFFKEY.hour[1]=0;
  RICFFKEY.day[0]=1;
  RICFFKEY.day[1]=1;
  RICFFKEY.mon[0]=0;
  RICFFKEY.mon[1]=0;
  RICFFKEY.year[0]=101;
  RICFFKEY.year[1]=107;
  VBLANK(RICFFKEY.fname_in,200);
  VBLANK(RICFFKEY.fname_out,200);
  //  FFKEY("RICAL",(float*)&RICFFKEY,sizeof(RICFFKEY_DEF)/sizeof(integer),"INTEGER");
  FFKEY("RICAL",(float*)&RICFFKEY,sizeof(RICFFKEY_DEF)/sizeof(integer),"MIXED");

  RICDBFFKEY.dump=0;
  RICDBFFKEY.sec[0]=0;
  RICDBFFKEY.sec[1]=0;
  RICDBFFKEY.min[0]=0;
  RICDBFFKEY.min[1]=0;
  RICDBFFKEY.hour[0]=0;
  RICDBFFKEY.hour[1]=0;
  RICDBFFKEY.day[0]=1;
  RICDBFFKEY.day[1]=1;
  RICDBFFKEY.mon[0]=0;
  RICDBFFKEY.mon[1]=0;
  RICDBFFKEY.year[0]=101;
  RICDBFFKEY.year[1]=107;
  FFKEY("RICDB",(float*)&RICDBFFKEY,sizeof(RICDBFFKEY_DEF)/sizeof(integer),"MIXED");
}

//------------------------------------------------------
void AMSJob::_sitrig2data(){
//
  TGL1FFKEY.trtype=-99; //(1) Requested PhysBranches(pattern) for LVL1 global OR(see datacards.doc)
//exapmles: 1->unbTOF;2->Z>=1;4->Z>=2;8->SlowZ>=2;16->e;32->gamma;64->EC(FTE);128->ext
// --> TOF :
// 
  TGL1FFKEY.toflc=-99; //(2)required TOF-FastTrigger(Z>=1) LAYERS configuration
//                     (=0/1/2/3-> accept  >=3of4/ALL4/Top&Bot2of4/any2of4 layer coincidence)
//                      Logically define(if>=0) short set of possible LUT1+LUT2 settings; "-" means
//                      usage of exactly defined DB settings of LUT1+LUT2
  TGL1FFKEY.tofsc=-99; //(3)required TOF-FT Plane-SIDEs configuration MN
//                                 (m(n)=0/1-> plane two-sides-AND/OR selection, n->FTC, m->BZ)
  TGL1FFKEY.toflcsz=-99;//(4)required TOF-FTZ(slowZ>=2) LAYERS configuration IJ(J=0/1/2/3-> 
//                        ->topAND*botAND/topAND*botOR/topOR*botAND/topOR*botOR;
//                        I=0/1 => "*"=AND/OR. "-" means usage of DB setting 
  TGL1FFKEY.toflcz=-99;//(5)required TOF-BZ(Z>=2 flag for LVL1) LAYERS configuration 
//                     (=0/1/2-> accept  >=3of4/ALL4/Top&Bot>=2of4 layer coincidence)
//                      Logically define(if>=0) short set of possible LUT-BZ settings; "-" means
//                      usage of exactly defined DB settings of LUT-BZ
  TGL1FFKEY.tofextwid=-99;//(6) ext.width code(top&bot) in FTZ(SlowZ>=2) logic(if <0 => take indiv.values from DB)
//5_lowsignbits/next5moresignbits=>widthExtentionCodes for TopTOF/BotTOF 
// --> ANTI :
  TGL1FFKEY.antismx[0]=-99; //(7) max. acceptable fired ANTI-paddles(logical, in equat.region)
  TGL1FFKEY.antismx[1]=-99; //(8) max. acceptable fired ANTI-paddles(logical, in polar.region)
  TGL1FFKEY.antisc=-99;     //(9) required ANTI-trigger SIDE configuration
//                                 (=0/1-> two-sides-AND/OR selection)
// --> FastTrigger :
  TGL1FFKEY.cftmask=-99;//(10) GlobalFTMask(i|j|k=>FTE|FTZ>=2|FTC>=1; i(j,k)=1(active)/0(disabled))
//
// For all above cards "-99" means usage of more detailed and up-to-date DB-settings
//   To use manual(through data-card) control put necessary >=0 value !!! 
//
  TGL1FFKEY.RebuildLVL1=0;//(11) (for RealData) rebuil lvl1-obj from subdet.RawEvent objects
// 
  TGL1FFKEY.MaxScalersRate=20000;//(12)
  TGL1FFKEY.MinLifeTime=0.;//(13)
//
// --> orbit:
  TGL1FFKEY.TheMagCut=0.7;// (14)geom.latitude cut for anti-cut selection(below-#7, above-#8)
//
// --> Ecal
  TGL1FFKEY.ecorand=-99;   //(15) 1/2=>OR/END of 2 proj. requirements on min. layer multiplicity(FTE)
  TGL1FFKEY.ecprjmask=-99; //(16) proj.mask(lkji: ij=1/0->XYproj active/disabled in FTE; kl=same for LVL1(angle)
//
  TGL1FFKEY.Lvl1ConfMCVers=1;//(17)MC def.version number of "Lvl1ConfMC.*" -file
  TGL1FFKEY.Lvl1ConfRDVers=1167606001;//(18)RD def. UTC-extention of "Lvl1ConfRD.***" -file (20070101 0000001)
  TGL1FFKEY.Lvl1ConfRead=1;  //(19) MN, N=0/1->read Lvl1Config-data from DB/raw_file
//                                      M=0/1->read Lvl1Config raw-file from official/private dir
  TGL1FFKEY.printfl=0;      // (20) PrintFlg=MN,N=0/1/2->no/stat+hist/+SetupInf(whenChanged)
//                                         M=0/1/2/3/4->no/ErrWarn/+TrigPatt(ev-by-ev)/+MoreDebug/+BlockBitDump
//
  TGL1FFKEY.Lvl1ConfSave=0; // (21) If RD LVL1-setup block found, 2/1/0 -> save2file+DB/save2file/mem.update_only
//
  TGL1FFKEY.NoPrescalingInMC=0;// (22) disable prescaling in MC if set to 1
//
  TGL1FFKEY.NoMaskingInMC=1;// (23) 1/0=> No/Yes Masking in PhysBrPatt(MC, =1 to see all fired branches, not only requested)
//
  TGL1FFKEY.sec[0]=0;//(24) 
  TGL1FFKEY.sec[1]=0;
  TGL1FFKEY.min[0]=0;
  TGL1FFKEY.min[1]=0;
  TGL1FFKEY.hour[0]=0;
  TGL1FFKEY.hour[1]=0;
  TGL1FFKEY.day[0]=1;
  TGL1FFKEY.day[1]=1;
  TGL1FFKEY.mon[0]=0;
  TGL1FFKEY.mon[1]=0;
  TGL1FFKEY.year[0]=101;
  TGL1FFKEY.year[1]=125;//(35)
//
  FFKEY("TGL1",(float*)&TGL1FFKEY,sizeof(TGL1FFKEY_DEF)/sizeof(integer),"MIXED");
//----
  LVL3SIMFFKEY.NoiseProb[1]=2.e-10;
  LVL3SIMFFKEY.NoiseProb[0]=LVL3SIMFFKEY.NoiseProb[1]*1.65;
  FFKEY("L3SIM",(float*)&LVL3SIMFFKEY,sizeof(LVL3SIMFFKEY_DEF)/sizeof(integer),"MIXED");


  LVL3EXPFFKEY.NEvents=2048;
  LVL3EXPFFKEY.ToBad=10;
  LVL3EXPFFKEY.TryAgain=10;
  LVL3EXPFFKEY.Range[0][0]=2.5;
  LVL3EXPFFKEY.Range[0][1]=2.;
  LVL3EXPFFKEY.Range[1][0]=1000000;
  LVL3EXPFFKEY.Range[1][1]=3;
  LVL3EXPFFKEY.Range[2][0]=2.;
  LVL3EXPFFKEY.Range[2][1]=2.5;

  FFKEY("L3EXP",(float*)&LVL3EXPFFKEY,sizeof(LVL3EXPFFKEY_DEF)/sizeof(integer),"MIXED");

  LVL3FFKEY.MinTOFPlanesFired=3;
  LVL3FFKEY.UseTightTOF=0;
  LVL3FFKEY.TrTOFSearchReg=6.;
  LVL3FFKEY.TrMinResidual=0.03;
  LVL3FFKEY.TrMaxResidual[0]=-1.2;
  LVL3FFKEY.TrMaxResidual[1]=-0.2;
  LVL3FFKEY.TrMaxResidual[2]=-0.3;
  LVL3FFKEY.TrMaxHits=30;
  LVL3FFKEY.Splitting=0.04;
  LVL3FFKEY.NRep=1;
  LVL3FFKEY.Accept=0;
  LVL3FFKEY.RebuildLVL3=2;
  LVL3FFKEY.NoK=1;
  LVL3FFKEY.TrHeavyIonThr=200;
  LVL3FFKEY.SeedThr=-32;
  LVL3FFKEY.Stat=0;
  LVL3FFKEY.histprf=0;
  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");

}
//
//===============================================================================
//
#ifndef _PGTRACK_
void AMSJob::_sitkdata(){
TKGEOMFFKEY.ReadGeomFromFile=1;
TKGEOMFFKEY.WriteGeomToFile=0;
TKGEOMFFKEY.UpdateGeomFile=0;
VBLANK(TKGEOMFFKEY.gfile,40);
FFKEY("TKGE",(float*)&TKGEOMFFKEY,sizeof(TKGEOMFFKEY_DEF)/sizeof(integer),
"MIXED");


TRMCFFKEY.alpha=250;
TRMCFFKEY.beta=0.46;
TRMCFFKEY.gamma=0.25;
TRMCFFKEY.fastswitch=5.e-5;  // inverse linear density of primary electrons
TRMCFFKEY.dedx2nprel=0.33e6;
TRMCFFKEY.ped[0]=500;
TRMCFFKEY.ped[1]=500;
TRMCFFKEY.gain[0]=8;
TRMCFFKEY.gain[1]=8;
TRMCFFKEY.sigma[1]=55./14./sqrt(3.); // sig/noise ratio is about 14 for y
TRMCFFKEY.sigma[0]=TRMCFFKEY.sigma[1]*1.41;   // x strip two times larger y
TRMCFFKEY.delta[0]=-0.67;
TRMCFFKEY.delta[1]=1.0;
TRMCFFKEY.gammaA[0]=-0.3;
TRMCFFKEY.gammaA[1]=0.1;
TRMCFFKEY.NonGaussianPart[0]=0;
TRMCFFKEY.NonGaussianPart[1]=0.1;
TRMCFFKEY.BadCh[0]=0.01;//556
TRMCFFKEY.BadCh[1]=0.01;//557

TRMCFFKEY.cmn[0]=10;
TRMCFFKEY.cmn[1]= 6;
TRMCFFKEY.adcoverflow=32700;
TRMCFFKEY.NoiseOn=1;
TRMCFFKEY.sec[0]=0;
TRMCFFKEY.sec[1]=0;
TRMCFFKEY.min[0]=0;
TRMCFFKEY.min[1]=0;
TRMCFFKEY.hour[0]=0;
TRMCFFKEY.hour[1]=0;
TRMCFFKEY.day[0]=1;
TRMCFFKEY.day[1]=1;
TRMCFFKEY.mon[0]=0;
TRMCFFKEY.mon[1]=0;
TRMCFFKEY.year[0]=102;
TRMCFFKEY.year[1]=120;
TRMCFFKEY.GenerateConst=0;
TRMCFFKEY.WriteHK=0;
TRMCFFKEY.thr1R[0]=-2.75;
TRMCFFKEY.thr1R[1]=3.5;
TRMCFFKEY.thr2R[0]=1;
TRMCFFKEY.thr2R[1]=1;
TRMCFFKEY.neib[0]=0;
TRMCFFKEY.neib[1]=1;
TRMCFFKEY.CalcCmnNoise[0]=1;
TRMCFFKEY.CalcCmnNoise[1]=0;
{
int i,j,k;
for(i=0;i<8;i++){
  for(j=0;j<2;j++){
   for(k=0;k<32;k++)TRMCFFKEY.RawModeOn[i][j][k]=0;
  }
}
}
FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");

TRCALIB.CalibProcedureNo=0;
TRCALIB.EventsPerCheck=1000;
TRCALIB.PedAccRequired[0]=0.12;
TRCALIB.PedAccRequired[1]=0.09;
TRCALIB.Validity[0]=0;
TRCALIB.Validity[1]=36000;
TRCALIB.RhoThrA=0.7;
TRCALIB.RhoThrV=0.8;
TRCALIB.BadChanThr[0]=3.3;
TRCALIB.BadChanThr[1]=0.002;
TRCALIB.Method=2;
TRCALIB.Pass=1;
TRCALIB.DPS=1;
TRCALIB.UPDF=4;
TRCALIB.LaserRun=0;
TRCALIB.PrintBadChList=0;
TRCALIB.EventsPerIteration[0]=100;
TRCALIB.EventsPerIteration[1]=100;
TRCALIB.EventsPerIteration[2]=100;
TRCALIB.EventsPerIteration[3]=100;
TRCALIB.NumberOfIterations[0]=100;
TRCALIB.NumberOfIterations[1]=100;
TRCALIB.NumberOfIterations[2]=100;
TRCALIB.NumberOfIterations[3]=100;
TRCALIB.BetaCut[0][0]=0.7;
TRCALIB.BetaCut[0][1]=10.;
TRCALIB.BetaCut[1][0]=1;
TRCALIB.BetaCut[1][1]=10;
TRCALIB.BetaCut[2][0]=0.7;
TRCALIB.BetaCut[2][1]=1.4;
TRCALIB.BetaCut[3][0]=0.7;
TRCALIB.BetaCut[3][1]=1.4;
TRCALIB.HitsRatioCut[0]=2.2;
TRCALIB.HitsRatioCut[1]=2.2;
TRCALIB.HitsRatioCut[2]=0.998;
TRCALIB.HitsRatioCut[3]=0.998;
TRCALIB.MomentumCut[0][0]=-FLT_MAX;
TRCALIB.MomentumCut[0][1]=FLT_MAX;
TRCALIB.MomentumCut[1][0]=3;
TRCALIB.MomentumCut[1][1]=FLT_MAX;
TRCALIB.MomentumCut[2][0]=0.4;
TRCALIB.MomentumCut[2][1]=2.5;
TRCALIB.MomentumCut[3][0]=0.1;
TRCALIB.MomentumCut[3][1]=10;
TRCALIB.Chi2Cut[0]=3;
TRCALIB.Chi2Cut[1]=3;
TRCALIB.Chi2Cut[2]=100;
TRCALIB.Chi2Cut[3]=100;
TRCALIB.PatStart=0;
TRCALIB.MultiRun=0;
TRCALIB.Version=0;
TRCALIB.EventsPerRun=10000001;
int i;
for(i=0;i<6;i++){
  TRCALIB.Ladder[i]=0;
  TRCALIB.ActiveParameters[i][0]=1;   // x
  TRCALIB.ActiveParameters[i][1]=1;   // y
  TRCALIB.ActiveParameters[i][2]=0;   // z
  TRCALIB.ActiveParameters[i][3]=0;   // pitch  zx
  TRCALIB.ActiveParameters[i][4]=1;   // yaw    xy
  TRCALIB.ActiveParameters[i][5]=0;   // roll   yz
}
 FFKEY("TRCALIB",(float*)&TRCALIB,sizeof(TRCALIB_DEF)/sizeof(integer),"MIXED");



 TRALIG.One=1.025;
 TRALIG.GlobalFit=0;
 TRALIG.InitDB=0;
 TRALIG.ReWriteDB=0;
 TRALIG.UpdateDB=0;
 TRALIG.MaxPatternsPerJob=50;
 TRALIG.MaxEventsPerFit=9999;
 TRALIG.MinEventsPerFit=999;
 for(i=0;i<9;i++){
  TRALIG.Cuts[i][0]=0;
  TRALIG.Cuts[i][1]=0;
}
  TRALIG.Cuts[0][1]=1;      // chi2 cut for alg=0
  TRALIG.Cuts[1][0]=0.992;   // cos  cut for alg =0 
  TRALIG.Cuts[2][0]=0.5;     //rel mom cut
  TRALIG.Cuts[2][1]=2;
  TRALIG.Cuts[3][0]=0;      // beta cut
  TRALIG.Cuts[3][1]=1.4;    
  TRALIG.Cuts[4][0]=0;      // beta pattern cut
  TRALIG.Cuts[4][1]=0;    
  TRALIG.Cuts[5][0]=0.5;      // mass cut
  TRALIG.Cuts[5][1]=2.0;    
  TRALIG.Cuts[6][0]=0.;         // betaerror cut
  TRALIG.Cuts[6][1]=0.04;   
  TRALIG.Cuts[7][0]=6;         //min tr point cut
  TRALIG.Cuts[7][1]=10000;     //max chi2   
  TRALIG.Cuts[8][0]=0;     //min rig   
  TRALIG.Cuts[8][1]=1;     //min iter   
  TRALIG.Cuts[9][0]=100000000; //diff between positive and negative rig
  TRALIG.Cuts[9][1]=0;   // 1 only pos rig -1 only neg rig
TRALIG.Algorithm=0;
for( i=0;i<9;i++){
  int one=1;
  int zero=0;

// layersonly
/*
  if(i==1)one=0;
  TRALIG.ActiveParameters[i][0]=zero;   // x
  TRALIG.ActiveParameters[i][1]=zero;   // y
  TRALIG.ActiveParameters[i][2]=one;   // z
  TRALIG.ActiveParameters[i][3]=zero;   // pitch  zx
  TRALIG.ActiveParameters[i][4]=zero;   // yaw    xy
  TRALIG.ActiveParameters[i][5]=zero;   // roll   yz
*/
//ladders 


  TRALIG.ActiveParameters[i][0]=one;   // x
  TRALIG.ActiveParameters[i][1]=one;   // y
  TRALIG.ActiveParameters[i][2]=one;   // z
  //TRALIG.ActiveParameters[i][2]=zero;
  TRALIG.ActiveParameters[i][3]=one;   // pitch  zx
  //TRALIG.ActiveParameters[i][3]=zero;   // roll   yz
  TRALIG.ActiveParameters[i][4]=one;   // yaw    xy
  TRALIG.ActiveParameters[i][5]=one;   // roll   yz
  //TRALIG.ActiveParameters[i][5]=zero;   // roll   yz
  if(i==1){
   for (int k=0;k<6;k++){
   if(TRALIG.ActiveParameters[i][k]==-one)TRALIG.ActiveParameters[i][k]=zero;  
   }
  }     


}
  TRALIG.EventsPerRun=1001;
  TRALIG.LayersOnly=0;
  TRALIG.LaddersOnly=1;
  TRALIG.GlobalGoodLimit=0.085;
  TRALIG.SingleLadderEntryLimit=10000;
 FFKEY("TRALIG",(float*)&TRALIG,sizeof(TRALIG_DEF)/sizeof(integer),"MIXED");





}
#endif
void AMSJob::_signdata(){
CCFFKEY.coo[0]=-1.e10;
CCFFKEY.coo[1]=-1.e10;
CCFFKEY.coo[2]=-1.e10;
CCFFKEY.coo[3]=1.e10;
CCFFKEY.coo[4]=1.e10;
CCFFKEY.coo[5]=1.e10;
CCFFKEY.dir[0]=-1.;
CCFFKEY.dir[1]=-1.;
CCFFKEY.dir[2]=-1.;
CCFFKEY.dir[3]=1.;
CCFFKEY.dir[4]=1.;
CCFFKEY.dir[5]=1.;
CCFFKEY.momr[0]=-1.e10;
CCFFKEY.momr[1]=1.e10;
CCFFKEY.fixp=0;
CCFFKEY.albedor=0.;
CCFFKEY.albedocz=0.05;
CCFFKEY.npat=1;
CCFFKEY.run=100;
CCFFKEY.low=5;
CCFFKEY.earth=0;
CCFFKEY.theta=51.;
CCFFKEY.phi=290.;
CCFFKEY.polephi=108.392;
CCFFKEY.begindate=1072010;
CCFFKEY.enddate=  1012020;
CCFFKEY.begintime=170000;
CCFFKEY.endtime=0;
CCFFKEY.oldformat=0;
CCFFKEY.sdir=1;
CCFFKEY.Fast=0;
CCFFKEY.StrCharge=2;
CCFFKEY.StrMass=-1;
CCFFKEY.SpecialCut=0;
VBLANK(CCFFKEY.FluxFile,40);
VBLANK(CCFFKEY.DirFile,40);
CCFFKEY.Angle=0;
CCFFKEY.DirFilePositions[0]=1;
CCFFKEY.DirFilePositions[1]=0;
CCFFKEY.curtime=0;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");

GMFFKEY.GammaSource=0; //ISN  
GMFFKEY.SourceCoo[0]=1.46; //RA
GMFFKEY.SourceCoo[1]=0.384; //DEC default:Crab
GMFFKEY.SourceVisib=0.873; //50 degrees
GMFFKEY.GammaBg=0;
GMFFKEY.BgAngle=0.087; //5 degrees
FFKEY("GMSRC",(float*)&GMFFKEY,sizeof(GMFFKEY_DEF)/sizeof(integer),"MIXED");

LVTMFFKEY.BufSize=12;
LVTMFFKEY.MeanTime=333e-6;
LVTMFFKEY.MinTime=140e-6;
LVTMFFKEY.MinFTime=1e-6;

FFKEY("LVTM",(float*)&LVTMFFKEY,sizeof(LVTMFFKEY_DEF)/sizeof(integer),"MIXED");

}
//=================================================================================
//
void AMSJob::_sitof2data(){
  TFMCFFKEY.TimeSigma=0.270; //(1) side time resolution(ns, now =CounterResol(0.19)*sqrt(2)) 
  TFMCFFKEY.sumHTdel=4.5;    //(2) eff. h/w delay of sumHT(sumSHT)-signal wrt LT-signal for MC
  TFMCFFKEY.canormf=1.;      //(3) amplif. factor(common for all bar-types,i.e. all SE MP are mult.by it))
  TFMCFFKEY.dzconv=0.;        //(4) if !=0 => thickness of optional TungstenConverter
  TFMCFFKEY.Thr=0.1;          //(5) Sc.bar Edep-thresh.(Mev) to participate in Simul.   
//
  TFMCFFKEY.mcprtf[0]=0;     //(6) TOF MC print flag for init arrays(=1/2-> SEspecra/PMpulseShape)
  TFMCFFKEY.mcprtf[1]=0;     //(7) TOF MC print flag for MC PM-pulses(1/2-> fine/coarse scale)
  TFMCFFKEY.mcprtf[2]=0;     //(8) ...................... histograms
  TFMCFFKEY.mcprtf[3]=0;     //(9) Debug-printing
  TFMCFFKEY.mcprtf[4]=0;     //(10) spare
  TFMCFFKEY.trlogic[0]=1; //(11) 1/0->do/not (CP+BZ) trpatt-masking in SPT2(not affect trigger decision) 
  TFMCFFKEY.trlogic[1]=0; //(12) spare 
  TFMCFFKEY.fast=0;       //(13) 0/1/2->off/on fast generation in mceventg.C(2-> EC), Not For precise Accept.calc
  TFMCFFKEY.daqfmt=0;     //(14) 0/1-> raw/reduced TDC format for DAQ simulation
  TFMCFFKEY.birks=1;      //(15) 0/1->  not/apply birks corrections
  TFMCFFKEY.mcseedo=0;    //(16) 1/0->Use RealDataCalib_Copy/MCCalib files as MCSeed
  TFMCFFKEY.blshift=0.;   //(17) base line shift at fast discr.input (mv)
  TFMCFFKEY.hfnoise=5.;   //(18) high freq. noise .......   
//     
  TFMCFFKEY.ReadConstFiles=1000;//(19)LPTS(L=TDClinCorSD,P=PedsMC,T=TimeDistr,S=MCCalibSeeds);L(P,T,S)=0/1->DB/RawFiles
//
  TFMCFFKEY.addpeds=0;//(20) add peds into empty(no MC dE/dX) channels
  TFMCFFKEY.calvern=2;//(21) TofCflistMC-file(barcal_files vers. list) version number
  TFMCFFKEY.tdclin=0;//(22) 1/0->do/not activate TDC-linearity logic for MC
  TFMCFFKEY.tdcovf=1;//(23) 1/0->do/not activate TDC-overf.protection logic(= usage of TDC TrigTimeTag subtraction)
//
FFKEY("TFMC",(float*)&TFMCFFKEY,sizeof(TFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_simag2data(){
  //PZMAG #ifdef _PGTRACK_
  //MAGSFFKEY.init();
  //#else
  MAGSFFKEY.magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  MAGSFFKEY.fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  MAGSFFKEY.ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 
  MAGSFFKEY.r0[0]=0;
  MAGSFFKEY.r0[1]=0;
  MAGSFFKEY.r0[2]=0;
  MAGSFFKEY.roll=0;
  MAGSFFKEY.yaw=0;
  MAGSFFKEY.pitch=0;
  MAGSFFKEY.rphi=0;
  //#endif
FFKEY("MAGS",(float*)&MAGSFFKEY,sizeof(MAGSFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_siecaldata(){
  ECMCFFKEY.fastsim=0;     //(1) 1/0-> fast/slow simulation algorithm(missing fast TBD)
  ECMCFFKEY.mcprtf=0;       //(2) print_hist flag (0/1->no/yes)
  ECMCFFKEY.cutge=0.0005;    //(3) cutgam=cutele cut for EC_volumes
  ECMCFFKEY.silogic[0]=0;   //(4) SIMU logic flag =0/1/2->peds+noise/no_noise/no_peds
  ECMCFFKEY.silogic[1]=0;   //(5) 1/0-> to use RealDataCopy(sd)/MC(mc) RLGA/FIAT-files as MC-Seeds
  ECMCFFKEY.mev2mev=78.24;  //(6) Geant dE/dX(MeV)->MCEmeas(=Evis,MeV,noRelGainsApplied,PmCouplingIncl, PE-fluct.incl)
//                                                   to have Tot.MCMeas=Einp(at center,at 500 kev geant3 cut)
  ECMCFFKEY.mev2adc=1.613;  //(7) MCEmeas(MeV)->ADCch factor(MIP-m.p.->16th channel)(...) (for fendr=0.0)
  ECMCFFKEY.safext=-10.;    //(8) Extention(cm) of EC transv.size when TFMC 13=2 is used
  ECMCFFKEY.mev2pes=55.;    //(9) PM ph.electrons/Mev(dE/dX)(8000*0.0344*0.2)
  ECMCFFKEY.pmseres=0.8;    //(10)PM single-electron spectrum resolution
  ECMCFFKEY.adc2q=0.045;    //(11)Anode(H,L) ADCch->Q(pC) conv.factor(pC/adc)(used for satur.eff)
  ECMCFFKEY.fendrf=0.;      //(12) (0.3)fiber end-cut reflection factor(=0 since 2.12.10 on EC-group request)
  ECMCFFKEY.physa2d=6.;     //(13) physical(for trigger) an/dyn ratio(mv/mv) 
  ECMCFFKEY.hi2low=36.;     //(14) not used now 
  ECMCFFKEY.sbcgn=1.241;    //(15) corr.factor CF (EdepG4/CF) to correct too high signal in G4  
  ECMCFFKEY.pedh=150;       //(16)Ped-HiCh    
  ECMCFFKEY.pedvh=30;       //(17)ch-to-ch variation(%)     
  ECMCFFKEY.pedl=150;       //(18)Ped-LoCh    
  ECMCFFKEY.pedvl=30;       //(19)ch-to-ch variations(%)
  ECMCFFKEY.pedsh=0.55;     //(20)PedSig-HiCh     
  ECMCFFKEY.pedsvh=30;      //(21)ch-to-ch variation(%)     
  ECMCFFKEY.pedsl=0.55;     //(22)PedSig-LoCh    
  ECMCFFKEY.pedsvl=30;      //(23)ch-to-ch variation(%)
  ECMCFFKEY.pedd=150;       //(24)Ped-DyCh    
  ECMCFFKEY.peddv=30;       //(25)ch-to-ch variation(%)     
  ECMCFFKEY.pedds=0.55;     //(26)PedSig-DyCh    
  ECMCFFKEY.peddsv=30;      //(27)ch-to-ch variation(%)
//     
  ECMCFFKEY.ReadConstFiles=0;//(28)CP=CalibrMCSeeds|MCPeds:C=1/0->Read MSCalibFile/DB
//                                          P=1/0->ReadFromFile/ReadFromDB
  ECMCFFKEY.calvern=2;//(29)EcalCflistMC-file vers.number(keep RlgaMC(SD),FiatMC(SD),AnorMC-calib.files vers#)
//
  ECMCFFKEY.mch2root=0;//(30) =1 to write ECmc-hits to root file when 'All' requested, =2 to write in any case
  //
FFKEY("ECMC",(float*)&ECMCFFKEY,sizeof(ECMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//---------------------------
void AMSJob::_reecaldata(){
  ECREFFKEY.reprtf[0]=0;   // (1) print flag (0/1/2->no/summ/+hist)
  ECREFFKEY.reprtf[1]=0;   // (2) print_profile flag (0/1->no/yes)
  ECREFFKEY.reprtf[2]=0;   // (3) DAQ-debug prints: 0/1/2/>2 (nodebug/Errors/fulldebug)
//
  ECREFFKEY.relogic[0]=1;  // (4) 1/0->write/not EcalHits into Ntuple
  ECREFFKEY.relogic[1]=0;  // (5) 0/1/2/3/4/5->norm/RLGA/RLGA+FIAT/ANOR/PedClassic/PedDowdScaled
  ECREFFKEY.relogic[2]=0;  // (6) 0/1/2->multipl/Ebackgr/Both_type cuts in ANOR_calib logic
  ECREFFKEY.relogic[3]=0;  // (7) 
  ECREFFKEY.relogic[4]=0;  // (8) spare 
//
// Run-time DAQ/trig/RECO-thresholds/cuts(time dependent):
  ECREFFKEY.thresh[0]=3.;     // (9)  Anode(High/low-gain chan) readout threshold(in sigmas)
  ECREFFKEY.thresh[1]=120.;   // (10) Etot "mip"-trig.thresh(mev tempor)
  ECREFFKEY.thresh[2]=0.;     // (11) spare
  ECREFFKEY.thresh[3]=0.;     // (12) spare
  ECREFFKEY.thresh[4]=3.;     // (13) Dynode chan. readout thershold(in sigmas)
  ECREFFKEY.thresh[5]=1.;     // (14) Trig. PMDynThr sl1(y) (in mev from S. Di Falco)
  ECREFFKEY.thresh[6]=80.;    // (15) (was 70)Trig. PMDynThr sl2(x) ...
  ECREFFKEY.thresh[7]=180.;   // (16) (was 100)Trig. PMDynThr sl3(y)
  ECREFFKEY.thresh[8]=140.;   // (17) (was 80)Trig. PMDynThr sl4(x)
  ECREFFKEY.thresh[9]=60.;    // (18) (was 60)Trig. PMDynThr sl5(y)
  ECREFFKEY.thresh[10]=100.;  // (19) (was 40)Trig. PMDynThr sl6(x)
  ECREFFKEY.thresh[11]=100.;  // (20) (was 40)Trig. PMDynThr sl7(y)
  ECREFFKEY.thresh[12]=1.;    // (21) Trig. PMDynThr sl8(x) 
  ECREFFKEY.thresh[13]=1.;    // (22) Trig. PMDynThr sl9(y) 
  ECREFFKEY.thresh[14]=0.;    // (23) spare
// 
  ECREFFKEY.cuts[0]=1.15;   // (24) Pisa Trig-algorithm: AngleCut1 
  ECREFFKEY.cuts[1]=2.15;   // (25)                      AngleCut1
  ECREFFKEY.cuts[2]=5.;     // (26)                      Xmult-boundary
  ECREFFKEY.cuts[3]=5.;     // (27)                      Ymult-boundary
  ECREFFKEY.cuts[4]=2.;     // (28) min.layers(out of 3 per proj) with at least 1 pm having Adynode > thr
  ECREFFKEY.cuts[5]=0.;     // (29)
  ECREFFKEY.cuts[6]=0.;     // (30)
  ECREFFKEY.cuts[7]=0.;     // (31)
  ECREFFKEY.cuts[8]=0.;     // (32)
  ECREFFKEY.cuts[9]=0.65;   // (33) LVL3-trig. EC-algorithm: "peak"/"average" methode boundary
//
  ECREFFKEY.ReadConstFiles=100;//(34)DCP (ThreshCuts-set | Calib.const(MC/RD) | RDpeds)
//                            D=1/0-> Take from DataCards/DB
//                            C=1/0-> Take from CalibFiles/DB
//                            P=1/0-> Take from CalibFiles/DB
//  ECREFFKEY.calutc=1167606001;//(35)(20070101 0000001)EcalCflistRD-file(rlga,fiat,anor-files utc-list) begin UTC-time
  ECREFFKEY.calutc=1199142001;//(35)(20080101 ~0000001)EcalCflistRD-file(rlga,fiat,anor-files utc-list) begin UTC-time
//
  ECREFFKEY.Thr1DSeed=3;//(36) this and below is for Vitali's clust. algorithm
  ECREFFKEY.Thr1DRSeed=0.05;
  ECREFFKEY.Cl1DLeakSize=9;
  ECREFFKEY.Cl1DCoreSize=2;
  ECREFFKEY.Thr2DMax=1.2;  //max tan(theta)
  ECREFFKEY.Length2DMin=3;
  ECREFFKEY.Chi22DMax=1000;
  ECREFFKEY.PosError1D=0.1;
  ECREFFKEY.Chi2Change2D=0.33;
  ECREFFKEY.TransShowerSize2D=10;
  ECREFFKEY.SimpleRearLeak[0]=-0.015;
  ECREFFKEY.SimpleRearLeak[1]=0.952e-3;
  ECREFFKEY.SimpleRearLeak[2]=3.1;
  ECREFFKEY.SimpleRearLeak[3]=0.9984e-3;  //  ==  [1]/(1-abs([0])*[2])
  ECREFFKEY.CalorTransSize=32;
  ECREFFKEY.EMDirCorrection=1.03;
  ECREFFKEY.HiEnThr=1650;
  ECREFFKEY.HiEnCorFac=0.225;
  ECREFFKEY.S1S3[0]=0.6;
  ECREFFKEY.S1S3[1]=0.25;
  ECREFFKEY.S1S3[2]=0.4;
  ECREFFKEY.S1S3[3]=0.8;
//
//  
  ECREFFKEY.sec[0]=0;//54 
  ECREFFKEY.sec[1]=0;//
  ECREFFKEY.min[0]=0;//
  ECREFFKEY.min[1]=0;//
  ECREFFKEY.hour[0]=0;//
  ECREFFKEY.hour[1]=0;//
  ECREFFKEY.day[0]=1;//
  ECREFFKEY.day[1]=1;//
  ECREFFKEY.mon[0]=0;//
  ECREFFKEY.mon[1]=0;//
  ECREFFKEY.year[0]=108;//64
  ECREFFKEY.year[1]=125;//65
FFKEY("ECRE",(float*)&ECREFFKEY,sizeof(ECREFFKEY_DEF)/sizeof(integer),"MIXED");
//
// REUN-Calibration  parameters:
// RLGA/FIAT part:
  ECCAFFKEY.hprintf=2;    // (1) calib-related hist.print-flag(0/1/2/->no/hi_import/+mid/+low_import hist)
  ECCAFFKEY.cafdir=0;     // (2) 0/1-> use official/private directory for calibr.files
  ECCAFFKEY.prtuse=-1;    // (3) 1/0/-1 -> use He4/proton/mu tracks for calibration
  ECCAFFKEY.refpid=118;   // (4) ref.pm ID (SPP-> S=SupLayer, PP=PM number) 
  ECCAFFKEY.trmin=4.;     // (5) presel-cut on min. rigidity of the track(gv) 
  ECCAFFKEY.adcmin=2.;    // (6) min ADC cut (~3sig)for indiv. SubCell (to remove noise)
  ECCAFFKEY.adcpmx=150.;  // (7) max ADC for indiv SC to consider PlaneAmpl as PunchThrough(prot)
  ECCAFFKEY.ntruncl=2;    // (8) remove this number of PixPlanes with highest Edep
  ECCAFFKEY.trxac=0.022;  // (9) TRK->EC extrapolation accuracy in X-proj(cm)
  ECCAFFKEY.tryac=0.019;  //(10) TRK->EC extrapolation accuracy in Y-proj............
  ECCAFFKEY.mscatp=1.;    //(11) EC mult.scatt. fine tuning parameter
  ECCAFFKEY.nortyp=0;     //(12) PM-resp. normaliz.type 0/1-> by crossed/fired tracks
  ECCAFFKEY.badplmx=4;    //(13) Accept max. bad Pix-planes(>2 fired pix/lay, high pix Ed, separated 2 pixels)
  ECCAFFKEY.etrunmn=40.;  //(14) Min ECenergy (Etrunc/sl in mev) to select PunchThrough particle(He)
  ECCAFFKEY.etrunmx=270.; //(15) Max ECenergy (Etrunc/sl in mev) ...................................
  ECCAFFKEY.nsigtrk=1.;   //(16) ImpPoint accur. extention param. for cell-crossing check
// ANOR part:
  ECCAFFKEY.pmin=3.;        // (17) presel-cut on min. mom. of the track(gev/c) 
  ECCAFFKEY.pmax=15.;       // (18) presel-cut on max. mom. of the track 
  ECCAFFKEY.scmin=2.;       // (19) min ADC for indiv. SubCell (to remove ped,noise)
  ECCAFFKEY.scmax=1500.;    // (20) max ADC .................. (to remove sparks,ovfl,...)
  ECCAFFKEY.spikmx=0;       // (21) max SC's(spikes) with ADC>max  (to remove sparks,ovfl,...)
  ECCAFFKEY.nhtlmx[0]=3;    // (22) max hits in 1st sc-plane (to remove early showering)
  ECCAFFKEY.nhtlmx[1]=4;    // (23) max hits in 2nd sc-plane (to remove early showering)
  ECCAFFKEY.nhtlmx[2]=4;    // (24) max hits in 3rd sc-plane (to remove early showering)
  ECCAFFKEY.nhtlmx[3]=5;    // (25) max hits in 4th sc-plane (to remove early showering)
  ECCAFFKEY.nhtlmx[4]=7;    // (26) max hits in 5th sc-plane (to remove early showering)
  ECCAFFKEY.nhtlmx[5]=7;    // (27) max hits in 6th sc-plane (to remove early showering)
  ECCAFFKEY.lmulmx=20;      // (28) max hits/sc-plane (to remove events with abn.multiplicity)
  ECCAFFKEY.nholmx[0]=2;    // (29) max holes(betw.fired cells) in 1st sc-plane(early show.prot)
  ECCAFFKEY.nholmx[1]=2;    // (30) max holes(betw.fired cells) in 2nd sc-plane(early show.prot)
  ECCAFFKEY.nholmx[2]=3;    // (31) max holes(betw.fired cells) in 3rd sc-plane(early show.prot)
  ECCAFFKEY.nholmx[3]=3;    // (32) max holes(betw.fired cells) in 4th sc-plane(early show.prot)
  ECCAFFKEY.nholmx[4]=3;    // (33) max holes(betw.fired cells) in 5th sc-plane(early show.prot)
  ECCAFFKEY.nholmx[5]=4;    // (34) max holes(betw.fired cells) in 6th sc-plane(early show.prot)
  ECCAFFKEY.nbplmx=0;       // (35) max bad sc-planes (with spikes or high multiplicity)
  ECCAFFKEY.edtmin=800.;    // (36) min Etot(mev) to remove MIP
  ECCAFFKEY.esleakmx=0.01;  // (37) max Eleak(side)/Etot to remove energy side leak
  ECCAFFKEY.ebleakmx=0.02;  // (38) max Eleak(back)/Etot
  ECCAFFKEY.edfrmn=300.;    // (39) min Efront(mev)
  ECCAFFKEY.edt2pmx=0.2;    // (40) max Etail/Epeak
  ECCAFFKEY.ed2momc=0.4;    // (41) Edep(EC)/Mom(TRK)-1 cut
  ECCAFFKEY.cog1cut=0.6;    // (42) Track-SCPlaneCOG mismatch cut(cm) for the 1st 6 SC-planes.
  ECCAFFKEY.scdismx[0]=0.8; // (43) max sc-track dist. to consider hit as backgroubd(pl-1) 
  ECCAFFKEY.scdismx[1]=1.;  // (44) max sc-track dist. to consider hit as backgroubd(pl-2) 
  ECCAFFKEY.scdismx[2]=1.6; // (45) max sc-track dist. to consider hit as backgroubd(pl-3)
  ECCAFFKEY.scdismx[3]=1.8; // (46) max sc-track dist. to consider hit as backgroubd(pl-4)
  ECCAFFKEY.scdismx[4]=3.;  // (47) max sc-track dist. to consider hit as backgroubd(pl-5)
  ECCAFFKEY.scdismx[5]=3.2; // (48) max sc-track dist. to consider hit as backgroubd(pl-6)
  ECCAFFKEY.scdisrs=8.;     // (49) as above for all other planes(not used really)
  ECCAFFKEY.b2scut[0]=0.1;  // (50) max backgr/signal energy(bound.from above) for pl-1 
  ECCAFFKEY.b2scut[1]=0.1;  // (51) max backgr/signal energy(bound.from above) for pl-2 
  ECCAFFKEY.b2scut[2]=0.2;  // (52) max backgr/signal energy(bound.from above) for pl-3 
  ECCAFFKEY.b2scut[3]=0.2;  // (53) max backgr/signal energy(bound.from above) for pl-4 
  ECCAFFKEY.b2scut[4]=0.15; // (54) max backgr/signal energy(bound.from above) for pl-5 
  ECCAFFKEY.b2scut[5]=0.15; // (55) max backgr/signal energy(bound.from above) for pl-6
// PedCalib:
  ECCAFFKEY.pedcpr=0.04;    // (56) PedCal : def portion of highest amplitudes to remove for ped/sig calc
  ECCAFFKEY.pedoutf=2;      // (57)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB(if AMSJ 87=1)/PedWr2File+Hist
  ECCAFFKEY.pedlim[0]=2.;  // (58) Ped low-lim in PedCalibJobs
  ECCAFFKEY.pedlim[1]=800.; // (59)      hi-lim ...............
  ECCAFFKEY.siglim[0]=0.1;  // (60) PedSig low-lim ............
  ECCAFFKEY.siglim[1]=10.; //  (61)         hi-lim ............
//g.chen
  ECCAFFKEY.ecshswit=0;        // (62) 0/1 switch to/(not to) use shower info in ANOR calibration
  ECCAFFKEY.chi2dirmx=10.;     //     max chi2 of shower dir fit
  ECCAFFKEY.prchi2mx=5.;       //     max chi2 of shower profile fit
  ECCAFFKEY.trchi2mx=10.;      //     max chi2 of shower trans. fit
  ECCAFFKEY.eshsleakmx=0.01;   //     max Eleak(shower side)/Etot
  ECCAFFKEY.eshrleakmx=0.05;;  //     max Eleak(shower back)/Etot
  ECCAFFKEY.eshdleakmx=0.02;   //     max Eleak(shower dead)/Etot
  ECCAFFKEY.esholeakmx=0.1;;   //     max Eleak(shower orph)/Etot
  ECCAFFKEY.difsummx=0.2;      //     cut of (E_x-E_y)/(E_x+E_y)
  ECCAFFKEY.trentmax[0]=0.5;   //     cut of distance between track and shower entry x-proj
  ECCAFFKEY.trentmax[1]=0.5;   //     cut of distance between track and shower entry y-proj
  ECCAFFKEY.trextmax[0]=1.0;   //     cut of distance between track and shower enit x-proj
  ECCAFFKEY.trextmax[1]=1.0;   // (74) cut of distance between track and shower exit y-proj
// for OnBoardPeds:
  ECCAFFKEY.onbpedspat=11; // (75) ijkl(binary bit-patt for peds|dynampeds|threshs|widths (msb->lsb) sections in table),
//                                i(j,..)-bitset => section present
//
FFKEY("ECCA",(float*)&ECCAFFKEY,sizeof(ECCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
//
void AMSJob::_sianti2data(){
  ATGEFFKEY.scradi=54.55;   // internal radious of ANTI sc. cylinder (cm)
  ATGEFFKEY.scinth=0.8;     // thickness of scintillator (cm)
  ATGEFFKEY.scleng=82.65;  //(ams02) scintillator paddle length (glob. Z-dim)
  ATGEFFKEY.wrapth=0.04;   // wrapper thickness (cm)
  ATGEFFKEY.groovr=0.4;   // groove radious (bump_rad = groove_rad-pdlgap)
  ATGEFFKEY.pdlgap=0.1;   // inter paddle gap (cm)(2*wrapth+extra)
  ATGEFFKEY.stradi=54.235; // inner radious of supp. tube
  ATGEFFKEY.stleng=85.;    // length of supp. tube
  ATGEFFKEY.stthic=0.12;   // thickness of supp. tube
  
//---
  ATMCFFKEY.mcprtf=0;//(1)print-flag(0/1/2/3->print:no/histogr/PulseSh_arr/print_pulse)
  ATMCFFKEY.FTdel=10.;// (2)aver FT-delay between JLV1 and S-crate(ns)
  ATMCFFKEY.LSpeed=14.7;// (3)Eff. light speed in anti-paddle (cm/ns)
  ATMCFFKEY.ReadConstFiles=0;//(4)Seedp|Realp (Seed|Real MCPeds), S,R=0/1-> read from DB/RawFiles
  ATMCFFKEY.calvern=1;//(5)AccCflistMC-file(acccal_files vers. list) version number
//---
  FFKEY("ATGE",(float*)&ATGEFFKEY,sizeof(ATGEFFKEY_DEF)/sizeof(integer),
  "MIXED");
  FFKEY("ATMC",(float*)&ATMCFFKEY,sizeof(ATMCFFKEY_DEF)/sizeof(integer),
  "MIXED");
}
//================================================================================

void AMSJob::_sitrddata(){
TRDMCFFKEY.mode=-1;
TRDMCFFKEY.g3trd=123654;
TRDMCFFKEY.cor=0.68;
TRDMCFFKEY.alpha=0;
TRDMCFFKEY.beta=0;
TRDMCFFKEY.gain=1;
TRDMCFFKEY.f2i=8;
TRDMCFFKEY.adcoverflow=4095;
TRDMCFFKEY.ped=355;
TRDMCFFKEY.pedsig=102;
TRDMCFFKEY.sigma=6;
TRDMCFFKEY.cmn=15;
TRDMCFFKEY.NoiseOn=1;
//TRDMCFFKEY.GeV2ADC=1.e8;
// changed vc 22-dec-2009
TRDMCFFKEY.GeV2ADC=1.e8/3;
TRDMCFFKEY.Thr1R=3.5;
TRDMCFFKEY.sec[0]=0;
TRDMCFFKEY.sec[1]=0;
TRDMCFFKEY.min[0]=0;
TRDMCFFKEY.min[1]=0;
TRDMCFFKEY.hour[0]=0;
TRDMCFFKEY.hour[1]=0;
TRDMCFFKEY.day[0]=1;
TRDMCFFKEY.day[1]=1;
TRDMCFFKEY.mon[0]=0;
TRDMCFFKEY.mon[1]=0;
TRDMCFFKEY.year[0]=101;
TRDMCFFKEY.year[1]=120;
TRDMCFFKEY.GenerateConst=0;
TRDMCFFKEY.NoiseLevel=0.01;
TRDMCFFKEY.DeadLevel=0.;
TRDMCFFKEY.multiples=0;
 TRDMCFFKEY.Gdens=0.00446;
 TRDMCFFKEY.Pecut=-0.65;
 TRDMCFFKEY.ntrcor=1.3;
 TRDMCFFKEY.etrcor=1.2;
 TRDMCFFKEY.ndecor=1.0;
 TRDMCFFKEY.edecor=1.0;
 TRDMCFFKEY.Tmean=293.15;
 TRDMCFFKEY.Pmean=1080;
 TRDMCFFKEY.FibreDiam=18.;
 TRDMCFFKEY.RadThick=20.;
 TRDMCFFKEY.AlphaFibre=100;
 TRDMCFFKEY.AlphaGas=100;
 TRDMCFFKEY.FleeceGas=0;
 TRDMCFFKEY.XenonFraction=0.8;
 TRDMCFFKEY.ArgonFraction=0.;
 TRDMCFFKEY.CO2Fraction=0.2;
 TRDMCFFKEY.GasStep=6.;
 TRDMCFFKEY.PAIModel=1;
 TRDMCFFKEY.debug=0;

FFKEY("TRDMC",(float*)&TRDMCFFKEY,sizeof(TRDMCFFKEY_DEF)/sizeof(integer),"MIXED");

TRDRECFFKEY.Chi2StrLine=5;
TRDRECFFKEY.ResCutStrLine=1;
TRDRECFFKEY.SearchRegStrLine=3;
 TRDRECFFKEY.CCAmpCut=6.;
FFKEY("TRDRE",(float*)&TRDRECFFKEY,sizeof(TRDRECFFKEY_DEF)/sizeof(integer),"MIXED");

TRDFITFFKEY.Chi2StrLine=3;
TRDFITFFKEY.ResCutStrLine=1;
TRDFITFFKEY.SearchRegStrLine=3;
TRDFITFFKEY.MinFitPoints=6;
TRDFITFFKEY.TwoSegMatch=0.1;
TRDFITFFKEY.MaxSegAllowed=50;
TRDFITFFKEY.FitMethod=2;
TRDFITFFKEY.SaveHistos=0;
TRDFITFFKEY.CalStartVal=0.;
FFKEY("TRDFI",(float*)&TRDFITFFKEY,sizeof(TRDFITFFKEY_DEF)/sizeof(integer),"MIXED");

TRDCLFFKEY.ADC2KeV=1.e6/TRDMCFFKEY.GeV2ADC/TRDMCFFKEY.gain;
TRDCLFFKEY.Thr1S=0.11*3; //VC
TRDCLFFKEY.Thr1A=0.13;
TRDCLFFKEY.Thr1R=7;
TRDCLFFKEY.Thr1H=5.9;
TRDCLFFKEY.MaxHitsInCluster=3;
LVL3FFKEY.TRDHMulThr=TRDCLFFKEY.Thr1H/TRDCLFFKEY.ADC2KeV*TRDMCFFKEY.f2i;
LVL3FFKEY.TRDHMulPart=0.249;
FFKEY("TRDCL",(float*)&TRDCLFFKEY,sizeof(TRDCLFFKEY_DEF)/sizeof(integer),"MIXED");


TRDCALIB.CalibProcedureNo=0;
TRDCALIB.EventsPerCheck=2000;
TRDCALIB.PedAccRequired=0.01;
TRDCALIB.Validity[0]=1;
TRDCALIB.Validity[1]=86400;
TRDCALIB.BadChanThr=3.3;
 FFKEY("TRDALIB",(float*)&TRDCALIB,sizeof(TRDCALIB_DEF)/sizeof(integer),"MIXED");


}

void AMSJob::_sisrddata(){
}

void AMSJob:: _reamsdata(){
  CALIB.InsertTimeProc=1;  // Insert Time by now
  CALIB.Ntuple=0;
  CALIB.SubDetInCalib=11111;//SubDets selection for proc. of OnBoard-calib data(msb->lsb =>trd|tof+acc|trk|rich|ec)
 FFKEY("CALIB",(float*)&CALIB,sizeof(CALIB_DEF)/sizeof(integer),"MIXED");


_remfdata();
_retkdata();

_retof2data();
_reanti2data();
  
_reecaldata();
_retrddata();
_resrddata();
_reaxdata();
_redaqdata();
}

void AMSJob::_remfdata(){
//PZMAG #ifdef _PGTRACK_
//   TKFIELD.init();
// #else
TKFIELD.iniok=1;
VBLANK(TKFIELD.mfile,40);
TKFIELD.isec[0]=0;
TKFIELD.isec[1]=0;
TKFIELD.imin[0]=0;
TKFIELD.imin[1]=0;
TKFIELD.ihour[0]=0;
TKFIELD.ihour[1]=0;
TKFIELD.imon[0]=0;
TKFIELD.imon[1]=0;
TKFIELD.iyear[0]=0;
TKFIELD.iyear[1]=0;
//#endif
FFKEY("BMAP",(float*)&TKFIELD,60,"MIXED");

}
#ifndef _PGTRACK_
void AMSJob::_retkdata(){

//number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
TRCLFFKEY.ThrClA[0]=12.;
TRCLFFKEY.ThrClA[1]=12.;

TRCLFFKEY.ThrClS[0]=20;
TRCLFFKEY.ThrClS[1]=15;

TRCLFFKEY.ThrClR[0]=3.5;
TRCLFFKEY.ThrClR[1]=4.;

TRCLFFKEY.Thr1A[0] =8.;
TRCLFFKEY.Thr1A[1] =8.;

TRCLFFKEY.Thr2A[0] =4.;
TRCLFFKEY.Thr2A[1] =3.;

TRCLFFKEY.Thr1S[0] =20;
TRCLFFKEY.Thr1S[1] =15;

TRCLFFKEY.Thr2S[0] =15;
TRCLFFKEY.Thr2S[1] =10;

TRCLFFKEY.Thr1R[0] =-2.75;
TRCLFFKEY.Thr1R[1] =3.5;

TRCLFFKEY.ThrClS[0]=20;
TRCLFFKEY.Thr1S[0] =15;
TRCLFFKEY.Thr2S[0] =15;

TRCLFFKEY.Thr2R[0] =1.;
TRCLFFKEY.Thr2R[1] =3.1;  // should be around 1 if ThrClNEl[1]=3;
                         // should be around 3 if ThrClNEl[1]=5;
TRCLFFKEY.Thr3R[0] =-2.;
TRCLFFKEY.Thr3R[1] =-2.;

TRCLFFKEY.ThrClNMin[0]=1;
TRCLFFKEY.ThrClNMin[1]=1;

TRCLFFKEY.ThrClNEl[0]=3;
TRCLFFKEY.ThrClNEl[1]=5;

TRCLFFKEY.ErrX=30.e-4;
TRCLFFKEY.ErrY=16.e-4;
TRCLFFKEY.ErrZ=30.e-4;
TRCLFFKEY.ThrDSide=1.;

TRCLFFKEY.CorFunParA[0][0]=400e-4;
//TRCLFFKEY.CorFunParB[0][0]=0.85;
TRCLFFKEY.CorFunParB[0][0]=1.;
TRCLFFKEY.CorFunParA[0][5]=400e-4;
TRCLFFKEY.CorFunParB[0][5]=1.;
int k;
for (k=1;k<5;k++){
 TRCLFFKEY.CorFunParA[0][k]=400e-4;
 TRCLFFKEY.CorFunParB[0][k]=1.;
}
for ( k=0;k<6;k++){
 TRCLFFKEY.CorFunParA[1][k]=65e-4;
 // TRCLFFKEY.CorFunParB[1][k]=0.5;
 TRCLFFKEY.CorFunParB[1][k]=1.;
}

for(k=0;k<6;k++){
  for(int l=0;l<3;l++){
   TRCLFFKEY.ResFunX[l][k]=0;
   TRCLFFKEY.ResFunY[l][k]=0;
  }
}
  TRCLFFKEY.CommonGain[0]=1.;
  TRCLFFKEY.CommonGain[1]=1.;
  TRCLFFKEY.CommonShift[0]=0.;
  TRCLFFKEY.CommonShift[1]=0.;

  TRCLFFKEY.EtaCor[0]=1;
  TRCLFFKEY.EtaCor[1]=0;


FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");

// Fit Par
{
  for( int k=0;k<sizeof(TRFITFFKEY.patternp)/sizeof(TRFITFFKEY.patternp[0]);k++)TRFITFFKEY.patternp[k]=0;
}
TRFITFFKEY.MainAlg=541;
TRFITFFKEY.B23[0]=30;
TRFITFFKEY.B23[1]=20;
TRFITFFKEY.UseTRD=1;
TRFITFFKEY.UseTOF=2;
TRFITFFKEY.Chi2FastFit=2000;
TRFITFFKEY.Chi2StrLine=100;
TRFITFFKEY.Chi2WithoutMS=50;
TRFITFFKEY.ResCutFastFit=0.5;
TRFITFFKEY.ResCutStrLine=0.5;
TRFITFFKEY.ResCutCircle=0.5;
TRFITFFKEY.SearchRegFastFit=1;
TRFITFFKEY.SearchRegStrLine=1.5;
TRFITFFKEY.SearchRegCircle=10.;
TRFITFFKEY.RidgidityMin=0.02;
TRFITFFKEY.FullReco=0;
TRFITFFKEY.MinRefitCos[0]=0.7;
TRFITFFKEY.MinRefitCos[1]=0.5;
TRFITFFKEY.FastTracking=1;
TRFITFFKEY.WeakTracking=0;
TRFITFFKEY.FalseXTracking=1;
TRFITFFKEY.Chi2FalseX=3.;
TRFITFFKEY.ForceFalseTOFX=1;
TRFITFFKEY.FalseTOFXTracking=1;
TRFITFFKEY.TOFTracking=1;
TRFITFFKEY.ForceAdvancedFit=1;
TRFITFFKEY.ThrClA[0]=0.;
TRFITFFKEY.ThrClA[1]=0.;
TRFITFFKEY.ThrClR[0]=0.;
TRFITFFKEY.ThrClR[1]=0.;
TRFITFFKEY.MaxTrRecHitsPerLayer=250;
TRFITFFKEY.LowMargin=1;
TRFITFFKEY.OnlyGammaVtx=0;
TRFITFFKEY.UseGeaneFitting=0;
TRFITFFKEY.OldTracking=0;
TRFITFFKEY.AddMS=1;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}
#endif
//=================================================================================
//
void AMSJob::_retof2data(){
// 
  TFREFFKEY.Thr1=0.2;//(1) Bar threshold (mev) in tof-cluster algorithm
  TFREFFKEY.ThrS=0.2; //(2) Threshold (mev) on total cluster energy(Not used now!)
//
  TFREFFKEY.reprtf[0]=0; //(3) RECO print flag for statistics(2/1/0->big/small/no print) 
  TFREFFKEY.reprtf[1]=0; //(4) print flag for histograms
  TFREFFKEY.reprtf[2]=0; //(5) print flag for TDC-hit multiplicity histograms 
  TFREFFKEY.reprtf[3]=0; //(6) print flag for Debug needs(mostly when error found, for ev-by-ev debug add #7)  
  TFREFFKEY.reprtf[4]=0; //(7) More debug & histogr flag(1->more)
//
  TFREFFKEY.relogic[0]=0;//(8) 0/1/2/3/4/5/6/7 ->normal/TDCL/TDIF/TZSL/AMPL/PEDScl/ds/OnBoardTable-calibr. run.
//                       (when =7 confirm ped-file writing + hist when glob. CALIB-flag >1 to be independent on ACC !) 
  TFREFFKEY.relogic[1]=1;//(9) 1/0-> permit/not LT/SumHT matching criterion for multi-LTs case 
  TFREFFKEY.relogic[2]=1;//(10) 1/0->use/not TofTdc NonLin-corrections at RECO-stage(RawClust creation)
  TFREFFKEY.relogic[3]=0;//(11) 1/0->Do/not recovering of missing side 
  TFREFFKEY.relogic[4]=1;//(12) 1/0->write/not TOF2RawSideObject-info into ntuple
//
  TFREFFKEY.daqthr[0]=50.;//(13)tempor Anode low discr.thresh(LT) for fine-time TDC 
  TFREFFKEY.daqthr[1]=80.;//(14)tempor Anode high discr.thresh(HT) for FT-trigger TDC(z>=1)  
  TFREFFKEY.daqthr[2]=940.;//(15)tempor Anode superhigh discr.thresh(SHT=940mV) for "z>=2"-trig(50% of He-mip)  
  TFREFFKEY.daqthr[3]=4.;//(16) Anode-ADC-readout threshold in DAQ (in PedSigmas)    
  TFREFFKEY.daqthr[4]=4.;//(17) Dynode-ADC-readout threshold in DAQ (in PedSigmas)
//
  TFREFFKEY.cuts[0]=6.;//(18) window(+-ns) around (LT-SumHT)-m.p for pairing of LT-/sumHT-hits in channels
  TFREFFKEY.cuts[1]=2000.;//(19)"befor"-cut in time history (ns)(max. integr.time?)
  TFREFFKEY.cuts[2]=100.;//(20)"after"-cut in time history (ns)
  TFREFFKEY.cuts[3]=3.33;//(21) error(cm) in longitudinal coordinate (for mip in single TOF bar)
  TFREFFKEY.cuts[4]=70.;//(22) JLV1 globFT decision_time + delay(JLV1-> S-crate, ns), i.e. glFT-time at S-crate
  TFREFFKEY.cuts[5]=5.;//(23) (LT-SumHT)-m.p to use with window(cuts[0])  for pairing of LT-/sumHT-hits in channels
  TFREFFKEY.cuts[6]=0.6;//(24) 2-bars assim.cut in TOFCluster energy calculation
  TFREFFKEY.cuts[7]=10.;// (25) T-type def.temperature (see card #29)
  TFREFFKEY.cuts[8]=5.;// (26) P-type def.temperature 
  TFREFFKEY.cuts[9]=8.;// (27) C-type def.temperature
//
  TFREFFKEY.ReadConstFiles=100;//(28) LQDPC(L->TDCLinCorCalib(mc/rd);Q->ChargeCalib(mc/rd),
//                                           D->ThrCuts-set(datacards),P->Peds(rd),C->CalibConst(rd/mc));
// L=1/0->Take TofTdcLinearityCorrections from RawFiles/DB
// Q=1/0->Take ChargeCalibDensFunctions from RawFiles/DB
// D=1/0->Take ThreshCuts-set from DataCards/DB,
// P=1/0->Take Peds(rd) from RawFiles/DB,
// C=1/0->Take PaddleCalibrConst from RawFiles/DB
//
  TFREFFKEY.TempHandlMode=0;//(29)CPT(Slow:C->SFEC,P->PMT; Fast:T->SFET)=> Temperature Handling Mode 
//                               C(P,T)=1/0->do/not corresponding temper-corrections at reco-stage
  TFREFFKEY.calutc=1167606001;//(30)(20070101 0000001)TofCflistRD-file(barcal_files vers. list) begin UTC-time
//
  TFREFFKEY.sec[0]=0;//(31) 
  TFREFFKEY.sec[1]=0;
  TFREFFKEY.min[0]=0;
  TFREFFKEY.min[1]=0;
  TFREFFKEY.hour[0]=0;
  TFREFFKEY.hour[1]=0;
  TFREFFKEY.day[0]=1;
  TFREFFKEY.day[1]=1;
  TFREFFKEY.mon[0]=0;
  TFREFFKEY.mon[1]=0;
  TFREFFKEY.year[0]=101;//(41)(since year 1900)
  TFREFFKEY.year[1]=125;//(42)
  FFKEY("TFRE",(float*)&TFREFFKEY,sizeof(TFREFFKEY_DEF)/sizeof(integer),
  "MIXED");

//    defaults for calibration:
// TZSL-calibration:
  TFCAFFKEY.pcut[0]=5.;// (1)track mom. low limit (gev/c) (prot, put 0.75 for mu)
  TFCAFFKEY.pcut[1]=100.;// (2)track mom. high limit
  TFCAFFKEY.bmeanpr=0.996;// (3)mean prot. velocity in above range
  TFCAFFKEY.tzref[0]=0.;//(4)T0 for ref. counter
  TFCAFFKEY.tzref[1]=0.;//(5) spare
  TFCAFFKEY.fixsl=7.0;// (6)def. slope
  TFCAFFKEY.bmeanmu=0.994;// (7)mean muon velocity at sea-level
  TFCAFFKEY.idref[0]=104;//(8)LBB for  ref. layer/counter(LBB, set L=2 if layer 1 is dead) 
  TFCAFFKEY.idref[1]=0;//(9)0/1/2->FitAll/IgnorTrapezCount/FitTrapezCount&FixOthers
  TFCAFFKEY.ifsl=1;//(10) 0/1 to fix/release slope param.
//
  TFCAFFKEY.caltyp=0;// (11) 0/1->space/earth calibration
//
  TFCAFFKEY.truse=1;//(12)-1/0/1->(req.TRK|TRD-track,no mom.check)/(req.TRK,no Mom.check)/(TRK with mom.check)
// AMPL-calibration:
  TFCAFFKEY.plhc[0]=0.5;// (13) track mom. low limit(gev/c) for space calibr
  TFCAFFKEY.plhc[1]=47.;// (14) track mom. high limit(gev/c) ..............
  TFCAFFKEY.minev=80;// (15)min.events needed for measurement in channel or bin
  TFCAFFKEY.trcut=0.92;// (16) cut to use for "truncated average" calculation
  TFCAFFKEY.spares[0]=0;//(17) if =1 -> special TofPMEquilization run
  TFCAFFKEY.spares[1]=0;//(18) if =1 -> ignore most of the cuts for calib.run(useful when starting from scratch)
  TFCAFFKEY.spares[2]=0;//(19) 0/1-> UseStrictLPatt(4of4)/not(3of4)
  TFCAFFKEY.spares[3]=0;//(20) 0/1->not/ignore BAR timing DB-status (set from Tzslw-results)
  TFCAFFKEY.adc2q=1.;//(21)adc->charge conv.factor(pC/ADCch, hope = for all ADC chips)
  TFCAFFKEY.plhec[0]=0.3;//(22)plow-cut for earth calibration
  TFCAFFKEY.plhec[1]=15.;//(23)phigh-cut ...................
  TFCAFFKEY.bgcut[0]=0.58; //(24) beta*gamma low-cut to be around mip-region(abs.calib)
  TFCAFFKEY.bgcut[1]=50.;//(25) beta*gamma high-cut ..............................
//
  TFCAFFKEY.tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  TFCAFFKEY.tofbetac=0.5;// (27) if nonzero->low beta cut (own TOF measurements !!!)
  TFCAFFKEY.hprintf=1; // (28) calib.histogr. print-flag, =0/>=1 => noprint/print(prior=1,2(low))
//
  TFCAFFKEY.cafdir=0;// (29) 0/1-> use official/private directory for calibr.files
  TFCAFFKEY.mcainc=0;// (30) spare
//TOFPedsCalib:
  TFCAFFKEY.pedcpr[0]=0.01; // (31) PedCalibJobClass: portion of highest adcs to remove for ped-calc
  TFCAFFKEY.pedcpr[1]=0.25;  // (32) PedCalibJobDScal: portion of highest adcs to remove for ped-calc
  TFCAFFKEY.pedoutf=2;      // (33)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB+File/PedWr2File
  TFCAFFKEY.pedlim[0]=10.;  // (34) Ped low-lim in PedCalibJobs
  TFCAFFKEY.pedlim[1]=700.; // (35)      hi-lim ...............
  TFCAFFKEY.siglim[0]=0.4;  // (36) PedSig low-lim ............
  TFCAFFKEY.siglim[1]=10.; //  (37)         hi-lim ............
//TOFTdcCalib:
  TFCAFFKEY.minstat=100;//(38) min.acceptable statistics per channel
  TFCAFFKEY.tdccum=10;//(39)tdc-calib usage mode: MN->M=1/0(Economy mode/norm);N=1/0->write/not final calibfile
//also for OnBoardPeds
  TFCAFFKEY.onbpedspat=47;//(40)ijklmn(bit-patt for peds|dynampeds|pretrs|statws|threshs|widths sections in table),
//                           i(j,..)-bitset => section present (47-> dynam.peds section missing)
  TFCAFFKEY.updbrcaldb=0;// (41) if=1 - update RD barcal DB "on flight"(just after new calib-file was written) 
//
  FFKEY("TFCA",(float*)&TFCAFFKEY,sizeof(TFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//======================================================================
void AMSJob::_reanti2data(){
  ATREFFKEY.reprtf[0]=0;//(1) Reco print flag (0/1/2->no/statistics/+hist)
  ATREFFKEY.reprtf[1]=0;//(2) Debug-print (0/1->no/print)
  ATREFFKEY.relogic=0;  //(3) =0/1/2/3/4->Normal/AbsCal/PedCal_Clas(randTrg)/PedCal_DwnScal(onData)/OnbPed
//                      (when =4 confirm ped-file writing + hist when glob. CALIB-flag >1 to be independent on TOF !) 
  ATREFFKEY.Edthr=0.1;  //(4) threshold to create Cluster(Paddle) object (mev)
  ATREFFKEY.zcerr1=10.; //(5) Err(cm).in longit.coord. when 2-sides times are known 
  ATREFFKEY.nlcorr=0;   //(6) =1/0--> apply/not nonlin.corr to raw amplitudes at cluster-build stage
  ATREFFKEY.ftdel=120.;  //(7) FT-delay wrt correlated Anti history-pulse
  ATREFFKEY.ftwin=70.;  //(8) window for Hist-hit/FT coincidence(+- around FT-delay corrected value)
//
  ATREFFKEY.ReadConstFiles=0;//(9)PVS(RD_Peds,VariabCalibPar(mc/rd),StabCalibPar(mc/rd)), P(V,S)=0/1-> DB/RawFiles
//  
  ATREFFKEY.calutc=1167606001;//(10)(20070101 0000001)AccCflistRD-file(acccal_files vers. list) begin UTC-time
//
  ATREFFKEY.sec[0]=0;//(11) 
  ATREFFKEY.sec[1]=0;//(12)
  ATREFFKEY.min[0]=0;//(13)
  ATREFFKEY.min[1]=0;//(14)
  ATREFFKEY.hour[0]=0;//(15)
  ATREFFKEY.hour[1]=0;//(16)
  ATREFFKEY.day[0]=1;
  ATREFFKEY.day[1]=1;
  ATREFFKEY.mon[0]=0;
  ATREFFKEY.mon[1]=0;
  ATREFFKEY.year[0]=101;//(21)
  ATREFFKEY.year[1]=125;//(22)
  FFKEY("ATRE",(float*)&ATREFFKEY,sizeof(ATREFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  ATCAFFKEY.trackmode=0;// (1) 0/1-> use TRK/TRK+TRD for calib
  ATCAFFKEY.cafdir=0;// (2)  0/1-> use official/private directory for calibr.files
  ATCAFFKEY.pedcpr[0]=0.005; // (3) PedCalibJobRandom(classic): portion of highest adcs to remove
  ATCAFFKEY.pedcpr[1]=0.035;// (4) PedCalibJobDownScaled(in trig): portion of highest adcs to remove
  ATCAFFKEY.pedoutf=2;      // (5)  --//-- outp.flag: 0/1/2-> HistosOnly/PedWr2DB+File/PedWr2File
  ATCAFFKEY.pedlim[0]=10.;  // (6) Ped low-lim in PedCalibJobs
  ATCAFFKEY.pedlim[1]=400.; // (7)      hi-lim ...............
  ATCAFFKEY.siglim[0]=0.4;  // (8) PedSig low-lim ............
  ATCAFFKEY.siglim[1]=50.; //  (9)         hi-lim ............
  ATCAFFKEY.mev2mv=150.;   // (10) spare, not used now
//
  FFKEY("ATCA",(float*)&ATCAFFKEY,sizeof(ATCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_redaqdata(){
DAQCFFKEY.ReadAhead=0;
DAQCFFKEY.mode=0;
DAQCFFKEY.Mode=1;
DAQCFFKEY.DAQVersion=1;
DAQCFFKEY.BTypeInDAQ[0]=0;
DAQCFFKEY.BTypeInDAQ[1]=6;  // 7 & 7 not implemented yet
VBLANK(DAQCFFKEY.ifile,40);
VBLANK(DAQCFFKEY.ofile,40);
VBLANK(DAQCFFKEY.BlocksDir,40);
DAQCFFKEY.SkipRec=0;
  FFKEY("DAQC",(float*)&DAQCFFKEY,sizeof(DAQCFFKEY_DEF)/sizeof(integer),"MIXED");

}

//----------------------------------
void AMSJob::_reaxdata(){
// Fit beta & charge
CHARGEFITFFKEY.NmembMax=3;
CHARGEFITFFKEY.Tracker=1;
CHARGEFITFFKEY.EtaMin[0]=0.05;
CHARGEFITFFKEY.EtaMin[1]=0.00;
CHARGEFITFFKEY.EtaMax[0]=0.95;
CHARGEFITFFKEY.EtaMax[1]=1.00;
CHARGEFITFFKEY.ProbTrkRefit=0.01;
CHARGEFITFFKEY.ResCut[0]=4.;//-1. to switch OFF incomp.clus.exclusion(as for simulation default,
CHARGEFITFFKEY.ResCut[1]=4.;//  see charge.C code with tempor. MC settings
CHARGEFITFFKEY.SigMin=0.1;
CHARGEFITFFKEY.SigMax=0.3;
CHARGEFITFFKEY.PdfNorm=1;//not used now (pdf's are normalized automatically)
CHARGEFITFFKEY.TrMeanRes=1;//(13) use normal(0)/"-incomp.hit"(1)truncated mean
CHARGEFITFFKEY.ProbMin=0.0001;
CHARGEFITFFKEY.TrackerOnly=9;//above this value use Tracker only
CHARGEFITFFKEY.ChrgMaxAnode=9;//not used now
CHARGEFITFFKEY.BetaPowAnode=0;//not used now
CHARGEFITFFKEY.TrackerForceSK=0;//(18)
CHARGEFITFFKEY.TrackerKSRatio=0.67;//(19)
CHARGEFITFFKEY.TrackerProbOnly=9;
CHARGEFITFFKEY.TrkPDFileMCVers=1;//MC vers.number of Trk-ElosPDFFile(trkpdffNNNmc.dat)
CHARGEFITFFKEY.TrkPDFileRDVers=1;//RD vers.number of Trk-ElosPDFFile(trkpdffNNNrl.dat)
CHARGEFITFFKEY.TrkPDFileRead=1;//read TrkPDF-info from DB(0) OR RawFile (1)
CHARGEFITFFKEY.sec[0]=0; 
CHARGEFITFFKEY.sec[1]=0;
CHARGEFITFFKEY.min[0]=0;
CHARGEFITFFKEY.min[1]=0;
CHARGEFITFFKEY.hour[0]=0;
CHARGEFITFFKEY.hour[1]=0;
CHARGEFITFFKEY.day[0]=1;
CHARGEFITFFKEY.day[1]=1;
CHARGEFITFFKEY.mon[0]=0;
CHARGEFITFFKEY.mon[1]=0;
CHARGEFITFFKEY.year[0]=101;
CHARGEFITFFKEY.year[1]=121;
//-------
BETAFITFFKEY.pattern[0]=1;
BETAFITFFKEY.pattern[1]=1;
BETAFITFFKEY.pattern[2]=1;
BETAFITFFKEY.pattern[3]=1;
BETAFITFFKEY.pattern[4]=1;
BETAFITFFKEY.pattern[5]=1;
BETAFITFFKEY.pattern[6]=1;
BETAFITFFKEY.pattern[7]=1;
BETAFITFFKEY.pattern[8]=1;
BETAFITFFKEY.pattern[9]=1;
BETAFITFFKEY.pattern[10]=1;
BETAFITFFKEY.Chi2=50;
BETAFITFFKEY.Chi2S=15;
BETAFITFFKEY.SearchReg[0]=3.;
BETAFITFFKEY.SearchReg[1]=3.;
BETAFITFFKEY.SearchReg[2]=3.;
BETAFITFFKEY.LowBetaThr=0.4;
BETAFITFFKEY.FullReco=0;
BETAFITFFKEY.MassFromBetaRaw=1;
BETAFITFFKEY.OldNew=1;
FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}
void AMSJob::_resrddata(){
}

//-----------------------------

void AMSJob::udata(){

if(DAQCFFKEY.SkipRec){
if(!(DAQCFFKEY.mode/10)%10)DAQCFFKEY.SkipRec=0;
else{
 IOPA.hlun=0;
 IOPA.WriteRoot=0;
}
}


#ifdef _WEBACCESS_
  if (char * pp=getenv("WEBACCESS")){
    cout<<"<---- AMSJob::udata TDV Web Access Enabled with dir "<<pp<<" !!! ;-) "<<endl;
    AMSTimeID::SetWebAccess();
    AMSTimeID::SetWebDir(pp);
  }
#endif

  if(CCFFKEY.StrMass<0){
    CCFFKEY.StrMass=0.938*pow(CCFFKEY.StrCharge/0.3,1.5);
  }
  if(GCKINE.ipart==113){
    cout <<" Stranglet Parameters Are Charge: "<<CCFFKEY.StrCharge<<" Mass: "<<CCFFKEY.StrMass<<endl;
  }


  GCTIME.TIMEND=GCTIME.TIMINT;
  GCTIME.ITIME=0;
  if(!MISCFFKEY.G4On && !MISCFFKEY.G3On){
    cerr<<"<---- AMSJob::udata-F-NeitherGeant3NorGeant4Selected"<<endl;
    exit(1);
  }
  else if(MISCFFKEY.G4On && MISCFFKEY.G3On){
    cerr<<"<---- AMSJob::udata-W-BothGeant3AndGeant4Selected"<<endl;
    exit(1);
  }
  else if(MISCFFKEY.G4On)cout<<"<---- AMSJob::udata-I-Geant4Selected"<<endl<<endl;
  else cout<<"<---- AMSJob::udata-I-Geant3Selected"<<endl<<endl;

  if(MISCFFKEY.BZCorr !=1){
    cout <<"<---- AMSJob::udata-W-magneticFieldRescaleModeOnWithFactor "<<MISCFFKEY.BZCorr<<endl<<endl;
  }

  if(CCFFKEY.Fast){
    GCPHYS.IHADR=0;
    GCPHYS.IMULS=0;
    GCPHYS.ILOSS=4;
    GCPHYS.IPAIR=0;
    GCPHYS.IBREM=0;
    GCPHYS.ICOMP=0;
    GCPHYS.IPHOT=0;
    GCPHYS.IANNI=0;
    TRMCFFKEY.NoiseOn=0;
#ifndef _PGTRACK_
    TRFITFFKEY.FastTracking=4;
#endif
    LVL3FFKEY.Accept=32;
    cout <<"AMSJob::udata-W-FastSimSelectedSomeDatacardsWereRedefined"<<endl;
  }

  {

    STATUSFFKEY.status[32]=0;
    for(int i=0;i<32;i++){
      STATUSFFKEY.status[32]=STATUSFFKEY.status[32] | 
	STATUSFFKEY.status[i];
    }
  }


  //#ifdef __MASSP__
  if(TRMFFKEY .OKAY!= 1234567890){
    cerr << "Datacards not terminated properly "<<endl;
    cerr << "Please add TERM=1234567890 in your datacards "<<endl;
    exit(1);  
  }
  //#endif




  {
    int i,j,k;
    for(i=0;i<2;i++){
      for(j=0;j<2;j++){
	if(TRMCFFKEY.RawModeOn[i][j][31]==1){
	  for(k=0;k<32;k++)TRMCFFKEY.RawModeOn[i][j][k]=1;
	}
      }
    }
  }


  const integer cl=161;
  char jobname[cl];
  char setupname[cl];
  char triggername[cl];
  char tdvname[1601];
  char ffile[cl];
  char ifile[cl];
  char ofile[cl];
  char sfile[cl];
  UHTOC(SELECTFFKEY.File,cl/sizeof(integer),sfile,cl-1);
  UHTOC(DAQCFFKEY.ifile,cl/sizeof(integer),ifile,cl-1);
  UHTOC(DAQCFFKEY.ofile,cl/sizeof(integer),ofile,cl-1);
  UHTOC(AMSFFKEY.Jobname,cl/sizeof(integer),jobname,cl-1);
  UHTOC(AMSFFKEY.Setupname,cl/sizeof(integer),setupname,cl-1);
  UHTOC(IOPA.ffile,cl/sizeof(integer),ffile,cl-1);
  UHTOC(IOPA.TriggerC,cl/sizeof(integer),triggername,cl-1);
  int i;

  //+
  for (i=cl-2; i>0; i--) {        // should be at least 1 char
    if(jobname[i] == ' ') jobname[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(setupname[i] == ' ') setupname[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ffile[i] == ' ') ffile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ifile[i] == ' ') ifile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(sfile[i] == ' ') sfile[i]='\0';
    else break;
  }
  for (i=cl-2; i>=0; i--) {
    if(ofile[i] == ' ') ofile[i]='\0';
    else break;
  }

  //-

  int len;
  //=======
  setsetup(setupname);
  if(getsetup())setname(strcat(jobname,getsetup()));
  else{
    cerr<<"AMSJOB::udata-F-NULLSETUP- Setup not defined"<<endl;
    exit(1);
  }

  AMSDBc::init(CCFFKEY.Angle);  
#ifndef _PGTRACK_
  TKDBc::init(0);
#endif
  TRDDBc::init();
  {
    int len=cl-1;

    for(i=cl-2;i>=0;i--){
      if(triggername[i]==' '){
	triggername[i]='\0';
	len=i+1;
      }
      else break;
    }
    integer ntrig=0;
    integer nold=0;
    integer orr=0;
    for (i=0;i<len;i++){
      if(triggername[i]=='|' || triggername[i]=='\0' || triggername[i]=='&'){
	// new trigger found
	if(triggername[i]=='|')orr=1;
	triggername[i]='\0';
	if(i-nold>0)settrigger(triggername+nold,ntrig++,orr);
	nold=i+1;
      }
    }
  }
  setjobtype(AMSFFKEY.Jobtype%10 != 0);
  setjobtype(((AMSFFKEY.Jobtype/10)%10 != 0)<<(RealData-1));
  uinteger ical=(AMSFFKEY.Jobtype/100)%10;//1-9
  uinteger ucal=1;
  if(ical)setjobtype(ucal<<(ical+1));
  uinteger imon=(AMSFFKEY.Jobtype/1000)%10;//1-9
  uinteger umon=1;
  if(imon)setjobtype(umon<<(imon+1+9));
  uinteger iprod=(AMSFFKEY.Jobtype/10000)%10;
  if(iprod){
    setjobtype(Production);
    if(IOPA.BuildMin<0){
      cerr<<"AMSJob::udata-F-IOPA.BuildNoNotSetWhileInProductionMode "<< IOPA.BuildMin<<" "<<endl;
      abort();
    }
    else if(IOPA.BuildMin>AMSCommonsI::getbuildno()){
      cerr<<"AMSJob::udata-F-IOPA.MinBuildNoRequired "<< IOPA.BuildMin<<" "<<endl;
      abort();
    }

  }

  if(IOPA.mode && isSimulation()){
    AMSIO::setfile(ffile);
    AMSIO::init(IOPA.mode);
  }
  if(isReconstruction() && DAQCFFKEY.mode%10 ==0)DAQCFFKEY.mode=DAQCFFKEY.mode+1;
  if(isSimulation() && DAQCFFKEY.mode%10 == 1)DAQCFFKEY.mode=DAQCFFKEY.mode-1;
  if(DAQCFFKEY.mode){
    AMSEvent::setfile(sfile);
    DAQEvent::setfiles(ifile,ofile);
  }



  //
  // Read/Write Synchronization
  if(AMSFFKEY.Read > 10 && AMSFFKEY.Read%2==0)AMSFFKEY.Read++;
  if(AMSFFKEY.Write > 0 && AMSFFKEY.Write%2==0)AMSFFKEY.Write++;

  // TDV
  if(AMSFFKEY.Update){
    UHTOC(AMSFFKEY.TDVC,400,tdvname,1600);
    {
      int len=1600;
      for(i=1599;i>=0;i--){
	if(tdvname[i]==' '){
	  tdvname[i]='\0';
	  len=i+1;
	}
	else break;
      }
      integer ntdv=0;
      integer nold=0;
      for (i=0;i<len;i++){
	if(tdvname[i]=='|' || tdvname[i]=='\0'){
	  // new tdv found
	  tdvname[i]='\0';
	  if(i-nold>0)settdv(tdvname+nold,ntdv++);
	  nold=i+1;
	}
      }
    }
  }


#ifdef _PGTRACK_
  TkDBc::CreateTkDBc();
#else
#endif
  int pgtrack_DB_ver=2;
  int STD_DB_ver=2;
  if(strstr(getsetup(),"AMS02") ){    
    if(strstr(getsetup(),"AMS02Pre") ){    
      cout <<"AMSJob::udata-I-2007TrackerConfigurationRequestred "<<endl;
      //      AMSTrIdSoft::inittable(2);
      STD_DB_ver=2;
      pgtrack_DB_ver=1;
    }
    else if(strstr(getsetup(),"AMS02P") ){
      cout <<"AMSJob::udata-I-PMTrackerConfigurationRequestred "<<endl;
      //      AMSTrIdSoft::inittable(4);
      STD_DB_ver=4;
      pgtrack_DB_ver=3;
    }
    else{
      cout <<"AMSJob::udata-I-FlightTrackerConfigurationRequestred "<<endl;
      //      AMSTrIdSoft::inittable(3);
      STD_DB_ver=3;
      pgtrack_DB_ver=2;
    }
#ifdef _PGTRACK_
    if(TKGEOMFFKEY.ReadGeomFromFile%10==1){
      char fname[1601];
      UHTOC(TKGEOMFFKEY.fname,400,fname,1600);
      //PZ FIXME metti a posto il path 
      TkDBc::Head->init(pgtrack_DB_ver,fname);
    }
    else
      TkDBc::Head->init(pgtrack_DB_ver);
#else
  AMSTrIdGeom::init(STD_DB_ver);
  float zpos[trconst::maxlad];
  for(int k=0;k<sizeof(zpos)/sizeof(zpos[0]);k++)zpos[k]=0;
  for(int k=0;k<TKDBc::nlay();k++)zpos[k]=TKDBc::zposl(k)-TKDBc::zpos(k);
  rkmsinit_(zpos);
    AMSTrIdSoft::inittable(STD_DB_ver);
    AMSTrIdSoft::init();
#endif


    AMSTRDIdSoft::init();
#ifndef __DARWIN__ 
    int env=fegetexcept();
    if(MISCFFKEY.RaiseFPE<=1)fedisableexcept(FE_ALL_EXCEPT);
#endif
    RichPMTsManager::Init();
#ifndef __DARWIN__  
    feclearexcept(FE_ALL_EXCEPT);
    if(env){
      feenableexcept(env);        
    }
#endif       
    RichRadiatorTileManager::Init();	
    RichAlignment::Init();
    RichLIPRec::InitGlobal();
    AMSTRDIdSoft::inittable();
    AMSECIds::inittable();
    if(RICRECFFKEY.recon[0]/10000){
      AMSRichRing::SetWindow((RICRECFFKEY.recon[0]/10000)/10.0);
    }
  }
  else {
    cerr<<"AMSJob::udate-E-NoAMSTrIdSoftTable exists for setup "<<
      getsetup()<< "yet "<<endl;
    exit(1);
  }

#ifdef _PGTRACK_
  // SetUp the Tracker reconstruction infrastructure
  Patt= new PATT(); // PZ FIXME dummy pattern structure used ionly for the dummy trigger302
  //PZ FIXME must be simplified
  TrCalDB* cc=new TrCalDB();
  TrLadCal::SetVersion(TKGEOMFFKEY.CalibVer);
  cc->init();
  cc->CreateLinear();
  //  cc->Load("prima.root");
  TrClusterR::UsingTrCalDB(TrCalDB::Head);
  TrRawClusterR::UsingTrCalDB(TrCalDB::Head);

  TrParDB* cc2=new TrParDB();
  cc2->init();
  cc2->CreateLinear();
  TrClusterR::UsingTrParDB(TrParDB::Head);

  TrRecon::Init();
  TrRecon::SetParFromDataCards();
  TrRecon::UsingTrCalDB(TrCalDB::Head);


  if (TrRecon::TasRecon) {
    TrTasDB *tasdb = new TrTasDB;
    tasdb->Init(0);
    tasdb->CreateLinear();
    TrDAQ::MaxClusterLength = 640;
  }

  float zpos[trconst::maxlay];
  int   lay [trconst::maxlay] = { 8, 1, 2, 3, 4, 5, 6, 7, 9 };
  for (int i = 0; i < trconst::maxlay; i++)
    zpos[i] = TkDBc::Head->GetZlayer(lay[i]);

  rkmsinit_(zpos); 

#else

  // TraligGlobalFit
  if(TRALIG.GlobalFit && TRALIG.MaxPatternsPerJob!=1){
    cout <<"AMSJob::udata-I-GlobalFitRequested ";
    TRALIG.MaxPatternsPerJob=1;
    TRALIG.MaxEventsPerFit=499999;
    TRALIG.MinEventsPerFit=999; 
    cout <<"Parameters Changed "<<TRALIG.MaxEventsPerFit<<" "<<TRALIG.MinEventsPerFit<<endl;
  }

#endif

  //MAGFIELD
  UHTOC(TKFIELD.mfile,160/sizeof(integer),AMSDATADIR.fname,160);

  for(i=159;i>=0;i--){
    if(AMSDATADIR.fname[i]!=' '){
      AMSDATADIR.fname[i+1]=0;
      break;
    }
    if(strlen(AMSDATADIR.fname)<=1)strcpy(AMSDATADIR.fname,"fld97int.txt");
  }

  //PZMAG #ifdef _PGTRACK_
  //   MagField* mmpp=MagField::GetPtr();
  //   MAGSFFKEY.BTempCorrection=MISCFFKEY.BTempCorrection;
  //   MAGSFFKEY.BZCorr=MISCFFKEY.BZCorr;
  //   if(mmpp) {
  //     mmpp->SetMfile(AMSDATADIR.fname);
  //     mmpp->SetInitOk(TKFIELD.iniok);
  //   }
  // #endif

  if(TRDMCFFKEY.mode==-1){
    if(!strstr(getsetup(),"AMSSHUTTLE")){
      TRDMCFFKEY.mode=0;
    }
  }

#ifndef _PGTRACK_

  //PZ FIXME
  // check delta etc
  if(TRMCFFKEY.gammaA[0]<0){
    TRMCFFKEY.gammaA[0]=0.2;
  }
  if(TRMCFFKEY.delta[0]<0){
    TRMCFFKEY.delta[0]=0.9;
  }
  if(TRMCFFKEY.thr1R[0]<0){
    TRMCFFKEY.thr1R[0]=3;
  }

  if(TRCLFFKEY.Thr1R[0]<0){
    TRCLFFKEY.Thr1R[0]=3;
  }
#endif
  char hfile[161];
  UHTOC(IOPA.hfile,40,hfile,160);  
  _hextname=hfile;

  char rfile[161];
  UHTOC(IOPA.rfile,40,rfile,160);  
  _rextname=rfile;

  // check lvl3
  if(LVL3FFKEY.TrMaxResidual[0]<0){
    for (int i=0;i<3;i++)LVL3FFKEY.TrMaxResidual[i]=6*fabs(LVL3FFKEY.TrMaxResidual[i]);
  }

}


void AMSJob::init(){
AMSEvent::debug=AMSFFKEY.Debug;


#ifdef _PGTRACK_
  if (TRCLFFKEY.TrTimeLim > 0 && 
      TRCLFFKEY.TrTimeLim > AMSFFKEY.CpuLimit) {
    TRCLFFKEY.TrTimeLim = AMSFFKEY.CpuLimit;
    TrRecon::RecPar.TrTimeLim = TRCLFFKEY.TrTimeLim;
  }
  cout << "AMSJob::init-I-TRCLFFKEY.TrTimeLim set as AMSFFKEY.CpuLimit: "
       << TRCLFFKEY.TrTimeLim << " " << AMSFFKEY.CpuLimit << endl;
#endif


//AMSgObj::BookTimer.book("AMSEvent::event");
AMSgObj::BookTimer.book("GUOUT");
AMSgObj::BookTimer.book("GUKINE");

_siamsinitjob();
_reamsinitjob();
_timeinitjob();
map(1);
if(isCalibration())_caamsinitjob();
_dbinitjob();
cout << *this << " "<<this;
}
void AMSJob::_siamsinitjob(){
AMSgObj::BookTimer.book("SIAMSEVENT");
  _sitkinitjob();
  _signinitjob();
  _sitof2initjob();
  _sianti2initjob();
  _siecalinitjob();
  _sirichinitjob();
  _sitrdinitjob();
  _sisrdinitjob();
  _sitrig2initjob();
}


void AMSJob::_dbinitjob(){
if (AMSFFKEY.Update>1){

  // Here update dbase

     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
    while(offspring){
            for(int i=0;i<AMSJob::gethead()->gettdvn();i++){     
               offspring->checkupdate(AMSJob::gethead()->gettdvc(i));
             }
    if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
    offspring=(AMSTimeID*)offspring->next();
    }
}


  AMSgObj::BookTimer.book("EventStatus");
  AMSStatus::init();
}

//-------------------------------------------------------------------------------------------
void AMSJob::_sitrig2initjob(){
//
  Trigger2LVL1::init();
  if(TGL1FFKEY.Lvl1ConfRead%10>0){//(N) Take TrigConfig(mc|rd) from raw files
    Trigger2LVL1::l1trigconf.read();
  }
  Trigger2LVL1::scalmon.setdefs();//set scalmon(static) defaults for rd/mc
//--------
  if(TGL1FFKEY.RebuildLVL1)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl1 Will be rebuild from TOF data. Original info will be lost"<<endl<<endl;
  if(LVL3FFKEY.RebuildLVL3==1)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl3 will be rebuild from TOF/Trk data. Original info will be lost"<<endl<<endl;
  else if(LVL3FFKEY.RebuildLVL3)
    cout <<"<---- AMSJob::sitrig2initjob:TriggerLvl3 will be rebuild from TOF/LVL1 data. Original info will be kept"<<endl<<endl;
  AMSgObj::BookTimer.book("LVL3");
  TriggerLVL302::init();  
}
//-------------------------------------------------------------------------------------------
#ifndef _PGTRACK_
void AMSJob::_sitkinitjob(){
  if(TRMCFFKEY.GenerateConst){
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+="trsigma.hbk";
    int iostat;
    int rstat=1024;
    HROPEN(1,"input",fnam.operator char*(),"P",rstat,iostat);
    if(iostat){
     cerr << "Error opening TrSigmaFile file "<<fnam<<endl;
     throw amsglobalerror("UnableToOpenHistoFile",3);
    }
    char input[]="//input";
    HCDIR(input," ");
    HRIN(0,9999,0);
//    HPRINT(101);
//    HPRINT(102);
//    HPRINT(201);
//    HPRINT(202);
     for(int l=0;l<2;l++){
       for (int i=0;i<TKDBc::nlay();i++){
         for (int j=0;j<TKDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             geant d;
             id.setped()=TRMCFFKEY.ped[l]*(1+RNDM(d));
             id.clearstatus(~0);
             if(RNDM(d)<TRMCFFKEY.BadCh[l]){
              id.setstatus(AMSDBc::BAD);
              id.setsig()=0.85*HRNDM1(201+l);
             }
             else{
              id.setsig()=(l==0?0.76:0.84)*HRNDM1(101+l);
              if(id.getsig()<1){
               id.setstatus(AMSDBc::BAD);
              }
              else if(id.getsig()>20){
               id.setsig()=HRNDM1(101+l);
               if(id.getsig()<1){
                id.setstatus(AMSDBc::BAD);
               }
              }
              else if(id.getsig()>10){
               if(RNDM(d)<0.1*(id.getsig()-10)){
                id.setsig()=HRNDM1(101+l);
                 if(id.getsig()<1){
                  id.setstatus(AMSDBc::BAD);
                 }
               }
              }
               
             } 
             if(TRMCFFKEY.GenerateConst>1)id.setsig()=TRMCFFKEY.sigma[l]*(0.9+0.2*RNDM(d));
             id.setgain()=TRMCFFKEY.gain[l];
             id.setcmnnoise()=TRMCFFKEY.cmn[l]*(1+RNDM(d));
             id.setindnoise()=oldone+
             AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
             oldone=id.getindnoise();
            }
           }
         }
       }
     }
  HREND("input");
  CLOSEF(1);
  char pawc[]="//PAWC";
  HCDIR(pawc," " );
  HDELET(101);
  HDELET(102);
  HDELET(201);
  HDELET(202);
      HBOOK1(101,"sigx",800,0.,200.,0.);
      HBOOK1(102,"sigy",800,0.,200.,0.);
      HBOOK1(201,"sigx",800,0.,200.,0.);
      HBOOK1(202,"sigy",800,0.,200.,0.);
     for(int l=0;l<2;l++){
       for (int i=0;i<TKDBc::nlay();i++){
         for (int j=0;j<TKDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             float sig=id.getsig();
             if(id.checkstatus(AMSDBc::BAD)){
               HF1(201+l,sig,1.);
             }
             else {
               HF1(101+l,sig,1.);
             }
            }
           }
         }
       }
     }

  }

else TRMCFFKEY.year[1]=TRMCFFKEY.year[0]-1;

   for(int l=0;l<2;l++){
   // book noise prob
    geant a=TRMCFFKEY.thr1R[l]-AMSTrMCCluster::step()/2.;
    geant b=a+AMSTrMCCluster::ncha()*AMSTrMCCluster::step();
    HBOOK1(AMSTrMCCluster::hid(l),"x prob",AMSTrMCCluster::ncha(),a,b,0.);
   }
     AMSgObj::BookTimer.book("GEANTTRACKING");
     AMSgObj::BookTimer.book("GUSTEP",10);
     AMSgObj::BookTimer.book("AMSGUSTEP",10);
     AMSgObj::BookTimer.book("SYSGUSTEP",10);
     AMSgObj::BookTimer.book("TrdRadiationGen",10);
     AMSgObj::BookTimer.book("SITKHITS");
     AMSgObj::BookTimer.book("SITKNOISE");
     AMSgObj::BookTimer.book("SITKDIGIa");
     AMSgObj::BookTimer.book("SITKDIGIb");
     AMSgObj::BookTimer.book("SITKDIGIc");
     AMSgObj::BookTimer.book("TrMCCluster");
}
#endif
void AMSJob::_signinitjob(){
     AMSgObj::BookTimer.book("SetTimeCoo");

  AMSmceventg::setcuts(CCFFKEY.coo,CCFFKEY.dir,CCFFKEY.momr,
  CCFFKEY.fixp,CCFFKEY.albedor,CCFFKEY.albedocz);
  //
  AMSmceventg::setspectra(CCFFKEY.begindate,CCFFKEY.begintime,
  CCFFKEY.enddate, CCFFKEY.endtime, GCKINE.ikine,CCFFKEY.low);
  
}
//----------------------------------------------------------------------------------------
void AMSJob::_sitof2initjob(){
//
  if(TFMCFFKEY.daqfmt==0)cout<<"<----- sitof2init-I-Raw Format selected"<<endl;
  else if(TFMCFFKEY.daqfmt==1)cout<<"<----- sitof2init-I-Reduced Format selected"<<endl;
  else {
    cout<<"<----- sitof2init:Error: TOF/aNTI: Unknown Format was requested !!!"<<endl;
    exit(1);
  }
//
    AMSgObj::BookTimer.book("SITOFDIGI");
    AMSgObj::BookTimer.book("TOF:Ghit->Tovt");
    AMSgObj::BookTimer.book("TOF:Tovt->RwEv");
    AMSgObj::BookTimer.book("TovtPM1loop");
    AMSgObj::BookTimer.book("TovtPM2loop");
    AMSgObj::BookTimer.book("TovtPM2sloopscpmesp");
    AMSgObj::BookTimer.book("TovtPM2sloopscmcscan");
    AMSgObj::BookTimer.book("TovtPM2sloopsum");
    AMSgObj::BookTimer.book("TovtOther");
//---------(*.ReadConstFiles card convention : tfmc->LPTS, tfre->LQDPC)
  if((TFMCFFKEY.ReadConstFiles%100)/10==1
              || TFREFFKEY.ReadConstFiles%10==1){//(T) MC-TimeScanDistr from ext.files
//  
    TOFWScan::build();//create scmcscan-objects (MC Bar's scan Time/Eff-distributions)
  }
  if(TFMCFFKEY.ReadConstFiles%10==1 && !isRealData()){//(S)
    TOFBrcalMS::build();//create sc.bar "MC Seeds" scbrcal-objects
  }
//---------
  if((TFMCFFKEY.ReadConstFiles%1000)/100>0 && !isRealData()){//(P) def.MCPeds from ext.files
    TOFBPeds::mcbuild();//create sc.bar scbrped-objects
  }
//---------
  if(TFMCFFKEY.tdclin>0 && (TFMCFFKEY.ReadConstFiles/1000)>0
                         && !isRealData()){//(L) MCSeed LinCorr(if requested) from ext.files
    TofTdcCorMS::build();//create MCSeed TofTdcCor objects
  }
//---------
//
   TOF2JobStat::bookhistmc();//Book histograms for MC
}
//----------------------------------------------------------------------------------------
void AMSJob::_siecalinitjob(){
  if(ECMCFFKEY.fastsim==1)cout <<"<----- siecinit-I-Fast(Crude) simulation algorithm selected."<<endl;
  else cout <<"<----- siecalinitjob:-I-Slow(Accurate) simulation algorithm selected !!!"<<endl;
//
//
    AMSgObj::BookTimer.book("SIECALDIGI");
    AMSgObj::BookTimer.book("SIECALEVENT");
//
// ===> Book histograms for MC :
//
    EcalJobStat::bookhistmc();
//
//make Toy RLGA-files[ ...sd.cof(mcseeds) and ...mc.cof(calib.outp)]:
//    ECREUNcalib::makeToyRLGAfile();//tempor
//
// ===> Read Peds from file, otherwise they will be taken from DB
  if(!isRealData()){
    if(ECMCFFKEY.ReadConstFiles%10>0){//P
      ECPMPeds::build();
    }
    if((ECMCFFKEY.ReadConstFiles%100)/10>0){//C(only Seeds parameters(rlga+fiat))
      ECcalibMS::build();
    }
  }
//
}
//-----------------------------------------------------------------------
void AMSJob::_sirichinitjob(){
  AMSgObj::BookTimer.book("SIRICH");
  AMSgObj::BookTimer.book("SIRINoise");
  RICHDB::bookhist();
}

void AMSJob::_rerichinitjob(){
  AMSgObj::BookTimer.book("RERICH");
  AMSgObj::BookTimer.book("RERICHLIP"); 
  AMSgObj::BookTimer.book("RERICHZ");   
  AMSgObj::BookTimer.book("RERICHBETA");
  AMSgObj::BookTimer.book("RERICHHITS");
}

//-----------------------------------------------------------------------
void AMSJob::_sianti2initjob(){
  AMSgObj::BookTimer.book("SIANTIEVENT");
//  ATMC: 4=Sp|Rp->Seed/Real MC-peds
  if(ATMCFFKEY.ReadConstFiles%10>0 && !isRealData()){//Real MC-Peds from ext.files
    ANTIPeds::mcbuild();//create anti-sector ANTIPeds::anscped-objects
  }
  if(ATMCFFKEY.ReadConstFiles/10>0 && !isRealData()){//Seed MC Peds from ext.files
    ANTIPedsMS::build();//create anti-sector ANTIPedsMS::anscped-objects
  }
  if(ATMCFFKEY.mcprtf)ANTI2JobStat::bookmch();
}
//-----------------------------------------------------------------------
void AMSJob::_sitrdinitjob(){

  if(TRDMCFFKEY.GenerateConst){
      HBOOK1(8001,"trd ped",1000,0.,1000.,0.);
      HBOOK1(8002,"trd sig",1000,0.,50.,0.);
      HBOOK1(8003,"trd gain",1000,0.,2.,0.);
       for (int i=0;i<TRDDBc::LayersNo(0);i++){
         for (int j=0;j<TRDDBc::LaddersNo(0,i);j++){
           for (int k=0;k<TRDDBc::TubesNo(0,i,j);k++){
             geant d;
             AMSTRDIdSoft id(i,j,k);
             id.setped()=TRDMCFFKEY.ped+rnormx()*TRDMCFFKEY.pedsig;
             id.clearstatus(~0);
             id.setsig()=TRDMCFFKEY.sigma*(0.9+0.2*RNDM(d));
             if(RNDM(d)<TRDMCFFKEY.NoiseLevel){
              id.setsig()=id.getsig()*10;
             }
             id.setgain()=TRDMCFFKEY.gain*(0.9+0.2*RNDM(d));
             if(RNDM(d)<TRDMCFFKEY.DeadLevel){
              id.setgain()=id.getgain()*0.1;
              id.setstatus(AMSDBc::BAD);
             }
             HF1(8001,id.getped(),1.);
             HF1(8002,id.getsig(),1.);
             HF1(8003,id.getgain(),1.);
//
//  common noise not yet defined
//             id.setcmnnoise()=TRDMCFFKEY.cmn*(0.5+RNDM(d));
           }
         }
       }
    }
    else TRDMCFFKEY.year[1]=TRDMCFFKEY.year[0]-1;

 AMSTRDMCCluster::init();
 AMSgObj::BookTimer.book("SITRDDigi");
 
 if (CCFFKEY.npat>1){
   AMSTRDRawHit::init_delay_table();
 }
}
void AMSJob::_sisrdinitjob(){
}


void AMSJob::_reamsinitjob(){
 AMSgObj::BookTimer.book("WriteEvent");
 AMSgObj::BookTimer.book("WriteEventC");
 AMSgObj::BookTimer.book("WriteEventR");
 AMSgObj::BookTimer.book("WriteEventF");
 AMSgObj::BookTimer.book("REAMSEVENT"); 
 AMSgObj::BookTimer.book("Vtx");

 _remfinitjob();
 _redaq2initjob();
 _retkinitjob();
 _retof2initjob();
 _reanti2initjob();
 _reecalinitjob();
 _retrdinitjob();
 _rerichinitjob();
 _resrdinitjob();
 _reaxinitjob();
 AMSUser::InitJob();
}
//-----------------------------------------------------
void AMSJob::_caamsinitjob(){
if(isCalibration() & CTracker)_catkinitjob();
  if(isCalibration() & CTOF)_catof2initjob();
  if(isCalibration() & CAnti)_cant2initjob();
  if(isCalibration() & CEcal)_caecinitjob();
  if(isCalibration() & CTRD)_catrdinitjob();
  if(isCalibration() & CSRD)_casrdinitjob();
  if(isCalibration() & CRICH)_carichinitjob();

if(isCalibration() & CAMS)_caaxinitjob();
//if(isCalibration() & CCerenkov)_cactcinitjob();
}
//-----------------------------------------------------

#ifndef _PGTRACK_

void AMSJob::_catkinitjob(){
cout <<" AMSJob::_catkinitjob()-i_Initialized for Proc no  "<<TRCALIB.CalibProcedureNo<<endl;
AMSgObj::BookTimer.book("CalTrFill");
AMSgObj::BookTimer.book("CalTrFit");
if(TRALIG.UpdateDB){

  AMSTrAligFit::Fillgl(gethead()->addup( new AMSTrAligFit()));
//  AMSTrAligFit::Fillgle(gethead()->addup( new AMSTrAligFit()));
}
if(TRCALIB.CalibProcedureNo == 1 || TRCALIB.CalibProcedureNo==4){
  AMSTrIdCalib::initcalib();
}
else if(TRCALIB.CalibProcedureNo == 2){
int i,j;
for(i=0;i<nalg;i++){
  for(j=0;j<tkcalpat;j++){
    AMSTrCalibFit::setHead(i,j, new 
    AMSTrCalibFit(j,TRCALIB.EventsPerIteration[i],TRCALIB.NumberOfIterations[i],i,GCKINE.ikine));
  }
}
}
}
#endif
//============================================================================
void AMSJob::_catof2initjob(){
 integer cmode=TFREFFKEY.relogic[0];
 if(cmode==1){
   TOFTdcCalib::init();// TOF TDC-calibr.
 }
 if(cmode==2 || cmode==3 || cmode==4 || cmode==23 || cmode==234 || cmode==34){
   TofTmAmCalib::initjob();// TOF Tdelv/Tzslw/Ampl-calibr.
 }
 if(TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6){
   TOFPedCalib::init();// TOF Ped-calibr.
 }
//
}
//=================================================================================
void AMSJob::_catrdinitjob(){
  AMSTRDIdCalib::initcalib();
  AMSgObj::BookTimer.book("CalTRDFill");
}
void AMSJob::_casrdinitjob(){
}

//==========================================
void AMSJob::_cant2initjob(){
  if(ATREFFKEY.relogic==1){
    AntiCalib::init();
  }
  if(ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3 || ATREFFKEY.relogic==4){
    ANTPedCalib::init();
  }
}
//==========================================
void AMSJob::_caecinitjob(){
 if(ECREFFKEY.relogic[1]>0 && ECREFFKEY.relogic[1]<4){
   ECREUNcalib::init();// ECAL REUN-calibr.
   cout<<endl;
   cout<<"<----- ECREUNcalib-init done !!!"<<'\n';
   cout<<endl;
 }
 if(ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5){
   ECPedCalib::init();
 }
 
 


}


void AMSJob::_carichinitjob(){
  AMSRichCal::init();
}

//==========================================

void AMSJob::_caaxinitjob(){
}

void AMSJob::_remfinitjob(){

#ifndef _PGTRACK_
READMFIELD();
if(MISCFFKEY.BTempCorrection){
cout <<"<---- AMSJob::_remfinitjob-I-Magnetic Field Temp Corrections will be used"<<endl<<endl; 
}
//PZMAG #else
//   if(TKFIELD.iniok==0 || TKFIELD.iniok==-2) {
//     char name[150];
//     //sprintf(name,"%s/v4.00/%s",AMSDATADIR.amsdatadir,AMSDATADIR.fname);
//     //MagField::GetPtr()->Read(name);
//         MagField::GetPtr()->Read(AMSDATADIR.fname,AMSDATADIR.amsdatadir);
// }

//   if(MAGSFFKEY.BTempCorrection){
//    cout <<"<---- AMSJob::_remfinitjob-I-Magnetic Field Temp Corrections are MEANING LESS for AMS02"<<endl<<endl; 
//  }
#endif
}

#ifndef _PGTRACK_
void AMSJob::_retkinitjob(){
AMSgObj::BookTimer.book("RETKEVENT");
AMSgObj::BookTimer.book("TrCluster");
AMSgObj::BookTimer.book("TrClusterRefit");
AMSgObj::BookTimer.book("TrRecHit");
AMSgObj::BookTimer.book("TrRecHit_build");
AMSgObj::BookTimer.book("TrRecHit_add");
AMSgObj::BookTimer.book("TrTrack");
AMSgObj::BookTimer.book("TrTrackPathI");
AMSgObj::BookTimer.book("TrTrackSimpleFit");
AMSgObj::BookTimer.book("TrTrackPathIA");
AMSgObj::BookTimer.book("TrTrackFalseTOFX");
AMSgObj::BookTimer.book("TrFalseX");
AMSgObj::BookTimer.book("TrAdvancedFit");
AMSgObj::BookTimer.book("TrAdvancedFit_3");

  if(CHARGEFITFFKEY.TrkPDFileRead%10>0){//(Q) Take ChargeCalibPDFs(mc|rd) from raw files
//
    TrkElosPDF::build();//create TrkElosPDF-objects from TRK raw ChargeCalibFile
  }
}
#endif
//===================================================================================
//
void AMSJob::_retof2initjob(){
//
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:validation");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");
//
// ===> some inits, common for reco/simu :
//
    TOF2JobStat::clear();//Clear JOB-stat arrays, set fast temp. defaults=undefined(999) for SFET(A)(TempT)
    TofSlowTemp::tofstemp.init(); // set default values for TOF slow temperatures(TempC,TempP)=undefined
// formally def=undefined settings autom.provide true(TOF2Varp-based) def.value at valid-stage if some 
//temper.data was not found !!! 
//
// ===> Book histograms for REC :
//
    TOF2JobStat::bookhist();
//
//---------(*.ReadConstFiles card convention : tfmc->LPTS, tfre->LQDPC)
//--------
  if((TFREFFKEY.ReadConstFiles%1000)/100>0){//(D) Take ThreshCuts-set from Data-Cards
    TOF2Varp::tofvpar.init(TFREFFKEY.daqthr, TFREFFKEY.cuts);//create ThrCuts-obj from Data-Cards
  }
//--------
  if(TFREFFKEY.ReadConstFiles%10>0){//(C) Take Paddles CalibConst from CalibFiles
    TOF2Brcal::build();//create scbrcal-objects from CalibFiles
  }
//--------
//
  if((TFREFFKEY.ReadConstFiles%100)/10>0 && isRealData()){//(P) Take default RealData-Peds from file
    TOFBPeds::build();//create RealData scbrped-objects from peds-file
  }
// 
//--------
  if((TFREFFKEY.ReadConstFiles%10000)/1000>0){//(Q) Take ChargeCalibPDFs(mc|rd) from files
    TofElosPDF::build();//create PDF-objects from CalibFiles
  }
//---------
  if((TFREFFKEY.ReadConstFiles/10000)>0 && isRealData()){//(L) RD LinCorr from ext.files
    TofTdcCor::build();//create TofTdcCor (RD) objects
  }
//---------
  if((TFMCFFKEY.ReadConstFiles/1000)>0 && TFMCFFKEY.tdclin>0 && !isRealData()){//(L) MC LinCorr from ext.files
    TofTdcCor::build();//create TofTdcCor (MC) objects
  }
//
//-----------
  if(((CALIB.SubDetInCalib/1000)%10)>0)TOFPedCalib::initb();//OnBoardPeds-calib, runs in normal(not calib) mode
//-----------
  AMSTOFCluster::init();
  AMSSCIds::inittable();
  AMSSCIds::selftest();
}
//=============================================================================================
void AMSJob::_reanti2initjob(){
//
    AMSgObj::BookTimer.book("REANTIEVENT");
    if(ATREFFKEY.reprtf[0]>1)ANTI2JobStat::bookh();
//
    ANTI2JobStat::clear();
//-----------
//
  if(ATREFFKEY.ReadConstFiles/100>0 && isRealData()){// (PVS) RealData Peds from ext.files
//
    ANTIPeds::build();//create sc.bar anscped-objects
  }
// 
  if(ATREFFKEY.ReadConstFiles%10){//(PVS) StableCalibParams(MC/RD) from ext.files
     ANTI2SPcal::build();//create antispcal-objects for each sector
  }
  if((ATREFFKEY.ReadConstFiles/10)%10>0){//(PVS) VariableCalibParams(MC/RD) from ext.files
     ANTI2VPcal::build();//create antivpcal-objects for each sector
  }
//-----------
  if(((CALIB.SubDetInCalib/1000)%10)>0)ANTPedCalib::initb();//OnBoardPeds-calib, runs in normal(not calib) mode
}
//============================================================================================
void AMSJob::_reecalinitjob(){
//

cout<<"====> AMSJob::reecalinitjob: starting..."<<endl;
// setup - data type dep init
if(ECREFFKEY.SimpleRearLeak[0]<0){
  cout<<"      EcalRearLeak parameters are set to :"<<endl;
  if(isRealData() ){
    ECREFFKEY.SimpleRearLeak[0]=0.015;
    ECREFFKEY.SimpleRearLeak[1]=0.99e-3;
    ECREFFKEY.SimpleRearLeak[2]=3.6;
    ECREFFKEY.SimpleRearLeak[3]=1.045e-3;
    for (int i=0;i<4;i++)cout <<"  "<<ECREFFKEY.SimpleRearLeak[i]<<endl;
  }
  else{
    for(int k=0;k<4;k++){
      if( ECREFFKEY.SimpleRearLeak[k]<0) ECREFFKEY.SimpleRearLeak[k]*=-1;
    }
    for (int i=0;i<4;i++)cout <<"  "<<ECREFFKEY.SimpleRearLeak[i]<<endl;
  }
}





  integer pr,pl,cell;
  number ct,cl,cz;
  integer fid,cid[4]={0,0,0,0};
  number w[4]={0.,0.,0.,0.};
//
    AMSgObj::BookTimer.book("REECALEVENT");
    AMSgObj::BookTimer.book("ReEcalShowerFit");
//
    ECALDBc::getscinfoa(0,0,0,pr,pl,cell,ct,cl,cz);// <--- init. PMCell-readout tables
//
    EcalJobStat::clear();// Clear JOB-statistics counters for SIM/REC
//
    EcalJobStat::bookhist();// Book histograms for REC
//
//-----------
// 
// ===> create Cuts/Thresh parameters (ecalvpar structure) fr.data-cards :
//              ECREFFKEY.ReadConstFiles=DCP
  if(ECREFFKEY.ReadConstFiles/100>0){//D Thresh/Cuts-object will be created from data-cards  
    ECALVarp::ecalvpar.init(ECREFFKEY.thresh, ECREFFKEY.cuts);
    cout<<" <--- ecalvpar(datacards) init done..."<<endl;
  }
//
// ===> create ecpmcal-calib-objects:
//
  if((ECREFFKEY.ReadConstFiles%100)/10>0){//C  Calib.const-object(MC/RD) will be created from calib-files 
    ECcalib::build();
  } 
//-------------------------
// ===> create EC-SubCell peds/sigs (RD):
//
  if(ECREFFKEY.ReadConstFiles%10>0 && isRealData()){//P  ECPMPeds-Objects will be created from def. calib-file
    ECPMPeds::build();
  }
//-----------
  if((CALIB.SubDetInCalib%10)>0)ECPedCalib::initb();//OnBoardPeds-calib, runs in normal(not calib) mode
//-----------
cout<<"<---- AMSJob::reecalinitjob: is successfully done !"<<endl<<endl;
}
//===================================================================

void AMSJob::_reaxinitjob(){
  AMSgObj::BookTimer.book("REAXEVENT");
  AMSgObj::BookTimer.book("REAXPART");
  AMSgObj::BookTimer.book("ReAxRefit");
AMSgObj::BookTimer.book("ReTRDRefit"); 
AMSgObj::BookTimer.book("ReTKRefit"); 
AMSgObj::BookTimer.book("ReGeaneRefit"); 
AMSgObj::BookTimer.book("ReECRefit"); 
AMSgObj::BookTimer.book("ReRICHRefit"); 
AMSgObj::BookTimer.book("ReTOFRefit"); 
  AMSgObj::BookTimer.book("ReAxPid");
  AMSgObj::BookTimer.book("part::loc2gl");


if (AMSJob::gethead()->isMonitoring()) {

  const   int nids = 17;
  
  int16u ids[nids] =
  { 0x200,
    0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1,  //  TOF Raw
    0x1680, 0x1740,                                           //  TRK Reduced
    0x1681, 0x1741,                                           //  TRK Raw
    0x168C, 0x174C,                                           //  TRK Mixed
    0x0440,                                                   //  Level-1
    0x1200                                                    //  Level-3
  };
  
  
  char   *ids_names[]  = {"Length (words) GenBlock    ",
                          "Length (words) TOF(raw) 01 ",
                          "Length (words) TOF(raw) 31 ",
                          "Length (words) TOF(raw) 41 ",
                          "Length (words) TOF(raw) 71 ",
                          "Length (words) TOF(raw) 03 ",
                          "Length (words) TOF(raw) 33 ",
                          "Length (words) TOF(raw) 43 ",
                          "Length (words) TOF(raw) 73 ",
                          "Length (words) TRK(red) 32",
                          "Length (words) TRK(red) 72",
                          "Length (words) TRK(raw) 32",
                        "Length (words) TRK(raw) 72",
                          "Length (words) TRK(mix) 32",
                        "Length (words) TRK(mix) 72",
                        "Length (words) Level1 block ",
                        "Length (words) Level3 block "};

  int nchans[nids] ={100,
                    100,100,100,100,100,100,100,100,
                    1000,1000,
                    800,800,
                    800,800,
                    100, 100};
                    
  for (int i=0; i<nids; i++) {
     int hid  = 300000 + ids[i];
     geant f = nchans[i];
     integer nbin = 50;
     if (i>17) nbin = 100;
     HBOOK1(hid,ids_names[i],nbin,0.,f,0.);
  }
    HBOOK1(300000,"Length (words) event",300,0.,1500.,0.);
    HBOOK1(300001,"Length (words) TOF",200,0.,600.,0.);
    HBOOK1(300002,"Length (words) Tracker",300,0.,1500.,0.);
    HBOOK1(300003,"Time Difference(msec)",500,-0.02,80.-0.02,0);


}


}

void AMSJob::_retrdinitjob(){
AMSgObj::BookTimer.book("RETRDEVENT"); 
AMSgObj::BookTimer.book("REHTRDEVENT"); 
}
void AMSJob::_resrdinitjob(){
AMSgObj::BookTimer.book("RESRDEVENT"); 
}

AMSgvolume * AMSJob::getgeom(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID(AMSDBc::ams_name,1);
  return (AMSgvolume*)AMSJob::JobMap.getp(id);
}
AMSG4Physics* & AMSJob::getg4physics(){
//    AMSID id("AMSG4Physics",0);
//    return (AMSG4Physics*)AMSJob::JobMap.getp(id);
      return _pAMSG4Physics;
}

AMSG4GeneratorInterface* & AMSJob::getg4generator(){
//    AMSID id("AMSG4GeneratorInterface",0);
//    return (AMSG4GeneratorInterface*)AMSJob::JobMap.getp(id);
      return _pAMSG4GeneratorInterface;
}

AMSStatus * AMSJob::getstatustable(){
  static AMSStatus * _Head=0;
  if(!_Head){
    _Head=AMSStatus::create(!strstr(getsetup(),"AMSSHUTTLE") );
  }
  return _Head;
}


AMSgmat * AMSJob::getmat(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID("Materials:",0);
  return (AMSgmat*)AMSJob::JobMap.getp(id);
}

AMSgtmed * AMSJob::getmed(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID("TrackingMedia:",0);
  return (AMSgtmed*)AMSJob::JobMap.getp(id);
}


void AMSJob::setsetup(char *setup){
  if(setup && (strlen(setup)>1 || AMSFFKEY.ZeroSetupOk))strcpy(_Setup,setup);
  else strcpy(_Setup,"AMSSHUTTLE");   //defaults
  
}
void AMSJob::settrigger(char *setup, integer N,integer orr){
  assert(N < maxtrig);
  if(setup){
    strcpy(_TriggerC[N],setup);
  }
  _TriggerI=1;
  _TriggerOr=orr;
  _TriggerN=N+1;
}
void AMSJob::settdv(char *setup, integer N){
  assert(N < maxtdv);
  if(setup){
   if(strlen(setup)<maxtdvsize)strcpy(_TDVC[N],setup);
   else cerr << "AMSJOB::settdv-E-length of "<<setup<< " "<<strlen(setup)<<
         ", exceeds "<<maxtdvsize<<". Card ignored"<<endl;
  }
  _TDVN=N+1;
}
//==========================================================
void AMSJob::_timeinitjob(){
AMSgObj::BookTimer.book("TDV");
#ifdef __DB__
AMSgObj::BookTimer.book("TDVdb");
#endif
static AMSTimeID TID(AMSID("TDV:",0));
gethead()->addup( &TID);

AMSTimeID::CType server=AMSTimeID::Standalone;
#ifdef __CORBA__
if(!AMSProducer::gethead()->IsSolo())server=AMSTimeID::Client;
#endif

{
  tm begin;
  tm end;
  if(AMSFFKEY.Update==88){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
  AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID(getstatustable()->getname(),
                          isRealData()),begin,end,getstatustable()->getsize(),
                          getstatustable()->getptr(),server));

  if(AMSFFKEY.Update==88)return;

}


{

//
// Magnetic Field Map
//
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=TKFIELD.isec[0];
begin.tm_min=TKFIELD.imin[0];
begin.tm_hour=TKFIELD.ihour[0];
begin.tm_mday=TKFIELD.iday[0];
begin.tm_mon=TKFIELD.imon[0];
begin.tm_year=TKFIELD.iyear[0];
end.tm_sec=TKFIELD.isec[1];
end.tm_min=TKFIELD.imin[1];
end.tm_hour=TKFIELD.ihour[1];
end.tm_mday=TKFIELD.iday[1];
end.tm_mon=TKFIELD.imon[1];
end.tm_year=TKFIELD.iyear[1];








int ssize=sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile)-sizeof(TKFIELD.iniok);
 char FieldMapName[100];    
 if(strstr(getsetup(),"AMS02D") ){    
   sprintf(FieldMapName,"MagneticFieldMapD");
 }
 else if(strstr(getsetup(),"AMS02PreAss")){
   sprintf(FieldMapName,"MagneticFieldMap09A");
   ssize=3309268-16;
   
   MAGSFFKEY.rphi=1;
 }
 else if(strstr(getsetup(),"AMS02P")){
  sprintf(FieldMapName,"MagneticFieldMapP");
   MAGSFFKEY.rphi=0;
   MAGSFFKEY.magstat=1;
   MAGSFFKEY.fscale=1;
   MISCFFKEY.BTempCorrection=1;
}

///*
 else{
  ssize=3309268-16;

   sprintf(FieldMapName,"MagneticFieldMap09A");
   MAGSFFKEY.rphi=1;
//   sprintf(FieldMapName,"MagneticFieldMap07");
 }
//*/
/*
 else{
   MAGSFFKEY.rphi=0;
   sprintf(FieldMapName,"MagneticFieldMap07");
 }
*/
{

//
// Magnetic field map status
//



  tm btm;
  tm etm;
  time_t begin,end;
  AMSTimeID *ptdv;
 if(strcmp(getsetup(),"AMS02P")){
  if(AMSFFKEY.Update==101){
    begin=MAGSFFKEY.begin;
    end=MAGSFFKEY.end>MAGSFFKEY.begin?MAGSFFKEY.end:MAGSFFKEY.begin+2e7;
    btm=*(localtime(&begin));
    etm=*(localtime(&end));   
    ptdv= new AMSTimeID(AMSID("MagneticFieldStatus",isRealData()),btm,etm,sizeof(MAGSFFKEY_DEF),(void*)&(MAGSFFKEY.magstat),server,1);
    ptdv->UpdateMe()=1;
    ptdv->UpdCRC();
    TID.add(ptdv);
    return;
  }
  else if(isRealData() ){
    begin=100000000;
    end=begin-1;
    btm=*(localtime(&begin));
    etm=*(localtime(&end));   
    ptdv= new AMSTimeID(AMSID("MagneticFieldStatus",isRealData()),btm,etm,sizeof(MAGSFFKEY_DEF),(void*)&(MAGSFFKEY.magstat),server,1);
    TID.add(ptdv);
  }
 }
 }

//PZMAG 
#ifdef _PGTRACK_
 MagField*  pp= MagField::GetPtr();

//  TID.add(new 
// 	 AMSTimeID(AMSID(FieldMapName,isRealData()),
// 		   begin,end,pp->GetSizeForDB(),        
// 		   (void*)pp->GetPointerForDB(),
//                 server,1));
 
 char bmap_fname[50];

 if(strstr(getsetup(),"AMS02P")){
 
   sprintf( bmap_fname,"MagneticFieldMapPermanent_NEW.bin");
   //sprintf( bmap_fname,"MagneticFieldMapPermanent.bin");
   pp->SetScale(1.);
 
 }else{
   sprintf( bmap_fname,"MagneticFieldMap07.bin");
   //set scale because fld07 map is at 460 A and we want 400 A
   pp->SetScale(400./460.);
 }
 char name[200];
 sprintf(name,"%s/%s",AMSDATADIR.amsdatadir,bmap_fname);
 pp->Read(name);

#else
 TID.add (new AMSTimeID(AMSID(FieldMapName,isRealData()),
                        begin,end,ssize,
                        (void*)TKFIELD.isec,server,1));

#endif

//TID.add (new AMSTimeID(AMSID("MagneticFieldMapAddOn",isRealData()),
//   begin,end,sizeof(TKFIELDADDON_DEF),(void*)&TKFIELDADDON.iqx,server));
}
bool NeededByDefault=isSimulation();
//----------------------------
//
// Pedestals, Gains,  Sigmas & commons noise for tracker
//      

#ifdef _PGTRACK_

  {
     tm begin;
     tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;    
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
   
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;
   
   
    int need=1;
    //if(isMonitoring())need=0;
    /// PZ NO !!! Calibrations must alwys be loaded !!!     if(isRealData())
    TID.add (new AMSTimeID(AMSID("TrackerCals",isRealData()),begin,end,
                           TrCalDB::GetLinearSize(),TrCalDB::linear,
                           server,need,SLin2CalDB));
    begin.tm_isdst=0;
    end.tm_isdst=0;    
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
   
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;
    TkDBc::CreateLinear();

    if(isRealData()) {
     if (TKGEOMFFKEY.ReadGeomFromFile/10==1)
       cout << "AMSJob::timeinitjob-I-TrackerAlign is NOT added to TimeID"
            << endl;
     else{
       if(TkDBc::Head->GetSetup()==3)
	 TID.add (new AMSTimeID(AMSID("TrackerAlignPM",isRealData()),begin,end,
                             TkDBc::GetLinearSize(),TkDBc::linear,
				server,need,SLin2Align));
       else
	 TID.add (new AMSTimeID(AMSID("TrackerAlign",isRealData()),begin,end,
				TkDBc::GetLinearSize(),TkDBc::linear,
				server,need,SLin2Align));
     }
    }
    
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;
    // if(isRealData()) 
    TID.add (new AMSTimeID(AMSID("TrackerPars",isRealData()),begin,end,
                           TrParDB::GetLinearSize(),TrParDB::linear,
                           server,need,SLin2ParDB));
    

    if (isRealData() && TrRecon::TasRecon) {
      begin.tm_isdst=0;
      begin.tm_sec  =0;
      begin.tm_min  =0;
      begin.tm_hour =0;
      begin.tm_mday =0;
      begin.tm_mon  =0;
      begin.tm_year =0;
   
      end.tm_isdst=0;
      end.tm_sec  =0;
      end.tm_min  =0;
      end.tm_hour =0;
      end.tm_mday =0;
      end.tm_mon  =0;
      end.tm_year =0;
      
      TID.add (new AMSTimeID(AMSID("TrackerTasPar",isRealData()),begin,end,
			     TrTasDB::GetLinearSize(),TrTasDB::linear,
 			     server,need,SLin2TasDB));
    }
   }

#else
{
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=TRMCFFKEY.sec[0];
begin.tm_min=TRMCFFKEY.min[0];
begin.tm_hour=TRMCFFKEY.hour[0];
begin.tm_mday=TRMCFFKEY.day[0];
begin.tm_mon=TRMCFFKEY.mon[0];
begin.tm_year=TRMCFFKEY.year[0];

end.tm_sec=TRMCFFKEY.sec[1];
end.tm_min=TRMCFFKEY.min[1];
end.tm_hour=TRMCFFKEY.hour[1];
end.tm_mday=TRMCFFKEY.day[1];
end.tm_mon=TRMCFFKEY.mon[1];
end.tm_year=TRMCFFKEY.year[1];
int need=1;
if(isMonitoring())need=0;
if(!strcmp(getsetup(),"AMS02P")){
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numel,
    (void*)AMSTrIdSoft::peds,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numel,
    (void*)AMSTrIdSoft::sigmaraws,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numel,
    (void*)AMSTrIdSoft::gains,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numel,
    (void*)AMSTrIdSoft::sigmas,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numel,
    (void*)AMSTrIdSoft::status,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerCmnNoise(),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::cmnnoise),
    (void*)AMSTrIdSoft::cmnnoise,server,need));
}
else{
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::peds,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell),server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmaraws,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell),server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::gains,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell),server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmas,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell),server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::status,server,need));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell),server,need));
 /*
 TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.l",isRealData()),
    begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*AMSTrIdSoft::_numell*2,
    (void*)AMSTrIdSoft::rhomatrix,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.r",isRealData()),
    begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*
    (2*AMSTrIdSoft::_numel-2*AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::rhomatrix+2*AMSTrIdSoft::_numell),server,NeededByDefault));
 */
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerCmnNoise(),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::cmnnoise),
    (void*)AMSTrIdSoft::cmnnoise,server,need));
//TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
//   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
//   (void*)AMSTrIdSoft::indnoise,server,NeededByDefault));
}
}
#endif




//----------------------------
//
// Pedestals, Gains,  Sigmas & commons noise for trd
//      

{
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=TRDMCFFKEY.sec[0];
begin.tm_min=TRDMCFFKEY.min[0];
begin.tm_hour=TRDMCFFKEY.hour[0];
begin.tm_mday=TRDMCFFKEY.day[0];
begin.tm_mon=TRDMCFFKEY.mon[0];
begin.tm_year=TRDMCFFKEY.year[0];

end.tm_sec=TRDMCFFKEY.sec[1];
end.tm_min=TRDMCFFKEY.min[1];
end.tm_hour=TRDMCFFKEY.hour[1];
end.tm_mday=TRDMCFFKEY.day[1];
end.tm_mon=TRDMCFFKEY.mon[1];
end.tm_year=TRDMCFFKEY.year[1];
 TID.add (new AMSTimeID(AMSID("TRDPedestals",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_ped[0])*AMSTRDIdSoft::getpedsize(),
    (void*)AMSTRDIdSoft::_ped,server,1));
 TID.add (new AMSTimeID(AMSID("TRDGains",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_gain[0])*AMSTRDIdSoft::getgaisize(),
    (void*)AMSTRDIdSoft::_gain,server,1));
 TID.add (new AMSTimeID(AMSID("TRDSigmas",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_sig[0])*AMSTRDIdSoft::getsigsize(),
    (void*)AMSTRDIdSoft::_sig,server,1));
 TID.add (new AMSTimeID(AMSID("TRDStatus",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_status[0])*AMSTRDIdSoft::getstasize(),
    (void*)AMSTRDIdSoft::_status,server,1));
}





//---------------------------------------
//
//   TOF : TDV-reservation for calibration parameters of all sc.bars
//
{
 tm begin;
 tm end;
 begin.tm_isdst=0;
 end.tm_isdst=0;
 int needval=1;
//
if((isCalibration() && CTOF) && AMSFFKEY.Update>0){//only for RD 
  if(TFREFFKEY.relogic[0]==6)needval=0;//only for ds tof-peds to DB
  time_t bdbw=MISCFFKEY.dbwrbeg;
  time_t edbw=MISCFFKEY.dbwrend;
  int jobt=AMSFFKEY.Jobtype;
  int need=0;
  if(jobt==211){//real data reco/calib
    tm * begdbw = localtime(& bdbw);
    TFREFFKEY.sec[0]=begdbw->tm_sec;
    TFREFFKEY.min[0]=begdbw->tm_min;
    TFREFFKEY.hour[0]=begdbw->tm_hour;
    TFREFFKEY.day[0]=begdbw->tm_mday;
    TFREFFKEY.mon[0]=begdbw->tm_mon;
    TFREFFKEY.year[0]=begdbw->tm_year;
    begin.tm_isdst=begdbw->tm_isdst;//to bypass isdst-problem 
    cout<<" Beg YY-M-D :"<<(TFREFFKEY.year[0]+1900)<<" "<<(TFREFFKEY.mon[0]+1)<<" "<<TFREFFKEY.day[0];
    cout<<"  hh:mm:ss :"<<TFREFFKEY.hour[0]<<" "<<TFREFFKEY.min[0]<<" "<<TFREFFKEY.sec[0]<<endl;
//
    tm * enddbw = localtime(& edbw);
    TFREFFKEY.sec[1]=enddbw->tm_sec;
    TFREFFKEY.min[1]=enddbw->tm_min;
    TFREFFKEY.hour[1]=enddbw->tm_hour;
    TFREFFKEY.day[1]=enddbw->tm_mday;
    TFREFFKEY.mon[1]=enddbw->tm_mon;
    TFREFFKEY.year[1]=enddbw->tm_year;
    end.tm_isdst=enddbw->tm_isdst;
    cout<<" End YY-M-D :"<<(TFREFFKEY.year[1]+1900)<<" "<<(TFREFFKEY.mon[1]+1)<<" "<<TFREFFKEY.day[1];
    cout<<"  hh:mm:ss :"<<TFREFFKEY.hour[1]<<" "<<TFREFFKEY.min[1]<<" "<<TFREFFKEY.sec[1]<<endl;
  }
}
//
  begin.tm_sec=TFREFFKEY.sec[0];
  begin.tm_min=TFREFFKEY.min[0];
  begin.tm_hour=TFREFFKEY.hour[0];
  begin.tm_mday=TFREFFKEY.day[0];
  begin.tm_mon=TFREFFKEY.mon[0];
  begin.tm_year=TFREFFKEY.year[0];

  end.tm_sec=TFREFFKEY.sec[1];
  end.tm_min=TFREFFKEY.min[1];
  end.tm_hour=TFREFFKEY.hour[1];
  end.tm_mday=TFREFFKEY.day[1];
  end.tm_mon=TFREFFKEY.mon[1];
  end.tm_year=TFREFFKEY.year[1];
//-----
//tfre->LQDPC,tfmc->LPTS
//-----
if(TFREFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//(C)BarCalibConst from DB

  TID.add (new AMSTimeID(AMSID("Tofbarcal2",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOF2Brcal::scbrcal[0][0]),
    (void*)&TOF2Brcal::scbrcal[0][0],server,needval));
  end.tm_year=TFREFFKEY.year[1];
//-----
if((TFREFFKEY.ReadConstFiles%1000)/100==0)end.tm_year=TFREFFKEY.year[0]-1;//(D)ThreshCuts-set from DB

  TID.add (new AMSTimeID(AMSID("Tofvpar2",isRealData()),
    begin,end,sizeof(TOF2Varp::tofvpar),
    (void*)&TOF2Varp::tofvpar,server,needval));
    
  end.tm_year=TFREFFKEY.year[1];
//----- 
if((TFREFFKEY.ReadConstFiles%10000)/1000==0)end.tm_year=TFREFFKEY.year[0]-1;//(Q)ChargeCalibPDFs from DB

  TID.add (new AMSTimeID(AMSID("Tofcpdfs",isRealData()),
    begin,end,sizeof(TofElosPDF::TofEPDFs),
    (void*)&TofElosPDF::TofEPDFs[0],server,needval));
    
  TID.add (new AMSTimeID(AMSID("ChargeIndxTof",isRealData()),
    begin,end,MaxZTypes*sizeof(AMSCharge::_chargeTOF[0]),
    (void*)AMSCharge::_chargeTOF,server,needval));
   
  end.tm_year=TFREFFKEY.year[1];
//----- 
if((TFREFFKEY.ReadConstFiles%100)/10==0 &&
    isRealData())end.tm_year=TFREFFKEY.year[0]-1;//Real data Peds.fromDB
if((TFMCFFKEY.ReadConstFiles%1000)/100==0 &&
     !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//MC data Peds.fromDB
     
  TID.add (new AMSTimeID(AMSID("Tofpeds",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOFBPeds::scbrped[0][0]),
    (void*)&TOFBPeds::scbrped[0][0],server,needval));
   
  end.tm_year=TFREFFKEY.year[1];
//----- 
if((TFMCFFKEY.ReadConstFiles%100)/10==0 && !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//(T)MCTimeDistr.fromDB

  TID.add (new AMSTimeID(AMSID("Tofmcscans2",0),
    begin,end,TOF2GC::SCBTPN*sizeof(TOFWScan::scmcscan[0]),
    (void*)&TOFWScan::scmcscan[0],server,needval));
    
  end.tm_year=TFREFFKEY.year[1];
//-----
if(!isRealData()){ 
  if(TFMCFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//(S)BarCal "MC Seeds" fromDB
    
  TID.add (new AMSTimeID(AMSID("TofbarcalMS",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOFBrcalMS::scbrcal[0][0]),
    (void*)&TOFBrcalMS::scbrcal[0][0],server,needval));
    
  end.tm_year=TFREFFKEY.year[1];
}
//-----
integer TOFCRSL=TOF2GC::SCCRAT*(TOF2GC::SCFETA-1);
if(!isRealData() && TFMCFFKEY.tdclin>0){ 
  if( TFMCFFKEY.ReadConstFiles/1000==0)end.tm_year=TFREFFKEY.year[0]-1;//(L)TofTdcCor "MC Seeds" fromDB
    
  TID.add (new AMSTimeID(AMSID("TofTdcCorMS",isRealData()),
    begin,end,TOFCRSL*sizeof(TofTdcCorMS::tdccor[0][0]),
    (void*)&TofTdcCorMS::tdccor[0][0],server,needval));
    
  end.tm_year=TFREFFKEY.year[1];
}
//----- 
if(isRealData() && 
   (TFREFFKEY.ReadConstFiles/10000)==0)end.tm_year=TFREFFKEY.year[0]-1;//(L)TofTdcCor(RD-reco) from DB
   
if(!isRealData() && (TFMCFFKEY.tdclin > 0) &&
   (TFREFFKEY.ReadConstFiles/10000)==0)end.tm_year=TFREFFKEY.year[0]-1;//(L)TofTdcCor(MC-reco) from DB

  TID.add (new AMSTimeID(AMSID("TofTdcCor",isRealData()),
    begin,end,TOFCRSL*sizeof(TofTdcCor::tdccor[0][0]),
    (void*)&TofTdcCor::tdccor[0][0],server,needval));
    
  end.tm_year=TFREFFKEY.year[1];
//
   
}
//---------------------------------------
//
//   LVL1 : TDV-reservation for Lvl1Config data
//
{
 tm begin;
 tm end;
 begin.tm_isdst=0;
 end.tm_isdst=0;


  begin.tm_sec=TGL1FFKEY.sec[0];
  begin.tm_min=TGL1FFKEY.min[0];
  begin.tm_hour=TGL1FFKEY.hour[0];
  begin.tm_mday=TGL1FFKEY.day[0];
  begin.tm_mon=TGL1FFKEY.mon[0];
  begin.tm_year=TGL1FFKEY.year[0];

  end.tm_sec=TGL1FFKEY.sec[1];
  end.tm_min=TGL1FFKEY.min[1];
  end.tm_hour=TGL1FFKEY.hour[1];
  end.tm_mday=TGL1FFKEY.day[1];
  end.tm_mon=TGL1FFKEY.mon[1];
  end.tm_year=TGL1FFKEY.year[1];
//-----
//tgl1->MN
//-----
if(TGL1FFKEY.Lvl1ConfRead%10==0)end.tm_year=TGL1FFKEY.year[0]-1;//(N)Lvl1Config-data from DB
  
  TID.add (new AMSTimeID(AMSID("Lvl1Config",isRealData()),
    begin,end,sizeof(Trigger2LVL1::l1trigconf),
    (void*)&Trigger2LVL1::l1trigconf,server,1));
   
  end.tm_year=TGL1FFKEY.year[1];
}
//---------------------------------------
//
//   ANTI : TDV-reservation for calibration parameters of all sc.sectors
//
{
 tm begin;
 tm end;
 begin.tm_isdst=0;
 end.tm_isdst=0;
 int needval=1;
 
if((isCalibration() && CAnti) && AMSFFKEY.Update>0){
  if(ATREFFKEY.relogic==3)needval=0;//only for ds acc-peds to DB
  time_t bdbw=MISCFFKEY.dbwrbeg;
  time_t edbw=MISCFFKEY.dbwrend;
  int jobt=AMSFFKEY.Jobtype;
  if(jobt==311){//real data reco/calib
    tm * begdbw = localtime(& bdbw);
    ATREFFKEY.sec[0]=begdbw->tm_sec;
    ATREFFKEY.min[0]=begdbw->tm_min;
    ATREFFKEY.hour[0]=begdbw->tm_hour;
    ATREFFKEY.day[0]=begdbw->tm_mday;
    ATREFFKEY.mon[0]=begdbw->tm_mon;
    ATREFFKEY.year[0]=begdbw->tm_year;
    begin.tm_isdst=begdbw->tm_isdst;//to bypass isdst-problem 
    cout<<" Beg YY-M-D :"<<(ATREFFKEY.year[0]+1900)<<" "<<(ATREFFKEY.mon[0]+1)<<" "<<ATREFFKEY.day[0];
    cout<<"  hh:mm:ss :"<<ATREFFKEY.hour[0]<<" "<<ATREFFKEY.min[0]<<" "<<ATREFFKEY.sec[0]<<endl;
//
    tm * enddbw = localtime(& edbw);
    ATREFFKEY.sec[1]=enddbw->tm_sec;
    ATREFFKEY.min[1]=enddbw->tm_min;
    ATREFFKEY.hour[1]=enddbw->tm_hour;
    ATREFFKEY.day[1]=enddbw->tm_mday;
    ATREFFKEY.mon[1]=enddbw->tm_mon;
    ATREFFKEY.year[1]=enddbw->tm_year;
    end.tm_isdst=enddbw->tm_isdst;
    cout<<" End YY-M-D :"<<(ATREFFKEY.year[1]+1900)<<" "<<(ATREFFKEY.mon[1]+1)<<" "<<ATREFFKEY.day[1];
    cout<<"  hh:mm:ss :"<<ATREFFKEY.hour[1]<<" "<<ATREFFKEY.min[1]<<" "<<ATREFFKEY.sec[1]<<endl;
  }
}

  begin.tm_sec=ATREFFKEY.sec[0];
  begin.tm_min=ATREFFKEY.min[0];
  begin.tm_hour=ATREFFKEY.hour[0];
  begin.tm_mday=ATREFFKEY.day[0];
  begin.tm_mon=ATREFFKEY.mon[0];
  begin.tm_year=ATREFFKEY.year[0];

  end.tm_sec=ATREFFKEY.sec[1];
  end.tm_min=ATREFFKEY.min[1];
  end.tm_hour=ATREFFKEY.hour[1];
  end.tm_mday=ATREFFKEY.day[1];
  end.tm_mon=ATREFFKEY.mon[1];
  end.tm_year=ATREFFKEY.year[1];
//---------
//atre->PVS, atmc->SeedPed|RealPed
if(ATREFFKEY.ReadConstFiles%10==0)end.tm_year=ATREFFKEY.year[0]-1;//StableParams from DB
//
  TID.add (new AMSTimeID(AMSID("Antispcal",isRealData()),
     begin,end,ANTI2C::MAXANTI*sizeof(ANTI2SPcal::antispcal[0]),
                         (void*)&ANTI2SPcal::antispcal[0],server,needval));
//
  end.tm_year=ATREFFKEY.year[1];
//---------
if((ATREFFKEY.ReadConstFiles/10)%10==0)end.tm_year=ATREFFKEY.year[0]-1;//VariableParams from DB
//
  TID.add (new AMSTimeID(AMSID("Antivpcal",isRealData()),
     begin,end,ANTI2C::MAXANTI*sizeof(ANTI2VPcal::antivpcal[0]),
                         (void*)&ANTI2VPcal::antivpcal[0],server,needval));
//
  end.tm_year=ATREFFKEY.year[1];
//---------
if(ATREFFKEY.ReadConstFiles/100==0 &&
    isRealData())end.tm_year=ATREFFKEY.year[0]-1;//Real data Peds. fromDB
    
if(ATMCFFKEY.ReadConstFiles%10==0 &&
   !isRealData())end.tm_year=ATREFFKEY.year[0]-1;//MC data RealPeds. fromDB
//
  TID.add (new AMSTimeID(AMSID("Antipeds",isRealData()),
    begin,end,ANTI2C::MAXANTI*sizeof(ANTIPeds::anscped[0]),
                            (void*)&ANTIPeds::anscped[0],server,needval));
  end.tm_year=ATREFFKEY.year[1];
//---------
if(ATMCFFKEY.ReadConstFiles/10==0 &&
   !isRealData())end.tm_year=ATREFFKEY.year[0]-1;//MC data SeedPeds. fromDB
//
  TID.add (new AMSTimeID(AMSID("AntipedsMS",isRealData()),
    begin,end,ANTI2C::MAXANTI*sizeof(ANTIPedsMS::anscped[0]),
                            (void*)&ANTIPedsMS::anscped[0],server,needval));
  end.tm_year=ATREFFKEY.year[1];
//---------
//
}
//---------------------------------------

{ //RICH: TDV reservation for calibration parameters of all 
  tm begin;
  tm end;
  begin.tm_isdst=0;
  end.tm_isdst=0;
  
  begin.tm_sec=RICFFKEY.sec[0];
  begin.tm_min=RICFFKEY.min[0];
  begin.tm_hour=RICFFKEY.hour[0];
  begin.tm_mday=RICFFKEY.day[0];
  begin.tm_mon=RICFFKEY.mon[0];
  begin.tm_year=RICFFKEY.year[0];


  end.tm_sec=RICFFKEY.sec[1];
  end.tm_min=RICFFKEY.min[1];
  end.tm_hour=RICFFKEY.hour[1];
  end.tm_mday=RICFFKEY.day[1];
  end.tm_mon=RICFFKEY.mon[1];
  end.tm_year=RICFFKEY.year[1];

  TID.add (new AMSTimeID(AMSID("RichPMTChannelStatus",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows
			 *sizeof(RichPMTsManager::_status[0]),
                         (void*)&RichPMTsManager::_status[0],server,1));

  // A Mask to allow masking out few channels for the reconstruction
  TID.add (new AMSTimeID(AMSID("RichPMTChannelMask",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows
			 *sizeof(RichPMTsManager::_mask[0]),
                         (void*)&RichPMTsManager::_mask[0],server,1));

  TID.add (new AMSTimeID(AMSID("RichPMTChannelPedestal",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows*2
			 *sizeof(RichPMTsManager::_pedestal[0]),
                         (void*)&RichPMTsManager::_pedestal[0],server,isRealData()));

  TID.add (new AMSTimeID(AMSID("RichPMTChannelPedestalSigma",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows*2
			 *sizeof(RichPMTsManager::_pedestal_sigma[0]),
                         (void*)&RichPMTsManager::_pedestal_sigma[0],server,isRealData()));

  TID.add (new AMSTimeID(AMSID("RichPMTChannelPedestalThreshold",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows*2
			 *sizeof(RichPMTsManager::_pedestal_threshold[0]),
                         (void*)&RichPMTsManager::_pedestal_threshold[0],server,isRealData()));


  TID.add (new AMSTimeID(AMSID("RichPMTChannelGain",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows*2
			 *sizeof(RichPMTsManager::_gain[0]),
                         (void*)&RichPMTsManager::_gain[0],server,1));

  TID.add (new AMSTimeID(AMSID("RichPMTChannelGainSigma",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows*2
			 *sizeof(RichPMTsManager::_gain_sigma[0]),
                         (void*)&RichPMTsManager::_gain_sigma[0],server,isRealData()));

  TID.add (new AMSTimeID(AMSID("RichPMTChannelGainThreshold",isRealData()),
                         begin,end,RICmaxpmts*RICnwindows
			 *sizeof(RichPMTsManager::_gain_sigma[0]),
                         (void*)&RichPMTsManager::_gain_sigma[0],server,isRealData()));
  

  // Further TDV  
  begin.tm_isdst=0;
  end.tm_isdst=0;
  
  begin.tm_sec=RICDBFFKEY.sec[0];
  begin.tm_min=RICDBFFKEY.min[0];
  begin.tm_hour=RICDBFFKEY.hour[0];
  begin.tm_mday=RICDBFFKEY.day[0];
  begin.tm_mon=RICDBFFKEY.mon[0];
  begin.tm_year=RICDBFFKEY.year[0];


  end.tm_sec=RICDBFFKEY.sec[1];
  end.tm_min=RICDBFFKEY.min[1];
  end.tm_hour=RICDBFFKEY.hour[1];
  end.tm_mday=RICDBFFKEY.day[1];
  end.tm_mon=RICDBFFKEY.mon[1];
  end.tm_year=RICDBFFKEY.year[1];

  int use_radiator=(RICDBFFKEY.dump%10)==0 && 
    !strstr(AMSJob::gethead()->getsetup(),"PreAss");

  int use_alignment=isRealData() && ((RICDBFFKEY.dump/10)%10)==0 &&
    !strstr(AMSJob::gethead()->getsetup(),"PreAss");

  AMSTimeID *pdtv;

  pdtv=(AMSTimeID*) TID.add  (new AMSTimeID(AMSID("RichRadTilesParameters",isRealData()),
                         begin,end,
			 RICmaxtiles*4*sizeof(RichRadiatorTileManager::_optical_parameters[0]),
			 (void*)&RichRadiatorTileManager::_optical_parameters[0],
			 server,use_radiator));

  pdtv=(AMSTimeID*) TID.add (new AMSTimeID(AMSID("RichAlignmentParameters",isRealData()),
                         begin,end,
			 12*sizeof(RichAlignment::_align_parameters[0]),
			 (void*)&RichAlignment::_align_parameters[0],
			 server,use_alignment));


  
}




//------------------------------------------
//
//   ECAL : TDV-reservation for calibration parameters of all PM's:
//
{
  tm begin;
  tm end;
  begin.tm_isdst=0;
  end.tm_isdst=0;
 int needval=1;
//
if((isCalibration() && CEcal) && AMSFFKEY.Update>0){//only for 
  if(ECREFFKEY.relogic[1]==5)needval=0;//only for ds ECAL-peds to DB
  time_t bdbw=MISCFFKEY.dbwrbeg;
  time_t edbw=MISCFFKEY.dbwrend;
  int jobt=AMSFFKEY.Jobtype;
  int need=0;
  if(jobt==911){//real data reco/calib
    tm * begdbw = localtime(& bdbw);
    ECREFFKEY.sec[0]=begdbw->tm_sec;
    ECREFFKEY.min[0]=begdbw->tm_min;
    ECREFFKEY.hour[0]=begdbw->tm_hour;
    ECREFFKEY.day[0]=begdbw->tm_mday;
    ECREFFKEY.mon[0]=begdbw->tm_mon;
    ECREFFKEY.year[0]=begdbw->tm_year;
    begin.tm_isdst=begdbw->tm_isdst;//to bypass isdst-problem 
    cout<<" Beg YY-M-D :"<<(ECREFFKEY.year[0]+1900)<<" "<<(ECREFFKEY.mon[0]+1)<<" "<<ECREFFKEY.day[0];
    cout<<"  hh:mm:ss :"<<ECREFFKEY.hour[0]<<" "<<ECREFFKEY.min[0]<<" "<<ECREFFKEY.sec[0]<<endl;
//
    tm * enddbw = localtime(& edbw);
    ECREFFKEY.sec[1]=enddbw->tm_sec;
    ECREFFKEY.min[1]=enddbw->tm_min;
    ECREFFKEY.hour[1]=enddbw->tm_hour;
    ECREFFKEY.day[1]=enddbw->tm_mday;
    ECREFFKEY.mon[1]=enddbw->tm_mon;
    ECREFFKEY.year[1]=enddbw->tm_year;
    end.tm_isdst=enddbw->tm_isdst;
    cout<<" End YY-M-D :"<<(ECREFFKEY.year[1]+1900)<<" "<<(ECREFFKEY.mon[1]+1)<<" "<<ECREFFKEY.day[1];
    cout<<"  hh:mm:ss :"<<ECREFFKEY.hour[1]<<" "<<ECREFFKEY.min[1]<<" "<<ECREFFKEY.sec[1]<<endl;
  }
}
//
  begin.tm_sec=ECREFFKEY.sec[0];
  begin.tm_min=ECREFFKEY.min[0];
  begin.tm_hour=ECREFFKEY.hour[0];
  begin.tm_mday=ECREFFKEY.day[0];
  begin.tm_mon=ECREFFKEY.mon[0];
  begin.tm_year=ECREFFKEY.year[0];

  end.tm_sec=ECREFFKEY.sec[1];
  end.tm_min=ECREFFKEY.min[1];
  end.tm_hour=ECREFFKEY.hour[1];
  end.tm_mday=ECREFFKEY.day[1];
  end.tm_mon=ECREFFKEY.mon[1];
  end.tm_year=ECREFFKEY.year[1];
//--------
//ecre->DCP; ecmc->CP
//
if((ECREFFKEY.ReadConstFiles%100)/10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib(MC/RD).fromDB


  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalib(),
     begin,end,ecalconst::ECPMSL*sizeof(ECcalib::ecpmcal[0][0]),
                                  (void*)&ECcalib::ecpmcal[0][0],server,needval));

  end.tm_year=ECREFFKEY.year[1];
//--------                                
if((ECREFFKEY.ReadConstFiles/100)==0)end.tm_year=ECREFFKEY.year[0]-1;//DataCardThresh/Cuts fromDB

  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVvpar(),
     begin,end,sizeof(ECALVarp::ecalvpar),
                                      (void*)&ECALVarp::ecalvpar,server,needval));
  end.tm_year=ECREFFKEY.year[1];
//--------
if(!isRealData()){//"MC.Seeds" TDV only for MC-run.    
  if((ECMCFFKEY.ReadConstFiles%100)/10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib"MC.Seeds" fromDB
             

  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalibMS(),
     begin,end,ecalconst::ECPMSL*sizeof(ECcalibMS::ecpmcal[0][0]),
                                  (void*)&ECcalibMS::ecpmcal[0][0],server,needval));

  end.tm_year=ECREFFKEY.year[1];
}
//--------
if(ECREFFKEY.ReadConstFiles%10==0 &&
              isRealData())end.tm_year=ECREFFKEY.year[0]-1;//Real data Peds.fromDB
if(ECMCFFKEY.ReadConstFiles%10==0 &&
             !isRealData())end.tm_year=ECREFFKEY.year[0]-1;//MC Peds fromDB
	     
  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVped(),
    begin,end,ecalconst::ECPMSL*sizeof(ECPMPeds::pmpeds[0][0]),
                                    (void*)&ECPMPeds::pmpeds[0][0],server,needval));
  end.tm_year=ECREFFKEY.year[1];
//--------
}
//
//---------------------------------------
//
// Data to fit particle charge magnitude(now for tracker only)
//
#ifndef _PGTRACK_
  //PZ FIXME CHARGE
{
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=CHARGEFITFFKEY.sec[0];
begin.tm_min=CHARGEFITFFKEY.min[0];
begin.tm_hour=CHARGEFITFFKEY.hour[0];
begin.tm_mday=CHARGEFITFFKEY.day[0];
begin.tm_mon=CHARGEFITFFKEY.mon[0];
begin.tm_year=CHARGEFITFFKEY.year[0];

end.tm_sec=CHARGEFITFFKEY.sec[1];
end.tm_min=CHARGEFITFFKEY.min[1];
end.tm_hour=CHARGEFITFFKEY.hour[1];
end.tm_mday=CHARGEFITFFKEY.day[1];
end.tm_mon=CHARGEFITFFKEY.mon[1];
end.tm_year=CHARGEFITFFKEY.year[1];

if(CHARGEFITFFKEY.TrkPDFileRead==0)end.tm_year=CHARGEFITFFKEY.year[0]-1;//ChargeCalibPDFs from DB

  TID.add (new AMSTimeID(AMSID("Trkcpdfs",isRealData()),
    begin,end,sizeof(TrkElosPDF::TrkEPDFs),
    (void*)&TrkElosPDF::TrkEPDFs[0],server,1));
    
  TID.add (new AMSTimeID(AMSID("ChargeIndxTrk",isRealData()),
    begin,end,MaxZTypes*sizeof(AMSCharge::_chargeTracker[0]),
    (void*)AMSCharge::_chargeTracker,server,1));
    
  end.tm_year=CHARGEFITFFKEY.year[1];




}
#endif
//---------------------------
{
  // TOF Slow Temperature  data (some PMTs inside of TOF-envelops + SFEC)
  if(isRealData()){
   tm begin=AMSmceventg::Orbit.End;
   tm end=AMSmceventg::Orbit.Begin;
   TID.add (new AMSTimeID(AMSID("TofEnvelopsTemper",isRealData()),
                  begin,end,
                  sizeof(TofSlowTemp::tofstemp),(void*)&TofSlowTemp::tofstemp,server,0));
  }  
}
//-----------------------------
// { PZ FIXME OBSOLETE
//   // Magnet Temperature data

//    tm begin=AMSmceventg::Orbit.End;
//    tm end=AMSmceventg::Orbit.Begin;
//     if(strstr(getsetup(),"AMSSHUTTLE") ){    
//    TID.add (new AMSTimeID(AMSID("MagnetTemperature",isRealData()),
//                          begin,end,
//                          MagnetVarp::getmagnettsize(),(void*)MagnetVarp::getmagnettp(),server));
//     }
   
// }
//-----------------------------
{
  // Scaler Data

   tm begin=AMSmceventg::Orbit.End;
   tm end=AMSmceventg::Orbit.Begin;
    if(strstr(getsetup(),"AMSSHUTTLE") ){    
     TID.add (new AMSTimeID(AMSID("ScalerN",isRealData()),
                         begin,end,
                         Trigger2LVL1::getscalerssize(),(void*)Trigger2LVL1::getscalersp(),server));
    }
   
}
//-----------------------------
{
  tm begin;
  tm end;
  if(AMSFFKEY.Update==87){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
    AMSEvent::SetShuttlePar();
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
    if(strstr(getsetup(),"AMSSHUTTLE") ){    
  TID.add (new AMSTimeID(AMSID("ShuttlePar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::Array),(void*)AMSEvent::Array,server));
   }
}



if(isRealData()){

// CCEB:

  tm begin;
  tm end;
  if(AMSFFKEY.Update==187){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
    AMSEvent::SetCCEBPar();
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
///*
  TID.add (new AMSTimeID(AMSID("CCEBPar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::ArrayC),(void*)AMSEvent::ArrayC,server));
//*/


// New TofSlowTemperature:
/*
  if(AMSFFKEY.Update==287){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
    AMSEvent::SetTofSTemp();
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
   begin=AMSmceventg::Orbit.Begin;//tempor to not read DB
   end=AMSmceventg::Orbit.End;//tempor
   AMSEvent::SetTofSTemp();
  }
  TID.add (new AMSTimeID(AMSID("UTofSTempPar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::UTofTemp),(void*)AMSEvent::UTofTemp,server));
  TID.add (new AMSTimeID(AMSID("LTofSTempPar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::LTofTemp),(void*)AMSEvent::LTofTemp,server));
*/
}



if(MISCFFKEY.BeamTest>1){
  tm begin=AMSmceventg::Orbit.End;
  tm end=AMSmceventg::Orbit.Begin;
  TID.add (new AMSTimeID(AMSID("BeamPar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::ArrayB),(void*)AMSEvent::ArrayB,server));
}





/*
{
  tm begin;
  tm end;
  if(AMSFFKEY.Update==89){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
    if(TRALIG.InitDB)AMSTrAligPar::InitDB();
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
  AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID("TrAligDB",
                          isRealData()),begin,end,AMSTrAligPar::gettraligdbsize(),
                          AMSTrAligPar::gettraligdbp(),server));
}

*/

#ifndef _PGTRACK_
  //PZ FIXME ALIGN
{
  tm begin;
  tm end;
  AMSTrAligFit::InitDB();
  if( TRALIG.UpdateDB ){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
   AMSTimeID * ptdv = (AMSTimeID*) TID.add(new AMSTimeID(strcmp(getsetup(),"AMS02P")?AMSTrAligFit::getTDVGLDB():AMSTrAligFit::getTDVGLDBP(),begin,end,AMSTrAligFit::gettraliggldbsize(),AMSTrAligFit::gettraliggldbp(),server));
   if(TRALIG.ReWriteDB)ptdv->UpdateMe()=1;
}




if(!isRealData()){
  tm begin;
  tm end;
  AMSTrAligFit::InitADB();
  if(TRALIG.UpdateDB>1 ){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }

  AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(strcmp(getsetup(),"AMS02P")?AMSTrAligFit::getTDVAGLDB():AMSTrAligFit::getTDVAGLDBP(),begin,end,AMSTrAligFit::gettraliggladbsize(),AMSTrAligFit::gettraliggladbp(),server));

}


#endif


}

//===============================================================================

AMSTimeID * AMSJob::gettimestructure(){
      AMSID id("TDV:",0);    
     AMSNode *p=JobMap.getp(id);
     if(!p){
      cerr << "AMSJob::gettimestructure-F-no time structure found"<<endl;
      exit(1);
      return 0;
     }
     else return  dynamic_cast<AMSTimeID*>(p);
}
AMSNode * AMSJob::getaligstructure(){
#ifdef _PGTRACK_
  return 0;
#else
      AMSTrAligFit a;
     AMSNode *p=JobMap.getp(AMSID(a.getname(),a.getid()));
     if(!p){
      cerr << "AMSJob::getaligtructure-F-no alig structure found"<<endl;
      exit(1);
      return 0;
     }
     else return  p;
#endif     
}

AMSTimeID * AMSJob::gettimestructure(const AMSID & id){
     AMSNode *p=JobMap.getp(id);
     if(!p){
       cerr << "AMSJob::gettimestructure-F-no time structure found "<<id<<" "<<&JobMap<<endl;
       JobMap.print();
      exit(1);
      return 0;
     }
     else return  dynamic_cast<AMSTimeID*>(p);
}

AMSJob::AMSJob(AMSID id, uinteger jobtype):AMSNode(id),_jobtype(jobtype),_pAMSG4Physics(0),_pAMSG4GeneratorInterface(0),_NtupleActive(false)
{_Setup[0]='\0';_TriggerC[0][0]='\0';_TriggerI=1;_TriggerN=0;
_TDVC[0][0]='\0';
_TDVN=0;
if(_Head){
 cerr<<"AMSJob-ctor-F-OnlyOnejobAlowed"<<endl;
 exit(1);
}
else _Head=this;
cout <<"   AMS Software version "<<AMSCommonsI::getversion()<<"/"<<AMSCommonsI::getbuildno()<<"/"<<AMSCommonsI::getosno()<<" build "<<AMSCommonsI::getbuildctime()<< endl;
}

AMSJob::~AMSJob(){
  cout << "   ~AMSJob called "<<endl;
//  HPRINT(9797+1);
  _signendjob();
  _tkendjob();
  cout <<"   tkendjob finished"<<endl;
    _tof2endjob();
    cout <<"   tof2endjob finished"<<endl;
    _anti2endjob();
    cout <<"   anti2endjob finished"<<endl;
    _ecalendjob();
    cout <<"   ecalendjob finished"<<endl;
    _trdendjob();
    cout <<"   trdendjob finished"<<endl;
    _richendjob();
    cout <<"   richendjob finished"<<endl;

_dbendjob();
  cout <<"   dbendjob finished"<<endl;
_axendjob();
  cout <<"   axendjob finished"<<endl;
//HPRINT(0);
  if(isSimulation()){
      uhend(CCFFKEY.run,GCFLAG.IEVENT,CCFFKEY.curtime);
      DAQEvent::CloseO(CCFFKEY.run,GCFLAG.IEVENT,CCFFKEY.curtime);
  } 
  else uhend(0,0,CCFFKEY.curtime);
  cout <<"   uhend finished"<<endl;


#ifndef __TFADBW__  
 TGL1JobStat::printstat();
 TriggerLVL302::printfc();
#endif
 
#ifdef __CORBA__
if(isSimulation())AMSProducer::gethead()->sendRunEndMC();
#endif
delete _pntuple;
  cout <<"pntuple deleted"<<endl;
#ifdef __CORBA__
delete AMSProducer::gethead();
  cout <<"producer deleted"<<endl;
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G4On){
// due to infinite geant4 destructors
#ifndef G4VIS_USE
//  delete &(AMSgObj::BookTimer);
#endif
//abort();
}
#endif
}
#include "producer.h"
void AMSJob::urend_immediate(){


if(IOPA.WriteRoot  && _NtupleActive ){
  _NtupleActive=false;
if(_pntuple)_pntuple->endR(false);
#ifdef __CORBA__
if(_pntuple){
int root_entries=_pntuple->getentries();
AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,_pntuple->lastev(),_pntuple->lasttime(),true);
}
cout <<"  AMSJob::urend_immediate-F-abort event "<<_pntuple->lastev()<<endl;
AMSProducer::gethead()-> FMessage("AMSProducer::AMSProducer-E-ImmediateShutdown",DPS::Client::CInAbort);

#endif



}
}

void AMSJob::uhend(uinteger r, uinteger e, time_t t){
if(IOPA.hlun && _NtupleActive){
  _NtupleActive=false;
  int ntuple_entries;
  HNOENT(IOPA.ntuple, ntuple_entries);
  cout << " Closing AMS ntuple at unit " << IOPA.hlun << " with ";
  cout << ntuple_entries << " events" << endl << endl;
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[9]="//output";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("output");
  CLOSEF(IOPA.hlun);
#ifdef __CORBA__
  if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,AMSEvent::gethead()->getmid(),AMSEvent::gethead()->getmtime(),true);
else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,e,t,true);
#endif
}
else if(IOPA.WriteRoot  && _NtupleActive ){
  _NtupleActive=false;
if(_pntuple)_pntuple->endR();
#ifdef __CORBA__
if(_pntuple){
int root_entries=_pntuple->getentries();
  if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,AMSEvent::gethead()->getmid(),AMSEvent::gethead()->getmtime(),true);
else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,e,t,true);
}
#endif
}
}
void AMSJob::urinit(char fnam[]){
  if(_pntuple)_pntuple->endR();
    strcpy(_rootfilename,fnam);
    strcat(_rootfilename,".root");
    if(_pntuple)_pntuple->initR(_rootfilename,0);
    else{
        _pntuple = new AMSNtuple(_rootfilename);
        _pntuple->initR(_rootfilename,0);
    }
    _NtupleActive=true;
}

void AMSJob::urinit(integer run, integer eventno, time_t tt)
throw (amsglobalerror){
  if(_pntuple)_pntuple->endR();
  if(IOPA.WriteRoot){
//   trail leading blancs if any
//  
    int offset=-1; 
    for(int i=0;i<strlen((const char*)_rextname);i++){
     if(*((const char*)_rextname+i)!=' ')break;
     else offset=i;
    } 
    strcpy(_rootfilename,(const char*)_rextname+offset+1);
     AString mdir("mkdir -p ");
     mdir+=_rootfilename;
    integer iostat;
    integer rsize=8000;
    if(eventno){
      char event[80];  
      if(isProduction()){
       int i=system((const char*)mdir);
       if(i){
        cerr<<"AMSjob::urinit-E-UnableToWriteNtupleDir "<<(const char *)mdir<<endl;
        if(getenv("NtupleDir") && !strstr((const char*)mdir,getenv("NtupleDir"))){
        mdir="mkdir -p ";
        strcpy(_rootfilename,getenv("NtupleDir"));
        mdir+=_rootfilename;
        system((const char*)mdir);
 }
       }
       strcat(_rootfilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_rootfilename,event);
      sprintf(event,".%08d",eventno);
      strcat(_rootfilename,event);
      strcat(_rootfilename,".root");
     }
    if(_pntuple)_pntuple->initR(_rootfilename,run);
    else{
        _pntuple = new AMSNtuple(_rootfilename);
        _pntuple->initR(_rootfilename,run);
    }
    _NtupleActive=true;
#ifdef __CORBA__
      AMSProducer::gethead()->sendNtupleStart(DPS::Producer::RootFile,_rootfilename,run,eventno,tt);
#endif
  }
}


void AMSJob::uhinit(integer run, integer eventno, time_t tt)
throw (amsglobalerror){
  if(_NtupleActive)uhend();
  if(IOPA.hlun){
   strcpy(_ntuplefilename,(const char *)_hextname);
   AString mdir("mkdir -p ");
   mdir+=_ntuplefilename;
    integer iostat;
    integer rsize=8000;
    if(eventno){
      char event[80];  
      if(isProduction()){
       system((const char*)mdir);
       strcat(_ntuplefilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_ntuplefilename,event);
      sprintf(event,".%08d",eventno);
      strcat(_ntuplefilename,event);
      strcat(_ntuplefilename,".hbk");
     }
     else if(run){
      char event[80];  
      if(isProduction()){
       system((const char*)mdir);
       strcat(_ntuplefilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_ntuplefilename,event);
      strcat(_ntuplefilename,".hbk");
     }
    cout <<"Trying to open histo file "<<_ntuplefilename<<endl;
    npq_(); 
     int beg=-1;
     for (int k=0;k<strlen(_ntuplefilename);k++){
      if(_ntuplefilename[k]!=' ')break;
      beg=k;
     }
     if(beg>=0)strcpy(_ntuplefilename,_ntuplefilename+beg+1);
     HROPEN(IOPA.hlun,"output",_ntuplefilename,"NPQ",rsize,iostat);
    if(iostat){
     cerr << "Error opening Histo file "<<_ntuplefilename<<endl;
     throw amsglobalerror("UnableToOpenHistoFile",3);
    }
    else cout <<"Histo file "<<_ntuplefilename<<" opened."<<endl;

// Open the n-tuple  //  never open the ntuple
/*
    if(_pntuple)_pntuple->init();
    else{
        _pntuple = new AMSNtuple(IOPA.ntuple,"AMS Ntuple");
    }
*/
    _NtupleActive=true;
#ifdef __CORBA__
      AMSProducer::gethead()->sendNtupleStart(DPS::Producer::Ntuple,_ntuplefilename,run,eventno,tt);
#endif
  }
  urinit(run,eventno,tt);
   HBOOK1(200101,"Number of Nois Hits x",100,-0.5,99.5,0.);
   HBOOK1(200102,"Number of Nois Hits y",100,-0.5,99.5,0.);
   HBOOK1(200105,"Above threshold spectrum x",200,-0.5,49.5,0.);
   HBOOK1(200106,"Above threshold spectrum y",200,-0.5,49.5,0.);
//   HBOOK1(200107," adc over",3000,29999.5,32999.5,0.);
}

#ifndef _PGTRACK_
void AMSJob::_tkendjob(){
  if(isCalibration() & CTracker){
    if(TRALIG.UpdateDB){
      if(!TRALIG.GlobalFit)AMSTrAligFit::Test(1);
      else AMSTrAligFit::Testgl(2);
    }
  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check(1);
    AMSTrIdCalib::printbadchanlist();
  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 2){
   int i,j;
for(i=0;i<nalg;i++){
  if(TRCALIB.Method==0 || TRCALIB.Method==i){
   for(j=TRCALIB.PatStart;j<tkcalpat;j++){
     if(AMSTrCalibFit::getHead(i,j)->Test(1)){
      AMSTrCalibFit::getHead(i,j)->Fit();
     }
  }
 }
}

   for(i=0;i<nalg;i++){
   if(TRCALIB.Method==0 || TRCALIB.Method==i){
    for(j=TRCALIB.PatStart;j<tkcalpat;j++){
      AMSTrCalibFit::getHead(i,j)->Anal();
    }
   }
   }
      AMSTrCalibFit::SAnal();

  }
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 3){
    AMSTrIdCalib::ntuple(AMSEvent::getSRun());
  }
  if(isMonitoring() & (AMSJob::MTracker | AMSJob::MAll)){
//   AMSTrIdCalib::offmonhist();    
  }



}
#endif
void AMSJob::_signendjob(){
AMSmceventg::endjob();
}

//
//-------------------------------------------------------------------
void AMSJob::_tof2endjob(){
  if(isSimulation())TOF2JobStat::outpmc();//histogr+some actions
  TOF2JobStat::outp();//histogr+some actions
//
  if((isCalibration() & CTOF) && (TFREFFKEY.relogic[0]==5 || TFREFFKEY.relogic[0]==6)){
    TOFPedCalib::outp(TFCAFFKEY.pedoutf);// 0/1/2->HistOnly/Wr2DB+File/Wr2File 
  }
//
//  if((isCalibration() & CTOF) && TFREFFKEY.relogic[0]==7){
//    TOFPedCalib::ntuple_close();// close OnBoardPedsTable ntuple 
//  }
//
  if((isCalibration() & CTOF) && TFREFFKEY.relogic[0]==1){
    TOFTdcCalib::outp(TFCAFFKEY.tdccum);
  }
//
  integer cmode=TFREFFKEY.relogic[0];
  if((isCalibration() & CTOF) && (cmode==2 || cmode==3 || cmode==4 || cmode==23
                                                       || cmode==234 || cmode==34)){
    TofTmAmCalib::endjob();//Tdelv/Tzslw
  }
//
  if(TFREFFKEY.reprtf[0]>0)TOF2JobStat::printstat(); // Print JOB TOF-statistics
}
//-----------------------------------------------------------------------
void AMSJob::_anti2endjob(){
  ANTI2JobStat::printstat();
  if(isSimulation() && ATMCFFKEY.mcprtf>0)ANTI2JobStat::outpmc();
  ANTI2JobStat::outp();//this routine keep AmplCalib staff inside !
  if((isCalibration() & CAnti) && (ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3)){
    ANTPedCalib::outp(ATCAFFKEY.pedoutf);// 0/1/2->HistOnly/Wr2DB+File/Wr2File 
  }
  if((isCalibration() & CAnti) && ATREFFKEY.relogic==4){
    ANTPedCalib::ntuple_close();// close ntuple for OnBoardPedTables 
  }
}
//-----------------------------------------------------------------------
void AMSJob::_ecalendjob(){
//
bool noTFAskip=(!(isCalibration() & CTOF));
//  if((isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]==6 ){
//    ECPedCalib::ntuple_close();
//  }
  if((isCalibration() & AMSJob::CEcal) && (ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5) ){
    ECPedCalib::outp(ECCAFFKEY.pedoutf);// 0/1/2->HistOnly/Writ2DB+File/Write2File 
  }
//
cout<<"   EcalEndJob:noTFAskip="<<noTFAskip<<endl;
  if(noTFAskip){//bypass when TOF/ACC-calib(to keep log-file small)
    if(ECREFFKEY.reprtf[0]>0)EcalJobStat::printstat(); // Print JOB-Ecal statistics
    if(isSimulation())EcalJobStat::outpmc();
    EcalJobStat::outp();
  }
}
//-----------------------------------------------------------------------
void AMSJob::_trdendjob(){

#pragma omp master
  {
    if( AMSFFKEY.Update){// && (AMSStatus::isDBWriteR()||AMSStatus::isDBUpdateR())){
      TrdHReconR::gethead(AMSEvent::get_thread_num())->update_tdv_array();
      AMSTRDRawHit::updtrdcalibSCI();
    }
  }

  if((isCalibration() & AMSJob::CTRD) ){
   if( TRDCALIB.CalibProcedureNo == 1){
    AMSTRDIdCalib::check(1);
    AMSTRDIdCalib::printbadchanlist();
    AMSTRDIdCalib::ntuple(AMSEvent::getSRun());
    }
   else{
//      AMSTRDIdCalib::ntuple(AMSEvent::getSRun());
   }
  }
  



}

void AMSJob::_richendjob(){
  RichPMTsManager::Finish();
  RichAlignment::Finish();
  RichRadiatorTileManager::Finish();
}


void AMSJob::_srdendjob(){

}


void AMSJob::_axendjob(){

  if(AMSJob::debug){
    // AMSJob::gethead()->printN(cout);
    AMSNode* p=AMSJob::gethead()->getmat();
          assert(p!=NULL);
          p=AMSJob::gethead()->getmed();
          assert(p!=NULL);
          AMSgvolume * pg=AMSJob::gethead()->getgeom();
          assert(pg!=NULL);
  }

    if(isProduction() && isRealData()){
      AMSEvent::_endofrun();
    }
    AMSUser::EndJob(); 
}

void AMSJob::_dbendjob(){

    if(getenv("TMPRawFile") && getenv("NtupleDir") && strstr(getenv("TMPRawFile"),getenv("NtupleDir"))){
       cout <<"AMSJob::_debendjob-I-Removing "<<getenv("TMPRawFile")<<endl;
       unlink(getenv("TMPRawFile"));
       unsetenv("TMPRawFile");
     }


  //Status Stuff
#ifndef __CORBA__
  if(AMSFFKEY.Update==1001){
/*
    AMSTrAligFit::rewrite();
      time_t begin,end,insert;
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSTrAligFit::getTDVGLDB());
      ptdv->gettime(insert,begin,end);
      
      ptdv=AMSJob::gethead()->gettimestructure(AMSTrAligFit::getTDVGLDBP());
    ptdv->UpdCRC();
      ptdv->SetTime(insert,begin,end);
      ptdv->UpdateMe()=1;
     ptdv=AMSJob::gethead()->gettimestructure(AMSTrAligFit::getTDVAGLDB());
     if(ptdv){
     ptdv->gettime(insert,begin,end);
     ptdv=AMSJob::gethead()->gettimestructure(AMSTrAligFit::getTDVAGLDBP());
    ptdv->UpdCRC();
      ptdv->SetTime(insert,begin,end);
      ptdv->UpdateMe()=1;
    }
*/
  }
  if( AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
      AMSJob::gethead()->getstatustable()->Sort();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(AMSEvent::gethead()->getTDVStatus());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time_t begin,end,insert;
      begin=AMSJob::gethead()->getstatustable()->getbegin();
      end=AMSJob::gethead()->getstatustable()->getend();
      time(&insert);
      ptdv->SetTime(insert,begin,end);
      cout <<" Event Status info  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
  }
#endif
  if( AMSFFKEY.Update && AMSStatus::isDBUpdateR()   ){
    AMSStatus::UpdateStatusTableDB();
  }



#ifdef __DB__
  if (AMSEvent::_checkUpdate() == 1) {
   Message("====> AMSJob::_dbendjob -I- UpdateMe is set. Update database and tables.");
   int rstatus = lms -> AddAllTDV();
  } else {
   Message("====> AMSJob::_dbendjob -I- UpdateMe != 1. NO UPDATE");
  }
#else
//    if (AMSFFKEY.Update && !isCalibration()){
    if (AMSFFKEY.Update){
      cout << "AMSjob::_dbendjob-I-updating database... "<<endl;
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"<---- AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }
#endif
}


  void AMSJob::_redaq2initjob(){
     AMSgObj::BookTimer.book("SIDAQ");
     AMSgObj::BookTimer.book("SIZIP");
     AMSgObj::BookTimer.book("REDAQ");
     if(IOPA.Portion<1. && isMonitoring())cout <<"AMSJob::_redaq2initjob()-W-Only about "<<IOPA.Portion*100<<"% events will be processed."<<endl; 
    // Add subdetectors to daq


//CCEB data

if(DAQCFFKEY.BTypeInDAQ[0]<=27 && DAQCFFKEY.BTypeInDAQ[1]>=27){    
 DAQEvent::addsubdetector(&AMSEvent::checkccebid,&AMSEvent::buildcceb,27);
}
if(DAQCFFKEY.BTypeInDAQ[0]<=896 && DAQCFFKEY.BTypeInDAQ[1]>=896){    
 DAQEvent::addsubdetector(&AMSEvent::checkccebid,&AMSEvent::buildcceb,896);
}

//TofSlowTemp short/long data

if(DAQCFFKEY.BTypeInDAQ[0]<=24 && DAQCFFKEY.BTypeInDAQ[1]>=24){//USCM TOF TEMP-SHORT
 DAQEvent::addsubdetector(&AMSEvent::checktofstid,&AMSEvent::buildtofst,24);
}
if(DAQCFFKEY.BTypeInDAQ[0]<=25 && DAQCFFKEY.BTypeInDAQ[1]>=25){//USCM TOF TEMP-LONG
 DAQEvent::addsubdetector(&AMSEvent::checktofstid,&AMSEvent::buildtofst,25);
}
//
if(DAQCFFKEY.BTypeInDAQ[0]<=896 && DAQCFFKEY.BTypeInDAQ[1]>=896){// when in envelope
 DAQEvent::addsubdetector(&AMSEvent::checktofstid,&AMSEvent::buildtofst,896);
}
//


if(DAQCFFKEY.BTypeInDAQ[0]<=5 && DAQCFFKEY.BTypeInDAQ[1]>=5){   // normal 

//ecal
     
    DAQEvent::addsubdetector(&DAQECBlock::checkblockid,&DAQECBlock::buildraw);// for RD
    DAQEvent::addblocktype(&DAQECBlock::getmaxblocks,&DAQECBlock::calcblocklength,&DAQECBlock::buildblock);//for MC ?
// rich
    DAQEvent::addsubdetector(&DAQRichBlock::checkdaqid,&DAQRichBlock::buildraw);
    DAQEvent::addsubdetector(&DAQRichBlock::checkdaqidnode,&DAQRichBlock::buildrawnode);
    DAQEvent::addblocktype(&DAQRichBlock::getmaxblocks,&DAQRichBlock::calcdaqlength,&DAQRichBlock::builddaq);


	{  // mc
		if( !isRealData()){
#ifndef __LVL3ONLY__
			DAQEvent::addsubdetector(&AMSmceventg::checkdaqid,&AMSmceventg::buildraw);
			DAQEvent::addblocktype(&AMSmceventg::getmaxblocks,&AMSmceventg::calcdaqlength,
								   &AMSmceventg::builddaq);
			
#ifdef _PGTRACK_
		DAQEvent::addsubdetector(&TrDAQMC::checkdaqid,&TrDAQMC::buildraw);
		DAQEvent::addblocktype(&TrDAQMC::getmaxblocks,&TrDAQMC::calcdaqlength,
								   &TrDAQMC::builddaq);
#else		
			DAQEvent::addsubdetector(&AMSTrMCCluster::checkdaqid,&AMSTrMCCluster::buildraw);
			DAQEvent::addblocktype(&AMSTrMCCluster::getmaxblocks,&AMSTrMCCluster::calcdaqlength,
								   &AMSTrMCCluster::builddaq);
#endif
#endif
			DAQEvent::addsubdetector(&AMSEvent::checkdaqidSh,&AMSEvent::buildrawSh);
			DAQEvent::addblocktype(&AMSEvent::getmaxblocksSh,
								   &AMSEvent::calcdaqlengthSh,&AMSEvent::builddaqSh);
			
			
			DAQEvent::addblocktype(&AMSEvent::getmaxblocks,
								   &AMSEvent::calcdaqlength,&AMSEvent::builddaq);
			
			
		}
	}
  


{
    // lvl1

    DAQEvent::addsubdetector(&Trigger2LVL1::checkdaqid,&Trigger2LVL1::buildraw);
    DAQEvent::addblocktype(&Trigger2LVL1::getmaxblocks,&Trigger2LVL1::calcdaqlength,&Trigger2LVL1::builddaq);


}



{
//           lvl3
    DAQEvent::addsubdetector(&TriggerLVL302::checkdaqid,&TriggerLVL302::buildraw);
//    DAQEvent::addblocktype(&TriggerLVL302::getmaxblocks,&TriggerLVL302::calcdaqlength,&TriggerLVL302::builddaq);

}
    
//tof + anti 

    DAQEvent::addsubdetector(&DAQS2Block::checkblockid,&DAQS2Block::buildraw);
    DAQEvent::addblocktype(&DAQS2Block::getmaxblocks,&DAQS2Block::calcblocklength,&DAQS2Block::buildblock);


//trd

    DAQEvent::addsubdetector(&AMSTRDRawHit::checkdaqid,&AMSTRDRawHit::buildraw);
    DAQEvent::addsubdetector(&AMSTRDRawHit::checkdaqidJ,&AMSTRDRawHit::buildrawJ);
    DAQEvent::addblocktype(&AMSTRDRawHit::getmaxblocks,&AMSTRDRawHit::calcdaqlength,&AMSTRDRawHit::builddaq);




//tracker
#ifdef _PGTRACK_
    DAQEvent::addsubdetector(&TrDAQ::checkdaqid,&TrDAQ::buildraw);
    DAQEvent::addblocktype(&TrDAQ::getmaxblocks,&TrDAQ::calcdaqlength,&TrDAQ::builddaq);

#else
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqid,&AMSTrRawCluster::buildraw);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocks,&AMSTrRawCluster::calcdaqlength,&AMSTrRawCluster::builddaq);
//    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocks,&AMSTrRawCluster::calcdaqlength,&AMSTrRawCluster::builddaq_new);
#endif


}
if(DAQCFFKEY.BTypeInDAQ[0]<=6 && DAQCFFKEY.BTypeInDAQ[1]>=6){   // OnBoard Calib
//TRD 
  if((CALIB.SubDetInCalib/10000)>0){
//    DAQEvent::addsubdetector(&AMSTRDRawHit::checkdaqidS,&AMSTRDRawHit::updtrdcalib,6);
    DAQEvent::addsubdetector(&AMSTRDRawHit::checkdaqidS,&AMSTRDRawHit::updtrdcalib2009,6);
    DAQEvent::addsubdetector(&AMSTRDRawHit::checkdaqidJ,&AMSTRDRawHit::updtrdcalibJ,6);
  }
//TOF+ACC
  if((CALIB.SubDetInCalib/1000)%10>0)
    DAQEvent::addsubdetector(&DAQS2Block::checkblockidP,&DAQS2Block::buildonbP,6);
//TRK    
  if((CALIB.SubDetInCalib/100)%10>0){
#ifdef _PGTRACK_
    DAQEvent::addsubdetector(&TrCalDB::checkdaqidS,&TrCalDB::updtrcalib2009S,6);
#else
//    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidS,&AMSTrRawCluster::updtrcalibS,6);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidS,&AMSTrRawCluster::updtrcalib2009S,6);
#endif    
}
// RICH
  if((CALIB.SubDetInCalib/10)%10>0)
    DAQEvent::addsubdetector(&DAQRichBlock::checkcalid,&DAQRichBlock::buildcal,6);
//ECAL    
  if(CALIB.SubDetInCalib%10>0){
    if(DAQCFFKEY.DAQVersion==0){//2008
      DAQEvent::addsubdetector(&DAQECBlock::checkblockidP,&DAQECBlock::buildonbP1,6);//no Dynamic peds
    }
    else if(DAQCFFKEY.DAQVersion==1){//2009
      DAQEvent::addsubdetector(&DAQECBlock::checkblockidP,&DAQECBlock::buildonbP,6);//with Dynamic peds
    }
    else{
      cout<<"<====== AMSJob::redaq2initjob: -E- DAQVersion is not defined correctly for PedCalibration !!!"<<endl;
    }
  }

}//--->endof "OnBoardCalib" check



}

#ifdef _PGTRACK_
#include "job_tk.C"
#endif
