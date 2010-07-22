#include "TrdHRecon.h"

ClassImp(TrdHSegmentR)

int TrdHSegmentR::NTrdRawHit(){return Nhits;};
int TrdHSegmentR::nTrdRawHit(){return Nhits;};
int TrdHSegmentR::iTrdRawHit(unsigned int i){return i<Nhits?fTrdRawHit[i]:-1;};
TrdRawHitR *TrdHSegmentR::pTrdRawHit(unsigned int i){ return ( i<TrdHReconR::getInstance()->nrhits)?TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]:0;};

TrdHSegmentR::TrdHSegmentR():d(-1),m(0.),r(0.),z(0.),w(0.),em(0.),er(0.),Nhits(0.),Chi2(0.){};

TrdHSegmentR::TrdHSegmentR(int d_, float m_, float em_, float r_, float er_,float z_, float w_)
  : d(d_), m(m_), em(em_), r(r_), er(er_), z(z_), w(w_), Nhits(0), Chi2(0.) 
{};
  
TrdHSegmentR::TrdHSegmentR(int d_, float m_, float em_, float r_, float er_, float z_, float w_, int Nhits_, TrdRawHitR* pthit[])
  : d(d_), m(m_), em(em_), r(r_), er(er_), z(z_) , w(w_), Nhits(Nhits_)
{
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
  //  Chi2=seg->Chi2;
  //  fTrdRawHit.clear();
  for(int i=0;i<seg->Nhits;i++)
    fTrdRawHit[i]=seg->fTrdRawHit[i];
  
  calChi2();
};

void TrdHSegmentR::SetHits(int Nhits_, TrdRawHitR* pthit[]){
  Nhits=0;
  for(int i=0;i!=Nhits_;i++){
    for(int j=0;j!=TrdHReconR::getInstance()->nrhits;j++)
      if(pthit[i]&&pthit[i]==TrdHReconR::getInstance()->rhits[j])fTrdRawHit[Nhits++]=j;
  }
}
  
float TrdHSegmentR::resid(float r_, float z_, int d_){
  if(d_!=d)return -1;
  return (r_ - (r + m*(z_-z)))*cos(atan(m));
};
  
void TrdHSegmentR::calChi2(){
  Chi2=0.;
  for(int i=0;i!=Nhits;i++){
    if(fTrdRawHit[i]>TrdHReconR::getInstance()->nrhits)exit(1);

    TRDHitRZD rzd=TRDHitRZD(*TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]);
    Chi2+=pow(resid(rzd.r,rzd.z,rzd.d)/ (0.62/sqrt(12.)),2);

  }
}
  
   
void TrdHSegmentR::Print(int level){
  printf("TrdHSegmentR - Info\n");
  printf("TrdHSegmentR d %i m %f r %f z %f w %f Nhits %i Chi2 %f\n", d, m,r,z,w,Nhits,Chi2);
  if(level>0){
    for(int i=0;i!=Nhits;i++){
      printf("  RawHit %i LLT %i %i %i\n",i,TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]->Layer,TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]->Ladder,TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]->Tube);
    }
  }
};
     
