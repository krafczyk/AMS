// $Id: job.C,v 1.470 2005/01/04 16:48:00 choumilo Exp $
// Author V. Choutko 24-may-1996
// TOF,CTC codes added 29-sep-1996 by E.Choumilov 
// ANTI codes added 5.08.97 E.Choumilov
// Sep 17, 1997. ak. don't write timeid to files in dbase version
// Oct  1, 1997. ak. add FindTheBestTDV, FillTDVTable functions
// Nov   , 1997. ak. FindTheBestTDV, check id's
// Dec   , 1997. ak. dbendjob is modified
//
// Last Edit : Dec 27, 1997. ak. 
//

#include <tofdbc02.h>
#include <ecaldbc.h>
#include <ecalcalib.h>
#include <antidbc02.h>
#include <amsgobj.h>
#include <astring.h>
#include <cern.h>
#include <math.h>
#include <amsdbc.h>
#include <commons.h>
#include <iostream.h>
#include <fstream.h>
//#include <trid.h>
#include <mccluster.h>
#include <extC.h>
#include <job.h>
#include <event.h>
#include <charge.h>
#include <timeid.h>
#include <mceventg.h>
#include <trcalib.h>
#include <antidbc02.h>
#include <tofsim02.h>
#include <tofrec02.h>
#include <tofcalib02.h>
#include <anticalib02.h>
#include <trigger102.h>
//#include <trigger3.h>
#include <trigger302.h>
#include <bcorr.h>
#include <trid.h>
#include <trrawcluster.h>
#include <ecalrec.h>
#include <daqevt.h>
#include <daqs2block.h>
#include <ntuple.h>
#include <user.h>
#include <tralig.h>
#include <status.h>
#include <richdbc.h>
#include <richid.h>
#include <mccluster.h>
#include <sys/stat.h>
#include <producer.h>
#include <trdid.h>
#include <ecid.h>
#include <tofid.h>
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
//-
//
char AMSJob::_ntuplefilename[256]="";
char AMSJob::_rootfilename[256]="";

long AMSJob::GetNtupleFileSize(){
 struct stat buffer;
 long size_n=stat(_ntuplefilename,&buffer)? 0:buffer.st_size;
 long size_r=stat(_rootfilename,&buffer)? 0:buffer.st_size;
 return size_n>size_r?size_n:size_r;
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
    for(int i=0;i<32;i++){
      STATUSFFKEY.status[i]=0;
    }
        STATUSFFKEY.status[33]=10;
  }
  FFKEY("ESTATUS",(float*)&STATUSFFKEY,sizeof(STATUSFFKEY_DEF)/sizeof(integer),"MIXED");

  AMSFFKEY.Simulation=0; // Simulation
  AMSFFKEY.Reconstruction=1; // Reconstruction
  AMSFFKEY.Jobtype=0; // Default
  AMSFFKEY.Debug=1;
  AMSFFKEY.CpuLimit=15;
  AMSFFKEY.Read=0;
  AMSFFKEY.Write=1;
  AMSFFKEY.Update=0;
  VBLANK(AMSFFKEY.Jobname,40);
  VBLANK(AMSFFKEY.Setupname,40);
  char amsetup[16]="AMS02";
  UCTOH(amsetup,AMSFFKEY.Setupname,4,16);
  AMSFFKEY.ZeroSetupOk=0;
  // Set Defaults
FFKEY("AMSJOB",(float*)&AMSFFKEY+2,sizeof(AMSFFKEY_DEF)/sizeof(integer),"MIXED");
_siamsdata();
_reamsdata();
}

void AMSJob::_siamsdata(){
IOPA.hlun=0;
VBLANK(IOPA.hfile,40);
IOPA.ntuple=1;
IOPA.Portion=.1;
IOPA.WriteAll=102;
VBLANK(IOPA.TriggerC,40);
VBLANK(AMSFFKEY.TDVC,400);
char amsp[12]="AMSParticle";
UCTOH(amsp,IOPA.TriggerC,4,12);
IOPA.mode=0;
VBLANK(IOPA.ffile,40);
IOPA.MaxNtupleEntries=100000;
IOPA.MaxFileSize=180000000;
IOPA.WriteRoot=0;
VBLANK(IOPA.rfile,40);
FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");
TRMFFKEY.OKAY=0;
FFKEY("TERM",(float*)&TRMFFKEY,sizeof(TRMFFKEY_DEF)/sizeof(integer),"MIXED");
MISCFFKEY.BTempCorrection=1;
MISCFFKEY.BeamTest=0;
MISCFFKEY.BZCorr=1;
MISCFFKEY.G3On=1;
MISCFFKEY.G4On=0;
FFKEY("MISC",(float*)&MISCFFKEY,sizeof(MISCFFKEY_DEF)/sizeof(integer),"MIXED");



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
//  FFKEY("RICGEOM",(float *)&RICGEOM,sizeof(RICGEOM_DEF)/sizeof(integer),"REAL");
  FFKEY("RICCONT",(float *)&RICCONTROLFFKEY,sizeof(RICCONTROLFFKEY_DEF)/sizeof(integer),"MIXED");


  RICRADSETUPFFKEY.setup=1;
  FFKEY("RICSET",(float *)&RICRADSETUPFFKEY,sizeof(RICRADSETUPFFKEY_DEF)/sizeof(integer),"MIXED");

  RICRECFFKEY.recon[0]=1;   //beta reconstruction
  RICRECFFKEY.recon[1]=1;   //Z reconstruction
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
  RICFFKEY.year[1]=110;
  FFKEY("RICAL",(float*)&RICFFKEY,sizeof(RICFFKEY_DEF)/sizeof(integer),"INTEGER");



}

//------------------------------------------------------
void AMSJob::_sitrig2data(){
  TGL1FFKEY.trtype=8; //(1) Requested branches(pattern) for LVL1 global OR(see datacards.doc)
//exapmles: 1->unbTOF;2->unbEC;4->unb(TOF&EC);8->unb(TOF|EC);240->GlobPhys;256->extern
// TOF :
// 
  TGL1FFKEY.toflc=0;//(2)required TOF-FastTrigger LAYER configuration
//                 (=0/1/2-> accept at least ANY3of4/ALL4/ANYTopBot2of4 layer coincidence)
  TGL1FFKEY.tofsc=1;//(3)required TOF-FastTrigger SIDE configuration
//                                 (=0/1-> two-sides-AND/OR selection)
  TGL1FFKEY.tfhzlc=1;//(4)TOF_HiZ_FT 2-top(2-bot)fired layers configurations(0/1/2/3-> 
//                                              ->top(bot)OR/topAND/botAND/top(bot)AND 
// ANTI :
  TGL1FFKEY.nanti=1;//(5) max. fired ANTI-paddles(logical) in equat.region 
//
  TGL1FFKEY.RebuildLVL1=0;//(6)
// 
  TGL1FFKEY.MaxScalersRate=20000;//(7)
  TGL1FFKEY.MinLifeTime=0.015;//(8)
// orbit:
  TGL1FFKEY.TheMagCut=0.7;//(9)geom.latitude cut when anti-cut is used(below-#5, above-0)
// Ecal
  TGL1FFKEY.ectrlog=3;//(10) EC-trigger logic type(1->MyOld,2->MyNew+ANSI,3->Pisa)
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
  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");

}
//
//===============================================================================
//
void AMSJob::_sitkdata(){
TKGEOMFFKEY.ReadGeomFromFile=1;
TKGEOMFFKEY.WriteGeomToFile=0;
TKGEOMFFKEY.UpdateGeomFile=0;
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
TRMCFFKEY.BadCh[0]=0.01;
TRMCFFKEY.BadCh[1]=0.01;

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
TRMCFFKEY.year[0]=101;
TRMCFFKEY.year[1]=110;
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
TRCALIB.EventsPerCheck=100;
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

TRALIG.Algorithm=0;
for( i=0;i<6;i++){
  TRALIG.ActiveParameters[i][0]=1;   // x
  TRALIG.ActiveParameters[i][1]=1;   // y
  TRALIG.ActiveParameters[i][2]=1;   // z
  TRALIG.ActiveParameters[i][3]=1;   // pitch  zx
  TRALIG.ActiveParameters[i][4]=1;   // yaw    xy
  TRALIG.ActiveParameters[i][5]=1;   // roll   yz
}
  TRALIG.EventsPerRun=1001;
  TRALIG.LayersOnly=0;
  TRALIG.GlobalGoodLimit=0.085;
  TRALIG.SingleLadderEntryLimit=10000;
 FFKEY("TRALIG",(float*)&TRALIG,sizeof(TRALIG_DEF)/sizeof(integer),"MIXED");



G4FFKEY.BFOrder=4;
G4FFKEY.Delta=0.001;
G4FFKEY.UniformMagField=0;
G4FFKEY.Geant3CutsOn=1;
G4FFKEY.PhysicsListUsed=1;
G4FFKEY.LowEMagProcUsed=0;
FFKEY("G4FF",(float*)&G4FFKEY,sizeof(G4FFKEY_DEF)/sizeof(integer),"MIXED");

PRODFFKEY.Debug=0;
FFKEY("PROD",(float*)&PRODFFKEY,sizeof(PRODFFKEY_DEF)/sizeof(integer),"MIXED");


}

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
CCFFKEY.begindate=2062004;
CCFFKEY.enddate=1012008;
CCFFKEY.begintime=170000;
CCFFKEY.endtime=0;
CCFFKEY.oldformat=0;
CCFFKEY.sdir=1;
CCFFKEY.Fast=0;
CCFFKEY.StrCharge=2;
CCFFKEY.StrMass=-1;
CCFFKEY.SpecialCut=0;
VBLANK(CCFFKEY.FluxFile,40);
CCFFKEY.curtime=0;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");

