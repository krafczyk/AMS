#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
#include "VCon.h"
#include "root.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "trdsim.h"
#include "tofrec02.h"
#include "event.h"
#include "commons.h"
#include "trdid.h"
#endif
#include "timeid.h"
#include "amsdbc.h"
#include "TF1.h"

#ifndef _PGTRACK_
#ifdef __ROOTSHAREDLIBRARY__
#include "commonsi.h"
#endif
#endif

#ifdef __MLD__
int TrdRawHitR::num=0;
int TrdHSegmentR::num=0;
int TrdHTrackR::num=0;
int TrdRawHitR::numa=0;
int TrdHSegmentR::numa=0;
int TrdHTrackR::numa=0;
#endif

ClassImp(TrdHReconR)

  TrdHReconR* TrdHReconR::_trdhrecon[maxtrdhrecon]=
{0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0};

TrdHTrackR *TrdHReconR::SegToTrack(int is1, int is2, int debug){
#ifndef __ROOTSHAREDLIBRARY__
  TrdHSegmentR *s1=0,*s2=0;
  VCon* cont=GetVCon()->GetCont("AMSTRDHSegment");
  if(cont){
    s1=(TrdHSegmentR*)cont->getelem(is1);
    s2=(TrdHSegmentR*)cont->getelem(is2);
    delete cont;
  }
  else return 0;
#else
  TrdHSegmentR* s1=&hsegvec.at(is1);
  TrdHSegmentR* s2=&hsegvec.at(is2);
#endif

  if(debug)printf("Entering TrdHReconR::SegToTrack s1 %d (i %i) s2 %d (i %i)\n",s1,is1,s2,is2);
  if(!s1||!s2)return 0;
  if(s1->d+s2->d!=1)return 0;
  if(isnan(s1->m)||isnan(s2->m))return 0;
  if(debug)printf("segments seem to be ok\n");

  float m[2]={0.,0.};
  float em[2]={0.,0.};
  float r[2]={0.,0.};
  float er[2]={0.,0.};
  float z[2]={0.,0.};
  
  em[s1->d]=s1->em;m[s1->d]=s1->m;r[s1->d]=s1->r;er[s1->d]=s1->er;z[s1->d]=s1->z;
  em[s2->d]=s2->em;m[s2->d]=s2->m;r[s2->d]=s2->r;er[s2->d]=s2->er;z[s2->d]=s2->z;
  
  if(debug)for(int d=0;d!=2;d++)
    printf("d %i m %.2f em %.2f r %.2f z %.2f\n",d,m[d],em[d],r[d],z[d]);

  // same center of segments
  float zcent=(s1->z * (float) s1->fTrdRawHit.size() + s2->z * (float) s2->fTrdRawHit.size()) /
    ((float) s1->fTrdRawHit.size() + (float) s2->fTrdRawHit.size());
  
  for(int d=0;d!=2;d++)
    r[d]=r[d] + m[d] * ( zcent - z[d] );

  float Dir[3],Coo[3];
  Dir[0]=-m[0];Dir[1]=-m[1],Dir[2]=-1.;
  Coo[0]=r[0];Coo[1]=r[1],Coo[2]=zcent;

  if(debug)printf("built track Coo %.2f %.2f %.2f mx %.2f my %.2f\n",Coo[0],Coo[1],Coo[2],Dir[0]/Dir[2],Dir[1]/Dir[2]);


  TrdHTrackR *tr=new TrdHTrackR(Coo,Dir);
  tr->fTrdHSegment.clear();
  tr->segments.clear();
  if(s1->d==0){
    tr->fTrdHSegment.push_back(is1);
    tr->fTrdHSegment.push_back(is2);
    tr->segments.push_back(*s1);
    tr->segments.push_back(*s2);
  }
  else{
    tr->fTrdHSegment.push_back(is2);
    tr->fTrdHSegment.push_back(is1);
    tr->segments.push_back(*s2);
    tr->segments.push_back(*s1);
  }

  tr->Nhits=(int)s1->fTrdRawHit.size()+(int)s2->fTrdRawHit.size();
  tr->Chi2=s1->Chi2+s2->Chi2; 
  
  if(debug)printf("Leaving SegToTrack - return AMSTRDHTrack\n");
  return tr;
}

