#ifndef __TRDHTra__
#define __TRDHTra__

#include "TrdHSegment.h"
#include "typedefs.h"
#include "TrElem.h"
#include <map>

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

  /// reconstructed absolute charge (positive! 0:e 1:p 2:He etc. -1:N/A)
  float charge;

  /// reconstructed electron likelihood (-log(like/(like+likp)))
  float elikelihood;

  /// Track fit status (1 - only TRD, 2- only TRD & matching TKtrack found, 3 - TRD fitted according to TKtrack)
  int status;

  /// energy depositions [ADC] on track per layer
  float elayer[20]; //!
  
  /// vector of TrdHSegment iterators
  vector<int> fTrdHSegment;
  vector<TrdHSegmentR> segments; //!

  /// container to store charge probabilities
  map<double,int> charge_probabilities; //!

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

  int UpdateLayerEdep(int corr, float bg,int charge, int debug=0);

  /// get probability for 'charge' (0:e, 1:p, 2:He)
  double GetProb(int charge, int debug=0);

  /// new likelihood method - multihypothesis - sig selected by 0/1/2 e/p/he - bkg bitwise flagged by -1:all 0:use electron 1:use proton 2:use helium etc. 
  float GetLikelihood(int sig=0,int bkg=1,int debug=0);
  
  /// get number of layers above CC amplitude cut
  int GetNCC(float cut=200.,int debug=0);
  
  /// virtual dtor 
  virtual ~TrdHTrackR(){
    segments.clear();
    fTrdHSegment.clear();
    charge_probabilities.clear();
  };

  void clear();

  void Print(int opt=0);

  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("TrdHTrack Info");
  };

  char* Info(int iRef=0){
    sprintf(_Info,"TrdHTrack Coo=(%5.2f,%5.2f,%5.2f)#pm((%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g NHits=%d chg %i elik %.2f",Coo[0],Coo[1],Coo[2],ex(),ey(),0.,Theta(),Phi(),Chi2,Nhits,(int)charge,elikelihood);
    return _Info;
  };

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
  ClassDef(TrdHTrackR, 10);
};


#endif
