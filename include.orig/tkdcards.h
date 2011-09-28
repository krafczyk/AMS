// $Id: tkdcards.h,v 1.43 2011/09/28 02:04:51 pzuccon Exp $
#ifndef tkcommons_h
#define tkcommons_h

#include "typedefs.h"
#include "TObject.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

//########################################################
// WARNING do not touch the order of the variables unless 
//         you understand all the consequences
//         Never delete or move the first data meber of 
//         each class (ask PZ)
//########################################################

//==============================================================


//! Data card to drive the geometry generation
class TKGEOMFFKEY_DEF: public TObject{
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
  ///[806] Alignment version; 1 :preflight, >1 flight
  integer alignver;
  ///[807]; Enable MC disalign (0 disabled(default), 1 from file, 2 from TDV)
  integer LoadMCDisalign;
  void init();
  TKGEOMFFKEY_DEF():TObject(){init();}
  ClassDef(TKGEOMFFKEY_DEF,2);

};
#define TKGEOMFFKEY COMMON_BLOCK(TKGEOMFFKEY,tkgeomffkey)
COMMON_BLOCK_DEF(TKGEOMFFKEY_DEF,TKGEOMFFKEY);

//=============================================================

//! Data Card to pilot the Tracker MC
class TRMCFFKEY_DEF: public TObject {
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

  ////////////////////////////////
  // New stuff
  ////////////////////////////////

  // Common Parameters

  // Simulation type  
  integer SimulationType; 
  // Minimum number of MC cluster required to trigger digitization
  integer MinMCClusters; 
  // Noise type 
  integer NoiseType;    
  // DSP seed strip S/N threshold per layer
  geant   DSPSeedThr[2][9]; 
  // DSP neigboring strips S/N threshold
  geant   DSPNeigThr[2];
 
  // TrSim2010 Parameters

  // ADC vertical MIP value
  geant   TrSim2010_ADCMipValue[2];
  // Apply p-strip deformation?
  integer TrSim2010_PStripCorr; 
  // Saturation limit for strips
  geant   TrSim2010_ADCSat[2]; 
  // Interstrip capacitance (pF)
  geant   TrSim2010_Cint[2];   
  // Backplane(+guardring) capacitance (pF)     
  geant   TrSim2010_Cbk[2];  
  // Readout capacitance (pF)
  geant   TrSim2010_Cdec[2]; 
  // Charge diffusion model
  integer TrSim2010_DiffType[2]; 
  // Diffusion radium (um)
  geant   TrSim2010_DiffRadius[2]; 
  // Tuning parameter: shaking a fraction
  geant   TrSim2010_FracNoise[2]; 
  // Tuning parameter: adding noise
  geant   TrSim2010_AddNoise[2];    
 
  TRMCFFKEY_DEF():TObject(){init();}
  void init();
  number ADC2KeV(){
    return 0.5e6/beta/dedx2nprel;
  }

  ClassDef(TRMCFFKEY_DEF,2);


};

#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);



//=============================================================

const integer nalg=4;
class TRCALIB_DEF: public TObject{
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
  TRCALIB_DEF():TObject(){init();}

  ClassDef(TRCALIB_DEF,1);

};


#define TRCALIB COMMON_BLOCK(TRCALIB,trcalib)
COMMON_BLOCK_DEF(TRCALIB_DEF,TRCALIB);

//=============================================================

class TRALIG_DEF: public TObject{
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
  
  TRALIG_DEF():TObject(){init();}
  ClassDef(TRALIG_DEF,1);

};
#define TRALIG COMMON_BLOCK(TRALIG,tralig)
COMMON_BLOCK_DEF(TRALIG_DEF,TRALIG);

//=============================
//==============================
//  TRACKER RECONSTRUCTION
//==============================
//! Data Cards to drive the Tracker reconstruction
class TRCLFFKEY_DEF : public TObject{
public:
  //! I 1 Drive the tracker Recon =0 No recon; =1 Cluster; =11 Cluster and hits; =111 Cluster, Hits and Tracks; if negative the MC digitization is redone
  integer recflag;
  //! R [2,19] Seed S/N Threshold per layer(1-9 -> 0-8)for 2-10:n(X) 11-19:p(Y) side
  geant     ThrSeed[2][9];  
  //! R [20,21] expansion S/N Threshold for 4:n(X) 5:p(Y) side
  geant     ThrNeig[2] ;
  //! I [22,23] Min Distance between seeds for 6:n(X) 7:p(Y) side
  integer   SeedDist[2] ;
  
