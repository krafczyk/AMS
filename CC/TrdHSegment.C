#include "TrdHRecon.h"

ClassImp(TrdHSegmentR)

int TrdHSegmentR::NTrdRawHit(){return fTrdRawHit.size();};
int TrdHSegmentR::nTrdRawHit(){return fTrdRawHit.size();};
int TrdHSegmentR::iTrdRawHit(unsigned int i){return i<fTrdRawHit.size()?fTrdRawHit[i]:-1;};
TrdRawHitR * TrdHSegmentR::pTrdRawHit(unsigned int i){ return ( i<trdhrecon.rhits.size())?trdhrecon.rhits[fTrdRawHit[i]]:0;};

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
  fTrdRawHit.clear();
  for(int i=0;i!=seg->nTrdRawHit();i++){
    fTrdRawHit.push_back(seg->iTrdRawHit(i));
  }
  calChi2();
};

void TrdHSegmentR::SetHits(int Nhits_, TrdRawHitR* pthit[]){
  for(int i=0;i!=Nhits_;i++){
    for(int j=0;j!=trdhrecon.rhits.size();j++)
      if(pthit[i]&&pthit[i]==trdhrecon.rhits[j])fTrdRawHit[i]=j;
  }
}
  
float TrdHSegmentR::resid(float r_, float z_, int d_){
  if(d_!=d)return -1;
  return (r_ - (r + m*(z_-z)))*cos(atan(m));
};
  
