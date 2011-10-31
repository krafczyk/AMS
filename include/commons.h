//  $Id: commons.h,v 1.327.4.2 2011/10/31 17:13:07 barao Exp $
//  Author V. Choutko 24-may-1996
//
//  To developpers:
//  All new vars to cblocks defs: only at the very end !!!
//
///

#ifndef __AMSCOMMONS__
#define __AMSCOMMONS__
#include "typedefs.h"
// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

#include "commonsi.h"
#ifdef _PGTRACK_
#include "tkdcards.h"
#endif


#ifdef __AMSVMC__
#include "geant3function_redefine.h"
#endif

#ifdef _OPENMP
#include <omp.h>
#endif


class TRMFFKEY_DEF{
public:
integer OKAY;
integer TIME;
};
#define TRMFFKEY COMMON_BLOCK(TRMFFKEY,trmffkey)
COMMON_BLOCK_DEF(TRMFFKEY_DEF,TRMFFKEY);

class MISCFFKEY_DEF{
public:
integer BTempCorrection;
integer BeamTest;
float   BZCorr; 
integer G3On;
integer G4On;
integer dbwrbeg;
integer dbwrend;
integer NumThreads;
integer DynThreads;
integer ChunkThreads;
integer DivideBy;
integer RaiseFPE;
};
COMMON_BLOCK_DEF(MISCFFKEY_DEF,MISCFFKEY);


class SELECTFFKEY_DEF {
public:
integer Run;
integer Event;
integer File[40];
integer RunE;
integer EventE;
};
#define SELECTFFKEY COMMON_BLOCK(SELECTFFKEY,selectffkey)
COMMON_BLOCK_DEF(SELECTFFKEY_DEF,SELECTFFKEY);

class STATUSFFKEY_DEF {
public:
uinteger status[36];
};
#define STATUSFFKEY COMMON_BLOCK(STATUSFFKEY,statusffkey)
COMMON_BLOCK_DEF(STATUSFFKEY_DEF,STATUSFFKEY);


class HVSEC_DEF {
public:
  int ihvse[2];
};
#define HVSEC COMMON_BLOCK(HVSEC,hvsec)
COMMON_BLOCK_DEF(HVSEC_DEF,HVSEC);
  

class DAQCFFKEY_DEF {
public:
integer mode;
integer ifile[40];
integer ofile[40];
integer BTypeInDAQ[2];
integer DAQVersion;
integer Mode;
integer ReadAhead;
integer BlocksDir[40];
integer SkipRec;
};
#define DAQCFFKEY COMMON_BLOCK(DAQCFFKEY,daqcffkey)
COMMON_BLOCK_DEF(DAQCFFKEY_DEF,DAQCFFKEY);



class LVL3SIMFFKEY_DEF{
public:
geant NoiseProb[2];
};
#define LVL3SIMFFKEY COMMON_BLOCK(LVL3SIMFFKEY,lvl3simffkey)
COMMON_BLOCK_DEF(LVL3SIMFFKEY_DEF,LVL3SIMFFKEY);

class LVL3EXPFFKEY_DEF{
public:
integer NEvents;
integer ToBad;
integer TryAgain;
geant   Range[3][2];
};
#define LVL3EXPFFKEY COMMON_BLOCK(LVL3EXPFFKEY,lvl3expffkey)
COMMON_BLOCK_DEF(LVL3EXPFFKEY_DEF,LVL3EXPFFKEY);



class LVL3FFKEY_DEF {
public:
integer MinTOFPlanesFired;
integer UseTightTOF;
geant TrTOFSearchReg; 
geant TrMinResidual;
geant TrMaxResidual[3];
integer TrMaxHits;
geant Splitting;
integer NRep;
integer Accept;
integer RebuildLVL3;
integer NoK;
geant TrHeavyIonThr;
integer SeedThr;
integer TRDHMulThr;
geant  TRDHMulPart;
integer Stat;
integer histprf;
};
#define LVL3FFKEY COMMON_BLOCK(LVL3FFKEY,lvl3ffkey)
COMMON_BLOCK_DEF(LVL3FFKEY_DEF,LVL3FFKEY);



class LVTMFFKEY_DEF {
public:
integer BufSize;
float MinFTime;
float MinTime;
float MeanTime;
};
#define LVTMFFKEY COMMON_BLOCK(LVTMFFKEY,lvtmffkey)
COMMON_BLOCK_DEF(LVTMFFKEY_DEF,LVTMFFKEY);




class AMSFFKEY_DEF {
public:
integer Simulation;
integer Reconstruction;
integer Jobtype;
integer Debug;
geant   CpuLimit;
integer Read;
integer Write;
integer Jobname[40];
integer Setupname[40];
integer ZeroSetupOk;
integer Update;
integer TDVC[400];
};
#define AMSFFKEY COMMON_BLOCK(AMSFFKEY,amsffkey)
COMMON_BLOCK_DEF(AMSFFKEY_DEF,AMSFFKEY);
//==============================================================
class IOPA_DEF {
public:
integer hlun;
integer hfile[40];
integer ntuple;
integer WriteAll;
float   Portion;
integer TriggerC[40];
integer mode;
integer ffile[40];
integer MaxNtupleEntries;
integer WriteRoot;
integer rfile[40];
uinteger MaxFileSize;
integer MaxFileTime;
integer BuildMin;
integer histoman;
integer skip;
integer WriteTGeometry;
integer TGeometryFileName[40];
integer WriteTDVDataInRoot;
integer ReadAMI;
  integer unitimegen;
  float unitimegenrate;
#ifdef __AMSVMC__
integer VMCVersion;
integer VMCEventno;
#endif
};
#define IOPA COMMON_BLOCK(IOPA,iopa)
COMMON_BLOCK_DEF(IOPA_DEF,IOPA);
//==============================================================
class TFMCFFKEY_DEF {
public:
geant TimeSigma;
geant sumHTdel;
geant canormf;
geant dzconv;
geant Thr;
//
integer mcprtf[5];     // TOF MC-print flag (=0 -> no_printing)
integer trlogic[2]; // to control trigger logic
integer fast;    // 1/0 fast/slow algo;
integer daqfmt;  // 0/1 raw/reduced TDC format for DAQ
integer birks;     // 0/1  not apply/apply birks corrections
integer mcseedo;   // 0/1->use MCCalib/RealData files as MCseed
geant blshift;     // base line shift
geant hfnoise;     // high freq. noise
integer ReadConstFiles;
integer addpeds;
integer calvern;//  TofCflistMC-file version number (file extention)
integer tdclin;//flag to activate(if =1) TDC-linearity logic for MC
integer tdcovf;//flag to activate(if =1) TDC-ovfl protection logic
};
#define TFMCFFKEY COMMON_BLOCK(TFMCFFKEY,tfmcffkey)
COMMON_BLOCK_DEF(TFMCFFKEY_DEF,TFMCFFKEY);
//==============================================================

