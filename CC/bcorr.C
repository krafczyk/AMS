//  $Id: bcorr.C,v 1.9 2013/02/12 15:50:13 choutko Exp $
#include "root.h"
#include "root_setup.h"
#include "bcorr.h"
#ifdef __ROOTSHAREDLIBRARY__
//  ClassImp(MagnetTemperature)
//  ClassImp(MagnetVarp)
#else
#include "event.h"
#include "commons.h"
#endif
extern "C" void btempcor_(float &factor);

extern "C" void bzcorr_(float& factor){
#ifdef __ROOTSHAREDLIBRARY__
#else
 factor=MISCFFKEY.BZCorr;
#endif
}

extern "C" void btempcor_(float& factor) {
 MagnetVarp::btempcor(factor);
}

void MagnetVarp::MagnetTemperature::loadValues(unsigned int utime,bool ccebload){
const string sensort[12]={"Port","Ram","StarBoard","Wake","M-3X:31:UCF3","M-3X:32:UCF4","M-3X:35:UCF7","M-3X:36:UCF8","M-3X:39:LCF3","M-3X:40:LCF4","M-3X:43:LCF7","M-3X:44:LCF8"};
const string sensorb[4][4]={"B-Field P0 Component X","B-Field P0 Component Y","B-Field P0 Component Z","B-Field P0 Sensor Tmp",
"B-Field P1 Component X","B-Field P1 Component Y","B-Field P1 Component Z","B-Field P1 Sensor Tmp",
"B-Field P2 Component X","B-Field P2 Component Y","B-Field P2 Component Z","B-Field P2 Sensor Tmp",
"B-Field P3 Component X","B-Field P3 Component Y","B-Field P3 Component Z","B-Field P3 Sensor Tmp"};
nsens=0;
ncceb=0;
if(! AMSSetupR::gethead())return;

for(int k=0;k<sizeof(stemp)/sizeof(stemp[0]);k++){
 vector<float>value;
 int ret= AMSSetupR::gethead()->fSlowControl.GetData(sensort[k].c_str(),utime,0.5,value);
 if(!ret && value.size()){
   nsens|=(1<<k);
   stemp[k]=value[0];
 }

}
if(ccebload){
for(int k=0;k<4;k++){
for(int j=0;j<4;j++){
 vector<float>value;
 int ret= AMSSetupR::gethead()->fSlowControl.GetData(sensorb[k][j].c_str(),utime,0.5,value);
 if(!ret && value.size()){
   int l=k*4+j;
   ncceb|=(1<<l);
   cceb[k][j]=value[0];
 }
}
}
}

if(nsens || ncceb)time=utime;
return;

}

int MagnetVarp::MagnetTemperature::getmeanmagnettemp(float &temp,int method){
float tlow=-50;
float thigh=+50;
int beg=0;
int end=0;
if(method==0)end=sizeof(stemp)/sizeof(stemp[0]);
else if(method==1)end=4;
else if(method==2){
 beg=4;
 end=sizeof(stemp)/sizeof(stemp[0]);
}
else if(method<0 && method>=-sizeof(stemp)/sizeof(stemp[0])){
beg=-method-1;
end=beg+1;
}
int nmeas=0;
temp=0;
for(int k=beg;k<end;k++){
 if((nsens & (1<<k))){
  nmeas++;
  temp+=stemp[k];
 }
}
if(nmeas>1 || (nmeas==1 && method<0)){
 temp/=nmeas;
 if(temp<tlow || temp>thigh)return 3;
 else return 0;
}
else if(nmeas==1 )return 1;
else return 2;
}
int MagnetVarp::getmagnettemp(float &temp,unsigned int time,unsigned int sensor){
temp=0;
if(time==0){
#ifdef  __ROOTSHAREDLIBRARY__
if(!AMSEventR::Head())return 1;
 else time=AMSEventR::Head()->UTime();
#else
if(!AMSEvent::gethead())return 1;
 else time=AMSEvent::gethead()->gettime();
#endif
}
if(time!=MagnetVarp::mgtt.time){
 MagnetVarp::mgtt.loadValues(time,true);
 if(time!=MagnetVarp::mgtt.time)return 2;
}
if(sensor>=12)return 3;
if(mgtt.nsens&(1<<sensor)){
temp=mgtt.stemp[sensor];
return 0;
}
else return 4;
}
int MagnetVarp::getmagfield(float &temp,unsigned int time,unsigned int sensor,unsigned int comp){
temp=0;
if(time==0){
#ifdef  __ROOTSHAREDLIBRARY__
if(!AMSEventR::Head())return 1;
 else time=AMSEventR::Head()->UTime();
#else
if(!AMSEvent::gethead())return 1;
 else time=AMSEvent::gethead()->gettime();
#endif
}
if(time!=MagnetVarp::mgtt.time){
 MagnetVarp::mgtt.loadValues(time,true);
 if(time!=MagnetVarp::mgtt.time)return 2;
}
if(sensor>=4 || comp>=4)return 3;
int l=sensor*4+comp;
if(mgtt.ncceb&(1<<l)){
temp=mgtt.cceb[sensor][comp];
return 0;
}
else return 4;
}

int MagnetVarp::btempcor(float &factor,unsigned int time,int method){
factor=1;
if(time==0){
#ifdef  __ROOTSHAREDLIBRARY__
if(!AMSEventR::Head())return 1;
 else time=AMSEventR::Head()->UTime();
#else
if(!AMSEvent::gethead())return 1;
 else time=AMSEvent::gethead()->gettime();
#endif
}
if(time!=MagnetVarp::mgtt.time){
 MagnetVarp::mgtt.loadValues(time,true);
 if(time!=MagnetVarp::mgtt.time)return 2;
}
float temp;
int ret=MagnetVarp::mgtt.getmeanmagnettemp(temp,method);
if(ret)return ret+2;
  const float dBdT=0.13e-2;
  const float TETH=18.5;
  factor = 1.-dBdT*(temp-TETH);
return 0;
}

MagnetVarp::MagnetTemperature MagnetVarp::mgtt;
