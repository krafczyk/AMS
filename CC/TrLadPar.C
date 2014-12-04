//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadPar.C
///\brief Source file of TrLadPar class
///
///\date  2008/06/19 AO  First version
///
//////////////////////////////////////////////////////////////////////////
#include "TrLadPar.h"
#include "TkDBc.h"

#include <cstdio>
#include <string>
#include <cstring> 
#include <iostream>
#include <vector>
#include <algorithm>


ClassImp(TrLadPar);


TrLadPar::TrLadPar(short int hwid) {
  _hwid = (short int) hwid;
  Clear();
}


TrLadPar::TrLadPar(const TrLadPar& orig):TObject(orig){
  _hwid = orig._hwid;
  _filled=orig._filled;
  for (int ii=0; ii< 2; ii++) _gain[ii]   = orig._gain[ii];
  for (int ii=0; ii<16; ii++) _vagain[ii] = orig._vagain[ii];
}


void TrLadPar::Clear(const Option_t* aa){
  // TObject::Clear(aa);
  _filled=0;
  for (int ii=0; ii< 2; ii++) _gain[ii]   = 1.;
  for (int ii=0; ii<16; ii++) _vagain[ii] = 1.;
}


void TrLadPar::PrintInfo(){
  TkLadder* lad = TkDBc::Head->FindHwId(_hwid);
  if(!lad) {
    printf("TrLadPar::PrintInfo() - Error - Can't find the ladder with HwId %4d\n",_hwid);
    return;
  }
  printf(" %03d %+04d ",_hwid,lad->GetTkId());
  for (int ii=0; ii< 2; ii++) printf("%5.3f ",_gain[ii]);
  for (int ii=0; ii<16; ii++) printf("%5.3f ",_vagain[ii]);
  printf("\n");
}


int  TrLadPar::Par2Lin(float* offset){
  if(!offset) return -1;
  offset[0] = (float)GetHwId();
  offset[1] = _gain[0];
  offset[2] = _gain[1];
  float* off2 = &(offset[3]);
  for (int ii=0; ii<16; ii++)
    off2[ii] = _vagain[ii];
  return 0;
}


int  TrLadPar::Lin2Par(float* offset){
  if(!offset) return -1;
  _hwid    = (int)offset[0];
  _gain[0] = offset[1];
  _gain[1] = offset[2];
  float* off2 = &(offset[3]);
  for (int ii=0; ii<16; ii++)
    _vagain[ii] = off2[ii];
  return 0;

}