#ifndef _PGTRACK_
class MAGSFFKEY_DEF {
public:
integer magstat; // status: 0/1-> off/on
geant fscale;    // nom.field reduction
geant ecutge;    // e/g energy cut for tracking in magnet materials
geant r0[3];     // shift & rotation of mag field map w/r nom
geant pitch;
geant yaw;
geant roll;
integer rphi;    // use xyz (0) or rphiz (1) grid
integer begin;    // begin time
integer end;    //end time
};
#define MAGSFFKEY COMMON_BLOCK(MAGSFFKEY,magsffkey)
COMMON_BLOCK_DEF(MAGSFFKEY_DEF,MAGSFFKEY);
#endif
//==============================================================


class TRDMCFFKEY_DEF{
public:
integer mode; // 0: saveliev 1: saveliev+Pai 2: garibyan + Pai
integer g3trd;  // g3identifier for trd aware media
float cor;     // saveliev parameter
float alpha;    // garibyan pars
float beta;    //  garibyan par
geant ped;     
geant pedsig;     
geant sigma;
geant gain;
geant cmn;
geant   f2i;
integer adcoverflow;
integer NoiseOn;
float GeV2ADC;
float Thr1R;
integer GenerateConst;
geant NoiseLevel;
geant DeadLevel;
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
integer multiples;
 geant  Gdens;
 geant  Pecut;
 geant  ntrcor;
 geant  etrcor;
 geant  ndecor;
 geant  edecor;

 geant Tmean;
 geant Pmean;
 geant FibreDiam;
 geant RadThick;
 geant AlphaFibre;
 geant AlphaGas;
 integer FleeceGas;
 geant XenonFraction;
 geant ArgonFraction;
 geant CO2Fraction;
 geant GasStep;
 integer PAIModel;
 integer debug;
 int CreatePDF;
};
#define TRDMCFFKEY COMMON_BLOCK(TRDMCFFKEY,trdmcffkey)
COMMON_BLOCK_DEF(TRDMCFFKEY_DEF,TRDMCFFKEY);


class TRDFITFFKEY_DEF{
public:
  float Chi2StrLine;     //  fit max chi2
  float ResCutStrLine;   // Max acc distance between point and fitted curve 
  float SearchRegStrLine;  
  int MinFitPoints;
  float TwoSegMatch;
  float MaxSegAllowed; 

  int FitMethod;
  int SaveHistos;
  float CalStartVal;
  float QP[2];
  integer UseTOF;
};
#define TRDFITFFKEY COMMON_BLOCK(TRDFITFFKEY,trdfitffkey)
COMMON_BLOCK_DEF(TRDFITFFKEY_DEF,TRDFITFFKEY);

class TRDRECFFKEY_DEF{
public:
  float Chi2StrLine;     //  fit max chi2
  float ResCutStrLine;   // Max acc distance between point and fitted curve 
  float SearchRegStrLine;  
  float CCAmpCut;        // cut value for likelihood analysis
  int ResolveAmb;       
};
#define TRDRECFFKEY COMMON_BLOCK(TRDRECFFKEY,trdrecffkey)
COMMON_BLOCK_DEF(TRDRECFFKEY_DEF,TRDRECFFKEY);
class TRDCLFFKEY_DEF{
public:
  float ADC2KeV;         //
  float Thr1A;           // cluster threshold amp kev
  float Thr1S;           // cluster threshold sigma kev
  float Thr1R;           // cluster threshold ratio kev
  float Thr1H;           // cluster threshold high amp kev
  int   MaxHitsInCluster; 
  float RNGP[3];    //  de/dx vs range correctionf function 
  float RNGB[2];    //  correction range   ---
};
#define TRDCLFFKEY COMMON_BLOCK(TRDCLFFKEY,trdclffkey)
COMMON_BLOCK_DEF(TRDCLFFKEY_DEF,TRDCLFFKEY);