int TrdHReconR::DoPrefit(int debug){
  TH2V H2V_mvrx("x", "x", 314,-105.,105., 119, -3., 3.);
  TH2V H2V_mvry("y", "y", 314,-105.,105., 119, -3., 3.);

  int pairs_x=0,pairs_y=0;;
  for(int i=0;i<rhits.size();i++){
    if(rhits.at(i).Amp<tracking_min_amp)continue;
    TRDHitRZD rzdi=TRDHitRZD(rhits.at(i));
    for(int j=i+1;j<rhits.size();j++){
      if(rhits.at(j).Amp<tracking_min_amp)continue;
      TRDHitRZD rzdj=TRDHitRZD(rhits.at(j));
      
      if(rzdi.d==rzdj.d && rzdi.z!=rzdj.z){
	float s = (rzdi.r-rzdj.r)/(rzdi.z-rzdj.z); //slope
	float o = rzdi.r - (rzdi.z-TRDz0)*s; //offset
	float z = (rzdi.z + rzdj.z)/2.;
	
	if(rzdj.d==0){H2V_mvrx.Fill(o,s,rzdi.z,rzdj.z,1.);pairs_x++;}
	else if(rzdj.d==1){H2V_mvry.Fill(o,s,rzdi.z,rzdj.z,1.);pairs_y++;}
      }
    }
  }
  
  if(debug)printf("pairs x %i y %i\n",pairs_x,pairs_y);
  if(pairs_x<5&&pairs_y<5)return -1;
  
  vector<PeakXYZW> peakvec[2];
  
  int numpeaks=0;
  for(int d=0;d<2;d++){
    vector<BIN> maxima;maxima.clear();
    TH2V *hist=0;
    if(d==0)hist=&H2V_mvrx;
    if(d==1)hist=&H2V_mvry;
    
    if(debug)printf("findpeaks d %i histo size %i integral %.2f\n",d,(int)hist->bins.size(),hist->Integral());
    if(!hist||hist->bins.size()==0)continue;
    
    for(vector<BIN>::iterator i=hist->bins.begin(); i!=hist->bins.end(); i++){

      // suppress number of segments for events lots of hits
      if(i->c<2)continue;//1+log(hist->Integral()))continue;

      // check distance to known maxima - check in 3x3 box around maximum
      int cb=-1;
      int biter=0;
      for(vector<BIN>::iterator b=maxima.begin();b!=maxima.end();b++,biter++)if(fabs(i->x-b->x)<=1&&fabs(i->y-b->y)<=1){cb=biter;break;}
      
      // if maximum already existing (cb>-1) check to overwrite else new maximum
      if(cb==-1)maxima.push_back(*i);
      else if( i->c > maxima[cb].c)maxima[cb]=*i;
    }

    // check if final maxima are too close to each other (3x3 box again)
    for(vector<BIN>::iterator i=maxima.begin();i!=maxima.end();i++){
      int jiter=0;
      for(vector<BIN>::iterator j=i+1;j!=maxima.end();j++,jiter++){
	if(fabs(i->x-j->x)<=1&&fabs(i->y-j->y)<=1){
	  if(maxima[jiter].c<=i->c)maxima[jiter].c=0;
	  else i->c=0;
	}
      }
    }
    
    if(maxima.size()==0)continue;

    // sort maxima in decreasing c (entries) order
    sort(maxima.begin(),maxima.end());
    
    if(debug)printf("found %i maxima:\n",(int)maxima.size());
    int miter1=0;
    if(debug)for(vector<BIN>::iterator i=maxima.begin();i!=maxima.end();i++,miter1++){
      float x=0.,y=0.;
      x=(hist->Xlo+(hist->Xup-hist->Xlo)*((float)i->x+0.5)/(float)hist->nbx);
      y=(hist->Ylo+(hist->Yup-hist->Ylo)*((float)i->y+0.5)/(float)hist->nby);
      printf("max %i s %.2f o %.2f z %.2f c %.2f\n",miter1,x,y,i->z,i->c);
      if(i->c<=0.)printf("ERROR maximum size <= 0!!\n");
    }
    if(numpeaks>0)while(maxima.size()>numpeaks)maxima.pop_back();
    else while(maxima.size()>1&&maxima.back().c<5)maxima.pop_back();
    
    float width=1.;
    int miter2=0;
    for(vector<BIN>::iterator j=maxima.begin();j!=maxima.end();j++,miter2++){
      float w=0.0, sx=0.0, sy=0.0, sz=0.0,sw=0.0,zmin=200.0,zmax=0.0,sc=0.0;
      for(vector<BIN>::iterator b=hist->bins.begin();b!=hist->bins.end();b++){
	if( fabs(b->x-j->x)<=width &&
	    fabs(b->y-j->y)<=width   ){
	  
	  w  =  (float)b->c * (b->zmax-b->zmin);
	  sx += w * (hist->Xlo+(hist->Xup-hist->Xlo)*((float)b->x+0.5)/(float)hist->nbx);
	  sy += w * (hist->Ylo+(hist->Yup-hist->Ylo)*((float)b->y+0.5)/(float)hist->nby);
	  sz += w * b->z/(float)b->c;
	  sw += w;
	  if(b->zmin<zmin)zmin=b->zmin;
	  if(b->zmax>zmax)zmax=b->zmax;
	}
      }

      if(debug)printf("maxima %i sw %.2f dz %.2f\n",miter2,sw,zmax-zmin);
      if(sw==0.||zmax-zmin==0.)continue;
      //      if((peakvec[d].size()>0&&sw<300)||zmax-zmin<8.)continue;
      if(sw<15||zmax-zmin<8.)continue;
      float prop_offset=sx/sw+(sz/sw-TRDz0)*sy/sw;

      PeakXYZW peak(prop_offset,sy/sw,sz/sw,zmin,zmax,sw);
      
      int found=0;
      for(vector<PeakXYZW>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++)if(peak==(*it)){found=1;break;}
      
      if(!found){
	peakvec[d].push_back(peak);
	sort(peakvec[d].begin(),peakvec[d].end());
      }
    }
  }
  
  if(debug)printf("DoPrefit segx %i segy %i\n",(int)peakvec[0].size(),(int)peakvec[1].size());
  
  int prefit=peakvec[0].size()+peakvec[1].size();
  while(prefit>100){
    cerr<<"TrdHRecon::DoPrefit - too many prefit candidates x "<<peakvec[0].size()<<" y "<<peakvec[1].size()<<endl;
    if(peakvec[0].size()>peakvec[1].size())peakvec[0].pop_back();
    else if(peakvec[1].size()>peakvec[0].size())peakvec[1].pop_back();
    else{
      if(peakvec[0].size()>10)peakvec[0].pop_back();
      if(peakvec[1].size()>10)peakvec[1].pop_back();
    }
    prefit=peakvec[0].size()+peakvec[1].size();
  }
  
  if(prefit<=0||prefit>100){
    if(debug)printf("skipping \n");
    return -2;
  }

  if(debug>0)printf("Entering DoLinReg\n");

  int nrh=rhits.size();
  if(debug>0)printf("Total number of hits %i\n",nrh);
  
  int i=0;
  for(int d=0;d<2;d++){
    for(vector<PeakXYZW>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++,i++){
      TrdHSegmentR iseg= TrdHSegmentR(d,it->y,0.,it->x,0.,it->z,it->w);
      TrdHSegmentR *seg=&iseg;
      seg->fTrdRawHit.clear();
      seg->em=0.6/(it->zmax-it->zmin);
      
      for(int h=0;h!=rhits.size();h++){	
	if(rhits.at(h).Amp<tracking_min_amp)continue;
	TRDHitRZD rzd=TRDHitRZD(rhits.at(h));
	if(rzd.d!=d)continue;
	
	float dz=rzd.z-seg->z;
	float expos=seg->r+ seg->m*dz;//*cos(atan(seg->m));
	float resid=rzd.r - expos;
	float maxresid=0.6+fabs(dz)* seg->em;
	if(debug>0)printf("d %i hit %i r %.2f expos %.2f resid %f < %f ok %i\n",d,h,rzd.r,expos,resid,maxresid,fabs(resid)<maxresid);
	
	if(fabs(resid)<maxresid)
	  seg->AddHit(rhits.at(h),h);
      }
      
      if(debug>0){
	printf("seg %i m %.2f r %.2f z %.2f hits %i\n",i,seg->m,seg->r,seg->z,(int)seg->fTrdRawHit.size());
      }
      
      int nlr=0;
      int lr=1;
      while(lr>=0){
	lr=seg->LinReg(debug);
	if(!lr)return 0;
	nlr++;
	int n0=seg->fTrdRawHit.size();
	seg->fTrdRawHit.clear();	
	seg->hits.clear();	
	
	for(int h=0;h!=rhits.size();h++){	
	  if(rhits.at(h).Amp<tracking_min_amp)continue;
	  TRDHitRZD rzd=TRDHitRZD(rhits.at(h));
	  if(rzd.d!=d)continue;
	  
	  float dz=rzd.z-seg->z;
	  float resid=(rzd.r - (seg->r+ seg->m*dz));//*cos(atan(seg->m));
	  float maxresid=0.6+fabs(dz)*seg->em;//0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
	  
	  if(debug>0)printf("Lin Reg residuals %.2f < %.2f %i (dz %.2f em %.2e)\n",resid,maxresid,fabs(resid)<maxresid,rzd.z-seg->z,seg->em);
	  if(fabs(resid)<maxresid)
	    seg->AddHit(rhits.at(h),h);
	  
	}
	
	if(n0==(int)seg->fTrdRawHit.size()){
	  if(debug)printf("no change in number of hits - breaking loop\n");
	  break;
	}
	else if(nlr>3&&seg->fTrdRawHit.size()>0){
	  if(debug)printf("nlr %i - more hits than in last iteration %i>%i - breaking loop\n",nlr,(int)seg->fTrdRawHit.size(),n0);
	  break;
	}
	if(debug)printf("d %i n0 %i n1 %i - refitting\n",d,n0,(int)seg->fTrdRawHit.size());
      }
      if(debug)printf("number of linreg %i\n",nlr);
      if(seg->fTrdRawHit.size()>3){
	seg->calChi2();
	
	if(debug>0){
	  printf("d %i after linreg loop: segment %i hits %i\n",d,i,(int)seg->fTrdRawHit.size());
	  printf("After linreg: seg %i d %i m %.2f r %.2f z %.2f hits %i\n",i,d,seg->m,seg->r,seg->z,(int)seg->fTrdRawHit.size());
	}
	
	//	AddSegment(seg);
	hsegvec.push_back(*seg);
      }
            //if(seg)delete seg;
    }
  }

  return 1;
}
  
