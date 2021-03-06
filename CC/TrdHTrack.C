#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
ClassImp(TrdHTrackR)

  TrdHTrackR::TrdHTrackR(float Coo_[3],float Dir_[3]):Chi2(0.),Nhits(0),elikelihood(0.),status(0)
{
  float mag=0.;
  for (int i=0;i!=3;i++){
    Coo[i]=Coo_[i];
    Dir[i]=Dir_[i];
    mag+=pow(Dir_[i],2);
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  for(int i=0;i!=3;i++)Dir[i]/=sqrt(mag);
  for(int i=0;i<10;i++){Charge[i]=-1;ChargeP[i]=0.;}
  clear();
};


TrdHTrackR::TrdHTrackR():Chi2(0.),Nhits(0),elikelihood(0.),status(0){
  for(int i=0;i!=3;i++){
    Coo[i]=0.;
    Dir[i]=0.;
  }
  for(int i=0;i<20;i++)elayer[i]=0.;
  for(int i=0;i<10;i++){Charge[i]=-1;ChargeP[i]=0.;}
  clear();
};


TrdHTrackR::TrdHTrackR(TrdHTrackR *tr){
  Chi2=tr->Chi2;
  Nhits=tr->Nhits;
  status=tr->status;
  elikelihood=tr->elikelihood;
  for(int i=0;i!=3;i++){
    Coo[i]=tr->Coo[i];
    Dir[i]=tr->Dir[i];
  }
  for(int i=0;i<20;i++)elayer[i]=tr->elayer[i];
  for(int i=0;i<10;i++){Charge[i]=tr->Charge[i];ChargeP[i]=tr->ChargeP[i];}

  clear();
  fTrdHSegment=tr->fTrdHSegment;
  segments=tr->segments;
};

int TrdHTrackR::NTrdHSegment() {return fTrdHSegment.size();;}
int TrdHTrackR::nTrdHSegment() {return fTrdHSegment.size();}
int TrdHTrackR::iTrdHSegment(unsigned int i){return (int(i)<nTrdHSegment()?fTrdHSegment[i]:-1);}

TrdHSegmentR * TrdHTrackR::pTrdHSegment(unsigned int i){
  if(segments.size()<=i&&i<2){
    segments.clear();
    VCon* cont2=GetVCon()->GetCont("AMSTRDHSegment");
    for(int s=0;s<2;s++)
      segments.push_back(*(TrdHSegmentR*)cont2->getelem(fTrdHSegment[s]));
    delete cont2;
  }
  
  if(i<segments.size())return &segments.at(i);
  return 0;
}

float TrdHTrackR::Theta(){ return acos(Dir[2]);}

float TrdHTrackR::ex(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(pTrdHSegment(i)->d==0)return pTrdHSegment(i)->er;
  }
  return 1.e6;
}

float TrdHTrackR::ey(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(pTrdHSegment(i)->d==1)return pTrdHSegment(i)->er;
  }
  return 1.e6;
}

float TrdHTrackR::emx(){
  for(int i=0;i!=nTrdHSegment();i++){
    if(pTrdHSegment(i)->d==0)return pTrdHSegment(i)->em;
  }
  return 1.e6;
}

float TrdHTrackR::emy(int debug){
  if(debug)cout<<"TrdHTrackR::emy-I-ntrdhsegment "<<nTrdHSegment()<<endl;;
  for(int i=0;i!=nTrdHSegment();i++){
    if(debug)
      //      printf("segment %i - d %i m %.2f em %.2f\n",i,pTrdHSegment(i)->d,pTrdHSegment(i)->m,pTrdHSegment(i)->em);
    cout<<"TrdHTrackR::emy-I-segment "<<i<<" - d "<<pTrdHSegment(i)->d<<" m "<<pTrdHSegment(i)->m<<" em "<<pTrdHSegment(i)->em<<endl;
    if(pTrdHSegment(i)->d==1)return pTrdHSegment(i)->em;
  }
  return 1.e6;
}

