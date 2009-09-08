#include "trrec.h"


TrTrack::TrTrack(TrTrackR *tr){
  _r.setp(tr->P0[0],tr->P0[1],tr->P0[2]);
  _d=AMSDir(tr->Theta,tr->Phi);
}


void TrTrack::interpolate(AMSPoint pnt,AMSDir dir,AMSPoint &point,
			  double &theta,double &phi,double &length){
  theta=_d.gettheta();
  phi=_d.getphi();
  point[0]=_r[0]+tan(theta)*cos(phi)*(pnt[2]-_r[2]);
  point[1]=_r[1]+tan(theta)*sin(phi)*(pnt[2]-_r[2]);
  point[2]=pnt[2];
}
