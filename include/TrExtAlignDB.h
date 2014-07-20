//  $Id$
#ifndef TREXTALIGNDB_H
#define TREXTALIGNDB_H

#include <map>
#include <vector>
#include "TObject.h"
#include "TrRecHit.h"


/// Repesentation of the plane alignement pars
class TrExtAlignPar: public TObject{

public:
  /// Plane shift (dx,dy,dz) in cm
  float dpos[3];
  /// Plane rotation (alpha, beta, gamma) in rad
  float angles[3];

  /// Plane shift fitting error in cm
  float edpos[3];
  /// Plane rotation fitting error in rad
  float eangles[3];

  /// Fitting chisquare
  float chisq;
  /// Fitting NDF
  int NDF;

public:
  /// Default constructor
  TrExtAlignPar() { Init(); }

  /// Constructor with parameters
  TrExtAlignPar(float x, float y, float z, float a, float b, float g) {
    Init();
    SetPar(x, y, z, a, b, g);
  }

  void Init() {
    SetPar(0, 0, 0, 0, 0, 0);
    SetErr(0, 0, 0, 0, 0, 0);
    chisq = 0;
    NDF   = 0;
  }

  void SetPar(float x, float y, float z, float a, float b ,float g) {
    dpos  [0] = x; dpos  [1] = y; dpos  [2] = z;
    angles[0] = a; angles[1] = b; angles[2] = g;
  }

  void SetErr(float x, float y, float z, float a, float b ,float g) {
    edpos  [0] = x; edpos  [1] = y; edpos  [2] = z;
    eangles[0] = a; eangles[1] = b; eangles[2] = g;
  }

  TrExtAlignPar & operator += (const TrExtAlignPar &par) {
    dpos  [0] += par.dpos  [0]; dpos  [1] += par.dpos  [1];
    dpos  [2] += par.dpos  [2]; angles[0] += par.angles[0];
    angles[1] += par.angles[1]; angles[2] += par.angles[2];
    return *this;
  }
  TrExtAlignPar & operator = (const TrExtAlignPar &par) {
    if( &par == this) return *this;
    dpos  [0] = par.dpos  [0]; dpos  [1] = par.dpos  [1];
    dpos  [2] = par.dpos  [2]; angles[0] = par.angles[0];
    angles[1] = par.angles[1]; angles[2] = par.angles[2];
    edpos  [0] = par.edpos  [0]; edpos  [1] = par.edpos  [1];
    edpos  [2] = par.edpos  [2]; eangles[0] = par.eangles[0];
    eangles[1] = par.eangles[1]; eangles[2] = par.eangles[2];
    chisq=par.chisq; NDF=par.NDF;
    return *this;
  }


  TrExtAlignPar operator +(const TrExtAlignPar &par) const {
    TrExtAlignPar pp = *this;
    pp.dpos   [0] = dpos   [0]+par.dpos   [0];
    pp.dpos   [1] = dpos   [1]+par.dpos   [1];
    pp.dpos   [2] = dpos   [2]+par.dpos   [2];
    pp.angles [0] = angles [0]+par.angles [0];
    pp.angles [1] = angles [1]+par.angles [1];
    pp.angles [2] = angles [2]+par.angles [2];
    pp.edpos  [0] = edpos  [0]+par.edpos  [0];
    pp.edpos  [1] = edpos  [1]+par.edpos  [1];
    pp.edpos  [2] = edpos  [2]+par.edpos  [2];
    pp.eangles[0] = eangles[0]+par.eangles[0];
    pp.eangles[1] = eangles[1]+par.eangles[1];
    pp.eangles[2] = eangles[2]+par.eangles[2];
    pp.chisq      = chisq     +par.chisq;
    pp.NDF        = NDF       +par.NDF;
    return pp;
  }

  TrExtAlignPar operator *(double a) const {
    TrExtAlignPar pp = *this;
    pp.dpos   [0] = dpos   [0]*a;
    pp.dpos   [1] = dpos   [1]*a;
    pp.dpos   [2] = dpos   [2]*a;
    pp.angles [0] = angles [0]*a;
    pp.angles [1] = angles [1]*a;
    pp.angles [2] = angles [2]*a;
    pp.edpos  [0] = edpos  [0]*a;
    pp.edpos  [1] = edpos  [1]*a;
    pp.edpos  [2] = edpos  [2]*a;
    pp.eangles[0] = eangles[0]*a;
    pp.eangles[1] = eangles[1]*a;
    pp.eangles[2] = eangles[2]*a;
    pp.chisq      = chisq*a;
    pp.NDF        = (int)(NDF*a);
    return pp;
  }