float TrdHTrackR::mx(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(pTrdHSegment(i)->d==0)return pTrdHSegment(i)->m;
    }
    return 1.e6;
  }

  float TrdHTrackR::my(){
    for(int i=0;i!=nTrdHSegment();i++){
      if(pTrdHSegment(i)->d==1)return pTrdHSegment(i)->m;
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
   if(debug)
     cout<<"TrdHTrackR::ETheta-I-r "<<r<<" drdx "<<drdx<<" drdy "<<drdy<<" er "<<er<<" dfdr "<<dfdr<<endl;
   //       printf("r %.2f drdx %.2f drdy %.2f er %.2f dfdr %.2f\n",r,drdx,drdy,er,dfdr);
   
   if(isnan(dfdr*er)||std::isinf(dfdr*er))return -1.;
   return dfdr*er;
 }

 float TrdHTrackR::Phi(){ return atan2(Dir[1],Dir[0]);}
 
 float TrdHTrackR::EPhi(int debug){
   if(fabs(Dir[0])<=1.e-6||pow(Dir[0],2)+pow(Dir[1],2)<=0.)return -1.;

   float dfdx=Dir[1]/(pow(Dir[0],2)+pow(Dir[1],2));
   float dfdy=1./(1+pow(Dir[1]/Dir[0],2))/Dir[0];
   float toReturn=sqrt(pow(dfdx*emx(),2)+pow(dfdy*emy(),2));

   if(debug)//printf("dfdx %.2f dfdy %.2f toReturn %.2f \n",dfdx,dfdy,toReturn);
     cout<<"TrdHTrackR::EPhi-I-dfdx "<<dfdx<<" dfdy "<<dfdy<<" ephi "<<toReturn<<endl;
   
   if(isnan(toReturn)||std::isinf(toReturn))return -1.;
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
  sprintf(_Info,"TrdHTrack Coo=(%5.2f,%5.2f,%5.2f)#pm((%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g NHits=%d chg %i elik %.2f",Coo[0],Coo[1],Coo[2],ex(),ey(),0.,Theta(),Phi(),Chi2,Nhits,(int)Charge[0],elikelihood);
  cout<<_Info<<endl;;
  return;
};
 
void TrdHTrackR::clear(){
  fTrdHSegment.clear();
  segments.clear();
}

float TrdHTrackR::TubePath(TrdRawHitR *hit, int method,int  opt,int debug){
  return TubePath(hit->Layer,hit->Ladder,hit->Tube,method,opt,debug);
}


float TrdHTrackR::TubePath(int layer, int ladder, int tube,int method, int opt,int debug){
  if(method==0){
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
    
    if(d==1)
      dr=fabs(exp_y-x);
    
    if(d==0)
      dr=fabs(exp_x-x);
    
    if(opt==3)return dr;
    
    double dradial=dr;
    //  if(d==0)dradial*=cos(mx());
    //  if(d==1)dradial*=cos(my());
    dradial*=cos(atan(Dir[d]/Dir[2]));
    //  if(d==1)dradial*=cos(atan(Dir[1]/trdir.z()));
    if(opt==2)return dradial;
    if(dradial>tuberad)return -1.;
    
    double path2d=2*sqrt(pow(tuberad,2)-pow(dradial,2));
    if(opt==1)return path2d;
    
    // 3d path lentgh
    double path3d=path2d;
    AMSDir wire(d,1-d,0);
    AMSDir track(Dir[0],Dir[1],Dir[2]);
    path3d/=sin(acos(wire.prod(track)/wire.norm()/track.norm()));

    if(debug)//printf("planar %.2f dradial %.2f path2d %.2f path3d %.2f\n",dr,dradial,path2d,path3d);
      cout<<"TrdHTrackR::TubePath-I-planar "<<dr<<" dradial "<<dradial<<" path2d "<<path2d<<" path3d "<<path3d<<endl;
    return  path3d;
  }
  else if (method==1){
    return GetPathLengthMH(layer,ladder,tube,opt);
  }
  else{
    cerr<<"TrdHTrackR::TubePath-E-unknown path length calculation method "<< method<<endl;
  }
  return -2;
}

// returns pathlength in tube if hit
float TrdHTrackR::GetPathLengthMH(int layer, int ladder, int tube, int i){           
    double radius=0.3;
    if(layer>19||layer<0)return -1.;
    int dir=-1;
    if((layer>=16)||(layer<=3)) dir = 1;
    else                        dir = 0;
    
    if(layer<12) ladder++;
    if(layer< 4) ladder++;
    
    double z = 85.275 + 2.9*(double)layer;
    if(ladder%2==0) z += 1.45;
    
    float exp_x=0.,exp_y=0.;
    propagateToZ(z,exp_x,exp_y);
    
    double x = 10.1*(double)(ladder-9);
    
    if((dir==1) && (ladder>=12)) x+=0.78;
    if((dir==1) && (ladder<= 5)) x-=0.78;
    
    x += 0.31 + 0.62*(double)tube;
    if(tube >=  1) x+=0.03;
    if(tube >=  4) x+=0.03;
    if(tube >=  7) x+=0.03;
    if(tube >=  9) x+=0.03;
    if(tube >= 12) x+=0.03;
    if(tube >= 15) x+=0.03;
    
    double r=x;
    
    double const Pi=4*atan(1);
    AMSPoint tr_pos(Coo[0],Coo[1],Coo[2]);
    AMSDir tr_dir(Dir[0],Dir[1],Dir[2]);
    AMSPoint pos0=tr_pos+tr_dir*(z-tr_pos.z())/tr_dir.z();
    
    AMSPoint cross[2];
    double W=0, p=0, q=0, x1=0, y1=0, z1=0, x2=0, y2=0, z2=0;
    
    if(dir==0){
      W= 1/(tan(tr_dir.gettheta())*cos(tr_dir.getphi()));
      p= ((2*(double)r)+(2*W*W*tr_pos.x())-(2*W*tr_pos.z())+(2*W*(double)z))/(0-1-(W*W));
      q= ((radius*radius)-((double)r*(double)r)-(tr_pos.z()*tr_pos.z())-((double)z*(double)z)+(2*tr_pos.z()*(double)z)-(2*W*(double)z*tr_pos.x())+(2*W*tr_pos.x(
)*tr_pos.z())-(W*W*tr_pos.x()*tr_pos.x()))/(0-1-(W*W));
      
      if(fabs((Pi/2)-fabs(tr_dir.getphi()))<0.000001) {
	x1= pos0.x();
	x2= pos0.x();
      }
      else{
	x1=0-(p/2.)+sqrt((p*p/4.)-q);
	x2=0-(p/2.)-sqrt((p*p/4.)-q);
    }
    y1=pos0.y()+tan(tr_dir.getphi())*(x1-pos0.x());
    y2=pos0.y()+tan(tr_dir.getphi())*(x2-pos0.x());
    
    if(fabs(pos0.x()-(double)r)<(double)radius){
      if(((tr_dir.getphi()<Pi/2 && tr_dir.getphi()>-Pi/2 ) &&
	  tr_dir.gettheta()>Pi/2) || ((tr_dir.getphi()>Pi/2 || tr_dir.getphi()<-Pi/2 )&&
				   tr_dir.gettheta()<Pi/2) ){
	z1=sqrt(((double)radius*(double)radius)-((x1-(double)r)*(x1-(double)r)))+(double)z;
	z2=0-sqrt(((double)radius*(double)radius)-((x2-(double)r)*(x2-(double)r)))+(double)z;
      }
      else {
	z1=0-sqrt(((double)radius*(double)radius)-((x1-(double)r)*(x1-(double)r)))+(double)z;
	z2=sqrt(((double)radius*(double)radius)-((x2-(double)r)*(x2-(double)r)))+(double)z;
      }
    }
    else if ((pos0.x()-(double)r > (double)radius && tr_dir.gettheta()>Pi/2
	      && (tr_dir.getphi()>Pi/2 || tr_dir.getphi()<-Pi/2)) ||
	     (pos0.x()>(double)radius && tr_dir.gettheta()<Pi/2 && (tr_dir.getphi()<Pi/2 && tr_dir.getphi()>-Pi/2)) ||
	     (pos0.x()-(double)r <0-(double)radius && tr_dir.gettheta()<Pi/2 &&
	      (tr_dir.getphi()>Pi/2 || tr_dir.getphi()<-Pi/2)) || (pos0.x()<0-(double)radius && tr_dir.gettheta()>Pi/2 &&
							     (tr_dir.getphi()<Pi/2 && tr_dir.getphi()>-Pi/2))){
      
      z1=sqrt(((double)radius*(double)radius)-((x1-(double)r)*(x1-(double)r)))+(double)z;
      
      z2=sqrt(((double)radius*(double)radius)-((x2-(double)r)*(x2-(double)r)))+(double)z;
    }
    else if  ((pos0.x()-(double)r <0-(double)radius &&
	       tr_dir.gettheta()>Pi/2 && (tr_dir.getphi()>Pi/2 || tr_dir.getphi()<-Pi/2)) ||
	      (pos0.x()<0-(double)radius && tr_dir.gettheta()<Pi/2 && (tr_dir.getphi()<Pi/2 && tr_dir.getphi()>-Pi/2)) ||
	      (pos0.x()-(double)r >(double)radius && tr_dir.gettheta()>Pi/2 &&
	       (tr_dir.getphi()<Pi/2 && tr_dir.getphi()>-Pi/2)) || (pos0.x()-(double)r >(double)radius &&
							     tr_dir.gettheta()<Pi/2 && (tr_dir.getphi()>Pi/2 || tr_dir.getphi()<-Pi/2))){
      
      z1=0-sqrt(((double)radius*(double)radius)-((x1-(double)r)*(x1-(double)r)))+(double)z;
      
      z2=0-sqrt(((double)radius*(double)radius)-((x2-(double)r)*(x2-(double)r)))+(double)z;
    }
    cross[0].setp(x1, y1, z1);
    cross[1].setp(x2, y2, z2);
    
  }

  else if(dir==1){

    W= 1/(tan(tr_dir.gettheta())*sin(tr_dir.getphi()));
    p= ((2*(double)r)+(2*W*W*tr_pos.y())-(2*W*tr_pos.z())+(2*W*(double)z))/(0-1-(W*W));
    q= (((double)radius*(double)radius)-((double)r*(double)r)-(tr_pos.z()*tr_pos.z())-((double)z*(double)z)+(2*tr_pos.z()*(double)z)-(2*W*(double)z*tr_pos.y())+(2*W*tr_pos.y()*tr_pos.z())-(W*W*tr_pos.y()*tr_pos.y()))/(0-1-(W*W));

    if((Pi-fabs(tr_dir.getphi()))<0.000001) {
      y1= pos0.y();
      y2= pos0.y();
    }
    else{
      if(((p*p/4.)-q)>=0){
	y1=0-(p/2.)+sqrt((p*p/4.)-q);
	y2=0-(p/2.)-sqrt((p*p/4.)-q);
      }
      else if( i==0) return 0.6;
      else if( i==1 || i==2) return 0.;
    }
    x1=pos0.x()+(y1-pos0.y())/tan(tr_dir.getphi());
    x2=pos0.x()+(y2-pos0.y())/tan(tr_dir.getphi());
    
    if(fabs(pos0.y()-(double)r)<(double)radius){
      if((tr_dir.getphi()>0 && tr_dir.gettheta()>Pi/2) || (tr_dir.getphi()<0 && tr_dir.gettheta()<Pi/2) ){
	z1=sqrt(((double)radius*(double)radius)-((y1-(double)r)*(y1-(double)r)))+(double)z;
	z2=0-sqrt(((double)radius*(double)radius)-((y2-(double)r)*(y2-(double)r)))+(double)z;
      }
      else {
	z1=0-sqrt(((double)radius*(double)radius)-((y1-(double)r)*(y1-(double)r)))+(double)z;
	z2=sqrt(((double)radius*(double)radius)-((y2-(double)r)*(y2-(double)r)))+(double)z;
      }
    }

    else if( (pos0.y()-(double)r >(double)radius && tr_dir.gettheta()>Pi/2 && tr_dir.getphi()<0) || (pos0.y()-(double)r >(double)radius && tr_dir.gettheta()<Pi/2 && tr_dir.getphi()>0) || (pos0.y()-(double)r<0-(double)radius && tr_dir.gettheta()>Pi/2 &&tr_dir.getphi()>0) || (pos0.y()-(double)r<0-(double)radius && tr_dir.gettheta()<Pi/2 &&tr_dir.getphi()<0)){
      z1=sqrt(((double)radius*(double)radius)-((y1-(double)r)*(y1-(double)r)))+(double)z;
      z2=sqrt(((double)radius*(double)radius)-((y2-(double)r)*(y2-(double)r)))+(double)z;
    }
    else if( (pos0.y()-(double)r <0-(double)radius && tr_dir.gettheta()>Pi/2 && tr_dir.getphi()<0) || (pos0.y()-(double)r <0-(double)radius && tr_dir.gettheta()<Pi/2 && tr_dir.getphi()>0) || (pos0.y()-(double)r>(double)radius && tr_dir.gettheta()>Pi/2 &&tr_dir.getphi()>0) || (pos0.y()-(double)r>(double)radius && tr_dir.gettheta()<Pi/2 &&tr_dir.getphi()<0) ){
      z1=0-sqrt(((double)radius*(double)radius)-((y1-(double)r)*(y1-(double)r)))+(double)z;
      z2=0-sqrt(((double)radius*(double)radius)-((y2-(double)r)*(y2-(double)r)))+(double)z;
    }

    cross[0].setp(x1, y1, z1);
    cross[1].setp(x2, y2, z2);
    
  }
  
  float l= sqrt((cross[0].x()-cross[1].x())*(cross[0].x()-cross[1].x())+
		(cross[0].y()-cross[1].y())*(cross[0].y()-cross[1].y())+(cross[0].z()-cross[1].z())*(cross[0].z()-cross[1].z()));
  float lx= sqrt((cross[0].x()-cross[1].x())*(cross[0].x()-cross[1].x())+(cross[0].z()-cross[1].z())*(cross[0].z()-cross[1].z()));
  float ly= sqrt((cross[0].y()-cross[1].y())*(cross[0].y()-cross[1].y())+(cross[0].z()-cross[1].z())*(cross[0].z()-cross[1].z()));

  if(i==0 && l>0) return l;
  else if(i==0) return 0.6;
  if(i==1 && lx>0) return lx;
  else if (i==1) return 0.;
  if(i==2 && ly>0) return ly;
  else if (i==2) return 0.;
  else return 0.;
}

int TrdHTrackR::UpdateLayerEdep(int corr, float bg,int pid, int debug){
  for(int i=0;i<20;i++)elayer[i]=0.;
  for(int s=0;s<2;s++)
    for(int n=0;n<pTrdHSegment(s)->nTrdRawHit();n++){
      TrdRawHitR* hit=pTrdHSegment(s)->pTrdRawHit(n);
      float hitamp=hit->Amp;
      
      if(hitamp<15)continue;
      
      if(debug)cout<<"TrdHTrackR::UpdateLayerEdep-I-hit energy deposition "<< hitamp<<" ADC"; 
      if(corr&(1<<0)){
	float gaincorr=TrdHCalibR::gethead()->GetGainCorr(hit,2);
	hitamp*=gaincorr;
	if(debug)cout<<" - gain corr "<< gaincorr;
      }
      if(corr&(1<<1)){
	float path=TubePath(hit,0,0,debug);
	if(path<0.1)continue;
	float amp_scaled=hitamp/pow(float(pid==0?1:pid),2);
	float pcorr=1.;
	if(pid<3)pcorr=TrdHCalibR::gethead()->GetPathCorr(path*10.,amp_scaled,3);
	hitamp*=pcorr;
	if(debug)cout<<" - path corr "<< pcorr;
      }
      if(corr&(1<<2)&&bg>0){
	float bgcorr=1.;
	if(pid==1)bgcorr=TrdHCalibR::gethead()->GetBetaGammaCorr(bg,hitamp);
	if(pid>1)bgcorr=TrdHCalibR::gethead()->bgcorr_helium(log10(bg));
	hitamp*=bgcorr;
	if(debug)cout<<" - bg corr "<< bgcorr;
      }
      
      hitamp*=1.2;//correct MPV of 8 GeV protons to 60 ADC

      if(debug)cout<<" - corrected edep: "<<hitamp<<" ADC"<<endl;
      elayer[hit->Layer]+=hitamp;
    }
  
  if(debug>1)
    for(int l=0;l<20;l++)
      cout<<"TrdHTrackR::UpdateLayerEdep-I-Layer Edep: CHG "<<pid<<" L"<<l<<" EDEP "<<elayer[l]<<" ADC"<<endl;
  
  return 0;
}

double TrdHTrackR::GetProb(int charge, int debug){
  for(int i=0;i<10;i++)
    if(Charge[i]==charge)return ChargeP[i];
  
  if(debug)cout<<"TrdHTrackR::GetProb-W-charge "<<charge<<" not found"<<endl;
  return 0.;
}

float TrdHTrackR::GetLikelihood(int sig,int bkg, int debug){
  if(debug)
    cout<<"TrdHTrackR::GetLikelihood-I-signal "<<sig<<" background "<<bkg<<endl;
  
  int ncharges=0;
  for(int i=0;i<10;i++)if(Charge[i]!=-1)ncharges++;
  if(ncharges<2){
    if(TrdHChargeR::gethead()->pdfs.size()>0)
      cerr<<"TrdHTrackR::GetLikelihood-E-PDFs found but charge probabilities not calculated"<<endl;
    return -1.;
  }
  
  double siglik=0.;
  double bkglik=0.;
  if(debug)cout<<"TrdHTrackR::GetLikelihood-I-probabilities size "<<ncharges<<endl;;
  for(int i=0;i<10;i++){
    if(debug)cout<<"TrdHTrackR::GetLikelihood-I-chg "<<Charge[i]<<" : prob "<<ChargeP[i]<<endl;;
    if(Charge[i]==sig) siglik=ChargeP[i];
    else if(bkg==0)   bkglik+=ChargeP[i];
    else if(bkg & (1<<(Charge[i]%10)))
      bkglik+=ChargeP[i];
  }
  if(siglik==0.||bkglik==0.)
    cerr<<"TrdHTrackR::GetLikelihood-E-probabilities zero - signal "<<siglik<<" background "<<bkglik<<endl;
  
  if(debug)cout<<"TrdHTrackR::GetLikelihood-I-siglik "<<siglik<<" bkglik "<<bkglik<<" - loglik "<<-log(siglik/(siglik+bkglik))<<endl;
  return -log(siglik/(siglik+bkglik));
}

int TrdHTrackR::GetNCC(float cut,int debug){
  int n=0;
  for(int l=0;l<20;l++)
    if(elayer[l]>cut)n++;
  
  if(debug)cout<<"TrdHTrackR::GetNCC-I-Number of layers above cut value "<<cut<<" : "<<n<<endl;
  return n;
}

