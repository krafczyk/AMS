// $Id: tkdcards.C,v 1.80 2012/09/13 15:52:02 oliva Exp $
#include "tkdcards.h"
#include "TrCluster.h"
#include <math.h>
#include <float.h>


ClassImp(TKGEOMFFKEY_DEF);
ClassImp(TRMCFFKEY_DEF);
ClassImp(TRCALIB_DEF);
ClassImp(TRALIG_DEF);
ClassImp(TRCLFFKEY_DEF);
ClassImp(TRFITFFKEY_DEF);
ClassImp(TRCHAFFKEY_DEF);


//==============================
//  TRACKER SIMULATION
//==============================
void TKGEOMFFKEY_DEF::init(){
  ReadGeomFromFile=0;
  WriteGeomToFile=0;
  UpdateGeomFile=0;
  CablVer=2;
  CalibVer=3;
  memset(fname,400,sizeof(fname[0]));
  memset(disfname,400,sizeof(disfname[0]));
  alignver=3;
  LoadMCDisalign=0;
  EnableExtMov=0;
  float p1[6]={0.02,0.02,0.02,0.0001,0.0001,0.0001};
  float phase1[6]={1.,2.,3.,2.1,3.4,4.5};
  float p9[6]={0.02,0.02,0.02,0.0001,0.0001,0.0001};
  float phase9[6]={1.4,2.3,3.2,2.11,3.14,4.85};
  for (int ii=0;ii<6;ii++){
    L1Amp[ii]=p1[ii];
    L1Phase[ii]=phase1[ii];
    L9Amp[ii]=p9[ii];
    L9Phase[ii]=phase9[ii];
  }
  exalignver=2;
  MdExAlignTag=0;
}

TKGEOMFFKEY_DEF TKGEOMFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (tkgeomffkey_)
#endif


//==============================
//  TRACKER RECONSTRUCTION
//==============================
void TRCLFFKEY_DEF::init(){
for (int ii=0;ii<9;ii++){
  recflag=1111;
  ThrSeed[0][ii]  = 3.50;
  ThrSeed[1][ii]  = 3.50;
}
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
  MaxNtrCls      = 2000;
  MaxNtrCls_ldt  = MaxNtrCls;
  MaxNtrHit      = 1600;
  TrTimeLim      =   10;
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
  //debug
  TrDEBUG = 0;
  PZDEBUG = 0;
  AdvancedFitFlag = 0xef; // kChikanian is off
  //TAS
  TasRecon = 0;
  TasCurr  = 0;
  TasLDDR  = 0;
  ExtMatch = 11;
  UseSensorAlign=1;
  statflag = 1;

  // Charge Recon. Parameters
  ChargeSeedTagActive = 1;
  ChargeSeedSelActive = 0; 
  CorrelationProbThr = 0.001; 
  YClusterSNThr = 4.5; 
  YClusterNStripThr = 0; 
  ReorderTrClusters = 1; 
  ReorderTrRecHits = 0; 
  BuildTracksSimpleChargeSeedTag = 1; 
  TrackFindChargeCutActive = 0; 
}

TRCLFFKEY_DEF TRCLFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trclffkey_)
#endif


