#ifndef _TrdHRecon_
#define _TrdHRecon_
#include "point.h"
#include "TrdRawHit.h"
#include "TrdHSegment.h"
#include "TrdHTrack.h"

/// class to perform and store TRD H reconstruction
class TrdHReconR{
 public:
  /// vector of TrdRawHits (used as input to reconstruction)
  vector<TrdRawHitR*> rhits;
  
  /// vector of reconstructed TrdHSegmentss
  vector<TrdHSegmentR*> hsegvec;

  /// vector of reconstructed TrdHTracks
  vector<TrdHTrackR*> htrvec;

  // vector of reference hits (optional)
  vector<AMSPoint> refhits;

  // vector of error on reference hits (optional)
  vector<AMSPoint> referr;

  //#pragma omp threadprivate (hsegvec)

  /// default ctor
  TrdHReconR(){
    H2A_mvr[0]=0;H2A_mvr[1]=0;
    H2V_mvr[0]=0;H2V_mvr[1]=0;
  };

  /// read reconstructed TRD event from ROOT-file
  void ReadTRDEvent(vector<TrdRawHitR> r,vector<TrdHSegmentR> s,vector<TrdHTrackR> t);

  /// reconstruct TRD event according to TrdRawHit selection
  void BuildTRDEvent(vector<TrdRawHitR> r);

  /// pointer to array histograms (prefit) 
  TH2A* H2A_mvr[2];

  /// pointer to vector histograms (prefit) 
  TH2V* H2V_mvr[2];
  
  /// remove multiple reconstructed TrdHSegment
  vector<TrdHSegmentR*> clean_segvec(vector<TrdHSegmentR*> vec,int debug=0);
  
  /// run prefit
  int DoPrefit(vector<PeakXYZW> &seg_x, vector<PeakXYZW> &seg_y);

  /// run linear regression
  vector<TrdHSegmentR*> DoLinReg(vector<PeakXYZW> *segvec_x,vector<PeakXYZW> *segvec_y,int debug=0);
  
  /// check if combined 3D track would have hits outside TRD volume 
  bool check_geometry(TrdHSegmentR *s1, TrdHSegmentR* s2);

  bool check_hits(TrdHSegmentR *s1, TrdHSegmentR* s2,int debug=0);

  /// try to combine TrdHSegments to TrdHTracks
  int  combine_segments(int debug=0);

  // check if there is a secondary track in TRD (vertex)
  vector<pair<int,int> > check_secondaries();

  /// combine 2 TrdHSegments (2D) to 1 TrdHTrack (3D)
  TrdHTrackR* SegToTrack(TrdHSegmentR *s1, TrdHSegmentR* s2, int debug=0);

  ClassDef(TrdHReconR,1)
};

extern TrdHReconR trdhrecon;

#endif

