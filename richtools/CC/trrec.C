#include "trrec.h"


TrTrack::TrTrack(TrTrackR *tr){
#ifndef _PGTRACK_
  _r.setp(tr->P0[0],tr->P0[1],tr->P0[2]);
  _d=AMSDir(tr->Theta,tr->Phi);
#else
  _r=tr->GetP0();
  _d=tr->GetDir();
#endif

}


void TrTrack::interpolate(AMSPoint pnt,AMSDir dir,AMSPoint &point,
			  double &theta,double &phi,double &length){
  // Find the impact point with the plane given by pnt and dir

  theta=_d.gettheta();
  phi=_d.getphi();
  double t=(pnt-_r).prod(dir)/_d.prod(dir);
  point=_r+_d*t;
}