//==============================
//  TRACKER CLUSTER SIMULATION
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

  ////////////////////////////////
  // New stuff
  ////////////////////////////////

  // Common Parameters

  // Simulation type  
  SimulationType = 2;
  // Minimum number of MC cluster required to trigger digitization
  MinMCClusters = 0;
  // Noise type 
  NoiseType = 3;
  // DSP seed strip S/N threshold per layer (updated 03/03/2011)
  // Settings for test beam 
  /*
  DSPSeedThr[0][0] = 3.500; DSPSeedThr[0][7] = 3.500; DSPSeedThr[0][8] = 3.500; // x: single layer on plane
  DSPSeedThr[0][1] = 3.500; DSPSeedThr[0][2] = 3.500; DSPSeedThr[0][3] = 3.500; // x: two layers on a single plane
  DSPSeedThr[0][4] = 3.500; DSPSeedThr[0][5] = 3.500; DSPSeedThr[0][6] = 3.500; // x: two layers on a single plane
  DSPSeedThr[1][0] = 3.500; DSPSeedThr[1][7] = 3.500; DSPSeedThr[1][8] = 3.500; // y: single layer on plane
  DSPSeedThr[1][1] = 3.500; DSPSeedThr[1][2] = 3.500; DSPSeedThr[1][3] = 3.500; // y: two layers on a single plane
  DSPSeedThr[1][4] = 3.500; DSPSeedThr[1][5] = 3.500; DSPSeedThr[1][6] = 3.500; // y: two layers on a single plane
  */
  // Settings for "space" 
  DSPSeedThr[0][0] = 3.500; DSPSeedThr[0][7] = 3.500; DSPSeedThr[0][8] = 3.500; // x: single layer on plane
  DSPSeedThr[0][1] = 3.875; DSPSeedThr[0][2] = 3.875; DSPSeedThr[0][3] = 3.875; // x: two layers on a single plane
  DSPSeedThr[0][4] = 3.875; DSPSeedThr[0][5] = 3.875; DSPSeedThr[0][6] = 3.875; // x: two layers on a single plane
  DSPSeedThr[1][0] = 3.625; DSPSeedThr[1][7] = 3.625; DSPSeedThr[1][8] = 3.625; // y: single layer on plane
  DSPSeedThr[1][1] = 4.000; DSPSeedThr[1][2] = 4.000; DSPSeedThr[1][3] = 4.000; // y: two layers on a single plane
  DSPSeedThr[1][4] = 4.000; DSPSeedThr[1][5] = 4.000; DSPSeedThr[1][6] = 4.000; // y: two layers on a single plane
  // DSP neigboring strips S/N threshold (updated 03/03/2011)
  DSPNeigThr[0] = 1.;
  DSPNeigThr[1] = 1.;

  // TrSim2010 Parameters

  // ADC vertical MIP value
  TrSim2010_ADCMipValue[0] = 34.5; 
  TrSim2010_ADCMipValue[1] = 32.0; 
  // Apply p-strip deformation?
  TrSim2010_PStripCorr = 0;
  // Saturation limit for strips
  TrSim2010_ADCSat[0] = 3500.;
  TrSim2010_ADCSat[1] = 3500.;
  // Interstrip capacitance (pF)
  TrSim2010_Cint[0] = 80.;
  TrSim2010_Cint[1] = 50.;  
  // Backplane(+guardring) capacitance (pF)     
  TrSim2010_Cbk[0] = 50.;
  TrSim2010_Cbk[1] = 5.;
  // Readout capacitance (pF)
  TrSim2010_Cdec[0] = 800.;
  TrSim2010_Cdec[1] = 800.;
  // Charge diffusion model
  TrSim2010_DiffType[0] = 2;
  TrSim2010_DiffType[1] = 2;
  // Diffusion radium (um)
  TrSim2010_DiffRadius[0] = 8.;
  TrSim2010_DiffRadius[1] = 8.; 
  // Tuning parameter: shaking a fraction
  TrSim2010_FracNoise[0] = 0.1; 
  TrSim2010_FracNoise[1] = 0.1; 
  // Tuning parameter: adding noise
  TrSim2010_AddNoise[0] = 2.5; 
  TrSim2010_AddNoise[1] = 2.5;
  // for the ladder shielding
  ActivateShielding=1;
  ShieldingDensity=2.5;
}

TRMCFFKEY_DEF TRMCFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trmcffkey_)
#endif


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
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trcalib_)
#endif


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
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (tralig_)
#endif


//==============================
//  TRACKER FITTING
//==============================
#include "TrTrack.h"

void TRFITFFKEY_DEF::init(){
  // Fit Par

  /// R [1,3] fit point error 
  ErrX =  25.e-4;  // Tuned with muon MC 25/Jan/2011 (SH)
  ErrY =  13.e-4;  // Tuned with muon MC 25/Jan/2011 (SH)
  ErrZ = 300.e-4;  // Not used

  /// Tuned with muon MC 25/Jan/2011 (SH)
  /// R [4,12] Error term for Multiple scattering per layer (1-9)
  float fwmsc[9] = { 15.5, 6.3, 5.2, 0.0, 0.0, 5.1, 6.1, 324, 243 };

  for (int i = 0; i < 9; i++) FitwMsc[i] = fwmsc[i];

  /// R 13 max dist to add external planes hits X
  MergeExtLimX=0.5;
  /// R 14 max dist to add external planes hits Y
  MergeExtLimY=0.5;

  /// R 15 default mass for the fitting
  DefaultMass    = TrTrackR::DefaultMass;

  /// R 16 default charge for the fitting
  DefaultCharge  = TrTrackR::DefaultCharge;

  /// R [17,18] thresholds for TrRecon::TryDropX
  RthdDropX[0] = 100;
  RthdDropX[1] =  10;

  /// R [19,20] Factor on ErrX/ErrY for He
  ErcHeX = 0.65;
  ErcHeY = 0.65;
  /// I 21 Activates the multiple Alignmnet fitting during the reconstruction (1=PG 2=PG+MD 3=PG+MD+Averagae)
  MultipleAlign=2;

}
int TRFITFFKEY_DEF::ReadFromFile = 1;

TRFITFFKEY_DEF TRFITFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trfitffkey_)
#endif


//=============================
//  TRACKER CHARGE RECON.
//=============================
void TRCHAFFKEY_DEF::init(){
  /// I 1 reconstruction type (default 2): 0 = no recon, 1 = trunc. mean, 2 = likelihood
  ReconType = 1;
  /// I 2 pdf version used (default 1): 0 = original (v5.00 dbase, from MC), 1 = first implementation (sep. 2011)
  PdfVersion = 1;
  /// I 3 max charge recontructed (default 8)
  MaxCharge = 8;
  /// I 4 enable histograms (default 0): 0 = disabled, 1 = enabled
  EnableHisto = 0;
}
   
TRCHAFFKEY_DEF TRCHAFFKEY;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (trchaffkey_)
#endif


