#ifndef __TRDHTra__
#define __TRDHTra__

#include "TrdHSegment.h"
#include "typedefs.h"

/// class to store 3D TRD track
class TrdHTrackR{
 public:

  /// Track coordinates
  float Coo[3];

  /// Track direction
  float Dir[3];

  /// Chi2
  float Chi2;

  /// Number of hits on track
  int Nhits;

  /// Track fit status (1 - only TRD, 2- only TRD & matching TKtrack found, 3 - TRD fitted according to TKtrack)
  int status;

  vector<int> fTrdHSegment;
  
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
  void SetSegment(TrdHSegmentR* segx, TrdHSegmentR* segy);

  /// set chiquare
  void setChi2(float Chi2_);
  
  /// return x and y for given z coordinate 
  void propagateToZ(float z, float &x , float& y);

  /// printout
  void Print();
  
  /// build global vector of TrdHTracks
  static integer build(int rerun=0);

  /// virtual dtor
  virtual ~TrdHTrackR(){};

  /// ROOT definition
  ClassDef(TrdHTrackR, 1);
};


#endif