//==================================================================
class ECMCFFKEY_DEF {
public:
integer fastsim;  //1/0-> fast/slow simulation
integer mcprtf;   //1/0-> print/not mc-histogr.
geant cutge;// cutgam=cutele cut in EC_radiator
integer silogic[2];// SIMU logic flags
geant mev2mev;
geant mev2adc;
geant safext;     // Extention(cm) of EC transv.size when TFMC 13=2 is used
geant mev2pes;
geant pmseres;
geant adc2q;//adc->q conv.factor(pC/adc)
geant fendrf;//fiber end-cut reflection factor
geant physa2d;//physical an/dyn ratio(mv/mv)
geant hi2low;//not used now
geant sbcgn;//not used now
geant pedh; //Ped-HiCh    
geant pedvh;//ch-to-ch variation(%)     
geant pedl; //Ped-LoCh    
geant pedvl;//ch-to-ch variations(%)
geant pedsh;//PedSig-HiCh     
geant pedsvh;//ch-to-ch variation(%)     
geant pedsl;//PedSig-LoCh    
geant pedsvl;//ch-to-ch variation(%)
geant pedd; //Ped-DyCh    
geant peddv;//ch-to-ch variation(%)     
geant pedds;//PedSig-DyCh    
geant peddsv;//ch-to-ch variation(%)
integer ReadConstFiles;
integer calvern;
integer mch2root;
 float Sl_gap;
 float HoneyRelDen;
 float SbMassFrac; // Antimonium mass fraction in Lead
 float effmn; // pixel efficiency correction
 float deffw;
 float cladgluex; // cladding+glue thickness(cm) horizontal
 float cladgluey; // cladding+glue thickness(cm) vertical
 float gap; // thickness of the gap from fiber to fiber 
 integer endplate;// flag to insert Aluminum end plate (0=not inserted) 
 float claddxy[36]; // clad+glue thick. horiz./vert. variation in layer
};
#define ECMCFFKEY COMMON_BLOCK(ECMCFFKEY,ecmcffkey)
COMMON_BLOCK_DEF(ECMCFFKEY_DEF,ECMCFFKEY);
//---
class ECREFFKEY_DEF {
public:
  integer reprtf[3];   //reco print-flags
  integer relogic[5];  //reco logic-flags
  geant thresh[15];       //Time dependent DAQ/Trig-thresholds
  geant cuts[10];       // ........ RECO cuts (clust.thr.,...)
  integer ReadConstFiles;
  integer calutc;//EcalCflistRD-file extention(utc-time)
//
  float Thr1DSeed;        // Threshold for 1d cluster search
  float Thr1DRSeed;        // Threshold for 1d cluster search
  int   Cl1DCoreSize;
  int   Cl1DLeakSize; 
  float Thr2DMax;
  float Length2DMin;
  float Chi22DMax;
  float PosError1D;
  float Chi2Change2D;
  float TransShowerSize2D;
  float SimpleRearLeak[4]; 
  float LAPPRearLeak[4]; //LAPP rear-leakage correction parameters (M.P.)

  float ealpha0;
  float ebeta;
  float egamma;
  float ealpha_par[2];

