// Author V. Choutko 24-may-1996
 
#ifndef __AMSCOMMONS__
#define __AMSCOMMONS__
#include <cern.h>
#include <mceventg.h>

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

class AMSFFKEY_DEF{
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
};
#define AMSFFKEY COMMON_BLOCK(AMSFFKEY,amsffkey)
COMMON_BLOCK_DEF(AMSFFKEY_DEF,AMSFFKEY);

class IOPA_DEF{
public:
integer hlun;
integer hfile[40];
integer ntuple;
integer WriteAll;
integer TriggerI;
integer TriggerC[40];
};
#define IOPA COMMON_BLOCK(IOPA,iopa)
COMMON_BLOCK_DEF(IOPA_DEF,IOPA);

class TOFMCFFKEY_DEF{
public:
geant TimeSigma;
geant padl;
geant Thr;
number edep2ph;     // edep(Mev)-to-Photons convertion
number pmqeff;      // PM mean quantum efficiency
number trtspr;      // PM transition time spread (ns)
number fladctb;     // MC flash-ADC time binning (ns)
number shaptb;      // MC shaper pulse time binning
number di2anr;      // MC dinode-to-anode signal ratio
number shrtim;      // MC shaper pulse rise time (ns)
number shftim;      // MC shaper pulse fall time (ns)
number accdel[2];   // fixed(in h/w) lev-1 "accept" signal delays(ns)
number accdelmx[2]; // max. lev-1 "accept" delays (incl. above fixed)
number strrat;      // stratcher ratio (~10)
number tdcbin[4];   // p/l TDC binning for ftdc/stdc/adca/adcd
number daqthr[5];   // MC DAQ-system thresholds
number daqpwd[10];  // MC DAQ-system pulse_widths/dead_times/...
number trigtb;      // MC time binning in logic(trigger) pulse manipulation
integer mcprtf;     // TOF MC-print flag (=0 -> no_printing)
integer trlogic[2]; // to control trigger logic

};
#define TOFMCFFKEY COMMON_BLOCK(TOFMCFFKEY,tofmcffkey)
COMMON_BLOCK_DEF(TOFMCFFKEY_DEF,TOFMCFFKEY);

class TOFRECFFKEY_DEF{
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
};
#define TOFRECFFKEY COMMON_BLOCK(TOFRECFFKEY,tofrecffkey)
COMMON_BLOCK_DEF(TOFRECFFKEY_DEF,TOFRECFFKEY);

class CTCGEOMFFKEY_DEF{
public:
  geant wallth;    // reflecting walls(separators) thickness (cm)
  geant agap;      // typical "air" gaps
  geant wgap;      // typical wls gaps
  geant agsize[3]; // max aerogel size
  geant wlsth;     // wave_length_shifter thickness
  geant hcsize[3]; // honeycomb plate size
  geant supzc;     // z-position of supp.hon.plate (front surface)
  geant aegden;    // aerogel density
  geant wlsden;    // wls dens.
  integer nblk;    // number of aerogel blocks (X-divisions) (=1 for solid)
  integer nwls;    // number of wls blocks
};
#define CTCGEOMFFKEY COMMON_BLOCK(CTCGEOMFFKEY,ctcgeomffkey)
COMMON_BLOCK_DEF(CTCGEOMFFKEY_DEF,CTCGEOMFFKEY);

class CTCMCFFKEY_DEF{
public:
  geant Refraction[2]; //refraction index (aerogel,wls)
  geant Path2PhEl[2];  // path to phel con factor (aerogel,wls)
  geant AbsLength[2]; // Absorbtion length in cm (aerogel, wls)
};
#define CTCMCFFKEY COMMON_BLOCK(CTCMCFFKEY,ctcmcffkey)
COMMON_BLOCK_DEF(CTCMCFFKEY_DEF,CTCMCFFKEY);

class CTCRECFFKEY_DEF{
public:
geant Thr1;  // limit on max
geant ThrS;  // limit on sum
};
#define CTCRECFFKEY COMMON_BLOCK(CTCRECFFKEY,ctcrecffkey)
COMMON_BLOCK_DEF(CTCRECFFKEY_DEF,CTCRECFFKEY);



class TRMCFFKEY_DEF{
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
integer cmn[2];
};
#define TRMCFFKEY COMMON_BLOCK(TRMCFFKEY,trmcffkey)
COMMON_BLOCK_DEF(TRMCFFKEY_DEF,TRMCFFKEY);

//
const integer npatb=9;
class BETAFITFFKEY_DEF{
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
//
const integer npat=19;
class TRFITFFKEY_DEF{
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
  //   16                   2345              4           off
  //   17                   2346              4           off
  //   18                   3456              4           off
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
};
//


#define TRFITFFKEY COMMON_BLOCK(TRFITFFKEY,trfitffkey)
COMMON_BLOCK_DEF(TRFITFFKEY_DEF,TRFITFFKEY);

//

class CCFFKEY_DEF{
public:
  geant coo[6];       //1-6
  geant dir[6];       //7-12
  geant momr[2];      //13-14
  integer fixp;       //15
  geant albedor;      //16
  geant albedocz;     //17
  integer npat;       //18
  integer run;
};
//
#define CCFFKEY COMMON_BLOCK(CCFFKEY,ccffkey)
COMMON_BLOCK_DEF(CCFFKEY_DEF,CCFFKEY);

//


class TRCLFFKEY_DEF{
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
geant ErrZ;
};
#define TRCLFFKEY COMMON_BLOCK(TRCLFFKEY,trclffkey)
COMMON_BLOCK_DEF(TRCLFFKEY_DEF,TRCLFFKEY);
#endif
