//  $Id: trigger3D.C,v 1.3 2001/01/22 17:32:31 choutko Exp $
// Original code by Choutko
// Last modification 19.09.97 AH.
#include <trigger3D.h>
#include <event.h>
#include <commons.h>
extern "C" float etime_(float ar[]);
  integer Triggerlvl3D::_TOFPattern[SCMXBR][SCMXBR];
  integer Triggerlvl3D::_TOFStatus[SCLRS][SCMXBR];
  integer Triggerlvl3D::_TrackerStatus[NTRHDRP2];
  integer Triggerlvl3D::_TrackerAux[NTRHDRP][2];
  integer Triggerlvl3D::_TOFAux[SCLRS][SCMXBR];
  integer Triggerlvl3D::_NTOF[SCLRS];
  geant Triggerlvl3D::_TOFCoo[SCLRS][SCMXBR][3];
  geant Triggerlvl3D::_TrackerCoo[NTRHDRP][2][3];
  geant Triggerlvl3D::_TrackerCooZ[nl];
  integer Triggerlvl3D::_TrackerDRP2Layer[NTRHDRP][2];
  const integer Triggerlvl3D::_patconf[LVL3NPAT][nl]={
                      0,1,2,3,4,5,   // 123456  0
                      0,1,2,3,5,0,   // 12346   1
                      0,1,2,4,5,0,   // 12356   2
                      0,1,3,4,5,0,   // 12456   3
                      0,2,3,4,5,0,   // 13456   4
                      0,1,2,3,4,0,   // 12345   5
                      1,2,3,4,5,0,   // 23456   6
                      0,1,2,3,0,0,   // 1234    7
                      0,1,2,4,0,0,   // 1235    8
                      0,1,2,5,0,0,   // 1236    9
                      0,1,3,4,0,0,   // 1245   10
                      0,1,3,5,0,0,   // 1246   11
                      0,1,4,5,0,0,   // 1256   12
                      0,2,3,4,0,0,   // 1345   13
                      0,2,3,5,0,0,   // 1346   14
                      0,2,4,5,0,0,   // 1356   15
                      0,3,4,5,0,0,   // 1456   16
                      1,2,3,4,0,0,   // 2345   17
                      1,2,3,5,0,0,   // 2346   18
                      1,2,4,5,0,0,   // 2356   19
                      1,3,4,5,0,0,   // 2456   20
                      2,3,4,5,0,0,   // 3456   21
                      0,1,2,0,0,0,   // 123    22
                      0,1,3,0,0,0,   // 124    23
                      0,1,4,0,0,0,   // 125    24
                      0,1,5,0,0,0,   // 126    25
                      0,2,3,0,0,0,   // 134    26
                      0,2,4,0,0,0,   // 135    27
                      0,2,5,0,0,0,   // 136    28
                      0,3,4,0,0,0,   // 145    29
                      0,3,5,0,0,0,   // 146    30
                      0,4,5,0,0,0,   // 156    31
                      1,2,3,0,0,0,   // 234    32
                      1,2,4,0,0,0,   // 235    33
                      1,2,5,0,0,0,   // 236    34
                      1,3,4,0,0,0,   // 245    35
                      1,3,5,0,0,0,   // 246    36
                      1,4,5,0,0,0,   // 256    37
                      2,3,4,0,0,0,   // 345    38
                      2,3,5,0,0,0,   // 346    39
                      2,4,5,0,0,0,   // 356    40
                      3,4,5,0,0,0};  // 456    41
                       
  const integer Triggerlvl3D::_patpoints[LVL3NPAT]=
    {6,5,5,5,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
     3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
  const integer Triggerlvl3D::_patd[5]={0,1,7,22,42};
  number Triggerlvl3D::_stripsize;
//  integer Triggerlvl3D::_TrackerOtherTDR[NTRHDRP][2];



Triggerlvl3D::Triggerlvl3D():_AntiTrigger(0),_TOFTrigger(0),
_TrackerTrigger(0),_NPatFound(0),_Time(0),_NTrHits(0){
  
  // Ctor

_Residual[0]=0;
_Residual[1]=0;
_Pattern[0]=-1;
_Pattern[1]=-1;
VZERO(_NTOF,SCLRS);
VZERO(_TrackerAux,NTRHDRP*2);
VZERO(_nhits,nl);

}
//
Triggerlvl3D::Triggerlvl3D(TriggerLVL3* p){
 _TOFTrigger     = p-> _TOFTrigger;
 _AntiTrigger    = p-> _AntiTrigger;
 _TrackerTrigger = p-> _TrackerTrigger;
 _NTrHits        = p->  _NTrHits;
 _NPatFound      = p-> _NPatFound;
//
for (integer i=0;i<2;i++){
 _Pattern[i]    = p->  _Pattern[i];
 _Residual[i]   = p->  _Residual[i];
}
 _Time          = p->  _Time;
//
//
for (integer j=0;j<nl;j++){
 _nhits[j]       = p->  _nhits[j];
  for (i=0;i<maxtrpl; i++){
  _drp[j][i]     = p->  _drp[j][i];
  _coo[j][i]     = p->  _coo[j][i];
  }
 _lowlimitY[j]   = p-> _lowlimitY[j];
 _upperlimitY[j] = p-> _upperlimitY[j];
 _lowlimitX[j]   = p->  _lowlimitX[j];
 _upperlimitX[j] = p->  _upperlimitX[j];
}
//
for (i=0;i<SCMXBR;i++){
   for (j=0;j<SCMXBR; j++){
    _TOFPattern[i][j] =  _TOFPattern[i][j]; 
   }
}
for (i=0;i<SCLRS;i++){
 _NTOF[i]              = p-> _NTOF[i];
    for (j=0;j<SCMXBR; j++){
    _TOFStatus[i][j]   = p-> _TOFStatus[i][j]; 
     _TOFAux[i][j]     = p-> _TOFAux[i][j];
      for (integer k=0;k<3;k++){
      _TOFCoo[i][j][k] = p-> _TOFCoo[i][j][k];
      }
    }
}
for (i=0;i<NTRHDRP;i++){
    for (j=0;j<2;j++){
    _TrackerDRP2Layer[i][j] = p->  _TrackerDRP2Layer[i][j];
    _TrackerAux[i][j]    = p-> _TrackerAux[i][j];
      for (integer k=0;k<3;k++){
       _TrackerCoo[i][j][k] = p -> _TrackerCoo[i][j][k];
      }
    }
}
for  (i=0;i<NTRHDRP2;i++){
    _TrackerStatus[i] = p-> _TrackerStatus[i];
}
for (j=0;j<nl;j++){
     _TrackerCooZ[j] = _TrackerCooZ[j];
}
    _stripsize = p-> _stripsize;
//
}
//
void Triggerlvl3D::getPattern(integer* Pattern)
     {UCOPY( &_Pattern, Pattern, sizeof(integer)*2/4);}
 
void Triggerlvl3D::getResidual(number* Residual)
     {UCOPY( &_Residual, Residual, sizeof(number)*2/4);}
//
void Triggerlvl3D::getnhits(integer* nhits){
     UCOPY( &_nhits,nhits,sizeof(integer)*nl/4);}

void Triggerlvl3D::getdrp(integer* bufdrp){
     UCOPY( &_drp,bufdrp,sizeof(integer)*maxtrpl*nl/4);}

void Triggerlvl3D::getcoo(geant* bufcoo){
     UCOPY( &_coo,bufcoo,sizeof(geant)*maxtrpl*nl/4);}

void Triggerlvl3D::getlowlimitY(geant* llimitY){
     UCOPY( &_lowlimitY,llimitY,sizeof(integer)*nl/4);}

void Triggerlvl3D::getupperlimitY(geant* ulimitY){
     UCOPY( &_upperlimitY,ulimitY,sizeof(integer)*nl/4);}

void Triggerlvl3D::getlowlimitX(geant* llimitX){
     UCOPY( &_lowlimitX,llimitX,sizeof(integer)*nl/4);}

void Triggerlvl3D::getupperlimitX(geant* ulimitX){
     UCOPY( &_upperlimitX,ulimitX,sizeof(integer)*nl/4);}

//
void Triggerlvl3D::getTOFPattern(integer* nbuff0){
     UCOPY( &_TOFPattern, nbuff0, sizeof(integer)*SCMXBR*SCMXBR/4); }

void Triggerlvl3D::getTOFStatus(integer* nbuff1){
     UCOPY( &_TOFStatus, nbuff1, sizeof(integer)*SCLRS*SCMXBR/4); }

void Triggerlvl3D::getTrackerStatus(integer* nbuff2){
     UCOPY( &_TrackerStatus, nbuff2, sizeof(integer)*NTRHDRP2/4); }

void Triggerlvl3D::getTrackerAux(integer* nbuff3){
     UCOPY( &_TrackerAux, nbuff3, sizeof(integer)*NTRHDRP*2/4); }

void Triggerlvl3D::getTOFAux(integer* nbuff4){
     UCOPY( &_TOFAux, nbuff4, sizeof(integer)*SCLRS*SCMXBR/4); }

void Triggerlvl3D::getNTOF(integer* nbuff5){
     UCOPY( &_NTOF, nbuff5, sizeof(integer)*SCLRS/4); }

void Triggerlvl3D::getTOFCoo(geant* nbuff6){
     UCOPY( &_TOFCoo, nbuff6, sizeof(geant)*SCLRS*SCMXBR*3/4); }

void Triggerlvl3D::getTrackerCoo(geant* nbuff7){
     UCOPY( &_TrackerCoo, nbuff7, sizeof(geant)*NTRHDRP*2*3/4); }

void Triggerlvl3D::getTrackerCooZ(geant* nbuff8){
     UCOPY( &_TrackerCooZ, nbuff8, sizeof(geant)*nl/4); }

void Triggerlvl3D::getTrackerDRP2Layer(integer* nbuff9){
     UCOPY( &_TrackerDRP2Layer,nbuff9, sizeof(integer)*NTRHDRP*2/4);}

