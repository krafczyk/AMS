// Author V. Choutko 24-may-1996
 
#ifndef __AMSCOMMONS__
#define __AMSCOMMONS__
#include <cern.h>

const int NWPAW=300000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=1000000;
struct GCBANK_DEF{
integer  IQ[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);


class SELECTFFKEY_DEF {
public:
integer Run;
integer Event;
};
#define SELECTFFKEY COMMON_BLOCK(SELECTFFKEY,selectffkey)
COMMON_BLOCK_DEF(SELECTFFKEY_DEF,SELECTFFKEY);

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

class IOPA_DEF {
public:
integer hlun;
integer hfile[40];
integer ntuple;
integer WriteAll;
integer TriggerI;
integer TriggerC[40];
integer mode;
integer ffile[40];
};
#define IOPA COMMON_BLOCK(IOPA,iopa)
COMMON_BLOCK_DEF(IOPA_DEF,IOPA);
//===============================================================
class TOFMCFFKEY_DEF {
public:
geant TimeSigma;
geant TimeSigma2;
geant TimeProbability2;
geant padl;
geant Thr;
//
integer mcprtf[5];     // TOF MC-print flag (=0 -> no_printing)
integer trlogic[2]; // to control trigger logic
integer fast;    // 1/0 fast/slow algo;
integer tdfnam[3];// f-name for PMT t-distr. files map
};
#define TOFMCFFKEY COMMON_BLOCK(TOFMCFFKEY,tofmcffkey)
COMMON_BLOCK_DEF(TOFMCFFKEY_DEF,TOFMCFFKEY);
//===============================================================
class TOFRECFFKEY_DEF {
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
//
  integer reprtf[5]; //RECO print flag 
  integer relogic[5];//RECO logic flag
  geant daqthr[5];// daq thresholds
  geant cuts[10];// cuts 
  integer config[3];// geometry-configuration generic file-name (max 11 letters)
  integer tzerca[3];// T0,slope,str-ratio calib.parameters gen. file-name
  integer spare1[3];// spare f.n
  integer spare2[3];// spare f.n
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define TOFRECFFKEY COMMON_BLOCK(TOFRECFFKEY,tofrecffkey)
COMMON_BLOCK_DEF(TOFRECFFKEY_DEF,TOFRECFFKEY);
//===============================================================

class CTCGEOMFFKEY_DEF {
public:
  geant wallth;    // reflecting walls(separators) thickness (cm)
  geant agap;      // typical "air" gaps for aerogel blocks
  geant wgap;      // typical gaps for WLS's
  geant agsize[3]; // max aerogel size
  geant wlsth;     // wave_length_shifter thickness
  geant hcsize[3]; // honeycomb plate size
  geant supzc;     // z-position of supp.hon.plate (front surface)
  geant aegden;    // aerogel density
  geant wlsden;    // wls dens.
  integer nblk;    // number of aerogel cells/layer (X-divisions)
  integer nwls;    // number of wls bars/layer (=nblk) 
                        };
#define CTCGEOMFFKEY COMMON_BLOCK(CTCGEOMFFKEY,ctcgeomffkey)
COMMON_BLOCK_DEF(CTCGEOMFFKEY_DEF,CTCGEOMFFKEY);

class CTCMCFFKEY_DEF {
public:
  geant Refraction[2]; // refraction index (aerogel,wls)
  geant Path2PhEl[2];  // path to phel con factor (aerogel,wls)
  geant AbsLength[2];  // Absorbtion length in cm (aerogel, wls)
  geant Edep2Phel[2];  // Edep to phel con factor (aerogel,wls)
};
#define CTCMCFFKEY COMMON_BLOCK(CTCMCFFKEY,ctcmcffkey)
COMMON_BLOCK_DEF(CTCMCFFKEY_DEF,CTCMCFFKEY);

class CTCRECFFKEY_DEF {
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
};
#define CTCRECFFKEY COMMON_BLOCK(CTCRECFFKEY,ctcrecffkey)
COMMON_BLOCK_DEF(CTCRECFFKEY_DEF,CTCRECFFKEY);



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
geant thr[2];
integer neib[2];
geant   cmn[2];
integer adcoverflow;
integer NoiseOn;
integer sec[2];
integer min[2];
integer hour[2];
integer day[2];
integer mon[2];
integer year[2];
};
#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);

//
const integer npatb=9;
class BETAFITFFKEY_DEF {
public:
  integer pattern[npatb];  //patterns  to fit; Priority decreases with number
  //    Number              Descr             Points      Default
  //    0                   1234              4           on
  //    1                   123               3           on
  //    2                   124               3           on
  //    3                   134               3           on
  //    4                   234               3           on
  //    5                   13                2           off
  //    6                   14                2           off
  //    7                   23                2           off
  //    8                   24                2           off
  geant Chi2;  //  Max acceptable chi2 for betafit
  geant SearchReg[3]; // Max distance between track & tof hit
};
//



#define BETAFITFFKEY COMMON_BLOCK(BETAFITFFKEY,betafitffkey)
COMMON_BLOCK_DEF(BETAFITFFKEY_DEF,BETAFITFFKEY);

class CHARGEFITFFKEY_DEF {
public:
  geant Thr;  // Threshold to refit
};
//



#define CHARGEFITFFKEY COMMON_BLOCK(CHARGEFITFFKEY,chargefitffkey)
COMMON_BLOCK_DEF(CHARGEFITFFKEY_DEF,CHARGEFITFFKEY);

//
const integer npat=22;
class TRFITFFKEY_DEF {
public:
  integer pattern[npat];  //patterns  to fit; Priority decreases with number
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
  integer UseTOF;     // Use(>=1)/Not use(0) 
                      // TOF info for pattern rec for pattern 16
  geant Chi2FastFit;  //  Max acceptable chi2 for tkhmd; 
  geant Chi2StrLine; // The same for Straight line fit
  geant Chi2Circle;  // The same for Circle fit
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
};
//


#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);

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
};
//
#define CCFFKEY COMMON_BLOCK(CCFFKEY,ccffkey)
COMMON_BLOCK_DEF(CCFFKEY_DEF,CCFFKEY);