int TrdHReconR::DoLinReg(int debug){
  if(debug>0)printf("Entering DoLinReg\n");
  return 0;
}

 
int TrdHReconR::clean_segvec(int debug){
  if(debug>0){
    printf("Enter clean_segvec\n");
    printf("before clean %i\n",(int)hsegvec.size());
    int n=0;
    for(vector<TrdHSegmentR>::iterator s1=hsegvec.begin();s1!=hsegvec.end();s1++,n++){
      printf("segment %i d %i n %i m %.2f r %.2f z %.2f Chi2 %.2e\n",
	     n,s1->d,(int)s1->fTrdRawHit.size(),s1->m,s1->r,s1->z,s1->Chi2);
    }
  }

  bool keepseg[hsegvec.size()];
  for(int i=0;i!=hsegvec.size();i++)keepseg[i]=1;

  int c1=0;
  for(vector<TrdHSegmentR>::iterator s1=hsegvec.begin();s1!=hsegvec.end();s1++,c1++){
    if(hsegvec.size()>2&&s1->Chi2/((float)s1->nTrdRawHit()-2)>6){
      if(debug>0)printf("removing segment %i because of high Chi2/ndof %f\n",c1,s1->Chi2/((float)s1->fTrdRawHit.size()-2));
      keepseg[c1]=0;
    }
    if(keepseg[c1]==0)continue;
    
    int c2=c1+1;
    for(vector<TrdHSegmentR>::iterator s2=s1+1;s2!=hsegvec.end();s2++,c2++){
      if(s1->d!=s2->d)continue;
      if(keepseg[c2]==0)continue;
      
      int nboth=0;
      int n1=s1->nTrdRawHit(),n2=s2->nTrdRawHit();

      int hiter1[n1],hiter2[n2];
      for(int hit1=0;hit1<n1;hit1++)hiter1[hit1]=0;
      for(int hit2=0;hit2<n2;hit2++)hiter2[hit2]=0;
      
      for(int hit1=0;hit1<n1;hit1++){
	TRDHitRZD rzd1=TRDHitRZD(s1->hits.at(hit1)); 
	for(int hit2=0;hit2<n2;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(s2->hits.at(hit2)); 
	  
	  if(debug>1) printf("hit1 %i r %.2f z %.2f hit2 %i r %.2f z %.2f both %i\n",hit1,rzd1.r,rzd1.z,hit2,rzd2.r,rzd2.z,rzd1.r == rzd2.r && rzd1.z == rzd2.z);
	  if( rzd1.r == rzd2.r &&
	      rzd1.z == rzd2.z){
	    hiter1[hit1]=1;hiter2[hit2]=1;
	    nboth++;
	  }
	}
      }
      
      if(debug>0)printf("d %i s1 %i n1 %i m1 %.2f Chi2/ndof %.2e s2 %i n2 %i m2 %.2f Chi2/ndof %.2e nboth %i\n",s1->d,c1,n1,s1->m,s1->Chi2/((float)s1->fTrdRawHit.size()-2.),c2,n2,s2->m,s2->Chi2/((float)s2->fTrdRawHit.size()-2.),nboth);
      
      if(n1<=2){keepseg[c1]=0;}
      if(n2<=2){keepseg[c2]=0;}
      if(keepseg[c1]==0||keepseg[c2]==0)continue;

      float exp1=s1->r+s1->m*(113.5-s1->z);
      float exp2=s2->r+s2->m*(113.5-s2->z);
      
      if((n1==n2&&nboth==n2)||((s1->d==s2->d)&&(fabs(exp1-exp2)<0.01)&&fabs(s1->r-s2->r)<0.01)){
	if(debug>0)printf("same segments\n");
	if (s1->Chi2==0||s1->nTrdRawHit()<3)keepseg[c1]=0;
	else if(s2->Chi2==0.||s2->nTrdRawHit()<3)keepseg[c2]=0;
	else if(s1->Chi2/((float)s1->nTrdRawHit()-2.) > s2->Chi2/((float)s2->nTrdRawHit()-2.))keepseg[c1]=0;
	else keepseg[c2]=0;
	continue;
      }
      else if(nboth>1){
	if(debug>0)printf("nboth %i > 1 !\n",nboth);
	float s1_Chi2ndof=s1->Chi2/((float)s1->nTrdRawHit()-2.);
	float s2_Chi2ndof=s1->Chi2/((float)s2->nTrdRawHit()-2.);
	
	if(s2_Chi2ndof==0.)keepseg[c2]=0;
	else if(s1_Chi2ndof==0.)keepseg[c1]=0;
	else if(s2_Chi2ndof>s1_Chi2ndof){
	  if(debug>0)printf("Refit s2\n");
	  for(int i=n2-1;i>=0;i--) if(hiter2[i]==1){
	    s2->RemoveHit(i);
	  }

	  if(s2->nTrdRawHit()>2){
	    int ok=hsegvec.at(c2).Refit(debug);
	    if(!ok) keepseg[c2]=0;
	  }
	  else keepseg[c2]=0;
	}
	else {
	  if(debug)printf("Refit s1\n");
	  for(int i=n1-1;i>=0;i--) if(hiter1[i]==1){
	    s1->RemoveHit(i);
	  }

	  if(s1->nTrdRawHit()>2){
	    int ok=(hsegvec.at(c1)).Refit(debug);
	    if(!ok) keepseg[c1]=0;
	  }
	  else keepseg[c1]=0;
	}
      }      
    }
  } 

  for(int i=hsegvec.size()-1;i>=0;i--)
    if(keepseg[i]==0){
      hsegvec.erase(hsegvec.begin()+i);
    }

  if(debug>0){
    printf ("REMAINING SEGMENTS:\n");
    int n=0;
    for(vector<TrdHSegmentR>::iterator i=hsegvec.begin();i!=hsegvec.end();i++,n++)
      printf("d %i s %i n %i m %.2f r %.2f Chi2 %.2f nhits %i \n",i->d,n,i->nTrdRawHit(),i->m,i->r,i->Chi2,i->nTrdRawHit());
    
    printf("after clean %i\n",(int)hsegvec.size());
    printf("Exiting clean_hsegvec\n");
  } 
  return hsegvec.size();
}