// fit  Hit.rz   with  r = A*z + B
int TrdHSegmentR::LinReg(int debug)
{ 
  if(debug>0)printf("Entering LinReg - %i RawHits\n",Nhits);
  double WSr=0.,WSz=0.,WSrr=0.,WSzz=0.,WSrz=0.,WS=0.,W=1.;
  if(Nhits<3)return -1;
    
  for(int i=0; i<Nhits; i++){
    TRDHitRZD rzdi=TRDHitRZD(*TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]);
    int nz=1;
    float ri=rzdi.r;
    float zi=rzdi.z;
      
    for(int j=i+1; j<Nhits; j++){
      TRDHitRZD rzdj=TRDHitRZD(*TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]);
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
      for(int j=0; j<Nhits; j++){
	TRDHitRZD rzd=TRDHitRZD(*TrdHReconR::getInstance()->rhits[fTrdRawHit[i]]);
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


TrdHSegmentR* TrdHSegmentR::Refit(int debug){
  if(debug>0)printf("Enter Refit - hit size %i\n",Nhits);
  int lr=LinReg(debug);
  
  int lay[20];for(int l=0;l!=20;l++)lay[l]=0;
  float sum=0.;
  float zmax=0.,zmin=200.;
  int nhits=0;
  for(int h=0;h!=Nhits;h++){	
    TRDHitRZD rzd=TRDHitRZD(*TrdHReconR::getInstance()->rhits[fTrdRawHit[h]]);
    //  for(vector<int>::iterator i=fTrdRawHit.begin();i<fTrdRawHit.end();i++){
    //    TRDHitRZD rzd=TRDHitRZD(TrdHReconR::getInstance()->TrdHReconR::getInstance()->rhits[*i]);


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
    Nhits=nhits;
    calChi2();
    return this;
  }
  else return 0;
}

integer TrdHSegmentR::build(int rerun){
  // This routine does the TRD 'H' Segment fitting based on TRDRawHits
  // It reconstructs segments separately for the two TRD projections:
  // in XZ plane (d=0) from TRD layers 5-16 
  // in YZ plane (d=1) from TRD layers 1-4 and 17-20
  int debug=0;
  
  if(debug)printf("Entering TrdHSegmentR::build\n");
  if(debug)printf("trdhrecon hit size %i\n",TrdHReconR::getInstance()->nrhits);
  TrdHReconR::getInstance()->nhsegvec=0;//clear();
  TrdHReconR::getInstance()->npeak[0]=0;//clear();
  TrdHReconR::getInstance()->npeak[1]=0;//clear();
  
  // skip arrays for now
  int nhcut=1.e6; //  below 40 nTRDRawHits: vector-histo 
  // above                 array-histo
  
  double Hcut=0.0;
  //if(pev->nTRDRawHit()>500) Hcut=30.0;
  
  int nrh=TrdHReconR::getInstance()->nrhits;
  if(debug)printf("RawHits %i\n",nrh);
  if(nrh<4||nrh>100) return 0;

  //  if(!TrdHReconR::getInstance()->H2A_mvr[0])TrdHReconR::getInstance()->H2A_mvr[0] = new TH2A ("ha_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0);
  //  if(!TrdHReconR::getInstance()->H2A_mvr[1])TrdHReconR::getInstance()->H2A_mvr[1] = new TH2A ("ha_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0);
		   
  if(!TrdHReconR::getInstance()->H2V_mvr[0])TrdHReconR::getInstance()->H2V_mvr[0]=new TH2V ("hv_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0);
  if(!TrdHReconR::getInstance()->H2V_mvr[1])TrdHReconR::getInstance()->H2V_mvr[1]=new TH2V ("hv_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0);
  
  //  if(nrh>nhcut){
  //    TrdHReconR::getInstance()->H2A_mvr[0]->Reset();
  //    TrdHReconR::getInstance()->H2A_mvr[1]->Reset();
  //  }else{
  TrdHReconR::getInstance()->H2V_mvr[0]->Reset();
  TrdHReconR::getInstance()->H2V_mvr[1]->Reset();
  //  }
  
  int prefit=TrdHReconR::getInstance()->DoPrefit(debug);
  if(debug)printf("TrdHSegmentR 1 NofSegments sum %i = %i?\n",TrdHReconR::getInstance()->npeak[0]+TrdHReconR::getInstance()->npeak[1],prefit);

  if(prefit<=0){
    if(debug)printf("skipping \n");
    return 0;
  }
  //  vector<TrdHSegmentR> segments=
  TrdHReconR::getInstance()->DoLinReg(debug);
  if(debug)printf("TrdHSegmentR 2 NofSegments sum %i = %i?\n",TrdHReconR::getInstance()->npeak[0]+TrdHReconR::getInstance()->npeak[1],prefit);
  
  //  vector<TrdHSegmentR> segments2=
  TrdHReconR::getInstance()->clean_segvec(debug);
  if(debug)printf("TrdHSegmentR 3 NofSegments sum %i = %i?\n",TrdHReconR::getInstance()->npeak[0]+TrdHReconR::getInstance()->npeak[1],prefit);
  
  for(int i=0;i!=TrdHReconR::getInstance()->nhsegvec;i++)
    TrdHReconR::getInstance()->hsegvec[i]->calChi2();
  
  if(debug) printf("returning %i segments\n",TrdHReconR::getInstance()->nhsegvec);

  return TrdHReconR::getInstance()->nhsegvec;
}

