//  $Id: amsdbc.C,v 1.39 2001/08/03 17:28:02 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include <math.h>
#include <amsdbc.h>
#include <amsstl.h>
#include <job.h>
//

char* AMSDBc::amsdatabase=0;
const integer AMSDBc::REFITTED=1;
const integer AMSDBc::WIDE=2;
const integer AMSDBc::AMBIG=4;
const integer AMSDBc::RELEASED=8;
const integer AMSDBc::BAD=16;
const integer AMSDBc::USED=32;
const integer AMSDBc::DELETED=64;
const integer AMSDBc::BADHIS=128;
const integer AMSDBc::ONESIDE=256;
const integer AMSDBc::BADTIME=512;
const integer AMSDBc::NEAR=1024;
const integer AMSDBc::WEAK=2048;
const integer AMSDBc::AwayTOF=4096;
const integer AMSDBc::FalseX=8192;
const integer AMSDBc::FalseTOFX=16384;
const integer AMSDBc::RECOVERED=16384*2;
const integer AMSDBc::LocalDB=16384*2*2;
const integer AMSDBc::GlobalDB=16384*2*2*2;
const integer AMSDBc::CHARGEUSED=16384*2*2*2*2;
const integer AMSDBc::GOOD      =16384*2*2*2*2*2;
const integer AMSDBc::BADINTERPOL =16384*2*2*2*2*2*2;
const integer AMSDBc::NOTRACK     =16384*2*2*2*2*2*2*2;
const integer AMSDBc::AOVERFLOW    =16384*2*2*2*2*2*2*2*2;
const integer AMSDBc::TRDTRACK     =16384*2*2*2*2*2*2*2*2*2;
const integer AMSDBc::LEAK         =16384*2*2*2*2*2*2*2*2*2*2;
const integer AMSDBc::CATLEAK      =16384*2*2*2*2*2*2*2*2*2*2*2;
const integer AMSDBc::JUNK         =16384*2*2*2*2*2*2*2*2*2*2*2*2;

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

void AMSDBc::init(){
 number angle=0;
 if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
  angle=-10./180*pi;
 }
 ams_nrm[0][0]=cos(angle);
 ams_nrm[1][0]=0;
 ams_nrm[2][0]=-sin(angle);
 ams_nrm[0][1]=0;
 ams_nrm[1][1]=1;
 ams_nrm[2][1]=0;
 ams_nrm[0][2]=sin(angle);
 ams_nrm[1][2]=0;
 ams_nrm[2][2]=cos(angle);

}
