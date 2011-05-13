#ifndef TREXTALIGNDB_H
#define TREXTALIGNDB_H
#include <map>
#include "TObject.h"


class TFile;

typedef unsigned int uint;


/// Class implemting the databse for tracker external planes alignment DB
class TrExtAlignDB: public TObject{
private:
  /// Repesentation of the plane alignement pars
  class apar{
  public:
    float dpos[3];   /// x,y,z
    float angles[3]; /// alpha, beta, gamma
    apar(){
      dpos[0]=dpos[1]=dpos[2]=0;
      angles[0]=angles[1]=angles[2]=0;
    }
    apar(float x,float y,float z, float a,float b ,float g){
      dpos[0]=x; dpos[1]=y; dpos[2]=z;
      angles[0]=a; angles[1]=b; angles[2]=g;
    }
    ClassDef(apar,1);
  };
  /// Map of the aligment const vs time for layer 8 (J1)
  std::map<uint, apar> L8;
  /// Map of the aligment const vs time for layer 9 (J9)
  std::map<uint, apar> L9;

  /// Static Accessor to the DB
  static TrExtAlignDB* Head;

public:
  /// Std dummy constructor
  TrExtAlignDB(){}
  /// Destructor
  ~TrExtAlignDB(){}
  /// reset the content of the DB
  void Clear(){ L8.clear(); L9.clear(); return;}
  /// Fill  an entry of the DB for Layer8 (J1)
  void Fill_L8(int time,
	       float dx,float dy,float dz,
	       float dalpha=0, float dbeta=0, float dgamma=0)
  {
    L8[time]=apar(dx,dy,dz,dalpha,dbeta,dgamma);
    return;
  }
  /// Fill  an entry of the DB for Layer9 (J9)
  void Fill_L9(int time,
	       float dx,float dy,float dz,
	       float dalpha=0, float dbeta=0, float dgamma=0)
  {
    L9[time]=apar(dx,dy,dz,dalpha,dbeta,dgamma);
    return;
  }
  /// Fill up the TkDBc with the most upt-to-date alignment pars for ext planes
  void UpdateTkDBc(uint time);
  /// Loaoad the DB from a file and make it available
  static void Load(TFile * ff);
  /// Get the pointer to the DB singleton
  static  TrExtAlignDB* GetHead()
  {if(!Head)Head=new TrExtAlignDB();return Head;}

  ClassDef(TrExtAlignDB,1);
};

#endif


