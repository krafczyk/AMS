#ifndef __TRDHSeg__
#define __TRDHSeg__

#include "typedefs.h"
#include <vector>
#include "TrdRawHit.h"
#include <cmath>

using namespace std;
const float TRDz0=113.5;
const float RSDmax=0.6;

/// internal class to store peaks in histogram prefit
class PeakXYZW {
public:
  float x,y,z;
  float w;
  float zmin,zmax;

 PeakXYZW(float x_, float y_,float z_,float zmin_, float zmax_,float w_):x(x_),y(y_),z(z_),w(w_),zmin(zmin_),zmax(zmax_){}
 PeakXYZW():x(0.),y(0.),z(0.),w(0.){}

  bool operator<(const PeakXYZW& other) const {
    return w < other.w;
  }
  bool operator==(const PeakXYZW& other) const {
    if(w != other.w)return 0;
    if(x != other.x)return 0;
    if(y != other.y)return 0;
    //    if(z != other.z)return 0;
    return 1;//return ( && x == other.x && y == other.y);
  }
};

/// internal 2-dim histo with int[] array
class TH2A{   

 private:
  int nbx,nby,i,size;
  double Xlo,Xup,Ylo,Yup,Fx,Fy;
  double *Hxy;

 public:

  TH2A(const char* name, const char* title, int nbx, double Xlo, double Xup, int nby, double Ylo, double Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
    { Hxy  =  new double[nbx*nby]; 
      Fx   = (double)nbx/(Xup-Xlo);
      Fy   = (double)nby/(Yup-Ylo);
      size =  nbx*nby;             }

  void dump(){
    printf("nbx %i nby %i\n",nbx,nby);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    for(int i=0;i!=size;i++){
      if(Hxy[i]!=0)printf("bin %i entries %f\n",i,Hxy[i]);
    }
  }

  void Fill( double x, double y, double weight ){
    i = int((y-Ylo)*Fy)*nbx + int((x-Xlo)*Fx);
    if(i>0 && i<size) Hxy[i]+=weight;
  }

  void Reset(){ 
#ifndef __ICC__
    memset(Hxy, 0, sizeof(float)*nbx*nby); 
#else
    __builtin_memset(Hxy, 0, sizeof(float)*nbx*nby); 
#endif
  }

 
};


/// internal bin class
class  BIN {
public:
  int x,y;
  float c,z,zmin,zmax;
  BIN(int x_,int y_,int z_, float c_):x(x_),y(y_),z(z_),c(c_),zmin(z_),zmax(z_){}

    bool operator<(const BIN& other) const {
      return c > other.c;
    }
};

/// internal 2-dim histo with vector<BIN>
class TH2V{  
private:

  int nbx,nby,ix,iy;
  bool add;
  float Xlo,Xup,Ylo,Yup,Fx,Fy;

public:
  vector<BIN> histo;
  
  TH2V(const char* name, const char* title, int nbx, float Xlo, float Xup, int
       nby, float Ylo, float Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
  { Fx =(float)nbx/(Xup-Xlo);
    Fy =(float)nby/(Yup-Ylo);  }
  
  void dump(){
    printf("nbx %i nby %i ix %i iy %i\n",nbx,nby,ix,iy);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    //    for(int i=0;i!=histo.size();i++){
    //printf("bin %i x %i y %i c %i\n",i,histo[i].x,histo[i].y,histo[i].c);
    //    }
  }
  void Fill( float x, float y, float za, float zb, float weight );
  void Reset(){ histo.clear(); }

  vector<PeakXYZW> FindPeaks(int npeak=0);

};

/// Class to store 2D tracks in xz or yz plane
class TrdHSegmentR{
 public:
  /// Orientation (0=tube along y (measuring x), 1=tube along x (measuring y))  
  int    d;
  /// Slope of fit dr/dz
  float      m;
  /// Offset of fit at z
  float      r;
  /// COG z coordinate (geometric)
  float      z;
  /// Weighting of hits on track
  float      w;
  /// Uncertainty on slope
  float      em;
  /// Uncertainty on offset
  float      er;
  /// Number of hits on track
  int    Nhits;
  /// Chi2 (not normalized to ndf)
  float      Chi2;
  
  /// vector of RawHits on segment
  vector<int> fTrdRawHit;

  /// return number of hits
  int NTrdRawHit();

  /// return number of hits
  int nTrdRawHit();

  /// return global iterator of i-th hit
  int iTrdRawHit(unsigned int i);

  /// return pointer to i-th hit
  TrdRawHitR * pTrdRawHit(unsigned int i);
  
  /// default ctor
  TrdHSegmentR();

  /// ctor - no hits associated
  TrdHSegmentR(int d_, float m_, float em_, float r_, float er_,float z_, float w_);

  /// ctor - hits associated
  TrdHSegmentR(int d_, float m_, float em_, float r_, float er_, float z_, float w_, int Nhits_, TrdRawHitR* pthit[]);

  /// copy ctor
  TrdHSegmentR(TrdHSegmentR* seg);

  /// set vector of hits
  void SetHits(int Nhits_, TrdRawHitR* pthit[]);
  
  /// get residual to point r (offset) at z (height) and projection (0=xz, 1=yz)
  float resid(float r_, float z_, int d_);

  /// calculate chisqaure of hit vector
  void calChi2();

  /// build global vector of TrdHSegments
  static integer build(int rerun=0);
  
  /// printout
  void Print(int level=0);

  /// linear regression
  int LinReg(int debug=0);

  /// refit segment 
  TrdHSegmentR* Refit(int debug=0);

  /// virtual dtor
  virtual ~TrdHSegmentR(){};

  /// ROOT definition
  ClassDef(TrdHSegmentR, 5);
};




#endif
