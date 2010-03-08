#include "tkdcards.h"
#include "TrCluster.h"
#include "TrTrack.h"
#include <math.h>
#include <float.h>


//==============================
//  TRACKER SIMULATION
//==============================
void TKGEOMFFKEY_DEF::init(){

  ReadGeomFromFile=0;
  WriteGeomToFile=0;
  UpdateGeomFile=0;
  CablVer=2;
  CalibVer=2;
  memset(fname,400,sizeof(fname[0]));
}

TKGEOMFFKEY_DEF TKGEOMFFKEY;

//=============================
//==============================
//  TRACKER RECONSTRUCTION
//==============================
void TRCLFFKEY_DEF::init(){
  recflag=111;
  ThrSeed[0]  = 3.50;
  ThrSeed[1]  = 3.50;
  ThrNeig[0]  = 1.00;
  ThrNeig[1]  = 1.00;

  SeedDist[0] =  3;
  SeedDist[1] =  3;
  
  // hit signal correlation (only muons/protons)
  GGpars[0]  = 1428.;
  GGpars[1]  = 0.0000;
  GGpars[2]  = 0.1444;
  GGpars[3]  = 1645.;
  GGpars[4]  = 0.0109;
  GGpars[5]  = 0.0972;
  GGintegral = 91765.;
  ThrProb    = 0;//0.00001;

  ClusterSigCorrOpt = (TrClusterR::kAsym|TrClusterR::kAngle);
  ClusterCofGUsedStrips = -1;
  TwoStripThresholdX    = 0.70;  // tan(35deg)
  TwoStripThresholdY    = 0.36;  // tan(20deg)

  MaxNrawCls     = 2000;
  lowdt          = 200;
  MaxNtrCls      = 500;
  MaxNtrCls_ldt  = 150;
  MaxNtrHit      = 200;
  TrTimeLim      = 500;
  //Track
  MaxNtrack = 4;
  MinNhitX  = 4;
  MinNhitY  = 5;
  MinNhitXY = 4;
  PatAllowOption = 7;
  MaxChisqAllowed   = 300;
  LadderScanRange   = 7.3; //= TkDBc::Head->_ladder_Ypitch
  ClusterScanRange  = 0.5;
  MaxChisqForLScan  = 2.2;
  ErrXForScan       = 300e-4;
  ErrYForScan       = 300e-4;
  TrackThrSeed[0]   = 4.0;
  TrackThrSeed[1]   = 4.0;

  AdvancedFitFlag   = TrTrackR::DefaultAdvancedFitFlags;

  //debug
  TrDEBUG = 0;
  PZDEBUG = 0;

  //TAS
  TasRecon = 0;
  TasCurr  = 0;
  TasLDDR  = 0;
}

TRCLFFKEY_DEF TRCLFFKEY;
//==============================



void TRMCFFKEY_DEF::init(){
  //used
  alpha=250;
  beta=0.46;
  gamma=0.25;
  fastswitch=5.e-5;  // inverse linear density of primary electrons
  dedx2nprel=0.33e6;
  delta[0]=0.9; //-0.67;
  delta[1]=1.0;
  gammaA[0]=0.2;  //-0.3;
  gammaA[1]=0.1;
  /// Give the conversion ADC count keV



  //! Seed Threshold (0=S 1=K) for the MC DSP like clusterization
  th1sim[0]=4.0;
  th1sim[1]=4.0;
  //! Enlargement Threshold (0=S 1=K) for the MC DSP like clusterization
  th2sim[0]=1.0;
  th2sim[1]=1.0;
  
  
  //unknown
  ped[0]=500;
  ped[1]=500;
  gain[0]=8;
  gain[1]=8;
  sigma[1]=55./14./sqrt(3.); // sig/noise ratio is about 14 for y
  sigma[0]=sigma[1]*1.41;   // x strip two times larger y
  NonGaussianPart[0]=0;
  NonGaussianPart[1]=0.1;
  BadCh[0]=0.01;
  BadCh[1]=0.01;

  cmn[0]=10;
  cmn[1]= 6;
  adcoverflow=32700;
  NoiseOn=1;
  sec[0]=0;
  sec[1]=0;
  min[0]=0;
  min[1]=0;
  hour[0]=0;
  hour[1]=0;
  day[0]=1;
  day[1]=1;
  mon[0]=0;
  mon[1]=0;
  year[0]=101;
  year[1]=110;
  GenerateConst=0;
  WriteHK=0;
  thr1R[0]=-2.75;
  thr1R[1]=3.5;
  thr2R[0]=1;
  thr2R[1]=1;
  neib[0]=0;
  neib[1]=1;
  CalcCmnNoise[0]=1;
  CalcCmnNoise[1]=0;
  {
    int i,j,k;
    for(i=0;i<8;i++){
      for(j=0;j<2;j++){
	for(k=0;k<32;k++)RawModeOn[i][j][k]=0;
      }
    }
  }

  pl9zgap=0.;//(558) PL9 supp.str. gap from EC front face Z (cm)
  pl9sthick=0.5;//(559)  PL9 supp.str.support thickness (cm)
}