bool TrdHReconR::check_hits(int is1,int is2,int debug){
  TrdHTrackR *tr=SegToTrack(is1,is2);
  if(!tr){
    if(debug>0)printf("No track found / Segments not compatible\n");
    return 0;
  }

  int nref=refhits.size();

  int hit_ok[nref];
  int nhitsok=0;
  for(int i=0;i!=nref;i++){
    hit_ok[i]=0;
    
    float dz=(refhits[i].z() - tr->Coo[2]);

    float pos_x = tr->Coo[0]+tr->Dir[0]/tr->Dir[2]*dz;
    float pos_y = tr->Coo[1]+tr->Dir[1]/tr->Dir[2]*dz;

    float ex   = fabs(dz)*tr->emx();
    float ey   = fabs(dz)*tr->emy();

    float sig_x = (pos_x-refhits[i].x())/(referr[i].x()+ex); 
    float sig_y = (pos_y-refhits[i].y())/(referr[i].y()+ey);
    
    if(debug>0){
      printf("x - hit %i %.2f expos %.2f err %.2f sig %.2f\n",i,refhits[i].x(),pos_x,ex, sig_x);
      printf("y - hit %i %.2f expos %.2f err %.2f sig %.2f\n",i,refhits[i].y(),pos_y,ey, sig_y);
      printf("Hit found - dx %.2f dy %.2f sigma ok ? %i\n",sig_x,sig_y,fabs(sig_x)<=2&&fabs(sig_y)<=2);
    }
    if(fabs(sig_x)<=2&&fabs(sig_y)<=2){
      hit_ok[i]=1;
      nhitsok++;
    }
  }

  if(debug)printf("return - all hits found? %i\n",nhitsok==nref);
  if(tr)delete tr;
  return (nhitsok==nref);
}