  float CalorTransSize;
  float EMDirCorrection;
  float HiEnThr;
  float HiEnCorFac;
  float S1S3[4];
  float S1S3X[6];
  float S1S3Y[6];
  float S1S3XA[4]; //LAPP impact-point correction parameters X side (M.P.)
  float S1S3YA[4]; //LAPP impact-point correction parameter Y side (M.P.)

  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define ECREFFKEY COMMON_BLOCK(ECREFFKEY,ecreffkey)
COMMON_BLOCK_DEF(ECREFFKEY_DEF,ECREFFKEY);
//---
class ECCAFFKEY_DEF {
public:
  integer hprintf;// ECcalib-related hist.print flag
  integer cafdir;// 0/1-> use officical/private directory for calib.files
  integer prtuse; // 1/0/-1 -> use He/pr/mu for calib
  integer refpid;// ref.pm id
//   RLGA+FIAT part
  geant trmin;// presel.cut on min. rigidity of the track
  geant adcmin;//cut on min ADC-value of indiv.sub-cell
  geant adcpmx;//cut on max ADC-value of indiv.PM
  integer ntruncl;// remove this number of highest layers
  geant trxac;// TRK->EC extrapolation accuracy in X-proj
  geant tryac;// TRK->EC extrapolation accuracy in Y-proj
  geant mscatp;// EC.mult.scat.param.
  integer nortyp;// normalization type
  integer badplmx;// max. accept. bad sc-planes 
  geant etrunmn;  // Min ECenergy (Etrunc in mev) to select particle(p or He ...)
  geant etrunmx;  // Max ECenergy (Etrunc in mev) ...............................
  geant nsigtrk;  // safety gap in TRK-accur. units for crossing check procedure
//   ANOR part
  geant pmin;       // presel-cut on min. mom. of the track(gev/c) 
  geant pmax;       // presel-cut on max. mom. of the track 
  geant scmin;      // min ADC for indiv. SubCell (to remove ped,noise)
  geant scmax;      // max ADC .................. (to remove sparks,ovfl,...)
  integer spikmx;   // max SC's(spikes) with ADC>above max
  integer nhtlmx[6];   // max hits in sc-plane 1-6(to remove early showering)
  integer lmulmx;      // max hits/sc-plane (to remove events with abn.multiplicity)
  integer nholmx[6];   // max holes(betw.fired cells) in sc-plane 1-6(early show.prot)
  integer nbplmx;      // max bad sc-planes (with spikes or high(abn) multiplicity)
  geant edtmin;        // min Etot(mev) to remove MIP
  geant esleakmx;      // max Eleak(side)/Etot to remove energy side leak
  geant ebleakmx;      // max Eleak(back)/Etot
  geant edfrmn;        // min Efront(mev)
  geant edt2pmx;       // max Etail/Epeak
  geant ed2momc;       // Edep(EC)/Mom(TRK)-1 cut
  geant cog1cut;       // Track-ScPlaneCOG mismatch cut(cm) for the 1st t6 SC-planes.
  geant scdismx[6];    // max sc-track dist. to consider hit as backgroubd(pl 1-6) 
  geant scdisrs;       // as above for all other planes(not used really)
  geant b2scut[6];     // max backgr/signal energy(bound.from above) for pl 1-6
  geant pedcpr;        //PedCal: def portion of highest amplitude to remove for ped calc.
  integer pedoutf;     //        PedOutpFlag 
  geant pedlim[2];     // Ped-limits for PedCalibJobs
  geant siglim[2];     // PedSig-limits .............
//g.chen
  integer ecshswit;      // 0/1 switch to/(not to)  use shower info in calibration
  geant chi2dirmx;     // max chi2 of shower dir fit
  geant prchi2mx;      // max chi2 of shower profile fit
  geant trchi2mx;      // max chi2 of shower trans. fit
  geant eshsleakmx;    // max Eleak(shower side)/Etot
  geant eshrleakmx;    // max Eleak(shower back)/Etot
  geant eshdleakmx;    // max Eleak(shower dead)/Etot
  geant esholeakmx;    // max Eleak(shower orph)/Etot
  geant difsummx;      // cut of (E_x-E_y)/(E_x+E_y)
  geant trentmax[2];   // cut of distance between track and shower entry
  geant trextmax[2];   // cut of distance between track and shower exit
//
  integer onbpedspat;  // bit-patt of present sections in OnBoardPed block
};
#define ECCAFFKEY COMMON_BLOCK(ECCAFFKEY,eccaffkey)
COMMON_BLOCK_DEF(ECCAFFKEY_DEF,ECCAFFKEY);

#ifndef _PGTRACK_
//===================================================================
class TKGEOMFFKEY_DEF{
public:
  integer ReadGeomFromFile;
  integer WriteGeomToFile;
  integer UpdateGeomFile;
  integer gfile[40];
};
#define TKGEOMFFKEY COMMON_BLOCK(TKGEOMFFKEY,tkgeomffkey)
COMMON_BLOCK_DEF(TKGEOMFFKEY_DEF,TKGEOMFFKEY);

class TRMCFFKEY_DEF {
public:
geant alpha;
geant beta;
geant gamma;
geant dedx2nprel;
geant fastswitch;
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
};
#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);
//
const integer npat=42;
class TRFITFFKEY_DEF {
public:
  integer patternp[npat];  //patterns  to fit; Priority decreases with number
  //    Number              Descr             Points      Default
  //    0                   123456            6           on
  //    1                   12346             5           on
  //    2                   12356             5           on
  //    3                   12456             5           on
  //    4                   13456             5           on
  //    5                   12345             5           off
  //    6                   23456             5           off
  //    7                   1234              4           off
  //    8                   1235              4           off
  //    9                   1236              4           off
  //   10                   1245              4           off
  //   11                   1246              4           off
  //   12                   1256              4           off
  //   13                   1345              4           off
  //   14                   1346              4           off
  //   15                   1356              4           off
  //   16                   1456              4           off
  //   17                   2345              4           off
  //   18                   2346              4           off
  //   19                   2356              4           off
  //   20                   2456              4           off
  //   21                   3456              4           off
  //   .... 3 points .....
  integer UseTOF;     // Use(>=1)/Not use(0) 
                      // TOF info for pattern rec for pattern 16
  geant Chi2FastFit;  //  Max acceptable chi2 for tkhmd; 
  geant Chi2StrLine; // The same for Straight line fit
  geant Chi2WithoutMS;  // The same for PI fit withut multiple scattering
  geant ResCutFastFit;   // Max acceptible distance betgween point and fitted curve 
  geant ResCutStrLine;    // ---
  geant ResCutCircle;     // ---
  geant SearchRegFastFit;   // Max distance between point and curve to include pint un the fit
  geant SearchRegStrLine;  // -------
  geant SearchRegCircle;   // ----------
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
  integer UseTRD;     // Use(>=1)/Not use(0) 
  integer LowMargin;
  integer OnlyGammaVtx;
  integer UseGeaneFitting; // Use Geane(1) or Yale (0) fitting method
  integer OldTracking;  //Use Old (VC) or new (JA) pattern recognition
  integer MainAlg;     //
  integer AddMS;      // Add MS error to ext planes
   integer B23[2];    //  angle to switch 2 to 3 strips cluster cofg calculation
};
//


#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);
class TRCLFFKEY_DEF {
public:
geant ThrClA[2];
geant ThrClS[2];
geant ThrClR[2];
geant Thr1A[2];
geant Thr1S[2];
geant Thr1R[2];
geant Thr2A[2];
geant Thr2S[2];
geant Thr2R[2];
geant Thr3R[2];
integer ThrClNMin[2];
integer ThrClNEl[2];
geant ThrDSide;
geant ErrX;
geant ErrY;
geant ErrZ;
geant CorFunParA[2][6];
geant CorFunParB[2][6];
geant ResFunX[3][6];
geant ResFunY[3][6];
geant CommonGain[2];
geant CommonShift[2];
geant EtaCor[2];
};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);


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
integer ActiveParameters[9][6]; //   for each plane: x,y,z, pitch, yaw, roll
integer GlobalFit;
integer EventsPerRun;
integer LayersOnly;
geant GlobalGoodLimit;
integer SingleLadderEntryLimit;
geant One;
integer gfile[40];
integer LaddersOnly;
integer Skip;
integer Max;
};
#define TRALIG COMMON_BLOCK(TRALIG,tralig)
COMMON_BLOCK_DEF(TRALIG_DEF,TRALIG);

#endif

