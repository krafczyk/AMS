#include "TrdHRecon.h"

ClassImp(TrdHTrackR)

TrdHTrackR::TrdHTrackR(float Coo_[3],float Dir_[3]):status(0),Chi2(0.),Nhits(0)
{
  float mag=0.;
  for (int i=0;i!=3;i++){
    Coo[i]=Coo_[i];
    Dir[i]=Dir_[i];
    mag+=pow(Dir_[i],2);
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  for (int i=0;i!=3;i++)Dir[i]/=sqrt(mag);
  for(int i=0;i!=2;i++){
    fTrdHSegment[i]=-1;
    segments[i]=0;
  }
};

TrdHTrackR::TrdHTrackR():status(0),Chi2(0.),Nhits(0){
  for(int i=0;i!=3;i++){
    Coo[i]=0.;
    Dir[i]=0.;
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  for(int i=0;i!=2;i++){
    fTrdHSegment[i]=-1;
    segments[i]=0;
  }
  
};


TrdHTrackR::TrdHTrackR(TrdHTrackR *tr){
  Chi2=tr->Chi2;
  Nhits=tr->Nhits;
  status=tr->status;
  for(int i=0;i!=3;i++){
    Coo[i]=tr->Coo[i];
    Dir[i]=tr->Dir[i];
  }
  for(int i=0;i<20;i++)elayer[i]=tr->elayer[i];
  for(int i=0;i!=2;i++){
    fTrdHSegment[i]=tr->fTrdHSegment[i];
    segments[i]=tr->segments[i];
  }
};

int TrdHTrackR::NTrdHSegment() {return 2;}
int TrdHTrackR::nTrdHSegment() {return 2;}
int TrdHTrackR::iTrdHSegment(unsigned int i){return (i<2?fTrdHSegment[i]:-1);}
TrdHSegmentR * TrdHTrackR::pTrdHSegment(unsigned int i){
  TrdHSegmentR* seg=0;
  //  if ( fTrdHSegment[i] >= 0) seg = segments[i];
  if (segments[i] == 0 && fTrdHSegment[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTRDHSegment");
    segments[i] = (TrdHSegmentR*)cont2->getelem(fTrdHSegment[i]);
    delete cont2;
  }

  return segments[i];
}

float TrdHTrackR::Theta(){ return acos(Dir[2]);}

float TrdHTrackR::ex(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i]->d==0)return segments[i]->er;
  }
  return 1.e6;
}

float TrdHTrackR::ey(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i]->d==1)return segments[i]->er;
  }
  return 1.e6;
}

float TrdHTrackR::emx(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i]->d==0)return segments[i]->em;
  }
  return 1.e6;
}

float TrdHTrackR::emy(int debug){
  if(debug)printf("TrdHTrackR::emy - ntrdhsegment %i\n",nTrdHSegment());
  for(int i=0;i!=nTrdHSegment();i++){
    if(debug)printf("segment %i - d %i m %.2f em %.2f\n",i,segments[i]->d,segments[i]->m,segments[i]->em);
    if(segments[i]->d==1)return segments[i]->em;
  }
  return 1.e6;
}

float TrdHTrackR::mx(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(segments[i]->d==0)return segments[i]->m;
    }
    return 1.e6;
  }

  float TrdHTrackR::my(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(segments[i]->d==1)return segments[i]->m;
    }
    return 1.e6;
  }

  float TrdHTrackR::ETheta(int debug){
   float r=sqrt(pow(Dir[0],2)+pow(Dir[1],2));
   if(r<=0.||emx()<=0.||emy()<=0.)return -1.;
   float drdx=Dir[0]/r;
   float drdy=Dir[1]/r;
   float er=sqrt(pow(drdx*emx(),2)+pow(drdy*emy(),2));

   float dfdr=1./(1+pow(r,2));
   if(debug)printf("r %.2f drdx %.2f drdy %.2f er %.2f dfdr %.2f\n",r,drdx,drdy,er,dfdr);
   
   if(isnan(dfdr*er)||isinf(dfdr*er))return -1.;
   return dfdr*er;
 }

 float TrdHTrackR::Phi(){ return atan2(Dir[1],Dir[0]);}
 
 float TrdHTrackR::EPhi(int debug){
   if(fabs(Dir[0])<=1.e-6||pow(Dir[0],2)+pow(Dir[1],2)<=0.)return -1.;

   float dfdx=Dir[1]/(pow(Dir[0],2)+pow(Dir[1],2));
   float dfdy=1./(1+pow(Dir[1]/Dir[0],2))/Dir[0];
   float toReturn=sqrt(pow(dfdx*emx(),2)+pow(dfdy*emy(),2));

   if(debug)printf("dfdx %.2f dfdy %.2f toReturn %.2f \n",dfdx,dfdy,toReturn);
   if(isnan(toReturn)||isinf(toReturn))return -1.;
   return toReturn;
 }

void TrdHTrackR::SetSegments(TrdHSegmentR* segx,TrdHSegmentR* segy){
  if(segx->d+segy->d!=1)return;
  VCon* cont2=GetVCon()->GetCont("AMSTRDHSegment");
  //  if(segx->d==0){
  segments[0]  = segx;
  segments[1]  = segy;
  
  for(int i=0;i<cont2->getnelem();i++){
    if(*segx==*((TrdHSegmentR*)cont2->getelem(i)))fTrdHSegment[0] = i;
    if(*segy==*((TrdHSegmentR*)cont2->getelem(i)))fTrdHSegment[1] = i;
  }
  
  //  fTrdHSegment[segx->d] = cont2->getindex(segx);
  //  fTrdHSegment[segy->d] = cont2->getindex(segy);
  
  Nhits=(int)segx->Nhits+(int)segy->Nhits;
  Chi2=segy->Chi2+segx->Chi2;
}
 
 void TrdHTrackR::setChi2(float Chi2_){Chi2=Chi2_;};
 
 void TrdHTrackR::propagateToZ(float z, float &x , float& y){
   x=Coo[0]+Dir[0]/Dir[2]*(z-Coo[2]);
   y=Coo[1]+Dir[1]/Dir[2]*(z-Coo[2]);
 };

void TrdHTrackR::Print(int opt){
  cout << "AMSTRDHTrack - Info" << endl;
};
 
void TrdHTrackR::clear(){
  for(int i=0;i<2;i++){
    delete segments[i];
    segments[i]=0;//delete fTrdHSegment[i];
    fTrdHSegment[i]=-1;
  }
}
