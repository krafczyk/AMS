//  $Id: TrExtAlignDB.h,v 1.12.2.1 2011/12/05 14:47:51 pzuccon Exp $
#ifndef TREXTALIGNDB_H
#define TREXTALIGNDB_H

#include <map>
#include <vector>
#include "TObject.h"

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
  std::vector<uint> GetVt(int lay, uint tmin = 0, uint tmax = 2000000000);

  /// Get TrExtAlignPar with [time]
  TrExtAlignPar &GetM(int lay, uint time);

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
 
  static int UpdateTkDBcDyn(int run,uint time, int plane=3);

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

  /// Read from AMSRoot file or not
  static int OverLoadFlag;

  /// Loaoad the DB from a file and make it available
  static void Load(TFile * ff);

  /// Loaoad the DB from a file and make it available
  static Bool_t Load(const char *fname);

  /// Get the pointer to the DB singleton
  static  TrExtAlignDB* GetHead()
  { if(!Head) Head = new TrExtAlignDB(); return Head; }

  /// Print all the entries
  void Print(Option_t *option = "") const;


  /// interface to GBTACH TDV database
  static float* fLinear;

  /// interface to GBTACH TDV database
  static int GetLinearSize() {
    int nlay = 2;
    int npar = 2+6;
    int ntim = 500; 
    return nlay*npar*ntim*sizeof(float);
  }

  /// interface to GBTACH TDV database
  void ExAlign2Lin();

  /// interface to GBTACH TDV database
  void Lin2ExAlign();

  /// Setup the linear space for the DB
  static void CreateLinear() {
    int nlin = GetLinearSize()/4;
    fLinear = new float[nlin];
    for (int i = 0; i < nlin; i++) fLinear[i] = 0;
  }
  /// Produce disaligment for MC TESTS
  static void ProduceDisalignment(unsigned int * tt);
  static long long GetDt(float rate);


  ClassDef(TrExtAlignDB,1);
};

void SLin2ExAlign();

#endif


