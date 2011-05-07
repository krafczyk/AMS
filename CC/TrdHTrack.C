#include "TrdHRecon.h"

ClassImp(TrdHTrackR)

  TrdHTrackR::TrdHTrackR(float Coo_[3],float Dir_[3]):status(0),Chi2(0.),Nhits(0),charge(0.),elikelihood(0.)
{
  float mag=0.;
  for (int i=0;i!=3;i++){
    Coo[i]=Coo_[i];
    Dir[i]=Dir_[i];
    mag+=pow(Dir_[i],2);
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  for (int i=0;i!=3;i++)Dir[i]/=sqrt(mag);

  clear();
};


TrdHTrackR::TrdHTrackR():status(0),Chi2(0.),Nhits(0),charge(0.),elikelihood(0.){
  for(int i=0;i!=3;i++){
    Coo[i]=0.;
    Dir[i]=0.;
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  clear();
};


TrdHTrackR::TrdHTrackR(TrdHTrackR *tr){
  Chi2=tr->Chi2;
  Nhits=tr->Nhits;
  status=tr->status;
  charge=tr->charge;
  elikelihood=tr->elikelihood;
  for(int i=0;i!=3;i++){
    Coo[i]=tr->Coo[i];
    Dir[i]=tr->Dir[i];
  }
  for(int i=0;i<20;i++)elayer[i]=tr->elayer[i];
  clear();

  for(vector<int>::iterator it=tr->fTrdHSegment.begin();it!=tr->fTrdHSegment.end();it++)
    fTrdHSegment.push_back(*it);
  
  for(vector<TrdHSegmentR>::iterator it=tr->segments.begin();it!=tr->segments.end();it++)
    segments.push_back(*it);
  
};

int TrdHTrackR::NTrdHSegment() {return fTrdHSegment.size();;}
int TrdHTrackR::nTrdHSegment() {return fTrdHSegment.size();}
int TrdHTrackR::iTrdHSegment(unsigned int i){return (i<nTrdHSegment()?fTrdHSegment[i]:-1);}

TrdHSegmentR * TrdHTrackR::pTrdHSegment(unsigned int i){
  if(segments.size()<=i&&i<2){
    segments.clear();
    VCon* cont2=GetVCon()->GetCont("AMSTRDHSegment");
    for(int i=0;i<cont2->getnelem();i++){
      for(int n=0;n<fTrdHSegment.size();n++){
        if(i==fTrdHSegment[n]){
          segments.push_back(*(TrdHSegmentR*)cont2->getelem(i));
	}
      }
    }
    delete cont2;
  }
  
  if(i<segments.size())return &segments.at(i);
  return 0;
}

float TrdHTrackR::Theta(){ return acos(Dir[2]);}

float TrdHTrackR::ex(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i].d==0)return segments[i].er;
  }
  return 1.e6;
}

float TrdHTrackR::ey(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i].d==1)return segments[i].er;
  }
  return 1.e6;
}

float TrdHTrackR::emx(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(segments[i].d==0)return segments[i].em;
  }
  return 1.e6;
}

float TrdHTrackR::emy(int debug){
  if(debug)printf("TrdHTrackR::emy - ntrdhsegment %i\n",nTrdHSegment());
  for(int i=0;i!=nTrdHSegment();i++){
    if(debug)printf("segment %i - d %i m %.2f em %.2f\n",i,segments[i].d,segments[i].m,segments[i].em);
    if(segments[i].d==1)return segments[i].em;
  }
  return 1.e6;
}

float TrdHTrackR::mx(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(segments[i].d==0)return segments[i].m;
    }
    return 1.e6;
  }

  float TrdHTrackR::my(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(segments[i].d==1)return segments[i].m;
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
  segments.clear();
  segments.push_back(segx);
  segments.push_back(segy);
  
  fTrdHSegment.clear();
  for(int i=0;i<cont2->getnelem();i++){
    if(*segx==*((TrdHSegmentR*)cont2->getelem(i)))fTrdHSegment.push_back(i);
    if(*segy==*((TrdHSegmentR*)cont2->getelem(i)))fTrdHSegment.push_back(i);
  }
  
  Nhits=(int)segx->Nhits+(int)segy->Nhits;
  Chi2=segy->Chi2+segx->Chi2; 
  delete cont2;
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
  fTrdHSegment.clear();
  segments.clear();
}

float TrdHTrackR::TubePath(int layer, int ladder, int tube,int opt,int debug){
  double tuberad=0.3;

  double dr=1.e6;
  if(layer>19||layer<0)return -1.;
  int d=-1;
  if((layer>=16)||(layer<=3)) d = 1;
  else                        d = 0;

  if(layer<12) ladder++;
  if(layer< 4) ladder++;

  double z = 85.275 + 2.9*(double)layer;
  if(ladder%2==0) z += 1.45;

  float exp_x=0.,exp_y=0.;
  propagateToZ(z,exp_x,exp_y);

  double x = 10.1*(double)(ladder-9);

  if((d==1) && (ladder>=12)) x+=0.78;
  if((d==1) && (ladder<= 5)) x-=0.78;

  x += 0.31 + 0.62*(double)tube;
  if(tube >=  1) x+=0.03;
  if(tube >=  4) x+=0.03;
  if(tube >=  7) x+=0.03;
  if(tube >=  9) x+=0.03;
  if(tube >= 12) x+=0.03;
  if(tube >= 15) x+=0.03;

  if(d==1&&fabs(exp_y-x)<tuberad)
    dr=fabs(exp_y-x);
  
  if(d==0&&fabs(exp_x-x)<tuberad)
    dr=fabs(exp_x-x);
  
  if(opt==3)return dr;
  
  double dradial=dr;
  //  if(d==0)dradial*=cos(mx());
  //  if(d==1)dradial*=cos(my());
  dradial*=cos(atan(Dir[d]/Dir[2]));
  //  if(d==1)dradial*=cos(atan(Dir[1]/trdir.z()));
  if(dradial>tuberad)return -1.;
  if(opt==2)return dradial;
  
  double path2d=2*sqrt(pow(tuberad,2)-pow(dradial,2));
  if(opt==1)return path2d;
  
  // angle between radial distance and track on plane defined by track and tub\e wire
  double alphaplane=0;
  // "unit" vector perpendicular to tube wire and on plane mentioned above
  double dplane=0.;

  // 3d path lentgh
  double path3d=path2d;
  dplane=(sqrt(pow(Dir[d],2)+pow(Dir[2],2)));
  alphaplane=atan(Dir[1-d]/dplane);
  path3d/=cos(alphaplane);

  if(debug)printf("dplanar %.2f dradial %.2f path2d %.2f \n dplane %.2f alphaplane %.2f path3d %.2f\n",dr,dradial,path2d,dplane,alphaplane,path3d);
  //   if(debug)printf("TubeDist L%i dx %.2f dr %.2f path %.2f (ladder %i tube %i)\n",layer,dr,dr*cos(M_PI-Theta()),path,thisladder,thistube);                       
  return  path3d;
}