  TrExtAlignPar operator /(double a) const {
    TrExtAlignPar pp = *this;
    pp.dpos   [0] = dpos   [0]/a;
    pp.dpos   [1] = dpos   [1]/a;
    pp.dpos   [2] = dpos   [2]/a;
    pp.angles [0] = angles [0]/a;
    pp.angles [1] = angles [1]/a;
    pp.angles [2] = angles [2]/a;
    pp.edpos  [0] = edpos  [0]/a;
    pp.edpos  [1] = edpos  [1]/a;
    pp.edpos  [2] = edpos  [2]/a;
    pp.eangles[0] = eangles[0]/a;
    pp.eangles[1] = eangles[1]/a;
    pp.eangles[2] = eangles[2]/a;
    pp.chisq      = chisq/a;
    pp.NDF        = (int)(NDF/a);
    return pp;
  }

  void Print(Option_t *option = "") const;

  ClassDef(TrExtAlignPar, 1);
};


class TFile;

typedef unsigned int uint;

typedef std::map<uint, TrExtAlignPar>::iterator       ealgIT;
typedef std::map<uint, TrExtAlignPar>::const_iterator ealgITc;


/// Class implemting the databse for tracker external planes alignment DB
class TrExtAlignDB: public TObject{
protected:
  /// Map of the aligment const vs time for layer 8 (J1)
  std::map<uint, TrExtAlignPar> L8;
  /// Map of the aligment const vs time for layer 9 (J9)
  std::map<uint, TrExtAlignPar> L9;

  /// Static Accessor to the DB
  static TrExtAlignDB* Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head)
#endif

public:
  /// Static flag used to force the used of ext align data from TDV
  static int ForceFromTDV;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (ForceFromTDV)
#endif

  static int ForceLocalAlign;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (ForceLocalAlign)
#endif

  static int NoCiematLocal;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (NoCiematLocal)
#endif

  /// TDV version (1: no errors,  2: with errors) 
  static int version;  // It should be thread-common

  // Offset correction for PM4 alignment
  static float Sofs[4];

  // Offset correction for PM5 alignment (dY, dZ)
  static float Sofs2[8];

  static float SL1[18];
#pragma omp threadprivate(SL1)
  static float SL9[18];
#pragma omp threadprivate(SL9)
private:
  static int Ciemat;
#pragma omp threadprivate(Ciemat)

  /// Get TrExtAlignPar with [time] 
  TrExtAlignPar &GetM(int lay, uint time);

