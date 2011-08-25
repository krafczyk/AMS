#include "TrdHRecon.h"
//#include "commonsi.h"
ClassImp(TrdHSegmentR)

#ifdef __MLD__
int TrdRawHitR::num=0;
//int TrdHSegmentR::num=0;
#endif

int TrdHSegmentR::NTrdRawHit(){return fTrdRawHit.size();};
int TrdHSegmentR::nTrdRawHit(){return fTrdRawHit.size();};
int TrdHSegmentR::iTrdRawHit(unsigned int i){return i<nTrdRawHit()?fTrdRawHit[i]:-1;};

TrdRawHitR *TrdHSegmentR::pTrdRawHit(unsigned int i){ 
//  if(AMSCommonsI::getbuildno()<=538){
if(0){
    if(hits.size()<=i&&i<Nhits){
      hits.clear();
      VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
      int n=0;
      for(int i=0;i<cont2->getnelem();i++){
	TRDHitRZD rzd=TRDHitRZD(*(TrdRawHitR*)cont2->getelem(i));
	if(rzd.d!=d)continue;
	
	float dz=rzd.z-z;
	float expos=r+ m*dz;//*cos(atan(seg->m));
	float resid=rzd.r - expos;
	float maxresid=0.6+fabs(dz)* em;
	if(fabs(resid)<maxresid)
	  hits.push_back(*(TrdRawHitR*)cont2->getelem(i));
      }
      delete cont2;
    }
  }
  else{
    if(hits.size()<=i&&i<Nhits){
      hits.clear();
      VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
      for(int i=0;i<cont2->getnelem();i++){
	for(int n=0;n<fTrdRawHit.size();n++){
	  if(i==fTrdRawHit[n])
	    hits.push_back(*(TrdRawHitR*)cont2->getelem(i));
	}
      }
      delete cont2;
    }
  }
  
  if(i<hits.size())return &hits[i];
  return 0;

}

TrdHSegmentR::TrdHSegmentR():d(-1),m(0.),r(0.),z(0.),w(0.),em(0.),er(0.),Nhits(0.),Chi2(0.){
  fTrdRawHit.clear();
  gbhits.clear();
  hits.clear();
}


TrdHSegmentR::TrdHSegmentR(int d_, float m_, float em_, float r_, float er_,float z_, float w_)
  : d(d_), m(m_), em(em_), r(r_), er(er_), z(z_), w(w_), Nhits(0), Chi2(0.) 
{
  fTrdRawHit.clear();
  gbhits.clear();
  hits.clear();
};

TrdHSegmentR::TrdHSegmentR(int d_, float m_, float em_, float r_, float er_, float z_, float w_, int Nhits_, TrdRawHitR* pthit[])
  : d(d_), m(m_), em(em_), r(r_), er(er_), z(z_) , w(w_), Nhits(Nhits_)
{
  fTrdRawHit.clear();
  gbhits.clear();
  hits.clear();
  SetHits(Nhits_,pthit);
};

TrdHSegmentR::TrdHSegmentR(TrdHSegmentR* seg){
  d=seg->d;
  m=seg->m;
  r=seg->r;
  z=seg->z;
  w=seg->w;
  em=seg->em;
  er=seg->er;
  Nhits=seg->Nhits;

  fTrdRawHit=seg->fTrdRawHit;
  hits=seg->hits;
  gbhits=seg->gbhits;
  Chi2=seg->Chi2;
};

void TrdHSegmentR::SetHits(int Nhits_, TrdRawHitR* pthit[]){

  Nhits=0;
  fTrdRawHit.clear();
  hits.clear();
  VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
  for(int i=0;i<Nhits_;i++){
    if(pthit[i]){
      hits.push_back(*pthit[i]);
      fTrdRawHit.push_back(cont2->getindex(pthit[i]));
    }
  }
  delete cont2;
}
  
float TrdHSegmentR::resid(float r_, float z_, int d_){
  if(d_!=d)return -1;
  return (r_ - (r + m*(z_-z)))*cos(atan(m));
};
  
void TrdHSegmentR::calChi2(){
  Chi2=0.;
  for(int i=0;i<nTrdRawHit();i++){
    TRDHitRZD rzd=TRDHitRZD(*pTrdRawHit(i));
    Chi2+=pow(resid(rzd.r,rzd.z,rzd.d)/ (0.62/sqrt(12.)),2);
  }
}
  
   
void TrdHSegmentR::Print(int level){
  printf("TrdHSegmentR - Info\n");
  printf("TrdHSegmentR d %i m %f r %f z %f w %f Nhits %i Chi2 %f\n", d, m,r,z,w,Nhits,Chi2);
  if(level>0){
    for(int i=0;i!=nTrdRawHit();i++){
      printf("  RawHit %i LLT %i %i %i\n",i,pTrdRawHit(i)->Layer,pTrdRawHit(i)->Ladder,pTrdRawHit(i)->Tube);
    }
  }
};
     