//===================================================================
class ATGEFFKEY_DEF {
public:
  integer nscpad;  // number of scintillator paddles 
  geant scradi;    // internal radious of ANTI sc. cylinder (cm)
  geant scinth;    // thickness of scintillator (cm)
  geant scleng;    // scintillator paddle length (glob. Z-dim)
  geant wrapth;    // wrapper thickness (cm)
  geant groovr;    // groove radious (bump_rad = groove_rad - pdlgap)
  geant pdlgap;    // inter paddle gap (cm)
  geant stradi;    // support tube intern.radious
  geant stleng;    // support tube length
  geant stthic;    // support tube thickness
};
#define ATGEFFKEY COMMON_BLOCK(ATGEFFKEY,atgeffkey)
COMMON_BLOCK_DEF(ATGEFFKEY_DEF,ATGEFFKEY);
//===================================================================
class ATMCFFKEY_DEF {
public:
integer mcprtf;// hist. print flag
geant FTdel;
geant LSpeed;
//
integer ReadConstFiles;
integer calvern;
//
};
#define ATMCFFKEY COMMON_BLOCK(ATMCFFKEY,atmcffkey)
COMMON_BLOCK_DEF(ATMCFFKEY_DEF,ATMCFFKEY);
//===================================================================
class ATREFFKEY_DEF {
public:
  integer reprtf[2];//  print flag
  integer relogic;// reco logic
  geant Edthr; // threshold (mev) to create Cluster-object
  geant zcerr1;// long.coo error(when 2-sides times are known) 
  integer nlcorr; //apply non-lin corrections to raw amplitudes 
  geant ftdel; // FT-delay wrt hist-TDC hit 
  geant ftwin; // t-window(ns) for Hist-TDC hit coinc.with FT 
//
  integer ReadConstFiles;
  uinteger calutc;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define ATREFFKEY COMMON_BLOCK(ATREFFKEY,atreffkey)
COMMON_BLOCK_DEF(ATREFFKEY_DEF,ATREFFKEY);
//================================================================
class ATCAFFKEY_DEF {
  public:
  integer trackmode; // use trk/trk+trd-track 
  integer cafdir; // use official/private directory for calib.file
  geant pedcpr[2]; //PedCalibJob: Class/DownScaled: portion of highest adcs to remove for ped-calc
  integer pedoutf;//              PedOutputFlag
  geant pedlim[2];// Ped-limits for PedCalibJobs
  geant siglim[2];// PedSig-limits .............
  geant mev2mv;//mev->mV conv.factor(side ampl[mV]/1MIP(mev) Edep at center)
};
#define ATCAFFKEY COMMON_BLOCK(ATCAFFKEY,atcaffkey)
COMMON_BLOCK_DEF(ATCAFFKEY_DEF,ATCAFFKEY);
//================================================================
class TFREFFKEY_DEF {
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
//
  integer reprtf[5]; //RECO print flag 
  integer relogic[5];//RECO logic flag
  geant daqthr[5];// daq thresholds
  geant cuts[10];// cuts 
//
  integer ReadConstFiles;
//
  integer TempHandlMode;
  uinteger calutc;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define TFREFFKEY COMMON_BLOCK(TFREFFKEY,tfreffkey)
COMMON_BLOCK_DEF(TFREFFKEY_DEF,TFREFFKEY);
//===================================================================
class TFCAFFKEY_DEF {
public:
// TZSL-calibration :
geant pcut[2];//low/high limits on momentum of calibr. events
geant bmeanpr;// mean proton velocity in this mom. range
geant tzref[2];// def. T0 for reference counter + spare
geant fixsl;// def. for slope
geant bmeanmu;// mean muon velocity in this mom. range
integer idref[2];// LBB for ref.counter and fix/release flag for trapez.counters
integer ifsl;// 0/1 to fix/release slope param.
integer caltyp;// 0/1 to select space/earth calibration
// AMPL-calibration :
integer truse;// 1/0 to use/not tracker
geant plhc[2];//low/high cuts on tracker mom. for space calibration
integer minev;// min.events needed for measurement in channel or bin
geant trcut;// cut to use for "truncated average" calculation (0.85)
integer spares[4];//
geant adc2q;//adc->charge conv.factor(pC/ADCch)(hope = for all ADC chips)
geant plhec[2];//low/high cuts on tracker mom. for earth calibration
geant bgcut[2];// beta*gamma low/high cuts for mip in abs.calibration
integer tofcoo;// 0/1-> use transv/longit coord. from TOF
geant tofbetac;// if !=0 -> low beta cut (own TOF measurement !!!) 
//                to use when previous calibration suppose to be good enought
integer hprintf;// hist.print flag
integer cafdir;// 0/1->use officical/private directory for calib.files
integer mcainc;// spare
geant pedcpr[2];//PedCalJob: portion of highest adcs to remove for ped-calc(Class/DScal)
integer pedoutf;//           PedOutputFlag
geant pedlim[2];// Ped-limits for PedCalibJobs
geant siglim[2];// PedSig-limits .............
// TOFTdcCalib (LINC)
integer minstat;//min.stat/ch
integer tdccum;//tdc-calib usage mode
//
integer onbpedspat;//bit-pattern for onboard ped-table sections
//
integer updbrcaldb;//update RD barcal DB "on flight"
};
#define TFCAFFKEY COMMON_BLOCK(TFCAFFKEY,tfcaffkey)
COMMON_BLOCK_DEF(TFCAFFKEY_DEF,TFCAFFKEY);
//===================================================================
class TGL1FFKEY_DEF {
public:
integer trtype;
integer toflc;
integer tofsc;
integer toflcsz;
integer toflcz;
integer tofextwid;
integer antismx[2];
integer antisc;
integer cftmask;
integer RebuildLVL1;
geant MaxScalersRate;
geant MinLifeTime;
geant TheMagCut;
integer ecorand;
integer ecprjmask;
integer Lvl1ConfMCVers;
integer Lvl1ConfRDVers;
integer Lvl1ConfRead;
integer printfl;
integer Lvl1ConfSave;
integer NoPrescalingInMC;
integer NoMaskingInMC;
//
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
};
#define TGL1FFKEY COMMON_BLOCK(TGL1FFKEY,tgl1ffkey)
COMMON_BLOCK_DEF(TGL1FFKEY_DEF,TGL1FFKEY);
//================================================================


//
const integer npatb=11;
class BETAFITFFKEY_DEF {
public:
  integer pattern[npatb];  //patterns  to fit; Priority decreases with number
  //    Number              Descr             Points      Default
  //    0                   1234              4           on
  //    1                   123               3           on
  //    2                   124               3           on
  //    3                   134               3           on
  //    4                   234               3           on
  //    5                   13                2           on
  //    6                   14                2           on
  //    7                   23                2           on
  //    8                   24                2           on
  //    9                   12                2           on
  //   10                   34                2           on
  geant Chi2;  //  Max acceptable chi2 for betafit
  geant SearchReg[3]; // Max distance between track & tof hit
  geant LowBetaThr;   // Threshold below refit should be done if possible 
                           // using 2 & 3 (comb # 7)i
  integer FullReco;
  geant Chi2S;  // Max acceptable chi2 for space fit
  integer MassFromBetaRaw;
  integer OldNew; // MN N==0 -> N beta ; N==1 Beta new; N==2 Beta old ; N==3 Beta Old & New; M==0 Master New, M==1 Master old
  float TRDNorm;  //  Trd beta charge normaqization factor
  float TRDP[3];  //  tof beta to trd extrapolation
};
//



#define BETAFITFFKEY COMMON_BLOCK(BETAFITFFKEY,betafitffkey)
COMMON_BLOCK_DEF(BETAFITFFKEY_DEF,BETAFITFFKEY);

class CHARGEFITFFKEY_DEF {
public:
//- AMSCharge Combination options
  integer RecEnable[4];
  integer SplitLevel[4];
  integer ChargeMax[4];
  geant   ProbMin[4];
  integer NormalizeProbs;
  integer UseLikelihood;   
  geant   ProbklMin;
//- TOF & Tracker (v4) Charge Reconstruction
  integer NmembMax;
  integer Tracker;
  geant   EtaMin[2];
  geant   EtaMax[2];
  geant   ProbTrkRefit;
  geant   ResCut[2];
  geant   SigMin;
  geant   SigMax;
  geant   TrMeanRes;
  integer TrackerForceSK;
  geant   TrackerKSRatio;
  geant   TOFMeVperMip;
  geant   TrkkeVperMip;
  integer TrkPDFileMCVers;
  integer TrkPDFileRDVers;
  integer TrkPDFileRead;
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
//



#define CHARGEFITFFKEY COMMON_BLOCK(CHARGEFITFFKEY,chargefitffkey)
COMMON_BLOCK_DEF(CHARGEFITFFKEY_DEF,CHARGEFITFFKEY);


//

class CCFFKEY_DEF {
public:
  geant coo[6];       //1-6
  geant dir[6];       //7-12
  geant momr[2];      //13-14
  integer fixp;       //15
  geant albedor;      //16
  geant albedocz;     //17
  integer npat;       //18
  integer run;
  integer low;
  integer earth;      // earth mafnetic field modulation
  geant theta;         // station theta at begin
  geant phi;           // station phi at begin
  geant polephi;       // north pole phi
  integer begindate;      // time begin format ddmmyyyy
  integer begintime;      // time begin format hhmmss
  integer enddate;        // time end   format ddmmyyyy
  integer endtime;        // time end   format hhmmss
  integer sdir;            // direction + 1 to higher -1 to lower theta
  integer oldformat;
  integer Fast;            // fast generation
  geant   StrCharge;
  geant   StrMass; 
  integer SpecialCut;     // special cut to accept
                          //  1:  focus on ecal 
  integer FluxFile[40];   // FluxFile 
  integer curtime;        // current event time
  integer DirFilePositions[2];
  integer DirFile[40];
  float Angle;
};
//
#define CCFFKEY COMMON_BLOCK(CCFFKEY,ccffkey)
COMMON_BLOCK_DEF(CCFFKEY_DEF,CCFFKEY);

//
//ISN 
class GMFFKEY_DEF {
public:
integer GammaSource; // gamma source flag/identifier. 1 for user defined
geant SourceCoo[2]; // user defined source equatorial coordinates (GammaSource=1)
geant SourceVisib; // maximum zenith angle (to speed up simulation)
integer GammaBg; // diffuse background flag 
geant BgAngle; // angle for background integration
};
#define GMFFKEY COMMON_BLOCK(GMFFKEY,gmffkey)
COMMON_BLOCK_DEF(GMFFKEY_DEF,GMFFKEY);



const integer nx=41;
const integer ny=41;
const integer nz=180;

//PZMAG #ifndef _PGTRACK_
class TKFIELD_DEF{
public:
integer mfile[40];
integer iniok;
integer isec[2];
integer imin[2];
integer ihour[2];
integer iday[2];
integer imon[2];
integer iyear[2];
integer na[3];
geant  x[nx];
geant  y[ny];
geant  z[nz];
geant bx[nz][ny][nx];
geant by[nz][ny][nx];
geant bz[nz][ny][nx];
geant xyz[nx+ny+nz];
geant bdx[2][nz][ny][nx];
geant bdy[2][nz][ny][nx];
geant bdz[2][nz][ny][nx];
//geant bxc[nz][ny][nx];
//geant byc[nz][ny][nx];
//geant bzc[nz][ny][nx];

};
#define TKFIELD COMMON_BLOCK(TKFIELD,tkfield)
COMMON_BLOCK_DEF(TKFIELD_DEF,TKFIELD);
//PZMAG #endif
/*
const integer mmax=nx*ny*nz;
const integer lrq=10*mmax+7;
const integer liq=2*mmax+1;

class TKFIELDADDON_DEF{
public:
int    iqx[liq];
int    iqy[liq];
int    iqz[liq];
geant  rqx[lrq][2];
geant  rqy[lrq][2];
geant  rqz[lrq][2];

};
#define TKFIELDADDON COMMON_BLOCK(TKFIELDADDON,tkfieldaddon)
COMMON_BLOCK_DEF(TKFIELDADDON_DEF,TKFIELDADDON);
*/



class CALIB_DEF{
public:
integer InsertTimeProc;             // 0 (default) by current time ; 1 by run time
integer Ntuple;
integer SubDetInCalib;//subDetectors selection for processing of OnBoard-calib data
integer SubDetRequestCalib;
};
#define CALIB COMMON_BLOCK(CALIB,calib)
COMMON_BLOCK_DEF(CALIB_DEF,CALIB);





class TRDCALIB_DEF{
public:
integer CalibProcedureNo;
integer EventsPerCheck;
geant PedAccRequired; 
integer Validity[2];
geant BadChanThr;
};
#define TRDCALIB COMMON_BLOCK(TRDCALIB,trdcalib)
COMMON_BLOCK_DEF(TRDCALIB_DEF,TRDCALIB);





class G4FFKEY_DEF{
public:
int BFOrder;
float Delta;
int UniformMagField;
int Geant3CutsOn;
int PhysicsListUsed;
int LowEMagProcUsed;
int IonPhysicsModel;
int UseEMDModel; 
int SigTerm;
};
#define G4FFKEY COMMON_BLOCK(G4FFKEY,g4ffkey)
COMMON_BLOCK_DEF(G4FFKEY_DEF,G4FFKEY);


// RICH Geometry card: currently unused

class 
RICGEOM_DEF{
public:
// Mirrors
geant top_radius; 
geant bottom_radius;
geant hole_radius;
geant height;
geant inner_mirror_height;
// Radiator
geant radiator_radius;
geant radiator_height;
geant radiator_box_length; // The aerogel is an array of this length
// lighguides and PMT
geant light_guides_height; // Unused
geant light_guides_length;
};
#define RICGEOM COMMON_BLOCK(RICGEOM,ricgeom)
COMMON_BLOCK_DEF(RICGEOM_DEF,RICGEOM);

class RICCONTROLFFKEY_DEF{
public:
integer iflgk_flag;
integer tsplit; // Allows task time spliting at the end of the job for OPTIMIZATION
integer pmttables[50];     // Precomputed input PMT tables if any
integer pmttables_out[50]; // Output of precomputed PMT tables if any
};
#define RICCONTROLFFKEY COMMON_BLOCK(RICCONTROLFFKEY,riccontrolffkey)
COMMON_BLOCK_DEF(RICCONTROLFFKEY_DEF,RICCONTROLFFKEY);


class RICRADSETUPFFKEY_DEF{
public:
integer setup;  // Allows to choose among several differen geometries
integer tables_in[50];     // Input file of radiator tables
integer tables_out[50];    // Output file of radiator tables
integer finemesh_in[50]; 
integer alignment_in[50];  // alignment input file, if any  
};
#define RICRADSETUPFFKEY COMMON_BLOCK(RICRADSETUPFFKEY,ricradsetupffkey)
COMMON_BLOCK_DEF(RICRADSETUPFFKEY_DEF,RICRADSETUPFFKEY);


class RICRECFFKEY_DEF{
public:
integer recon[2];
integer liprflag[2];  // LIP reconstruction flags
float   lipsigma[4];  // LIP residuals sigmas
float   lipdcut[2];   // LIP distance cut in sigmas
float   lipznorm[2];  // LIP charge overall factors
integer lipdisp;      // LIP display flag
integer store_rec_hits;
};
#define RICRECFFKEY COMMON_BLOCK(RICRECFFKEY,ricrecffkey)
COMMON_BLOCK_DEF(RICRECFFKEY_DEF,RICRECFFKEY);



class RICFFKEY_DEF{
 public:  
  integer ReadFile;  
  integer sec[2];        // calibration validation
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
  integer fname_in[200];
  integer fname_out[200];
};
#define RICFFKEY COMMON_BLOCK(RICFFKEY,ricffkey)
COMMON_BLOCK_DEF(RICFFKEY_DEF,RICFFKEY);


class RICDBFFKEY_DEF{
 public:  
  integer dump;  
  integer sec[2];        // calibration validation
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define RICDBFFKEY COMMON_BLOCK(RICDBFFKEY,ricdbffkey)
COMMON_BLOCK_DEF(RICDBFFKEY_DEF,RICDBFFKEY);




class
RICGTKOV_DEF{
public:
geant usrcla;
geant scatprob;
geant scatang;
};

#define RICGTKOV COMMON_BLOCK(RICGTKOV,gtckovext)
COMMON_BLOCK_DEF(RICGTKOV_DEF,RICGTKOV);

class
PRODFFKEY_DEF{
public:
integer Debug;
integer Job;
};

#define PRODFFKEY COMMON_BLOCK(PRODFFKEY,prodffkey)
COMMON_BLOCK_DEF(PRODFFKEY_DEF,PRODFFKEY);


//+LIP
// commons

// parameters used in array sizes below
// (should match those in richrec_lipc.h, richrec_lipf.h, richrec.h)
// value of LIP_NBHITSMAX changed 13-Jan-2011
//#define LIP_NBHITSMAX 11000
#define LIP_NBHITSMAX 1000
#define LIP_NHITMAX 1000
#define LIP_NRMAX 2
#define LIP_NMAXTOFCLU 100
#define LIP_NMAXLIPREC 10
#define LIP_NMAXMIRSECCC 10
#define LIP_NPMTLIP 756

// input data (see richrec_lipc.h for details)

class LIPC2F_DEF{
 public:

