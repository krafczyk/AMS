#ifndef tkcommons_h
#define tkcommons_h

#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"



//==============================================================


//! Data card to drive the geometry generation
class TKGEOMFFKEY_DEF{
public:
  integer ReadGeomFromFile;
  integer WriteGeomToFile;
  integer UpdateGeomFile;
  ///Cabling version 1= Pre-Integration 2= flight
  integer CablVer;
  ///Cabibration format  1= Pre-Integration 2= flight
  integer CalibVer;
  integer fname[400];
  ///[406] Disalignment file name (PGTRACK)
  integer disfname[400];
  void init();

};
#define TKGEOMFFKEY COMMON_BLOCK(TKGEOMFFKEY,tkgeomffkey)
COMMON_BLOCK_DEF(TKGEOMFFKEY_DEF,TKGEOMFFKEY);

//! Data Card to pilot the Tracker MC
class TRMCFFKEY_DEF {
public:
geant alpha;
geant beta;
geant gamma;
geant dedx2nprel;
geant fastswitch;

//! Seed Threshold (0=S 1=K) for the MC DSP like clusterization
geant th1sim[2];
//! Enlargement Threshold (0=S 1=K) for the MC DSP like clusterization
geant th2sim[2];

geant ped[2];
geant sigma[2];
geant gain[2];
geant   cmn[2];
integer adcoverflow;
integer NoiseOn;
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
integer CalcCmnNoise[2];
geant thr1R[2];
geant thr2R[2];
integer neib[2];
integer GenerateConst;
integer RawModeOn[8][2][32];
integer WriteHK; 
geant delta[2];
geant gammaA[2];
geant NonGaussianPart[2];
geant BadCh[2];
geant pl9zgap;
geant pl9sthick;
void init();
number ADC2KeV(){
  return 0.5e6/beta/dedx2nprel;
}

// SimulationType = 0:RawSimulation, 1:SkipRawSimulation, 2:TrSim2010 
integer SimulationType; 
// Common Parameters
integer MinMCClusters;
integer NoiseType;    
integer FakeClusterType;
geant   DSPSeedThr[2];  
geant   DSPNeigThr[2];
// TrSim2010 Parameters
integer TrSim2010_ADCConvType[2];
integer TrSim2010_EDepType[2];   
geant   TrSim2010_ADCSat[2]; 
geant   TrSim2010_Cint[2];        
geant   TrSim2010_Cbk[2];  
geant   TrSim2010_Cdec[2]; 
integer TrSim2010_DiffType[2]; 
geant   TrSim2010_DiffRadius[2]; 
geant   TrSim2010_FracNoise[2]; 
geant   TrSim2010_AddNoise[2];     

};

#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);

const integer nalg=4;
class TRCALIB_DEF{
public:
integer CalibProcedureNo;
                                // Proc # 1 starts here
integer EventsPerCheck;
geant PedAccRequired[2]; 
integer Validity[2];
geant RhoThrA; 
geant RhoThrV; 
integer Method;
geant BadChanThr[2];
integer Pass;
integer DPS;
integer UPDF;
integer LaserRun;
integer PrintBadChList;
                                // Proc # 2 starts here
integer EventsPerIteration[nalg];
integer NumberOfIterations[nalg];
                                 // Select Cut
geant BetaCut[nalg][2];             // Beta limits
geant HitsRatioCut[nalg];           // Hit Ratio  // cos(pred,fitted) for alg 3
                                 // Global fit cuts
geant MomentumCut[nalg][2];         // momentum ----------
geant Chi2Cut[nalg];                // chi2 --------------- 
integer ActiveParameters[8][6]; //   for each plane: x,y,z, pitch, yaw, roll
integer Ladder[8];            // ladder*10+half no for each plane 0 == all
integer PatStart;
integer MultiRun;
integer EventsPerRun;
integer Version;
void init();

};
#define TRCALIB COMMON_BLOCK(TRCALIB,trcalib)
COMMON_BLOCK_DEF(TRCALIB_DEF,TRCALIB);

class TRALIG_DEF{
public:
integer InitDB;    
integer ReWriteDB;
integer UpdateDB;
integer MaxEventsPerFit;
integer MinEventsPerFit;
integer MaxPatternsPerJob;
geant Cuts[10][2];  
integer Algorithm;     
integer ActiveParameters[8][6]; //   for each plane: x,y,z, pitch, yaw, roll
integer GlobalFit;
integer EventsPerRun;
integer LayersOnly;
geant GlobalGoodLimit;
integer SingleLadderEntryLimit;
geant One;
  void init();

};
#define TRALIG COMMON_BLOCK(TRALIG,tralig)
COMMON_BLOCK_DEF(TRALIG_DEF,TRALIG);

//=============================
//==============================
//  TRACKER RECONSTRUCTION
//==============================
//! Data Cards to drive the Tracker reconstruction
class TRCLFFKEY_DEF {
public:
  //! I 1 Drive the tracker Recon =0 No recon; =1 Cluster; =11 Cluster and hits; =111 Cluster, Hits and Tracks
  integer recflag;
  //! R [2,3] Seed S/N Threshold for 2:n(X) 3:p(Y) side
  geant     ThrSeed[2] ;  
  //! R [4,5] expansion S/N Threshold for 4:n(X) 5:p(Y) side
  geant     ThrNeig[2] ;
  //! I [6,7] Min Distance between seeds for 6:n(X) 7:p(Y) side
  integer   SeedDist[2] ;
  