vector<pair<int,int> > TrdHReconR::check_secondaries(int debug){
  vector<pair<int, int> > toReturn;toReturn.clear();

  // rough division of TRD height in dm (0=0-10 cm, 6=60-70 cm)
  int zvec[7];for(int i=0;i!=7;i++)zvec[i]=0;

  for(int s1=0;s1!=hsegvec.size();s1++){
    for(int s2=s1+1;s2!=hsegvec.size();s2++){
      if(hsegvec.at(s1).d==hsegvec.at(s2).d){

	// check for z of crossing
	float z_cross = (hsegvec.at(s2).r - hsegvec.at(s1).r - hsegvec.at(s2).m * hsegvec.at(s2).z + 
			 hsegvec.at(s1).m * hsegvec.at(s1).z) / ( hsegvec.at(s1).m - hsegvec.at(s2).m );
	
	if(z_cross>150||z_cross < 80)continue;
	zvec[(int)floor((z_cross-80.)/10.)]++;;
	if(hsegvec.at(s1).d==0&&(z_cross>120||z_cross<102))continue;

	float s1zmax=0.,s1zmin=200.;
	for(int hit1=0;hit1!=hsegvec.at(s1).fTrdRawHit.size();hit1++){
	  TRDHitRZD rzd1=TRDHitRZD(*(hsegvec.at(s1)).pTrdRawHit(hit1));
	  
	  if(rzd1.z>s1zmax)s1zmax=rzd1.z;
	  if(rzd1.z<s1zmin)s1zmin=rzd1.z;
	}
	float s2zmax=0.,s2zmin=200.;
	for(int hit2=0;hit2!=hsegvec.at(s2).fTrdRawHit.size();hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(*hsegvec.at(s2).pTrdRawHit(hit2));

	  if(rzd2.z>s2zmax)s1zmax=rzd2.z;
	  if(rzd2.z<s2zmin)s1zmin=rzd2.z;
	}

	if(s1zmax<z_cross+3){
	  if(debug)printf("vertex z %.2f s1 downward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s2,-s1));
	}
	else if(s2zmax<z_cross+3){
	  if(debug)printf("vertex z %.2f s2 downward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s1,-s2));
	}
	else if(s1zmin>z_cross-3){
	  if(debug)printf("vertex z %.2f s1 upward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s2,s1));
	}
	else if(s2zmin>z_cross-3){
	  if(debug)printf("vertex z %.2f s2 upward\n",z_cross);
	  toReturn.push_back(make_pair<int, int>(s1,s2));
	}
      }
    }
  }

  for(int i=0;i!=7;i++)
    if(zvec[i]>4)toReturn.push_back(make_pair<int,int>(99,99));

  return toReturn;
}

bool TrdHReconR::check_geometry(int is1, int is2){
  // idea is to calculate if track candidate leaves TRD but has hits "outside" TRD volume
  // for this define TRD contours as a function of z and compare to highest/ lowest hitz

  TrdHTrackR *tr=SegToTrack(is1, is2);
  if(!tr)return 0;

  if(hsegvec[is1].d==0){
    tr->fTrdHSegment[hsegvec[is1].d]=is1;
    tr->fTrdHSegment[hsegvec[is2].d]=is2;
  }
  else{
    tr->fTrdHSegment[hsegvec[is2].d]=is2;
    tr->fTrdHSegment[hsegvec[is1].d]=is1;
  }

  for(int i=0;i!=hsegvec.at(is1).nTrdRawHit();i++){
    TRDHitRZD rzd=TRDHitRZD(*hsegvec.at(is1).pTrdRawHit(i));
    float x=0.,y=0.;
    tr->propagateToZ(rzd.z,x,y);
    
    float base=77.+23.*(rzd.z-82.)/60.;
    float side=32.+10.*(rzd.z-82.)/60.;
    float line=72.+20.5*(rzd.z-82.)/60.;
    float corner=base+side;

    if(fabs(x)>base || fabs(y)>base || fabs(y)>line ||fabs(x)+fabs(y)>corner){
      if(tr)delete tr;  
      return 0;
    } 
  }
  for(int i=0;i!=hsegvec.at(is2).nTrdRawHit();i++){
    TRDHitRZD rzd=TRDHitRZD(hsegvec.at(is2).hits.at(i));
    float x=0.,y=0.;
    tr->propagateToZ(rzd.z,x,y);
    
    float base=77.+23.*(rzd.z-82.)/60.;
    float side=32.+10.*(rzd.z-82.)/60.;
    float line=72.+20.5*(rzd.z-82.)/60.;
    float corner=base+side;
    
    if(fabs(x)>base || fabs(y)>base || fabs(y)>line ||fabs(x)+fabs(y)>corner){
      if(tr)delete tr;  
      return 0;
    } 
  }
  if(tr)delete tr;
  return 1;
}

int TrdHReconR::combine_segments(int debug){
  bool s_done[hsegvec.size()];
  bool s_poss[hsegvec.size()][hsegvec.size()];
  
  for(int i=0;i!=hsegvec.size();i++){
    s_done[i]=0;
    for(int j=0;j!=hsegvec.size();j++){
      if(j==i||hsegvec[i].d==hsegvec[j].d){s_poss[i][j]=0;s_poss[j][i]=0;}
      else {s_poss[i][j]=1;s_poss[j][i]=1;}
    }
  }
  
  for(int i=0;i!=hsegvec.size();i++){
    for(int j=i+1;j!=hsegvec.size();j++){
      if(s_poss[i][j]==0)continue;

      if(hsegvec[i].fTrdRawHit.size()<3){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_geometry(i,j)==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_hits(i,j)==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
    }
  }
  
  int n_poss[hsegvec.size()];
  for(int i=0;i!=hsegvec.size();i++){
    if(s_done[i]==1)continue;
    
    n_poss[i]=0;
    for(int j=0;j!=hsegvec.size();j++)if(s_poss[i][j]==1)n_poss[i]++;
    
    else if(n_poss[i]==1){
      for(int j=0;j!=hsegvec.size();j++){
	if(s_poss[i][j]!=1)continue;
	if(s_done[i]==1||s_done[j]==1)continue;
	TrdHTrackR *tr=SegToTrack(i,j);
	if(tr){
	  tr->status=1;
	  //	  htrvec.push_back(tr);
	  AddTrack(tr);
	  delete tr;
	}
      }
    }
  }

  int n_done=0;
  for(int i=0;i!=hsegvec.size();i++)if(s_done[i]==1)n_done++;
  if(hsegvec.size()==n_done)return htrvec.size();

  // check for vertices
  /*  vector<pair<int,int> > check=check_secondaries();
      
  if(check.size()>0){
  // skip secondary vertices for now
  for(int i=0;i!=check.size();i++){
  int s=abs(check[i].second);
  s_done[s]=1;
  if(check[i].second==99&&check[i].first==99){
  if(debug) printf("\nHADRONIC INTERACTION IN TRD\n");
  //	save_ev=1;
  return nhtrvec;
  }
  }
  }
  */

  n_done=0;
  for(int i=0;i!=hsegvec.size();i++)if(s_done[i]==1)n_done++;

  if(debug)printf("%i segment(s) not done after sec. search \n",(int)hsegvec.size()-n_done);

  return htrvec.size();
}



void TrdHReconR::ReadTRDEvent(vector<TrdRawHitR> r, vector<TrdHSegmentR> s, vector<TrdHTrackR> t){
  rhits.clear();
  gbhits.clear();
  hsegvec.clear();
  htrvec.clear();

  for(int i=0;i!=r.size();i++){
    rhits.push_back(r[i]);
  }
  for(int i=0;i!=s.size();i++)hsegvec.push_back(s[i]);  
  for(int i=0;i!=t.size();i++)htrvec.push_back(t[i]);
}

void TrdHReconR::BuildTRDEvent(vector<TrdRawHitR> r, int debug ){
  rhits.clear();
  gbhits.clear();
  htrvec.clear();
  hsegvec.clear();
  refhits.clear();
  referr.clear();

  //  clear();
  if(r.size()<3)return;
  for(int n=0;n<r.size();n++)
    AddHit(&r[n]);

  retrdhevent(debug);
}

int TrdHReconR::build(){
  rhits.clear();
  gbhits.clear();
  hsegvec.clear();
  htrvec.clear();
  refhits.clear();
  referr.clear();

#ifndef __ROOTSHAREDLIBRARY__
  // fill reference hits - first TOF clusters (top layer) - then TKtop clusters
  AMSTOFCluster *Hi;int i;
  for (i=0;i<4;i++) {
    for (Hi=AMSTOFCluster::gethead(i); Hi!=NULL; Hi=Hi->next()){
      if(Hi->getntof()<3){
	refhits.push_back(Hi->getcoo());
	referr.push_back(Hi->getecoo());}	
    }
  }

  // no TR hits at this stage of reco
  /*
  AMSTrRecHit *ttr;
  for (i=0;i<2;i++) {
    for (ttr=AMSTrRecHit::gethead(i); ttr!=NULL; ttr=ttr->next()){
      int lay=-1;float z=0.;
#ifndef _PGTRACK_
      lay=ttr->getLayer();
      z=ttr->getHit()[2];
#else
      lay=ttr->GetLayer();
      z=ttr->GetCoord()[2];
      
#endif
      printf("lay %i z %.2f\n",lay,z);
#ifndef _PGTRACK_
      refhits.push_back(ttr->getHit());
      referr.push_back(ttr->getEHit());
#else
      refhits.push_back(ttr->GetCoord());
      referr.push_back(ttr->GetECoord());
#endif
    }
  }
  */
  //  for(int i=0;i<refhits.size();i++)
  //    printf("refhit %i - %.2f %.2f %.2f\n",i,refhits[i][0],refhits[i][1],refhits[i][2]);
  
  // fill array of TrdRawHits
  VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
  for(int s=0;s<2;s++)
    for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",s);Hi;Hi=Hi->next()){
      gbhits.push_back(Hi);
      rhits.push_back(Hi);
    }
  delete cont2;
  
  retrdhevent();
 
  return 1;
#else
  return 0;
#endif
    
}    

void TrdHReconR::clear(){
#ifdef __ROOTSHAREDLIBRARY__
  VCon* conth=GetVCon()->GetCont("AMSTRDRawHit");
  if(conth){
    for(int i=0;i<conth->getnelem();i++)conth->removeEl(conth->getelem(i)-1);
    conth->eraseC();
  }
  delete conth;
  
  VCon* conts=GetVCon()->GetCont("AMSTRDHSegment");
  if(conts){
    for(int i=0;i<conts->getnelem();i++)conts->removeEl(conts->getelem(i)-1);
    conts->eraseC();
  }
  delete conts;
  
  VCon* contt=GetVCon()->GetCont("AMSTRDHTrack");
  if(contt){
    for(int i=0;i<contt->getnelem();i++)contt->removeEl(contt->getelem(i)-1);
    contt->eraseC();
  }
  delete contt;

  return;
#endif
}
int TrdHReconR::retrdhevent(int debug){
  // skip arrays for now
  //  int nhcut=1.e6; //  below 40 nTRDRawHits: vector-histo 
  // above                 array-histo

  //  double Hcut=tracking_min_amp;
  //  if(pev->nTRDRawHit()>100) Hcut=10.0;
  
  if(debug)printf("RawHits %i\n",(int)rhits.size());
  if(rhits.size()<4||rhits.size()>100) return 0;

  int prefit=DoPrefit(debug);

  if(prefit<0)return 0;
  clean_segvec(debug);

#ifndef __ROOTSHAREDLIBRARY__
  for(int i=0;i<hsegvec.size();i++)
    AddSegment((TrdHSegmentR*)&hsegvec.at(i));
#endif

  if(debug) printf("got %i segment(s)\n",(int)hsegvec.size());

  int nhseg=hsegvec.size();
  
  if(nhseg==2||nhseg==3){
    int segiter_sel[2]={-1,-1};
    
    for(int s=0;s!=nhseg;s++){
      int d=hsegvec.at(s).d;

      float chi2ndofs=1.e6;
      if(hsegvec.at(s).nTrdRawHit()>2)
	chi2ndofs=(float) hsegvec.at(s).Chi2 / (float) hsegvec.at(s).nTrdRawHit();
      
      if(segiter_sel[d]>-1){
	float chi2ndof=1.e6;
	if(hsegvec.at(segiter_sel[d]).nTrdRawHit()>2)
	  chi2ndof=(float) hsegvec.at(segiter_sel[d]).Chi2 / (float) hsegvec.at(segiter_sel[d]).nTrdRawHit();
	
	if(debug)printf("cand s %.2f < %.2f ?\n",chi2ndof,chi2ndofs);
	if( ( hsegvec.at(s).nTrdRawHit() >  hsegvec.at(segiter_sel[d]).nTrdRawHit()) ||
	    ((hsegvec.at(s).nTrdRawHit() == hsegvec.at(segiter_sel[d]).nTrdRawHit()) &&
	     (chi2ndof < chi2ndofs))) segiter_sel[d]=s;
      }
      else if(hsegvec.at(s).nTrdRawHit()>0)segiter_sel[d]=s;
      
      if(debug)printf("sel %i d %i nTrdRawHit() %i chi2ndof %.2f\n",s,d,hsegvec.at(segiter_sel[d]).nTrdRawHit(),chi2ndofs);
    }

    if(debug) printf("segiter %i %i\n",segiter_sel[0],segiter_sel[1]);
    if(segiter_sel[0]>-1&&segiter_sel[1]>-1){
      TrdHTrackR *tr=SegToTrack(segiter_sel[0],segiter_sel[1],debug);
      if(tr){
	tr->status=1;
	//	htrvec.push_back(tr);
	AddTrack(tr);
	delete tr;
      }
    }
  }
  else if(nhseg>2) combine_segments();

  if(debug)printf("TrdHTrackR::build tracks %i\n",(int)htrvec.size());
  
  return 1;
}

void TrdHReconR::AddTrack(TrdHTrackR* tr){
  if(tr){

    for(int i=0;i<20;i++)tr->elayer[i]=0.;
    for(int s=0;s<tr->fTrdHSegment.size();s++){
      for(int h=0;h<tr->pTrdHSegment(s)->fTrdRawHit.size();h++){
	tr->elayer[tr->pTrdHSegment(s)->hits.at(h).Layer]+=tr->pTrdHSegment(s)->hits.at(h).Amp;
      }
    }
   
#ifndef __ROOTSHAREDLIBRARY__
    AMSTRDHTrack *amstr=new AMSTRDHTrack(tr);
#else
    TrdHTrackR* amstr=tr;
#endif
    
    amstr->charge=TrdHChargeR::gethead()->GetCharge(amstr);
    amstr->elikelihood=TrdHChargeR::gethead()->GetELikelihood(amstr);

#ifndef __ROOTSHAREDLIBRARY__
    VCon* cont2=GetVCon()->GetCont("AMSTRDHTrack");
    if(amstr){
      cont2->addnext(amstr);
    }
    delete cont2;
#endif

    htrvec.push_back(*amstr);
  }
}
void TrdHReconR::AddSegment(TrdHSegmentR* seg){
  if(seg){
    VCon* cont2=GetVCon()->GetCont("AMSTRDHSegment");
#ifndef __ROOTSHAREDLIBRARY__
    for(int i=0;i<seg->nTrdRawHit();i++)
      seg->gbhits.push_back(gbhits[seg->fTrdRawHit[i]]);

    AMSTRDHSegment* amsseg=new AMSTRDHSegment(seg);
#else
    TrdHSegmentR* amsseg=seg;
#endif

    cont2->addnext(amsseg);
    //    hsegvec.push_back(*amsseg);
    delete cont2;
  }
}

void TrdHReconR::AddHit(TrdRawHitR* hit){
  if(hit){
    VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
    cont2->addnext(hit);
    rhits.push_back(*hit);
    delete cont2;
  }
}

int TrdHReconR::SelectTrack(TrdHTrackR *track){
  if(!track)return 0;

  int ntracklay[20];
  for(int i=0;i<20;i++)ntracklay[i]=0;
  
  // loop over hits on both segments and calculate number hits on track per layer
  for(int seg=0;seg<track->nTrdHSegment();seg++){
    if(!track->pTrdHSegment(seg)) continue;
    for(int i=0;i<(int)track->pTrdHSegment(seg)->fTrdRawHit.size();i++){
      TrdRawHitR* hit=track->pTrdHSegment(seg)->pTrdRawHit(i);
      if(!hit)continue;
      if(hit->Amp>10)ntracklay[hit->Layer]++;
    }
  }

  int ok[3]={0,0,0};

  for(int i=0;i<20;i++){
    if(ntracklay[i]>0){
      if(i<4)ok[0]++;
      else if(i<16)ok[1]++;
      else ok[2]++;
    }
  }

  //  if(ok[0]<2||ok[1]<8||ok[2]<2||fraction<0.8)return 0;
  if(ok[0]<2||ok[1]<8||ok[2]<2)return 0;
  else return 1;
}

int TrdHReconR::SelectEvent(int level){
  int nhit_ontrack=0;
  for(int tr=0;tr<(int)htrvec.size();tr++)
    for(int seg=0;seg<2;seg++){
      if(!htrvec[tr].pTrdHSegment(seg)) continue;
      for(int i=0;i<(int)htrvec[tr].pTrdHSegment(seg)->fTrdRawHit.size();i++){
	TrdRawHitR* hit=htrvec[tr].pTrdHSegment(seg)->pTrdRawHit(i);
	if(!hit)continue;
	if(hit->Amp>5)nhit_ontrack++;
      }
    }
  
  int nhit=0;
  for(int i=0;i!=rhits.size();i++){
    TrdRawHitR* rhit=&rhits.at(i);
    if(!rhit)continue;
    if(rhit->Amp>5)nhit++;
  }
  
  float fraction=float(nhit_ontrack)/float(nhit);
  if(!level&&fraction>0.5) return 1;
  else if(level==1&&fraction>0.5&&(int)hsegvec.size()==2&&(int)htrvec.size()==1)return 1;
  else return 0;
}


bool TrdHReconR::update_tdv_array(int debug){
  bool toReturn=false;
#ifndef __ROOTSHAREDLIBRARY__
  for(int i=0;i<5248;i++){
    if(TrdHCalibR::gethead()->tube_occupancy[i]>0){
      int layer,ladder,tube;
      TrdHCalibR::gethead()->GetLLTFromTubeId(layer,ladder,tube,i);
      AMSTRDIdGeom geom(layer,ladder,tube);
      AMSTRDIdSoft id(geom);
      if(!id.dead()){
	id.setgain()=TrdHCalibR::gethead()->tube_medians[i]/TRDMCFFKEY.GeV2ADC*1.e6;

	if(!toReturn&&TrdHCalibR::gethead()->tube_occupancy[i]>100)toReturn=true;
	if(debug){
#pragma omp critical (dbggains)
	  {
	    printf("LLT %02i%02i%02i id %i median %.2f = %.2f occ %i -> gain %.2f\n",layer,ladder,tube,id.getchannel(),id.getgain(),TrdHCalibR::gethead()->tube_medians[i],TrdHCalibR::gethead()->tube_occupancy[i],id.getgain());
	  }
	}
      }
    }
  }
#else
  cout<<"Entering TrdHReconR::update_tdv_array"<<endl;
  cout<<"Currently testing shared library version"<<endl;
  
#endif
  return toReturn;;
}
