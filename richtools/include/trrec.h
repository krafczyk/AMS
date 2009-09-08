#ifndef __TRTRACK__
#define __TRTRACK__

#include "point.h"
#include "root.h"

class TrTrack{
 public:
  AMSPoint _r;
  AMSDir   _d;
 public:
  TrTrack(AMSPoint r,AMSDir p):_r(r),_d(p){};
  TrTrack(TrTrackR *tr);
  void interpolate(AMSPoint pnt,AMSDir dir,AMSPoint &point,
		   double &theta,double &phi,double &length);
};

#endif