void TrdHSegmentR::calChi2(){
  Chi2=0.;
  for(int i=0;i!=nTrdRawHit();i++){
    TRDHitRZD rzd=TRDHitRZD(pTrdRawHit(i));
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
    TRDHitRZD rzdi=TRDHitRZD(pTrdRawHit(i));
    int nz=1;
    float ri=rzdi.r;
    float zi=rzdi.z;
      
    for(int j=i+1; j<nTrdRawHit(); j++){
      TRDHitRZD rzdj=TRDHitRZD(pTrdRawHit(j));
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
	TRDHitRZD rzd=TRDHitRZD(pTrdRawHit(j));
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

  if(debug>1){
    printf("WSr %.2f WSz %.2f WSrr %.2f WSZZ %.2f WSrz %.2f WS %.2f\n",WSr,WSz,WSrr,WSzz,WSrz,WS);
  }

  if(WS<=0.){
    if(debug>0)printf("LinReg Sum of weights WS = 0.\n");
    return -1;
  }

  double WMr  = WSr / WS;
  double WMz  = WSz / WS;
  double WMrr = WSrr / WS;
  double WMzz = WSzz / WS;
  double WMrz = WSrz / WS;
 
  if(debug>1){
    printf("WMr %.2f WMz %.2f WMrr %.2f WMZZ %.2f WMrz %.2f \n",WMr,WMz,WMrr,WMzz,WMrz);
  }

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
  if(debug>0)printf("Enter Refit - hit size %i\n",nTrdRawHit());
  int lr=LinReg(debug);
  
  int lay[20];for(int l=0;l!=20;l++)lay[l]=0;
  float sum=0.;
  float zmax=0.,zmin=200.;
  int nhits=0;
  for(int h=0;h!=nTrdRawHit();h++){	
    TRDHitRZD rzd=TRDHitRZD(pTrdRawHit(h));
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

  vector<PeakXYZW> TH2V::FindPeaks(int npeak)
  {
    int debug=0;
    vector<BIN> maxima;maxima.clear();
    vector<PeakXYZW> vec;vec.clear();
    if(debug)printf("findpeaks histo size %i\n",histo.size());
    if(histo.size()==0)return vec;

    for( int i=0; i!=histo.size(); i++){
      // suppress number of segments for events lot of hits (hadronic interaction)
      if(histo[i].c<1+log(histo.size()))continue;
      
      // check distance to known maxima - check in 3x3 box around maximum
      int cb=-1;
      for(int b=0;b!=maxima.size();b++)if(fabs(histo[i].x-maxima[b].x)<=1&&fabs(histo[i].y-maxima[b].y)<=1){cb=b;break;}

      // if maximum already existing (cb>-1) check to overwrite else new maximum
      if(cb==-1)maxima.push_back(histo[i]);
      else if(histo[i].c>maxima[cb].c)maxima[cb]=histo[i];
    }

    // check if final maxima are too close to each other (3x3 box again)
    for(int i=0;i!=maxima.size();i++){
      for(int j=i+1;j!=maxima.size();j++){
	if(fabs(maxima[i].x-maxima[j].x)<=1&&fabs(maxima[i].y-maxima[j].y)<=1){
	  if(maxima[j].c<=maxima[i].c)maxima[j].c=0;
	  else maxima[i].c=0;
	}
      }
    }
    
    if(maxima.size()==0)return vec;
    
    // sort maxima in decreasing c (entries) order
    sort(maxima.begin(),maxima.end());
    
    if(debug)printf("found %i maxima:\n",maxima.size());
    if(debug)for(int i=0;i<maxima.size();i++){
      float x=0.,y=0.;
      x=(Xlo+(Xup-Xlo)*((float)histo[i].x+0.5)/(float)nbx);
      y=(Ylo+(Yup-Ylo)*((float)histo[i].y+0.5)/(float)nby);
      printf("max %i s %.2f o %.2f z %.2f c %.2f\n",i,x,y,maxima[i].z,maxima[i].c);
      if(maxima[i].c<=0.)printf("ERROR maximum size <= 0!!\n");
    }
    if(npeak>0)while(maxima.size()>npeak)maxima.pop_back();
    else while(maxima.size()>1&&maxima.back().c<5)maxima.pop_back();
    
    
    float width=1.;
    for(int j=0;j<maxima.size();j++){
      float w=0.0, sx=0.0, sy=0.0, sz=0.0,sw=0.0,zmin=200.0,zmax=0.0,sc=0.0;
      for( int i=0; i<histo.size(); i++){
	if( fabs(histo[i].x-maxima[j].x)<=width &&
	    fabs(histo[i].y-maxima[j].y)<=width   ){
	  
	  w  =  (float)histo[i].c * (histo[i].zmax-histo[i].zmin);
	  sx += w * (Xlo+(Xup-Xlo)*((float)histo[i].x+0.5)/(float)nbx);
	  sy += w * (Ylo+(Yup-Ylo)*((float)histo[i].y+0.5)/(float)nby);
	  sz += w * histo[i].z/(float)histo[i].c;
	  sw += w;
	  if(histo[i].zmin<zmin)zmin=histo[i].zmin;
	  if(histo[i].zmax>zmax)zmax=histo[i].zmax;
	}
      }


      if(debug)printf("maxima %i sw %.2f dz %.2f\n",j,sw,zmax-zmin);
      if(sw==0.||zmax-zmin==0.)continue;
      if(vec.size()>0&&(sw<300||zmax-zmin<10.))continue;
      float prop_offset=sx/sw+(sz/sw-TRDz0)*sy/sw;

      PeakXYZW peak(prop_offset,sy/sw,sz/sw,zmin,zmax,sw);
      vector<PeakXYZW>::iterator f = find(vec.begin(), vec.end(),peak);
      if(f==vec.end())vec.push_back(peak);
    }
    
    sort(vec.begin(),vec.end());
    return vec;
  }


integer TrdHSegmentR::build(int rerun){
  // This routine does the TRD 'H' Segment fitting based on TRDRawHits
  // It reconstructs segments separately for the two TRD projections:
  // in XZ plane (d=0) from TRD layers 5-16 
  // in YZ plane (d=1) from TRD layers 1-4 and 17-20
  int debug=0;
  
  if(debug)printf("Entering TrdHSegmentR::build\n");
  if(debug)printf("trdhrecon hit size %i\n",trdhrecon.rhits.size());
  trdhrecon.hsegvec.clear();
  
  // skip arrays for now
  int nhcut=1.e6; //  below 40 nTRDRawHits: vector-histo 
  // above                 array-histo
  
  double Hcut=0.0;
  //if(pev->nTRDRawHit()>500) Hcut=30.0;
  
  int nrh=trdhrecon.rhits.size();
  if(debug)printf("RawHits %i\n",nrh);
  if(nrh<4||nrh>100) return 0;

  if(!trdhrecon.H2A_mvr[0])trdhrecon.H2A_mvr[0] = new TH2A ("ha_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0);
  if(!trdhrecon.H2A_mvr[1])trdhrecon.H2A_mvr[1] = new TH2A ("ha_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0);
		   
  if(!trdhrecon.H2V_mvr[0])trdhrecon.H2V_mvr[0]=new TH2V ("hv_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0);
  if(!trdhrecon.H2V_mvr[1])trdhrecon.H2V_mvr[1]=new TH2V ("hv_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0);

  if(nrh>nhcut){
    trdhrecon.H2A_mvr[0]->Reset();
    trdhrecon.H2A_mvr[1]->Reset();
  }else{
    trdhrecon.H2V_mvr[0]->Reset();
    trdhrecon.H2V_mvr[1]->Reset();
  }
  
  vector<PeakXYZW> segvec_x;
  vector<PeakXYZW> segvec_y;
  
  int prefit=trdhrecon.DoPrefit(segvec_x,segvec_y);
  if(debug)printf("TrdHSegmentR build NofSegments sum %i = %i?\n",segvec_x.size()+segvec_y.size(),prefit);
  if(prefit<=0){
    if(debug)printf("skipping \n");
    return 0;
  }
  vector<TrdHSegmentR*> segments=trdhrecon.DoLinReg(&segvec_x,&segvec_y);
  
  trdhrecon.hsegvec=trdhrecon.clean_segvec(segments);
  
  for(int i=0;i!=trdhrecon.hsegvec.size();i++)
    trdhrecon.hsegvec[i]->calChi2();

  if(debug) printf("returning %i segments\n",trdhrecon.hsegvec.size());

  return trdhrecon.hsegvec.size();
}




void TH2V::Fill( float x, float y, float za, float zb, float weight ){
  ix = int((x-Xlo)*Fx);
  iy = int((y-Ylo)*Fy);
  //    iz = int((z-Zlo)*Fz);
  add=true;
  for( int i=0; i<histo.size(); i++ ){
    if( ix==histo[i].x && iy==histo[i].y){// && iz == histo[i].z){
      histo[i].c+=weight;
      histo[i].z+=(za+zb)/2.;
      if(za<histo[i].zmin)histo[i].zmin=za;
      if(zb<histo[i].zmin)histo[i].zmin=zb;
      if(za>histo[i].zmax)histo[i].zmax=za;
      if(zb>histo[i].zmax)histo[i].zmax=zb;
      add = false;
      break;
    }
  }
  if( add ){
    int kk=(za+zb)/2.;
    BIN bin(ix,iy,kk,weight);
    histo.push_back(bin);
  }
}
