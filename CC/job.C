//
// Author V. Choutko 24-may-1996
// CTC codes added 29-sep-1996 by E.Choumilov 
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
#include <ctcdbc.h>
#include <timeid.h>
#include <mceventg.h>
#include <trcalib.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <tofcalib.h>
#include <tofrec.h>

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
//
void AMSJob::data(){
#ifdef __HPUX__
  AMSTrIdSoft::init();
  AMSCharge::init();
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
IOPA.TriggerI=1;
IOPA.WriteAll=0;
VBLANK(IOPA.TriggerC,40);
VBLANK(AMSFFKEY.TDVC,400);
char amsp[12]="AMSParticle";
UCTOH(amsp,IOPA.TriggerC,4,12);
IOPA.mode=0;
VBLANK(IOPA.ffile,40);
FFKEY("IOPA",(float*)&IOPA,sizeof(IOPA_DEF)/sizeof(integer),"MIXED");

_sitkdata();
_signdata();
_sitofdata();
_sitrddata();
_sictcdata();
}

void AMSJob::_sitkdata(){
TRMCFFKEY.alpha=220;
TRMCFFKEY.beta=1;
TRMCFFKEY.gamma=0.08;
TRMCFFKEY.fastswitch=5.e-5;  // inverse linear density of primary electrons
TRMCFFKEY.dedx2nprel=0.33e6;
TRMCFFKEY.ped[0]=100;
TRMCFFKEY.ped[1]=100;
TRMCFFKEY.gain[0]=1;
TRMCFFKEY.gain[1]=1;
TRMCFFKEY.sigma[1]=110/20/sqrt(3.); // sig/noise ratio is about 20 for y
TRMCFFKEY.sigma[0]=TRMCFFKEY.sigma[1]*1.41;   // x strip two times larger y
TRMCFFKEY.thr[0]=17;      // should be sync with sigma approx 4*sigma
TRMCFFKEY.thr[1]=12;      // ------------------------
TRMCFFKEY.neib[0]=2;
TRMCFFKEY.neib[1]=2;
TRMCFFKEY.cmn[0]=50;
TRMCFFKEY.cmn[1]=50;
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
TRMCFFKEY.year[0]=96;
TRMCFFKEY.year[1]=99;
FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");

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
CCFFKEY.theta=51.65;
CCFFKEY.phi=290.;
CCFFKEY.polephi=290.;
CCFFKEY.begindate=2041998;
CCFFKEY.enddate=  6041998;
CCFFKEY.begintime=80000;
CCFFKEY.endtime=120000;
CCFFKEY.oldformat=0;
CCFFKEY.sdir=1;
FFKEY("MCGEN",(float*)&CCFFKEY,sizeof(CCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=================================================================================
void AMSJob::_sitofdata(){
  char tfname[12]="tdfmap01";//file-name for t_distr.files map(max 11 letters )
  TOFMCFFKEY.TimeSigma=1.1e-10; // time resolution(sec) for simplified algorithm
  TOFMCFFKEY.TimeSigma2=4.5e-10;
  TOFMCFFKEY.TimeProbability2=0.035;
  TOFMCFFKEY.padl=10.5;        // sc. bar transv. step ........................
  TOFMCFFKEY.Thr=0.1;          // Sc.bar Elos-thresh.(Mev) to participate in Reco   
//
  TOFMCFFKEY.mcprtf[0]=0;     // TOF MC print flag for init arrays
  TOFMCFFKEY.mcprtf[1]=0;     // TOF MC print flag for MC pulses
  TOFMCFFKEY.mcprtf[2]=0;     // ...................... histograms
  TOFMCFFKEY.mcprtf[3]=0;     // spare
  TOFMCFFKEY.mcprtf[4]=0;     // spare
  TOFMCFFKEY.trlogic[0]=0; // MC trigger logic flag (=0/1-> two-sides-AND/OR of counter) 
  TOFMCFFKEY.trlogic[1]=0; // spare 
  TOFMCFFKEY.fast=1;
  UCTOH(tfname,TOFMCFFKEY.tdfnam,4,12);
FFKEY("TOFMC",(float*)&TOFMCFFKEY,sizeof(TOFMCFFKEY_DEF)/sizeof(integer),"MIXED");
}
//=================================================================================

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
  CTCGEOMFFKEY.supzc=-89.4;     // supp. honeycomb plate front face Z-pos
  CTCGEOMFFKEY.aegden=0.125;    // aerogel density
  CTCGEOMFFKEY.wlsden=1.03;     // WLS density
  CTCGEOMFFKEY.nblk=1;          // number of aer. blocks (X-div.)(=1 for solid)
  CTCGEOMFFKEY.nwls=1;          // number of wls blocks
  CTCGEOMFFKEY.ptfx=0.025;      // PTFE thickness in cm
  CTCGEOMFFKEY.thcsize[0]=150.; //
  CTCGEOMFFKEY.thcsize[1]=110.; // Support structure dimensions (cm)
  CTCGEOMFFKEY.thcsize[2]=6.;   //
  CTCGEOMFFKEY.upper[0]=116.2;  //
  CTCGEOMFFKEY.upper[1]=92.2;   // Upper/Lower layer dimensions (cm)
  CTCGEOMFFKEY.upper[2]=15.50;  //
  CTCGEOMFFKEY.wall[0]=0.2;     //
  CTCGEOMFFKEY.wall[1]=92.2;    // Wall dimensions (cm)
  CTCGEOMFFKEY.wall[2]=7.175;   //
  CTCGEOMFFKEY.cell[0]=23.0;    //
  CTCGEOMFFKEY.cell[1]=23.0;    // Super cell dimensions (cm)
  CTCGEOMFFKEY.cell[2]=15.50;   //
  CTCGEOMFFKEY.ygap[0]=23.0;    // 
  CTCGEOMFFKEY.ygap[1]=0.2;     // Gap dimensions (cm)
  CTCGEOMFFKEY.ygap[2]=15.50;   //
  CTCGEOMFFKEY.agel[0]=11.35;   //
  CTCGEOMFFKEY.agel[1]=11.35;   // AeroGEL dimensions (cm)
  CTCGEOMFFKEY.agel[2]=7.025;   //
  CTCGEOMFFKEY.pmt[0]=3.0;      //
  CTCGEOMFFKEY.pmt[1]=3.0;      // Phototube dimensions (including housing
  CTCGEOMFFKEY.pmt[2]=7.5;      //          (and shielding)
  CTCGEOMFFKEY.ptfe[0]=11.50;   //
  CTCGEOMFFKEY.ptfe[1]=11.50;   // PTFE box dimensions (cm)
  CTCGEOMFFKEY.ptfe[2]=7.175;   //
  CTCGEOMFFKEY.xdiv=5;
  CTCGEOMFFKEY.ydiv=4;
  CTCMCFFKEY.Refraction[0]=1.03;   // Refraction indexes
  CTCMCFFKEY.Refraction[1]=1.58;
  CTCMCFFKEY.Path2PhEl[0]=25;   // Path to photoelectrons conv fact
  CTCMCFFKEY.Path2PhEl[1]=28;
  CTCMCFFKEY.AbsLength[0]=15;   // Abs Length in cm  
  CTCMCFFKEY.AbsLength[1]=100;
  CTCMCFFKEY.Edep2Phel[0]=0;      // Agel is not a scint
  CTCMCFFKEY.Edep2Phel[1]=184e3;  // WLS is scint if vertical readout

FFKEY("CTCGEOM",(float*)&CTCGEOMFFKEY,sizeof(CTCGEOMFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CTCMC",(float*)&CTCMCFFKEY,sizeof(CTCMCFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_sitrddata(){
}

void AMSJob:: _reamsdata(){
_retkdata();
_retofdata();
_retrddata();
_rectcdata();
_reaxdata();

}

void AMSJob::_retkdata(){

TRCLFFKEY.ThrClA[1]=45;
TRCLFFKEY.Thr1A[1] =25;
TRCLFFKEY.Thr2A[1] =7;

TRCLFFKEY.ThrClS[1]=10;
TRCLFFKEY.Thr1S[1] =7;
TRCLFFKEY.Thr2S[1] =7;

TRCLFFKEY.ThrClR[1]=8;
TRCLFFKEY.Thr1R[1] =5;
TRCLFFKEY.Thr2R[1] =1.;  // should be around 1 if ThrClNEl[1]=3;
TRCLFFKEY.Thr3R[1] =-2.;

TRCLFFKEY.ThrClNMin[1]=2;
TRCLFFKEY.ThrClNEl[1]=3;

TRCLFFKEY.ThrClA[0]=45;
TRCLFFKEY.Thr1A[0] =30;
TRCLFFKEY.Thr2A[0] =6;

TRCLFFKEY.ThrClS[0]=14;
TRCLFFKEY.Thr1S[0] =10;
TRCLFFKEY.Thr2S[0] =10;

TRCLFFKEY.ThrClR[0]=7;
TRCLFFKEY.Thr1R[0] =6;
TRCLFFKEY.Thr2R[0] =1.;
TRCLFFKEY.Thr3R[0] =-2.;

TRCLFFKEY.ThrClNMin[0]=1;
TRCLFFKEY.ThrClNEl[0]=3;

TRCLFFKEY.ErrX=30.e-4;
TRCLFFKEY.ErrY=10.e-4;
TRCLFFKEY.ErrZ=30.e-4;
TRCLFFKEY.ThrDSide=1.;

TRCLFFKEY.CorFunParA[0][0]=400e-4;
TRCLFFKEY.CorFunParB[0][0]=0.85;
TRCLFFKEY.CorFunParA[0][5]=400e-4;
TRCLFFKEY.CorFunParB[0][5]=0.85;
int k;
for (k=1;k<5;k++){
 TRCLFFKEY.CorFunParA[0][k]=400e-4;
 TRCLFFKEY.CorFunParB[0][k]=0.85;
}
for ( k=0;k<6;k++){
 TRCLFFKEY.CorFunParA[1][k]=65e-4;
 TRCLFFKEY.CorFunParB[1][k]=0.5;
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
TRFITFFKEY.UseTOF=1;
TRFITFFKEY.Chi2FastFit=10;
TRFITFFKEY.Chi2StrLine=20;
TRFITFFKEY.Chi2Circle=20;
TRFITFFKEY.ResCutFastFit=0.5;
TRFITFFKEY.ResCutStrLine=0.5;
TRFITFFKEY.ResCutCircle=0.5;
TRFITFFKEY.SearchRegFastFit=1;
TRFITFFKEY.SearchRegStrLine=0.5;
TRFITFFKEY.SearchRegCircle=1;
TRFITFFKEY.RidgidityMin=0.2;
TRFITFFKEY.FullReco=0;
TRFITFFKEY.MinRefitCos[0]=0.7;
TRFITFFKEY.MinRefitCos[1]=0.5;
TRFITFFKEY.FastTracking=0;
FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
TKFINI();
}
void AMSJob::_retofdata(){
  char cfname[12]="geomconf";//generic geomconfig-file name (max 11 letters)
//                          (version #01/02-> shuttle/Alpha will be added autom.)
  char tzslsr[12]="tzcalib01";//generic t0,slope,str-ratio file-name(max.11 lett)
//                           (mc/rl->MC/Real will be added automatically)
// 
  TOFRECFFKEY.Thr1=0.45;// Threshold (mev) on peak bar energy
  TOFRECFFKEY.ThrS=0.9; // Threshold (mev) on total cluster energy
//
  TOFRECFFKEY.reprtf[0]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[1]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[2]=0; // RECO print flag for histograms
  TOFRECFFKEY.reprtf[3]=0; // RECO print flag 
  TOFRECFFKEY.reprtf[4]=0; // RECO print flag
//
  TOFRECFFKEY.relogic[0]=0;// 0/1 -> normal/calibr. run. 
  TOFRECFFKEY.relogic[1]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[2]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[3]=0;// RECO logic flag 
  TOFRECFFKEY.relogic[4]=0;// RECO logic flag
//
  TOFRECFFKEY.daqthr[0]=40.;//thresh(mV) for discr. of "z>=1"-trig (fast/slow_TDC) 
  TOFRECFFKEY.daqthr[1]=100.;//thresh(mV) for discr. of "z>1"-trig  
  TOFRECFFKEY.daqthr[2]=200.;//thresh(mV) for discr. of "z>2"-trig  
  TOFRECFFKEY.daqthr[3]=10.;//thresh(pC) for anode Time_over_Thresh. discr.  
  TOFRECFFKEY.daqthr[4]=10.;//thresh(pC) for dinode Time_over_Thresh. discr.
//
  TOFRECFFKEY.cuts[0]=5.;//t-window(ns) for "the same hit" search in f/s_tdc
  TOFRECFFKEY.cuts[1]=50.;//"befor"-cut in time history (ns)
  TOFRECFFKEY.cuts[2]=150.;//"after"-cut in time history (ns)
  TOFRECFFKEY.cuts[3]=0.;// spare
  TOFRECFFKEY.cuts[4]=0.;
  TOFRECFFKEY.cuts[5]=0.;
  TOFRECFFKEY.cuts[6]=0.;
  TOFRECFFKEY.cuts[7]=0.;
  TOFRECFFKEY.cuts[8]=0.;
  TOFRECFFKEY.cuts[9]=0.;
//
  UCTOH(cfname,TOFRECFFKEY.config,4,12);
//
  UCTOH(tzslsr,TOFRECFFKEY.tzerca,4,12);
//  
  TOFRECFFKEY.sec[0]=0; 
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
  FFKEY("TOFREC",(float*)&TOFRECFFKEY,sizeof(TOFRECFFKEY_DEF)/sizeof(integer),"MIXED");
//-----------------------
//    defaults for calibration:
//
  TOFCAFFKEY.pcut[0]=8.;// track mom. low limit (gev/c)
  TOFCAFFKEY.pcut[1]=25.;// track mom. high limit
  TOFCAFFKEY.bmean=0.996;// mean prot. velocity 
  TOFCAFFKEY.tzref[0]=6.;// T0 for ref. counters
  TOFCAFFKEY.tzref[1]=6.;// T0 for ref. counters
  TOFCAFFKEY.fixsl=3.2;// def. slope
  TOFCAFFKEY.fixstr=0.025;// def. inv. stratcher ratio
  TOFCAFFKEY.idref[0]=108;//LBB for first ref. counter (if non zero)
  TOFCAFFKEY.idref[1]=0;//LBB for second ref. counter (if nonzero)
  TOFCAFFKEY.ifsl=1;// 0/1 to fix/release slope param.
  TOFCAFFKEY.ifstr=0;// 0/1 to fix/release str.ratio param.
  FFKEY("TOFCA",(float*)&TOFCAFFKEY,sizeof(TOFCAFFKEY_DEF)/sizeof(integer),"MIXED");
}
void AMSJob::_rectcdata(){
  CTCRECFFKEY.Thr1=1.;
  CTCRECFFKEY.ThrS=1.;
  FFKEY("CTCREC",(float*)&CTCRECFFKEY,sizeof(CTCRECFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_reaxdata(){
// Fit beta & charge
CHARGEFITFFKEY.Thr=1.;
BETAFITFFKEY.pattern[0]=1;
BETAFITFFKEY.pattern[1]=1;
BETAFITFFKEY.pattern[2]=1;
BETAFITFFKEY.pattern[3]=1;
BETAFITFFKEY.pattern[4]=1;
BETAFITFFKEY.pattern[5]=0;
BETAFITFFKEY.pattern[6]=0;
BETAFITFFKEY.pattern[7]=0;
BETAFITFFKEY.pattern[8]=0;
BETAFITFFKEY.Chi2=10;
BETAFITFFKEY.SearchReg[0]=3.;
BETAFITFFKEY.SearchReg[1]=3.;
BETAFITFFKEY.SearchReg[2]=3.;
FFKEY("BETAFIT",(float*)&BETAFITFFKEY,sizeof(BETAFITFFKEY_DEF)/sizeof(integer),"MIXED");
FFKEY("CHARGEFIT",(float*)&CHARGEFITFFKEY,sizeof(CHARGEFITFFKEY_DEF)/sizeof(integer),"MIXED");
}

void AMSJob::_retrddata(){
}



void AMSJob::udata(){
char jobname[160];
char setupname[160];
char triggername[160];
char tdvname[1600];
char ffile[160];
UHTOC(AMSFFKEY.Jobname,40,jobname,160);
UHTOC(AMSFFKEY.Setupname,40,setupname,160);
UHTOC(IOPA.ffile,40,ffile,160);
UHTOC(IOPA.TriggerC,40,triggername,160);
jobname[159]='\0';
setupname[159]='\0';
triggername[159]='\0';
ffile[159]='\0';
int i;

//+
for (i=158; i>0; i--) {        // should be at least 1 char
 if(jobname[i] == ' ') jobname[i]='\0';
 else break;
}
for (i=158; i>=0; i--) {
 if(setupname[i] == ' ') setupname[i]='\0';
 else break;
}
for (i=158; i>=0; i--) {
 if(ffile[i] == ' ') ffile[i]='\0';
 else break;
}
if(IOPA.mode){
 AMSIO::setfile(ffile);
 AMSIO::init(IOPA.mode);
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
int len=160;

for(i=158;i>=0;i--){
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
       if(i-nold>0)settrigger(triggername+nold,ntrig++,IOPA.TriggerI,or);
       nold=i+1;
  }
}
}
_jobtype=setjobtype(AMSFFKEY.Jobtype%10 != 0);
_jobtype=setjobtype((AMSFFKEY.Jobtype/10)%10 != 0);
uinteger ical=(AMSFFKEY.Jobtype/100)%10;
uinteger ucal=1;
if(ical)_jobtype=setjobtype(ucal<<(ical+1));

//
// Read/Write Synchronization
if(AMSFFKEY.Read > 10 && AMSFFKEY.Read%2==0)AMSFFKEY.Read++;
if(AMSFFKEY.Write > 0 && AMSFFKEY.Write%2==0)AMSFFKEY.Write++;

// TDV
if(AMSFFKEY.Update){
 UHTOC(AMSFFKEY.TDVC,400,tdvname,1600);
 tdvname[1599]='\0';
 {
 int len=1600;
 for(i=1598;i>=0;i--){
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
}


void AMSJob::init(){
if(isSimulation())_siamsinitjob();
_reamsinitjob();
if(isCalibration())_caamsinitjob();
_timeinitjob();
cout << *this;
}
void AMSJob::_siamsinitjob(){
  AMSEvent::debug=AMSFFKEY.Debug;
  _sitkinitjob();
  _signinitjob();
  _sitofinitjob();
  _sitrdinitjob();
  _sictcinitjob();
}

void AMSJob::_sitkinitjob(){
     for(int l=0;l<2;l++){
       for (int i=0;i<AMSDBc::nlay();i++){
         for (int j=0;j<AMSDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l);
            number oldone=0;
            for(int k=0;k<AMSDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             id.setped()=TRMCFFKEY.ped[l];
             id.setstatus(0);
             id.setsig()=TRMCFFKEY.sigma[l];
             id.setgain()=TRMCFFKEY.gain[l];
             id.setcmnnoise()=TRMCFFKEY.cmn[l];
             id.setindnoise()=oldone+
             AMSTrMCCluster::sitknoiseprob(id ,TRMCFFKEY.thr[l]);
             oldone=id.getindnoise();
            }
           }
         }
       }
     }

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
void AMSJob::_sitofinitjob(){
  if(TOFMCFFKEY.fast==1)cout <<"_sitofinit-I-Fast/Crude TOF simulation algorithm selected."<<endl;
  else cout <<"_sitofinit-I-Slow/Accurate TOF simulation algorithm selected."<<endl;
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
      HBOOK1(1070,"Log(PulseTotCharge(pC)),Sd-1,L-1",50,0.,10.,0.);
      HBOOK1(1071,"Total bar pulse-charge(pC),L-1",80,100.,1700.,0.);
      HBOOK1(1072,"Total bar pulse-charge(pC),L-1",80,1000.,17000.,0.);
    }
//------------------------------------------------
    AMSTOFScan::build();// create scmcscan-objects for all sc. bars,
                        // using MC t/eff-distributions from ext. files
}
void AMSJob::_sictcinitjob(){
     AMSgObj::BookTimer.book("SICTCDIGI");
     AMSCTCRawCluster::init();
}

void AMSJob::_sitrdinitjob(){
}


void AMSJob::_reamsinitjob(){
_retkinitjob();
_retofinitjob();
_retrdinitjob();
_rectcinitjob();
_reaxinitjob();
}
void AMSJob::_caamsinitjob(){
if(isCalibration() & CTracker)_catkinitjob();
if(isCalibration() & CTOF)_catofinitjob();
if(isCalibration() & CTRD)_catrdinitjob();
if(isCalibration() & CCerenkov)_cactcinitjob();
if(isCalibration() & CAMS)_caaxinitjob();
}

void AMSJob::_catkinitjob(){
AMSgObj::BookTimer.book("CalTrFill");
AMSgObj::BookTimer.book("CalTrFit");
int i,j;
for(i=0;i<2;i++){
  for(j=0;j<tkcalpat;j++){
    AMSTrCalibFit::setHead(i,j, new 
    AMSTrCalibFit(j,TRCALIB.EventsPerIteration[i],TRCALIB.NumberOfIterations[i],i));
  }
}

}
//---------------------------------------------------------------------
void AMSJob::_catofinitjob(){
 if(TOFRECFFKEY.relogic[0]==1){
   TOFTZSLcalib::init();// TOF TzSl-calibr.
   cout<<"TOFTZSLcalib-init done !"<<'\n';
 }
}
//---------------------------------------------------------------------
void AMSJob::_catrdinitjob(){
}

void AMSJob::_cactcinitjob(){
}

void AMSJob::_caaxinitjob(){
}

void AMSJob::_retkinitjob(){
AMSgObj::BookTimer.book("RETKEVENT");
AMSgObj::BookTimer.book("TrCluster");
AMSgObj::BookTimer.book("TrClusterRefit");
AMSgObj::BookTimer.book("TrRecHit");
AMSgObj::BookTimer.book("TrTrack");
}
void AMSJob::_retofinitjob(){
    AMSgObj::BookTimer.book("RETOFEVENT");
    AMSgObj::BookTimer.book("TOF:DAQ->RwEv");
    AMSgObj::BookTimer.book("TOF:RwEv->RwCl");
    AMSgObj::BookTimer.book("TOF:RwCl->Cl");
    if(TOFRECFFKEY.reprtf[2]!=0){ // Book reco-hist
      HBOOK1(1100,"The same hit (Tf-Ts),(ns)",80,-40.,40.,0.);
      HBOOK1(1101,"Time_history:befor_hit dist(ns)",80,0.,160.,0.);
      HBOOK1(1102,"Time_history:after_hit dist(ns)",80,0.,160.,0.);
      HBOOK1(1529,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1526,"L=1,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1530,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1527,"L=3,Edep_anode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1531,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,24.,0.);
      HBOOK1(1528,"L=1,Edep_dinode(mev),corr,ideal evnt",80,0.,240.,0.);
      HBOOK1(1532,"(T1-T3)(ns),corr,ideal evnt",50,3.,6.,0.);
      HBOOK1(1533,"L=1,side1/2 Tdiff(ns),ideal evnt",100,-4.,0.,0.);
      HBOOK1(1543,"L=1,longit.coordinate,ideal evnt",100,-50.,50.,0.);
      HBOOK1(1534,"(T2-T4)(ns),corr,ideal evnt",50,3.,6.,0.);
      HBOOK1(1535,"L=1,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1536,"L=3,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1537,"L=1,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1538,"L=3,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1539,"L=2,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1540,"L=4,TOF Eclust(mev)",80,0.,24.,0.);
      HBOOK1(1541,"L=2,TOF Eclust(mev)",80,0.,240.,0.);
      HBOOK1(1542,"L=4,TOF Eclust(mev)",80,0.,240.,0.);
      if(TOFRECFFKEY.relogic[0]==1){
        HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
        HBOOK1(1501,"Proton beta",80,0.9,1.1,0.);
        HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
        HBOOK2(1502,"Layer-1,T vs exp(-A)",50,0.,0.3,50,23.,26.,0.);
        HBOOK2(1503,"Layer-2,T vs exp(-A)",50,0.,0.3,50,23.,26.,0.);
        HBOOK2(1504,"Layer-3,T vs exp(-A)",50,0.,0.3,50,19.,22.,0.);
        HBOOK2(1505,"Layer-4,T vs exp(-A)",50,0.,0.3,50,19.,22.,0.);
        HBOOK1(1508,"T1-T3, not corrected",50,3.,6.,0.);
        HBOOK1(1509,"T2-T4, not corrected",50,3.,6.,0.);
        HBOOK1(1510,"Layer-1 PM-1 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1511,"Layer-1 PM-2 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1512,"Layer-1 PM-1 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1513,"Layer-1 PM-2 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1514,"Layer-2 PM-1 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1515,"Layer-2 PM-2 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1516,"Layer-2 PM-1 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1517,"Layer-2 PM-2 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1518,"Layer-3 PM-1 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1519,"Layer-3 PM-2 s-time,noncor",80,18.,28.,0.);
        HBOOK1(1520,"Layer-3 PM-1 a-ampl,noncor",80,50.,290.,0.);
        HBOOK1(1521,"Layer-3 PM-2 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1522,"Layer-4 PM-1 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1523,"Layer-4 PM-2 s-time,noncor",80,18.,28.,0.);
         HBOOK1(1524,"Layer-4 PM-1 a-ampl,noncor",80,50.,290.,0.);
         HBOOK1(1525,"Layer-4 PM-2 a-ampl,noncor",80,50.,290.,0.);
      }
    }
//-----------
//     ===> Clear JOB-statistics counters for SIM/REC :
//
    TOFJobStat::clear();
//-------------------------
// ===> create tofvpar structure :
//
 tofvpar.init(TOFRECFFKEY.daqthr, TOFRECFFKEY.cuts);//daqthr/cuts reading
//-------------------------
//                ===> create scbrcal structures :
// 
 integer i,j,ila,ibr,ibrm,isp,nsp,ibt,cnum,dnum,mult;
//
 geant scp[SCANPNT];
 geant rlo[SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 geant ef1[SCANPNT],ef2[SCANPNT];
 integer i1,i2,sta[2];
 geant r,eff1,eff2;
 integer sid,brt;
 geant gna[2],gnd[2],qath,qdth,a2dr,tth,strat;
 geant slope,fstrd,tzer,mip2q;
 geant tzerf[SCLRS][SCMXBR],slpf,strf; 
 char fname[80];
 char name[80];
 geant asatl=20.;//(mev,~10MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
 geant td2p[2]={16.9,1.8};// tempor timeD->coord conv.factor(cm/ns) and error(cm)
//  
//
//   ---> Read tzero/slope/str_ratio calib.file :
//-----------
//     ===> create tofvpar structure :
//
    TOFBrcal::build(); 
//-----------
}
//====================================================================
void AMSJob::_rectcinitjob(){
AMSgObj::BookTimer.book("RECTCEVENT");
}

void AMSJob::_reaxinitjob(){
AMSgObj::BookTimer.book("REAXEVENT");
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
  else strcpy(_Setup,"AMSSHUTTLE STRPLANES CTCHor");   //defaults
  
}
void AMSJob::settrigger(char *setup, integer N, integer I,integer or){
  assert(N < maxtrig);
  if(setup){
    strcpy(_TriggerC[N],setup);
  }
  _TriggerI=I;
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


TID.add (new AMSTimeID(AMSID("TrackerPedestals",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::peds[0])*AMSTrIdSoft::_numel,
   (void*)AMSTrIdSoft::peds));
TID.add (new AMSTimeID(AMSID("TrackerGains",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::gains[0])*AMSTrIdSoft::_numel,
   (void*)AMSTrIdSoft::gains));
TID.add (new AMSTimeID(AMSID("TrackerSigmas",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::sigmas[0])*AMSTrIdSoft::_numel,
   (void*)AMSTrIdSoft::sigmas));
// change deliberately one sigma;
// AMSTrIdSoft::sigmas[1]=3.44;
TID.add (new AMSTimeID(AMSID("TrackerStatus",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::status[0])*AMSTrIdSoft::_numel,
   (void*)AMSTrIdSoft::status));
TID.add (new AMSTimeID(AMSID("TrackerIndNoise",isRealData()),
   begin,end,sizeof(AMSTrIdSoft::indnoise[0])*AMSTrIdSoft::_numel,
   (void*)AMSTrIdSoft::indnoise));
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
   begin,end,SCBLMX*sizeof(AMSTOFScan),
   (void*)&scmcscan[0]));
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

  // Here update dbase
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


AMSJob::~AMSJob(){
  cout << "~AMSJob called "<<endl;
_tkendjob();
_ctcendjob();
_tofendjob();
_trdendjob();
_dbendjob();
_axendjob();

if(IOPA.hlun){
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

void AMSJob::_tkendjob(){

}


void AMSJob::_ctcendjob(){

}

//-------------------------------------------------------------------
void AMSJob::_tofendjob(){
//--------> some TOF stuff :
       TOFJobStat::print(); // Print JOB-TOF statistics
       if(TOFMCFFKEY.mcprtf[2]!=0){ // tempor! print MC-hists
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
       }
       if(TOFRECFFKEY.reprtf[2]!=0){ // tempor! print RECO-hists
         HPRINT(1100);
         HPRINT(1101);
         HPRINT(1102);
         HPRINT(1526);
         HPRINT(1527);
         HPRINT(1528);
         HPRINT(1529);
         HPRINT(1530);
         HPRINT(1531);
         HPRINT(1532);
         HPRINT(1533);
         HPRINT(1543);
         HPRINT(1534);
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1541);
         HPRINT(1542);
         if(TOFRECFFKEY.relogic[0]==1){// for calibr. runs
           HPRINT(1500);
           HPRINT(1501);
           HPRINT(1502);
           HPRINT(1503);
           HPRINT(1504);
           HPRINT(1505);
           HPRINT(1506);
           HPRINT(1508);
           HPRINT(1509);
           HPRINT(1510);
           HPRINT(1511);
           HPRINT(1512);
           HPRINT(1513);
           HPRINT(1514);
           HPRINT(1515);
           HPRINT(1516);
           HPRINT(1517);
           HPRINT(1518);
           HPRINT(1519);
           HPRINT(1520);
           HPRINT(1521);
           HPRINT(1522);
           HPRINT(1523);
           HPRINT(1524);
           HPRINT(1525);
           TOFTZSLcalib::mfit();
         }
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


}

void AMSJob::_dbendjob(){} // moved to A_LMS.C

