#include "ACQtProducer-include.C"
#include "GM_SubLibrary.h"

// FIXME: This is a temporary solution, once we merge the changes into vdev and there's a new release, it's obsolete.

//--------------------------------------------------------------------------------------------
// implement here to return MaxR for neg/pos
double ACsoft::ACQtProducer::GetCustomGeoCutoff(ParticleR* pPart, AMSEventR* pEvent) {

  double deg2rad = TMath::DegToRad();
  double Re =  6371.2; //km Earth radius
#ifndef __ROOTSHAREDLIBRARY__
  EventNtuple02 *ptr = AMSJob::gethead()->getntuple()->Get_event02();
  if (!ptr) return 0;
  time_t Utime = ptr->Time[0];
  //...km
  double Altitude = ptr->RadS/1.e5-Re;
  //...ISS rad
  double ThetaISS=  ptr->ThetaS;
  double PhiISS=    ptr->PhiS;
#else
  time_t Utime =pEvent->UTime() ;
  //...km
  double Altitude = pEvent->fHeader.RadS/1.e5-Re;
  //...ISS rad
  double ThetaISS=  pEvent->fHeader.ThetaS;
  double PhiISS=    pEvent->fHeader.PhiS;
#endif
  //...ISS deg 
  double thetaISS =ThetaISS/deg2rad ;
  double phiISS = PhiISS/deg2rad;
  //----to be centered in 0!!
  //............................ZeroCentered
  if(phiISS > 180)  phiISS-=360.;

  //.....particle direction in GTOD:
  double dirTheta = pPart->ThetaGl;//----------COLATITUDINE!  lat = colat -90
  double dirPhi   = pPart->PhiGl ;//------------LONG!
  //from polar to cos dir.
  AMSDir dir(dirTheta ,dirPhi);
  dir[0] = - dir[0] ;
  dir[1] = - dir[1] ;
  dir[2] = - dir[2] ;

  //....particle coordinates in GTOD deg  
  double theta = dir.gettheta()/deg2rad;
  //theta is colatitude --> I need Latitude:
  theta=90.- theta;
  double phi = dir.getphi()/deg2rad ;

  //...GV!!
  CutOffNegPos[0] = GeoMagCutoff( Utime,  Altitude , theta,  phi, thetaISS,  phiISS,  0 );  
  CutOffNegPos[1] = GeoMagCutoff( Utime,  Altitude , theta,  phi, thetaISS,  phiISS,  1 ) ;

  int pos = -1;
  int sign = pPart->Momentum>0?1:-1;//sign
  if (sign ==-1) pos = 0 ;//...negative particles         
  if (sign == 1) pos = 1 ;//...positive particles         

  //...GV!!
  double R = pPart->Charge * CutOffNegPos[pos];
  //Charge is part.Charge!        

  return R;

}

int ACsoft::ACQtProducer::DoCustomBacktracing(ParticleR* pPart) {

  AMSDir dir(pPart->Theta, pPart->Phi);
  int    ichg = pPart->Charge;
  double beta = pPart->Beta;
  double momt = pPart->Momentum;
  if (beta < 0) { dir  = dir*(-1); beta = -beta; }
  if (momt < 0) { momt = -momt;    ichg = -ichg; }

  int pos = (int)(ichg>0); // 0:Neg  1:Pos
  int neg = (int)(ichg<0); // 1:Neg  0:Pos

  if(CutOffNegPos[0]==0.0)  GetCustomGeoCutoff(pPart, AMSEventR::Head());

  float Eecal = 0.0; if( pPart->pEcalShower() ) Eecal = pPart->pEcalShower()->EnergyE;
  if( Eecal/momt < 0.5 ) Eecal = 0.0; // not e.m. shower

  float  rgt = ichg != 0 ? momt/ichg : 0.0;
  if(rgt<0) Eecal *= -1;

  int bt_status,ret;
  double glong = 0, glat = 0, RPTO[3] = { 0, 0, 0 }, time;
  ACsoft::ACQtProducer::BT_status = 0;

  pPart->BT_status = 0;

  if( fabs(Eecal/momt)>0.5 ){ // electron/positron candidate - backtrace also for -Eecal and +Eecal:

    ret = AMSEventR::Head()->DoBacktracing(pPart->BT_result, bt_status, glong, glat, RPTO, time, dir.gettheta(), dir.getphi(), fabs(Eecal), beta, -ichg);
    if(bt_status<0){ printf("DoBacktracing(): bt_status < 0 %4d\n",bt_status); bt_status = 0; }
    if(bt_status>3){ printf("DoBacktracing(): bt_status > 3 %4d\n",bt_status); bt_status = 0; }
    if(ret>=0){
      BT_status += bt_status<<(2*(2+neg));
    }
    ret = AMSEventR::Head()->DoBacktracing(pPart->BT_result, bt_status, glong, glat, RPTO, time, dir.gettheta(), dir.getphi(), fabs(Eecal), beta,  ichg);
    if(bt_status<0){ printf("DoBacktracing(): bt_status < 0 %4d\n",bt_status); bt_status = 0; }
    if(bt_status>3){ printf("DoBacktracing(): bt_status > 3 %4d\n",bt_status); bt_status = 0; }
    if(ret>=0){
      BT_status += bt_status<<(2*(2+pos));
    }
  }

  if(   fabs(rgt) < 1.5 * max(fabs(CutOffNegPos[0]),CutOffNegPos[1]) ){  // below 1.5*StoermerMaxCutoff

    ret = AMSEventR::Head()->DoBacktracing(pPart->BT_result, bt_status, glong, glat, RPTO, time, dir.gettheta(), dir.getphi(), momt, beta, -ichg);
    if(bt_status<0){ printf("DoBacktracing(): bt_status < 0 %4d\n",bt_status); bt_status = 0; }
    if(bt_status>3){ printf("DoBacktracing(): bt_status > 3 %4d\n",bt_status); bt_status = 0; }
    if(ret>=0) {
      BT_status += bt_status<<(2*neg);
    }
  }

  ret = AMSEventR::Head()->DoBacktracing(pPart->BT_result, bt_status, glong, glat, RPTO, time, dir.gettheta(), dir.getphi(), momt, beta,  ichg);
  if(bt_status<0){ printf("DoBacktracing(): bt_status < 0 %4d\n",bt_status); bt_status = 0; }
  if(bt_status>3){ printf("DoBacktracing(): bt_status > 3 %4d\n",bt_status); bt_status = 0; }
  if(ret>=0){ 
    pPart->BT_status = bt_status; // result for particle parameters (momt, beta,  ichg) - will also set BT_glong...  
    BT_status += bt_status<<(2*pos);
  }


  pPart->BT_glong   = glong;
  pPart->BT_glat    = glat;
  pPart->BT_RPTO[0] = RPTO[0];
  pPart->BT_RPTO[1] = RPTO[1];
  pPart->BT_RPTO[2] = RPTO[2];
  pPart->BT_time    = time;

  return ret;
}