  // job data
  int jobc_cp_c2f;

  // detector data
  geant ztoprad_ams_c2f;
  int rcgeom_c2f[2];                           
  int levgeom_c2f;
  int levgrad_c2f;	           
  int levacc_c2f;	           
  int levghit_c2f;
  geant ztarg_c2f;                 
  int nradts_c2f;                
  geant hrad_c2f;	           
  geant hrnaf_c2f;	           
  geant radtile_pitch_c2f;         
  geant radtile_supthk_c2f;        
  geant radix_c2f[LIP_NRMAX];
  geant radclarity_c2f;	           
  geant hpgl_c2f;	           
  geant ztmirgap_c2f;	           
  geant zbmirgap_c2f;	           
  geant reflec_c2f;	           
  geant zlgsignal_c2f;
  geant rtmir_c2f;
  geant rbmir_c2f;
  geant hmir_c2f;
  int nmirsec_c2f;
  int idmirs_c2f[LIP_NMAXMIRSECCC];
  geant phimirs_c2f[LIP_NMAXMIRSECCC];
  geant refmirs_c2f[LIP_NMAXMIRSECCC];
  geant pmtwx_c2f;
  geant pmtwy_c2f;
  geant shieldw_c2f;
  int ipmtstat_c2f[LIP_NPMTLIP];
  geant pglix_c2f;
  geant emcxlim_c2f;
  geant emcylim_c2f;
  geant lg_top_width_c2f;
  geant lg_bot_width_c2f;
  geant lg_pitch_c2f;
  geant xbc_c2f;
  geant xbd_c2f;
  geant yef_c2f;
  geant xpc_c2f;

