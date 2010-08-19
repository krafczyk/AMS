#ifndef _TrdHRecon_
#define _TrdHRecon_
#include <vector>
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHSegment.h"
#include "TrdHTrack.h"
#include "VCon.h"
#include <cstring>
#include <algorithm>

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
  TH2A(){Hxy=0;}
 ~TH2A(){if(Hxy)delete Hxy;Hxy=0;} 
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
  bool add;
public:
  int nbx,nby,ix,iy;
  float Xlo,Xup,Ylo,Yup,Fx,Fy;
  vector<BIN> bins;

  TH2V(const char* name, const char* title, int nbx, float Xlo, float Xup, int
       nby, float Ylo, float Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
  { Fx =(float)nbx/(Xup-Xlo);
    Fy =(float)nby/(Yup-Ylo);  }

  void dump(){
    printf("nbx %i nby %i ix %i iy %i\n",nbx,nby,ix,iy);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    //    for(int i=0;i!=bins.size();i++){
      //printf("bin %i x %i y %i c %i\n",i,bins[i].x,bins[i].y,bins[i].c);
    //    }
  }
  void Fill( float x, float y, float za, float zb, float weight ){
    ix = int((x-Xlo)*Fx);
    iy = int((y-Ylo)*Fy);
    //    iz = int((z-Zlo)*Fz);
    add=true;
    for( int i=0; i<bins.size(); i++ ){
      if( ix==bins[i].x && iy==bins[i].y){// && iz == bins[i].z){
        bins[i].c+=weight;
	bins[i].z+=(za+zb)/2.;
	if(za<bins[i].zmin)bins[i].zmin=za;
	if(zb<bins[i].zmin)bins[i].zmin=zb;
	if(za>bins[i].zmax)bins[i].zmax=za;
	if(zb>bins[i].zmax)bins[i].zmax=zb;
        add = false;
        break;
      }
    }
    if( add ){
      BIN bin(ix,iy,(za+zb)/2.,weight);
      bins.push_back(bin);
    }
  }
  void Reset(){ bins.clear(); }

};


//namespace TRDHRECO{

/// class to perform and store TRD H reconstruction
class TrdHReconR{
 public:

  /// array of TrdRawHit pointers (used as input to reconstruction)
  int         irhits[1024];
  TrdRawHitR* rhits[1024];

  /// counter of TrdRawHit pointers
  int nrhits;

  /// array of reconstructed TrdHSegment pointers
  int          ihsegvec[100];
  TrdHSegmentR* hsegvec[100];
  
  /// counter of reconstructed TrdHSegment pointers
  int nhsegvec;

  /// array of reconstructed TrdHTrack pointers
  int        ihtrvec[20];
  TrdHTrackR* htrvec[20];
  
  /// counter of reconstructed TrdHTrack pointers
  int nhtrvec;

  // array of reference hits (optional)
  AMSPoint refhits[50];

  // array of error on reference hits (optional)
  AMSPoint referr[50];

  // counter of reference hits (optional)
  int nref;
  
  /// default ctor
  TrdHReconR(){
    for(int i=0;i<1024;i++){rhits[i]=0;irhits[i]=-1;}
    for(int i=0;i<100;i++){hsegvec[i]=0;ihsegvec[i]=-1;}
    for(int i=0;i<20;i++){htrvec[i]=0;ihtrvec[i]=-1;}
    for(int i=0;i<50;i++)referr[i]=0;
    for(int i=0;i<50;i++)refhits[i]=0;
    nrhits=0;
    nhsegvec=0;
    nhtrvec=0;
    nref=0;
  };

  ~TrdHReconR(){clear();}

  /// clear memory
  void clear();

  /// remove multiple reconstructed TrdHSegment
  int clean_segvec(int debug=0);
  
  /// run prefit
  int DoPrefit(int debug=0);

  /// run linear regression
  //  vector<TrdHSegmentR> 
  int DoLinReg(int debug=0);
  
  /// check if combined 3D track would have hits outside TRD volume 
  bool check_geometry(int is1, int is2);

  /// check if combined 3D track matches given reference hits
  bool check_hits(int is1, int i2,int debug=0);

  /// try to combine TrdHSegments to TrdHTracks
  int  combine_segments(int debug=0);

  // check if there is a secondary track in TRD (vertex)
  vector<pair<int,int> > check_secondaries();

  /// read reconstructed TRD event from ROOT-file
  void ReadTRDEvent(vector<TrdRawHitR> r,vector<TrdHSegmentR> s,vector<TrdHTrackR> t);

  /// reconstruct TRD event according to TrdRawHit selection
  void BuildTRDEvent(vector<TrdRawHitR> r);

  int retrdhevent();

  void AddSegment(TrdHSegmentR* hit);

  void AddTrack(TrdHTrackR* hit);

  void AddHit(TrdRawHitR* hit);

  /// combine 2 TrdHSegments (2D) to 1 TrdHTrack (3D)
  static TrdHTrackR* SegToTrack(TrdHSegmentR *s1, TrdHSegmentR* s2, int debug=0);

  ClassDef(TrdHReconR,4)
};
#endif