GMFFKEY.GammaSource=0; //ISN  
GMFFKEY.SourceCoo[0]=1.46; //RA
GMFFKEY.SourceCoo[1]=0.384; //DEC default:Crab
GMFFKEY.SourceVisib=0.873; //50 degrees
GMFFKEY.GammaBg=0;
GMFFKEY.BgAngle=0.087; //5 degrees
FFKEY("GMSRC",(float*)&GMFFKEY,sizeof(GMFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=================================================================================
//
void AMSJob::_sitof2data(){
  TFMCFFKEY.TimeSigma=0.240; //(1) side time resolution(ns,=CounterResol(0.17)*sqrt(2)) 
  TFMCFFKEY.TimeSigma2=0.45; //(2)
  TFMCFFKEY.TimeProbability2=0.035;//(3)
  TFMCFFKEY.padl=11.5;        //(4) not used now (spare)
  TFMCFFKEY.Thr=0.1;          //(5) Sc.bar Edep-thresh.(Mev) to participate in Simul.   
//
  TFMCFFKEY.mcprtf[0]=0;     //(6) TOF MC print flag for init arrays
  TFMCFFKEY.mcprtf[1]=0;     //(7) TOF MC print flag for MC pulses
  TFMCFFKEY.mcprtf[2]=0;     //(8) ...................... histograms
  TFMCFFKEY.mcprtf[3]=0;     //(9) spare
  TFMCFFKEY.mcprtf[4]=0;     //(10) spare
  TFMCFFKEY.trlogic[0]=1; //(11) spare 
  TFMCFFKEY.trlogic[1]=0; //(12) spare 
  TFMCFFKEY.fast=0;       //(13) 0/1/2->off/on fast generation in mceventg.C(2-> EC requirement)
  TFMCFFKEY.daqfmt=0;     //(14) 0/1-> raw/reduced TDC format for DAQ simulation
  TFMCFFKEY.birks=1;      //(15) 0/1->  not apply/apply birks corrections
  TFMCFFKEY.mcseedo=0;    //(16) 1/0->Use RealDataCalib_Copy/MCCalib files as MCSeed
  TFMCFFKEY.blshift=0.;   //(17) base line shift at fast discr.input (mv)
  TFMCFFKEY.hfnoise=5.;   //(18) high freq. noise .......   
//     
  TFMCFFKEY.ReadConstFiles=0;//(19)PTS(P=PedsMC,T=TimeDistr,S=MCCalibSeeds);P(T,S)=0/1->DB/RawFiles
//
FFKEY("TFMC",(float*)&TFMCFFKEY,sizeof(TFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_simag2data(){
  MAGSFFKEY.magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  MAGSFFKEY.fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  MAGSFFKEY.ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 
FFKEY("MAGS",(float*)&MAGSFFKEY,sizeof(MAGSFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_siecaldata(){
  ECMCFFKEY.fastsim=0;     //(1) 1/0-> fast/slow simulation algorithm(missing fast TBD)
  ECMCFFKEY.mcprtf=0;       //(2) print_hist flag (0/1->no/yes)
  ECMCFFKEY.cutge=0.0005;    //(3) cutgam=cutele cut for EC_volumes
  ECMCFFKEY.silogic[0]=0;   //(4) SIMU logic flag =0/1/2->peds+noise/no_noise/no_peds
  ECMCFFKEY.silogic[1]=0;   //(5) 1/0-> to use RealDataCopy(sd)/MC(mc) RLGA/FIAT-files as MCSeeds
  ECMCFFKEY.mev2mev=59.27/1.007;  //(6) Geant dE/dX(MeV)->MCEmeas(MeV) conv.factor(at EC-center)  ! corrected for  500 kev geant3 cut
  ECMCFFKEY.mev2adc=0.873;  //(7) MCEmeas(MeV)->ADCch factor(MIP-m.p.->10th channel)(...)
  ECMCFFKEY.safext=0.;      //(8) Extention(cm) of EC transv.size when TFMC 13=2 is used
  ECMCFFKEY.mev2pes=55.;    //(9) PM ph.electrons/Mev(dE/dX)(8000*0.0344*0.2)
  ECMCFFKEY.pmseres=0.8;    //(10)PM single-electron spectrum resolution
  ECMCFFKEY.adc2q=0.045;    //(11)Anode(H,L) ADCch->Q(pC) conv.factor(pC/adc)
  ECMCFFKEY.fendrf=0.3;     //(12) fiber end-cut reflection factor
  ECMCFFKEY.physa2d=6.;     //(13) physical(for trigger) an/dyn ratio(mv/mv) 
  ECMCFFKEY.hi2low=36.;     //(14) not used now 
  ECMCFFKEY.sbcgn=1.;       //(15) not used now 
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
  ECMCFFKEY.ReadConstFiles=0;//(28)CP=CalibrMS|Peds:C=1/0->Read MSCalibFile/DB
//                                          P=1/0->GeneratePeds/ReadFromDB
FFKEY("ECMC",(float*)&ECMCFFKEY,sizeof(ECMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//---------------------------
void AMSJob::_reecaldata(){
  ECREFFKEY.reprtf[0]=0;     // (1) print_hist flag (0/1->no/yes)
  ECREFFKEY.reprtf[1]=0;     // (2) print_profile flag (0/1->no/yes)
  ECREFFKEY.reprtf[2]=0;     // (3) spare
//
  ECREFFKEY.relogic[0]=0;    // (4) 1/0->write/not EcalHits into Ntuple
  ECREFFKEY.relogic[1]=0;    // (5) 0/1/2/3->normal/RLGA_calib/RLGA+FIAT_calib/ANOR_calib run
  ECREFFKEY.relogic[2]=0;    // (6) 0/1/2->multipl/Ebackgr/Both_type cuts in ANOR_calib logic
  ECREFFKEY.relogic[3]=0;    // (7) 
  ECREFFKEY.relogic[4]=0;    // (8) spare
//
// Run-time DAQ/trig-thresholds(time dependent):
  ECREFFKEY.thresh[0]=3.;     // (9)  Anode(High-chan) readout threshold(in sigmas)
  ECREFFKEY.thresh[1]=120.;   // (10) Etot "mip"-trig.thresh(mev tempor)
  ECREFFKEY.thresh[2]=500.;   // (11) ... 1st 3SL "em"-trig.thresh(mev tempor)
  ECREFFKEY.thresh[3]=3000.;  // (12) Etot thresh-1(soft, mev tempor)
  ECREFFKEY.thresh[4]=3.;     // (13) Low-chan. readout thershold(in sigmas)
  ECREFFKEY.thresh[5]=8000.;  // (14) Etot thresh-2(hard, mev tempor)  
  ECREFFKEY.thresh[6]=3.5;    // (15) min Epeak/Ebase (add. to #11 for "electromagneticity")
  ECREFFKEY.thresh[7]=1.;     // (16) min Epeak/Efron .....................................
  ECREFFKEY.thresh[8]=25.;    // (17) PM-dynode threshold for width calc.(mev tempor) 
  ECREFFKEY.thresh[9]=13.;    // (18) trig. width cut(columns, bending plane)
  ECREFFKEY.thresh[10]=1.;    // (19) Trig. PMDynThr sl1(y)
  ECREFFKEY.thresh[11]=70.;   // (20) Trig. PMDynThr sl2(x)
  ECREFFKEY.thresh[12]=100.;  // (21) Trig. PMDynThr sl3(y)
  ECREFFKEY.thresh[13]=80.;  // (22) Trig. PMDynThr sl4(x)
  ECREFFKEY.thresh[14]=60.;   // (23) Trig. PMDynThr sl5(y)
  ECREFFKEY.thresh[15]=40.;  // (24) Trig. PMDynThr sl6(x)
  ECREFFKEY.thresh[16]=40.;  // (25) Trig. PMDynThr sl7(y)
  ECREFFKEY.thresh[17]=1.;   // (26) Trig. PMDynThr sl8(x) 
  ECREFFKEY.thresh[18]=1.;   // (27) Trig. PMDynThr sl9(y) 
  ECREFFKEY.thresh[19]=0.;   // (28) 
// Run-time RECO-thresholds(time dependent):
  ECREFFKEY.cuts[0]=1.15;   // (29) Italy Trig-algor: WidthCut1 
  ECREFFKEY.cuts[1]=2.15;   // (30)                   WidthCut1
  ECREFFKEY.cuts[2]=5.;     // (31)                   Xmult-boundary
  ECREFFKEY.cuts[3]=6.;     // (32)                    Ymult-boundary
  ECREFFKEY.cuts[4]=0.65;//  (33) LVL3-trig. EC-algorithm: "peak"/"average" methode boundary
//
  ECREFFKEY.ReadConstFiles=10;//(34)DC (ThreshCuts-set | Calib.const(MC/RD))
//                            D=1/0-> Take from DataCards/DB
//                            C=1/0-> Take from CalibFiles/DB

  ECREFFKEY.Thr1DSeed=10;//(35) this and below is for Vitali's clust. algorithm
  ECREFFKEY.Thr1DRSeed=0.18;
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
//  
  ECREFFKEY.sec[0]=0;//53 
  ECREFFKEY.sec[1]=0;//
  ECREFFKEY.min[0]=0;//
  ECREFFKEY.min[1]=0;//
  ECREFFKEY.hour[0]=0;//
  ECREFFKEY.hour[1]=0;//
  ECREFFKEY.day[0]=1;//
  ECREFFKEY.day[1]=1;//
  ECREFFKEY.mon[0]=0;//
  ECREFFKEY.mon[1]=0;//
  ECREFFKEY.year[0]=101;//
  ECREFFKEY.year[1]=110;//64
FFKEY("ECRE",(float*)&ECREFFKEY,sizeof(ECREFFKEY_DEF)/sizeof(integer),"MIXED");
//
// REUN-Calibration  parameters:
// RLGA/FIAT part:
  ECCAFFKEY.cfvers=4;     // (1) 1-999 -> vers.number for ecalcvlistNNN.dat file
  ECCAFFKEY.cafdir=0;     // (2) 0/1-> use official/private directory for calibr.files
  ECCAFFKEY.truse=1;      // (3) (1)/0-> use He4/proton tracks for calibration
  ECCAFFKEY.refpid=118;   // (4) ref.pm ID (SPP-> S=SupLayer, PP=PM number) 
  ECCAFFKEY.trmin=4.;     // (5) presel-cut on min. rigidity of the track(gv) 
  ECCAFFKEY.adcmin=3.;    // (6) min ADC cut for indiv. SubCell (to remove noise)
  ECCAFFKEY.adcpmx=1000.; // (7) max ADC cut for indiv SC to consider Plane as bad(non PunchThrough)
  ECCAFFKEY.ntruncl=1;    // (8) remove this number of scPlanes with highest Edep
  ECCAFFKEY.trxac=0.022;  // (9) TRK->EC extrapolation accuracy in X-proj(cm)
  ECCAFFKEY.tryac=0.019;  //(10) TRK->EC extrapolation accuracy in Y-proj............
  ECCAFFKEY.mscatp=1.;    //(11) EC mult.scatt. fine tuning parameter
  ECCAFFKEY.nortyp=0;     //(12) normaliz.type 0/1-> by crossed/fired counters
  ECCAFFKEY.badplmx=0;   // (13) Accept max. bad sc-planes(>2 fired sc, high sc Ed, separated sc)
  ECCAFFKEY.etrunmn=60.;  //(14) Min ECenergy (Etrunc in mev) to select particle(He)
  ECCAFFKEY.etrunmx=160.; //(15) Max ECenergy (Etrunc in mev) ......................
  ECCAFFKEY.nsigtrk=1.5;  //(16) Safety gap param. for crossing check(-> ~2 sigma of TRK accur.)
// ANOR part:
  ECCAFFKEY.pmin=3.;        // (17) presel-cut on min. mom. of the track(gev/c) 
  ECCAFFKEY.pmax=15.;       // (18) presel-cut on max. mom. of the track 
  ECCAFFKEY.scmin=3.;       // (19) min ADC for indiv. SubCell (to remove ped,noise)
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
  ECCAFFKEY.b2scut[1]=0.1; // (51) max backgr/signal energy(bound.from above) for pl-2 
  ECCAFFKEY.b2scut[2]=0.2;  // (52) max backgr/signal energy(bound.from above) for pl-3 
  ECCAFFKEY.b2scut[3]=0.2;  // (53) max backgr/signal energy(bound.from above) for pl-4 
  ECCAFFKEY.b2scut[4]=0.15;  // (54) max backgr/signal energy(bound.from above) for pl-5 
  ECCAFFKEY.b2scut[5]=0.15;  // (55) max backgr/signal energy(bound.from above) for pl-6 
//g.chen
  ECCAFFKEY.ecshswit=0;        // (56) 0/1 switch to/(not to) use shower info in ANOR calibration
  ECCAFFKEY.chi2dirmx=10.;      // (57) max chi2 of shower dir fit
  ECCAFFKEY.prchi2mx=5.;       // (58)max chi2 of shower profile fit
  ECCAFFKEY.trchi2mx=10.;      // (59)max chi2 of shower trans. fit
  ECCAFFKEY.eshsleakmx=0.01;   // (60) max Eleak(shower side)/Etot
  ECCAFFKEY.eshrleakmx=0.05;;  // (61) max Eleak(shower back)/Etot
  ECCAFFKEY.eshdleakmx=0.02;   // (62) max Eleak(shower dead)/Etot
  ECCAFFKEY.esholeakmx=0.1;;  // (63) max Eleak(shower orph)/Etot
  ECCAFFKEY.difsummx=0.2;      // (64) cut of (E_x-E_y)/(E_x+E_y)
  ECCAFFKEY.trentmax[0]=0.5;   // (65) cut of distance between track and shower entry x-proj
  ECCAFFKEY.trentmax[1]=0.5;   // (66) cut of distance between track and shower entry y-proj
  ECCAFFKEY.trextmax[0]=1.0;   // (67) cut of distance between track and shower enit x-proj
  ECCAFFKEY.trextmax[1]=1.0;   // (68) cut of distance between track and shower exit y-proj
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
  ATGEFFKEY.stleng=85.;    // (ams02)length of supp. tube
  ATGEFFKEY.stthic=0.12;   // thickness of supp. tube
  
//---
  ATMCFFKEY.mcprtf=0;//(1)print-flag(0/1/2/3->print:no/histogr/PulseSh_arr/print_pulse)
  ATMCFFKEY.LZero=0; // (2)spare
  ATMCFFKEY.LSpeed=17.;// (3)Eff. light speed in anti-paddle (cm/ns)
  ATMCFFKEY.ReadConstFiles=0;//(4)P(PedsMC), P=0/1-> read from DB/RawFiles
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
TRDMCFFKEY.GeV2ADC=1.e8;
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
TRDMCFFKEY.year[1]=110;
TRDMCFFKEY.GenerateConst=0;
TRDMCFFKEY.NoiseLevel=0.01;
TRDMCFFKEY.DeadLevel=0.015;
TRDMCFFKEY.multiples=0;

FFKEY("TRDMC",(float*)&TRDMCFFKEY,sizeof(TRDMCFFKEY_DEF)/sizeof(integer),"MIXED");

TRDRECFFKEY.Chi2StrLine=5;
TRDRECFFKEY.ResCutStrLine=1;
TRDRECFFKEY.SearchRegStrLine=3;
FFKEY("TRDRE",(float*)&TRDRECFFKEY,sizeof(TRDRECFFKEY_DEF)/sizeof(integer),"MIXED");

TRDFITFFKEY.Chi2StrLine=3;
TRDFITFFKEY.ResCutStrLine=1;
TRDFITFFKEY.SearchRegStrLine=3;
TRDFITFFKEY.MinFitPoints=6;
TRDFITFFKEY.TwoSegMatch=0.1;
TRDFITFFKEY.MaxSegAllowed=50;
FFKEY("TRDFI",(float*)&TRDFITFFKEY,sizeof(TRDFITFFKEY_DEF)/sizeof(integer),"MIXED");

TRDCLFFKEY.ADC2KeV=1.e6/TRDMCFFKEY.GeV2ADC/TRDMCFFKEY.gain;
TRDCLFFKEY.Thr1S=0.11;
TRDCLFFKEY.Thr1A=0.33;
TRDCLFFKEY.Thr1R=6;
TRDCLFFKEY.Thr1H=5.9;
TRDCLFFKEY.MaxHitsInCluster=3;
LVL3FFKEY.TRDHMulThr=TRDCLFFKEY.Thr1H/TRDCLFFKEY.ADC2KeV*TRDMCFFKEY.f2i;
LVL3FFKEY.TRDHMulPart=0.249;
FFKEY("TRDCL",(float*)&TRDCLFFKEY,sizeof(TRDCLFFKEY_DEF)/sizeof(integer),"MIXED");

}

void AMSJob::_sisrddata(){
}

void AMSJob:: _reamsdata(){
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
FFKEY("BMAP",(float*)&TKFIELD,60,"MIXED");

}

void AMSJob::_retkdata(){

//number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
TRCLFFKEY.ThrClA[0]=10.;
TRCLFFKEY.ThrClA[1]=10.;

TRCLFFKEY.ThrClS[0]=20;
TRCLFFKEY.ThrClS[1]=15;

TRCLFFKEY.ThrClR[0]=3.;
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

TRCLFFKEY.ErrX=34.e-4;
TRCLFFKEY.ErrY=20.e-4;
TRCLFFKEY.ErrZ=34.e-4;
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
TRFITFFKEY.UseTRD=1;
TRFITFFKEY.UseTOF=2;
TRFITFFKEY.Chi2FastFit=2000;
TRFITFFKEY.Chi2StrLine=10;
TRFITFFKEY.Chi2WithoutMS=5;
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
TRFITFFKEY.WeakTracking=1;
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
TRFITFFKEY.LowMargin=0;
TRFITFFKEY.OnlyGammaVtx=0;
TRFITFFKEY.UseGeaneFitting=1;
TRFITFFKEY.OldTracking=0;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}
//=================================================================================
//
void AMSJob::_retof2data(){
// 
  TFREFFKEY.Thr1=0.2;//(1) Bar threshold (mev) in tof-cluster algorithm
  TFREFFKEY.ThrS=0.2; //(2) Threshold (mev) on total cluster energy(Not used now!)
//
  TFREFFKEY.reprtf[0]=0; //(3) RECO print flag for statistics 
  TFREFFKEY.reprtf[1]=0; //(4) print flag for DAQ (1/2-> print for decoding/dec+encoding)
  TFREFFKEY.reprtf[2]=0; //(5) print flag for histograms
  TFREFFKEY.reprtf[3]=0; //(6) print flag for TDC-hit multiplicity histograms 
  TFREFFKEY.reprtf[4]=0; //(7) if non-zero (LBBS) print stretcher T1/T2 for LBBS
//
  TFREFFKEY.relogic[0]=0;//(8) 0/1/2/3/4 ->normal/STRR+AVSD-/TDIF-/TZSL-/AMPL-calibr. run. 
  TFREFFKEY.relogic[1]=2;//(9) 0/1/2-> full_fTDC_use/no_time_matching/not_use 
  TFREFFKEY.relogic[2]=0;//(10) 0/1-> force 1-side suppression(useful for MC processing)
  TFREFFKEY.relogic[3]=0;//(11) 1/0->Do/not recovering of missing side 
  TFREFFKEY.relogic[4]=0;//(12) spare RECO logic flag
//
  TFREFFKEY.daqthr[0]=30.;//(13)tempor Anode low discr.thresh(30mV) for fast/slow_TDC 
  TFREFFKEY.daqthr[1]=100.;//(14)tempor Anode high discr.thresh(100mV) for FT-trigger (z>=1)  
  TFREFFKEY.daqthr[2]=250.;//(15)tempor Anode superhigh discr.thresh(mV) for  "z>=2"-trig  
  TFREFFKEY.daqthr[3]=2.5;//(16)A/D(H/L)-ADC-readout threshold in DAQ (in PedSigmas)    
  TFREFFKEY.daqthr[4]=1.;//(17)Thresh(pC) for anode-integrator
//
  TFREFFKEY.cuts[0]=40.;//(18)t-window(ns) for "the same hit" search in f/s_tdc
  TFREFFKEY.cuts[1]=15.;//(19)"befor"-cut in time history (ns)(max.PMT-pulse length?)
  TFREFFKEY.cuts[2]=400.;//(20)"after"-cut in time history (ns)(max. shaper integr.time?)
  TFREFFKEY.cuts[3]=2.8; //(21) error(cm) in longitudinal coordinate (single TOF bar)
  TFREFFKEY.cuts[4]=80.;//(22) FT delay(min), for me is constant now
  TFREFFKEY.cuts[5]=40.;//(23) sTDC-delay wrt fTDC
  TFREFFKEY.cuts[6]=0.6;//(24) 2-bars assim.cut in TOFCluster energy calculation
  TFREFFKEY.cuts[7]=8.;// (25) internal longit.coo matching cut ...Not used (spare)
  TFREFFKEY.cuts[8]=50.;//(26) spare 
  TFREFFKEY.cuts[9]=0.;// (27) 
//
  TFREFFKEY.ReadConstFiles=100;//(28) DPC(D->ThreshCuts-set,P->Peds,C->CalibConst(rd/mc));
// D=1/0->Take ThreshCuts-set from DataCards/DB; P(C)=1/0->Take Peds(CalibConst) from Files/DB

  TFREFFKEY.sec[0]=0;//(29) 
  TFREFFKEY.sec[1]=0;
  TFREFFKEY.min[0]=0;
  TFREFFKEY.min[1]=0;
  TFREFFKEY.hour[0]=0;
  TFREFFKEY.hour[1]=0;
  TFREFFKEY.day[0]=1;
  TFREFFKEY.day[1]=1;
  TFREFFKEY.mon[0]=0;
  TFREFFKEY.mon[1]=0;
  TFREFFKEY.year[0]=101;
  TFREFFKEY.year[1]=110;//(40)
  FFKEY("TFRE",(float*)&TFREFFKEY,sizeof(TFREFFKEY_DEF)/sizeof(integer),
  "MIXED");

//    defaults for calibration:
// TZSL-calibration:
  TFCAFFKEY.pcut[0]=5.;// (1)track mom. low limit (gev/c) (prot, put 0.75 for mu)
  TFCAFFKEY.pcut[1]=100.;// (2)track mom. high limit
  TFCAFFKEY.bmeanpr=0.996;// (3)mean prot. velocity in the above range
  TFCAFFKEY.tzref[0]=0.;//(4)T0 for ref. counter
  TFCAFFKEY.tzref[1]=0.;//(5) spare
  TFCAFFKEY.fixsl=5.;// (6)def. slope
  TFCAFFKEY.bmeanmu=0.997;// (7)mean muon velocity in the above range
  TFCAFFKEY.idref[0]=104;//(8)LBB for first ref. counter 
  TFCAFFKEY.idref[1]=0;//(9)0/1/2->FitAll/IgnorTrapezCount/FitTrapezCount&FixOthers
  TFCAFFKEY.ifsl=1;//(10) 0/1 to fix/release slope param.
//
  TFCAFFKEY.caltyp=0;// (11) 0/1->space/earth calibration
//
// AMPL-calibration:
  TFCAFFKEY.truse=1; // (12) 1/-1-> to use/not tracker
  TFCAFFKEY.plhc[0]=0.5;// (13) track mom. low limit(gev/c) for space calibr
  TFCAFFKEY.plhc[1]=47.;// (14) track mom. high limit(gev/c) ..............
  TFCAFFKEY.minev=80;// (15)min.events needed for measurement in channel or bin
  TFCAFFKEY.trcut=0.92;// (16) cut to use for "truncated average" calculation
  TFCAFFKEY.spares[0]=0;//spare integers
  TFCAFFKEY.spares[1]=0;//spare integers
  TFCAFFKEY.spares[2]=0;//spare integers
  TFCAFFKEY.spares[3]=0;//spare integers
  TFCAFFKEY.adc2q=1.;//(21)adc->charge conv.factor(pC/ADCch, hope = for all ADC chips)
  TFCAFFKEY.plhec[0]=0.5;//(22)plow-cut for earth calibration
  TFCAFFKEY.plhec[1]=7.;  //(23)phigh-cut ...................
  TFCAFFKEY.bgcut[0]=0.58; //(24) beta*gamma low-cut to be around mip-region(abs.calib)
  TFCAFFKEY.bgcut[1]=50.;//(25) beta*gamma high-cut ..............................
//
  TFCAFFKEY.tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  TFCAFFKEY.dynflg=0; // (27)  not used now
  TFCAFFKEY.cfvers=6; // (28) 1-999 -> vers.number for tof2cvlistNNN.dat file
  TFCAFFKEY.cafdir=0;// (29) 0/1-> use official/private directory for calibr.files
  TFCAFFKEY.mcainc=0;// (30) =1->Anode-integrators calibration(MC only)(not used now)
  TFCAFFKEY.tofbetac=0.5;// (31) if nonzero->low beta cut (own TOF measurements !!!)
  FFKEY("TFCA",(float*)&TFCAFFKEY,sizeof(TFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//======================================================================
void AMSJob::_reanti2data(){
  ATREFFKEY.reprtf[0]=0;//(1) Reco print_hist flag (0/1->no/yes)
  ATREFFKEY.reprtf[1]=0;//(2) DAQ-print (1/2->print for decoding/decoding+encoding)
  ATREFFKEY.relogic=0;  //(3) =0/1->NormalRun/CalibrationRun
  ATREFFKEY.Edthr=0.1;  //(4) threshold to create Cluster(Paddle) object (mev)
  ATREFFKEY.zcerr1=10.; //(5) Err(cm).in longit.coord. when 2-sides times are known 
  ATREFFKEY.daqthr=3.;  //(6) spare
  ATREFFKEY.dathr=2.;   //(7) spare
  ATREFFKEY.ftwin=50.;  //(8) Tof FT-pulse <-> Anti "pattern"-pulse t-adjustment(ns)(used now in MC!) 
//
  ATREFFKEY.ReadConstFiles=0;//(9)PVS(Peds,VarCalPar,StabCalPar), P(V,S)=0/1-> read from DB/RawFiles
//  
  ATREFFKEY.sec[0]=0;//(10) 
  ATREFFKEY.sec[1]=0;//(11)
  ATREFFKEY.min[0]=0;//(12)
  ATREFFKEY.min[1]=0;//(13)
  ATREFFKEY.hour[0]=0;//(14)
  ATREFFKEY.hour[1]=0;//(15)
  ATREFFKEY.day[0]=1;
  ATREFFKEY.day[1]=1;
  ATREFFKEY.mon[0]=0;
  ATREFFKEY.mon[1]=0;
  ATREFFKEY.year[0]=101;//(20)
  ATREFFKEY.year[1]=110;
  FFKEY("ATRE",(float*)&ATREFFKEY,sizeof(ATREFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  ATCAFFKEY.cfvers=4; //(1) (001-999) vers.number NN for antiverlistNN.dat file
  ATCAFFKEY.cafdir=0;// (2)  0/1-> use official/private directory for calibr.files
  FFKEY("ATCA",(float*)&ATCAFFKEY,sizeof(ATCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_redaqdata(){
DAQCFFKEY.mode=0;
DAQCFFKEY.OldFormat=0;
DAQCFFKEY.LCrateinDAQ=1;
DAQCFFKEY.SCrateinDAQ=-1;
DAQCFFKEY.NoRecAtAll=0;
DAQCFFKEY.TrFormatInDAQ=3;
VBLANK(DAQCFFKEY.ifile,40);
VBLANK(DAQCFFKEY.ofile,40);
  FFKEY("DAQC",(float*)&DAQCFFKEY,sizeof(DAQCFFKEY_DEF)/sizeof(integer),"MIXED");

}


void AMSJob::_reaxdata(){
// Fit beta & charge
CHARGEFITFFKEY.NmembMax=3;
CHARGEFITFFKEY.Tracker=1;
CHARGEFITFFKEY.EtaMin[0]=0.05;
CHARGEFITFFKEY.EtaMin[1]=0.00;
CHARGEFITFFKEY.EtaMax[0]=0.95;
CHARGEFITFFKEY.EtaMax[1]=1.00;
CHARGEFITFFKEY.ProbTrkRefit=0.01;
CHARGEFITFFKEY.ResCut[0]=4.;
CHARGEFITFFKEY.ResCut[1]=4.;
CHARGEFITFFKEY.SigMin=0.1;
CHARGEFITFFKEY.SigMax=0.3;
CHARGEFITFFKEY.PdfNorm=1;
CHARGEFITFFKEY.TrMeanRes=1;
CHARGEFITFFKEY.ProbMin=0.01;
CHARGEFITFFKEY.TrackerOnly=8;
CHARGEFITFFKEY.ChrgMaxAnode=3;
CHARGEFITFFKEY.BetaPowAnode=1;
CHARGEFITFFKEY.TrackerForceSK=0;
CHARGEFITFFKEY.TrackerKSRatio=0.67;
CHARGEFITFFKEY.TrackerProbOnly=3;

BETAFITFFKEY.pattern[0]=1;
BETAFITFFKEY.pattern[1]=1;
BETAFITFFKEY.pattern[2]=1;
BETAFITFFKEY.pattern[3]=1;
BETAFITFFKEY.pattern[4]=1;
BETAFITFFKEY.pattern[5]=1;
BETAFITFFKEY.pattern[6]=1;
BETAFITFFKEY.pattern[7]=1;
BETAFITFFKEY.pattern[8]=1;
BETAFITFFKEY.Chi2=50;
BETAFITFFKEY.Chi2S=15;
BETAFITFFKEY.SearchReg[0]=3.;
BETAFITFFKEY.SearchReg[1]=3.;
BETAFITFFKEY.SearchReg[2]=3.;
BETAFITFFKEY.LowBetaThr=0.4;
BETAFITFFKEY.FullReco=0;
BETAFITFFKEY.MassFromBetaRaw=1;
FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}
void AMSJob::_resrddata(){
}

//-----------------------------

void AMSJob::udata(){

if(CCFFKEY.StrMass<0){
 CCFFKEY.StrMass=0.938*pow(CCFFKEY.StrCharge/0.3,1.5);
}
 if(GCKINE.ipart==113){
  cout <<" Stranglet Parameters Are Charge: "<<CCFFKEY.StrCharge<<" Mass: "<<CCFFKEY.StrMass<<endl;
 }


GCTIME.TIMEND=GCTIME.TIMINT;
GCTIME.ITIME=0;
if(!MISCFFKEY.G4On && !MISCFFKEY.G3On){
cerr<<"AMSJob::udata-F-NeitherGeant3NorGeant4Selected"<<endl;
exit(1);
}
else if(MISCFFKEY.G4On && MISCFFKEY.G3On){
cerr<<"AMSJob::udata-W-BothGeant3AndGeant4Selected"<<endl;
exit(1);
}
else if(MISCFFKEY.G4On)cout<<"AMSJob::udata-I-Geant4Selected"<<endl;
else cout<<"AMSJob::udata-I-Geant3Selected"<<endl;

if(MISCFFKEY.BZCorr !=1){
cout <<"AMSJob::udata-W-magneticFieldRescaleModeOnWithFactor "<<MISCFFKEY.BZCorr<<endl;
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
 TRFITFFKEY.FastTracking=4;
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
  if(DAQCFFKEY.SCrateinDAQ<0){
   DAQCFFKEY.SCrateinDAQ=0;
  }

  TKDBc::init();
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
if(iprod)setjobtype(Production);

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
    AMSTrIdGeom::init();
    if(strstr(getsetup(),"AMS02") ){    
       AMSTrIdSoft::inittable(2);
       AMSTrIdSoft::init();
       AMSTRDIdSoft::init();
       AMSRICHIdGeom::Init();
       RichRadiatorTile::Init();	
       AMSRICHIdSoft::Init();
       AMSTRDIdSoft::inittable();
       AMSECIdSoft::inittable();
    }
    else {
      cerr<<"AMSJob::udate-E-NoAMSTrIdSoftTable exists for setup "<<
        getsetup()<< "yet "<<endl;
        exit(1);
    }


      // TraligGlobalFit
     if(TRALIG.GlobalFit && TRALIG.MaxPatternsPerJob!=1){
       cout <<"AMSJob::udata-I-GlobalFitRequested ";
       TRALIG.MaxPatternsPerJob=1;
       TRALIG.MaxEventsPerFit=499999;
       TRALIG.MinEventsPerFit=999; 
      cout <<"Parameters Changed "<<TRALIG.MaxEventsPerFit<<" "<<TRALIG.MinEventsPerFit<<endl;
     }


UHTOC(TKFIELD.mfile,160/sizeof(integer),AMSDATADIR.fname,160);

for(i=159;i>=0;i--){
  if(AMSDATADIR.fname[i]!=' '){
   AMSDATADIR.fname[i+1]=0;
   break;
  }
  if(strlen(AMSDATADIR.fname)<=1)strcpy(AMSDATADIR.fname,"fld97int.txt");
}


if(TRDMCFFKEY.mode==-1){
 if(!strstr(getsetup(),"AMSSHUTTLE")){
    TRDMCFFKEY.mode=0;
 }
}
if(TRFITFFKEY.FastTracking==-1){
 TRFITFFKEY.FastTracking=1;
}

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

//AMSgObj::BookTimer.book("AMSEvent::event");
AMSgObj::BookTimer.book("GUOUT");
AMSgObj::BookTimer.book("GUKINE");

_siamsinitjob();
_reamsinitjob();



if(isCalibration())_caamsinitjob();
_timeinitjob();
map(1);
_dbinitjob();
cout << *this;
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
  AMSgObj::BookTimer.book("EventStatus");
  AMSStatus::init();
}

//-------------------------------------------------------------------------------------------
void AMSJob::_sitrig2initjob(){
  if(TGL1FFKEY.RebuildLVL1)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl1 Will be rebuild from TOF data; Original Trigger info will be lost"<<endl;
  if(LVL3FFKEY.RebuildLVL3==1)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl3 will be rebuild from TOF/Tracker data; Original Trigger info will be lost"<<endl;
  else if(LVL3FFKEY.RebuildLVL3)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl3 will be rebuild from TOF/Trigger data Original Trigger info will be kept"<<endl;
  AMSgObj::BookTimer.book("LVL3");
  TriggerLVL302::init();  
}
//-------------------------------------------------------------------------------------------
void AMSJob::_sitkinitjob(){
  if(TRMCFFKEY.GenerateConst){
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+="trsigma.hbk";
    int iostat;
    int rstat=1024;
    HROPEN(1,"input",fnam,"P",rstat,iostat);
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
  cout <<"_sitof2init-I-Slow(Accurate) simulation algorithm selected."<<endl;
//
  if(TFMCFFKEY.daqfmt==0)cout<<"_sitof2init-I-Raw TDC_Format selected"<<endl;
  else if(TFMCFFKEY.daqfmt==1)cout<<"_sitof2init-I-Reduced TDC_Format selected"<<endl;
  else {
    cout<<"_sitof2init-I- Unknown TDC_Format was requested !!!"<<endl;
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
//---------(.ReadConstFiles card convention : tfmc->PTS, tfre->DPC)
  if((TFMCFFKEY.ReadConstFiles%100)/10==1
              || TFREFFKEY.ReadConstFiles%10==1){//MC-Time-Distr from ext.files
//  
    TOFWScan::build();//create scmcscan-objects (MC Bars Time/Eff-distributions)
  }
  if(TFMCFFKEY.ReadConstFiles%10==1 && !isRealData()){
    TOFBrcalMS::build();//create sc.bar "MC Seeds" scbrcal-objects
  }
//---------
  if((TFMCFFKEY.ReadConstFiles%1000)/100>0 && !isRealData()){//MCPeds from ext.files
    TOFBPeds::mcbuild();//create sc.bar scbrped-objects
  }
//---------
//
   TOF2JobStat::bookhistmc();//Book histograms for MC
}
//----------------------------------------------------------------------------------------
void AMSJob::_siecalinitjob(){
  if(ECMCFFKEY.fastsim==1)cout <<"_siecinit-I-Fast(Crude) simulation algorithm selected."<<endl;
  else cout <<"_siecinit-I-Slow(Accurate) simulation algorithm selected."<<endl;
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
// ===> Generate MC-pedestals if requested, otherwise they will be taken from DB
  if(!isRealData()){
    if(ECMCFFKEY.ReadConstFiles%10>0){//P
      cout <<"_siecalinitjob: prepare default MC-pedestals ..."<<endl;
      ECPMPeds::mcbuild();
    }
    if((ECMCFFKEY.ReadConstFiles%100)/10>0){//C(only Seeds parameters(rlga+fiat))
      cout <<"_siecalinitjob: read MC-Seeds calibr.const file..."<<endl;
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
  if(ATMCFFKEY.ReadConstFiles>0 && !isRealData()){//(P) MCPeds from ext.files
    ANTIPeds::mcbuild();//create sc.bar anscped-objects
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

if(isCalibration() & CAMS)_caaxinitjob();
//if(isCalibration() & CCerenkov)_cactcinitjob();
}
//-----------------------------------------------------
void AMSJob::_catkinitjob(){
cout <<" AMSJob::_catkinitjob()-i_Initialized for Proc no  "<<TRCALIB.CalibProcedureNo<<endl;
AMSgObj::BookTimer.book("CalTrFill");
AMSgObj::BookTimer.book("CalTrFit");
if(TRALIG.UpdateDB){

  gethead()->addup( new AMSTrAligFit());
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
//============================================================================
void AMSJob::_catof2initjob(){
 if(TFREFFKEY.relogic[0]==1){
   TOF2STRRcalib::init();// TOF STRR-calibr.
   cout<<"TOF2STRRcalib-init done !!!"<<'\n';
 }
 if(TFREFFKEY.relogic[0]==2){
   TOF2TDIFcalib::init();// TOF TDIF-calibr.
   cout<<"TOF2TDIFcalib-init done !!!"<<'\n';
 }
 if(TFREFFKEY.relogic[0]==3){
   TOF2TZSLcalib::init();// TOF TzSl-calibr.
   cout<<"TOF2TZSLcalib-init done !!!"<<'\n';
 }
 if(TFREFFKEY.relogic[0]==4){
   TOF2AMPLcalib::init();// TOF AMPL-calibr.
   cout<<"TOF2AMPLcalib-init done !!!"<<'\n';
   TOF2AVSDcalib::init();// TOF AVSD-calibr.
   cout<<"TOF2AVSDcalib-init done !!!"<<'\n';
 }
//
}
//=================================================================================
void AMSJob::_catrdinitjob(){
}
void AMSJob::_casrdinitjob(){
}

//==========================================
void AMSJob::_cant2initjob(){
  AntiCalib::init();
}
//==========================================
void AMSJob::_caecinitjob(){
 if(ECREFFKEY.relogic[1]>0){
   ECREUNcalib::init();// ECAL REUN-calibr.
   cout<<"ECREUNcalib-init done !!!"<<'\n';
 }
 
 AMSECIdCalib::init();
 AMSECIdCalib::clear();


}
//==========================================

void AMSJob::_caaxinitjob(){
}

void AMSJob::_remfinitjob(){
READMFIELD();
if(MISCFFKEY.BTempCorrection){
cout <<"AMSJob::_remfinitjob-I-Magnetic Field Temp Corrections will be used"<<endl; 
}
}


void AMSJob::_retkinitjob(){
AMSgObj::BookTimer.book("RETKEVENT");
AMSgObj::BookTimer.book("TrCluster");
AMSgObj::BookTimer.book("TrClusterRefit");
AMSgObj::BookTimer.book("TrRecHit");
AMSgObj::BookTimer.book("TrTrack");
AMSgObj::BookTimer.book("TrFalseX");
}
//===================================================================================
//
void AMSJob::_retof2initjob(){
//
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:validation");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");

      if(isMonitoring() & (AMSJob::MTOF | AMSJob::MAll)){ // TOF Online histograms
        _retof2onlineinitjob();      // (see tofonline02.C)
      }
//
// ===> Clear JOB-statistics counters for SIM/REC :
//
    TOF2JobStat::clear();
//
// ===> Book histograms for REC :
//
    TOF2JobStat::bookhist();
//
//-------------------------
// 
  if(TFREFFKEY.ReadConstFiles/100>0){//(DPC) Take ThreshCuts-set from Data-Cards
//
    TOF2Varp::tofvpar.init(TFREFFKEY.daqthr, TFREFFKEY.cuts);//create ThrCuts-obj from Data-Cards
  }
//--------
  if(TFREFFKEY.ReadConstFiles%10>0){//(DPC) Take CalibConst from CalibFiles
//
    TOF2Brcal::build();//create scbrcal-objects from CalibFiles
  }
//--------
//
  if((TFREFFKEY.ReadConstFiles%100)/10>0 && isRealData()){//(DPC) Take RealData Peds from files
//
    TOFBPeds::build();//create RealData scbrped-objects from peds-file
  }
// 
//-----------
  AMSTOFCluster::init();
  AMSSCIds::inittable();
}
//=============================================================================================
void AMSJob::_reanti2initjob(){
//
    AMSgObj::BookTimer.book("REANTIEVENT");
    if(ATREFFKEY.reprtf[0]>0)ANTI2JobStat::bookh();
//
    ANTI2JobStat::clear();
//-----------
//
  if(ATREFFKEY.ReadConstFiles/10>0 && isRealData()){// (PC) RDPeds from ext.files
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
}
//============================================================================================
void AMSJob::_reecalinitjob(){
//


// setup - data type dep init
if(ECREFFKEY.SimpleRearLeak[0]<0){
  if(isRealData() ){
    ECREFFKEY.SimpleRearLeak[0]=0.015;
    ECREFFKEY.SimpleRearLeak[1]=0.99e-3;
    ECREFFKEY.SimpleRearLeak[2]=3.6;
    ECREFFKEY.SimpleRearLeak[3]=1.045e-3;
    for (int i=0;i<4;i++)cout <<" RearLeak["<<i<<"]="<<ECREFFKEY.SimpleRearLeak[i]<<endl;
  }
  else{
    for(int k=0;k<4;k++){
      if( ECREFFKEY.SimpleRearLeak[k]<0) ECREFFKEY.SimpleRearLeak[k]*=-1;
    }
    for (int i=0;i<4;i++)cout <<" RearLeak["<<i<<"]="<<ECREFFKEY.SimpleRearLeak[i]<<endl;
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
//              ECREFFKEY.ReadConstFiles=DC
  if(ECREFFKEY.ReadConstFiles/10>0){//D Thresh/Cuts-object will be created from data-cards  
    ECALVarp::ecalvpar.init(ECREFFKEY.thresh, ECREFFKEY.cuts);
  }
//
// ===> create ecpmcal-calib-objects:
//
  if(ECREFFKEY.ReadConstFiles%10>0){//C  Calib.const-object(MC/RD) will be created from calib-files 
    ECcalib::build();
  } 
//-------------------------
// ===> create EC-SubCell peds/sigs (ECPMPeds structure):
//
//    if(isRealData){
//      ECPMPeds::build();
//    }
//-----------
}
//===================================================================

void AMSJob::_reaxinitjob(){
  AMSgObj::BookTimer.book("REAXEVENT");
  AMSgObj::BookTimer.book("ReAxRefit");
AMSgObj::BookTimer.book("ReTRDRefit"); 
AMSgObj::BookTimer.book("ReTKRefit"); 
AMSgObj::BookTimer.book("ReGeaneRefit"); 
AMSgObj::BookTimer.book("ReECRefit"); 
AMSgObj::BookTimer.book("ReRICHRefit"); 
AMSgObj::BookTimer.book("ReTOFRefit"); 
  AMSgObj::BookTimer.book("ReAxPid");
  AMSgObj::BookTimer.book("part::loc2gl");
  if(AMSFFKEY.Update){
    for(int i=0;i<gethead()->gettdvn();i++){
      if( strcmp(gethead()->gettdvc(i),"ChargeLkhd01")==0 ){
        AMSCharge::init();
      }
    }
  }


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
//
// Magnetic Field Map
//
AMSTimeID::CType server=AMSTimeID::Standalone;
#ifdef __CORBA__
if(!AMSProducer::gethead()->IsSolo())server=AMSTimeID::Client;
#endif
{
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
TID.add (new AMSTimeID(AMSID("MagneticFieldMap",isRealData()),
   begin,end,sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile),(void*)&TKFIELD.iniok,server));

//TID.add (new AMSTimeID(AMSID("MagneticFieldMapAddOn",isRealData()),
//   begin,end,sizeof(TKFIELDADDON_DEF),(void*)&TKFIELDADDON.iqx,server));
}
bool NeededByDefault=isSimulation();
//----------------------------
//
// Pedestals, Gains,  Sigmas & commons noise for tracker
//      

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
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::peds,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell),server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmaraws,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell),server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::gains,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell),server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmas,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell),server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::status,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell),server,NeededByDefault));
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
    (void*)AMSTrIdSoft::cmnnoise,server,NeededByDefault));
//TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
//   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
//   (void*)AMSTrIdSoft::indnoise,server,NeededByDefault));
}





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
    (void*)AMSTRDIdSoft::_ped,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID("TRDGains",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_gain[0])*AMSTRDIdSoft::getgaisize(),
    (void*)AMSTRDIdSoft::_gain,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID("TRDSigmas",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_sig[0])*AMSTRDIdSoft::getsigsize(),
    (void*)AMSTRDIdSoft::_sig,server,NeededByDefault));
 TID.add (new AMSTimeID(AMSID("TRDStatus",isRealData()),
    begin,end,sizeof(AMSTRDIdSoft::_status[0])*AMSTRDIdSoft::getstasize(),
    (void*)AMSTRDIdSoft::_status,server,NeededByDefault));
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
//tfre->DPC,tfmc->PTS
if(TFREFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//CalibConst from DB
  
  TID.add (new AMSTimeID(AMSID("Tofbarcal2",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOF2Brcal::scbrcal[0][0]),
    (void*)&TOF2Brcal::scbrcal[0][0],server,NeededByDefault));
   
  end.tm_year=TFREFFKEY.year[1];
//-----
if(TFREFFKEY.ReadConstFiles/100==0)end.tm_year=TFREFFKEY.year[0]-1;//ThreshCuts-set from DB

  TID.add (new AMSTimeID(AMSID("Tofvpar2",isRealData()),
    begin,end,sizeof(TOF2Varp::tofvpar),
    (void*)&TOF2Varp::tofvpar,server,NeededByDefault));
    
  end.tm_year=TFREFFKEY.year[1];
//----- 
if((TFREFFKEY.ReadConstFiles%100)/10==0 &&
    isRealData())end.tm_year=TFREFFKEY.year[0]-1;//Real data Peds.fromDB
if((TFMCFFKEY.ReadConstFiles%1000)/100==0 &&
     !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//MC data Peds.fromDB
    
  TID.add (new AMSTimeID(AMSID("Tofpeds",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOFBPeds::scbrped[0][0]),
    (void*)&TOFBPeds::scbrped[0][0],server,NeededByDefault));
   
  end.tm_year=TFREFFKEY.year[1];
//----- 
if((TFMCFFKEY.ReadConstFiles%100)/10==0 && !isRealData())end.tm_year=TFREFFKEY.year[0]-1;//MCTimeDistr.fromDB

  TID.add (new AMSTimeID(AMSID("Tofmcscans2",0),
    begin,end,TOF2GC::SCBTPN*sizeof(TOFWScan::scmcscan[0]),
    (void*)&TOFWScan::scmcscan[0],server,NeededByDefault));
    
  end.tm_year=TFREFFKEY.year[1];
//-----
if(!isRealData()){ 
  if(TFMCFFKEY.ReadConstFiles%10==0)end.tm_year=TFREFFKEY.year[0]-1;//"MC Seeds" fromDB
    
  TID.add (new AMSTimeID(AMSID("TofbarcalMS",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOFBrcalMS::scbrcal[0][0]),
    (void*)&TOFBrcalMS::scbrcal[0][0],server,NeededByDefault));
    
  end.tm_year=TFREFFKEY.year[1];
}
 
//
   
}
//---------------------------------------
//
//   ANTI : TDV-resetvation for calibration parameters of all sc.sectors
//
{
 tm begin;
 tm end;
 begin.tm_isdst=0;
 end.tm_isdst=0;
 
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
//atre->PVS, atmc->P
if(ATREFFKEY.ReadConstFiles%10==0)end.tm_year=ATREFFKEY.year[0]-1;//StableParams from DB
//
  TID.add (new AMSTimeID(AMSID("Antispcal2",isRealData()),
     begin,end,ANTI2C::MAXANTI*sizeof(ANTI2SPcal::antispcal[0]),
                         (void*)&ANTI2SPcal::antispcal[0],server,NeededByDefault));
//
  end.tm_year=ATREFFKEY.year[1];
//---------
if((ATREFFKEY.ReadConstFiles/10)%10==0)end.tm_year=ATREFFKEY.year[0]-1;//VariableParams from DB
//
  TID.add (new AMSTimeID(AMSID("Antivpcal2",isRealData()),
     begin,end,ANTI2C::MAXANTI*sizeof(ANTI2VPcal::antivpcal[0]),
                         (void*)&ANTI2VPcal::antivpcal[0],server,NeededByDefault));
//
  end.tm_year=ATREFFKEY.year[1];
//---------
if(ATREFFKEY.ReadConstFiles/100==0 &&
    isRealData())end.tm_year=ATREFFKEY.year[0]-1;//Real data Peds.fromDB
if(ATMCFFKEY.ReadConstFiles==0 &&
   !isRealData())end.tm_year=ATREFFKEY.year[0]-1;//MC data Peds.fromDB
//
  TID.add (new AMSTimeID(AMSID("Antipeds",isRealData()),
    begin,end,ANTI2C::MAXANTI*sizeof(ANTIPeds::anscped[0]),
                            (void*)&ANTIPeds::anscped[0],server,NeededByDefault));
//
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

  TID.add (new AMSTimeID(AMSID("RichPedCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
			 *sizeof(AMSRICHIdSoft::_ped[0]),
                         (void*)&AMSRICHIdSoft::_ped[0],server,NeededByDefault));

  TID.add (new AMSTimeID(AMSID("RichSpedCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
                         *sizeof(AMSRICHIdSoft::_sig_ped[0]),
                         (void*)&AMSRICHIdSoft::_sig_ped[0],server,NeededByDefault));


  TID.add (new AMSTimeID(AMSID("Richlambdacalib",isRealData()),
			 begin,end,AMSRICHIdSoft::_nchannels*2
			           *sizeof(AMSRICHIdSoft::_lambda[0]),
			 (void*)&AMSRICHIdSoft::_lambda[0],server,NeededByDefault));


  TID.add (new AMSTimeID(AMSID("RichScaleCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
			 *sizeof(AMSRICHIdSoft::_scale[0]),
                         (void*)&AMSRICHIdSoft::_scale[0],server,NeededByDefault));

  TID.add (new AMSTimeID(AMSID("RichTransmCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
			 *sizeof(AMSRICHIdSoft::_transparency[0]),
                         (void*)&AMSRICHIdSoft::_transparency[0],server,NeededByDefault));

  
  TID.add (new AMSTimeID(AMSID("RichLDynCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
                         *sizeof(AMSRICHIdSoft::_lambda_dyn[0]),
                         (void*)&AMSRICHIdSoft::_lambda_dyn[0],server,NeededByDefault));
  
  TID.add (new AMSTimeID(AMSID("RichSDynCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
                         *sizeof(AMSRICHIdSoft::_scale_dyn[0]),
                         (void*)&AMSRICHIdSoft::_scale_dyn[0],server,NeededByDefault));

  TID.add (new AMSTimeID(AMSID("RichThresholdCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels*2
                         *sizeof(AMSRICHIdSoft::_threshold[0]),
                         (void*)&AMSRICHIdSoft::_threshold[0],server,NeededByDefault));

  TID.add (new AMSTimeID(AMSID("RichModeBoundaryCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels
                         *sizeof(AMSRICHIdSoft::_gain_mode_boundary[0]),
                         (void*)&AMSRICHIdSoft::_gain_mode_boundary[0],server,NeededByDefault));


  TID.add (new AMSTimeID(AMSID("RichStatusCalib",isRealData()),
                         begin,end,AMSRICHIdSoft::_nchannels
                         *sizeof(AMSRICHIdSoft::_status[0]),
                         (void*)&AMSRICHIdSoft::_status[0],server,NeededByDefault));


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
//ecre->DC; ecmc->CP
//
if(ECREFFKEY.ReadConstFiles%10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib(MC/RD).fromDB

  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalib(),
     begin,end,ecalconst::ECPMSL*sizeof(ECcalib::ecpmcal[0][0]),
                                  (void*)&ECcalib::ecpmcal[0][0],server));
  end.tm_year=ECREFFKEY.year[1];
//--------				  
if(ECREFFKEY.ReadConstFiles/10==0)end.tm_year=ECREFFKEY.year[0]-1;//DataCardThresh/Cuts fromDB

  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVvpar(),
     begin,end,sizeof(ECALVarp::ecalvpar),
                                      (void*)&ECALVarp::ecalvpar,server));
  end.tm_year=ECREFFKEY.year[1];
//--------
if(!isRealData()){//"MC.Seeds" TDV only for MC-run.    
  if((ECMCFFKEY.ReadConstFiles%100)/10==0)end.tm_year=ECREFFKEY.year[0]-1;//Calib"MC.Seeds" fromDB
	     
  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVcalibMS(),
     begin,end,ecalconst::ECPMSL*sizeof(ECcalibMS::ecpmcal[0][0]),
                                  (void*)&ECcalibMS::ecpmcal[0][0],server));
  end.tm_year=ECREFFKEY.year[1];
}
//--------
if(ECMCFFKEY.ReadConstFiles%10==0 &&
             !isRealData())end.tm_year=ECREFFKEY.year[0]-1;//MC Peds fromDB
	     
  TID.add (new AMSTimeID(AMSEcalRawEvent::getTDVped(),
    begin,end,ecalconst::ECPMSL*sizeof(ECPMPeds::pmpeds[0][0]),
                                    (void*)&ECPMPeds::pmpeds[0][0],server));
  end.tm_year=ECREFFKEY.year[1];
//--------
}
//
//---------------------------------------
//
// Data to fit particle charge magnitude
//
{
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=AMSCharge::_sec[0];
begin.tm_min=AMSCharge::_min[0];
begin.tm_hour=AMSCharge::_hour[0];
begin.tm_mday=AMSCharge::_day[0];
begin.tm_mon=AMSCharge::_mon[0];
begin.tm_year=AMSCharge::_year[0];

end.tm_sec=AMSCharge::_sec[1];
end.tm_min=AMSCharge::_min[1];
end.tm_hour=AMSCharge::_hour[1];
end.tm_mday=AMSCharge::_day[1];
end.tm_mon=AMSCharge::_mon[1];
end.tm_year=AMSCharge::_year[1];



TID.add (new AMSTimeID(AMSID("ChargeLkhd01",isRealData()),
   begin,end,100*ncharge*TOFTypes*sizeof(AMSCharge::_lkhdTOF[0][0][0]),
   (void*)AMSCharge::_lkhdTOF[0],server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd02",isRealData()),
   begin,end,100*ncharge*TrackerTypes*sizeof(AMSCharge::_lkhdTracker[0][0][0]),
   (void*)AMSCharge::_lkhdTracker[0],server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd03",isRealData()),
   begin,end,ncharge*TOFTypes*sizeof(AMSCharge::_lkhdStepTOF[0][0]),
   (void*)AMSCharge::_lkhdStepTOF,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd04",isRealData()),
   begin,end,ncharge*TrackerTypes*sizeof(AMSCharge::_lkhdStepTracker[0][0]),
   (void*)AMSCharge::_lkhdStepTracker,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd05",isRealData()),
   begin,end,ncharge*TOFTypes*sizeof(AMSCharge::_lkhdNormTOF[0][0]),
   (void*)AMSCharge::_lkhdNormTOF,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd06",isRealData()),
   begin,end,ncharge*TrackerTypes*sizeof(AMSCharge::_lkhdNormTracker[0][0]),
   (void*)AMSCharge::_lkhdNormTracker,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd07",isRealData()),
   begin,end,ncharge*TOFTypes*sizeof(AMSCharge::_lkhdSlopTOF[0][0]),
   (void*)AMSCharge::_lkhdSlopTOF,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd08",isRealData()),
   begin,end,ncharge*TrackerTypes*sizeof(AMSCharge::_lkhdSlopTracker[0][0]),
   (void*)AMSCharge::_lkhdSlopTracker,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd09",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_chargeTOF[0]),
   (void*)AMSCharge::_chargeTOF,server));
TID.add (new AMSTimeID(AMSID("ChargeLkhd10",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_chargeTracker[0]),
   (void*)AMSCharge::_chargeTracker,server));


}
//---------------------------
{
  // TOF Temperature data

   tm begin=AMSmceventg::Orbit.End;
   tm end=AMSmceventg::Orbit.Begin;
   
     TID.add (new AMSTimeID(AMSID("TOF2Temperature",isRealData()),
                         begin,end,
                         TOF2Varp::tofvpar.gettoftsize(),(void*)TOF2Varp::tofvpar.gettoftp(),server));
}
//-----------------------------
{
  // Magnet Temperature data

   tm begin=AMSmceventg::Orbit.End;
   tm end=AMSmceventg::Orbit.Begin;
   TID.add (new AMSTimeID(AMSID("MagnetTemperature",isRealData()),
                         begin,end,
                         MagnetVarp::getmagnettsize(),(void*)MagnetVarp::getmagnettp(),server));
   
}
//-----------------------------
{
  // Scaler Data

   tm begin=AMSmceventg::Orbit.End;
   tm end=AMSmceventg::Orbit.Begin;
     TID.add (new AMSTimeID(AMSID("ScalerN",isRealData()),
                         begin,end,
                         Trigger2LVL1::getscalerssize(),(void*)Trigger2LVL1::getscalersp(),server));
   
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
  TID.add (new AMSTimeID(AMSID("ShuttlePar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::Array),(void*)AMSEvent::Array,server));
}


if(MISCFFKEY.BeamTest){
  tm begin=AMSmceventg::Orbit.End;
  tm end=AMSmceventg::Orbit.Begin;
  TID.add (new AMSTimeID(AMSID("BeamPar",isRealData()),
                         begin,end,
                         sizeof(AMSEvent::ArrayB),(void*)AMSEvent::ArrayB,server));
}




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
}

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



{
  tm begin;
  tm end;
  AMSTrAligFit::InitDB();
  if(AMSFFKEY.Update==90){
    begin=AMSmceventg::Orbit.Begin;
    end=AMSmceventg::Orbit.End;
  }
  else{
     begin=AMSmceventg::Orbit.End;
     end=AMSmceventg::Orbit.Begin;
  }
  AMSTimeID * ptdv= (AMSTimeID*) TID.add(new AMSTimeID(AMSID("TrAligglDB",
                          isRealData()),begin,end,AMSTrAligFit::gettraliggldbsize(),
                          AMSTrAligFit::gettraliggldbp(),server));
}



if (AMSFFKEY.Update){

#ifdef __DB__
  if (AMSFFKEY.Write == 0 ) {
#endif
  // Here update dbase

    AMSTimeID * offspring=(AMSTimeID*)TID.down();
    while(offspring){
            for(int i=0;i<AMSJob::gethead()->gettdvn();i++){     
               offspring->checkupdate(AMSJob::gethead()->gettdvc(i));
             }
    if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
    offspring=(AMSTimeID*)offspring->next();
    }
#ifdef __DB__
  } 
#endif  
}
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
      AMSTrAligFit a;
     AMSNode *p=JobMap.getp(AMSID(a.getname(),a.getid()));
     if(!p){
      cerr << "AMSJob::getaligtructure-F-no alig structure found"<<endl;
      exit(1);
      return 0;
     }
     else return  p;
}

AMSTimeID * AMSJob::gettimestructure(const AMSID & id){
     AMSNode *p=JobMap.getp(id);
     if(!p){
       cerr << "AMSJob::gettimestructure-F-no time structure found "<<id<<endl;
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
cout <<"AMS Software version "<<AMSCommonsI::getversion()<<"/"<<AMSCommonsI::getbuildno()<<"/"<<AMSCommonsI::getosno()<<endl;
}

AMSJob::~AMSJob(){
  cout << "~AMSJob called "<<endl;
  HPRINT(9797+1);
  _signendjob();
  if(isSimulation())uhend(CCFFKEY.run,GCFLAG.IEVENT,CCFFKEY.curtime);
  else uhend();
  cout <<"uhend finished"<<endl;
  _tkendjob();
  cout <<"tkendjob finished"<<endl;
    _tof2endjob();
    cout <<"tof2endjob finished"<<endl;
    _anti2endjob();
    cout <<"anti2endjob finished"<<endl;
    _ecalendjob();
    cout <<"ecalendjob finished"<<endl;
    _trdendjob();
    cout <<"trdendjob finished"<<endl;
    _srdendjob();
    cout <<"srdendjob finished"<<endl;

_dbendjob();
  cout <<"dbendjob finished"<<endl;
_axendjob();
  cout <<"axendjob finished"<<endl;
  
 TriggerLVL302::printfc();
 
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
delete &(AMSgObj::BookTimer);
#endif
abort();
}
#endif
}
#include <producer.h>
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
  if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime(),true);
else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::Ntuple,ntuple_entries,e,t,true);
#endif
}
else if(IOPA.WriteRoot  ){
if(_pntuple)_pntuple->endR();
#ifdef __CORBA__
if(_pntuple){
int root_entries=_pntuple->getentries();
  if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime(),true);
else AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RootFile,root_entries,e,t,true);
}
#endif
}
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
       system((const char*)mdir);
       strcat(_rootfilename,"/");
      }
      sprintf(event,"%d",run);
      strcat(_rootfilename,event);
      sprintf(event,".%07d",eventno);
      strcat(_rootfilename,event);
      strcat(_rootfilename,".root");
     }
    if(_pntuple)_pntuple->initR(_rootfilename);
    else{
        _pntuple = new AMSNtuple(_rootfilename);
        _pntuple->initR(_rootfilename);
    }
#ifdef __CORBA__
      AMSProducer::gethead()->sendNtupleStart(DPS::Producer::RootFile,_rootfilename,run,eventno,tt);
#endif
  }
}


void AMSJob::uhinit(integer run, integer eventno, time_t tt) 
throw (amsglobalerror){
  if(_NtupleActive)uhend();
  if(IOPA.hlun ){
   
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
      sprintf(event,".%d",eventno);
      strcat(_ntuplefilename,event);
      strcat(_ntuplefilename,".hbk");
     }
    
    cout <<"Trying to open histo file "<<_ntuplefilename<<endl;
    npq_(); 
    HROPEN(IOPA.hlun,"output",_ntuplefilename,"NPQ",rsize,iostat);
    if(iostat){
     cerr << "Error opening Histo file "<<_ntuplefilename<<endl;
     throw amsglobalerror("UnableToOpenHistoFile",3);
    }
    else cout <<"Histo file "<<_ntuplefilename<<" opened."<<endl;

// Open the n-tuple
    if(_pntuple)_pntuple->init();
    else{
        _pntuple = new AMSNtuple(IOPA.ntuple,"AMS Ntuple");
    }
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
   HBOOK1(200107," adc over",3000,29999.5,32999.5,0.);
}

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
  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 4){
    AMSTrIdCalib::getaverage();
  }
  if(isMonitoring() & (AMSJob::MTracker | AMSJob::MAll)){
   AMSTrIdCalib::offmonhist();    
  }



}

void AMSJob::_signendjob(){
AMSmceventg::endjob();
}

//
//-------------------------------------------------------------------
void AMSJob::_tof2endjob(){
       if(isSimulation())TOF2JobStat::outpmc();
       TOF2JobStat::outp();
       TOF2JobStat::printstat(); // Print JOB-TOF statistics
}
//-----------------------------------------------------------------------
void AMSJob::_anti2endjob(){
  ANTI2JobStat::printstat();
  if(isSimulation() && ATMCFFKEY.mcprtf>0)ANTI2JobStat::outpmc();
  ANTI2JobStat::outp();
}
//-----------------------------------------------------------------------
void AMSJob::_ecalendjob(){
//

  if((isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]<=0 ){
    AMSECIdCalib::getaverage();
    AMSECIdCalib::write();
  }

       EcalJobStat::printstat(); // Print JOB-Ecal statistics
       if(isSimulation())EcalJobStat::outpmc();
       EcalJobStat::outp();
}
//-----------------------------------------------------------------------
void AMSJob::_trdendjob(){

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


  //Status Stuff
#ifndef __CORBA__
  if( AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
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
   Message("AMSJob::_dbendjob -I- UpdateMe is set. Update database and tables.");
   int rstatus = lms -> AddAllTDV();
  } else {
   Message("AMSJob::_dbendjob -I- UpdateMe != 1. NO UPDATE");
  }
#else
    if (AMSFFKEY.Update && !isCalibration()){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }
#endif
}



//=================================================================================================
  void AMSJob::_redaq2initjob(){
     AMSgObj::BookTimer.book("SIDAQ");
     AMSgObj::BookTimer.book("REDAQ");
     if(IOPA.Portion<1. && isMonitoring())cout <<"AMSJob::_redaq2initjob()-W-Only about "<<IOPA.Portion*100<<"% events will be processed."<<endl; 
    // Add subdetectors to daq
    //
     
// ecal
   if((isCalibration() & CEcal) && ECREFFKEY.relogic[1]==0){
    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSECIdCalib::buildSigmaPed);
   }
   else if((isCalibration() & CEcal) && ECREFFKEY.relogic[1]==-1){
    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSECIdCalib::buildPedDiff);
   }
   else{
    DAQEvent::addsubdetector(&AMSEcalRawEvent::checkdaqid,&AMSEcalRawEvent::buildrawRaw);
   }

  {  // mc
    if(!isRealData()){
    DAQEvent::addsubdetector(&AMSmceventg::checkdaqid,&AMSmceventg::buildraw);
    DAQEvent::addblocktype(&AMSmceventg::getmaxblocks,&AMSmceventg::calcdaqlength,
    &AMSmceventg::builddaq);

    DAQEvent::addsubdetector(&AMSTrMCCluster::checkdaqid,&AMSTrMCCluster::buildraw);
    DAQEvent::addblocktype(&AMSTrMCCluster::getmaxblocks,&AMSTrMCCluster::calcdaqlength,
    &AMSTrMCCluster::builddaq);



    DAQEvent::addsubdetector(&AMSEvent::checkdaqidSh,&AMSEvent::buildrawSh);
    DAQEvent::addblocktype(&AMSEvent::getmaxblocksSh,
    &AMSEvent::calcdaqlengthSh,&AMSEvent::builddaqSh);
    }

  }


  {
    // lvl1

    DAQEvent::addsubdetector(&Trigger2LVL1::checkdaqid,&Trigger2LVL1::buildraw);
    DAQEvent::addblocktype(&Trigger2LVL1::getmaxblocks,&Trigger2LVL1::calcdaqlength,
    &Trigger2LVL1::builddaq);


}

{
//           Header
    DAQEvent::addblocktype(&AMSEvent::getmaxblocks,&AMSEvent::calcdaqlength,
    &AMSEvent::builddaq);
//           Header Tracker HK
    DAQEvent::addblocktype(&AMSEvent::getmaxblocks,&AMSEvent::calcTrackerHKl,
    &AMSEvent::buildTrackerHKdaq,4);
}


{
//           lvl3
    DAQEvent::addsubdetector(&TriggerLVL302::checkdaqid,&TriggerLVL302::buildraw);
    DAQEvent::addblocktype(&TriggerLVL302::getmaxblocks,&TriggerLVL302::calcdaqlength,
    &TriggerLVL302::builddaq);

}    
if(DAQCFFKEY.SCrateinDAQ){
//         tof+anti+???
    DAQEvent::addsubdetector(&DAQS2Block::checkblockid,&DAQS2Block::buildraw);
    DAQEvent::addblocktype(&DAQS2Block::getmaxblocks,&DAQS2Block::calcblocklength,
                           &DAQS2Block::buildblock);
}    

if((AMSJob::gethead()->isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
 {

  if(DAQCFFKEY.OldFormat || !isRealData()){
// special tracker ped/sigma calc
    if(TRCALIB.Method == 1)
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrIdCalib::buildSigmaPed);
    else
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,
&AMSTrIdCalib::buildSigmaPedA);
  }
  else{
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrIdCalib::buildSigmaPedB);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidMixed,&AMSTrIdCalib::buildSigmaPedB);
 
    if(TRMCFFKEY.GenerateConst){
    //Tracker ped/sigma etc ( "Event" mode)

      DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkcmnSRawid,&AMSTrRawCluster::updcmnSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);
      DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidParameters,&AMSTrRawCluster::buildrawParameters);


    }
  }

 }
}
else {

if(DAQCFFKEY.LCrateinDAQ){
//           tracker reduced

    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidMixed,&AMSTrRawCluster::buildrawMixed);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkcmnSRawid,&AMSTrRawCluster::updcmnSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidCompressed,&AMSTrRawCluster::buildrawCompressed);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqid,&AMSTrRawCluster::buildraw);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocks,
    &AMSTrRawCluster::calcdaqlength,&AMSTrRawCluster::builddaq);



    //Tracker ped/sigma etc ( "Event" mode)

    DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidParameters,&AMSTrRawCluster::buildrawParameters);


}   


if(DAQCFFKEY.LCrateinDAQ ){
//           tracker raw

  if(DAQCFFKEY.OldFormat || !isRealData()){
    if(TRCALIB.Method == 1)
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRaw);
    else
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRawA);
  }
  else{
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqidRaw,&AMSTrRawCluster::buildrawRawB);
  }

    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocksRaw,
    &AMSTrRawCluster::calcdaqlengthRaw,&AMSTrRawCluster::builddaqRaw);



}    


