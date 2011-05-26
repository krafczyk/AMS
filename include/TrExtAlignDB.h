//  $Id: TrExtAlignDB.h,v 1.4 2011/05/26 01:30:29 pzuccon Exp $
#ifndef TREXTALIGNDB_H
#define TREXTALIGNDB_H
#include <map>
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

  void Print(Option_t *) const;

  ClassDef(TrExtAlignPar, 1);
};


class TFile;

typedef unsigned int uint;

/// Class implemting the databse for tracker external planes alignment DB
class TrExtAlignDB: public TObject{
private:
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
  void Clear(Option_t* oo=""){TObject::Clear(oo); L8.clear(); L9.clear(); return;}

  /// Get the closest TrExtAlignDB to time
  const TrExtAlignPar &Get(int lay, uint time) {
    std::map<uint, TrExtAlignPar>::iterator it;
    static TrExtAlignPar dummy;
    if (lay == 8 && (it = L8.lower_bound(time)) != L8.end()) return it->second;
    if (lay == 9 && (it = L9.lower_bound(time)) != L9.end()) return it->second;
    return dummy;
  }

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
  void UpdateTkDBc(uint time);
  /// Loaoad the DB from a file and make it available
  static void Load(TFile * ff);
  /// Get the pointer to the DB singleton
  static  TrExtAlignDB* GetHead()
  {if(!Head)Head=new TrExtAlignDB();return Head;}

  ClassDef(TrExtAlignDB,1);
};

#endif


