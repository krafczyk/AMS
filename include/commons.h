// Author V. Choutko 24-may-1996
// modifications for TOF,ANTI by E.Choumilov 
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

class TRMFFKEY_DEF{
public:
integer OKAY;
};
#define TRMFFKEY COMMON_BLOCK(TRMFFKEY,trmffkey)
COMMON_BLOCK_DEF(TRMFFKEY_DEF,TRMFFKEY);

class MISCFFKEY_DEF{
public:
integer BTempCorrection;
integer BeamTest;
};
#define MISCFFKEY COMMON_BLOCK(MISCFFKEY,miscffkey)
COMMON_BLOCK_DEF(MISCFFKEY_DEF,MISCFFKEY);


class SELECTFFKEY_DEF {
public:
integer Run;
integer Event;
};
#define SELECTFFKEY COMMON_BLOCK(SELECTFFKEY,selectffkey)
COMMON_BLOCK_DEF(SELECTFFKEY_DEF,SELECTFFKEY);

class STATUSFFKEY_DEF {
public:
uinteger status[33];
};
#define STATUSFFKEY COMMON_BLOCK(STATUSFFKEY,statusffkey)
COMMON_BLOCK_DEF(STATUSFFKEY_DEF,STATUSFFKEY);


class DAQCFFKEY_DEF {
public:
integer mode;
integer ifile[40];
integer ofile[40];
integer OldFormat;
integer LCrateinDAQ;
integer SCrateinDAQ;
integer NoRecAtAll;
integer TrFormatInDAQ;
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
};
#define LVL3FFKEY COMMON_BLOCK(LVL3FFKEY,lvl3ffkey)
COMMON_BLOCK_DEF(LVL3FFKEY_DEF,LVL3FFKEY);



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
};
#define IOPA COMMON_BLOCK(IOPA,iopa)
COMMON_BLOCK_DEF(IOPA_DEF,IOPA);
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
integer daqfmt;  // 0/1 raw/reduced TDC format for DAQ
integer birks;     // 0/1  not apply/apply birks corrections
integer adsimpl;   // 0/1->precise/simplified sim. of A/D-TovT
geant blshift;     // base line shift
geant hfnoise;     // high freq. noise
};
#define TOFMCFFKEY COMMON_BLOCK(TOFMCFFKEY,tofmcffkey)
COMMON_BLOCK_DEF(TOFMCFFKEY_DEF,TOFMCFFKEY);