  // hit data
  int nbhits_ev;                 
  int nbhitsmax_ntup_ev;         
  geant hitsnpe_sim_ev[LIP_NBHITSMAX]; 
  int hitspmt_ev[LIP_NBHITSMAX];     
  geant hitscoo_ev[LIP_NBHITSMAX][3];   
  geant hitsnpe_ev[LIP_NBHITSMAX];     
  int hitstat_ev[LIP_NBHITSMAX];     

  // simulated main track parameters
  geant pimp_sim[3];               
  geant pthe_sim;                  
  geant pphi_sim;                  
  geant pmom_sim;                  
  geant pbeta_sim;                 
  geant pchg_sim;

  // main track parameters
  geant pimp_main[3];              
  geant epimp_main[3];              
  geant pthe_main;                 
  geant epthe_main;                 
  geant pphi_main;                 
  geant epphi_main;                 
  geant pmom_main;                 
  int prad_main;                 

  // TOF track parameters
  int iflag_tof;
  geant pimp_tof[3];              
  geant epimp_tof[3];              
  geant pthe_tof;                 
  geant epthe_tof;                 
  geant pphi_tof;                 
  geant epphi_tof;                 
  geant pmom_tof;                 
  int prad_tof;                 
  int nusedclu_tof;
  int iusedclu_tof[10];