// fit  Hit.rz   with  r = A*z + B
int TrdHSegmentR::LinReg(int debug)
{ 
  if(debug>0)printf("Entering LinReg - %i RawHits\n",nTrdRawHit());
  double WSr=0.,WSz=0.,WSrr=0.,WSzz=0.,WSrz=0.,WS=0.,W=1.;
  if(nTrdRawHit()<3)return -1;
    
  for(int i=0; i<nTrdRawHit(); i++){
    TRDHitRZD rzdi=TRDHitRZD(*pTrdRawHit(i));
    int nz=1;
    float ri=rzdi.r;
    float zi=rzdi.z;
      
    for(int j=i+1; j<nTrdRawHit(); j++){
      TRDHitRZD rzdj=TRDHitRZD(*pTrdRawHit(j));
      if(rzdj.z!=zi)continue;
      ri+=rzdj.r;nz++;
    }
      
    TRDHitRZD rzd;
    float sigma=0.;
    if(nz==1){
      rzd.r=ri;rzd.z=zi,rzd.d=rzdi.d;
      sigma=0.6/sqrt(12);
    }
    else if(nz>1){
      float rmean=ri/(float)nz;
      for(int j=0; j<nTrdRawHit(); j++){
	TRDHitRZD rzd=TRDHitRZD(*pTrdRawHit(j));
	if(rzd.z==zi)sigma+=pow(rzd.r-rmean,2);
      }
	
      sigma=sqrt(sigma/((float)nz-1));
      sigma=sqrt(pow(sigma,2)+pow(0.6/sqrt(12),2));
      rzd.r=rmean;rzd.z=zi,rzd.d=rzdi.d;
    }
      
    if(debug>0)printf("rzd %i d %i r %.2f z %.2f sigma %.2f double? %i\n",i,rzd.d,rzd.r,rzd.z,sigma,nz>1);
      
    W = 1./(sigma*sigma);
      
    float dz=(rzd.z-z);
    float dr=rzd.r;
      
    WSr  += W * dr;
    WSz  += W * dz;
    WSrr += W * dr * dr;
    WSzz += W * dz * dz;
    WSrz += W * dr * dz;
    WS   += W;
  }

  if(debug>1)
    printf("WSr %.2f WSz %.2f WSrr %.2f WSZZ %.2f WSrz %.2f WS %.2f\n",WSr,WSz,WSrr,WSzz,WSrz,WS);


  if(WS<=0.){
    if(debug>0)printf("LinReg Sum of weights WS = 0.\n");
    return -1;
  }

  double WMr  = WSr / WS;
  double WMz  = WSz / WS;
  double WMrr = WSrr / WS;
  double WMzz = WSzz / WS;
  double WMrz = WSrz / WS;
 
  if(debug>1)
    printf("WMr %.2f WMz %.2f WMrr %.2f WMZZ %.2f WMrz %.2f \n",WMr,WMz,WMrr,WMzz,WMrz);


  double SDr = 0.0;
  if(WMr*WMr<WMrr)SDr=sqrt(WMrr - WMr * WMr);
  double SDz = 0.0;
  if(WMz*WMz<WMzz)SDz=sqrt(WMzz - WMz * WMz);
    
  if(debug>1)
    printf("SDr %.2e SDz %.2e",SDr,SDz);


  if(SDr == 0.0 || isnan(SDr)){
    r=WMr;m=0.;Chi2=0.;er=0.6/sqrt(12);
    if(debug)printf("problem occured SDr %f -> r %.2f m %.2f Chi2 %.2f er %.2f\n",SDr,r,m,Chi2,er);
  }
  if(SDr*SDz == 0.0 || isnan(SDr*SDz)){
    if(debug>0)printf("ERROR SDr %f SDz %f\n",SDr,SDz);
    return -1;
  }

  double Rrz = (WMrz - WMr * WMz)/SDr/SDz;
  m = Rrz * SDr / SDz; 
  r = WMr - m * WMz;
  em = 1./sqrt(WS)/SDz;
  er = em*sqrt(WMzz);
  Chi2 = WS*SDr*SDr*(1.-Rrz*Rrz);

  if(debug>0)printf("linreg result m %.2e (+/- %.2e ) r %.2e (+/- %.2e)\n",m,em,r,er);
  return 1;
}


int TrdHSegmentR::Refit(int debug){
  if(debug>0)printf("Enter Refit - hit size %i\n",nTrdRawHit());
  int lr=LinReg(debug);
  
  int lay[20];for(int l=0;l!=20;l++)lay[l]=0;
  float sum=0.;
  float zmax=0.,zmin=200.;
  int nhits=0;
  for(int h=0;h!=nTrdRawHit();h++){	
    TRDHitRZD rzd=TRDHitRZD(*pTrdRawHit(h));

    if(rzd.d!=d)continue;
    if(rzd.z>zmax)zmax=rzd.z;
    if(rzd.z<zmin)zmin=rzd.z;
    
    for(int l=0;l!=20;l++){
      if(fabs((rzd.z-85.275)/2.9-(float)l)<0.01)lay[l]++;
      else if (fabs((rzd.z-85.275-1.45)/2.9 -(float)l)<0.01)lay[l]++;
    }
    nhits++;
  }
  
  int lsum=0;
  for(int l=0;l!=20;l++)if(lay[l]>0)lsum++;
  if(debug)printf("linreg stat %i dz %.2f layers %i\n",lr,zmax-zmin,lsum);
  if(lr==1&&zmax-zmin>10.&&lsum>2){
    calChi2();
    return 1;
  }
  else return 0;
}

void TrdHSegmentR::AddHit(TrdRawHitR hit,int iter){
  hits.push_back(hit);
  fTrdRawHit.push_back(iter);
  Nhits=fTrdRawHit.size();
}

void TrdHSegmentR::RemoveHit(int iter){
  if(iter>hits.size())printf("trying to delete hit %i of %i hits\n",iter,(int)hits.size()); if(iter>fTrdRawHit.size())printf("trying to delete hit %i of %i hits\n",iter,(int)fTrdRawHit.size());
  hits.erase(hits.begin()+iter);
  fTrdRawHit.erase(fTrdRawHit.begin()+iter);
  Nhits=fTrdRawHit.size();
}


void TrdHSegmentR::clear(){
  fTrdRawHit.clear();
  hits.clear();
}