class TKGEOMFFKEY_DEF{
public:
  integer ReadGeomFromFile;
  integer WriteGeomToFile;
  integer UpdateGeomFile;
};
#define TKGEOMFFKEY COMMON_BLOCK(TKGEOMFFKEY,tkgeomffkey)
COMMON_BLOCK_DEF(TKGEOMFFKEY_DEF,TKGEOMFFKEY);
class ANTIGEOMFFKEY_DEF {
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
#define ANTIGEOMFFKEY COMMON_BLOCK(ANTIGEOMFFKEY,antigeomffkey)
COMMON_BLOCK_DEF(ANTIGEOMFFKEY_DEF,ANTIGEOMFFKEY);
class ANTIMCFFKEY_DEF {
public:
integer mcprtf;// hist. print flag
geant SigmaPed;
geant MeV2PhEl;
geant LZero;
geant PMulZPos;
geant LSpeed;
};
#define ANTIMCFFKEY COMMON_BLOCK(ANTIMCFFKEY,antimcffkey)
COMMON_BLOCK_DEF(ANTIMCFFKEY_DEF,ANTIMCFFKEY);
//==============================================================

class ANTIRECFFKEY_DEF {
public:
  integer reprtf[3];//  print flag
  geant ThrS; // threshold (p.e) to create Cluster-object
  geant PhEl2MeV;
  geant dtthr; // trig.discr.theshold (same for all sides now) (p.e.'s for now)
  geant dathr; // Amplitude(charge) discr.threshold(...) (p.e.)
  geant ftwin; // time_window in true TDCA-hits search (ns) 
//
  integer ReadConstFiles;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define ANTIRECFFKEY COMMON_BLOCK(ANTIRECFFKEY,antirecffkey)
COMMON_BLOCK_DEF(ANTIRECFFKEY_DEF,ANTIRECFFKEY);
//
class ANTICAFFKEY_DEF {
  public:
  integer cfvers; // vers.number NN for antiverlistNN.dat file
};
#define ANTICAFFKEY COMMON_BLOCK(ANTICAFFKEY,anticaffkey)
COMMON_BLOCK_DEF(ANTICAFFKEY_DEF,ANTICAFFKEY);

//================================================================
class TOFRECFFKEY_DEF {
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
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define TOFRECFFKEY COMMON_BLOCK(TOFRECFFKEY,tofrecffkey)
COMMON_BLOCK_DEF(TOFRECFFKEY_DEF,TOFRECFFKEY);
//===================================================================
class TOFCAFFKEY_DEF {
public:
// TZSL-calibration :
geant pcut[2];//low/high limits on momentum of calibr. events
geant bmeanpr;// mean proton velocity in this mom. range
geant tzref[2];// def. T0 for two reference counters
geant fixsl;// def. for slope
geant bmeanmu;// mean muon velocity in this mom. range
integer idref[2];// LBB for two ref.counter 
integer ifsl;// 0/1 to fix/release slope param.
integer caltyp;// 0/1 to select space/earth calibration
// AMPL-calibration :
integer truse;// 1/0 to use/not tracker
geant plhc[2];//low/high cuts on tracker mom. for space calibration
integer minev;// min.events needed for measurement in channel or bin
geant trcut;// cut to use for "truncated average" calculation (0.85)
integer refbid[5];//ref. id's list to use for longit. uniformity meas.
geant plhec[2];//low/high cuts on tracker mom. for earth calibration
geant bgcut[2];// beta*gamma low/high cuts for mip in abs.calibration
integer tofcoo;// 0/1-> use transv/longit coord. from TOF
integer dynflg;// 0/1-> use stand/special(Contin's) dynode calibr.
integer cfvers;// 1-99 -> vers.number for tofverslistNN.dat file
integer cafdir;// 0/1->use officical/private directory for calib.files
integer mcainc;// to swich on/off A-integr. calibr. in MC
geant tofbetac;// if !=0 -> low beta cut (own TOF measurement !!!) 
//                to use when previous calibration suppose to be good enought
};
#define TOFCAFFKEY COMMON_BLOCK(TOFCAFFKEY,tofcaffkey)
COMMON_BLOCK_DEF(TOFCAFFKEY_DEF,TOFCAFFKEY);
//===================================================================
class CTCGEOMFFKEY_DEF {
public:
  geant wallth;    // reflecting walls(separators) thickness (cm)
  geant agap;      // typical "air" gaps for aerogel blocks
  geant wgap;      // typical gaps for WLS's
  geant supzc;     // Support position 
  geant agsize[3]; // max aerogel size
  geant wlsth;     // wave_length_shifter thickness
  geant hcsize[3]; // Old honeycomb thickness
  geant thcsize[3];// honeycomb plate size
  geant aegden;    // aerogel density
  geant wlsden;    // wls dens.
  integer nblk;    // number of aerogel cells/layer (X-divisions)
  integer nwls;    // number of wls bars/layer (=nblk) 
  geant ptfe[3];   // PTFE dimensions
  geant ptfx;      // Single PTFE layer thickness
  geant upper[3];  // UPPER/LOWER dimensions
  geant wall[3];   // WALL dimensions (between modules)
  geant cell[3];   // Super CELL dimensions
  geant ygap[3];   // Gap dimensions (between supercells in same module)
  geant agel[3];   // AeroGEL dimensions
  geant pmt[3];    // PMT dimensions (including housing)
  integer xdiv;   // x-scell number      
  integer ydiv;   // y-scell number      
};
#define CTCGEOMFFKEY COMMON_BLOCK(CTCGEOMFFKEY,ctcgeomffkey)
COMMON_BLOCK_DEF(CTCGEOMFFKEY_DEF,CTCGEOMFFKEY);

class CTCMCFFKEY_DEF {
public:
  geant Refraction[3]; // refraction index (aerogel,ptf,pmt)
  geant Path2PhEl[3];  // path to phel con factor (aerogel,ptf,pmt)
  geant AbsLength[3];  // Absorbtion length in cm (aerogel, ptf,pmt)
  geant Edep2Phel[3];  // Edep to phel con factor (aerogel,ptf,pmt)
  
