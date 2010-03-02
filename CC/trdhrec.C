//  $Id: trdhrec.C,v 1.8 2010/03/02 14:40:42 choutko Exp $
#include "trdrec.h"
#include "trdhrec.h"
#include "event.h"
#include "ntuple.h"
#include "extC.h"
#include "root.h"
#include "tofrec02.h"
#include "fenv.h"

static AMSTRDHSegment* pTRDHSegment_[10];
#pragma omp threadprivate (pTRDHSegment_)

static AMSTRDHTrack* pTRDHTrack_[4];
#pragma omp threadprivate (pTRDHTrack_)

static int nhseg,nhtr;
#pragma omp threadprivate (nhseg)
#pragma omp threadprivate (nhtr)

const float TRDz0=113.5;
const float RSDmax=0.6;

static TH2A* H2A_mvr[2] = {
  new TH2A ("ha_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0),
  new TH2A ("ha_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0)
};

static TH2V* H2V_mvr[2] = {
  new TH2V ("hv_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0),
  new TH2V ("hv_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0)
};
#pragma omp threadprivate (H2V_mvr)
#pragma omp threadprivate (H2A_mvr)


int AMSTRDHSegment::LinReg2(int debug=0)
  { // fit  Hit.rz   with  r = A*z + B
    if(debug>0)printf("Entering LinReg2 - %i RawHits\n",fTRDRawHit.size());
    double WSr=0.,WSz=0.,WSrr=0.,WSzz=0.,WSrz=0.,WS=0.,W=1.;
    if(fTRDRawHit.size()<3)return -1;
    
    for(int i=0; i<fTRDRawHit.size(); i++){
      //      TRDHitRZD rzd(fTRDRawHit[i]);
      TRDHitRZD rzdi=TRDHitRZD(fTRDRawHit[i]);
      int nz=1;
      float ri=rzdi.r;
      float zi=rzdi.z;
      
      for(int j=i+1; j<fTRDRawHit.size(); j++){
	//	TRDHitRZD rzd(fTRDRawHit[j]);
	TRDHitRZD rzdj=TRDHitRZD(fTRDRawHit[j]);
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
	for(int j=0; j<fTRDRawHit.size(); j++){
	  //	  TRDHitRZD rzd=fTrdRawHit[j];
	  TRDHitRZD rzd=TRDHitRZD(fTRDRawHit[j]);
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
      if(debug>0)printf("LinReg2 Sum of weights WS = 0.\n");
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
      r=WMr;m=0.;chi2=0.;er=0.6/sqrt(12);
      if(debug)printf("problem occured SDr %f -> r %.2f m %.2f chi2 %.2f er %.2f\n",SDr,r,m,chi2,er);
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
    chi2 = WS*SDr*SDr*(1.-Rrz*Rrz);

    if(debug>0)printf("linreg result m %.2e (+/- %.2e ) r %.2e (+/- %.2e)\n",m,em,r,er);
    return 1;
  }

vector<AMSTRDHSegment*> DoLinReg(vector<PeakXYZW> *segvec_x,vector<PeakXYZW> *segvec_y,int debug=0){
  if(debug>0)printf("Entering DoLinReg\n");

  vector<AMSTRDRawHit*> rhits;
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",0);Hi;Hi=Hi->next()) rhits.push_back(Hi);
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",1);Hi;Hi=Hi->next()) rhits.push_back(Hi);

  if(debug>0)printf("Total number of hits %i\n",rhits.size());

  vector<AMSTRDHSegment*> segments;
  for(int d=0;d!=2;d++){
    vector<PeakXYZW> *segvec=NULL;
    if(d==0) segvec=segvec_x;
    if(d==1) segvec=segvec_y;
    for(int i=0;i!=segvec->size();i++){
      AMSTRDHSegment *seg=new AMSTRDHSegment(d,segvec->at(i).y,0.,segvec->at(i).x,0.,segvec->at(i).z,segvec->at(i).w);
      seg->em=0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
      
      for(int h=0;h!=rhits.size();h++){	
	TRDHitRZD rzd=TRDHitRZD(rhits[h]);
	if(rzd.d!=d)continue;
	
	float dz=rzd.z-seg->z;
	float expos=seg->r+ seg->m*dz;//*cos(atan(seg->m));
	float resid=rzd.r - expos;
	float maxresid=0.6+fabs(dz)* seg->em;
	if(debug>0)printf("d %i hit %i r %.2f expos %.2f resid %f < %f ok %i\n",d,h,rzd.r,expos,resid,maxresid,fabs(resid)<maxresid);
	
	if(fabs(resid)<maxresid)
	  seg->fTRDRawHit.push_back(rhits[h]);
      }
      seg->nhits=seg->fTRDRawHit.size();
      
      if(debug>0)printf("seg %i m %.2f r %.2f z %.2f hits %i\n",i,seg->m,seg->r,seg->z,seg->nhits);
      //      seg=Refit(seg);
      
      int lr=1;
      while(lr>=0){
	lr=seg->LinReg2(0);
	int n0=seg->fTRDRawHit.size();
	seg->fTRDRawHit.clear();
	for(int h=0;h!=rhits.size();h++){	
	  TRDHitRZD rzd=TRDHitRZD(rhits[h]);
	  if(rzd.d!=d)continue;
	  
	  float dz=rzd.z-seg->z;
	  float resid=(rzd.r - (seg->r+ seg->m*dz));//*cos(atan(seg->m));
	  float maxresid=0.6+fabs(dz)*seg->em;//0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
	  
	  if(debug>0)printf("Lin Reg residuals %.2f < %.2f %i (dz %.2f em %.2e)\n",resid,maxresid,fabs(resid)<maxresid,rzd.z-seg->z,seg->em);
	  if(fabs(resid)<maxresid)
	    seg->fTRDRawHit.push_back(rhits[h]);
	  
	}
	
	if(n0>=seg->fTRDRawHit.size()){
	  if(debug)printf("no more hits - breaking loop\n");
	  break;
	}
	if(debug)printf("d %i n0 %i n1 %i - refitting\n",d,n0,seg->fTRDRawHit.size());
      }
      
      seg->nhits=seg->fTRDRawHit.size();
      seg->calChi2();
      
      if(debug>0){
	printf("d %i after linreg loop: segment %i hits %i\n",d,i,seg->nhits);
	printf("After linreg: seg %i d %i m %.2f r %.2f z %.2f hits %i\n",i,d,seg->m,seg->r,seg->z,seg->nhits);
      }
      segments.push_back(seg);
    }
  }
  return segments;
}

AMSTRDHSegment* Refit(AMSTRDHSegment* seg,int debug=0){
  if(debug>0)printf("Enter Refit - hit size %i\n",seg->fTRDRawHit.size());
  int lr=seg->LinReg2(debug);
  
  int lay[20];for(int l=0;l!=20;l++)lay[l]=0;
  float sum=0.;
  float zmax=0.,zmin=200.;
  for(int h=0;h!=seg->fTRDRawHit.size();h++){	
    TRDHitRZD rzd=TRDHitRZD(seg->fTRDRawHit[h]);
    if(rzd.d!=seg->d)continue;
    if(rzd.z>zmax)zmax=rzd.z;
    if(rzd.z<zmin)zmin=rzd.z;
    
    for(int l=0;l!=20;l++){
      if(fabs((rzd.z-85.275)/2.9-(float)l)<0.01)lay[l]++;
      else if (fabs((rzd.z-85.275-1.45)/2.9 -(float)l)<0.01)lay[l]++;
    }
  }
  
  int lsum=0;
  for(int l=0;l!=20;l++)if(lay[l]>0)lsum++;
  if(debug)printf("linreg stat %i dz %.2f layers %i\n",lr,zmax-zmin,lsum);
  if(lr==1&&zmax-zmin>10.&&lsum>2){
    seg->nhits=seg->fTRDRawHit.size();
    seg->calChi2();
    return seg;
  }
  else return 0;
}

vector<AMSTRDHSegment*> clean_segvec(vector<AMSTRDHSegment*> vec,int debug=0){
  //PZ FPE bugfix 
  // this function contains broken code 
  // it should be rewritten
  // for the moment really-force FPE masking

    int env=0;
#ifndef __DARWIN__
    env=fegetexcept();
    if(MISCFFKEY.RaiseFPE<=2){
      fedisableexcept(FE_OVERFLOW);
      fedisableexcept(FE_INVALID);
      fedisableexcept(FE_DIVBYZERO);
    }
#endif    

  if(debug>0){
    printf("Enter clean_segvec\n");
    printf("before clean %i\n",vec.size());
    for(int s1=0;s1!=vec.size();s1++){
      printf("segment %i d %i n %i m %.2f r %.2f z %.2f chi2 %.2e\n",
	     s1,vec[s1]->d,vec[s1]->nhits,vec[s1]->m,vec[s1]->r,vec[s1]->z,vec[s1]->chi2);
    }
  }
  
 restart:;
  for(int s1=0;s1!=vec.size();s1++){
    if(vec.size()>2&&vec[s1]->chi2/((float)vec[s1]->nhits-2)>6){
      if(debug>0)printf("removing segment %i because of high chi2/ndof %f\n",s1,vec[s1]->chi2/((float)vec[s1]->nhits-2));
      
      vec.erase(vec.begin()+s1);goto restart;
    }
    
    for(int s2=s1+1;s2!=vec.size();s2++){
      if(vec[s1]->d!=vec[s2]->d)continue;
      
      int nboth=0;
      int n1=vec[s1]->fTRDRawHit.size(),n2=vec[s2]->fTRDRawHit.size();
      
      int hiter1[n1],hiter2[n2];
      for(int hit1=0;hit1!=n1;hit1++)hiter1[hit1]=0;
      for(int hit2=0;hit2!=n2;hit2++)hiter2[hit2]=0;
      
      for(int hit1=0;hit1!=n1;hit1++){
	TRDHitRZD rzd1=TRDHitRZD(vec[s1]->fTRDRawHit[hit1]); 
	for(int hit2=0;hit2!=n2;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(vec[s2]->fTRDRawHit[hit2]); 

	  if(debug>1) printf("hit1 %i r %.2f z %.2f hit2 %i r %.2f z %.2f both %i\n",hit1,rzd1.r,rzd1.z,hit2,rzd2.r,rzd2.z,rzd1.r == rzd2.r && rzd1.z == rzd2.z);
	  if( rzd1.r == rzd2.r &&
	      rzd1.z == rzd2.z){
	    hiter1[hit1]=1;hiter2[hit2]=1;
	    nboth++;
	  }
	}
      }
      
      if(debug>0)printf("d %i s1 %i n1 %i m1 %.2f chi2/ndof %.2e s2 %i n2 %i m2 %.2f chi2/ndof %.2e nboth %i\n",vec[s1]->d,s1,n1,vec[s1]->m,vec[s1]->chi2/((float)vec[s1]->fTRDRawHit.size()-2.),s2,n2,vec[s2]->m,vec[s2]->chi2/((float)vec[s2]->fTRDRawHit.size()-2.),nboth);
      
      
      if(n1<=2){vec.erase(vec.begin()+s1);goto restart;;}
      if(n2<=2){vec.erase(vec.begin()+s2);goto restart;}
      
      if(n1==n2&&nboth==n2){
	if(debug>0)printf("same segments\n");
	if (vec[s1]->chi2==0||vec[s1]->nhits<3)vec.erase(vec.begin()+s1);
	else if(vec[s2]->chi2==0.||vec[s2]->nhits<3)vec.erase(vec.begin()+s2);
	else if(vec[s1]->chi2/((float)vec[s1]->fTRDRawHit.size()-2.) > vec[s2]->chi2/((float)vec[s2]->fTRDRawHit.size()-2.))vec.erase(vec.begin()+s1);
	else vec.erase(vec.begin()+s2);
	goto restart;
      }
      else if(nboth>1){
	if(debug>0)printf("nboth %i > 1 !\n",nboth);
	float s1_chi2ndof=vec[s1]->chi2/((float)vec[s1]->fTRDRawHit.size()-2.);
	float s2_chi2ndof=vec[s2]->chi2/((float)vec[s2]->fTRDRawHit.size()-2.);

	if(s2_chi2ndof==0.)vec.erase(vec.begin()+s2);
	else if(s1_chi2ndof==0.)vec.erase(vec.begin()+s1);
	else if(s2_chi2ndof>s1_chi2ndof){
	  if(debug>0)printf("Refit s2\n");
	  for(int i=n2-1;i>=0;i--) if(hiter2[i]==1) vec[s2]->fTRDRawHit.erase(vec[s2]->fTRDRawHit.begin()+i);
	  if(debug>0)printf("refit hit size %i -> %i\n",vec[s2]->nhits,vec[s2]->fTRDRawHit.size());
	  if(vec[s2]->fTRDRawHit.size()>2){
	    AMSTRDHSegment *seg=Refit(vec[s2],debug);
	    if(seg) replace(vec.begin(),vec.end(),vec[s2],seg);
	    else vec.erase(vec.begin()+s2);
	  }
	  else vec.erase(vec.begin()+s2);
	}
	else {
	  if(debug)printf("Refit s1\n");
	  for(int i=n1-1;i>=0;i--) if(hiter1[i]==1) vec[s1]->fTRDRawHit.erase(vec[s1]->fTRDRawHit.begin()+i);
	  if(debug>0)printf("refit hit size %i -> %i\n",vec[s1]->nhits,vec[s1]->fTRDRawHit.size());
	  if(vec[s1]->fTRDRawHit.size()>2){
	    AMSTRDHSegment *seg=Refit(vec[s1],debug);
	    if(seg) replace(vec.begin(),vec.end(),vec[s1],seg);
	    else vec.erase(vec.begin()+s1);
	  }
	  else vec.erase(vec.begin()+s1);
	}
	goto restart;
      }      
    }
  } 
  

  sort(vec.begin(),vec.end());
  //  while(vec.size()>10)vec.pop_back();
  int n0=0,n1=0;
  for(int i=0;i!=vec.size();i++){
    if(vec[i]->d==0)n0++;
    if(vec[i]->d==1)n1++;
  }
  for(int i=vec.size()-1;i>=0;i--){
    if(vec[i]->d==0&&n0>5){vec.erase(vec.begin()+i);n0--;}
    if(vec[i]->d==1&&n1>5){vec.erase(vec.begin()+i);n1--;}
  }



  if(debug>0){
    printf ("REMAINING SEGMENTS:\n");
    for(int i=0;i!=vec.size();i++){
      printf("d %i s %i n %i m %.2f r %.2f chi2/ndof %.2e \n",vec[i]->d,i,vec[i]->nhits,vec[i]->m,vec[i]->r,vec[i]->chi2/((float)vec[i]->nhits-2.));
    }
    printf("after clean %i\n",vec.size());
    printf("Exiting clean_segvec\n");
  } 

#ifndef __DARWIN__
    feclearexcept(FE_OVERFLOW);
    feclearexcept(FE_INVALID);
    feclearexcept(FE_DIVBYZERO);
    if(env){
      feenableexcept(env);        
    }
#endif    
  
  return vec;
}
AMSTRDHTrack* SegToTrack(AMSTRDHSegment *s1, AMSTRDHSegment* s2, int debug){
  if(debug)printf("Entering AMSTRDHTrack::SegToTrack\n");
  if(!s1||!s2)return 0;
  if(s1->d==s2->d)return 0;
  if(isnan(s1->m)||isnan(s2->m))return 0;
  if(debug)printf("segments seem to be ok\n");

  float m[2]={0.,0.};
  float em[2]={0.,0.};
  float r[2]={0.,0.};
  float er[2]={0.,0.};
  float z[2]={0.,0.};
  
  em[s1->d]=s1->em;m[s1->d]=s1->m;r[s1->d]=s1->r;er[s1->d]=s1->er;z[s1->d]=s1->z;
  em[s2->d]=s2->em;m[s2->d]=s2->m;r[s2->d]=s2->r;er[s2->d]=s2->er;z[s2->d]=s2->z;
  
  if(debug)for(int d=0;d!=2;d++){
    printf("d %i m %.2f em %.2f r %.2f z %.2f\n",d,m[d],em[d],r[d],z[d]);
  }
  // same center of segments
  float zcent=(s1->z * (float) s1->nhits + s2->z * (float) s2->nhits) /
    ((float) s1->nhits + (float) s2->nhits);
  
  for(int d=0;d!=2;d++)
    r[d]=r[d] + m[d] * ( zcent - z[d] );

  float dir[3],pos[3];
  dir[0]=-m[0];dir[1]=-m[1],dir[2]=-1.;
  pos[0]=r[0];pos[1]=r[1],pos[2]=zcent;

  if(debug)printf("built track pos %.2f %.2f %.2f mx %.2f my %.2f\n",pos[0],pos[1],pos[2],dir[0]/dir[2],dir[1]/dir[2]);

  AMSTRDHTrack* tr=new AMSTRDHTrack(pos,dir);
  tr->chi2=s1->chi2+s2->chi2;
  tr->emx=em[0];tr->emy=em[1];
  tr->ex=er[0];tr->ey=er[1];
  tr->nhits=s1->fTRDRawHit.size()+s2->fTRDRawHit.size();
  tr->SetSegment(s1,s2);

  //  printf("Leaving SegToTrack - return AMSTRDHTrack\n");
  return tr;
}

vector<pair<int,int> > check_secondaries(){
  vector<pair<int, int> > toReturn;toReturn.clear();

  int zvec[7];for(int i=0;i!=7;i++)zvec[i]=0.;
  for(int s1=0;s1!=nhseg;s1++){
    for(int s2=s1+1;s2!=nhseg;s2++){
      if(pTRDHSegment_[s1]->d==pTRDHSegment_[s2]->d){
	// check for z of crossing
	float z_cross = (pTRDHSegment_[s2]->r - pTRDHSegment_[s1]->r - pTRDHSegment_[s2]->m * pTRDHSegment_[s2]->z + 
			 pTRDHSegment_[s1]->m * pTRDHSegment_[s1]->z) / ( pTRDHSegment_[s1]->m - pTRDHSegment_[s2]->m );
	
	//	printf("d %i s1 %i s2 %i zcross %.2f\n",pTRDHSegment_[s1]->d,s1,s2,z_cross);
	if(z_cross>150||z_cross < 80)continue;
	zvec[(int)floor((z_cross-80.)/10.)]++;;
	if(pTRDHSegment_[s1]->d==0&&(z_cross>120||z_cross<102))continue;

	float s1zmax=0.,s1zmin=200.;
	for(int hit1=0;hit1!=pTRDHSegment_[s1]->nhits;hit1++){
	  TRDHitRZD rzd1=TRDHitRZD(pTRDHSegment_[s1]->fTRDRawHit[hit1]);
	  if(rzd1.z>s1zmax)s1zmax=rzd1.z;
	  if(rzd1.z<s1zmin)s1zmin=rzd1.z;
	}
	float s2zmax=0.,s2zmin=200.;
	for(int hit2=0;hit2!=pTRDHSegment_[s2]->nhits;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(pTRDHSegment_[s2]->fTRDRawHit[hit2]);
	  if(rzd2.z>s2zmax)s1zmax=rzd2.z;
	  if(rzd2.z<s2zmin)s1zmin=rzd2.z;
	}

	if(s1zmax<z_cross+3){
	  //printf("vertex z %.2f s1 downward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s2,-s1));
	}
	else if(s2zmax<z_cross+3){
	  //printf("vertex z %.2f s2 downward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s1,-s2));
	}
	else if(s1zmin>z_cross-3){
	  //printf("vertex z %.2f s1 upward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s2,s1));
	}
	else if(s2zmin>z_cross-3){
	  //printf("vertex z %.2f s2 upward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s1,s2));
	}
	
      }
    }
  }

  for(int i=0;i!=7;i++){
    if(zvec[i]>4)toReturn.push_back(make_pair<int,int>(99,99));
  }

  return toReturn;
}
bool check_geometry(AMSTRDHSegment *s1, AMSTRDHSegment* s2){
  
  // idea is to calculate if possible track would leave TRD but has hits "outside" TRD
  // for this define TRD contours as a function of z and compare to highest/ lowest hitz
  AMSTRDHTrack* tr=SegToTrack(s1, s2);
  if(!tr)return 0;
  for(int i=0;i!=s1->nhits;i++){
    TRDHitRZD rzd=TRDHitRZD(s1->fTRDRawHit[i]);
    float x=0.,y=0.;
    tr->propagateToZ(rzd.z,x,y);
    
    float base=770+230./(rzd.z-132.);
    float side=320+100./(rzd.z-132.);
    float line=720+205./(rzd.z-132);

    if(fabs(x)>base)return 0;
    if(fabs(y)>base)return 0;
    if(fabs(y)>line)return 0;
    if(fabs(x)+fabs(x)>side)return 0;
  }

  return 1;
}

int  combine_segments(){
  bool s_done[nhseg];
  bool s_poss[nhseg][nhseg];

  for(int i=0;i!=nhseg;i++){
    s_done[i]=0;
    for(int j=0;j!=nhseg;j++){
      if(j==i||pTRDHSegment_[i]->d==pTRDHSegment_[j]->d){s_poss[i][j]=0;s_poss[j][i]=0;}
      else {s_poss[i][j]=1;s_poss[j][i]=1;}
    }
  }
  
  for(int i=0;i!=nhseg;i++){
    for(int j=i+1;j!=nhseg;j++){
      if(s_poss[i][j]==0)continue;
      //      printf("seg %i %i geometry %i TOF %i TRtop %i\n",i,j,check_geometry(pTRDHSegment_[i],pTRDHSegment_[j]),check_TOF(pev,pTRDHSegment_[i],pTRDHSegment_[j]),check_TRtop(pev,pTRDHSegment_[i],pTRDHSegment_[j]));

      if(pTRDHSegment_[i]->nhits<3){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_geometry(pTRDHSegment_[i],pTRDHSegment_[j])==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_TOF(pTRDHSegment_[i],pTRDHSegment_[j])<0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_TRtop(pTRDHSegment_[i],pTRDHSegment_[j])==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
    }
  }
  
  //  vector<TrackR*> trvec;
  int n_poss[nhseg];
  for(int i=0;i!=nhseg;i++){
    if(s_done[i]==1)continue;
    
    n_poss[i]=0;
    for(int j=0;j!=nhseg;j++)if(s_poss[i][j]==1)n_poss[i]++;
    
    //    if(n_poss[i]>1)printf("more than one possibility left (%i)\n",n_poss[i]);
    else if(n_poss[i]==1){
      for(int j=0;j!=nhseg;j++){
	if(s_poss[i][j]!=1)continue;
	if(s_done[i]==1||s_done[j]==1)continue;
	//	printf("generat track of segments %i %i \n",i,j);
	AMSTRDHTrack *tr=SegToTrack(pTRDHSegment_[i],pTRDHSegment_[j]);
	tr->status=1;
	if(tr){pTRDHTrack_[nhtr++]=tr;s_done[i]=1;s_done[j]=1;}
      }
    }
  }

  int n_done=0;
  for(int i=0;i!=nhseg;i++)if(s_done[i]==1)n_done++;
  if(nhseg==n_done)return nhtr;

  //  printf("%i segment(s) not done yet before sec. search \n",nhseg-n_done);

  // check for vertices
  vector<pair<int,int> > check=check_secondaries();
  //  vector<int> check;
  if(check.size()>0){

    // skip secondary vertices for now
    for(int i=0;i!=check.size();i++){
      //      printf("check %i primary %i secondary %i\n",i,check[i].first,check[i].second);
      int s=abs(check[i].second);
      s_done[s]=1;
      if(check[i].second==99&&check[i].first==99){
	//	save_ev=1;
	//	printf("\nHADRONIC INTERACTION IN TRD\n");
	return nhtr;
      }
    }
  }

  n_done=0;
  for(int i=0;i!=nhseg;i++)if(s_done[i]==1)n_done++;
  //  if(nhseg==n_done)return trvec;
  //  printf("%i segment(s) not done after sec. search \n",nhseg-n_done);
  //  sleep(5);
  return nhtr;
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


int DoPrefit(vector<PeakXYZW> &seg_x, vector<PeakXYZW> &seg_y){
  int debug=0;

  vector<AMSTRDRawHit*> rhits;
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",0);Hi;Hi=Hi->next()) rhits.push_back(Hi);
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",1);Hi;Hi=Hi->next()) rhits.push_back(Hi);


  for(int d=0;d!=2;d++){
    if(!H2V_mvr[d])H2V_mvr[d]=new TH2V ("", ";offet [cm];slope", 314,-105,105, 119, -3, 3);
    H2V_mvr[d]->Reset();
    
    // ARRAY should be included
    
  }
  
  int pairs_x=0,pairs_y=0;;
  for(int i=0;i!=rhits.size();i++){
    TRDHitRZD rzdi=TRDHitRZD(rhits[i]);
    for(int j=i+1;j!=rhits.size();j++){
      //      TrdHitRZD rzdj=Trd_LLT2XYZ(&rhitvec[j]);
      TRDHitRZD rzdj=TRDHitRZD(rhits[j]);
      
      if(rzdi.d==rzdj.d && rzdi.z!=rzdj.z){
	float s = (rzdi.r-rzdj.r)/(rzdi.z-rzdj.z); //slope
	float o = rzdi.r - (rzdi.z-TRDz0)*s; //offset
	float z = (rzdi.z + rzdj.z)/2.;
	
	if(rzdj.d==0){H2V_mvr[0]->Fill(o,s,rzdi.z,rzdj.z,1.);pairs_x++;}
	else if(rzdj.d==1){H2V_mvr[1]->Fill(o,s,rzdi.z,rzdj.z,1.);pairs_y++;}
      }
    }
  }
  if(debug)printf("pairs x %i y %i\n",pairs_x,pairs_y);
  if(pairs_x<5&&pairs_y<5)return 0;
  
  seg_x=H2V_mvr[0]->FindPeaks();
  seg_y=H2V_mvr[1]->FindPeaks();
  if(debug)printf("DoPrefit segx %i segy %i\n",seg_x.size(),seg_y.size());

  return seg_x.size()+seg_y.size();
}

integer AMSTRDHSegment::build(int rerun){
// This routine does the TRD Track fitting based on TRDRawHits
// It reconstructs segments separately for the two TRD projections:
// in XZ plane (d=0) from TRD layers 5-16 
// in YZ plane (d=1) from TRD layers 1-4 and 17-20

  int debug=0;
  if(debug)printf("Entering AMSTRDHSegment::build\n");
  
  nhseg=0;
  for(int i=0;i!=6;i++) pTRDHSegment_[i]=NULL;
  
  // skip arrays for now
  int nhcut=1.e6; //  below 40 nTRDRawHits: vector-histo 
  // above                 array-histo
  
  double Hcut=0.0;
  //if(pev->nTRDRawHit()>500) Hcut=30.0;
  
  int nrh0=0,nrh1=0;
  nrh0= AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem();
  nrh1= AMSEvent::gethead()->getC("AMSTRDRawHit",1)->getnelem();
  
  int nrh=nrh0+nrh1;
  if(debug)printf("RawHits %i\n",nrh);
  if(nrh<4||nrh>100) return 0;
  
  vector<AMSTRDRawHit*> rhits;
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",0);Hi;Hi=Hi->next()) rhits.push_back(Hi);
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",1);Hi;Hi=Hi->next()) rhits.push_back(Hi);
  
  if(nrh>nhcut){
    H2A_mvr[0]->Reset();
    H2A_mvr[1]->Reset();
  }else{
    H2V_mvr[0]->Reset();
    H2V_mvr[1]->Reset();
  }
  
  vector<PeakXYZW> segvec_x;
  vector<PeakXYZW> segvec_y;
  
  int prefit=DoPrefit(segvec_x,segvec_y);
  if(debug)printf("AMSTRDHSegment build NofSegments sum %i = %i?\n",segvec_x.size()+segvec_y.size(),prefit);
  if(prefit<=0){
    if(debug)printf("skipping \n");
    return 0;
  }
  vector<AMSTRDHSegment*> segments=DoLinReg(&segvec_x,&segvec_y);
  
  segments=clean_segvec(segments);
  nhseg=segments.size();
  
  for(int i=0;i!=min(nhseg,10);i++){
    pTRDHSegment_[i]=segments[i];
    pTRDHSegment_[i]->nhits=pTRDHSegment_[i]->fTRDRawHit.size();
    pTRDHSegment_[i]->calChi2();
    AMSEvent::gethead()->addnext(AMSID("AMSTRDHSegment",0),pTRDHSegment_[i]);
  }
  
  return nhseg;
}
 
  
integer AMSTRDHTrack::build(int rerun){
  int debug=0; 
  for(int i=0;i!=4;i++) pTRDHTrack_[i]=NULL;
  nhtr=0;
  if(debug)printf("Entering AMSTRDHTrack::build - %i segments\n",nhseg);
  
  if(nhseg==2||nhseg==3){
    int segiter_sel[2]={-1,-1};
    
    for(int s=0;s!=nhseg;s++){
      int d=pTRDHSegment_[s]->d;
      float chi2ndofs=1.e6;
      if(segiter_sel[d]>-1){
	float chi2ndof=1.e6;
	if(pTRDHSegment_[s]->nhits>2)chi2ndof=(float) pTRDHSegment_[s]->chi2 / (float) pTRDHSegment_[s]->nhits;
	if(pTRDHSegment_[segiter_sel[d]]->nhits>2)chi2ndofs=(float) pTRDHSegment_[segiter_sel[d]]->chi2 / (float) pTRDHSegment_[segiter_sel[d]]->nhits;
	
	if(debug)printf("cand s %.2f < %.2f ?\n",chi2ndof,chi2ndofs);
	if( ( pTRDHSegment_[s]->nhits >  pTRDHSegment_[segiter_sel[d]]->nhits) ||
	    ((pTRDHSegment_[s]->nhits == pTRDHSegment_[segiter_sel[d]]->nhits) &&
	     (chi2ndof < chi2ndofs))) segiter_sel[d]=s;
      }
      else if(pTRDHSegment_[s]->nhits>0)segiter_sel[d]=s;
      
      if(debug)printf("sel %i d %i nhits %i chi2ndof %.2f\n",s,d,pTRDHSegment_[segiter_sel[d]]->nhits,chi2ndofs);
    }

    if(debug) printf("segiter %i %i\n",segiter_sel[0],segiter_sel[1]);
    if(segiter_sel[0]>-1&&segiter_sel[1]>-1){
      AMSTRDHTrack *tr=SegToTrack(pTRDHSegment_[segiter_sel[0]],pTRDHSegment_[segiter_sel[1]]);
      tr->status=1;
      if(tr)pTRDHTrack_[nhtr++]=tr;
    }
  }
  else if(nhseg>2) nhtr=combine_segments();
  
  if(TRDFITFFKEY.SaveHistos>0){
    TRDInfoNtuple02* info = AMSJob::gethead()->getntuple()->Get_trdinfo02();
    if(info){
      for(int i=0;i!=2;i++){
	AMSTRDHSegment* seg=pTRDHTrack_[0]->fTRDHSegment[i];
	for(int j=0;j!=seg->nhits;j++){
	  AMSTRDIdSoft id(seg->fTRDRawHit[j]->getidsoft());
	  int lay=id.getlayer();
	  int lad=id.getladder();
	  int tub=id.gettube();
	  float amp=seg->fTRDRawHit[j]->Amp();
	  
#ifdef __WRITEROOT__
	  int hid=41000+lay*290+lad*16+tub;
	  //	  TH1F* h=AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid);
	  //	  if(!h){
	  if(!AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid)){
	    //      if(info->hit_arr[lay][lad][tub]==0){
	    char hnam[30];
	    sprintf(hnam,"TRD_L%iL%iT%i",lay,lad,tub);
	    AMSJob::gethead()->getntuple()->Get_evroot02()->hbook1(hid,hnam,50,0,200);
	  }
	  AMSJob::gethead()->getntuple()->Get_evroot02()->hfill(hid,amp,0.,1.);
	  
#endif
	  info->hit_arr[lay][lad][tub]++;
	}
      }
    }
  }
  
  if(debug)printf("AMSTRDHTrack::build tracks %i\b",nhtr);
  for(int i=0;i!=nhtr;i++){
    AMSEvent::gethead()->addnext(AMSID("AMSTRDHTrack",0),pTRDHTrack_[i]);
  }
  
  return nhtr;
}

void AMSTRDHSegment::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  TrdHSegmentR & ptr =AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHSegment(_vpos); 
  for (int i=0; i<nhits; i++) {
    AMSTRDRawHit* rh=fTRDRawHit[i];
    if (rh) ptr.fTrdRawHit.push_back(rh->GetClonePointer());
  }
#endif
}

void AMSTRDHSegment::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
    || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHSegment::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHSegment",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

integer AMSTRDHSegment::_addnext(integer d, float m, float em, float r, float er, float z, float w, integer nhits, AMSTRDRawHit* pthit[]){
#ifdef __UPOOL__
  cout<< "AMSTRDHSegment::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else
  AMSTRDHSegment *ptrack= new AMSTRDHSegment(d,m,em,r,er,z,w,nhits,pthit);
#endif
  ptrack->_addnextR(0);

  delete ptrack;  
  return 0;
}

AMSTRDHSegment *AMSTRDHSegment::gethead(uinteger i){
  if(i<TRDDBc::nlayS()){
    return (AMSTRDHSegment*)AMSEvent::gethead()->getheadC("AMSTRDHSegment",i,1);
  }
  else {
#ifdef __AMSDEBUG__
    cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
    return 0;
  }
}

AMSTRDHTrack *AMSTRDHTrack::gethead(uinteger i){
  return (AMSTRDHTrack*)AMSEvent::gethead()->getheadC("AMSTRDHTrack",i,1);
#ifdef __AMSDEBUG__
  cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
  return 0;
}


void AMSTRDHSegment::_addnextR(uinteger iseg){
  // permanently add;
  AMSEvent::gethead()->addnext(AMSID("AMSTRDHSegment",iseg),this);
}

void AMSTRDHTrack::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  TrdHTrackR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHTrack(_vpos);
  for (int i=0; i<2; i++) {
    AMSTRDHSegment *seg=fTRDHSegment[i];
    if(seg)ptr.fTrdHSegment.push_back(seg->GetClonePointer()); 
  }
#endif
}

void AMSTRDHTrack::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHTrack::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

void AMSTRDHSegment::_printEl(ostream &o){
  o << " TRDHSegment printout "<<endl;
}

void AMSTRDHTrack::_printEl(ostream &o){
  o <<"TRDHTrack printout"<<endl;
}

integer AMSTRDHTrack::_addnext(float pos[3],float dir[3], AMSTRDHSegment* pthit[]){

#ifdef __UPOOL__
  cout<< "AMSTRDHTrack::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else

  AMSTRDHTrack *ptrack=new AMSTRDHTrack(pos,dir);
  if(!pthit[0]&&!pthit[1]) ptrack->SetSegment(pthit[0],pthit[1]);
  
#endif
  
  ptrack->_addnextR(0);
  
  delete ptrack;  
  return 0;
}

void AMSTRDHTrack::_addnextR(uinteger itr){
  int i;
  // permanently add;
  if(pTRDHTrack_[itr]!=NULL)
    AMSEvent::gethead()->addnext(AMSID("AMSTRDHTrack",itr),pTRDHTrack_[itr]);
}

int check_TOF(AMSTRDHSegment* s1,AMSTRDHSegment* s2 ,int debug){
  AMSTRDHTrack* tr=SegToTrack(s1,s2);
  if(!tr){
    if(debug>0)printf("No track found / Segments not compatible\n");
    return 0;
  }
  
  vector<AMSTOFCluster*> tcls;
  for(AMSTOFCluster* Hi=(AMSTOFCluster*)AMSEvent::gethead()->getheadC("AMSTOFCluster",0);Hi;Hi=Hi->next()) tcls.push_back(Hi);
  for(AMSTOFCluster* Hi=(AMSTOFCluster*)AMSEvent::gethead()->getheadC("AMSTOFCluster",1);Hi;Hi=Hi->next()) tcls.push_back(Hi);
  for(AMSTOFCluster* Hi=(AMSTOFCluster*)AMSEvent::gethead()->getheadC("AMSTOFCluster",2);Hi;Hi=Hi->next()) tcls.push_back(Hi);
  for(AMSTOFCluster* Hi=(AMSTOFCluster*)AMSEvent::gethead()->getheadC("AMSTOFCluster",3);Hi;Hi=Hi->next()) tcls.push_back(Hi);


  int toflay_ok[2]={0,0};
  for(int i=0;i!=tcls.size();i++){
    AMSTOFCluster* tcl=tcls[i];
    if(tcl->getplane()>2)continue;

    float dz=(tcl->getcoo()[2]-tr->pos[2]);

    float pos_x = tr->pos[0]+tr->dir[0]/tr->dir[2]*dz;
    float pos_y = tr->pos[1]+tr->dir[1]/tr->dir[2]*dz;

    float ex   = fabs(dz)*tr->emx;
    float ey   = fabs(dz)*tr->emy;

    float sig_x = (pos_x-tcl->getcoo()[0])/(tcl->getnmemb()*5.5+ex); 
    float sig_y = (pos_y-tcl->getcoo()[1])/(tcl->getnmemb()*5.5+ey);// (pos_y-tcl->getcoo()[1])/ey;
    
    if(debug>0){
      printf("x - tcl %.2f expos %.2f err %.2f sig %.2f\n",tcl->getcoo()[0],pos_x,ex, sig_x);
      printf("y - tcl %.2f expos %.2f err %.2f sig %.2f\n",tcl->getcoo()[1],pos_y,ey, sig_y);
      printf("TofCluster layer %i found - dx %.2f dy %.2f sigma ok ? %i\n",tcl->getplane(),sig_x,sig_y,fabs(sig_x)<=2&&fabs(sig_y)<=2);
    }
    if(fabs(sig_x)<=2&&fabs(sig_y)<=2) toflay_ok[tcl->getplane()-1]++;
  }

  if(toflay_ok[0]==0){
    if(debug>0)printf("TOF Layer 1 has no hits matching to track\n");
    return -1;
  }
  else if(toflay_ok[1]==0){
    if(debug>0)printf("TOF Layer 2 has no hits matching to track\n");
    return -2;
  }
  else return 1;
}


bool check_TRtop(AMSTRDHSegment* s1,AMSTRDHSegment* s2 ,int debug){
  AMSTRDHTrack* tr=SegToTrack(s1,s2);
  if(!tr){
    if(debug>0)printf("No track found / Segments not compatible\n");
    return 0;
  }

  //  if(debug>0)printf("TR rec hits %i cluster %in",pev->NTrRecHit(),pev->NTrCluster());

  AMSTrRecHit *ttr;int i;
  for (i=0;i<2;i++) {
    for (ttr=AMSTrRecHit::gethead(i); ttr!=NULL; ttr=ttr->next()){
      if(ttr->getHit()[2]<50)continue;
      if(debug>0)printf("ttr %i x %.2f y %.2f z %.2f sum %.2f\n",i,ttr->getHit()[0],ttr->getHit()[1],ttr->getHit()[2],ttr->getsum());
      
      float dz=(ttr->getHit()[2]-tr->pos[2]);
      float pos_y = tr->pos[1]+tr->dir[1]/tr->dir[2]*dz;
      float ey   = fabs(dz)*tr->emy;
      float sigma=(pos_y-ttr->getHit()[1])/ey;
      //    if(debug)printf("expos y %.2f resid %.2f sigma %.2f\n",pos_y,pos_y-ttr->getHit()[1],);
      if(sigma<2){return 1;}
    }
  }

  return 0;
}