  //! R [8 - 13] hit signal correlation (only muons/protons)
  geant GGpars[6]  ;//! {1428.,0.0000,0.1444,1645.,0.0109,0.0972};
  //! R 14 hit signal correlation (only muons/protons)
  geant GGintegral ;//! 91765.;
  //! R 15 Threshold on the S/K Correlation probability
  geant ThrProb    ;//! 0.00001;

  //! I 16 TrClusterR::DefaultCorrOpt
  int ClusterSigCorrOpt;
  //! I 17 TrClusterR::DefaultUsedStrips
  int ClusterCofGUsedStrips;
  //! R 18 _dxdz threshold for 2/3-strip CofG
  geant TwoStripThresholdX;
  //! R 19 _dydz threshold for 2/3-strip CofG
  geant TwoStripThresholdY;

  // HIT PARS
  //PRE-SELECTION PARS
  //! I 20 MaxNrawCls 
  int MaxNrawCls ;// 2000
  //! I 21 Threeshold used to define a small dt event
  int lowdt    ;// 200
  //! I 22 MaxNtrCls
  int MaxNtrCls  ;// 1000
  //! I 23 MaxNtrCls_ldt
  int MaxNtrCls_ldt;// 150
  //! I 24 MaxNtrHit
  int MaxNtrHit  ;// 1000
  //! F 25  Trackin time limit
  float TrTimeLim  ;// 1000
  //TRACK PARS
  //! I 26 MaxNtrack 
  int    MaxNtrack ;//4
  //! I 27 MinNhitX
  int    MinNhitX  ;//4
  //! I 28 MinNhitY
  int    MinNhitY  ;//5
  //! I 29 MinNhitXY
  int    MinNhitXY ;//4
  //! I 30 PatAllowOption
  int    PatAllowOption ;//0
  //! R 31 MaxChisqAllowed
  float MaxChisqAllowed   ;//300
  //! R 32  LadderScanRange
  float LadderScanRange   ;//7.3 = TkDBc::Head->_ladder_Ypitch
  //! R 33 ClusterScanRange
  float ClusterScanRange  ;//0.5
  //! R 34 MaxChisqForLScan
  float MaxChisqForLScan  ;//2.2
  //! R 35 ErrXForScan
  float ErrXForScan       ;//300e-4
  //! R 36 ErrYForScan
  float ErrYForScan       ;//300e-4
  //! R [37,38] Seed S/N Threshold for 37:n(X) 38:p(Y) side
  geant TrackThrSeed[2] ;  

  //! I 39 Fit methods to be included for AdvancedFit see TrTrack.h for detail
  int AdvancedFitFlag;

  //DEBUG
  //! I 40 TrDEBUG (DEBUG on Track finding)
  int TrDEBUG ;
  //! I 41 PZDEBUG
  int PZDEBUG ;
  //! I 42 TAS reconstruction
  int TasRecon;
  //! I 43 TAS current intensity
  int TasCurr;
  //! I 44 TAS LDDR mode
  int TasLDDR;
  //! I 45 mn;  m=1 TOF_match ON; n=1 Trd mathcing On; 
  int ExtMatch;

  void init();
};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);

//
const integer npat=42;
const integer npat02=218;


//! Data Cards to drive the tracke fiting
class TRFITFFKEY_DEF {
public:
  integer patternp[npat];  //patterns  to fit (AMS-01); Priority decreases with number
  // Pattern list for AMS-02 tracker is written at the bottom of this file

  /// Number of maximum tracks
  int MaxNtrack;
  /// Minimum allowed number of hits
  int MinNhit;
  geant SearchRegFastFit;   //! Max distance between point and curve to include pint un the fit
  geant SearchRegStrLine;  //! -------
  geant SearchRegCircle;   //! ----------  
  
  double ErrX;
  double ErrY;
  double ErrZ;
  /// Maximum allowed chisquare for 0:Simple and 1:Normal fitting
  /// (was beforeChi2WithoutMS, Chi2FastFit of TRFITFFKEY)
  double MaxChisq[2];

  integer UseTOF;     //! Use(>=1)/Not use(0) 
                      //! TOF info for pattern rec for pattern 16
  geant Chi2StrLine; //! The same for Straight line fit
  geant ResCutFastFit;   //! Max acceptible distance betgween point and fitted curve 
  geant ResCutStrLine;    //! ---
  geant ResCutCircle;     //! ---

  geant RidgidityMin;
  integer FullReco;
  geant MinRefitCos[2];
  integer FastTracking;
  integer WeakTracking;
  integer FalseXTracking;
  geant Chi2FalseX;
  integer ForceFalseTOFX;
  integer FalseTOFXTracking;
  integer TOFTracking;
  integer ForceAdvancedFit;
  geant ThrClA[2];
  geant ThrClR[2];
  integer MaxTrRecHitsPerLayer;
  integer UseTRD;     //! Use(>=1)/Not use(0) 
  integer LowMargin;
  /// If !=0 limit the vertex creation to two tracks
  integer OnlyGammaVtx;
  integer UseGeaneFitting; //! Use Geane(1) or Yale (0) fitting method
  integer OldTracking;  //!Use Old (VC) or new (JA) pattern recognition

  integer patternp02[npat02];  //patterns  to fit (AMS-02); Priority decreases with number
  // Pattern list for AMS-02 tracker is written at the bottom of TrRecon.C

  void init();
};
//


#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);

#endif
