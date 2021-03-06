//  $Id: amsdbc.C,v 1.61 2011/05/16 16:02:40 mmilling Exp $
// Author V. Choutko 24-may-1996
#define __AMSDBC_C__ 
#include <math.h>
#include "amsdbc.h"
#include "amsstl.h"
//#include "commons.h"
//

char* AMSDBc::amsdatabase=0;
const uinteger AMSDBc::REFITTED=1;
const uinteger AMSDBc::WIDE=2;
const uinteger AMSDBc::AMBIG=4;
const uinteger AMSDBc::RELEASED=8;
const uinteger AMSDBc::BAD=16;
const uinteger AMSDBc::USED=32;
const uinteger AMSDBc::DELETED=64;
const uinteger AMSDBc::BADHIS=128;
const uinteger AMSDBc::ONESIDE=256;
const uinteger AMSDBc::BADTIME=512;
const uinteger AMSDBc::NEAR=1024;
const uinteger AMSDBc::WEAK=2048;
const uinteger AMSDBc::AwayTOF=4096;
const uinteger AMSDBc::FalseX=8192;
const uinteger AMSDBc::FalseTOFX=16384;
const uinteger AMSDBc::RECOVERED=16384*2;
const uinteger AMSDBc::LocalDB=16384*2*2;
const uinteger AMSDBc::GlobalDB=16384*2*2*2;
const uinteger AMSDBc::CHARGEUSED=16384*2*2*2*2;
const uinteger AMSDBc::GOOD      =16384*2*2*2*2*2;
const uinteger AMSDBc::BADINTERPOL =16384*2*2*2*2*2*2;
const uinteger AMSDBc::NOTRACK     =16384*2*2*2*2*2*2*2;
const uinteger AMSDBc::AOVERFLOW    =16384*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::TRDTRACK     =16384*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::LEAK         =16384*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::CATLEAK      =16384*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::JUNK         =16384*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::ECALTRACK    =16384*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::LOWGCHUSED   =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::S_AMBIG      =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::GOLDEN       =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;
const uinteger AMSDBc::TOFFORGAMMA  =16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2U;
//alas have to duplicate (all bits are occupied):
const uinteger AMSDBc::TGL1NONDATA=AMSDBc::BADHIS;
const uinteger AMSDBc::TGL1ASSERR=AMSDBc::ONESIDE;
const uinteger AMSDBc::TGL1SIDERR=AMSDBc::BADTIME;
const uinteger AMSDBc::ECHCISBAD=AMSDBc::BADHIS;
const uinteger AMSDBc::ECLCISBAD=AMSDBc::ONESIDE;
const uinteger AMSDBc::ECDCISBAD=AMSDBc::BADTIME;
const uinteger AMSDBc::TRDLOWOCC=AMSDBc::TOFFORGAMMA;
const uinteger AMSDBc::TRDGROUPED=AMSDBc::LOWGCHUSED;

integer AMSDBc::debug=1;
integer AMSDBc::BigEndian=0;
const number AMSDBc::raddeg=57.295779;
const number AMSDBc::pi=3.14159265358979;
const number AMSDBc::twopi=2*pi;
geant AMSDBc::ams_size[3]={390.,390.,390.};
const char AMSDBc::ams_name[]="AMSG";
geant AMSDBc::ams_coo[3]={0.,0.,0.};
const integer AMSDBc::ams_rotmno=1;
number AMSDBc::ams_nrm[3][3];
const geant AMSDBc::amsdext=43.;

void AMSDBc::init(float angle){
  angle=angle/180*AMSDBc::pi;
  ams_nrm[0][0]=cos(angle);
  ams_nrm[1][0]=0;
  ams_nrm[2][0]=-sin(angle);
  ams_nrm[0][1]=0;
  ams_nrm[1][1]=1;
  ams_nrm[2][1]=0;
  ams_nrm[0][2]=sin(angle);
  ams_nrm[1][2]=0;
  ams_nrm[2][2]=cos(angle);
  // cout <<"  angle "<<angle<<endl;
}

void AMSDBc::transform(geant coo[3]){
  geant coo1[3];
  memcpy(coo1,coo,sizeof(coo1));
  for(int i=0;i<3;i++){
    coo[i]=0;
    for(int k=0;k<3;k++)coo[i]+=coo1[k]*ams_nrm[k][i];
  }
}

void AMSDBc::transform(AMSPoint & dir){
  number coo[3];
  number coo1[3];
  for(int k=0;k<3;k++)coo1[k]=dir[k];
  for(int i=0;i<3;i++){
    coo[i]=0;
    for(int k=0;k<3;k++)coo[i]+=coo1[k]*ams_nrm[k][i];
  }
  for(int k=0;k<3;k++)dir[k]=coo[k];
}

#ifndef __DARWIN__
#include <fenv.h>

void fegetexcept_(int * val){
*val=fegetexcept();
}

void fesetexcept_(int *val){
feclearexcept(*val);
feenableexcept(*val);
}

void feunsetexcept_(){
fedisableexcept(FE_ALL_EXCEPT);
}
#endif
