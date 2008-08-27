
#ifndef __TKDCARDS__
#define __TKDCARDS__
#include "typedefs.h"
// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

//===================================================================
class TKGEOMFFKEY_DEF{
public:
  integer ReadGeomFromFile;
  integer WriteGeomToFile;
  integer UpdateGeomFile;
  integer gfile[40];
  void init();

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

  void init();
};
#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);


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
  void init();
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
  void init();
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
  integer gfile[40];
  integer LaddersOnly;
  void init();
};
#define TRALIG COMMON_BLOCK(TRALIG,tralig)
COMMON_BLOCK_DEF(TRALIG_DEF,TRALIG);

#endif
