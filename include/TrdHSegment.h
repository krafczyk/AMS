#ifndef __TRDHSeg__
#define __TRDHSeg__

#include "typedefs.h"
#include "TrdRawHit.h"
#include <cmath>
#include "TrElem.h"

using namespace std;

class AMSTRDRawHit;

/// Class to store 2D tracks in xz or yz plane
class TrdHSegmentR: public TrElem {
 public:
  /// Orientation (0=tube along y (measuring x), 1=tube along x (measuring y))  
  int    d;
  /// Slope of fit dr/dz
  float      m;
  /// Offset of fit at z
  float      r;
  /// COG z coordinate (geometric)
  float      z;
  /// Weighting of hits on segment
  float      w;
  /// Uncertainty on slope
  float      em;
  /// Uncertainty on offset
  float      er;
  /// Number of hits on segment
  int    Nhits;
  /// Chi2 (not normalized to ndf)
  float      Chi2;


  /// vector of TrdRawHit iterators
  vector<int> fTrdRawHit;

  vector<TrdRawHitR> hits; //!

  vector<AMSTRDRawHit*> gbhits; //!

  /// return number of hits
  int NTrdRawHit();

  /// return number of hits
  int nTrdRawHit();

  /// return global iterator of i-th hit
  int iTrdRawHit(unsigned int i);

  /// return pointer to i-th hit
  TrdRawHitR *pTrdRawHit(unsigned int i);
  
  /// default ctor
  TrdHSegmentR();

  /// ctor - no hits associated
  TrdHSegmentR(int d_, float m_, float em_, float r_, float er_,float z_, float w_);

  /// ctor - hits associated
  TrdHSegmentR(int d_, float m_, float em_, float r_, float er_, float z_, float w_, int Nhits_, TrdRawHitR* pthit[]);

  /// copy ctor
  TrdHSegmentR(TrdHSegmentR* seg);

  /// set hits array
  void SetHits(int Nhits_, TrdRawHitR* pthit[]);
  
  /// get residual to point r (offset) at z (height) and projection (0=xz, 1=yz)
  float resid(float r_, float z_, int d_);

  /// calculate chisqaure of hit array
  void calChi2();

  /// printout
  void Print(int level=0);

  /// linear regression
  int LinReg(int debug=0);

  /// refit segment 
  int Refit(int debug=0);

  void AddHit(TrdRawHitR hit,int iter);

  void RemoveHit(int iter);

  /// virtual dtor
  virtual ~TrdHSegmentR(){
    fTrdRawHit.clear();
    hits.clear();
    gbhits.clear();
    clear();}

  void clear();

  bool operator==(const TrdHSegmentR& other) const {
    if(d != other.d)return 0;
    if(m != other.m)return 0;
    if(r != other.r)return 0;
    if(z != other.z)return 0;
    if(w != other.w)return 0;
    return 1;
  }

  std::string _PrepareOutput(int opt=0){
    return "TrdHSegment Info";
  };

  const char* Info(int iRef=0){return "TrdHSegment::Info";};

  std::ostream& putout(std::ostream &ostr = std::cout){
    return ostr << _PrepareOutput(1)  << std::endl; 
  };

  /// ROOT definition
#ifdef __MLD__
static int num;
static int numa;
static void print(){cout<<" segs "<<num<<" arrays "<<numa<<endl;}

void* operator new(size_t t) {
  num++;
  return ::operator new(t);
}

void operator delete(void *p){
if(p){
num--;
::operator delete(p);
p=0;
}
} 


void* operator new[](size_t t) {
  numa++;
  return ::operator new[](t);
}

void operator delete[](void *p){
if(p){
numa--;
::operator delete[](p);
p=0;
}
} 

#endif


  ClassDef(TrdHSegmentR, 9);
};
#endif
