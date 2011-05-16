#ifndef __TRDHTra__
#define __TRDHTra__

#include "TrdHSegment.h"
#include "typedefs.h"
#include "TrElem.h"

/// class to store 3D TRD track
class TrdHTrackR:public TrElem{
 public:

  /// Track coordinates
  float Coo[3];

  /// Track direction
  float Dir[3];

  /// Chi2
  float Chi2;

  /// Number of hits on track
  int Nhits;

  /// reconstructed charge
  float charge;

  // reconstructed electron likelihood (-log(like/(like+likp)))
  float elikelihood;

  /// Track fit status (1 - only TRD, 2- only TRD & matching TKtrack found, 3 - TRD fitted according to TKtrack)
  int status;

  /// vector of TrdHSegment iterators
  vector<int> fTrdHSegment;
  vector<TrdHSegmentR> segments; //!'

  float         elayer[20]; //!
  
  /// return number of segments (should be 2)
  int NTrdHSegment();
  /// return number of segments (should be 2)
  int nTrdHSegment();

  /// return global iterator of i-th segments
  int iTrdHSegment(unsigned int i);

  /// return pointer to i-th segment
  TrdHSegmentR * pTrdHSegment(unsigned int i);

  /// default ctor
  TrdHTrackR();
    
  /// ctor - position direction array
  TrdHTrackR(float Coo_[3],float Dir_[3]);

  /// copy ctor
  TrdHTrackR(TrdHTrackR *tr);
  
  /// return zenith angle (0 - upwards, pi - downwards)
  float Theta ();

  /// return error on zenith angle
  float ETheta (int debug=0);

  /// return azimuth angle (0 - positoive x direction)
  float Phi ();

  /// return error on azimuth angle (0 - positoive x direction)
  float EPhi (int debug=0);

  /// return slope in xz-projection
  float mx ();

  /// return error on slope in xz-projection
  float emx ();

  /// return slope in yz-projection
  float my ();

  /// return error on slope in yz-projection
  float emy (int debug=0);

  /// return error on position in xz-projection
  float ex ();

  /// return error on position in yz-projection
  float ey ();

  /// set segment pointers
  void SetSegments(TrdHSegmentR* segx, TrdHSegmentR* segy);
  //  void AddSegment(TrdHSegmentR* seg);

  /// set chiquare
  void setChi2(float Chi2_);
  
  /// return x and y for given z coordinate 
  void propagateToZ(float z, float &x , float& y);

  /// return path length inside tube for layer [cm]
  float TubePath(int layer, int ladder, int tube, int method=0,int  opt=0,int debug=0);
  float TubePath(TrdRawHitR* hit, int method=0,int  opt=0,int debug=0);

  float GetPathLengthMH(int layer, int ladder, int tube, int i);
  
  /// virtual dtor 
  virtual ~TrdHTrackR(){
    segments.clear();
    fTrdHSegment.clear();
    };

  void clear();

  void   Print(int opt=0);

  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("TrdHTrack Info");
  };

  char* Info(int iRef=0){return "TrdHTrack::Info";};

  std::ostream& putout(std::ostream &ostr = std::cout){
    _PrepareOutput(1);
    return ostr << sout  << std::endl; 
  };

  /// ROOT definition
#ifdef __MLD__
static int num;
static int numa;
static void print(){cout<<" tracks "<<num<<" arrays "<<numa<<endl;}

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
  ClassDef(TrdHTrackR, 9);
};


#endif
