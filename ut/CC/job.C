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
#include <amsgobj.h>
#include <cern.h>
#include <math.h>
#include <amsdbc.h>
#include <commons.h>
#include <iostream.h>
#include <fstream.h>
#include <trid.h>
#include <mccluster.h>
#include <extC.h>
#include <job.h>
#include <event.h>
#include <charge.h>
#include <timeid.h>
#include <mceventg.h>
#include <trcalib.h>
#include <tofdbc.h>
#include <antidbc.h>
#include <ctcdbc.h>
#include <tofsim.h>
#include <tofrec.h>
#include <tofcalib.h>
#include <trigger1.h>
#include <trigger3.h>
#include <trid.h>
#include <trrawcluster.h>
#include <daqevt.h>
#include <daqblock.h>
#include <user.h>
//+
 integer        ntdvNames;                       // number of TDV's types
 char           *tdvNameTab[maxtdv];             // TDV's nomenclature
 int             tdvIdTab[maxtdv];
//
#ifdef __DB__

#include <dbS.h>

integer*   AMSJob::_ptr_start;
integer*   AMSJob::_ptr_end;
tdv_time*  AMSJob::_tdv;

extern LMS* lms;

#endif
//-

realorbit AMSJob::Orbit;
AMSJob* AMSJob::_Head=0;
AMSNodeMap AMSJob::JobMap;
integer AMSJob::debug=1;
const uinteger AMSJob::Reconstruction=1;
const uinteger AMSJob::RealData=2;
const uinteger AMSJob::CTracker=4;
const uinteger AMSJob::CTOF=8;
const uinteger AMSJob::CAnti=16;
const uinteger AMSJob::CCerenkov=32;
const uinteger AMSJob::CMagnet=64;
const uinteger AMSJob::CRICH=128;
const uinteger AMSJob::CTRD=256;
const uinteger AMSJob::CAMS=512;
const uinteger AMSJob::Calibration=AMSJob::CTracker+
                                   AMSJob::CTOF+
                                   AMSJob::CAnti+
                                   AMSJob::CCerenkov+
                                   AMSJob::CMagnet+
                                   AMSJob::CRICH+
                                   AMSJob::CTRD+
                                   AMSJob::CAMS;
//
extern AMSTOFScan scmcscan[SCBLMX];// TOF MC time/eff-distributions
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// TOF individual sc.bar parameters 
TOFVarp tofvpar; // TOF general parameters (not const !)
extern ANTIPcal antisccal[MAXANTI]; // ANTI-counter individ.parameters
extern CTCCCcal ctcfcal[CTCCCMX];//  CTC calibr. objects
//
void AMSJob::data(){
#ifdef __HPUX__
  AMSTrIdSoft::init();
  //AMSCharge::init();
  AMSgObj::GTrMedMap=*(new  AMSNodeMap() );
  AMSgObj::GVolMap= *(new  AMSNodeMap() );
  AMSgObj::BookTimer= *(new AMSStat() );
#endif  



  SELECTFFKEY.Run=0;
  SELECTFFKEY.Event=0;
  FFKEY("SELECT",(float*)&SELECTFFKEY,sizeof(SELECTFFKEY_DEF)/sizeof(integer),"MIXED");

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
IOPA.SplitNtuple=1;
IOPA.WriteAll=0;
VBLANK(IOPA.TriggerC,40);
VBLANK(AMSFFKEY.TDVC,400);
char amsp[12]="AMSParticle";
UCTOH(amsp,IOPA.TriggerC,4,12);
IOPA.mode=0;
VBLANK(IOPA.ffile,40);
FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");
TRMFFKEY.OKAY=0;
FFKEY("TERM",(float*)&TRMFFKEY,sizeof(TRMFFKEY_DEF)/sizeof(integer),"MIXED");




_sitkdata();
_signdata();
_sitofdata();
_siantidata();
_sitrddata();
_sictcdata();
_sitrigdata();
}

void AMSJob::_sitrigdata(){
// TOF :
// these are additional requir. to "hardware"-defined TOFMCFFKEY.trlogic[]
  LVL1FFKEY.ntof=3;// min. fired TOF-planes
// ANTI :
  LVL1FFKEY.nanti=2;// max. fired ANTI-paddles 
//
  LVL1FFKEY.RebuildLVL1=0;
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
  LVL3FFKEY.TrTOFSearchReg=6.;
  LVL3FFKEY.TrMinResidual=0.03;
  LVL3FFKEY.TrMaxResidual[0]=1.2;
  LVL3FFKEY.TrMaxResidual[1]=0.2;
  LVL3FFKEY.TrMaxResidual[2]=0.3;
  LVL3FFKEY.TrMaxHits=20;
  LVL3FFKEY.Splitting=0.04;
  LVL3FFKEY.NRep=1;
  LVL3FFKEY.Accept=0;
  LVL3FFKEY.RebuildLVL3=0;
  LVL3FFKEY.NoK=1;
  LVL3FFKEY.TrHeavyIonThr=200;
  FFKEY("L3REC",(float*)&LVL3FFKEY,sizeof(LVL3FFKEY_DEF)/sizeof(integer),"MIXED");


}

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
TRMCFFKEY.delta[0]=0.85;
TRMCFFKEY.delta[1]=1.0;
TRMCFFKEY.gammaA[0]=0.3;
TRMCFFKEY.gammaA[1]=0.1;



TRMCFFKEY.cmn[0]=10;
TRMCFFKEY.cmn[1]= 6;
TRMCFFKEY.adcoverflow=32767;
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
TRMCFFKEY.year[0]=95;
TRMCFFKEY.year[1]=99;
TRMCFFKEY.GenerateConst=0;
TRMCFFKEY.WriteHK=0;
TRMCFFKEY.thr1R[0]=4.5;
TRMCFFKEY.thr1R[1]=4.5;
TRMCFFKEY.thr2R[0]=1;
TRMCFFKEY.thr2R[1]=1;
TRMCFFKEY.neib[0]=1;
TRMCFFKEY.neib[1]=1;
TRMCFFKEY.CalcCmnNoise[0]=1;
TRMCFFKEY.CalcCmnNoise[1]=3;
{
int i,j,k;
for(i=0;i<2;i++){
  for(j=0;j<2;j++){
   for(k=0;k<32;k++)TRMCFFKEY.RawModeOn[i][j][k]=0;
  }
}
}
FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");

TRCALIB.CalibProcedureNo=1;
TRCALIB.EventsPerCheck=100;
TRCALIB.PedAccRequired[0]=0.12;
TRCALIB.PedAccRequired[1]=0.09;
TRCALIB.Validity[0]=1800;
TRCALIB.Validity[1]=1800+3600;
TRCALIB.RhoThrA=0.7;
TRCALIB.RhoThrV=0.8;
TRCALIB.BadChanThr[0]=3.3;
TRCALIB.BadChanThr[1]=0.002;
TRCALIB.Method=2;
TRCALIB.Pass=1;
TRCALIB.DPS=0;
TRCALIB.UPDF=4;
TRCALIB.PrintBadChList=0;
TRCALIB.EventsPerIteration[0]=100;
TRCALIB.EventsPerIteration[1]=100;
TRCALIB.NumberOfIterations[0]=200;
TRCALIB.NumberOfIterations[1]=200;
TRCALIB.BetaCut[0][0]=0.7;
TRCALIB.BetaCut[0][1]=10.;
TRCALIB.BetaCut[1][0]=1;
TRCALIB.BetaCut[1][1]=10;
TRCALIB.HitsRatioCut[0]=2.2;
TRCALIB.HitsRatioCut[1]=2.2;
TRCALIB.MomentumCut[0][0]=-FLT_MAX;
TRCALIB.MomentumCut[0][1]=FLT_MAX;
TRCALIB.MomentumCut[1][0]=3;
TRCALIB.MomentumCut[1][1]=FLT_MAX;
TRCALIB.Chi2Cut[0]=3;
TRCALIB.Chi2Cut[1]=3;
int i,j,k;
for (i=0;i<6;i++){
 for(j=0;j<3;j++){
   TRCALIB.InitialCoo[i][j]=0;
   for(k=0;k<3;k++)TRCALIB.InitialRM[i][j][k]=0;
 }
}
FFKEY("TRCALIB",(float*)&TRCALIB,sizeof(TRCALIB_DEF)/sizeof(integer),"MIXED");

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
CCFFKEY.albedor=0.01;
CCFFKEY.albedocz=0.05;
CCFFKEY.npat=1;
CCFFKEY.run=100;
CCFFKEY.low=0;
CCFFKEY.earth=1;
CCFFKEY.theta=51.;
CCFFKEY.phi=290.;
CCFFKEY.polephi=290.;
CCFFKEY.begindate=22051998;
CCFFKEY.enddate=  26051998;
CCFFKEY.begintime=80000;
CCFFKEY.endtime=120000;
CCFFKEY.oldformat=0;

