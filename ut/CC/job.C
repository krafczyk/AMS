// $Id: job.C,v 1.356 2001/01/22 10:09:53 choutko Exp $
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
#include <tofdbc.h>
#include <ecaldbc.h>
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
#include <antidbc.h>
#include <ctcdbc.h>
#include <tofsim02.h>
#include <tofsim.h>
#include <tofrec02.h>
#include <tofrec.h>
#include <tofcalib02.h>
#include <tofcalib.h>
#include <trigger102.h>
#include <trigger1.h>
#include <trigger3.h>
#include <bcorr.h>
#include <trid.h>
#include <trrawcluster.h>
#include <daqevt.h>
#include <daqs2block.h>
#include <daqblock.h>
#include <ntuple.h>
#include <user.h>
#include <tralig.h>
#include <status.h>
#include <richdbc.h>
#include <mccluster.h>
#include <sys/stat.h>
#include <producer.h>
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

long AMSJob::GetNtupleFileSize(){
 struct stat buffer;
 
 return stat(_ntuplefilename,&buffer)?0:buffer.st_size;
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
const uinteger AMSJob::MTracker=1024;
const uinteger AMSJob::MTOF=2048;
const uinteger AMSJob::MAnti=4096;
const uinteger AMSJob::MCerenkov=4096*2;
const uinteger AMSJob::MLVL1=4096*4;
const uinteger AMSJob::MLVL3=4096*8;
const uinteger AMSJob::MAxAMS=4096*16;
const uinteger AMSJob::MAll=4096*32;
const uinteger AMSJob::Calibration=AMSJob::CTracker+
                                   AMSJob::CTOF+
                                   AMSJob::CAnti+
                                   AMSJob::CCerenkov+
                                   AMSJob::CMagnet+
                                   AMSJob::CRICH+
                                   AMSJob::CTRD+
                                   AMSJob::CAMS;

const uinteger AMSJob::Monitoring=(AMSJob::MTracker)+
                                  (AMSJob::MTOF)+
                                  (AMSJob::MAnti)+
                                  (AMSJob::MCerenkov)+
                                  (AMSJob::MLVL1)+
                                  (AMSJob::MLVL3)+
                                  (AMSJob::MAxAMS)+
                                  (AMSJob::MAll);
const uinteger AMSJob::Production=4096*64;
//
extern CTCCCcal ctcfcal[CTCCCMX];//  CTC calibr. objects


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
IOPA.WriteAll=2;
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

_sitofdata();
  _sitof2data();
  
_siantidata();
  _sianti2data();
  
_sictcdata();

_sitrigdata();
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

// Geometry defaults
  RICGEOM.top_radius=RICHDB::top_radius;
  RICGEOM.bottom_radius=RICHDB::bottom_radius;
  RICGEOM.hole_radius=RICHDB::hole_radius;
  RICGEOM.height=RICHDB::height;
  RICGEOM.radiator_radius=RICHDB::rad_radius;
  RICGEOM.radiator_height=RICHDB::rad_height;
  RICGEOM.radiator_box_length=RICHDB::rad_tile_size;
  RICGEOM.light_guides_height=RICHDB::lg_height;
  RICGEOM.light_guides_length=RICHDB::lg_tile_size;

// Move information to fortran commons
  RICGTKOV.usrcla=RICHDB::rad_clarity;

  RICCONTROL.iflgk_flag=0;
  RICCONTROL.recon=1;
//  FFKEY("RICGEOM",(float *)&RICGEOM,sizeof(RICGEOM_DEF)/sizeof(integer),"REAL");
  FFKEY("RICCONT",(float *)&RICCONTROL,sizeof(RICCONTROL_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_sitrigdata(){
// TOF :
// these are additional requir. to "hardware"-defined TOFMCFFKEY.trlogic[]
  LVL1FFKEY.ntof=3;// min. fired TOF-planes
// ANTI :
  LVL1FFKEY.nanti=20;// max. fired ANTI-paddles 
//
  LVL1FFKEY.RebuildLVL1=0;
  LVL1FFKEY.MaxScalersRate=20000;
  LVL1FFKEY.MinLifeTime=0.015;
  FFKEY("LVL1",(float*)&LVL1FFKEY,sizeof(LVL1FFKEY_DEF)/sizeof(integer),"MIXED");


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
  LVL3FFKEY.UseTightTOF=1;
  LVL3FFKEY.TrTOFSearchReg=9.;
  LVL3FFKEY.TrMinResidual=0.03;
  LVL3FFKEY.TrMaxResidual[0]=1.2;
  LVL3FFKEY.TrMaxResidual[1]=0.2;
  LVL3FFKEY.TrMaxResidual[2]=0.3;
  LVL3FFKEY.TrMaxHits=20;
  LVL3FFKEY.Splitting=0.04;
  LVL3FFKEY.NRep=1;
  LVL3FFKEY.Accept=0;
  LVL3FFKEY.RebuildLVL3=2;
  LVL3FFKEY.NoK=1;
  LVL3FFKEY.TrHeavyIonThr=200;
  LVL3FFKEY.SeedThr=-32;
  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");
}
//----------
//
void AMSJob::_sitrig2data(){
// TOF :
// these are additional requir. to "hardware"-defined TOFMCFFKEY.trlogic[]
  TGL1FFKEY.ntof=3;// min. fired TOF-planes
// ANTI :
  TGL1FFKEY.nanti=20;// max. fired ANTI-paddles 
//
  TGL1FFKEY.RebuildLVL1=0;
  TGL1FFKEY.MaxScalersRate=20000;
  TGL1FFKEY.MinLifeTime=0.015;
  FFKEY("TGL1",(float*)&TGL1FFKEY,sizeof(TGL1FFKEY_DEF)/sizeof(integer),"MIXED");


//  LVL3SIMFFKEY.NoiseProb[1]=2.e-10;
//  LVL3SIMFFKEY.NoiseProb[0]=LVL3SIMFFKEY.NoiseProb[1]*1.65;
//  FFKEY("L3SIM",(float*)&LVL3SIMFFKEY,sizeof(LVL3SIMFFKEY_DEF)/sizeof(integer),"MIXED");


//  LVL3EXPFFKEY.NEvents=2048;
//  LVL3EXPFFKEY.ToBad=10;
//  LVL3EXPFFKEY.TryAgain=10;
//  LVL3EXPFFKEY.Range[0][0]=2.5;
//  LVL3EXPFFKEY.Range[0][1]=2.;
//  LVL3EXPFFKEY.Range[1][0]=1000000;
//  LVL3EXPFFKEY.Range[1][1]=3;
//  LVL3EXPFFKEY.Range[2][0]=2.;
//  LVL3EXPFFKEY.Range[2][1]=2.5;

//  FFKEY("L3EXP",(float*)&LVL3EXPFFKEY,sizeof(LVL3EXPFFKEY_DEF)/sizeof(integer),"MIXED");


//  LVL3FFKEY.MinTOFPlanesFired=3;
//  LVL3FFKEY.UseTightTOF=1;
//  LVL3FFKEY.TrTOFSearchReg=9.;
//  LVL3FFKEY.TrMinResidual=0.03;
//  LVL3FFKEY.TrMaxResidual[0]=1.2;
//  LVL3FFKEY.TrMaxResidual[1]=0.2;
//  LVL3FFKEY.TrMaxResidual[2]=0.3;
//  LVL3FFKEY.TrMaxHits=20;
//  LVL3FFKEY.Splitting=0.04;
//  LVL3FFKEY.NRep=1;
//  LVL3FFKEY.Accept=0;
//  LVL3FFKEY.RebuildLVL3=2;
//  LVL3FFKEY.NoK=1;
//  LVL3FFKEY.TrHeavyIonThr=200;
//  LVL3FFKEY.SeedThr=-32;
//  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");
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
TRMCFFKEY.year[0]=96;
TRMCFFKEY.year[1]=101;
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
CCFFKEY.low=0;
CCFFKEY.earth=1;
CCFFKEY.theta=51.;
CCFFKEY.phi=290.;
CCFFKEY.polephi=108.392;
CCFFKEY.begindate=2061998;
CCFFKEY.enddate=12061998;
CCFFKEY.begintime=170000;
CCFFKEY.endtime=220000;
CCFFKEY.oldformat=0;
CCFFKEY.sdir=1;
CCFFKEY.Fast=0;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=================================================================================
void AMSJob::_sitofdata(){
  TOFMCFFKEY.TimeSigma=1.71e-10; //(1) side time resolution(sec,/1.41=121ps) 
  TOFMCFFKEY.TimeSigma2=4.5e-10;//(2)
  TOFMCFFKEY.TimeProbability2=0.035;//(3)
  TOFMCFFKEY.padl=10.5;        //(4) sc. bar transv. step ........................
  TOFMCFFKEY.Thr=0.1;          //(5) Sc.bar Elos-thresh.(Mev) to participate in Simul.   
//
  TOFMCFFKEY.mcprtf[0]=0;     //(6) TOF MC print flag for init arrays
  TOFMCFFKEY.mcprtf[1]=0;     //(7) TOF MC print flag for MC pulses
  TOFMCFFKEY.mcprtf[2]=0;     //(8) ...................... histograms
  TOFMCFFKEY.mcprtf[3]=0;     //(9) spare
  TOFMCFFKEY.mcprtf[4]=0;     //(10) spare
  TOFMCFFKEY.trlogic[0]=1; //(11) MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TOFMCFFKEY.trlogic[1]=0; //(12)..............(=0/1-> ANY3/ALL4 layer coincidence) 
  TOFMCFFKEY.fast=0;       //(13) 0/1->off/on fast generation in mceventg.C
  TOFMCFFKEY.daqfmt=0;     //(14) 0/1-> raw/reduced TDC format for DAQ simulation
  TOFMCFFKEY.birks=1;      //(15) 0/1->  not apply/apply birks corrections
  TOFMCFFKEY.adsimpl=0;    //(16)0/1->precise/simplified sim. of A/D-TovT
  TOFMCFFKEY.blshift=0.;   //(17) base line shift at fast discr.input (mv)
  TOFMCFFKEY.hfnoise=7.;   //(18) high freq. noise .......   
FFKEY("TOFMC",(float*)&TOFMCFFKEY,sizeof(TOFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//
//-----------
//
void AMSJob::_sitof2data(){
  TFMCFFKEY.TimeSigma=1.71e-10; //(1) side time resolution(sec,/1.41=121ps) 
  TFMCFFKEY.TimeSigma2=4.5e-10; //(2)
  TFMCFFKEY.TimeProbability2=0.035;//(3)
  TFMCFFKEY.padl=11.5;        //(4) not used now (spare)
  TFMCFFKEY.Thr=0.1;          //(5) Sc.bar Elos-thresh.(Mev) to participate in Simul.   
//
  TFMCFFKEY.mcprtf[0]=0;     //(6) TOF MC print flag for init arrays
  TFMCFFKEY.mcprtf[1]=0;     //(7) TOF MC print flag for MC pulses
  TFMCFFKEY.mcprtf[2]=0;     //(8) ...................... histograms
  TFMCFFKEY.mcprtf[3]=0;     //(9) spare
  TFMCFFKEY.mcprtf[4]=0;     //(10) spare
  TFMCFFKEY.trlogic[0]=1; //(11) MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TFMCFFKEY.trlogic[1]=0; //(12)........(=0/1/2-> accept at least ANY3/ALL4/SPECIAL2 layer coincidence) 
  TFMCFFKEY.fast=0;       //(13) 0/1/2->off/on fast generation in mceventg.C(2-> EC requirement)
  TFMCFFKEY.daqfmt=0;     //(14) 0/1-> raw/reduced TDC format for DAQ simulation
  TFMCFFKEY.birks=1;      //(15) 0/1->  not apply/apply birks corrections
  TFMCFFKEY.adsimpl=0;    //(16) not used now
  TFMCFFKEY.blshift=0.;   //(17) base line shift at fast discr.input (mv)
  TFMCFFKEY.hfnoise=5.;   //(18) high freq. noise .......   
FFKEY("TFMC",(float*)&TFMCFFKEY,sizeof(TFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_simag2data(){
  MAGSFFKEY.magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
  MAGSFFKEY.fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
  MAGSFFKEY.ecutge=0.002; //(3) e/g ener.cut for tracking in magnet materials(Gev) 
FFKEY("MAGS",(float*)&MAGSFFKEY,sizeof(MAGSFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================
void AMSJob::_siecaldata(){
  ECMCFFKEY.fastsim=0.;     //(1) 1/0-> fast/slow simulation algorithm(missing fast TBD)
  ECMCFFKEY.mcprtf=0;       //(2) print_hist flag (0/1->no/yes)
  ECMCFFKEY.cutge=0.001;    //(3) cutgam=cutele cut for EC_volumes
  ECMCFFKEY.silogic[0]=0;   //(4) SIMU logic flag =0/1/2->peds+noise/no_noise/no_peds
  ECMCFFKEY.silogic[1]=0;   //(5) spare
  ECMCFFKEY.mev2mev=34.33;  //(6) Geant dE/dX(MeV)->Emeas(MeV) conv.factor
  ECMCFFKEY.mev2adc=0.3788; //(7) Emeas(MeV)->ADCch conv.factor(to put MIP-m.p. in 5th channel)
FFKEY("ECMC",(float*)&ECMCFFKEY,sizeof(ECMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//---------------------------
void AMSJob::_reecaldata(){
  ECREFFKEY.reprtf[0]=0;     // (1) print_hist flag (0/1->no/yes)
  ECREFFKEY.reprtf[1]=0;     // (2) print_prof flag (0/1->no/yes)
  ECREFFKEY.reprtf[2]=0;     // (3) spare
//
  ECREFFKEY.relogic[0]=0;    // (4) 1/0->write/not EcalHits into Ntuple
  ECREFFKEY.relogic[1]=0;    // (5) spare
  ECREFFKEY.relogic[2]=0;    // (6) spare
  ECREFFKEY.relogic[3]=0;    // (7) spare
  ECREFFKEY.relogic[4]=0;    // (8) spare
//
// Run-time DAQ-thresholds(time dependent):
  ECREFFKEY.thresh[0]=2.;     // (9)  Anode(High-chan) readout threshold(ADCch)
  ECREFFKEY.thresh[1]=120.;   // (10) Anode(high,tot) "mip"-trig.thresh(mev tempor)
  ECREFFKEY.thresh[2]=300.;   // (11) ... 1st 4X0 "em"-trig.thresh(mev tempor)
  ECREFFKEY.thresh[3]=400.;   // (12) Anode(high,tot) min.Et-cut to be "HighEn-em"(mev tempor)
  ECREFFKEY.thresh[4]=2.;     // (13) Low-chan. readout thershold(ADCch)
  ECREFFKEY.thresh[5]=10000.; // (14) energy upp.limit for action of cut below (mev tempor)  
  ECREFFKEY.thresh[6]=0.15;   // (15) cut on Etail/Epeak (add. to #11,16 for "electromagneticity")
  ECREFFKEY.thresh[7]=400.;   // (16) Anode(high,tot) min.Et-cut to be "em"(mev tempor)
  ECREFFKEY.thresh[8]=0.;     // (17) 
  ECREFFKEY.thresh[9]=0.;     // (18) 
// Run-time RECO-thresholds(time dependent):
  ECREFFKEY.cuts[0]=5.;   // (19) mev/cell thresh. to create cluster(~2adc) 
  ECREFFKEY.cuts[1]=0.;   // (20)
  ECREFFKEY.cuts[2]=0.;   // (21)
  ECREFFKEY.cuts[3]=0.;// (22)
  ECREFFKEY.cuts[4]=0.;// (23)
//
  ECREFFKEY.ReadConstFiles=1;//(24)read const. from DB/RawFiles (0/1)
//  
  ECREFFKEY.sec[0]=0;//(25) 
  ECREFFKEY.sec[1]=0;//(26)
  ECREFFKEY.min[0]=0;//(27)
  ECREFFKEY.min[1]=0;//(28)
  ECREFFKEY.hour[0]=0;//(29)
  ECREFFKEY.hour[1]=0;//(30)
  ECREFFKEY.day[0]=1;//(31)
  ECREFFKEY.day[1]=1;//(32)
  ECREFFKEY.mon[0]=0;//(33)
  ECREFFKEY.mon[1]=0;//(34)
  ECREFFKEY.year[0]=101;//(35)
  ECREFFKEY.year[1]=108;//(36)
FFKEY("ECRE",(float*)&ECREFFKEY,sizeof(ECREFFKEY_DEF)/sizeof(integer),"MIXED");
//
// Calibration parameter defaults:
//
  ECCAFFKEY.cfvers=1; // (1) 1-999 -> vers.number for ecalcvlistNNN.dat file
  ECCAFFKEY.cafdir=0; // (2) 0/1-> use official/private directory for calibr.files
FFKEY("ECCA",(float*)&ECCAFFKEY,sizeof(ECCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//===============================================================================


void AMSJob::_sictcdata(){


  CTCGEOMFFKEY.wallth=0.03;    // reflecting wall and separators thickness(cm)
  CTCGEOMFFKEY.agap=0.02;      // typical "air" gaps for aerogel
  CTCGEOMFFKEY.wgap=0.01;      // typical "air" gaps for WLS's
  CTCGEOMFFKEY.agsize[0]=120.; // max aerogel x-size
  CTCGEOMFFKEY.agsize[1]=90.;  // max aerogel y-size
  CTCGEOMFFKEY.agsize[2]=10.;  // max aerogel z-size
  //  CTCGEOMFFKEY.wlsth=0.3;      // WLS thickness
  //  CTCGEOMFFKEY.hcsize[0]=165.; // supp. honeycomb plate X-size
  //  CTCGEOMFFKEY.hcsize[1]=165.; // supp. honeycomb plate Y-size
  //  CTCGEOMFFKEY.hcsize[2]=5.;   // supp. honeycomb plate Z-size
  //  CTCGEOMFFKEY.supzc=-84.;     // supp. honeycomb plate front face Z-pos
  //  CTCGEOMFFKEY.aegden=0.24;    // aerogel density
  //  CTCGEOMFFKEY.wlsden=1.03;    // WLS density
  //  CTCGEOMFFKEY.nblk=12;        // number of aer. blocks (X-div.)(=1 for solid)
  //  CTCGEOMFFKEY.nwls=12;        // number of wls blocks
  CTCGEOMFFKEY.wlsth=0.3;       // WLS thickness
  CTCGEOMFFKEY.hcsize[0]=165.;  // supp. honeycomb plate X-size
  CTCGEOMFFKEY.hcsize[1]=165.;  // supp. honeycomb plate Y-size
  CTCGEOMFFKEY.hcsize[2]=5.;    // supp. honeycomb plate Z-size
  CTCGEOMFFKEY.supzc=-91.2-9.4-1.9625+6.5125+0.4;     // supp. honeycomb plate (center) Z-pos
  CTCGEOMFFKEY.aegden=0.125;    // aerogel density
  CTCGEOMFFKEY.wlsden=1.03;     // WLS density
  CTCGEOMFFKEY.nblk=1;          // number of aer. blocks (X-div.)(=1 for solid)
  CTCGEOMFFKEY.nwls=1;          // number of wls blocks
  CTCGEOMFFKEY.ptfx=0.0125;      // PTFE thickness in cm
  CTCGEOMFFKEY.thcsize[0]=150.; //
  CTCGEOMFFKEY.thcsize[1]=110.; // Support structure dimensions (cm)
  CTCGEOMFFKEY.thcsize[2]=6.;   //
  CTCGEOMFFKEY.upper[0]=120.5;  //
  CTCGEOMFFKEY.upper[1]=96.;   // Upper/Lower layer dimensions (cm)
  CTCGEOMFFKEY.upper[2]=15.50+3.9;  //
  CTCGEOMFFKEY.wall[0]=0.12;     //
  CTCGEOMFFKEY.wall[1]=96.;    // Wall dimensions (cm)
  CTCGEOMFFKEY.wall[2]=7.175+1+0.8;   //
  CTCGEOMFFKEY.cell[0]=23.4+0.6;    //
  CTCGEOMFFKEY.cell[1]=23.4+0.6;    // Super cell dimensions (cm)
  CTCGEOMFFKEY.cell[2]=15.50+2;   //
  CTCGEOMFFKEY.ygap[0]=23.4+0.6;    // 
  CTCGEOMFFKEY.ygap[1]=0.0;     // Gap dimensions (cm)
  CTCGEOMFFKEY.ygap[2]=15.50+2;   //
  CTCGEOMFFKEY.agel[0]=11.60;   //
  CTCGEOMFFKEY.agel[1]=11.60;   // AeroGEL dimensions (cm)
  CTCGEOMFFKEY.agel[2]=7.025+1;   //
  CTCGEOMFFKEY.pmt[0]=3.0;      //
  CTCGEOMFFKEY.pmt[1]=3.0;      // Phototube dimensions (including housing
  CTCGEOMFFKEY.pmt[2]=7.5;      //          (and shielding)
  CTCGEOMFFKEY.ptfe[0]=11.75+0.25;   //
  CTCGEOMFFKEY.ptfe[1]=11.75+0.25;   // PTFE box dimensions (cm)
  CTCGEOMFFKEY.ptfe[2]=7.175+1+0.8;   //
  CTCGEOMFFKEY.xdiv=5;
  CTCGEOMFFKEY.ydiv=4;
//
  CTCMCFFKEY.Refraction[0]=1.036;   // Refraction indexes
  CTCMCFFKEY.Refraction[1]=1.4;     //ptfe
  CTCMCFFKEY.Refraction[2]=1.458;    // pmt

  CTCMCFFKEY.Path2PhEl[0]=6.;   // Path to photoelectrons conv fact ( ~ 3.3 p.e.)
  CTCMCFFKEY.Path2PhEl[1]=50;
  CTCMCFFKEY.Path2PhEl[2]=100;
  CTCMCFFKEY.AbsLength[0]=15;   // Abs Length in cm  
  CTCMCFFKEY.AbsLength[1]=100;
  CTCMCFFKEY.AbsLength[2]=100;
  CTCMCFFKEY.Edep2Phel[0]=0;      // Agel is not a scint
  CTCMCFFKEY.Edep2Phel[1]=0; 
  CTCMCFFKEY.Edep2Phel[2]=0;     
  CTCMCFFKEY.mcprtf=0;//print MC-hist if !=0

FFKEY("CTCGEOM",(float*)&CTCGEOMFFKEY,sizeof(CTCGEOMFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CTCMC",(float*)&CTCMCFFKEY,sizeof(CTCMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//================================================================================
void AMSJob::_siantidata(){
  ANTIGEOMFFKEY.scradi=54.4;   // internal radious of ANTI sc. cylinder (cm)
  ANTIGEOMFFKEY.scinth=1.;     // thickness of scintillator (cm)
  ANTIGEOMFFKEY.scleng=83.;    //(ams01) scintillator paddle length (glob. Z-dim)
  ANTIGEOMFFKEY.wrapth=0.04;   // wrapper thickness (cm)
  ANTIGEOMFFKEY.groovr=0.45;   // groove radious (bump_rad = groove_rad-pdlgap)
  ANTIGEOMFFKEY.pdlgap=0.1;   // inter paddle gap (cm)(2*wrapth+extra)
  ANTIGEOMFFKEY.stradi=54.235; // inner radious of supp. tube
  ANTIGEOMFFKEY.stleng=83.;    // (ams01)length of supp. tube
  ANTIGEOMFFKEY.stthic=0.12;   // thickness of supp. tube
  
//---
  ANTIMCFFKEY.mcprtf=0;     // print_hist flag (0/1->no/yes)
  ANTIMCFFKEY.SigmaPed=1;   // ped.distribution width (p.e)
  ANTIMCFFKEY.MeV2PhEl=20.; // Mev->Ph.el. MC conv.nfactor (pe/Mev)(= 2side_signal(pe)/Eloss(mev)
//                                                                  measured at center)
  ANTIMCFFKEY.LZero=120;    // attenuation length for one-side signal (cm)
  ANTIMCFFKEY.PMulZPos=41.5;  // PM-position(=Ltot/2)
  ANTIMCFFKEY.LSpeed=17.;   // Eff. light speed in anti-paddle (cm/ns)
//---
  FFKEY("ANGE",(float*)&ANTIGEOMFFKEY,sizeof(ANTIGEOMFFKEY_DEF)/sizeof(integer),
  "MIXED");
  FFKEY("ANMC",(float*)&ANTIMCFFKEY,sizeof(ANTIMCFFKEY_DEF)/sizeof(integer),
  "MIXED");
}
//
//--------------
//
void AMSJob::_sianti2data(){
  ATGEFFKEY.scradi=54.4;   // internal radious of ANTI sc. cylinder (cm)
  ATGEFFKEY.scinth=1.;     // thickness of scintillator (cm)
  ATGEFFKEY.scleng=86.;    //(ams02) scintillator paddle length (glob. Z-dim)
  ATGEFFKEY.wrapth=0.04;   // wrapper thickness (cm)
  ATGEFFKEY.groovr=0.45;   // groove radious (bump_rad = groove_rad-pdlgap)
  ATGEFFKEY.pdlgap=0.1;   // inter paddle gap (cm)(2*wrapth+extra)
  ATGEFFKEY.stradi=54.235; // inner radious of supp. tube
  ATGEFFKEY.stleng=86.;    // (ams02)length of supp. tube
  ATGEFFKEY.stthic=0.12;   // thickness of supp. tube
  
//---
  ATMCFFKEY.mcprtf=0;     // print_hist flag (0/1->no/yes)
  ATMCFFKEY.SigmaPed=1;   // ped.distribution width (p.e)
  ATMCFFKEY.MeV2PhEl=20.; // Mev->Ph.el. MC conv.nfactor (pe/Mev)(= 2side_signal(pe)/Eloss(mev)
//                                                                  measured at center)
  ATMCFFKEY.LZero=120;    // attenuation length for one-side signal (cm)
  ATMCFFKEY.PMulZPos=43.;  // PM-position(=Ltot/2)-is taken later from geometry in siantiinitjob
  ATMCFFKEY.LSpeed=17.;   // Eff. light speed in anti-paddle (cm/ns)
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
FFKEY("TRDMC",(float*)&TRDMCFFKEY,sizeof(TRDMCFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_sisrddata(){
}

void AMSJob:: _reamsdata(){
_remfdata();
_retkdata();

_retofdata();
  _retof2data();
  
_reantidata();
  _reanti2data();
  
_reecaldata();
_retrddata();
_resrddata();
_rectcdata();
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
TRFITFFKEY.UseTOF=2;
TRFITFFKEY.Chi2FastFit=2000;
TRFITFFKEY.Chi2StrLine=9;
TRFITFFKEY.Chi2Circle=4;
TRFITFFKEY.ResCutFastFit=0.5;
TRFITFFKEY.ResCutStrLine=0.5;
TRFITFFKEY.ResCutCircle=0.5;
TRFITFFKEY.SearchRegFastFit=1;
TRFITFFKEY.SearchRegStrLine=0.5;
TRFITFFKEY.SearchRegCircle=10.;
TRFITFFKEY.RidgidityMin=0.02;
TRFITFFKEY.FullReco=0;
TRFITFFKEY.MinRefitCos[0]=0.7;
TRFITFFKEY.MinRefitCos[1]=0.5;
TRFITFFKEY.FastTracking=0;
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
TRFITFFKEY.MaxTrRecHitsPerLayer=50;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}
//=================================================================================
void AMSJob::_retofdata(){
// 
  TOFRECFFKEY.Thr1=0.45;//(1) Threshold (mev) on peak bar energy
  TOFRECFFKEY.ThrS=0.45; //(2) Threshold (mev) on total cluster energy
//
  TOFRECFFKEY.reprtf[0]=0; //(3) RECO print flag for statistics 
  TOFRECFFKEY.reprtf[1]=0; //(4) print flag for DAQ (1/2-> print for decoding/dec+encoding)
  TOFRECFFKEY.reprtf[2]=0; //(5) print flag for histograms
  TOFRECFFKEY.reprtf[3]=0; //(6) print flag for TDC-hit multiplicity histograms 
  TOFRECFFKEY.reprtf[4]=0; //(7) if non-zero (LBBS) print stretcher T1/T2 for LBBS
//
  TOFRECFFKEY.relogic[0]=0;//(8) 0/1/2/3/4 ->normal/STRR+AVSD-/TDIF-/TZSL-/AMPL-calibr. run. 
  TOFRECFFKEY.relogic[1]=0;//(9) 0/1/2-> full_fTDC_use/no_time_matching/not_use 
  TOFRECFFKEY.relogic[2]=0;//(10) 0/1-> force L4S2 suppression(useful for MC processing)
  TOFRECFFKEY.relogic[3]=0;//(11) 0/1-> use new/old parametrization for TOF integrators 
  TOFRECFFKEY.relogic[4]=0;//(12) spare RECO logic flag
//
  TOFRECFFKEY.daqthr[0]=35.;//(13)Fast discr. thresh(mV) for fast/slow_TDC 
  TOFRECFFKEY.daqthr[1]=150.;//(14)Fast discr. thresh(mV) for FT-trigger (z>=1)  
  TOFRECFFKEY.daqthr[2]=150.;//(15)thresh(mV) for discr. of "z>1"-trig  
  TOFRECFFKEY.daqthr[3]=0.;//(16)spare  
  TOFRECFFKEY.daqthr[4]=0.;//(17)spare
//
  TOFRECFFKEY.cuts[0]=40.;//(18)t-window(ns) for "the same hit" search in f/s_tdc
  TOFRECFFKEY.cuts[1]=15.;//(19)"befor"-cut in time history (ns)(max.PMT-pulse length?)
  TOFRECFFKEY.cuts[2]=400.;//(20)"after"-cut in time history (ns)(max. shaper integr.time?)
  TOFRECFFKEY.cuts[3]=2.;//(21) error in longitudinal coordinate (single TOF bar)
  TOFRECFFKEY.cuts[4]=65.;//(22) FT const. delay
  TOFRECFFKEY.cuts[5]=40.;//(23) sTDC-delay wrt fTDC
  TOFRECFFKEY.cuts[6]=35.;//(24) sTDC-delay wrt aTDC
  TOFRECFFKEY.cuts[7]=0.;// (25) sTDC-delay wrt dTDC 
  TOFRECFFKEY.cuts[8]=40.;//(26) gate for a/s-TDC
  TOFRECFFKEY.cuts[9]=40.;//(27) gate for d/s-TDC
//
  TOFRECFFKEY.ReadConstFiles=0;//(28)read const. from DB/myFiles (0/1)
//  
  TOFRECFFKEY.sec[0]=0;//(29) 
  TOFRECFFKEY.sec[1]=0;
  TOFRECFFKEY.min[0]=0;
  TOFRECFFKEY.min[1]=0;
  TOFRECFFKEY.hour[0]=0;
  TOFRECFFKEY.hour[1]=0;
  TOFRECFFKEY.day[0]=1;
  TOFRECFFKEY.day[1]=1;
  TOFRECFFKEY.mon[0]=0;
  TOFRECFFKEY.mon[1]=0;
  TOFRECFFKEY.year[0]=96;
  TOFRECFFKEY.year[1]=101;
  FFKEY("TOFREC",(float*)&TOFRECFFKEY,sizeof(TOFRECFFKEY_DEF)/sizeof(integer),
  "MIXED");

//    defaults for calibration:
// TZSL-calibration:
  TOFCAFFKEY.pcut[0]=8.;// (1)track mom. low limit (gev/c) (prot, put 0.75 for mu)
  TOFCAFFKEY.pcut[1]=50.;// (2)track mom. high limit
  TOFCAFFKEY.bmeanpr=0.996;// (3)mean prot. velocity in the above range
  TOFCAFFKEY.tzref[0]=0.;// (4)T0 for ref. counters
  TOFCAFFKEY.tzref[1]=0.;// (5)T0 for ref. counters
  TOFCAFFKEY.fixsl=8.02;// (6)def. slope
  TOFCAFFKEY.bmeanmu=0.997;// (7)mean muon velocity in the above range
  TOFCAFFKEY.idref[0]=108;//(8)LBB for first ref. counter 
  TOFCAFFKEY.idref[1]=0;//(9)LBB for second ref. counter (if nonzero)
  TOFCAFFKEY.ifsl=1;//(10) 0/1 to fix/release slope param.
//
  TOFCAFFKEY.caltyp=0;// (11) 0/1->space/earth calibration
//
// AMPL-calibration:
  TOFCAFFKEY.truse=1; // (12) 1/-1-> to use/not tracker
  TOFCAFFKEY.plhc[0]=0.;// (13) track mom. low limit(gev/c) for space calibr
  TOFCAFFKEY.plhc[1]=500.;// (14) track mom. high limit(gev/c) ..............
  TOFCAFFKEY.minev=50;// (15)min.events needed for measurement in channel or bin
  TOFCAFFKEY.trcut=0.75;// (16) cut to use for "truncated average" calculation
  TOFCAFFKEY.spares[0]=0;//(17) spares
  TOFCAFFKEY.spares[1]=0; 
  TOFCAFFKEY.spares[2]=0; 
  TOFCAFFKEY.spares[3]=0; 
  TOFCAFFKEY.spares[4]=0;//(21)
  TOFCAFFKEY.plhec[0]=0.8;//(22)plow-cut for earth calibration
  TOFCAFFKEY.plhec[1]=20.;  //(23)phigh-cut ...................
  TOFCAFFKEY.bgcut[0]=2.; //(24) beta*gamma low-cut to be in mip-region(abs.calib)
  TOFCAFFKEY.bgcut[1]=50.;//(25) beta*gamma high-cut ...
//
  TOFCAFFKEY.tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  TOFCAFFKEY.dynflg=1; // (27) 0/1-> use stand/special(Contin's) dynode-calibration
  TOFCAFFKEY.cfvers=1; // (28) 1-99 -> vers.number for tofverlistNN.dat file
  TOFCAFFKEY.cafdir=0;// (29) 0/1-> use official/private directory for calibr.files
  TOFCAFFKEY.mcainc=0;// (30) 0/1-> off/on A-integr. calibration in MC
  TOFCAFFKEY.tofbetac=0.6;// (31) if nonzero->low beta cut (own TOF measurements !!!)
  FFKEY("TOFCA",(float*)&TOFCAFFKEY,sizeof(TOFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//
//-------------
//
void AMSJob::_retof2data(){
// 
  TFREFFKEY.Thr1=0.4;//(1) Threshold (mev) on peak bar energy
  TFREFFKEY.ThrS=0.5; //(2) Threshold (mev) on total cluster energy
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
  TFREFFKEY.relogic[3]=0;//(11) 0/1-> use new/old parametrization for TOF integrators 
  TFREFFKEY.relogic[4]=0;//(12) spare RECO logic flag
//
  TFREFFKEY.daqthr[0]=30.;//(13)Fast discr. thresh(mV) for fast/slow_TDC 
  TFREFFKEY.daqthr[1]=100.;//(14)Fast discr. thresh(mV) for FT-trigger (z>=1)  
  TFREFFKEY.daqthr[2]=150.;//(15)thresh(mV) for discr. of "z>1"-trig  
  TFREFFKEY.daqthr[3]=1.;//(16)H(L)-ADC-readout threshold in DAQ (in ADC-channels)    
  TFREFFKEY.daqthr[4]=0.;//(17)spare
//
  TFREFFKEY.cuts[0]=40.;//(18)t-window(ns) for "the same hit" search in f/s_tdc
  TFREFFKEY.cuts[1]=15.;//(19)"befor"-cut in time history (ns)(max.PMT-pulse length?)
  TFREFFKEY.cuts[2]=400.;//(20)"after"-cut in time history (ns)(max. shaper integr.time?)
  TFREFFKEY.cuts[3]=2.; //(21) error in longitudinal coordinate (single TOF bar)
  TFREFFKEY.cuts[4]=65.;//(22) FT const. delay
  TFREFFKEY.cuts[5]=40.;//(23) sTDC-delay wrt fTDC
  TFREFFKEY.cuts[6]=0.36;//(24) assim.cut in TOFCluster energy calculation
  TFREFFKEY.cuts[7]=8.;// (25) internal longit.coo matching cut ............
  TFREFFKEY.cuts[8]=0.;// (26) spare
  TFREFFKEY.cuts[9]=0.;// (27) 
//
  TFREFFKEY.ReadConstFiles=1;//(28)read const. from DB/RawFiles (0/1)
//  
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
  TFREFFKEY.year[1]=108;
  FFKEY("TFRE",(float*)&TFREFFKEY,sizeof(TFREFFKEY_DEF)/sizeof(integer),
  "MIXED");

//    defaults for calibration:
// TZSL-calibration:
  TFCAFFKEY.pcut[0]=8.;// (1)track mom. low limit (gev/c) (prot, put 0.75 for mu)
  TFCAFFKEY.pcut[1]=50.;// (2)track mom. high limit
  TFCAFFKEY.bmeanpr=0.996;// (3)mean prot. velocity in the above range
  TFCAFFKEY.tzref[0]=0.;// (4)T0 for ref. counters
  TFCAFFKEY.tzref[1]=0.;// (5)T0 for ref. counters
  TFCAFFKEY.fixsl=5.;// (6)def. slope
  TFCAFFKEY.bmeanmu=0.997;// (7)mean muon velocity in the above range
  TFCAFFKEY.idref[0]=106;//(8)LBB for first ref. counter 
  TFCAFFKEY.idref[1]=0;//(9)LBB for second ref. counter (if nonzero)
  TFCAFFKEY.ifsl=1;//(10) 0/1 to fix/release slope param.
//
  TFCAFFKEY.caltyp=0;// (11) 0/1->space/earth calibration
//
// AMPL-calibration:
  TFCAFFKEY.truse=1; // (12) 1/-1-> to use/not tracker
  TFCAFFKEY.plhc[0]=0.8;// (13) track mom. low limit(gev/c) for space calibr
  TFCAFFKEY.plhc[1]=500.;// (14) track mom. high limit(gev/c) ..............
  TFCAFFKEY.minev=50;// (15)min.events needed for measurement in channel or bin
  TFCAFFKEY.trcut=0.75;// (16) cut to use for "truncated average" calculation
  TFCAFFKEY.spares[0]=0;//spare integers
  TFCAFFKEY.spares[1]=0;//spare integers
  TFCAFFKEY.spares[2]=0;//spare integers
  TFCAFFKEY.spares[3]=0;//spare integers
  TFCAFFKEY.spares[4]=0;//spare integers
  TFCAFFKEY.plhec[0]=0.8;//(22)plow-cut for earth calibration
  TFCAFFKEY.plhec[1]=20.;  //(23)phigh-cut ...................
  TFCAFFKEY.bgcut[0]=2.; //(24) beta*gamma low-cut to be in mip-region(abs.calib)
  TFCAFFKEY.bgcut[1]=50.;//(25) beta*gamma high-cut ...
//
  TFCAFFKEY.tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  TFCAFFKEY.dynflg=0; // (27)  not used now
  TFCAFFKEY.cfvers=1; // (28) 1-999 -> vers.number for tofverlistNN.dat file
  TFCAFFKEY.cafdir=0;// (29) 0/1-> use official/private directory for calibr.files
  TFCAFFKEY.mcainc=0;// (30) not used now
  TFCAFFKEY.tofbetac=0.6;// (31) if nonzero->low beta cut (own TOF measurements !!!)
  FFKEY("TFCA",(float*)&TFCAFFKEY,sizeof(TFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//======================================================================
void AMSJob::_reantidata(){
  ANTIRECFFKEY.reprtf[0]=0;//(1) Reco print_hist flag (0/1->no/yes)
  ANTIRECFFKEY.reprtf[1]=0;//(2) DAQ-print (1/2->print for decoding/decoding+encoding)
  ANTIRECFFKEY.reprtf[2]=0;//(3)spare
  ANTIRECFFKEY.ThrS=6;//(4) threshold to create Cluster object (p.e.)
  ANTIRECFFKEY.PhEl2MeV=0.05;//(5) reco conv. Phel->Mev(Mev/pe)( =Elos/2sides_signal measured at center)
  ANTIRECFFKEY.dtthr=3.; //(6) trig.discr.theshold (same for all sides now) (p.e.'s for now)
  ANTIRECFFKEY.dathr=6.; //(7) Amplitude(charge) discr.threshold(...) (p.e.)
  ANTIRECFFKEY.ftwin=100.;//(8) t-window(ns) for true TDCA-hit search wrt TDCT-hit(FT) 
//
  ANTIRECFFKEY.ReadConstFiles=0;//(9)read const. from DB/myFiles (0/1)
//  
  ANTIRECFFKEY.sec[0]=0;//(10) 
  ANTIRECFFKEY.sec[1]=0;//(11)
  ANTIRECFFKEY.min[0]=0;//(12)
  ANTIRECFFKEY.min[1]=0;//(13)
  ANTIRECFFKEY.hour[0]=0;//(14)
  ANTIRECFFKEY.hour[1]=0;//(15)
  ANTIRECFFKEY.day[0]=1;
  ANTIRECFFKEY.day[1]=1;
  ANTIRECFFKEY.mon[0]=0;
  ANTIRECFFKEY.mon[1]=0;
  ANTIRECFFKEY.year[0]=96;
  ANTIRECFFKEY.year[1]=101;
  FFKEY("ANRE",(float*)&ANTIRECFFKEY,sizeof(ANTIRECFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  ANTICAFFKEY.cfvers=2; // (01-99) vers.number NN for antiverlistNN.dat file
  FFKEY("ANCA",(float*)&ANTICAFFKEY,sizeof(ANTICAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//----------------------------
void AMSJob::_reanti2data(){
  ATREFFKEY.reprtf[0]=0;//(1) Reco print_hist flag (0/1->no/yes)
  ATREFFKEY.reprtf[1]=0;//(2) DAQ-print (1/2->print for decoding/decoding+encoding)
  ATREFFKEY.reprtf[2]=0;//(3)spare
  ATREFFKEY.ThrS=6;//(4) threshold to create Cluster object (p.e.)
  ATREFFKEY.PhEl2MeV=0.05;//(5) reco conv. Phel->Mev(Mev/pe)( =Elos/2sides_signal measured at center)
  ATREFFKEY.dtthr=3.; //(6) trig.discr.theshold (same for all sides now) (p.e.'s for now)
  ATREFFKEY.dathr=6.; //(7) Amplitude(charge) discr.threshold(...) (p.e.)
  ATREFFKEY.ftwin=100.;//(8) t-window(ns) for true TDCA-hit search wrt TDCT-hit(FT) 
//
  ATREFFKEY.ReadConstFiles=1;//(9)read const. from DB/myFiles (0/1)
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
  ATREFFKEY.year[0]=101;
  ATREFFKEY.year[1]=108;
  FFKEY("ATRE",(float*)&ATREFFKEY,sizeof(ATREFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  ATCAFFKEY.cfvers=2; // (01-99) vers.number NN for antiverlistNN.dat file
  FFKEY("ATCA",(float*)&ATCAFFKEY,sizeof(ATCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_rectcdata(){
  CTCRECFFKEY.Thr1=0.25/2; //(1)
  CTCRECFFKEY.ThrS=0.5/2;  //(2)
  CTCRECFFKEY.reprtf[0]=0;//(3) Reco print_hist flag (0/1->no/yes)
  CTCRECFFKEY.reprtf[1]=0;//(4) DAQ-print (1/2->print for decoding/decoding+encoding)
  CTCRECFFKEY.reprtf[2]=0;//(5) spare
  CTCRECFFKEY.ftwin=100.;//(6) t-window(ns) for true TDCA-hit search wrt TDCT-hit(FT)
  CTCRECFFKEY.q2pe=0.327;  //(7)
  CTCRECFFKEY.ft2edg=1;  //(8) 0/1-> 1/2 edges readout for FT signal
//
  CTCRECFFKEY.ReadConstFiles=0;//(9)read const. from DB/myFiles (0/1)
//  
  CTCRECFFKEY.sec[0]=0; //(10)
  CTCRECFFKEY.sec[1]=0; //(11)
  CTCRECFFKEY.min[0]=0; //(12)
  CTCRECFFKEY.min[1]=0; //(13)
  CTCRECFFKEY.hour[0]=0; //(14)
  CTCRECFFKEY.hour[1]=0; //(15)
  CTCRECFFKEY.day[0]=1;
  CTCRECFFKEY.day[1]=1;
  CTCRECFFKEY.mon[0]=0;
  CTCRECFFKEY.mon[1]=0;
  CTCRECFFKEY.year[0]=96;
  CTCRECFFKEY.year[1]=105;
  FFKEY("CTCREC",(float*)&CTCRECFFKEY,sizeof(CTCRECFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  CTCCAFFKEY.cfvers=2; // (01-99) vers.number NN for ctcverlistNN.dat file
  FFKEY("CTCCA",(float*)&CTCCAFFKEY,sizeof(CTCCAFFKEY_DEF)/sizeof(integer),"MIXED");
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
BETAFITFFKEY.Chi2=15;
BETAFITFFKEY.Chi2S=10;
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



void AMSJob::udata(){
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
   if(strstr(getsetup(),"AMS02"))DAQCFFKEY.SCrateinDAQ=0;
   else DAQCFFKEY.SCrateinDAQ=1; 
  }
  if(strstr(getsetup(),"AMS02")){
   if(CCFFKEY.enddate%10000 < 2000){
     CCFFKEY.enddate+=5;
     CCFFKEY.begindate+=5;
     TRMCFFKEY.year[0]+=5;
     TRMCFFKEY.year[1]+=5;
   }
  }

  TKDBc::init();
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
integer or=0;
for (i=0;i<len;i++){
  if(triggername[i]=='|' || triggername[i]=='\0' || triggername[i]=='&'){
    // new trigger found
       if(triggername[i]=='|')or=1;
       triggername[i]='\0';
       if(i-nold>0)settrigger(triggername+nold,ntrig++,or);
       nold=i+1;
  }
}
}
setjobtype(AMSFFKEY.Jobtype%10 != 0);
setjobtype(((AMSFFKEY.Jobtype/10)%10 != 0)<<(RealData-1));
uinteger ical=(AMSFFKEY.Jobtype/100)%10;
uinteger ucal=1;
if(ical)setjobtype(ucal<<(ical+1));
uinteger imon=(AMSFFKEY.Jobtype/1000)%10;
uinteger umon=1;
if(imon)setjobtype(umon<<(imon+1+8));
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
 integer or=0;
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
    if(strstr(getsetup(),"AMSSHUTTLE") ){    
       AMSTrIdSoft::inittable(1);
    }
    else if(strstr(getsetup(),"AMS02") ){    
       AMSTrIdSoft::inittable(2);
    }
    else {
      cerr<<"AMSJob::_retkinitjob-E-NoAMSTrIdSoftTable exists for setup "<<
        getsetup()<< "yet "<<endl;
        exit(1);
    }
       AMSTrIdSoft::init();


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

// check delta etc
if(TRMCFFKEY.gammaA[0]<0){
 if(strstr(getsetup(),"AMSSHUTTLE")){
  TRMCFFKEY.gammaA[0]=-TRMCFFKEY.gammaA[0];
 }
 else TRMCFFKEY.gammaA[0]=0.2;
}
if(TRMCFFKEY.delta[0]<0){
 if(strstr(getsetup(),"AMSSHUTTLE")){
  TRMCFFKEY.delta[0]=-TRMCFFKEY.delta[0];
 }
 else TRMCFFKEY.delta[0]=0.9;
}
if(TRMCFFKEY.thr1R[0]<0){
 if(strstr(getsetup(),"AMSSHUTTLE")){
  TRMCFFKEY.thr1R[0]=-TRMCFFKEY.thr1R[0];
 }
 else TRMCFFKEY.thr1R[0]=3;
}

if(TRCLFFKEY.Thr1R[0]<0){
 if(strstr(getsetup(),"AMSSHUTTLE")){
  TRCLFFKEY.Thr1R[0]=-TRCLFFKEY.Thr1R[0];
 }
 else TRCLFFKEY.Thr1R[0]=3;
}

    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    _hextname=hfile;


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
  if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    _sitof2initjob();
    _sianti2initjob();
    _siecalinitjob();
    _sirichinitjob();
    _sitrdinitjob();
    _sisrdinitjob();
    _sitrig2initjob();
  }
  else{ 
   _sitofinitjob();
   _siantiinitjob();
   _sictcinitjob();
   _sitriginitjob();
  }
  
}


void AMSJob::_dbinitjob(){
  AMSgObj::BookTimer.book("EventStatus");
  AMSStatus::init();
}

//-------------------------------------------------------------------------------------------
void AMSJob::_sitriginitjob(){
  if(LVL1FFKEY.RebuildLVL1)cout <<"AMSJob::_sitriginitjob-W-TriggerLvl1 Will be rebuild from TOF data; Original Trigger info will be lost"<<endl;
  if(LVL3FFKEY.RebuildLVL3==1)cout <<"AMSJob::_sitriginitjob-W-TriggerLvl3 will be rebuild from TOF/Tracker data; Original Trigger info will be lost"<<endl;
  else if(LVL3FFKEY.RebuildLVL3)cout <<"AMSJob::_sitriginitjob-W-TriggerLvl3 will be rebuild from TOF/Trigger data Original Trigger info will be kept"<<endl;
  AMSgObj::BookTimer.book("LVL3");
  TriggerLVL3::init();  
}
//-----------
void AMSJob::_sitrig2initjob(){
  if(TGL1FFKEY.RebuildLVL1)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl1 Will be rebuild from TOF data; Original Trigger info will be lost"<<endl;
  if(LVL3FFKEY.RebuildLVL3==1)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl3 will be rebuild from TOF/Tracker data; Original Trigger info will be lost"<<endl;
  else if(LVL3FFKEY.RebuildLVL3)cout <<"AMSJob::_sitrig2initjob-W-TriggerLvl3 will be rebuild from TOF/Trigger data Original Trigger info will be kept"<<endl;
  AMSgObj::BookTimer.book("LVL3");
  TriggerLVL3::init();  
}
//-------------------------------------------------------------------------------------------
void AMSJob::_sitkinitjob(){
  if(TRMCFFKEY.GenerateConst){
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
             id.setsig()=TRMCFFKEY.sigma[l]*(0.9+0.2*RNDM(d));
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
void AMSJob::_sitofinitjob(){
  cout <<"_sitofinit-I-Slow(Accurate) simulation algorithm selected."<<endl;
//
  if(TOFMCFFKEY.daqfmt==0)cout<<"_sitofinit-I-Raw TDC_Format selected"<<endl;
  else if(TOFMCFFKEY.daqfmt==1)cout<<"_sitofinit-I-Reduced TDC_Format selected"<<endl;
  else {
    cout<<"_sitofinit-I- Unknown TDC_Format was requested !!!"<<endl;
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
//
// ===> Book histograms for MC :
//
   TOFJobStat::bookhistmc();
}
//--------------
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
//
// ===> Book histograms for MC :
//
   TOF2JobStat::bookhistmc();
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
}

void AMSJob::_sirichinitjob(){
  AMSgObj::BookTimer.book("SIRICH");
  RICHDB::bookhist();
}

void AMSJob::_rerichinitjob(){
  AMSgObj::BookTimer.book("RERICH");
}

//-----------------------------------------------------------------------
void AMSJob::_siantiinitjob(){
  AMSgObj::BookTimer.book("SIANTIEVENT");
  if(ANTIMCFFKEY.mcprtf){
    HBOOK1(2000,"ANTI_counters total energy (geant,Mev)",60,0.,30.,0.); 
  }
}
//-------------
void AMSJob::_sianti2initjob(){
  AMSgObj::BookTimer.book("SIANTIEVENT");
  if(ATMCFFKEY.mcprtf){
    HBOOK1(2000,"ANTI_counters total energy (geant,Mev)",60,0.,30.,0.); 
  }
}
//-----------------------------------------------------------------------
void AMSJob::_sictcinitjob(){
     AMSgObj::BookTimer.book("SICTCEVENT");
     if(CTCMCFFKEY.mcprtf>0){
       HBOOK1(3500,"CTC total MC raw signal(p.e.)",80,0.,40.,0.);
     }
}

void AMSJob::_sitrdinitjob(){

 AMSTRDMCCluster::init();
}
void AMSJob::_sisrdinitjob(){
}


void AMSJob::_reamsinitjob(){
AMSgObj::BookTimer.book("WriteEvent");
AMSgObj::BookTimer.book("REAMSEVENT"); 

_remfinitjob();

if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE"))_redaqinitjob();
else if(strstr(AMSJob::gethead()->getsetup(),"AMS02"))_redaq2initjob();
else {
      cerr <<"AMSJob::_redaqinitjob-E-NoYetDAQFormat for "<<
      AMSJob::gethead()->getsetup()<<endl;
}

_retkinitjob();
if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
  _retof2initjob();
  _reanti2initjob();
  _reecalinitjob();
  _retrdinitjob();
  _rerichinitjob();
  _resrdinitjob();
}
else{
  _retofinitjob();
  _reantiinitjob();
  _rectcinitjob();
}
_reaxinitjob();

AMSUser::InitJob();
}
//-----------------------------------------------------
void AMSJob::_caamsinitjob(){
if(isCalibration() & CTracker)_catkinitjob();
if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
  if(isCalibration() & CTOF)_catof2initjob();
  if(isCalibration() & CAnti)_cant2initjob();
}
else{
  if(isCalibration() & CTOF)_catofinitjob();
  if(isCalibration() & CAnti)_cantinitjob();
}
if(isCalibration() & CTRD)_catrdinitjob();
if(isCalibration() & CSRD)_casrdinitjob();
if(isCalibration() & CCerenkov)_cactcinitjob();
if(isCalibration() & CAMS)_caaxinitjob();
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
void AMSJob::_catofinitjob(){
 if(TOFRECFFKEY.relogic[0]==1){
   TOFSTRRcalib::init();// TOF STRR-calibr.
   cout<<"TOFSTRRcalib-init done !!!"<<'\n';
 }
 if(TOFRECFFKEY.relogic[0]==2){
   TOFTDIFcalib::init();// TOF TDIF-calibr.
   cout<<"TOFTDIFcalib-init done !!!"<<'\n';
 }
 if(TOFRECFFKEY.relogic[0]==3){
   TOFTZSLcalib::init();// TOF TzSl-calibr.
   cout<<"TOFTZSLcalib-init done !!!"<<'\n';
 }
 if(TOFRECFFKEY.relogic[0]==4){
   TOFAMPLcalib::init();// TOF AMPL-calibr.
   cout<<"TOFAMPLcalib-init done !!!"<<'\n';
   TOFAVSDcalib::init();// TOF AVSD-calibr.
   cout<<"TOFAVSDcalib-init done !!!"<<'\n';
 }
}
//------------------------------------------------
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

void AMSJob::_cactcinitjob(){
}
//==========================================
void AMSJob::_cantinitjob(){
}
void AMSJob::_cant2initjob(){
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
void AMSJob::_retofinitjob(){
//
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:validation");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");

      if(isMonitoring() & (AMSJob::MTOF | AMSJob::MAll)){ // TOF Online histograms
        _retofonlineinitjob();      // (see tofonline.C)
      }
//
// ===> Clear JOB-statistics counters for SIM/REC :
//
    TOFJobStat::clear();
//
// ===> Book histograms for REC :
//
    TOFJobStat::bookhist();
// 
  if(TOFRECFFKEY.ReadConstFiles){// Constants will be taken from ext.files
//
//-----------
// ===> create common parameters (tofvpar structure) fr.data-cards :
//
    TOFVarp::tofvpar.init(TOFRECFFKEY.daqthr, TOFRECFFKEY.cuts);//daqthr/cuts reading
//
//-----------
//     ===> create indiv. sc.bar scmcscan-objects (MC t/eff-distributions) 
//
    AMSTOFScan::build();
//
//-------------------------
// ===> create indiv. sc.bar parameters (sc.bar scbrcal-objects) fr.ext.files:
//
    TOFBrcal::build();
  }
//-------------------------
  else{ // Constants will be taken from DB (TDV)
    TOFRECFFKEY.year[1]=TOFRECFFKEY.year[0]-1;    
  }
// 
//-----------
  AMSTOFCluster::init();
}
//-------------------------------------------------------------------------
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
  if(TFREFFKEY.ReadConstFiles){// Constants will be taken from ext.files
//
//-----------
// ===> create common parameters (tofvpar structure) fr.data-cards :
//
    TOF2Varp::tofvpar.init(TFREFFKEY.daqthr, TFREFFKEY.cuts);//daqthr/cuts reading
//
//-----------
//     ===> create indiv. sc.bar scmcscan-objects (MC t/eff-distributions) 
//
    TOF2Scan::build();
//
//-------------------------
// ===> create indiv. sc.bar parameters (sc.bar scbrcal-objects) fr.ext.files:
//
    TOF2Brcal::build();
//-------------------------
// ===> create sc.bar peds/sigs (sc.bar scbrped-objects) fr.ext.files:
//
    TOFBPeds::build();
  }
//-------------------------
  else{ // Constants will be taken from DB(TDV)->this is normal run mode !!!
    TFREFFKEY.year[1]=TFREFFKEY.year[0]-1;    
  }
// 
//-----------
  AMSTOFCluster::init();
}
//=============================================================================================
void AMSJob::_reantiinitjob(){
//
    AMSgObj::BookTimer.book("REANTIEVENT");
    if(ANTIRECFFKEY.reprtf[0]>0){
      HBOOK1(2500,"ANTI_counters total  energy(reco,Mev)",80,0.,16.,0.);
      HBOOK1(2501,"ANTI_bar: T_tdca-T_tdct(reco,ns)",80,-20.,220.,0.);
    }
//
    ANTIJobStat::clear();// Clear JOB-statistics counters for SIM/REC
//-----------
  if(ANTIRECFFKEY.ReadConstFiles){// Constants will be taken from ext.files
     ANTIPcal::build();//create calibr.objects(antisccal-objects for each sector)
  }
  else{ // Constants will be taken from DB (TDV)
    ANTIRECFFKEY.year[1]=ANTIRECFFKEY.year[0]-1;    
  } 
}
//--------------------------------------------------------------------
void AMSJob::_reanti2initjob(){
//
    AMSgObj::BookTimer.book("REANTIEVENT");
    if(ATREFFKEY.reprtf[0]>0){
      HBOOK1(2500,"ANTI_counters total  energy(reco,Mev)",80,0.,16.,0.);
      HBOOK1(2501,"ANTI_bar: T_tdca-T_tdct(reco,ns)",80,-20.,220.,0.);
    }
//
    ANTI2JobStat::clear();
//-----------
  if(ATREFFKEY.ReadConstFiles){// Constants will be taken from ext.files
     ANTI2Pcal::build();//create calibr.objects(antisccal-objects for each sector)
  }
  else{ // Constants will be taken from DB (TDV)
    ATREFFKEY.year[1]=ATREFFKEY.year[0]-1;    
  } 
}
//============================================================================================
void AMSJob::_reecalinitjob(){
//
  integer pr,pl,cell;
  number ct,cl,cz;
  integer fid,cid[4]={0,0,0,0};
  number w[4]={0.,0.,0.,0.};
//
    AMSgObj::BookTimer.book("REECALEVENT");
//
    ECALDBc::getscinfoa(0,0,0,pr,pl,cell,ct,cl,cz);// <--- init. PMCell-readout tables
//
    EcalJobStat::clear();// Clear JOB-statistics counters for SIM/REC
//
    EcalJobStat::bookhist();// Book histograms for REC
//
  if(ECREFFKEY.ReadConstFiles){// Calibr.constants will be taken from ext.files 
//-----------
// ===> create common parameters (ecalvpar structure) fr.data-cards :
//
    ECALVarp::ecalvpar.init(ECREFFKEY.thresh, ECREFFKEY.cuts);//daqthr/cuts reading
//
//-----------
 // ===> create ecpmcal-calib-objects:
    ECcalib::build(); 
//-------------------------
// ===> create EC-SubCell peds/sigs ECcalib structure):
//
    ECPMPeds::build();
  }
//-----------
  else{ // Constants will be taken from DB(TDV)
    ECREFFKEY.year[1]=ECREFFKEY.year[0]-1;    
  } 
}
//===================================================================
void AMSJob::_rectcinitjob(){
AMSgObj::BookTimer.book("RECTCEVENT");
// AMSCTCRawHit::init();
  if(CTCRECFFKEY.reprtf[0]>0){
    HBOOK1(3000,"CTC total RECO raw signal(p.e.)",80,0.,40.,0.);
    HBOOK1(3001,"CTC_channel: T_tdca-Ttdct (reco,ns)",80,-20.,220.,0.);
  }
// ===> Clear JOB-statistics counters for SIM/REC :
//
    CTCJobStat::clear();
//
// ===> create ctcfcal-objects(TDV) for each CTC cells group (combination) :
//
  if(CTCRECFFKEY.ReadConstFiles){// Constants will be taken from ext.files
    CTCCCcal::build();
  }
  else{ // Constants will be taken from DB (TDV)
    CTCRECFFKEY.year[1]=CTCRECFFKEY.year[0]-1;    
  } 
}

void AMSJob::_reaxinitjob(){
  AMSgObj::BookTimer.book("REAXEVENT");
  AMSgObj::BookTimer.book("ReAxRefit");
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
                    100.,100.,100.,100.,100.,100.,100.,100.,
                    1000.,1000.,
                    800.,800.,
                    800.,800.,
                    100., 100.};
                    
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
void AMSJob::settrigger(char *setup, integer N,integer or){
  assert(N < maxtrig);
  if(setup){
    strcpy(_TriggerC[N],setup);
  }
  _TriggerI=1;
  _TriggerOr=or;
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
server=AMSTimeID::Client;
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
}
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
    (void*)AMSTrIdSoft::peds,server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerPedestals(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::peds[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell),server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmaraws,server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerRawSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell),server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::gains,server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerGains(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::gains[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell),server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::sigmas,server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerSigmas(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell),server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(0),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
    (void*)AMSTrIdSoft::status,server));
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerStatus(1),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::status[0])*
    (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell),server));
 /*
 TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.l",isRealData()),
    begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*AMSTrIdSoft::_numell*2,
    (void*)AMSTrIdSoft::rhomatrix,server));
 TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.r",isRealData()),
    begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*
    (2*AMSTrIdSoft::_numel-2*AMSTrIdSoft::_numell),
    (void*)(AMSTrIdSoft::rhomatrix+2*AMSTrIdSoft::_numell),server));
 */
 TID.add (new AMSTimeID(AMSID(AMSTrIdSoft::TrackerCmnNoise(),isRealData()),
    begin,end,sizeof(AMSTrIdSoft::cmnnoise),
    (void*)AMSTrIdSoft::cmnnoise,server));
//TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
//   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
//   (void*)AMSTrIdSoft::indnoise,server));
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

 if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){

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

  TID.add (new AMSTimeID(AMSID("Tofbarcal2",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOF2Brcal::scbrcal[0][0]),
    (void*)&TOF2Brcal::scbrcal[0][0],server));
   
  TID.add (new AMSTimeID(AMSID("Tofvpar2",isRealData()),
    begin,end,sizeof(TOF2Varp::tofvpar),
    (void*)&TOF2Varp::tofvpar,server));
   
  TID.add (new AMSTimeID(AMSID("Tofmcscans2",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOF2Scan::scmcscan[0]),
    (void*)&TOF2Scan::scmcscan[0],server));
   
  TID.add (new AMSTimeID(AMSID("Tofpeds",isRealData()),
    begin,end,TOF2GC::SCBLMX*sizeof(TOFBPeds::scbrped[0][0]),
    (void*)&TOFBPeds::scbrped[0][0],server));
 }
//
 else{   

  begin.tm_sec=TOFRECFFKEY.sec[0];
  begin.tm_min=TOFRECFFKEY.min[0];
  begin.tm_hour=TOFRECFFKEY.hour[0];
  begin.tm_mday=TOFRECFFKEY.day[0];
  begin.tm_mon=TOFRECFFKEY.mon[0];
  begin.tm_year=TOFRECFFKEY.year[0];

  end.tm_sec=TOFRECFFKEY.sec[1];
  end.tm_min=TOFRECFFKEY.min[1];
  end.tm_hour=TOFRECFFKEY.hour[1];
  end.tm_mday=TOFRECFFKEY.day[1];
  end.tm_mon=TOFRECFFKEY.mon[1];
  end.tm_year=TOFRECFFKEY.year[1];

  TID.add (new AMSTimeID(AMSID("Tofbarcal1",isRealData()),
    begin,end,TOF1GC::SCBLMX*sizeof(TOFBrcal::scbrcal[0][0]),
    (void*)&TOFBrcal::scbrcal[0][0],server));
   
  TID.add (new AMSTimeID(AMSID("Tofvpar",isRealData()),
    begin,end,sizeof(TOFVarp::tofvpar),
    (void*)&TOFVarp::tofvpar,server));
   
  TID.add (new AMSTimeID(AMSID("Tofmcscans",isRealData()),
    begin,end,TOF1GC::SCBLMX*sizeof(AMSTOFScan::scmcscan[0]),
    (void*)&AMSTOFScan::scmcscan[0],server));
 }
   
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
 if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
 
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

  TID.add (new AMSTimeID(AMSID("Antisccal2",isRealData()),
     begin,end,ANTI2C::MAXANTI*sizeof(ANTI2Pcal::antisccal[0]),
                                  (void*)&ANTI2Pcal::antisccal[0],server));
//
// TID.add (new AMSTimeID(AMSID("Antivpar2",isRealData()),
//    begin,end,sizeof(ANTI2Varp::antivpar),
//    (void*)&ANTI2Varp::antivpar,server));
 }
//
 else{   
 
  begin.tm_sec=ANTIRECFFKEY.sec[0];
  begin.tm_min=ANTIRECFFKEY.min[0];
  begin.tm_hour=ANTIRECFFKEY.hour[0];
  begin.tm_mday=ANTIRECFFKEY.day[0];
  begin.tm_mon=ANTIRECFFKEY.mon[0];
  begin.tm_year=ANTIRECFFKEY.year[0];

  end.tm_sec=ANTIRECFFKEY.sec[1];
  end.tm_min=ANTIRECFFKEY.min[1];
  end.tm_hour=ANTIRECFFKEY.hour[1];
  end.tm_mday=ANTIRECFFKEY.day[1];
  end.tm_mon=ANTIRECFFKEY.mon[1];
  end.tm_year=ANTIRECFFKEY.year[1];

  TID.add (new AMSTimeID(AMSID("Antisccal",isRealData()),
     begin,end,MAXANTI*sizeof(ANTIPcal::antisccal[0]),
                                  (void*)&ANTIPcal::antisccal[0],server));
//
// TID.add (new AMSTimeID(AMSID("Antivpar",isRealData()),
//    begin,end,sizeof(ANTIVarp::antivpar),
//    (void*)&ANTIVarp::antivpar,server));
 }   
}
//---------------------------------------
//
//   ECAL : TDV-reservation for calibration parameters of all PM's:
//
{
 if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
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

  TID.add (new AMSTimeID(AMSID("Ecalpmcalib",isRealData()),
     begin,end,ECPMSL*sizeof(ECcalib::ecpmcal[0][0]),
                                  (void*)&ECcalib::ecpmcal[0][0],server));
//
  TID.add (new AMSTimeID(AMSID("Ecalvpar",isRealData()),
     begin,end,sizeof(ECALVarp::ecalvpar),
                                      (void*)&ECALVarp::ecalvpar,server));
   
//  TID.add (new AMSTimeID(AMSID("Ecalpeds",isRealData()),
//    begin,end,ECPMSL*sizeof(ECPMPeds::pmpeds[0][0]),
//    (void*)&ECPMPeds::pmpeds[0][0],server));
 }
}
//
//-----------------------------------------
//
//   CTC : calibration parameters for all combinations
//
{
tm begin;
tm end;
begin.tm_isdst=0;
end.tm_isdst=0;
begin.tm_sec=CTCRECFFKEY.sec[0];
begin.tm_min=CTCRECFFKEY.min[0];
begin.tm_hour=CTCRECFFKEY.hour[0];
begin.tm_mday=CTCRECFFKEY.day[0];
begin.tm_mon=CTCRECFFKEY.mon[0];
begin.tm_year=CTCRECFFKEY.year[0];

end.tm_sec=CTCRECFFKEY.sec[1];
end.tm_min=CTCRECFFKEY.min[1];
end.tm_hour=CTCRECFFKEY.hour[1];
end.tm_mday=CTCRECFFKEY.day[1];
end.tm_mon=CTCRECFFKEY.mon[1];
end.tm_year=CTCRECFFKEY.year[1];

   
TID.add (new AMSTimeID(AMSID("CTCccal",isRealData()),
   begin,end,CTCCCMX*sizeof(ctcfcal[0]),
   (void*)&ctcfcal[0],server));
}
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
   if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
   
     TID.add (new AMSTimeID(AMSID("TOF2Temperature",isRealData()),
                         begin,end,
                         TOF2Varp::tofvpar.gettoftsize(),(void*)TOF2Varp::tofvpar.gettoftp(),server));
   }
   else{
   
     TID.add (new AMSTimeID(AMSID("TOFTemperature",isRealData()),
                         begin,end,
                         TOFVarp::tofvpar.gettoftsize(),(void*)TOFVarp::tofvpar.gettoftp(),server));
   }   
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
   if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
     TID.add (new AMSTimeID(AMSID("ScalerN",isRealData()),
                         begin,end,
                         Trigger2LVL1::getscalerssize(),(void*)Trigger2LVL1::getscalersp(),server));
   }
   else{
     TID.add (new AMSTimeID(AMSID("ScalerN",isRealData()),
                         begin,end,
                         TriggerLVL1::getscalerssize(),(void*)TriggerLVL1::getscalersp(),server));
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
  _signendjob();
  uhend();
  cout <<"uhend finished"<<endl;
  _tkendjob();
  cout <<"tkendjob finished"<<endl;
  if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
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
}
else{
  _tofendjob();
  cout <<"tofendjob finished"<<endl;
  _antiendjob();
  cout <<"antiendjob finished"<<endl;
  _ctcendjob();
  cout <<"ctcendjob finished"<<endl;
}

_dbendjob();
  cout <<"dbendjob finished"<<endl;
_axendjob();
  cout <<"axendjob finished"<<endl;
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
  if(AMSEvent::gethead())AMSProducer::gethead()->sendNtupleEnd(ntuple_entries,AMSEvent::gethead()->getid(),AMSEvent::gethead()->gettime(),true);
else AMSProducer::gethead()->sendNtupleEnd(ntuple_entries,e,t,true);
#endif
}
}

void AMSJob::urinit(integer eventno){
  if(IOPA.WriteRoot){
    char rfile[161];
    UHTOC(IOPA.rfile,40,rfile,160);  
    char filename[256];
    strcpy(filename,rfile);
    integer iostat;
    integer rsize=8000;
    if(eventno){
      char event[80];  
      sprintf(event,".%d",eventno);
      strcat(filename,event);
    }
    if(_pntuple)_pntuple->initR(filename);
    else{
        _pntuple = new AMSNtuple(filename);
    }
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
  }
   HBOOK1(200101,"Number of Nois Hits x",100,-0.5,99.5,0.);
   HBOOK1(200102,"Number of Nois Hits y",100,-0.5,99.5,0.);
   HBOOK1(200105,"Above threshold spectrum x",200,-0.5,49.5,0.);
   HBOOK1(200106,"Above threshold spectrum y",200,-0.5,49.5,0.);
   HBOOK1(200107," adc over",3000,29999.5,32999.5,0.);
    _NtupleActive=true;
#ifdef __CORBA__
  AMSProducer::gethead()->sendNtupleStart(_ntuplefilename,run,eventno,tt);
#endif
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

//------------------------------------------------------------------
void AMSJob::_ctcendjob(){
  if(isSimulation() && CTCRECFFKEY.reprtf[0]>0){
    HPRINT(3500);
  }
  if(CTCRECFFKEY.reprtf[0]>0){
    HPRINT(3000);
    HPRINT(3001);
  }
//

  if(CTCRECFFKEY.reprtf[0]>0)CTCJobStat::print(); // Print CTC JOB-statistics
}
//
//-------------------------------------------------------------------
void AMSJob::_tofendjob(){
       if(isSimulation())TOFJobStat::outpmc();
       TOFJobStat::outp();
       TOFJobStat::printstat(); // Print JOB-TOF statistics
}
//-----------------
void AMSJob::_tof2endjob(){
       if(isSimulation())TOF2JobStat::outpmc();
       TOF2JobStat::outp();
       TOF2JobStat::printstat(); // Print JOB-TOF statistics
}
//-----------------------------------------------------------------------
void AMSJob::_antiendjob(){
  if(ANTIRECFFKEY.reprtf[0]>0){
    ANTIJobStat::print(); // Print JOB-ANTI statistics
  }
  if(isSimulation() && ANTIMCFFKEY.mcprtf>0){
    HPRINT(2000);
  }
  if(ANTIRECFFKEY.reprtf[0]>0){
    HPRINT(2500);
    HPRINT(2501);
  }
}
//---------------------------
void AMSJob::_anti2endjob(){
  if(ATREFFKEY.reprtf[0]>0){
    ANTI2JobStat::print();
  }
  if(isSimulation() && ATMCFFKEY.mcprtf>0){
    HPRINT(2000);
  }
  if(ATREFFKEY.reprtf[0]>0){
    HPRINT(2500);
    HPRINT(2501);
  }
}
//-----------------------------------------------------------------------
void AMSJob::_ecalendjob(){
//
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
  void AMSJob::_redaqinitjob(){
     AMSgObj::BookTimer.book("SIDAQ");
     AMSgObj::BookTimer.book("REDAQ");
     if(IOPA.Portion<1. && isMonitoring())cout <<"AMSJob::_redaqinitjob()-W-Only about "<<IOPA.Portion*100<<"% events will be processed."<<endl; 
    // Add subdetectors to daq
    //

  {  // mc
    if(!isRealData()){
    DAQEvent::addsubdetector(&AMSmceventg::checkdaqid,&AMSmceventg::buildraw);
    DAQEvent::addblocktype(&AMSmceventg::getmaxblocks,&AMSmceventg::calcdaqlength,
    &AMSmceventg::builddaq);

    DAQEvent::addsubdetector(&AMSEvent::checkdaqidSh,&AMSEvent::buildrawSh);
    DAQEvent::addblocktype(&AMSEvent::getmaxblocksSh,
    &AMSEvent::calcdaqlengthSh,&AMSEvent::builddaqSh);
    }

  }


  {
    // lvl1

      DAQEvent::addsubdetector(&TriggerLVL1::checkdaqid,&TriggerLVL1::buildraw);
      DAQEvent::addblocktype(&TriggerLVL1::getmaxblocks,&TriggerLVL1::calcdaqlength,
      &TriggerLVL1::builddaq);

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
      DAQEvent::addsubdetector(&TriggerLVL3::checkdaqid,&TriggerLVL3::buildraw);
      DAQEvent::addblocktype(&TriggerLVL3::getmaxblocks,&TriggerLVL3::calcdaqlength,
      &TriggerLVL3::builddaq);

}    
if(DAQCFFKEY.SCrateinDAQ){
//         tof+anti+ctc
    DAQEvent::addsubdetector(&DAQSBlock::checkblockid,&DAQSBlock::buildraw);
    DAQEvent::addblocktype(&DAQSBlock::getmaxblocks,&DAQSBlock::calcblocklength,
                           &DAQSBlock::buildblock);
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
//------------------------------------------------------------------
  void AMSJob::_redaq2initjob(){
     AMSgObj::BookTimer.book("SIDAQ");
     AMSgObj::BookTimer.book("REDAQ");
     if(IOPA.Portion<1. && isMonitoring())cout <<"AMSJob::_redaq2initjob()-W-Only about "<<IOPA.Portion*100<<"% events will be processed."<<endl; 
    // Add subdetectors to daq
    //

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
    DAQEvent::addsubdetector(&TriggerLVL3::checkdaqid,&TriggerLVL3::buildraw);
    DAQEvent::addblocktype(&TriggerLVL3::getmaxblocks,&TriggerLVL3::calcdaqlength,
    &TriggerLVL3::builddaq);

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