  // TOF cluster data
  int ntofclu;                   
  int istatus_tof[LIP_NMAXTOFCLU];   
  int ilayer_tof[LIP_NMAXTOFCLU];    
  int ibar_tof[LIP_NMAXTOFCLU];      
  geant edep_tof[LIP_NMAXTOFCLU];      
  geant edepd_tof[LIP_NMAXTOFCLU];     
  geant time_tof[LIP_NMAXTOFCLU];      
  geant errtime_tof[LIP_NMAXTOFCLU];   
  geant coo_tof[LIP_NMAXTOFCLU][3];     
  geant errcoo_tof[LIP_NMAXTOFCLU][3];

  // track and reconstruction counters
  int irecnumb;
  int itrknumb;

  // flags for lip code
  int idispflag;
};
#define LIPC2F COMMON_BLOCK(LIPC2F,lipc2f)
COMMON_BLOCK_DEF(LIPC2F_DEF,LIPC2F);
//LA 18Out10
#pragma omp threadprivate(LIPC2F)

// output data (see richrec_lipc.h for details)

class LIPF2C_DEF{
 public:

  // beta reconstruction results
  int resb_iflag[LIP_NMAXLIPREC];          
  int resb_itype[LIP_NMAXLIPREC];          
  int resb_itrk[LIP_NMAXLIPREC];          
  geant resb_beta[LIP_NMAXLIPREC];           
  geant resb_thc[LIP_NMAXLIPREC];
  geant resb_chi2[LIP_NMAXLIPREC];           
  geant resb_like[LIP_NMAXLIPREC];           
  int resb_nhit[LIP_NMAXLIPREC];           
  int resb_phit[LIP_NMAXLIPREC][LIP_NHITMAX];  
  int resb_used[LIP_NMAXLIPREC][LIP_NHITMAX];
  geant resb_hres[LIP_NMAXLIPREC][LIP_NHITMAX];  
  geant resb_invchi2[LIP_NMAXLIPREC];        
  geant resb_flatsin[LIP_NMAXLIPREC];        
  geant resb_flatcos[LIP_NMAXLIPREC];        
  geant resb_probkl[LIP_NMAXLIPREC];         

  // charge reconstruction results
  int resc_iflag[LIP_NMAXLIPREC];          
  geant resc_cnpe[LIP_NMAXLIPREC];           
  geant resc_cnpedir[LIP_NMAXLIPREC];        
  geant resc_cnperef[LIP_NMAXLIPREC];        
  geant resc_chg[LIP_NMAXLIPREC];            
  geant resc_chgdir[LIP_NMAXLIPREC];         
  geant resc_chgmir[LIP_NMAXLIPREC];         
  geant resc_accgeom[LIP_NMAXLIPREC][3];
  geant resc_eff[LIP_NMAXLIPREC][6];
  int resc_nmirsec[LIP_NMAXLIPREC];
  geant resc_accmsec[LIP_NMAXLIPREC][2][LIP_NMAXMIRSECCC];
  geant resc_effmsec[LIP_NMAXLIPREC][2][LIP_NMAXMIRSECCC];
  geant resc_chgprob[LIP_NMAXLIPREC][3];

  //track parameters
  geant resb_pimp[LIP_NMAXLIPREC][3];
  geant resb_epimp[LIP_NMAXLIPREC][3];
  geant resb_pvtx[LIP_NMAXLIPREC][3];
  geant resb_epvtx[LIP_NMAXLIPREC][3];
  geant resb_pthe[LIP_NMAXLIPREC];           
  geant resb_epthe[LIP_NMAXLIPREC];           
  geant resb_pphi[LIP_NMAXLIPREC];           
  geant resb_epphi[LIP_NMAXLIPREC];           

  //standalone rec details
  geant rstd_creclike[LIP_NMAXLIPREC][50];
  geant rstd_crecx0[LIP_NMAXLIPREC][50];
  geant rstd_crecy0[LIP_NMAXLIPREC][50];
  geant rstd_crectheta[LIP_NMAXLIPREC][50];
  geant rstd_crecphi[LIP_NMAXLIPREC][50];
  geant rstd_crecbeta[LIP_NMAXLIPREC][50];
  int rstd_crecuhits[LIP_NMAXLIPREC][50];
  geant rstd_crecpkol[LIP_NMAXLIPREC][50];
};
#define LIPF2C COMMON_BLOCK(LIPF2C,lipf2c)
COMMON_BLOCK_DEF(LIPF2C_DEF,LIPF2C);
//LA 18Out10
#pragma omp threadprivate(LIPF2C)

// output parameters, obsolete (FOR DEBUGGING ONLY!)

class LIPVAR_DEF{
public:
int   liphused;
geant lipthc;
geant lipbeta;
geant lipebeta;
geant liplikep;
geant lipchi2;
geant liprprob;
};
#define LIPVAR COMMON_BLOCK(LIPVAR,lipvar)
COMMON_BLOCK_DEF(LIPVAR_DEF,LIPVAR);
//LA 18Out10
#pragma omp threadprivate(LIPVAR)

// ENDofLIP






#endif
