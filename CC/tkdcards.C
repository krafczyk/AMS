#include "tkdcards.h"
#include <cmath>
#ifdef __DARWIN__
#include <limits.h>   // for FLT_MAX
#include <float.h>    // for FLT_MAX (for Dawin GCC 4)
#else
#include <values.h>   // for FLT_MAX (for Linux)
#endif


TKGEOMFFKEY_DEF TKGEOMFFKEY;
void TKGEOMFFKEY_DEF::init(){

  ReadGeomFromFile=1;
  WriteGeomToFile=0;
  UpdateGeomFile=0;
  gfile[0]='\0';
}

TRMCFFKEY_DEF TRMCFFKEY;
void TRMCFFKEY_DEF::init(){


  alpha=250;
  beta=0.46;
  gamma=0.25;
  fastswitch=5.e-5;  // inverse linear density of primary electrons
  dedx2nprel=0.33e6;
  ped[0]=500;
  ped[1]=500;
  gain[0]=8;
  gain[1]=8;
  sigma[1]=55./14./sqrt(3.); // sig/noise ratio is about 14 for y
  sigma[0]=sigma[1]*1.41;   // x strip two times larger y
  delta[0]=-0.67;
  delta[1]=1.0;
  gammaA[0]=-0.3;
  gammaA[1]=0.1;
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
  year[0]=102;
  year[1]=120;
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

  int i,j,k;
  for(i=0;i<8;i++){
    for(j=0;j<2;j++){
      for(k=0;k<32;k++)RawModeOn[i][j][k]=0;
    }
  }
}


TRCALIB_DEF TRCALIB;
void TRCALIB_DEF::init(){


  CalibProcedureNo=0;
  EventsPerCheck=1000;
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
  LaserRun=0;
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
  Version=0;
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

TRALIG_DEF TRALIG;
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
  Cuts[7][0]=6;         //min tr point cut
  Cuts[7][1]=10000;     //max chi2   
  Cuts[8][0]=0;     //min rig   
  Cuts[8][1]=1;     //min iter   
  Cuts[9][0]=100000000; //diff between positive and negative rig
  Cuts[9][1]=0;   // 1 only pos rig -1 only neg rig
  Algorithm=0;
  for( int i=0;i<8;i++){
    int one=1;
    int zero=0;

    // layersonly
    /*
      if(i==1)one=0;
      ActiveParameters[i][0]=zero;   // x
      ActiveParameters[i][1]=zero;   // y
      ActiveParameters[i][2]=one;   // z
      ActiveParameters[i][3]=zero;   // pitch  zx
      ActiveParameters[i][4]=zero;   // yaw    xy
      ActiveParameters[i][5]=zero;   // roll   yz
    */
    //ladders 


    ActiveParameters[i][0]=one;   // x
    ActiveParameters[i][1]=one;   // y
    ActiveParameters[i][2]=one;   // z
    //ActiveParameters[i][2]=zero;
    ActiveParameters[i][3]=one;   // pitch  zx
    //ActiveParameters[i][3]=zero;   // roll   yz
    ActiveParameters[i][4]=one;   // yaw    xy
    ActiveParameters[i][5]=one;   // roll   yz
    //ActiveParameters[i][5]=zero;   // roll   yz
    if(i==1){
      for (int k=0;k<6;k++){
	if(ActiveParameters[i][k]==-one)ActiveParameters[i][k]=zero;  
      }
    }     


  }
  EventsPerRun=1001;
  LayersOnly=0;
  LaddersOnly=1;
  GlobalGoodLimit=0.085;
  SingleLadderEntryLimit=10000;

}


TRCLFFKEY_DEF TRCLFFKEY;
void TRCLFFKEY_DEF::init(){

  //number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
  TRCLFFKEY.ThrClA[0]=12.;
  ThrClA[1]=12.;

  ThrClS[0]=20;
  ThrClS[1]=15;

  ThrClR[0]=3.;
  ThrClR[1]=4.;

  Thr1A[0] =8.;
  Thr1A[1] =8.;

  Thr2A[0] =4.;
  Thr2A[1] =3.;

  Thr1S[0] =20;
  Thr1S[1] =15;

  Thr2S[0] =15;
  Thr2S[1] =10;

  Thr1R[0] =-2.75;
  Thr1R[1] =3.5;

  ThrClS[0]=20;
  Thr1S[0] =15;
  Thr2S[0] =15;

  Thr2R[0] =1.;
  Thr2R[1] =3.1;  // should be around 1 if ThrClNEl[1]=3;
  // should be around 3 if ThrClNEl[1]=5;
  Thr3R[0] =-2.;
  Thr3R[1] =-2.;

  ThrClNMin[0]=1;
  ThrClNMin[1]=1;

  ThrClNEl[0]=3;
  ThrClNEl[1]=5;

  ErrX=34.e-4;
  ErrY=20.e-4;
  ErrZ=34.e-4;
  ThrDSide=1.;

  CorFunParA[0][0]=400e-4;
  //CorFunParB[0][0]=0.85;
  CorFunParB[0][0]=1.;
  CorFunParA[0][5]=400e-4;
  CorFunParB[0][5]=1.;
  int k;
  for (k=1;k<5;k++){
    CorFunParA[0][k]=400e-4;
    CorFunParB[0][k]=1.;
  }
  for ( k=0;k<6;k++){
    CorFunParA[1][k]=65e-4;
    // CorFunParB[1][k]=0.5;
    CorFunParB[1][k]=1.;
  }

  for(k=0;k<6;k++){
    for(int l=0;l<3;l++){
      ResFunX[l][k]=0;
      ResFunY[l][k]=0;
    }
  }
  CommonGain[0]=1.;
  CommonGain[1]=1.;
  CommonShift[0]=0.;
  CommonShift[1]=0.;

  EtaCor[0]=1;
  EtaCor[1]=0;

}



TRFITFFKEY_DEF TRFITFFKEY;
void TRFITFFKEY_DEF::init(){


  for( int k=0;k<sizeof(patternp)/sizeof(patternp[0]);k++)patternp[k]=0;
  
  MainAlg=541;
  UseTRD=1;
  UseTOF=2;
  Chi2FastFit=2000;
  Chi2StrLine=100;
  Chi2WithoutMS=50;
  ResCutFastFit=0.5;
  ResCutStrLine=0.5;
  ResCutCircle=0.5;
  SearchRegFastFit=1;
  SearchRegStrLine=1.5;
  SearchRegCircle=10.;
  RidgidityMin=0.02;
  FullReco=0;
  MinRefitCos[0]=0.7;
  MinRefitCos[1]=0.5;
  FastTracking=1;
  WeakTracking=0;
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

}