//


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
};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);

const integer nx=29;
const integer ny=29;
const integer nz=35;
class TKFIELD_DEF{
public:
integer iniok;
geant  x[nx];
geant  y[ny];
geant  z[nz];
geant bx[nz][ny][nx];
geant by[nz][ny][nx];
geant bz[nz][ny][nx];
geant xyz[nx+ny+nz];
integer isec[2];
integer imin[2];
integer ihour[2];
integer iday[2];
integer imon[2];
integer iyear[2];
};
#define TKFIELD COMMON_BLOCK(TKFIELD,tkfield)
COMMON_BLOCK_DEF(TKFIELD_DEF,TKFIELD);

class AMSDATADIR_DEF{
public:
integer amsdlength;
char amsdatadir[128];
};
#define AMSDATADIR COMMON_BLOCK(AMSDATADIR,amsdatadir)
COMMON_BLOCK_DEF(AMSDATADIR_DEF,AMSDATADIR);

class TRCALIB_DEF{
public:
integer EventsPerIteration[2];
integer NumberOfIterations[2];
                                 // Select Cut
geant BetaCut[2][2];             // Beta limits
geant HitsRatioCut[2];           // Hit Ratio
                                 // Global fit cuts
geant MomentumCut[2][2];         // momentum ----------
geant Chi2Cut[2];                // chi2 --------------- 
geant InitialCoo[6][3];          // Coordinates displacement
geant InitialRM[6][3][3];        // Rot Matrix --------
};
#define TRCALIB COMMON_BLOCK(TRCALIB,trcalib)
COMMON_BLOCK_DEF(TRCALIB_DEF,TRCALIB);



class AMSCommonsI{
public:
AMSCommonsI();
void init();
private:
static integer _Count;
};
static AMSCommonsI cmnI;








#endif