TRMCFFKEY_DEF TRMCFFKEY;
//=============================
void TRCALIB_DEF::init(){
  CalibProcedureNo=0;
  EventsPerCheck=100;
  PedAccRequired[0]=0.12;
  PedAccRequired[1]=0.09;
  Validity[0]=0;
  Validity[1]=36000;
  RhoThrA=0.7;
  RhoThrV=0.8;
  BadChanThr[0]=3.3;
  BadChanThr[1]=0.002;
  Method=2;
  Pass=1;
  DPS=1;
  UPDF=4;
  PrintBadChList=0;
  EventsPerIteration[0]=100;
  EventsPerIteration[1]=100;
  EventsPerIteration[2]=100;
  EventsPerIteration[3]=100;
  NumberOfIterations[0]=100;
  NumberOfIterations[1]=100;
  NumberOfIterations[2]=100;
  NumberOfIterations[3]=100;
  BetaCut[0][0]=0.7;
  BetaCut[0][1]=10.;
  BetaCut[1][0]=1;
  BetaCut[1][1]=10;
  BetaCut[2][0]=0.7;
  BetaCut[2][1]=1.4;
  BetaCut[3][0]=0.7;
  BetaCut[3][1]=1.4;
  HitsRatioCut[0]=2.2;
  HitsRatioCut[1]=2.2;
  HitsRatioCut[2]=0.998;
  HitsRatioCut[3]=0.998;
  MomentumCut[0][0]=-FLT_MAX;
  MomentumCut[0][1]=FLT_MAX;
  MomentumCut[1][0]=3;
  MomentumCut[1][1]=FLT_MAX;
  MomentumCut[2][0]=0.4;
  MomentumCut[2][1]=2.5;
  MomentumCut[3][0]=0.1;
  MomentumCut[3][1]=10;
  Chi2Cut[0]=3;
  Chi2Cut[1]=3;
  Chi2Cut[2]=100;
  Chi2Cut[3]=100;
  PatStart=0;
  MultiRun=0;
  EventsPerRun=10000001;
  int i;
  for(i=0;i<6;i++){
    Ladder[i]=0;
    ActiveParameters[i][0]=1;   // x
    ActiveParameters[i][1]=1;   // y
    ActiveParameters[i][2]=0;   // z
    ActiveParameters[i][3]=0;   // pitch  zx
    ActiveParameters[i][4]=1;   // yaw    xy
    ActiveParameters[i][5]=0;   // roll   yz
  }
}

TRCALIB_DEF TRCALIB;
//=============================
void TRALIG_DEF::init(){
  One=1.025;
  GlobalFit=0;
  InitDB=0;
  ReWriteDB=0;
  UpdateDB=0;
  MaxPatternsPerJob=50;
  MaxEventsPerFit=9999;
  MinEventsPerFit=999;
  for(int i=0;i<9;i++){
    Cuts[i][0]=0;
    Cuts[i][1]=0;
  }
  Cuts[0][1]=1;      // chi2 cut for alg=0
  Cuts[1][0]=0.992;   // cos  cut for alg =0 
  Cuts[2][0]=0.5;     //rel mom cut
  Cuts[2][1]=2;
  Cuts[3][0]=0;      // beta cut
  Cuts[3][1]=1.4;    
  Cuts[4][0]=0;      // beta pattern cut
  Cuts[4][1]=0;    
  Cuts[5][0]=0.5;      // mass cut
  Cuts[5][1]=2.0;    
  Cuts[6][0]=0.;         // betaerror cut
  Cuts[6][1]=0.04;   

  Algorithm=0;
  for( int i=0;i<6;i++){
    ActiveParameters[i][0]=1;   // x
    ActiveParameters[i][1]=1;   // y
    ActiveParameters[i][2]=1;   // z
    ActiveParameters[i][3]=1;   // pitch  zx
    ActiveParameters[i][4]=1;   // yaw    xy
    ActiveParameters[i][5]=1;   // roll   yz
  }
  EventsPerRun=1001;
  LayersOnly=0;
  GlobalGoodLimit=0.085;
  SingleLadderEntryLimit=10000;
}

TRALIG_DEF TRALIG;

//=============================
void TRFITFFKEY_DEF::init(){
  // Fit Par
 
  MaxNtrack = 7;
  MinNhit   = 5;
  SearchRegStrLine=1.5;
  SearchRegCircle=10.;
  ErrX = 34.e-4;  
  ErrY = 20.e-4;
  ErrZ = 34.e-4;
  MaxChisq[0] = 5;
  MaxChisq[1] =  2000;

 {
    for( int k=0;k<(int)(sizeof(patternp)/sizeof(patternp[0]));k++)patternp[k]=0;
  }
  UseTRD=1;
  UseTOF=2;
  Chi2StrLine=10;
  ResCutFastFit=0.5;
  ResCutStrLine=0.5;
  ResCutCircle=0.5;
  SearchRegFastFit=1;
  RidgidityMin=0.02;
  FullReco=0;
  MinRefitCos[0]=0.7;
  MinRefitCos[1]=0.5;
  FastTracking=1;
  WeakTracking=1;
  FalseXTracking=1;
  Chi2FalseX=3.;
  ForceFalseTOFX=1;
  FalseTOFXTracking=1;
  TOFTracking=1;
  ForceAdvancedFit=1;
  ThrClA[0]=0.;
  ThrClA[1]=0.;
  ThrClR[0]=0.;
  ThrClR[1]=0.;
  MaxTrRecHitsPerLayer=250;
  LowMargin=0;
  OnlyGammaVtx=0;
  UseGeaneFitting=0;
  OldTracking=0;
  for( int k=0;k<(int)(sizeof(patternp02)/sizeof(patternp02[0]));k++)patternp02[k]=0;
}

TRFITFFKEY_DEF TRFITFFKEY;
//=============================
