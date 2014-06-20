// $Id$
#ifndef tkcommons_h
#define tkcommons_h
#ifdef _PGTRACK_
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
  ///[807]; Enable MC disalign (0 disabled(default), 1 from default file, 2 from file specified in disfname)
  integer LoadMCDisalign;
  ///[808]; Enable ext planes time dependent disalignment
  integer EnableExtMov;
  ///[809,814]; L1 Amplitude(cm,rad) of dx,dy,dz,dTheta_XY,dTheta_YZ,dTheta_XZ
  float L1Amp[6];
  ///[815,820]; L1 relative phase ofdx,dy,dz,dTheta_XY,dTheta_YZ,dTheta_XZ
  float L1Phase[6];
  ///[821,826]; L9 Amplitude(cm,rad) of dx,dy,dz,dTheta_XY,dTheta_YZ,dTheta_XZ
  float L9Amp[6];
  ///[827,832]; L9 relative phase ofdx,dy,dz,dTheta_XY,dTheta_YZ,dTheta_XZ
  float L9Phase[6];
  ///[833] PG External Alignment version; (1: no errors,  2: with errors)
  integer exalignver;
  ///[834] CIEMAT External Alignment tag; (0: default,  otherwise version tag)
  integer MdExAlignTag;
  ///[835] PG/MD Max aligned run
  integer MaxAlignedRun;
  void init();
  TKGEOMFFKEY_DEF():TObject(){init();}
  ClassDef(TKGEOMFFKEY_DEF,8);

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

  integer ActivateShielding;
  geant ShieldingDensity;

  // Outer smearing to simulate residual disalignment [0:L1,1:L9][0:X,1:Y]
  geant OuterSmearing[2][2];

  // new stuff 2013 (PZ)

  // Additional smearing of the MC true position
  //                         p_x     p_y       He_x    He_y
  geant SmearPos[2][2];//={{0.0008,      0.},{0.0008,  0.0}};

  // Enegy smearing, scaling, and convert to ADC
  //                          p_x p_y  He_x  He_y
  geant ADCMipValue[2][2];//={ {44, 32},{46,    32.}};
  geant SigQuadLoss[2][2];//={{0.0002,0.0004},{0.0001,0.00022}};

  // multiplication of the strips nearbt the gap
  float NearStripMult[2][2][3];//={{{1.,  8., 8.},{1.3, 6., 8.5}},    {{1.1,  6., 8.},{1.3, 5.,  5.}} };

  // additional noise fraction in the signal region
  geant noise_fac[2];//={1.1,1.45};
  // 1 sigma correction to tune the number of noise cluster
  float GlobalNoiseFactor[2];//={1.08,1.037};

  // 2014.05.23 SH
  // Workaround to retune the MC resolution (not activated by default)
  geant MCtuneDmax;  //=0  typically put 100e-4 to activate
  geant MCtuneDs;    //=0  typically put   1e-4 to improve the resolution
                     //                   -1e-4 to smare
                     //    if MCtuneDs > MCtuneDmax, use exactly as MC coo
  geant MCtuneDy9;   //=0  typically put   1e-4 to mitigate the propagation bug

  // parameter for simple ion tuning (2014.06.14 AO) 
  int   UseNonLinearity;

  /// Read from AMSRoot file or not
  static int ReadFromFile;

  TRMCFFKEY_DEF():TObject(){init();}
  void init();
  number ADC2KeV(){
    return 0.5e6/beta/dedx2nprel;
  }

  ClassDef(TRMCFFKEY_DEF,8);


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
  ClassDef(TRALIG_DEF,2);

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
  integer   recflag;
  //! R [2,19] Seed S/N Threshold per layer(1-9 -> 0-8)for 2-10:n(X) 11-19:p(Y) side
  geant     ThrSeed[2][9];  
  //! R [20,21] expansion S/N Threshold for 4:n(X) 5:p(Y) side
  geant     ThrNeig[2];
  //! I [22,23] Min Distance between seeds for 6:n(X) 7:p(Y) side
  integer   SeedDist[2];
  
  //! R [24,29] hit signal correlation (only muons/protons) (DEPRECATED)
  geant GGpars[6]; //! {1428.,0.0000,0.1444,1645.,0.0109,0.0972};
  //! R 30 hit signal correlation (only muons/protons) (DEPRECATED)
  geant GGintegral; //! 91765.; 
  //! R 31 Threshold on the S/K Correlation probability (DEPRECATED)
  geant ThrProb; //! 0.00001;

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
  int MaxNrawCls; // 2000
  //! I 37 Threeshold used to define a small dt event
  int lowdt; // 200
  //! I 38 MaxNtrCls
  int MaxNtrCls; // 1000
  //! I 39 MaxNtrCls_ldt
  int MaxNtrCls_ldt;// 150
  //! I 40 MaxNtrHit
  int MaxNtrHit; // 1000
  //! R 41  Trackin time limit
  geant TrTimeLim; // 1000
  //TRACK PARS
  //! I 42 MaxNtrack 
  int    MaxNtrack; //4
  //! I 43 MinNhitX
  int    MinNhitX; //4
  //! I 44 MinNhitY
  int    MinNhitY; //5
  //! I 45 MinNhitXY
  int    MinNhitXY; //4
  //! I 46 PatAllowOption
  int    PatAllowOption; //0
  //! R 47 MaxChisqAllowed
  geant MaxChisqAllowed; //300
  //! R 48  LadderScanRange
  geant LadderScanRange; //7.3 = TkDBc::Head->_ladder_Ypitch
  //! R 49 ClusterScanRange
  geant ClusterScanRange; //0.5
  //! R 50 MaxChisqForLScan
  geant MaxChisqForLScan; //2.2
  //! R 51 ErrXForScan
  geant ErrXForScan; //300e-4
  //! R 52 ErrYForScan
  geant ErrYForScan; //300e-4
  //! R [53,54] Seed S/N Threshold for 37:n(X) 38:p(Y) side
  geant TrackThrSeed[2];  
  //! I 55 Advanced fit bits
  int AdvancedFitFlag;  // 0xffff

  // DEBUG
  //! I 56 TrDEBUG (DEBUG on Track finding)
  int TrDEBUG;
  //! I 57 PZDEBUG
  int PZDEBUG;

  // TAS
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

  // New parameters for reconstruction (2012)
  //! I 64 BuildRecHits: tagging hits and clusters compatible with charge seed 
  int   ChargeSeedTagActive;
  //! I 65 BuildRecHits: eliminate hits not compatible with charge seed (high-z clean rec.)
  int   ChargeSeedSelActive;
  //! R 66 BuildRecHits: hit correlation probability threshold
  geant CorrelationProbThr;
  //! R 67 BuildRecHits: choose only-Y hits with a cluster Signal-to-Noise over the threshold 
  geant YClusterSNThr;
  //! I 68 BuildRecHits: minimum number of strips for only-Y hits 
  int   YClusterNStripThr;

  //! I 69 Build: reorder cluster by signal 
  int   ReorderTrClusters;
  //! I 70 Build: reorder hits by correlation probability (1), by y-signal and probability (2) or by y-signal and x-signal 
  int   ReorderTrRecHits;
  //! I 71 Build: reconstruct first a track with hits compatible with the unbiased charge seed (only for TrTracksSimple) 
  int   BuildTracksSimpleChargeSeedTag;
 
  //! I 72 BuildTrTracksSimple: charge cut during track finding
  int   TrackFindChargeCutActive;
 
  //! I 73 BuildTrTracksSimple: Numver of x candidates to use help of TRD
  int   NhitXForTRDTrackHelp;

  //! R 74 MatchTOF_TRD: Maximum logChisqX to declare a good multiplicity resolution after TRD match
  geant logChisqXmax; 

  TRCLFFKEY_DEF():TObject(){init();}
  void init();
  ClassDef(TRCLFFKEY_DEF,6);

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

  /// R [19,20] Factor on ErrX/ErrY for He
  float ErcHeX;
  float ErcHeY;
  /// I 21 Activates the multiple Alignmnet fitting during the reconstruction (1=PG 2=PG+MD 3=PG+MD+Averagae)
  int  MultipleAlign;
  /// I 22 Determine the max |X| accepted to associate a layer 9 hit to the track
  float EcalMaxX;
  // I  23 magnet temp correction enabled
  int   magtemp;

  /// R 24 Additional fitting error (Layer 1) due to misalignment
  float ErrYL1;
  /// R 25 Additional fitting error (Layer 9) due to misalignment
  float ErrYL9;


  TRFITFFKEY_DEF():TObject(){init();}
  void init();

  /// Read from AMSRoot file or not
  static int ReadFromFile;

  ClassDef(TRFITFFKEY_DEF,9);
};
//

#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);

//=============================================================
//! Data card driving the Tracker charge reconstruction 
class TRCHAFFKEY_DEF : public TObject {
 public:
  /// I 1 reconstruction type (default 2): 0 = no recon, 1 = trunc. mean, 2 = likelihood
  int ReconType;
  /// I 2 pdf version used (default 1): 0 = original (v5.00 dbase, from MC), 1 = first implementation (sep. 2011)
  int PdfVersion;
  /// I 3 max charge recontructed (default 8)
  int MaxCharge;
  /// I 4 enable histograms (default 0): 0 = disabled, 1 = enabled
  int EnableHisto;
  /// c-tor
  TRCHAFFKEY_DEF() : TObject() { init(); }
  /// initializer
  void init();
  /// ROOT class definition
  ClassDef(TRCHAFFKEY_DEF,1);
};
//

#define TRCHAFFKEY COMMON_BLOCK(TRCHAFFKEY,trchaffkey)
COMMON_BLOCK_DEF(TRCHAFFKEY_DEF,TRCHAFFKEY);

#endif
#endif
