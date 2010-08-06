#include <cstdlib>
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
  for (int i=0;i!=3;i++)Dir[i]/=sqrt(mag);
};

TrdHTrackR::TrdHTrackR():status(0),Chi2(0.),Nhits(0){
  for(int i=0;i!=3;i++){
    Coo[i]=0.;
    Dir[i]=0.;
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
  for(int i=0;i!=2;i++)
    fTrdHSegment.push_back(tr->fTrdHSegment[i]);
};

int TrdHTrackR::NTrdHSegment() {return fTrdHSegment.size();}
int TrdHTrackR::nTrdHSegment() {return fTrdHSegment.size();}
int TrdHTrackR::iTrdHSegment(unsigned int i){return (i<fTrdHSegment.size())?fTrdHSegment[i]:-1;}
TrdHSegmentR * TrdHTrackR::pTrdHSegment(unsigned int i){return (i<TrdHReconR::getInstance()->nhsegvec)?TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]:0;}


float TrdHTrackR::Theta(){ return acos(Dir[2]);}

float TrdHTrackR::ex(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==0)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->er;
  }
  return 1.e6;
}

float TrdHTrackR::ey(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==1)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->er;
  }
  return 1.e6;
}

float TrdHTrackR::emx(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==0)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->em;
  }
  return 1.e6;
}

float TrdHTrackR::emy(int debug){
  if(debug)printf("TrdHTrackR::emy - ntrdhsegment %i\n",nTrdHSegment());
  for(int i=0;i!=nTrdHSegment();i++){
    if(debug)printf("segment %i - d %i m %.2f em %.2f\n",i,TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d,TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->m,TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->em);
    if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==1)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->em;
  }
  return 1.e6;
}

float TrdHTrackR::mx(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==0)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->m;
    }
    return 1.e6;
  }

  float TrdHTrackR::my(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->d==1)return TrdHReconR::getInstance()->hsegvec[iTrdHSegment(i)]->m;
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

 void TrdHTrackR::SetSegment(TrdHSegmentR* segx, TrdHSegmentR* segy){
   int foundx=0,foundy=0;
   for(int i=0;i!=TrdHReconR::getInstance()->nhsegvec;i++)if(TrdHReconR::getInstance()->hsegvec[i]==segx){fTrdHSegment.push_back(i);foundx=1;}
   for(int i=0;i!=TrdHReconR::getInstance()->nhsegvec;i++)if(TrdHReconR::getInstance()->hsegvec[i]==segy){fTrdHSegment.push_back(i);foundy=1;}
   if(foundx!=1||foundy!=1){
     printf("segment found? x %i y %i\n",foundx,foundy);
     exit(1);
   }

   Nhits=(int)TrdHReconR::getInstance()->hsegvec[iTrdHSegment(0)]->Nhits+(int)TrdHReconR::getInstance()->hsegvec[iTrdHSegment(1)]->Nhits;
   Chi2=TrdHReconR::getInstance()->hsegvec[iTrdHSegment(0)]->Chi2+TrdHReconR::getInstance()->hsegvec[iTrdHSegment(1)]->Chi2;
 }
 
 void TrdHTrackR::setChi2(float Chi2_){Chi2=Chi2_;};
 
 void TrdHTrackR::propagateToZ(float z, float &x , float& y){
   x=Coo[0]+Dir[0]/Dir[2]*(z-Coo[2]);
   y=Coo[1]+Dir[1]/Dir[2]*(z-Coo[2]);
 };

 void TrdHTrackR::Print(){
   cout << "AMSTRDHTrack - Info" << endl;
 };
 

integer TrdHTrackR::build(int rerun){
  int debug=0; 
  //  TrdHReconR::getInstance()->htrvec.clear();
  TrdHReconR::getInstance()->nhtrvec=0;//.clear();

  if(debug)printf("Entering TrdHTrackR::build - %i segments\n",TrdHReconR::getInstance()->nhsegvec);
  
  int nhseg=TrdHReconR::getInstance()->nhsegvec;
  if(nhseg==2||nhseg==3){
    int segiter_sel[2]={-1,-1};
    
    for(int s=0;s!=nhseg;s++){
      int d=TrdHReconR::getInstance()->hsegvec[s]->d;
      float chi2ndofs=1.e6;
      if(TrdHReconR::getInstance()->hsegvec[s]->Nhits>2)
	chi2ndofs=(float) TrdHReconR::getInstance()->hsegvec[s]->Chi2 / (float) TrdHReconR::getInstance()->hsegvec[s]->Nhits;

      if(segiter_sel[d]>-1){
	float chi2ndof=1.e6;
	if(TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Nhits>2)
	  chi2ndof=(float) TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Chi2 / (float) TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Nhits;
	
	if(debug)printf("cand s %.2f < %.2f ?\n",chi2ndof,chi2ndofs);
	if( ( TrdHReconR::getInstance()->hsegvec[s]->Nhits >  TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Nhits) ||
	    ((TrdHReconR::getInstance()->hsegvec[s]->Nhits == TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Nhits) &&
	     (chi2ndof < chi2ndofs))) segiter_sel[d]=s;
      }
      else if(TrdHReconR::getInstance()->hsegvec[s]->Nhits>0)segiter_sel[d]=s;
      
      if(debug)printf("sel %i d %i Nhits %i chi2ndof %.2f\n",s,d,TrdHReconR::getInstance()->hsegvec[segiter_sel[d]]->Nhits,chi2ndofs);
    }

    if(debug) printf("segiter %i %i\n",segiter_sel[0],segiter_sel[1]);
    if(segiter_sel[0]>-1&&segiter_sel[1]>-1){
      TrdHTrackR *tr=TrdHReconR::getInstance()->SegToTrack(TrdHReconR::getInstance()->hsegvec[segiter_sel[0]],TrdHReconR::getInstance()->hsegvec[segiter_sel[1]]);
      tr->status=1;


      if(TrdHReconR::getInstance()->nhtrvec<20)TrdHReconR::getInstance()->htrvec[TrdHReconR::getInstance()->nhtrvec++]=tr;
      else delete tr;
    }
  }
  else if( nhseg>2) TrdHReconR::getInstance()->combine_segments();

  if(debug)printf("TrdHTrackR::build tracks %i\n",TrdHReconR::getInstance()->nhtrvec);
  
  return TrdHReconR::getInstance()->nhtrvec;
}