CCFFKEY.sdir=1;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=================================================================================
void AMSJob::_sitofdata(){
  TOFMCFFKEY.TimeSigma=1.6e-10; // time resolution(sec) for simplified algorithm
  TOFMCFFKEY.TimeSigma2=4.5e-10;
  TOFMCFFKEY.TimeProbability2=0.035;
  TOFMCFFKEY.padl=10.5;        // sc. bar transv. step ........................
  TOFMCFFKEY.Thr=0.1;          // Sc.bar Elos-thresh.(Mev) to participate in Simul.   
//
  TOFMCFFKEY.mcprtf[0]=0;     // TOF MC print flag for init arrays
  TOFMCFFKEY.mcprtf[1]=0;     // TOF MC print flag for MC pulses
  TOFMCFFKEY.mcprtf[2]=0;     // ...................... histograms
  TOFMCFFKEY.mcprtf[3]=0;     // spare
  TOFMCFFKEY.mcprtf[4]=0;     // spare
  TOFMCFFKEY.trlogic[0]=1; // MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TOFMCFFKEY.trlogic[1]=0; // ......................(=0/1-> ANY3/ALL4 layer coincidence) 
  TOFMCFFKEY.fast=0;       // 0/1-> fast/slow simulation algorithm
  TOFMCFFKEY.daqfmt=0;     // 0/1-> raw/reduced TDC format for DAQ simulation
  TOFMCFFKEY.birks=1;       // 0/1->  not apply/apply birks corrections
FFKEY("TOFMC",(float*)&TOFMCFFKEY,sizeof(TOFMCFFKEY_DEF)/sizeof(integer),"MIXED");
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
  CTCGEOMFFKEY.supzc=-91.2;     // supp. honeycomb plate (center) Z-pos
  CTCGEOMFFKEY.aegden=0.125;    // aerogel density
  CTCGEOMFFKEY.wlsden=1.03;     // WLS density
  CTCGEOMFFKEY.nblk=1;          // number of aer. blocks (X-div.)(=1 for solid)
  CTCGEOMFFKEY.nwls=1;          // number of wls blocks
  CTCGEOMFFKEY.ptfx=0.025;      // PTFE thickness in cm
  CTCGEOMFFKEY.thcsize[0]=150.; //
  CTCGEOMFFKEY.thcsize[1]=110.; // Support structure dimensions (cm)
  CTCGEOMFFKEY.thcsize[2]=6.;   //
  CTCGEOMFFKEY.upper[0]=118.2;  //
  CTCGEOMFFKEY.upper[1]=94.2;   // Upper/Lower layer dimensions (cm)
  CTCGEOMFFKEY.upper[2]=15.50;  //
  CTCGEOMFFKEY.wall[0]=0.2;     //
  CTCGEOMFFKEY.wall[1]=94.2;    // Wall dimensions (cm)
  CTCGEOMFFKEY.wall[2]=7.175;   //
  CTCGEOMFFKEY.cell[0]=23.4;    //
  CTCGEOMFFKEY.cell[1]=23.4;    // Super cell dimensions (cm)
  CTCGEOMFFKEY.cell[2]=15.50;   //
  CTCGEOMFFKEY.ygap[0]=23.4;    // 
  CTCGEOMFFKEY.ygap[1]=0.2;     // Gap dimensions (cm)
  CTCGEOMFFKEY.ygap[2]=15.50;   //
  CTCGEOMFFKEY.agel[0]=11.60;   //
  CTCGEOMFFKEY.agel[1]=11.60;   // AeroGEL dimensions (cm)
  CTCGEOMFFKEY.agel[2]=7.025;   //
  CTCGEOMFFKEY.pmt[0]=3.0;      //
  CTCGEOMFFKEY.pmt[1]=3.0;      // Phototube dimensions (including housing
  CTCGEOMFFKEY.pmt[2]=7.5;      //          (and shielding)
  CTCGEOMFFKEY.ptfe[0]=11.75;   //
  CTCGEOMFFKEY.ptfe[1]=11.75;   // PTFE box dimensions (cm)
  CTCGEOMFFKEY.ptfe[2]=7.175;   //
  CTCGEOMFFKEY.xdiv=5;
  CTCGEOMFFKEY.ydiv=4;
//
  CTCMCFFKEY.Refraction[0]=1.036;   // Refraction indexes
  CTCMCFFKEY.Refraction[1]=1.58;
  CTCMCFFKEY.Path2PhEl[0]=11;   // Path to photoelectrons conv fact ( ~ 6 p.e.)
  CTCMCFFKEY.Path2PhEl[1]=28;
  CTCMCFFKEY.AbsLength[0]=15;   // Abs Length in cm  
  CTCMCFFKEY.AbsLength[1]=100;
  CTCMCFFKEY.Edep2Phel[0]=0;      // Agel is not a scint
  CTCMCFFKEY.Edep2Phel[1]=184e3;  // WLS is scint if vertical readout
  CTCMCFFKEY.mcprtf=0;//print MC-hist if !=0

FFKEY("CTCGEOM",(float*)&CTCGEOMFFKEY,sizeof(CTCGEOMFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CTCMC",(float*)&CTCMCFFKEY,sizeof(CTCMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//================================================================================
void AMSJob::_siantidata(){
  ANTIGEOMFFKEY.scradi=54.385; // internal radious of ANTI sc. cylinder (cm)
  ANTIGEOMFFKEY.scinth=1.;     // thickness of scintillator (cm)
  ANTIGEOMFFKEY.scleng=83.;    // scintillator paddle length (glob. Z-dim)
  ANTIGEOMFFKEY.wrapth=0.04;   // wrapper thickness (cm)
  ANTIGEOMFFKEY.groovr=0.45;   // groove radious (bump_rad = groove_rad-pdlgap)
  ANTIGEOMFFKEY.pdlgap=0.1;   // inter paddle gap (cm)(2*wrapth+extra)
  ANTIGEOMFFKEY.stradi=54.235; // inner radious of supp. tube
  ANTIGEOMFFKEY.stleng=83.;    // length of supp. tube
  ANTIGEOMFFKEY.stthic=0.12;   // thickness of supp. tube
  
//---
  ANTIMCFFKEY.mcprtf=0;     // print_hist flag (0/1->no/yes)
  ANTIMCFFKEY.SigmaPed=1;   // ped.distribution width (p.e)
  ANTIMCFFKEY.MeV2PhEl=20.; // Mev->Ph.el. MC conv.nfactor (pe/Mev)(= 2side_signal(pe)/Eloss(mev)
//                                                                  measured at center)
  ANTIMCFFKEY.LZero=120;    // attenuation length for one-side signal (cm)
  ANTIMCFFKEY.PMulZPos=0.;  // PM-position(=Ltot/2)-is taken later from geometry in siantiinitjob
  ANTIMCFFKEY.LSpeed=17.;   // Eff. light speed in anti-paddle (cm/ns)
//---
  FFKEY("ANGE",(float*)&ANTIGEOMFFKEY,sizeof(ANTIGEOMFFKEY_DEF)/sizeof(integer),
  "MIXED");
  FFKEY("ANMC",(float*)&ANTIMCFFKEY,sizeof(ANTIMCFFKEY_DEF)/sizeof(integer),
  "MIXED");
}
//================================================================================

void AMSJob::_sitrddata(){
}

void AMSJob:: _reamsdata(){
_remfdata();
_retkdata();
_retofdata();
_reantidata();
_retrddata();
_rectcdata();
_reaxdata();
_redaqdata();
}

void AMSJob::_remfdata(){
TKFIELD.iniok=1;
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
FFKEY("BMAP",(float*)&TKFIELD,11,"MIXED");

}

void AMSJob::_retkdata(){

number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
TRCLFFKEY.ThrClA[1]=45/fac;
TRCLFFKEY.Thr1A[1] =30/fac;
TRCLFFKEY.Thr2A[1] =10/fac;

TRCLFFKEY.ThrClS[1]=15;
TRCLFFKEY.Thr1S[1] =10;
TRCLFFKEY.Thr2S[1] =10;

TRCLFFKEY.ThrClR[1]=10/fac;
TRCLFFKEY.Thr1R[1] =8/fac;
TRCLFFKEY.Thr2R[1] =1.;  // should be around 1 if ThrClNEl[1]=3;
TRCLFFKEY.Thr3R[1] =-2.;

TRCLFFKEY.ThrClNMin[1]=2;
TRCLFFKEY.ThrClNEl[1]=3;

TRCLFFKEY.ThrClA[0]=37/fac;
TRCLFFKEY.Thr1A[0] =30/fac;
TRCLFFKEY.Thr2A[0] =15/fac;





TRCLFFKEY.ThrClS[0]=20;
TRCLFFKEY.Thr1S[0] =15;
TRCLFFKEY.Thr2S[0] =15;

TRCLFFKEY.ThrClR[0]=10/fac;
TRCLFFKEY.Thr1R[0] =8/fac;
TRCLFFKEY.Thr2R[0] =1.;
TRCLFFKEY.Thr3R[0] =-2.;

TRCLFFKEY.ThrClNMin[0]=1;
TRCLFFKEY.ThrClNEl[0]=3;

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

FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");

// Fit Par
TRFITFFKEY.pattern[0]=1;
TRFITFFKEY.pattern[1]=1;
TRFITFFKEY.pattern[2]=1;
TRFITFFKEY.pattern[3]=1;
TRFITFFKEY.pattern[4]=1;
// Changed to 1 for shuttle setup
TRFITFFKEY.pattern[5]=1;
TRFITFFKEY.pattern[6]=1;
TRFITFFKEY.pattern[7]=1;
TRFITFFKEY.pattern[8]=1;
TRFITFFKEY.pattern[9]=1;
TRFITFFKEY.pattern[10]=1;
TRFITFFKEY.pattern[11]=1;
TRFITFFKEY.pattern[12]=1;
TRFITFFKEY.pattern[13]=1;
TRFITFFKEY.pattern[14]=1;
TRFITFFKEY.pattern[15]=1;
TRFITFFKEY.pattern[16]=1;
TRFITFFKEY.pattern[17]=1;
TRFITFFKEY.pattern[18]=1;
TRFITFFKEY.pattern[19]=1;
TRFITFFKEY.pattern[20]=1;
TRFITFFKEY.pattern[21]=1;
{
  for( int k=22;k<42;k++)TRFITFFKEY.pattern[k]=0;
}
TRFITFFKEY.UseTOF=2;
TRFITFFKEY.Chi2FastFit=2000;
TRFITFFKEY.Chi2StrLine=20;
TRFITFFKEY.Chi2Circle=20;
TRFITFFKEY.ResCutFastFit=0.5;
TRFITFFKEY.ResCutStrLine=0.5;
TRFITFFKEY.ResCutCircle=0.5;
TRFITFFKEY.SearchRegFastFit=1;
TRFITFFKEY.SearchRegStrLine=0.25;
TRFITFFKEY.SearchRegCircle=1.5;
TRFITFFKEY.RidgidityMin=0.2;
TRFITFFKEY.FullReco=0;
TRFITFFKEY.MinRefitCos[0]=0.7;
TRFITFFKEY.MinRefitCos[1]=0.5;
TRFITFFKEY.FastTracking=1;
TRFITFFKEY.WeakTracking=0;
TRFITFFKEY.FalseXTracking=0;
TRFITFFKEY.Chi2FalseX=3.;
TRFITFFKEY.SearchRegTOF=6.;
TRFITFFKEY.FalseTOFXTracking=0;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}

void AMSJob::_retofdata(){
// 
  TOFRECFFKEY.Thr1=0.45;//(1) Threshold (mev) on peak bar energy
  TOFRECFFKEY.ThrS=0.45; //(2) Threshold (mev) on total cluster energy
//
  TOFRECFFKEY.reprtf[0]=0; //(3) RECO print flag for statistics 
  TOFRECFFKEY.reprtf[1]=0; //(4) print flag for DAQ (1/2-> print for decoding/dec+encoding)
  TOFRECFFKEY.reprtf[2]=0; //(5) print flag for histograms
  TOFRECFFKEY.reprtf[3]=0; //(6) print flag for TDC-hit multiplicity histograms 
  TOFRECFFKEY.reprtf[4]=0; //(7) print flag (spare)
//
  TOFRECFFKEY.relogic[0]=0;//(8) 0/1/2/3/4 ->normal/STRR+AVSD-/TDIF-/TZSL-/AMPL-calibr. run. 
  TOFRECFFKEY.relogic[1]=0;//(9) 0/1/2-> full_fTDC_use/no_time_matching/not_use 
  TOFRECFFKEY.relogic[2]=0;//(10) spare RECO logic flag 
  TOFRECFFKEY.relogic[3]=0;//(11) spare RECO logic flag 
  TOFRECFFKEY.relogic[4]=0;//(12) spare RECO logic flag
//
  TOFRECFFKEY.daqthr[0]=35.;//(13)Fast discr. thresh(mV) for fast/slow_TDC 
  TOFRECFFKEY.daqthr[1]=150.;//(14)Fast discr. thresh(mV) for FT-trigger (z>=1)  
  TOFRECFFKEY.daqthr[2]=150.;//(15)thresh(mV) for discr. of "z>2"-trig (dinode) 
  TOFRECFFKEY.daqthr[3]=0.;//(16)spare  
  TOFRECFFKEY.daqthr[4]=0.;//(17)spare
//
  TOFRECFFKEY.cuts[0]=50.;//(18)t-window(ns) for "the same hit" search in f/s_tdc
  TOFRECFFKEY.cuts[1]=15.;//(19)"befor"-cut in time history (ns)(max.PMT-pulse length?)
  TOFRECFFKEY.cuts[2]=400.;//(20)"after"-cut in time history (ns)(max. shaper integr.time?)
  TOFRECFFKEY.cuts[3]=2.;//(21) error in longitudinal coordinate (single TOF bar)
  TOFRECFFKEY.cuts[4]=0.;//(22)
  TOFRECFFKEY.cuts[5]=0.;//(23)
  TOFRECFFKEY.cuts[6]=0.;//(24)
  TOFRECFFKEY.cuts[7]=0.;//(25)
  TOFRECFFKEY.cuts[8]=0.;//(26)
  TOFRECFFKEY.cuts[9]=0.;//(27)
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
  TOFRECFFKEY.year[1]=99;
  FFKEY("TOFREC",(float*)&TOFRECFFKEY,sizeof(TOFRECFFKEY_DEF)/sizeof(integer),
  "MIXED");

//    defaults for calibration:
// TZSL-calibration:
  TOFCAFFKEY.pcut[0]=8.;// (1)track mom. low limit (gev/c) (prot, 0.85 for mu)
  TOFCAFFKEY.pcut[1]=50.;// (2)track mom. high limit
  TOFCAFFKEY.bmeanpr=0.996;// (3)mean prot. velocity in the above range
  TOFCAFFKEY.tzref[0]=0.;// (4)T0 for ref. counters
  TOFCAFFKEY.tzref[1]=0.;// (5)T0 for ref. counters
  TOFCAFFKEY.fixsl=25.;// (6)def. slope
  TOFCAFFKEY.bmeanmu=0.998;// (7)mean muon velocity in the above range
  TOFCAFFKEY.idref[0]=108;//(8)LBB for first ref. counter 
  TOFCAFFKEY.idref[1]=0;//(9)LBB for second ref. counter (if nonzero)
  TOFCAFFKEY.ifsl=1;//(10) 0/1 to fix/release slope param.
  TOFCAFFKEY.caltyp=0;// (11) 0/1->space/earth calibration
// AMPL-calibration:
  TOFCAFFKEY.truse=0; // (12) 1/0-> to use/not tracker
  TOFCAFFKEY.plhc[0]=1.;// (13) track mom. low limit(gev/c) for space calibr
  TOFCAFFKEY.plhc[1]=30.;// (14) track mom. high limit(gev/c) ..............
  TOFCAFFKEY.minev=100;// (15)min.events needed for measurement in channel or bin
  TOFCAFFKEY.trcut=0.85;// (16) cut to use for "truncated average" calculation
  TOFCAFFKEY.refbid[0]=201;//(17) ref.bar id's list (LBB) for btype=1->5
  TOFCAFFKEY.refbid[1]=202; 
  TOFCAFFKEY.refbid[2]=103; 
  TOFCAFFKEY.refbid[3]=104; 
  TOFCAFFKEY.refbid[4]=107;//(21)
  TOFCAFFKEY.plhec[0]=0.1;//(22)plow-cut for earth calibration
  TOFCAFFKEY.plhec[1]=10;  //(23)phigh-cut ...................
  TOFCAFFKEY.bgcut[0]=2; //(24) beta*gamma low-cut to be in mip-region(abs.calib)
  TOFCAFFKEY.bgcut[1]=10;//(25) beta*gamma high-cut ...
//
  TOFCAFFKEY.tofcoo=0; // (26) 0/1-> use transv/longit coord. from TOF 
  TOFCAFFKEY.dynflg=1; // (27) 0/1-> use stand/special(Contin's) dynode-calibration
  TOFCAFFKEY.cfvers=1; // (28) 1-99 -> vers.number for tofverlistNN.dat file 
  FFKEY("TOFCA",(float*)&TOFCAFFKEY,sizeof(TOFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//======================================================================
void AMSJob::_reantidata(){
  ANTIRECFFKEY.reprtf[0]=0;// Reco print_hist flag (0/1->no/yes)
  ANTIRECFFKEY.reprtf[1]=0;// DAQ-print (1/2->print for decoding/decoding+encoding)
  ANTIRECFFKEY.reprtf[2]=0;//spare
  ANTIRECFFKEY.ThrS=6;// threshold to create Cluster object (p.e.)
  ANTIRECFFKEY.PhEl2MeV=0.05;// reco conv. Phel->Mev(Mev/pe)( =Elos/2sides_signal measured at center)
  ANTIRECFFKEY.dtthr=3.; // trig.discr.theshold (same for all sides now) (p.e.'s for now)
  ANTIRECFFKEY.dathr=6.; // Amplitude(charge) discr.threshold(...) (p.e.)
  ANTIRECFFKEY.ftwin=100.;// t-window(ns) for true TDCA-hit search wrt TDCT-hit(FT) 
  ANTIRECFFKEY.sec[0]=0; 
  ANTIRECFFKEY.sec[1]=0;
  ANTIRECFFKEY.min[0]=0;
  ANTIRECFFKEY.min[1]=0;
  ANTIRECFFKEY.hour[0]=0;
  ANTIRECFFKEY.hour[1]=0;
  ANTIRECFFKEY.day[0]=1;
  ANTIRECFFKEY.day[1]=1;
  ANTIRECFFKEY.mon[0]=0;
  ANTIRECFFKEY.mon[1]=0;
  ANTIRECFFKEY.year[0]=96;
  ANTIRECFFKEY.year[1]=99;
  FFKEY("ANRE",(float*)&ANTIRECFFKEY,sizeof(ANTIRECFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  ANTICAFFKEY.cfvers=1; // (01-99) vers.number NN for antiverlistNN.dat file
  FFKEY("ANCA",(float*)&ANTICAFFKEY,sizeof(ANTICAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_rectcdata(){
  CTCRECFFKEY.Thr1=0.5;
  CTCRECFFKEY.ThrS=1.;
  CTCRECFFKEY.reprtf[0]=0;// Reco print_hist flag (0/1->no/yes)
  CTCRECFFKEY.reprtf[1]=0;// DAQ-print (1/2->print for decoding/decoding+encoding)
  CTCRECFFKEY.reprtf[2]=0;//spare
  CTCRECFFKEY.ftwin=100.;// t-window(ns) for true TDCA-hit search wrt TDCT-hit(FT)
  CTCRECFFKEY.sec[0]=0; 
  CTCRECFFKEY.sec[1]=0;
  CTCRECFFKEY.min[0]=0;
  CTCRECFFKEY.min[1]=0;
  CTCRECFFKEY.hour[0]=0;
  CTCRECFFKEY.hour[1]=0;
  CTCRECFFKEY.day[0]=1;
  CTCRECFFKEY.day[1]=1;
  CTCRECFFKEY.mon[0]=0;
  CTCRECFFKEY.mon[1]=0;
  CTCRECFFKEY.year[0]=96;
  CTCRECFFKEY.year[1]=99;
  FFKEY("CTCREC",(float*)&CTCRECFFKEY,sizeof(CTCRECFFKEY_DEF)/sizeof(integer),"MIXED");
// defaults for calibration:
  CTCCAFFKEY.cfvers=1; // (01-99) vers.number NN for ctcverlistNN.dat file
  FFKEY("CTCCA",(float*)&CTCCAFFKEY,sizeof(CTCCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
//========================================================================
void AMSJob::_redaqdata(){
DAQCFFKEY.mode=0;
DAQCFFKEY.OldFormat=0;
DAQCFFKEY.LCrateinDAQ=1;
DAQCFFKEY.SCrateinDAQ=1;
DAQCFFKEY.NoRecAtAll=0;
DAQCFFKEY.TrFormatInDAQ=3;
VBLANK(DAQCFFKEY.ifile,40);
VBLANK(DAQCFFKEY.ofile,40);
  FFKEY("DAQC",(float*)&DAQCFFKEY,sizeof(DAQCFFKEY_DEF)/sizeof(integer),"MIXED");

}


void AMSJob::_reaxdata(){
// Fit beta & charge
CHARGEFITFFKEY.Thr=1.;
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
FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}



void AMSJob::udata(){

#ifdef __MASSP__
  if(TRMFFKEY .OKAY!= 1234567890){
    cerr << "Datacards not terminated properly "<<endl;
    exit(1);  
   }
#endif

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
_jobtype=setjobtype(AMSFFKEY.Jobtype%10 != 0);
_jobtype=setjobtype(((AMSFFKEY.Jobtype/10)%10 != 0)<<(RealData-1));
uinteger ical=(AMSFFKEY.Jobtype/100)%10;
uinteger ucal=1;
if(ical)_jobtype=setjobtype(ucal<<(ical+1));



if(IOPA.mode && isSimulation()){
   AMSIO::setfile(ffile);
   AMSIO::init(IOPA.mode);
}
if(isReconstruction() && DAQCFFKEY.mode%10 ==0)DAQCFFKEY.mode=DAQCFFKEY.mode+1;
if(isSimulation() && DAQCFFKEY.mode%10 == 1)DAQCFFKEY.mode=DAQCFFKEY.mode-1;
if(DAQCFFKEY.mode){
   DAQEvent::setfiles(ifile,ofile);
   DAQEvent::init(DAQCFFKEY.mode);
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
    if(!strstr(getsetup(),"AMSSTATION") ){    
       AMSTrIdSoft::inittable();
    }
    else {
      cerr<<"AMSJob::_retkinitjob-E-NoAMSTrIdSoftTable exists for setup "<<
        getsetup()<< "yet "<<endl;
    }
       AMSTrIdSoft::init();

}


void AMSJob::init(){

AMSEvent::debug=AMSFFKEY.Debug;

_siamsinitjob();

_reamsinitjob();

if(isCalibration())_caamsinitjob();
_timeinitjob();
cout << *this;
}
void AMSJob::_siamsinitjob(){
  _sitkinitjob();
  if(isSimulation())_signinitjob();
  _sitofinitjob();
  _siantiinitjob();
  _sitrdinitjob();
  _sictcinitjob();
  _sitriginitjob();
}

void AMSJob::_sitriginitjob(){

     AMSgObj::BookTimer.book("LVL3");
     TriggerLVL3::init();  
}


void AMSJob::_sitkinitjob(){
  if(TRMCFFKEY.GenerateConst){
     for(int l=0;l<2;l++){
       for (int i=0;i<AMSDBc::nlay();i++){
         for (int j=0;j<AMSDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<AMSDBc::NStripsDrp(i+1,l);k++){
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

     AMSgObj::BookTimer.book("GEANTTRACKING");
     AMSgObj::BookTimer.book("SITKHITS");
     AMSgObj::BookTimer.book("SITKNOISE");
     AMSgObj::BookTimer.book("SITKDIGIa");
     AMSgObj::BookTimer.book("SITKDIGIb");
     AMSgObj::BookTimer.book("SITKDIGIc");
     AMSgObj::BookTimer.book("TrMCCluster");
}

void AMSJob::_signinitjob(){

  AMSmceventg::setcuts(CCFFKEY.coo,CCFFKEY.dir,CCFFKEY.momr,
  CCFFKEY.fixp,CCFFKEY.albedor,CCFFKEY.albedocz);
  //
  AMSmceventg::setspectra(CCFFKEY.begindate,CCFFKEY.begintime,
  CCFFKEY.enddate, CCFFKEY.endtime, GCKINE.ikine,CCFFKEY.low);

}
//----------------------------------------------------------------------------------------
void AMSJob::_sitofinitjob(){
  if(TOFMCFFKEY.fast==1)cout <<"_sitofinit-I-Fast(Crude) simulation algorithm selected."<<endl;
  else cout <<"_sitofinit-I-Slow(Accurate) simulation algorithm selected."<<endl;
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
    if(TOFMCFFKEY.mcprtf[2]!=0){ // Book mc-hist
      HBOOK1(1050,"Geant-hits in layer-1",80,0.,80.,0.);
      HBOOK1(1051,"Geant-hits in layer-2",80,0.,80.,0.);
      HBOOK1(1052,"Geant-hits in layer-3",80,0.,80.,0.);
      HBOOK1(1053,"Geant-hits in layer-4",80,0.,80.,0.);
      HBOOK1(1060,"Geant-Edep(mev) in layer-1",80,0.,24.,0.);
      HBOOK1(1061,"Geant-Edep(mev) in layer-1",80,0.,240.,0.);
      HBOOK1(1062,"Geant-Edep(mev) in layer-3",80,0.,24.,0.);
      HBOOK1(1063,"Geant-Edep(mev) in layer-3",80,0.,240.,0.);
      HBOOK2(1070,"Log(Qa) vs TovT,all channels",70,0.,280.,70,0.,8.4,0.);
      HBOOK1(1071,"Total bar pulse-charge(pC),L-1",80,0.,1600.,0.);
      HBOOK1(1072,"Total bar pulse-charge(pC),L-1",80,0.,16000.,0.);
      HBOOK1(1073,"PMT-pulse amplitude(mV,id=108,s1)",80,0.,1000.,0.);
      HBOOK1(1074,"Shaper amplitude (pC),all channels",80,0.,160.,0.);
      HBOOK1(1075,"L=1,PM=1,Shaper TovT (ns)",80,50.,290.,0.);
    }


}

void AMSJob::_siantiinitjob(){
  AMSgObj::BookTimer.book("SIANTIEVENT");
  if(ANTIMCFFKEY.mcprtf){
    HBOOK1(2000,"ANTI_counters total energy (geant,Mev)",60,0.,30.,0.); 
  }
//
  AMSgvolume *pg=AMSJob::gethead()->getgeomvolume(AMSID("ANTS",1));
      #ifdef __AMSDEBUG__
       assert (pg != NULL);
      #endif
     number par[5];
     for(int i=0;i<5;i++)par[i]=pg->getpar(i);
     ANTIMCFFKEY.PMulZPos=par[2];// half-length (in Z)

}

void AMSJob::_sictcinitjob(){
     AMSgObj::BookTimer.book("SICTCEVENT");
     if(CTCMCFFKEY.mcprtf>0){
       HBOOK1(3500,"CTC total MC raw signal(p.e.)",80,0.,80.,0.);
     }
}

void AMSJob::_sitrdinitjob(){
}


void AMSJob::_reamsinitjob(){
_remfinitjob();
_redaqinitjob();
_retkinitjob();
_retofinitjob();
_reantiinitjob();
_retrdinitjob();
_rectcinitjob();
_reaxinitjob();
AMSUser::InitJob();
}
void AMSJob::_caamsinitjob(){
if(isCalibration() & CTracker)_catkinitjob();
if(isCalibration() & CTOF)_catofinitjob();
if(isCalibration() & CAnti)_cantinitjob();
if(isCalibration() & CTRD)_catrdinitjob();
if(isCalibration() & CCerenkov)_cactcinitjob();
if(isCalibration() & CAMS)_caaxinitjob();
}

void AMSJob::_catkinitjob(){
AMSgObj::BookTimer.book("CalTrFill");
AMSgObj::BookTimer.book("CalTrFit");
if(TRCALIB.CalibProcedureNo == 1){
  AMSTrIdCalib::initcalib();
}
else if(TRCALIB.CalibProcedureNo == 2){
int i,j;
for(i=0;i<2;i++){
  for(j=0;j<tkcalpat;j++){
    AMSTrCalibFit::setHead(i,j, new 
    AMSTrCalibFit(j,TRCALIB.EventsPerIteration[i],TRCALIB.NumberOfIterations[i],i));
  }
}
}
}
//---------------------------------------------------------------------
void AMSJob::_catofinitjob(){
 if(TOFRECFFKEY.relogic[0]==1){
   TOFSTRRcalib::init();// TOF STRR-calibr.
   cout<<"TOFSTRRcalib-init done !!!"<<'\n';
   TOFAVSDcalib::init();// TOF AVSD-calibr.
   cout<<"TOFAVSDcalib-init done !!!"<<'\n';
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
 }
}
//---------------------------------------------------------------------
void AMSJob::_catrdinitjob(){
}

void AMSJob::_cactcinitjob(){
}

void AMSJob::_cantinitjob(){
}


void AMSJob::_caaxinitjob(){
}

void AMSJob::_remfinitjob(){
READMFIELD();
}


void AMSJob::_retkinitjob(){
AMSgObj::BookTimer.book("RETKEVENT");
AMSgObj::BookTimer.book("TrCluster");
AMSgObj::BookTimer.book("TrClusterRefit");
AMSgObj::BookTimer.book("TrRecHit");
AMSgObj::BookTimer.book("TrTrack");
}
//--------------------------------------------------------------------------
void AMSJob::_retofinitjob(){
  int i,j,k,ich,il,ib,ii,jj;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:validation");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");
    if(TOFRECFFKEY.reprtf[2]!=0){ // Book reco-hist
      HBOOK1(1100,"Fast-Slow hit time-difference(single hist/slow-hit measurements)",80,0.,80.,0.);
      HBOOK1(1105,"Anode-Slow hit time-difference(single an/slow-hit measurements)",80,-40.,40.,0.);
      HBOOK1(1106,"Dynode-Slow hit time-difference(single dyn/slow-hit measurements)",80,-120.,40.,0.);
      HBOOK1(1107,"TOF/CTC/ANTI data length(raw format)",80,100.,1100.,0.);
      HBOOK1(1108,"TOF/CTC/ANTI data length(reduced format)",80,0.,1000.,0.);
      HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,160.,0.);
      HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,1600.,0.);
      HBOOK1(1103,"Time_history:inp.pulse width(ns)",80,0.,80.,0.);
      HBOOK1(1104,"Anode TovT:inp.pulse width(ns)",80,0.,320.,0.);
      HBOOK1(1110,"RawClusterLevel:Total fired layers per event",5,0.,5.,0.);
      HBOOK1(1111,"RawClusterLevel:Layer appearence frequency",5,0.,5.,0.);
      HBOOK1(1112,"RawClusterLevel:Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
      HBOOK1(1113,"RawClusterLevel:SingleBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
      HBOOK1(1114,"RawClusterLevel:Single2SidedBarLayer Configuration(<2;2;>2->missingL)",10,-1.,9.,0.);
      HBOOK1(1115,"Fast-Slow hit time-difference(all hist/slow-hit meas.",80,-40.,120.,0.);
    HBOOK1(1130,"W1,L4-B3-S1",50,0.,200.,0.);
    HBOOK1(1131,"W2,L4-B3-S1",50,0.,100.,0.);
    HBOOK1(1132,"W3,L4-B3-S1",60,0.,6000.,0.);
    HBOOK1(1133,"W1,L4-B3-S2",50,0.,200.,0.);
    HBOOK1(1134,"W2,L4-B3-S2",50,0.,100.,0.);
    HBOOK1(1135,"W3,L4-B3-S2",60,0.,6000.,0.);
      HBOOK1(1529,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1526,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1530,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1527,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1531,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1528,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1532,"(T1-T3)(ns),corr,trl-normalized,ideal evnt",80,1.,9.,0.);
      HBOOK1(1533,"L=1,side1/2 raw T-diff(ns),ideal evnt",100,-2.,2.,0.);
      HBOOK1(1543,"L=1,Y-local(longit.coord),ideal evnt",100,-50.,50.,0.);
      HBOOK1(1534,"(T2-T4)(ns),corr,trl-normalized,ideal evnt",80,1.,9.,0.);
      HBOOK1(1544,"(T1-T3)-(T2-T4),(ns),corr,ideal evnt",80,-4.,4.,0.);
      HBOOK1(1545,"(T1-T4),(ns),uncorr,ideal evnt",50,3.5,6.5,0.);
      HBOOK1(1535,"L=1,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1536,"L=3,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1537,"L=1,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1538,"L=3,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1539,"L=2,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1540,"L=4,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1541,"L=2,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1542,"L=4,TOF Eclust(mev)",80,0.,240.,0.);
      if(TOFRECFFKEY.relogic[0]==1){ // STRR-calibration
        HBOOK1(1200,"Stretcher-ratio for indiv. channel",80,35.,55.,0.);
        HBOOK1(1201,"Offsets for indiv. channels",80,-100.,2300.,0.);
        HBOOK1(1202,"Chi2 for indiv. channel",50,0.,5.,0.);
        HBOOK1(1204,"Bin Tin-RMS in Tin-Tout fit",50,0.,10.,0.);
// hist.1600-1711 are booked in init-function for Tin vs Tout correl.!!!(TDLV)
// hist.1720-1790 are booked in init-function for BarRawTime histogr.!!!(TDLV)
        if(TOFCAFFKEY.dynflg==1){ // for special(Contin's) Dynode calibr.
          HBOOK1(1240,"Slope in Td vs Ta correlation",50,0.,2.,0.);
          HBOOK1(1241,"Offset in Td vs Ta correlation",50,-200.,50.,0.);
          HBOOK1(1242,"Chi2 in Td vs Ta correlation",50,0.,5.,0.);
// hist.1800-1911 are booked in init-function for Tin vs Tout correl.!!!
        }
      }
      if(TOFRECFFKEY.relogic[0]==3){ // TZSL-calibration
        HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
        HBOOK1(1501,"Particle beta(tracker)",80,0.9,1.1,0.);
        HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
        HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
        HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
        HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
        HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
        HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
        HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
        HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
        HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
        HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
        HBOOK1(1216,"Cos_tr",50,0.5,1.,0.);
        HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
        HBOOK1(1218,"TOF track-fit chi2-x",50,0.,5.,0.);
        HBOOK1(1219,"TOF track-fit chi2-y",50,0.,5.,0.);
        HBOOK1(1503,"Anticounter energy(4Lx1bar events)(mev)",80,0.,20.,0.);
        HBOOK1(1505,"Qmax ratio",80,0.,16.,0.);
        HBOOK1(1507,"T0-difference inside bar-types 5",80,-0.4,0.4,0.);
        HBOOK2(1502,"Layer-1,T vs exp(-TovT)",50,0.,0.4,50,3.,8.,0.);
        HBOOK2(1514,"Layer-1,T vs SUM(1/Q)",50,0.,0.1,50,3.,8.,0.);
        HBOOK2(1504,"Layer-3,T vs exp(-TovT)",50,0.,0.4,50,-2.,3.,0.);
        HBOOK1(1524,"TRlen13-TRlen24",80,-4.,4.,0.);
//        HBOOK1(1550,"Bar-time(corected),L=1",80,24.,26.,0.);
//        HBOOK1(1551,"Bar-time(corected),L=2",80,23.5,25.5,0.);
//        HBOOK1(1552,"Bar-time(corected),L=3",80,19.5,21.5,0.);
//        HBOOK1(1553,"Bar-time(corected),L=4",80,19.,21.,0.);
      }
      if(TOFRECFFKEY.relogic[0]==4){ // AMPL-calibration
        HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
        HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
        HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
        HBOOK1(1502,"Particle beta(tof)",80,0.7,1.2,0.);
        HBOOK1(1503,"Anticounter energy(4Lx1bar events)(mev)",80,0.,40.,0.);
        HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
        HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
        HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
        HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
        HBOOK1(1204,"Mass",80,0.,1.6,0.);
        HBOOK1(1207,"Mass in working betga-range",80,0.,1.6,0.);
        HBOOK1(1205,"Chisq (tof-beta-fit)",50,0.,10.,0.);
        HBOOK1(1206,"Tzer (tof-beta-fit)",50,-2.5,2.5,0.);
        HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
        HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
        HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
        HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
        HBOOK1(1215,"(Cos_tr-Cos_sc)/Cos_tr",50,-1.,1.,0.);
        HBOOK1(1216,"Cos_tr",50,0.5,1.,0.);
        HBOOK1(1217,"Cos_sc",50,0.5,1.,0.);
        HBOOK2(1218,"TOF-beta vs TRACKER-momentum",80,0.,4.,60,0.5,1.1,0.);
        HBOOK2(1219,"Q(ref.btyp=5) vs Log(beta*gamma)",50,-0.1,1.9,80,0.,240.,0.);
// hist.# 1220-1239 are reserved for imp.point distr.(later in TOFAMPLcalib.init()
        HBOOK2(1248,"Cnannel-2  D-signal vs A-signal",80,0.,320.,80,0.,160.,0.);
        HBOOK2(1249,"Ref.bar(type=5) A2D-ratio vs A-signal",80,0.,320.,50,0.,10.,0.);
        HBOOK1(1250,"Ref.bar(type=5) Q-distr.(s=1,centre)",80,0.,160.,0.);        
        HBOOK1(1251,"Ref.bar(type=5) Q-distr.(s=2,centre)",80,0.,240.,0.);
        HBOOK1(1252,"Relative anode-gains(all channels)",80,0.5,2.,0.);
        HBOOK1(1254,"Ref.bar A-profile (type-1)",70,-70.,70.,0.);        
        HBOOK1(1255,"Ref.bar A-profile (type-2)",70,-70.,70.,0.);        
        HBOOK1(1256,"Ref.bar A-profile (type-3)",70,-70.,70.,0.);        
        HBOOK1(1257,"Ref.bar A-profile (type-4)",70,-70.,70.,0.);        
        HBOOK1(1258,"Ref.bar A-profile (type-5)",70,-70.,70.,0.);        
        HBOOK1(1259,"Anode_to_Dinode signal ratio(all channels)",80,9.,11.,0.);
        HBOOK1(1260,"Anode_to_Dinode ratio error(all channels)",80,0.,0.4,0.);
      }
      if(TOFRECFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
        for(il=0;il<SCLRS;il++){
          for(ib=0;ib<SCMXBR;ib++){
            for(i=0;i<2;i++){
              strcpy(htit1,"FTDC/STDC/ATDC/DTDC multipl. for chan(LBBS) ");
              in[0]=inum[il+1];
              strcat(htit1,in);
              ii=(ib+1)/10;
              jj=(ib+1)%10;
              in[0]=inum[ii];
              strcat(htit1,in);
              in[0]=inum[jj];
              strcat(htit1,in);
              in[0]=inum[i+1];
              strcat(htit1,in);
              ich=2*SCMXBR*il+2*ib+i;
              HBOOK1(1300+ich,htit1,80,0.,80.,0.);
            }
          }
        }
      }
    }
//-----------
// ===> Clear JOB-statistics counters for SIM/REC :
//
    TOFJobStat::clear();
//
  if(TOFRECFFKEY.ReadConstFiles){// Constants will be taken from ext.files
//
//-----------
// ===> create common parameters (tofvpar structure) fr.data-cards :
//
    tofvpar.init(TOFRECFFKEY.daqthr, TOFRECFFKEY.cuts);//daqthr/cuts reading
//
//-----------
//     ===> create indiv. sc.bar scmcscan-objects (MC t/eff-distributions) 
//                                                          from ext. files :
    AMSTOFScan::build();
//
//-------------------------
// ===> create indiv. sc.bar parameters (sc.bar scbrcal-objects) fr.ext.files:
//
    TOFBrcal::build();
  }
  else{ // Constants will be taken from DB (TDV)
    TOFRECFFKEY.year[1]=TOFRECFFKEY.year[0]-1;    
  }
// 
//-----------
  AMSTOFCluster::init();
}
//===================================================================
void AMSJob::_reantiinitjob(){
//
    AMSgObj::BookTimer.book("REANTIEVENT");
    if(ANTIRECFFKEY.reprtf[0]>0){
      HBOOK1(2500,"ANTI_counters total  energy(reco,Mev)",80,0.,20.,0.);
      HBOOK1(2501,"ANTI_bar: T_tdca-T_tdct(reco,ns)",80,-20.,220.,0.);
    }
//
    ANTIJobStat::clear();// Clear JOB-statistics counters for SIM/REC
//-----------
    ANTIPcal::build();//create calibr.objects(antisccal-objects for each sector) 
}
//===================================================================
void AMSJob::_rectcinitjob(){
AMSgObj::BookTimer.book("RECTCEVENT");
// AMSCTCRawHit::init();
  if(CTCRECFFKEY.reprtf[0]>0){
    HBOOK1(3000,"CTC total RECO raw signal(p.e.)",80,0.,800.,0.);
    HBOOK1(3001,"CTC_channel: T_tdca-Ttdct (reco,ns)",80,-20.,220.,0.);
  }
// ===> Clear JOB-statistics counters for SIM/REC :
//
    CTCJobStat::clear();
//
// ===> create ctcfcal-objects(TDV) for each CTC cells group (combination) :
//
    CTCCCcal::build();
}

void AMSJob::_reaxinitjob(){
AMSgObj::BookTimer.book("REAXEVENT");
AMSCharge::init();
}

void AMSJob::_retrdinitjob(){
}

AMSgvolume * AMSJob::getgeom(AMSID id){
  if(id.getname() ==0 && id.getid()==0)id=AMSID(AMSDBc::ams_name,1);
  return (AMSgvolume*)AMSJob::JobMap.getp(id);
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

void AMSJob::_timeinitjob(){
static AMSTimeID TID(AMSID("TDV:",0));
gethead()->addup( &TID);
//
// Magnetic Field Map
//
{
tm begin;
tm end;
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
   begin,end,sizeof(TKFIELD_DEF),(void*)&TKFIELD));
}
//----------------------------
//
// Pedestals, Gains,  Sigmas & commons noise for tracker
//      

{
tm begin;
tm end;
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


TID.add (new AMSTimeID(AMSID("TrackerPedestals.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numell,
   (void*)AMSTrIdSoft::peds));
TID.add (new AMSTimeID(AMSID("TrackerPedestals.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::peds[0])*
   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::peds+AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerRawSigmas.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*AMSTrIdSoft::_numell,
   (void*)AMSTrIdSoft::sigmaraws));
TID.add (new AMSTimeID(AMSID("TrackerRawSigmas.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::sigmaraws[0])*
   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::sigmaraws+AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerGains.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numell,
   (void*)AMSTrIdSoft::gains));
TID.add (new AMSTimeID(AMSID("TrackerGains.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::gains[0])*
   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::gains+AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerSigmas.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numell,
   (void*)AMSTrIdSoft::sigmas));
TID.add (new AMSTimeID(AMSID("TrackerSigmas.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*
   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::sigmas+AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerStatus.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numell,
   (void*)AMSTrIdSoft::status));
TID.add (new AMSTimeID(AMSID("TrackerStatus.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::status[0])*
   (AMSTrIdSoft::_numel-AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::status+AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.l",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*AMSTrIdSoft::_numell*2,
   (void*)AMSTrIdSoft::rhomatrix));
TID.add (new AMSTimeID(AMSID("TrackerRhoMatrix.r",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::rhomatrix[0])*
   (2*AMSTrIdSoft::_numel-2*AMSTrIdSoft::_numell),
   (void*)(AMSTrIdSoft::rhomatrix+2*AMSTrIdSoft::_numell)));
TID.add (new AMSTimeID(AMSID("TrackerCmnNoise",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::cmnnoise),
   (void*)AMSTrIdSoft::cmnnoise));
//TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
//   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
//   (void*)AMSTrIdSoft::indnoise));
}






//---------------------------------------
//
//   TOF : calibration parameters for all sc.bars
//
{
tm begin;
tm end;
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
   begin,end,SCBLMX*sizeof(scbrcal[0][0]),
   (void*)&scbrcal[0][0]));
   
TID.add (new AMSTimeID(AMSID("Tofvpar",isRealData()),
   begin,end,sizeof(TOFVarp),
   (void*)&tofvpar));
   
TID.add (new AMSTimeID(AMSID("Tofmcscans",isRealData()),
   begin,end,SCBLMX*sizeof(scmcscan[0]),
   (void*)&scmcscan[0]));
}
//---------------------------------------
//
//   ANTI : calibration parameters for all sc.sectors
//
{
tm begin;
tm end;
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
   begin,end,MAXANTI*sizeof(antisccal[0]),
   (void*)&antisccal[0]));
   
//TID.add (new AMSTimeID(AMSID("Antivpar",isRealData()),
//   begin,end,sizeof(ANTIVarp),
//   (void*)&antivpar));
   
}
//-----------------------------------------
//
//   CTC : calibration parameters for all combinations
//
{
tm begin;
tm end;
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
   (void*)&ctcfcal[0]));
}
//---------------------------------------
//
// Data to fit particle charge magnitude
//
{
tm begin;
tm end;
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



TID.add (new AMSTimeID(AMSID("ChargeLkhd1",isRealData()),
   begin,end,100*ncharge*sizeof(AMSCharge::_lkhdTracker[0][0]),
   (void*)AMSCharge::_lkhdTracker[0]));
TID.add (new AMSTimeID(AMSID("ChargeLkhd2",isRealData()),
   begin,end,100*ncharge*sizeof(AMSCharge::_lkhdTOF[0][0]),
   (void*)AMSCharge::_lkhdTOF[0]));
TID.add (new AMSTimeID(AMSID("ChargeLkhd3",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_lkhdStepTOF[0]),
   (void*)AMSCharge::_lkhdStepTOF));
TID.add (new AMSTimeID(AMSID("ChargeLkhd4",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_lkhdStepTracker[0]),
   (void*)AMSCharge::_lkhdStepTracker));
TID.add (new AMSTimeID(AMSID("ChargeLkhd5",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_chargeTOF[0]),
   (void*)AMSCharge::_chargeTOF));
TID.add (new AMSTimeID(AMSID("ChargeLkhd6",isRealData()),
   begin,end,ncharge*sizeof(AMSCharge::_chargeTracker[0]),
   (void*)AMSCharge::_chargeTracker));


}

if (AMSFFKEY.Update){

#ifdef __DB__
  if (AMSFFKEY.Write == 0 ) {
#endif
  // Here update dbase

    AMSTimeID * offspring=(AMSTimeID*)TID.down();
    while(offspring){
    if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatadir))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
    offspring=(AMSTimeID*)offspring->next();
    }
#ifdef __DB__
  } 
#endif  
}
}


AMSTimeID * AMSJob::gettimestructure(){
      AMSID id("TDV:",0);    
     AMSNode *p=JobMap.getp(id);
     if(!p){
      cerr << "AMSJob::gettimestructe-F-no time structre found"<<endl;
      exit(1);
     }
     else return  (AMSTimeID*)p;
}

AMSTimeID * AMSJob::gettimestructure(const AMSID & id){
     AMSNode *p=JobMap.getp(id);
     if(!p){
       cerr << "AMSJob::gettimestructe-F-no time structre found "<<id<<endl;
      exit(1);
     }
     else return  (AMSTimeID*)p;
}


AMSJob::~AMSJob(){
  cout << "~AMSJob called "<<endl;
_tkendjob();
_ctcendjob();
_tofendjob();
_antiendjob();
_trdendjob();
_dbendjob();
_axendjob();

if(IOPA.hlun){
  cout <<"Trying to close ntuple ... "<<endl;
        char hpawc[256]="//PAWC";
        HCDIR (hpawc, " ");
        char houtput[9]="//output";
        HCDIR (houtput, " ");
        integer ICYCL=0;
        HROUT (0, ICYCL, " ");
        HREND ("output");
        CLOSEF(IOPA.hlun);
}


}


void AMSJob::uhinit(integer pass){

  
  if(IOPA.hlun){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    integer iostat;
    integer rsize=1024;
    HROPEN(IOPA.hlun,"output",hfile,"N",rsize,iostat);
    if(iostat){
     cerr << "Error opening Histo file "<<hfile<<endl;
     exit(1);
    }
    else cout <<"Histo file opened."<<endl;
  }
   HBOOK1(200101,"Number of Nois Hits x",100,-0.5,99.5,0.);
   HBOOK1(200102,"Number of Nois Hits y",100,-0.5,99.5,0.);
   HBOOK1(200103,"Normal Spectrum  x",200,-50.5,49.5,0.);
   HBOOK1(200104,"Normal Spectrum y",200,-50.5,49.5,0.);
   HBOOK1(200105,"Above threshold spectrum x",200,-0.5,49.5,0.);
   HBOOK1(200106,"Above threshold spectrum y",200,-0.5,49.5,0.);
   HBNT(IOPA.ntuple,"Simulation"," ");

}


void AMSJob::_tkendjob(){

  if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check(1);
   
  }
  AMSTrIdCalib::printbadchanlist();
    

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
  int i,j,k,ich;
//--------> some TOF stuff :
       if(isSimulation() && TOFMCFFKEY.mcprtf[2]!=0 && TOFMCFFKEY.fast==0){ // print MC-hists
         HPRINT(1050);
         HPRINT(1051);
         HPRINT(1052);
         HPRINT(1053);
         HPRINT(1060);
         HPRINT(1061);
         HPRINT(1062);
         HPRINT(1063);
         HPRINT(1070);
         HPRINT(1071);
         HPRINT(1072);
         HPRINT(1073);
         HPRINT(1074);
         HPRINT(1075);
       }
       if(TOFRECFFKEY.reprtf[2]!=0){ // print RECO-hists
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1541);
         HPRINT(1542);
         if(isRealData() || (!isRealData() && TOFMCFFKEY.fast==0)){
           HPRINT(1100);
           HPRINT(1115);
           HPRINT(1105);
           HPRINT(1106);
           HPRINT(1107);
           HPRINT(1108);
           HPRINT(1101);
           HPRINT(1102);
           HPRINT(1103);
           HPRINT(1104);
           HPRINT(1110);
           HPRINT(1111);
           HPRINT(1112);
           HPRINT(1113);
           HPRINT(1114);
       HPRINT(1130);
       HPRINT(1131);
       HPRINT(1132);
       HPRINT(1133);
       HPRINT(1134);
       HPRINT(1135);
           HPRINT(1526);
           HPRINT(1527);
           HPRINT(1528);
           HPRINT(1529);
           HPRINT(1530);
           HPRINT(1531);
           HPRINT(1532);
           HPRINT(1533);
           HPRINT(1543);
           HPRINT(1544);
           HPRINT(1534);
           HPRINT(1545);
         }
         if(TOFRECFFKEY.reprtf[3]!=0){//TDC-hit multiplicity histograms
           for(i=0;i<SCLRS;i++){
             for(j=0;j<SCMXBR;j++){
               for(k=0;k<2;k++){
                 ich=2*SCMXBR*i+2*j+k;
                 HPRINT(1300+ich);
               }
             }
           }
         }
       }
// ---> calibration specific :
       if(TOFRECFFKEY.relogic[0]==3){// for TZSL-calibr. runs
         HPRINT(1500);
         HPRINT(1501);
         HPRINT(1506);
         if(isRealData() || (!isRealData() && TOFMCFFKEY.fast==0)){
           HPRINT(1502);
           HPRINT(1514);
           HPRINT(1504);
           HPRINT(1503);
           HPRINT(1505);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1216);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1524);
//           HPRINT(1550);
//           HPRINT(1551);
//           HPRINT(1552);
//           HPRINT(1553);
           TOFTZSLcalib::mfit();
           HPRINT(1507);
         }
       }
//
       if(TOFRECFFKEY.relogic[0]==4){// for AMPL-calibr. runs
         if(isRealData() || (!isRealData() && TOFMCFFKEY.fast==0)){
           HPRINT(1506);
           HPRINT(1500);
           HPRINT(1501);
           HPRINT(1502);
           HPRINT(1205);
           HPRINT(1206);
           HPRINT(1503);
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1203);
           HPRINT(1210);
           HPRINT(1211);
           HPRINT(1212);
           HPRINT(1213);
           HPRINT(1215);
           HPRINT(1216);
           HPRINT(1217);
           HPRINT(1218);
           HPRINT(1219);
           HPRINT(1204);
           HPRINT(1207);
           HPRINT(1248);
           HPRINT(1249);
           HPRINT(1250);
           HPRINT(1251);
           TOFAMPLcalib::fit();
           HPRINT(1259);
           HPRINT(1260);
           HPRINT(1252);
           HPRINT(1254);
           HPRINT(1255);
           HPRINT(1256);
           HPRINT(1257);
           HPRINT(1258);
         }
       }
//
//
       if(TOFRECFFKEY.relogic[0]==1){// for STRR-calibr. runs
         if(isRealData() || (!isRealData() && TOFMCFFKEY.fast==0)){//for Real/Mc-SLOW
           TOFSTRRcalib::outp();
           HPRINT(1200);
           HPRINT(1201);
           HPRINT(1202);
           HPRINT(1204);
           if(TOFCAFFKEY.dynflg==1){
             TOFAVSDcalib::fit();
             HPRINT(1240);
             HPRINT(1241);
             HPRINT(1242);
           }
         }
       }
//
       if(TOFRECFFKEY.relogic[0]==2){// for TDIF-calibr. runs
         if(isRealData() || (!isRealData() && TOFMCFFKEY.fast==0)){//for Real/Mc-SLOW
           TOFTDIFcalib::fit();
         }
       }
       TOFJobStat::print(); // Print JOB-TOF statistics
}
//-----------------------------------------------------------------------
void AMSJob::_antiendjob(){
//
  ANTIJobStat::print(); // Print JOB-ANTI statistics
  if(isSimulation() && ANTIMCFFKEY.mcprtf>0){
    HPRINT(2000);
  }
  if(ANTIRECFFKEY.reprtf[0]){
    HPRINT(2500);
    HPRINT(2501);
  }
}
//-----------------------------------------------------------------------
void AMSJob::_trdendjob(){

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

    AMSUser::EndJob(); 
}

void AMSJob::_dbendjob()
{
#ifdef __DB__
  if (AMSEvent::_checkUpdate() == 1) {
   Message("AMSJob::_dbendjob -I- UpdateMe is set. Update database and tables.");
   int rstatus = lms -> AddAllTDV();
  } else {
   Message("AMSJob::_dbendjob -I- UpdateMe != 1. NO UPDATE");
  }
#endif
} 



void AMSJob::_setorbit(){

 const number MIR=51.65;
  tm Begin;
  integer begindate=CCFFKEY.begindate;
  integer begintime=CCFFKEY.begintime;
  Begin.tm_year  =  begindate%10000-1900;
  Begin.tm_mon = (begindate/10000)%100-1;
  Begin.tm_mday   = (begindate/1000000)%100;
  Begin.tm_hour  = (begintime/10000)%100;
  Begin.tm_min= (begintime/100)%100;
  Begin.tm_sec=(begintime)%100;
  Begin.tm_isdst =  Begin.tm_mon>=3 &&  Begin.tm_mon<=8;
  Orbit.Begin=mktime(&Begin);
  Orbit.ThetaI=CCFFKEY.theta/AMSDBc::raddeg;
  Orbit.PhiI=fmod(CCFFKEY.phi/AMSDBc::raddeg+AMSDBc::twopi,AMSDBc::twopi);
  Orbit.AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  number r= tan(Orbit.ThetaI)/Orbit.AlphaTanThetaMax;
  if(r > 1 || r < -1){
    cerr <<"AMSJob::setorbit-E-ThetaI too high "<<Orbit.ThetaI<<endl;
    if(Orbit.ThetaI < 0 )Orbit.ThetaI = -MIR/AMSDBc::raddeg;
    else Orbit.ThetaI= MIR/AMSDBc::raddeg;
    cerr <<"AMSJob::setorbit-ThetaI brought to "<<Orbit.ThetaI<<endl;
    r=tan(Orbit.ThetaI)/Orbit.AlphaTanThetaMax;
  }
  Orbit.PhiZero=Orbit.PhiI-atan2(r,CCFFKEY.sdir*sqrt(1-r*r));         
  Orbit.AlphaSpeed=AMSDBc::twopi/92.36/60.;
  Orbit.EarthSpeed=AMSDBc::twopi/24/3600;
  Orbit.PolePhi=CCFFKEY.polephi/AMSDBc::raddeg;
  Orbit.PoleTheta=78.6/AMSDBc::raddeg;


}


  void AMSJob::_redaqinitjob(){
     AMSgObj::BookTimer.book("SIDAQ");
     AMSgObj::BookTimer.book("REDAQ");
    if(!strstr(AMSJob::gethead()->getsetup(),"AMSSTATION") ){    
     _setorbit();
    // Add subdetectors to daq
    //
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
 
    if(TRMCFFKEY.GenerateConst){
    //Tracker ped/sigma etc ( "Event" mode)

    DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);


    }
  }

}
}
else {

if(DAQCFFKEY.LCrateinDAQ){
//           tracker reduced

    if(DAQCFFKEY.TrFormatInDAQ & 1)
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkdaqid,&AMSTrRawCluster::buildraw);
    DAQEvent::addblocktype(&AMSTrRawCluster::getmaxblocks,
    &AMSTrRawCluster::calcdaqlength,&AMSTrRawCluster::builddaq);



    //Tracker ped/sigma etc ( "Event" mode)

    DAQEvent::addsubdetector(&AMSTrRawCluster::checkpedSRawid,&AMSTrRawCluster::updpedSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checksigSRawid,&AMSTrRawCluster::updsigSRaw);
    DAQEvent::addsubdetector(&AMSTrRawCluster::checkstatusSRawid,&AMSTrRawCluster::updstatusSRaw);


}   


if(DAQCFFKEY.LCrateinDAQ && (DAQCFFKEY.TrFormatInDAQ & 2)){
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
else {
      cerr <<"AMSJob::_redaqinitjob-E-NoYetDAQFormat for "<<
      AMSJob::gethead()->getsetup()<<endl;
}
}

#ifdef __DB__

integer AMSJob::FindTheBestTDV(char* name, integer id, time_t timeV, integer &S, 
                               time_t &I, time_t &B, time_t &E)
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

#ifdef __DB__
  
  if (nobj < 1) {
    cerr <<"AMSJob::FillJobTDV -E- value out of range "<<nobj<<endl;
    return rstatus;
  }
  _tdv = new tdv_time[nobj];
  for (int i=0; i<nobj; i++) { _tdv[i] = tdv[i];}
 
#endif
  return 1;
}
#endif

#ifdef __DB__

integer AMSJob::SetTDVPtrs(integer start[], integer end[])
//
// Set start/end ptrs for TDV table 
//
{
#ifdef __DB__
  for (int i=0; i<ntdvNames; i++) {
   _ptr_start[i] = start[i];
   _ptr_end[i]   = end[i];
  }
#endif
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