  integer mcprtf;// hist. print flag
};
#define CTCMCFFKEY COMMON_BLOCK(CTCMCFFKEY,ctcmcffkey)
COMMON_BLOCK_DEF(CTCMCFFKEY_DEF,CTCMCFFKEY);
//
class CTCRECFFKEY_DEF {
  public:
  geant Thr1;  // limit on max
  geant ThrS;  // limit on sum
  integer reprtf[3];//  print flag
  geant ftwin; // time_window(ns) in true TDCA-hits search wrt TDCT-hit(FT)
  geant q2pe;  // charge to photoelectrons conversion
  integer ft2edg;//  0/1 -> 1/2 edges readout for FT pulse
//
  integer ReadConstFiles;
//
  integer sec[2];
  integer min[2];
  integer hour[2];
  integer day[2];
  integer mon[2];
  integer year[2];
};
#define CTCRECFFKEY COMMON_BLOCK(CTCRECFFKEY,ctcrecffkey)
COMMON_BLOCK_DEF(CTCRECFFKEY_DEF,CTCRECFFKEY);
//
class CTCCAFFKEY_DEF {
  public:
  integer cfvers; // vers.number NN for ctcverlistNN.dat file
};
#define CTCCAFFKEY COMMON_BLOCK(CTCCAFFKEY,ctccaffkey)
COMMON_BLOCK_DEF(CTCCAFFKEY_DEF,CTCCAFFKEY);
//==============================================================

class LVL1FFKEY_DEF {
public:
integer ntof;
integer nanti;
integer RebuildLVL1;
geant MaxScalersRate;
geant MinLifeTime;
};
#define LVL1FFKEY COMMON_BLOCK(LVL1FFKEY,lvl1ffkey)
COMMON_BLOCK_DEF(LVL1FFKEY_DEF,LVL1FFKEY);

//================================================================


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
integer RawModeOn[2][2][32];
integer WriteHK; 
geant delta[2];
geant gammaA[2];
geant NonGaussianPart[2];
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
  //    5                   13                2           on
  //    6                   14                2           on
  //    7                   23                2           on
  //    8                   24                2           on
  geant Chi2;  //  Max acceptable chi2 for betafit
  geant SearchReg[3]; // Max distance between track & tof hit
  geant LowBetaThr;   // Threshold below refit should be done if possible 
                           // using 2 & 3 (comb # 7)i
  integer FullReco;
  geant Chi2S;  // Max acceptable chi2 for space fit
};
//



#define BETAFITFFKEY COMMON_BLOCK(BETAFITFFKEY,betafitffkey)
COMMON_BLOCK_DEF(BETAFITFFKEY_DEF,BETAFITFFKEY);

class CHARGEFITFFKEY_DEF {
public:
  geant Thr;  // Threshold to refit
  geant OneChargeThr;
  geant EtaMin[2];
  geant EtaMax[2];
};
//



#define CHARGEFITFFKEY COMMON_BLOCK(CHARGEFITFFKEY,chargefitffkey)
COMMON_BLOCK_DEF(CHARGEFITFFKEY_DEF,CHARGEFITFFKEY);

//
const integer npat=42;
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
  //   .... 3 points .....
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
  integer WeakTracking;
  integer FalseXTracking;
  geant Chi2FalseX;
  integer ForceFalseTOFX;
  integer FalseTOFXTracking;
  integer TOFTracking;
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
geant CommonGain[2];
geant CommonShift[2];
geant EtaCor[2];
};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);

const integer nx=41;
const integer ny=41;
const integer nz=130;
class TKFIELD_DEF{
public:
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

};
#define TKFIELD COMMON_BLOCK(TKFIELD,tkfield)
COMMON_BLOCK_DEF(TKFIELD_DEF,TKFIELD);

class AMSDATADIR_DEF{
public:
integer amsdlength;
integer amsdblength;
char amsdatadir[128];
char amsdatabase[128];
};
#define AMSDATADIR COMMON_BLOCK(AMSDATADIR,amsdatadir)
COMMON_BLOCK_DEF(AMSDATADIR_DEF,AMSDATADIR);

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
integer ActiveParameters[6][6]; //   for each plane: x,y,z, pitch, yaw, roll
integer Ladder[6];            // ladder*10+half no for each plane 0 == all
integer PatStart;
};
#define TRCALIB COMMON_BLOCK(TRCALIB,trcalib)
COMMON_BLOCK_DEF(TRCALIB_DEF,TRCALIB);





class AMSCommonsI{
private:
 static integer _Count;
 static char _version[6];
 static integer _build;
public:
 AMSCommonsI();
 void init();
 static char * getversion(){return _version;}
 static integer getbuildno()  {return _build;}
};
static AMSCommonsI cmnI;








#endif
