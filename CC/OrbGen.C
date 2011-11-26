#include "OrbGen.h"
#include <math.h>
#include "cgmmap.h"
#include "TF1.h"
#include "TProfile2D.h"
#include "TFile.h"
#include "pfunction.h"
#include "random.h"
#include "amsdbc.h"


time_rep& time_rep::operator+(const time_rep o){
  Time_s+=o.Time_s;
  uint ns=Time_ns+o.Time_ns;
  uint tt=ns/1000000000;
  uint ns2=ns%1000000000;
  Time_s+=tt;
  Time_ns=ns2;
  return *this;
}

time_rep& time_rep::operator-(const time_rep o){
  long long diff = Time_s  - o.Time_s;
  long long  ns  = Time_ns - o.Time_ns;
  if(diff>0) {
    if ( ns<0 ){
      diff--;
      ns=1000000000+ns;
    }
  }else if(diff==0) {
    if ( ns<0 ){
      ns=1000000000+ns;
      sign=-1;
    }
  }else{
    if (ns<0){
      diff++;
      ns=(1000000000+ns);
      sign=-1;
    }
  }
  Time_s=abs(diff);
  Time_ns=ns;
  return *this;
}

OrbGen* OrbGen::Head=0;

OrbGen::OrbGen():Lat(0),Lon(0),CLat(0),CLon(0),Rigidity(0),cutoff(0),Pid(0){
  Coo[0]=Coo[1]=Coo[2]=0;
  Dir[0]=Dir[1]=Dir[2]=0;

  proton= new MTF1("Proton",ProtonSpectrum,0.1,10000,1);
  proton->SetParameter(0,3768);
  
  mexp= new MTF1("exp","[0]*exp(-x/[1])",0,.1);
  mexp->SetParameters(1,400.);
  char name[300];
  sprintf(name,"%s/v5.00/OrbitGeneration.root",getenv("AMSDataDir"));
  TFile f(name);
  Rate=(TProfile2D*) f.Get("RateMapC");
  Rate->SetDirectory(0);
  LT=(TProfile2D*) f.Get("LTMapC");
  LT->SetDirectory(0);
  f.Close();
  
}

OrbGen::~OrbGen(){  
  if(proton) delete proton;
  if(mexp) delete mexp;
  if(Rate) delete Rate;
  if(LT) delete LT;
}

void OrbGen::NextTime(){
  int bx=Rate->GetXaxis()->FindBin(CLon);
  int by=Rate->GetYaxis()->FindBin(CLat);

  float rate=Rate->GetBinContent(bx,by);
  float lt=LT->GetBinContent(bx,by);
  if(lt<0.7) rate=1000;
  if(rate>1500) rate=1000;
  if(rate<50) rate=50;
  mexp->SetParameters(1,rate);
  uint rr=mexp->GetRandom()*1E9;
  //  printf("%d %d\n",rr/1000000000,rr%1000000000);
  time_rep dd(rr/1000000000,rr%1000000000);
  time_rep d0=Time;
  TimeN=d0+dd;
  return;
}



void OrbGen::TimePos(){

  //  void* fun=(void*)ProtonSpectrum;
  Time=TimeN;

  GetPos(Time.Time_s,Lat,Lon);
  CGMmap::GetCGMRad(Lat, Lon,CLat,CLon);
  cutoff=StormerCutoff(CLat);
  return;
}

double OrbGen::StormerCutoff(double ThetaM,double alt){
  double r=(6371.14+alt)/6371.14;
  return 59.6*pow(cos(ThetaM),4)/(4*r*r);
}
void OrbGen::GenDir(){
  int dd=1;
  TimePos();
  
  float fpid=RNDM(dd);
  Pid=14;
  if(fpid>0.9) Pid=47;

  while (1){
  //  Rigidity=proton->GetRandom(cutoff,10000.);
    Rigidity=proton->GetRandom();
    if(Rigidity>=cutoff) break; 
  }
  Coo[2]=195.;
  Coo[0]=RNDM(dd)*390-195;
  Coo[1]=RNDM(dd)*390-195;
  double phi=RNDM(dd)*2*M_PI;
  double theta=sqrt((double)RNDM(dd));//uniform cos**2
  theta=acos(theta);

  Dir[0]=sin(theta)*cos(phi);
  Dir[1]=sin(theta)*sin(phi);
  Dir[2]=-cos(theta);
  return;
}

void OrbGen::GetPos(uint t, double & Lat, double& Lon){

  double r=6761200.92;
  double omega_iss=2*M_PI*15.60585804/(24*3600);
  double omega_E=2*M_PI/(23.9436*3600);
  double incl=51.6382/180*M_PI;

  
  // Orbit as first approximation is a a
  // circle on a plane rotated on Y axis 
  // passing by GMT by "incl"
  
  double Xt=sin(omega_iss*t) *cos(incl);
  double Yt=cos(omega_iss*t);
  double Zt=sin(omega_iss*t) *sin(incl);
  
  Lat=atan(Zt/sqrt(Xt*Xt+Yt*Yt));
  Lon=atan2(Yt,Xt);
  if(Lon<0) Lon=2*M_PI+Lon;
  Lon=fmod(atan2(Yt,Xt)+omega_E*t,2*M_PI);
  if(Lon>M_PI) Lon-=2*M_PI;
  return;
}


void OrbGen::Print(){
  float r2d=180/atan(1)/4;
  time_t tt=Time.Time_s;
  printf("\n\n%s",ctime(&tt));
  printf("%d\n",Time.Time_ns);
  printf("Lat %f Lon %f  CLat %f CLon %f \n",Lat*r2d,Lon*r2d,CLat*r2d,CLon*r2d);
  printf("Rig %f PID %d \n",Rigidity,Pid);
  printf("X: %f Y %f Z %f Th: %f Phi %f\n",
	 Coo[0],Coo[1],Coo[2],atan2(Dir[2],sqrt(Dir[0]*Dir[0]+Dir[1]*Dir[1]))*r2d,atan2(Dir[1],Dir[0])*r2d);
	 return;
}