public:
  /// Std dummy constructor
  TrExtAlignDB(){}
  /// Destructor
  ~TrExtAlignDB(){}
  /// reset the content of the DB
  void Clear(Option_t *option = 0){ L8.clear(); L9.clear(); return;}

  /// Get number of entries
  int GetSize(int lay) const {
    if (lay == 8) return L8.size();
    if (lay == 9) return L9.size();
    return 0;
  }

  /// Find the closest time
  uint Find(int lay, uint time) const;

  /// Get the closest TrExtAlignPar to time
  const TrExtAlignPar *Get(int lay, uint time) const;

  /// Get a list of time in range
  std::vector<uint> GetVt(int lay, uint tmin = 0,
			           uint tmax = 2000000000) const;

  /// Get the first time
  uint GetTmin(int lay) const;

  /// Get the last time
  uint GetTmax(int lay) const;

  /// Get TrExtAlignPar with lower_bound
  TrExtAlignPar &FindM(int lay, uint time);

  ///  Get TrExtAlignPar with [time] for Madrid alignment 
  ///  The returned object includes the alignment parameters and the error
  ///  in the position of the origin of the layer. Errors in the angular 
  ///  parameters are not returned, neither NDF or chisquare.
  static TrExtAlignPar &GetMDyn(int lay, uint time);


  /// Update TrExtAlignPar[time]
  void AddM(int lay, uint time, Double_t *par);

  /// Fill an entry of the DB for Layer 8/9
  void Fill(int layer, uint time, const TrExtAlignPar &par)
  {
    if (layer == 8) L8[time] = par;
    if (layer == 9) L9[time] = par;
  }

  /// Fill  an entry of the DB for Layer8 (J1)
  void Fill_L8(uint time,
	       float dx,float dy,float dz,
	       float dalpha=0, float dbeta=0, float dgamma=0)
  {
    L8[time]=TrExtAlignPar(dx,dy,dz,dalpha,dbeta,dgamma);
    return;
  }
  /// Fill  an entry of the DB for Layer9 (J9)
  void Fill_L9(uint time,
	       float dx,float dy,float dz,
	       float dalpha=0, float dbeta=0, float dgamma=0)
  {
    L9[time]=TrExtAlignPar(dx,dy,dz,dalpha,dbeta,dgamma);
    return;
  }

  /// Check if there is an wrong entry
  int Check(void);

  /// Fill up the TkDBc with the most up-to-date alignment pars for ext planes
  /// plane =1 upper plane =2 lower plane = 3 both
  /// return 0 if success
  ///  return -1  if no AMSEventR found
  ///  return -2  of no TkDBc found
  ///  return -3 if no upper plane in tkdbc found
  ///  return -13 if no lower plane in tkdbc found
  ///  return -4 if no alig info for upper plane 
  ///  return -14 if no alig info for lower plane 
  //   return 1 if alignment reset was done
 
  static int UpdateTkDBcDyn(int run,uint time, int plane=3,int lad1=-1,int lad9=-1);

  /// Fill up the TkDBc with the most up-to-date alignment pars for ext planes
  /// Takes the required information from AMSEventR
  /// plane =1 upper plane =2 lower plane = 3 both
  /// return 0 if success
  ///  return -1  if no AMSEventR found
  ///  return -2  of no TkDBc found
  ///  return -3 if no upper plane in tkdbc found
  ///  return -13 if no lower plane in tkdbc found
  ///  return -4 if no alig info for upper plane 
  ///  return -14 if no alig info for lower plane 
  ///  return -5 if called outside ROOTSHAREDLIB env
  //   return 1 if alignment reset was done
  static int  UpdateTkDBcDyn(int plane=3);

  /// Fill up the TkDBc with the most up-to-date alignment pars for ext planes
  int UpdateTkDBc(uint time) const;

  /// Set Ext align paremeters to 0
  static void ResetExtAlign();

  /// Set Ext align paremeters to random number (supposed for MC)
  static void SmearExtAlign();

  /// Read from AMSRoot file or not
  static int OverLoadFlag;

  /// Load the DB from a file and make it available
  static void Load(TFile * ff);

  /// Loaoad the DB from a file and make it available
  static Bool_t Load(const char *fname, const char *dname = 0);

  /// Merge two dbs into one
  static Bool_t Load(TFile *f8, TFile *f9);

  /// Get the pointer to the DB singleton
  static  TrExtAlignDB* GetHead()
  { if(!Head) Head = new TrExtAlignDB(); return Head; }

  /// Print all the entries
  void Print(Option_t *option = "") const;

  /// Get TDV name
  static const char *GetTDVName();

  /// Load from TDV (ver= 1: no errors,  2: with errors) 
  static int GetFromTDV(uint time, int ver = 1, bool force = false);

  /// Update TDV (ver= 1: no errors,  2: with errors) 
  static int UpdateTDV(uint begin, uint end, int ver = 1);

  /// interface to GBTACH TDV database
  static float* fLinear;

  /// Number of parameters to be stored in TDV, depends on version
  static int GetNpar() {
    return (version >= 2) ? 6+6+2+2 : 6+2;
  }

  /// interface to GBTACH TDV database
  static int GetLinearSize() {
    int nlay = 2;
    int npar = GetNpar();
    int ntim = 500; 
    return nlay*npar*ntim*sizeof(float);
  }

  /// interface to GBTACH TDV database
  void ExAlign2Lin();

  /// interface to GBTACH TDV database
  void Lin2ExAlign();

  /// Setup the linear space for the DB
  static void CreateLinear() {
    if(fLinear) delete[] fLinear;
    int nlin = GetLinearSize()/4;
    fLinear = new float[nlin];
    for (int i = 0; i < nlin; i++) fLinear[i] = 0;
  }
  /// Produce disaligment for MC TESTS
  static void ProduceDisalignment(unsigned int * tt);
  static long long GetDt(float rate);
  static float GetDynCorr(int layerJ,int par){
    int off=0;
    if(Ciemat==1) off=6;
    else if(Ciemat==2) off=12;
    else off=0;
    float* ll=SL1;
    if(layerJ==9) ll=SL9;
    if(par>=6){
      printf("TrExtAlignDB::GetDynCoor-E- parameter request is out of boundaries MAX 5 reuested %d\n",par);
      return 0;
    }
    return ll[off+par];
  }

  /// Sets the kind of alignment to be use to calculate ext coo (0 PG 1 CIEMAT)
  static void  SetAlKind(int ii){
    if(ii==1) Ciemat=1;
    else if(ii==2) Ciemat=2;
    else Ciemat=0;
    return;
  }
  /// Recalculate the coordinates of all the hits of external planes according to a given alignment (0=PG, 1=CIEMA,T 2=NO_ALIGNAMENT)
  static int RecalcAllExtHitCoo(int kind);

  /// Return the difference in the hit position between PG and CIEMAT alignments (CIEMAT-PG)
  static AMSPoint  GetAlDist(TrRecHitR *hit);

  // Force using the latest alignment fomr TDV
  static void ForceLatestAlignmentFromTDV(int pgversion=2,char* CIEMAT_name="DynAlignmentV5T120628");


  ClassDef(TrExtAlignDB,4);
};

void SLin2ExAlign();

#endif