{
//           tracker H/K Static

    DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSid,&AMSTrRawCluster::updstatusS,4);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSid,&AMSTrRawCluster::updpedS,4);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checksigmaSid,&AMSTrRawCluster::updsigmaS,4);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
    &AMSTrRawCluster::calcstatusSl,&AMSTrRawCluster::writestatusS,4);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
    &AMSTrRawCluster::calcpedSl,&AMSTrRawCluster::writepedS,4);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblockS,
    &AMSTrRawCluster::calcsigmaSl,&AMSTrRawCluster::writesigmaS,4);


}    

}
}
//==========================================================================================
#ifdef __DB__

integer AMSJob::FindTheBestTDV(char* name, integer id, time_t timeV, 
                              integer &S, time_t &I, time_t &B, time_t &E)
//
//  (1) find TDV by name in tdvNameTab
//  (2) find timeB =< timeV =< timeE
//  for all TDV's which satisfies (2) choose one with the latest 
//  insert time
//
// name   - object name
// id     - object id
// time   - object time
// S      - object size
// I,B,E  - insert, begin, end time the most satisfies to 'time'
//
// rstatus = -1  not found
// rstatus =  1  found
{
  integer rstatus = -1;
#ifdef __DB__
  //geant t1, t2;
  //TIMEX(t1);
  for (int i=0; i<ntdvNames; i++) {
    if(strcmp(name,tdvNameTab[i]) == 0) {
     if(_ptr_start[i] > -1 && _ptr_end[i] > -1) {
       integer  ptr = -1;
       time_t   insert = 0;
       for (int j=_ptr_start[i]; j<_ptr_end[i]; j++) {
        if(_tdv[j]._id == id) {
         if (timeV >= _tdv[j]._begin && timeV <= _tdv[j]._end) {
           if (_ptr_end[i] - _ptr_start[i] - 1 != 0) {
             if (insert == 0) {
               insert = _tdv[j]._insert;
               ptr    = j;
             } else {
               if (insert < _tdv[j]._insert) {
                insert = _tdv[j]._insert;
                ptr    = j;
               }
             }
           } else {
            ptr = j;
           }
         }
        } // compare id's
       }
       if (ptr > -1) {
        I = _tdv[ptr]._insert;
        B = _tdv[ptr]._begin;
        E = _tdv[ptr]._end;
        S = _tdv[ptr]._size;
#ifdef __AMSDEBUG__
        //TIMEX(t2);
        //cout <<t2-t1<<" sec."<<endl;
        //cout <<"found TDV for "<<tdvNameTab[i]<<", size "<<S<<endl;
        //cout <<"Time of Event "<<asctime(localtime(&timeV))<<endl;
        //cout <<"i/b/e "<<asctime(localtime(&_tdv[ptr]._insert))
        //<<"      "<<asctime(localtime(&_tdv[ptr]._begin))
        //<<"      "<<asctime(localtime(&_tdv[ptr]._end))<<endl;
#endif
       rstatus = 1;
       } else {
         cerr <<"AMSJob::FindTheBestTDV -W- cannot find TDV for "<<name<<endl;
         cerr <<"time "<<asctime(localtime(&timeV))<<endl;
       }
      break;
     } else {
       cout<<"AMSJob::FindTheBestTDV -W- no TDV for "<<name<<endl;
       rstatus = -1;
     }
    }
  }
#endif
 return rstatus;
}