  //! R [24 - 29] hit signal correlation (only muons/protons)
  geant GGpars[6]  ;//! {1428.,0.0000,0.1444,1645.,0.0109,0.0972};
  //! R 30 hit signal correlation (only muons/protons)
  geant GGintegral ;//! 91765.;
  //! R 31 Threshold on the S/K Correlation probability
  geant ThrProb    ;//! 0.00001;

  //! I 32 TrClusterR::DefaultCorrOpt
  int ClusterSigCorrOpt;
  //! I 33 TrClusterR::DefaultUsedStrips
  int ClusterCofGUsedStrips;
  //! R 34 _dxdz threshold for 2/3-strip CofG
  geant TwoStripThresholdX;
  //! R 35 _dydz threshold for 2/3-strip CofG
  geant TwoStripThresholdY;

  // HIT PARS
  //PRE-SELECTION PARS
  //! I 36 MaxNrawCls 
  int MaxNrawCls ;// 2000
  //! I 37 Threeshold used to define a small dt event
  int lowdt    ;// 200
  //! I 38 MaxNtrCls
  int MaxNtrCls  ;// 1000
  //! I 39 MaxNtrCls_ldt
  int MaxNtrCls_ldt;// 150
  //! I 40 MaxNtrHit
  int MaxNtrHit  ;// 1000
  //! F 41  Trackin time limit
  float TrTimeLim  ;// 1000
  //TRACK PARS
  //! I 42 MaxNtrack 
  int    MaxNtrack ;//4
  //! I 43 MinNhitX
  int    MinNhitX  ;//4
  //! I 44 MinNhitY
  int    MinNhitY  ;//5
  //! I 45 MinNhitXY
  int    MinNhitXY ;//4
  //! I 46 PatAllowOption
  int    PatAllowOption ;//0
  //! R 47 MaxChisqAllowed
  float MaxChisqAllowed   ;//300
  //! R 48  LadderScanRange
  float LadderScanRange   ;//7.3 = TkDBc::Head->_ladder_Ypitch
  //! R 49 ClusterScanRange
  float ClusterScanRange  ;//0.5
  //! R 50 MaxChisqForLScan
  float MaxChisqForLScan  ;//2.2
  //! R 51 ErrXForScan
  float ErrXForScan       ;//300e-4
  //! R 52 ErrYForScan
  float ErrYForScan       ;//300e-4
  //! R [53,54] Seed S/N Threshold for 37:n(X) 38:p(Y) side
  geant TrackThrSeed[2] ;  

  //! I 55 Advanced fit bits
  int AdvancedFitFlag;  // 0xffff

  //DEBUG
  //! I 56 TrDEBUG (DEBUG on Track finding)
  int TrDEBUG ;
  //! I 57 PZDEBUG
  int PZDEBUG ;
  //! I 58 TAS reconstruction
  int TasRecon;
  //! I 59 TAS current intensity
  int TasCurr;
  //! I 60 TAS LDDR mode
  int TasLDDR;
  //! I 61 mn;  m=1 TOF_match ON; n=1 Trd mathcing On; 
  int ExtMatch;
  //! I 62 Use Sensor Alignement 1=yes 0=no; must be 0 for MC
  int UseSensorAlign;
  //! I 63  ReconStats flag  0=no 1=yes (default 1)
  int statflag;
  TRCLFFKEY_DEF():TObject(){init();}
  void init();
  ClassDef(TRCLFFKEY_DEF,3);

};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);

//
const integer npat=42;
const integer npat02=218;

//=============================================================

//! Data Cards to drive the track fiting
class TRFITFFKEY_DEF: public TObject {
public:
  /// R 1 fit point error
  float ErrX;
  /// R 2 fit point error
  float ErrY;
  /// R 3 fit point error
  float ErrZ;

  /// R [4,12] Error term for Multiple scattering per layer (1-9)
  float FitwMsc[9];

  /// R 13 max dist to add external planes hits X
  float MergeExtLimX;
  /// R 14 max dist to add external planes hits Y
  float MergeExtLimY;

  /// R 15 default mass for the fitting
  float DefaultMass;

  /// R 16 default charge for the fitting
  float DefaultCharge;

  /// R [17,18] thresholds for TrRecon::TryDropX
  float RthdDropX[2];

  TRFITFFKEY_DEF():TObject(){init();}
  void init();

  /// Read from AMSRoot file or not
  static int ReadFromFile;

  ClassDef(TRFITFFKEY_DEF,4);
};
//


#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);

#endif
