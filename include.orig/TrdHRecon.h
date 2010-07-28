#ifndef _TrdHRecon_
#define _TrdHRecon_
#include <vector>
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHSegment.h"
#include "TrdHTrack.h"

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
  void Fill( float x, float y, float za, float zb, float weight ){
    ix = int((x-Xlo)*Fx);
    iy = int((y-Ylo)*Fy);
    //    iz = int((z-Zlo)*Fz);
    add=true;
    for( int i=0; i<histo.size(); i++ ){
      if( ix==histo[i].x && iy==histo[i].y){// && iz == histo[i].z){
        histo[i].c+=weight;
	histo[i].z+=(za+zb)/2.;
	if(za<histo[i].zmin)histo[i].zmin=za;
	if(zb<histo[i].zmin)histo[i].zmin=zb;
	if(za>histo[i].zmax)histo[i].zmax=za;
	if(zb>histo[i].zmax)histo[i].zmax=zb;
        add = false;
        break;
      }
    }
    if( add ){
      BIN bin(ix,iy,(za+zb)/2.,weight);
      histo.push_back(bin);
    }
  }
  void Reset(){ histo.clear(); }

  void FindPeaks(int d,int npeak=0);

};


//namespace TRDHRECO{

/// class to perform and store TRD H reconstruction
class TrdHReconR{
 public:
  static TrdHReconR* getInstance();
  static void Initialize(int n);

  /// array of TrdRawHit pointers (used as input to reconstruction)
  TrdRawHitR** rhits;
  
  /// counter of TrdRawHit pointers
  int nrhits;

  /// array of peak pointers (peaks in internal 2D (slope/offset in xz and yz)  prefit )
  PeakXYZW*** peakvec;

  /// counter of peak pointers 
  int npeak[2];

  /// array of reconstructed TrdHSegment pointers
  TrdHSegmentR** hsegvec;
  
  /// counter of reconstructed TrdHSegment pointers
  int nhsegvec;

  /// array of reconstructed TrdHTrack pointers
  TrdHTrackR** htrvec;
  
  /// counter of reconstructed TrdHTrack pointers
  int nhtrvec;

  // array of reference hits (optional)
  AMSPoint refhits[50];

  // array of error on reference hits (optional)
  AMSPoint referr[50];

  // counter of reference hits (optional)
  int nref;
  
  /// pointers to array histograms (2D prefit in xz/yz) 
  TH2A* H2A_mvr[2];

  /// pointers to vector histograms (2D prefit in xz/yz) 
  TH2V* H2V_mvr[2];
  

  /// default ctor
  TrdHReconR(){
    rhits=(TrdRawHitR**)malloc(sizeof(TrdRawHitR*)*1024);
    hsegvec=(TrdHSegmentR**)malloc(sizeof(TrdHSegmentR*)*100);
    htrvec=(TrdHTrackR**)malloc(sizeof(TrdHTrackR*)*20);
    peakvec=(PeakXYZW***)malloc(sizeof(PeakXYZW**)*2048);
    for(int i=0;i!=2;i++)peakvec[i]=(PeakXYZW**)malloc(sizeof(PeakXYZW*)*1024);

    nref=0;

    for(int i=0;i!=1024;i++)rhits[i]=0;nrhits=0;
    for(int i=0;i!=50;i++)hsegvec[i]=0;nhsegvec=0;
    for(int i=0;i!=10;i++)htrvec[i]=0;nhtrvec=0;

    for(int i=0;i!=2;i++){
      npeak[i]=0;
      for(int j=0;j!=1024;j++)peakvec[i][j]=0;
    }

    H2A_mvr[0]=0;H2A_mvr[1]=0;
    H2V_mvr[0]=0;H2V_mvr[1]=0;
  };

  /// reset all variables
  void reset();

  /// remove multiple reconstructed TrdHSegment
  //  vector<TrdHSegmentR> 
  int clean_segvec(int debug=0);
  
  /// run prefit
  int DoPrefit(int debug=0);

  /// run linear regression
  //  vector<TrdHSegmentR> 
  int DoLinReg(int debug=0);
  
  /// check if combined 3D track would have hits outside TRD volume 
  bool check_geometry(TrdHSegmentR *s1, TrdHSegmentR* s2);

  /// check if combined 3D track matches given reference hits
  bool check_hits(TrdHSegmentR *s1, TrdHSegmentR* s2,int debug=0);

  /// try to combine TrdHSegments to TrdHTracks
  int  combine_segments(int debug=0);

  // check if there is a secondary track in TRD (vertex)
  vector<pair<int,int> > check_secondaries();

  /// read reconstructed TRD event from ROOT-file
  void ReadTRDEvent(vector<TrdRawHitR> r,vector<TrdHSegmentR> s,vector<TrdHTrackR> t);

  /// reconstruct TRD event according to TrdRawHit selection
  void BuildTRDEvent(vector<TrdRawHitR> r);

  /// combine 2 TrdHSegments (2D) to 1 TrdHTrack (3D)
  TrdHTrackR* SegToTrack(TrdHSegmentR *s1, TrdHSegmentR* s2, int debug=0);

  ClassDef(TrdHReconR,2)
};

static TrdHReconR **trdhreconarr=0;
static int ntrdhrecon=0;
#endif