#endif

#ifdef __DB__

integer AMSJob::FillJobTDV(integer nobj, tdv_time *tdv)
//
// Fill TDV table by values read from database
//
{
  integer rstatus = -1;

  
  if (nobj < 1) {
    cerr <<"AMSJob::FillJobTDV -E- value out of range "<<nobj<<endl;
    return rstatus;
  }
  _tdv = new tdv_time[nobj];
  for (int i=0; i<nobj; i++) { _tdv[i] = tdv[i];}
 
  return 1;
}
#endif


#ifdef __DB__

integer AMSJob::SetTDVPtrs(integer start[], integer end[])
//
// Set start/end ptrs for TDV table 
//
{
  for (int i=0; i<ntdvNames; i++) {
   _ptr_start[i] = start[i];
   _ptr_end[i]   = end[i];
  }
 return 1;
}
integer AMSJob::FillTDVTable(){

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
integer nobj = 0;
while(offspring){
  tdvNameTab[nobj] = offspring -> getname();
  tdvIdTab[nobj]   = offspring -> getid();
  nobj++;
  offspring=(AMSTimeID*)offspring->next();
}
if (_ptr_start) delete [] _ptr_start;
if (_ptr_end)   delete [] _ptr_end;
_ptr_start = new integer[nobj];
_ptr_end   = new integer[nobj];
ntdvNames  = nobj;
return nobj;
}

#endif
