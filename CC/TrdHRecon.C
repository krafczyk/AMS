#include "TrdHRecon.h"
#include "VCon.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#ifndef __ROOTSHAREDLIBRARY__
//#include "event.h"
#include "trdhrec.h"
#endif

ClassImp(TrdHReconR);

TrdHTrackR* TrdHReconR::SegToTrack(TrdHSegmentR *s1, TrdHSegmentR* s2, int debug){
  if(debug)printf("Entering TrdHReconR::SegToTrack\n");
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
  float zcent=(s1->z * (float) s1->Nhits + s2->z * (float) s2->Nhits) /
    ((float) s1->Nhits + (float) s2->Nhits);
  
  for(int d=0;d!=2;d++)
    r[d]=r[d] + m[d] * ( zcent - z[d] );

  float Dir[3],Coo[3];
  Dir[0]=-m[0];Dir[1]=-m[1],Dir[2]=-1.;
  Coo[0]=r[0];Coo[1]=r[1],Coo[2]=zcent;

  if(debug)printf("built track Coo %.2f %.2f %.2f mx %.2f my %.2f\n",Coo[0],Coo[1],Coo[2],Dir[0]/Dir[2],Dir[1]/Dir[2]);


  TrdHTrackR *tr=new TrdHTrackR(Coo,Dir);
  //  tr->Chi2=s1->Chi2+s2->Chi2;
  //  tr->Nhits=s1->Nhits+s2->Nhits;
  tr->SetSegments(s1,s2);
  
  if(debug)printf("Leaving SegToTrack - return AMSTRDHTrack\n");
  return tr;
}

int TrdHReconR::DoPrefit(int debug){
  TH2V H2V_mvrx("x", "x", 314,-105.,105., 119, -3., 3.);
  TH2V H2V_mvry("y", "y", 314,-105.,105., 119, -3., 3.);

  int pairs_x=0,pairs_y=0;;
  for(int i=0;i<nrhits;i++){
    TRDHitRZD rzdi=TRDHitRZD(*rhits[i]);
    for(int j=i+1;j<nrhits;j++){
      TRDHitRZD rzdj=TRDHitRZD(*rhits[j]);
      
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
  if(pairs_x<5&&pairs_y<5)return 0;

  vector<PeakXYZW*> peakvec[2];

  int numpeaks=0;
  for(int d=0;d<2;d++){
    vector<BIN> maxima;maxima.clear();
    TH2V *hist=0;
    if(d==0)hist=&H2V_mvrx;
    if(d==1)hist=&H2V_mvry;
    
    if(debug)printf("findpeaks d %i histo size %i\n",d,hist->bins.size());
    if(!hist||hist->bins.size()==0)continue;
    
    for(vector<BIN>::iterator i=hist->bins.begin(); i!=hist->bins.end(); i++){
      // suppress number of segments for events lot of hits (hadronic interaction)
      if(i->c<1+log(hist->bins.size()))continue;
      
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
    
    if(debug)printf("found %i maxima:\n",maxima.size());
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
      if(peakvec[d].size()>0&&(sw<300||zmax-zmin<10.))continue;
      float prop_offset=sx/sw+(sz/sw-TRDz0)*sy/sw;

      PeakXYZW *peak=new PeakXYZW(prop_offset,sy/sw,sz/sw,zmin,zmax,sw);
      
      //      if(find(peakvec[d].begin(),peakvec[d].end(),peak)==peakvec[d].end()){
      int found=0;
      for(vector<PeakXYZW*>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++)if(*peak==(**it)){found=1;break;}
	
      if(!found){//	peakvec[d][npeak[d]++]=new PeakXYZW(peak);
	peakvec[d].push_back(peak);
	sort(peakvec[d].begin(),peakvec[d].end());
      }
      else delete peak;
    }
  }

  if(debug)printf("DoPrefit segx %i segy %i\n",peakvec[0].size(),peakvec[1].size());
  
  int prefit=peakvec[0].size()+peakvec[1].size();
  if(prefit<=0||prefit>100){
    if(debug)printf("skipping \n");

    for(int d=0;d<2;d++){
      for(vector<PeakXYZW*>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++)delete *it;
      peakvec[d].clear();
    }
    return 0;

  }

  if(debug>0)printf("Entering DoLinReg\n");

  int nrh=nrhits;
  if(debug>0)printf("Total number of hits %i\n",nrh);
  
  int i=0;
  for(int d=0;d<2;d++){
    for(vector<PeakXYZW*>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++,i++){
      TrdHSegmentR *seg=new TrdHSegmentR(d,(*it)->y,0.,(*it)->x,0.,(*it)->z,(*it)->w);
      seg->Nhits=0;
      seg->em=0.6/((*it)->zmax-(*it)->zmin);
      
      for(int h=0;h!=nrh;h++){	
	TRDHitRZD rzd=TRDHitRZD(*rhits[h]);
	if(rzd.d!=d)continue;
	
	float dz=rzd.z-seg->z;
	float expos=seg->r+ seg->m*dz;//*cos(atan(seg->m));
	float resid=rzd.r - expos;
	float maxresid=0.6+fabs(dz)* seg->em;
	if(debug>0)printf("d %i hit %i r %.2f expos %.2f resid %f < %f ok %i\n",d,h,rzd.r,expos,resid,maxresid,fabs(resid)<maxresid);
	
	if(fabs(resid)<maxresid)
	  if(seg->Nhits<100)seg->AddHit(rhits[h],h);
	
      }
      //      seg->Nhits=seg->Nhits;
      
      if(debug>0){
	printf("seg %i m %.2f r %.2f z %.2f hits %i\n",i,seg->m,seg->r,seg->z,seg->Nhits);
	for(int i=0;i<seg->Nhits;i++){
	  printf("hit %i iter %i\n",i,seg->fTrdRawHit[i]);
	}
      }
      
      int lr=1;
      while(lr>=0){
	lr=seg->LinReg(debug);
	int n0=seg->Nhits;
	seg->Nhits=0;

	for(int h=0;h!=nrh;h++){	
	  TRDHitRZD rzd=TRDHitRZD(*rhits[h]);
	  if(rzd.d!=d)continue;
	  
	  float dz=rzd.z-seg->z;
	  float resid=(rzd.r - (seg->r+ seg->m*dz));//*cos(atan(seg->m));
	  float maxresid=0.6+fabs(dz)*seg->em;//0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
	  
	  if(debug>0)printf("Lin Reg residuals %.2f < %.2f %i (dz %.2f em %.2e)\n",resid,maxresid,fabs(resid)<maxresid,rzd.z-seg->z,seg->em);
	  if(fabs(resid)<maxresid)
	    if(seg->Nhits<100)seg->AddHit(rhits[h],h);
	}
	
	if(n0>=seg->Nhits){
	  if(debug)printf("no more hits - breaking loop\n");
	  break;
	}
	if(debug)printf("d %i n0 %i n1 %i - refitting\n",d,n0,seg->Nhits);
      }
      
      if(seg->Nhits>3){
	seg->calChi2();
	
	if(debug>0){
	  printf("d %i after linreg loop: segment %i hits %i\n",d,i,seg->Nhits);
	  printf("After linreg: seg %i d %i m %.2f r %.2f z %.2f hits %i\n",i,d,seg->m,seg->r,seg->z,seg->Nhits);
	}
	
	if(nhsegvec<100)AddSegment(seg);
	else delete seg;
      }
    }
  }

  for(int d=0;d<2;d++){
    for(vector<PeakXYZW*>::iterator it=peakvec[d].begin();it!=peakvec[d].end();it++)delete *it;
    peakvec[d].clear();
  }

  return 1;//(npeak[0]+npeak[1]);

}
  
int TrdHReconR::DoLinReg(int debug){
  if(debug>0)printf("Entering DoLinReg\n");
  return 0;
}

 
int TrdHReconR::clean_segvec(int debug){
  if(debug>0){
    printf("Enter clean_segvec\n");
    printf("before clean %i\n",nhsegvec);
    for(int s1=0;s1<nhsegvec;s1++){
      printf("segment %i d %i n %i m %.2f r %.2f z %.2f Chi2 %.2e\n",
	     s1,hsegvec[s1]->d,hsegvec[s1]->Nhits,hsegvec[s1]->m,hsegvec[s1]->r,hsegvec[s1]->z,hsegvec[s1]->Chi2);
    }
  }

  bool keepseg[nhsegvec];
  for(int i=0;i!=nhsegvec;i++)keepseg[i]=1;

  for(int s1=0;s1!=nhsegvec;s1++){
    if(nhsegvec>2&&hsegvec[s1]->Chi2/((float)hsegvec[s1]->Nhits-2)>6){
      if(debug>0)printf("removing segment %i because of high Chi2/ndof %f\n",s1,hsegvec[s1]->Chi2/((float)hsegvec[s1]->Nhits-2));
      keepseg[s1]=0;
    }
    if(keepseg[s1]==0)continue;
    
    for(int s2=s1+1;s2!=nhsegvec;s2++){
      if(hsegvec[s1]->d!=hsegvec[s2]->d)continue;
      if(keepseg[s2]==0)continue;
      
      int nboth=0;
      int n1=hsegvec[s1]->Nhits,n2=hsegvec[s2]->Nhits;

      int hiter1[n1],hiter2[n2];
      for(int hit1=0;hit1<n1;hit1++)hiter1[hit1]=0;
      for(int hit2=0;hit2<n2;hit2++)hiter2[hit2]=0;
      
      for(int hit1=0;hit1<n1;hit1++){
	TRDHitRZD rzd1=TRDHitRZD(*hsegvec[s1]->pTrdRawHit(hit1)); 
	for(int hit2=0;hit2<n2;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(*hsegvec[s2]->pTrdRawHit(hit2)); 
	  
	  if(debug>1) printf("hit1 %i r %.2f z %.2f hit2 %i r %.2f z %.2f both %i\n",hit1,rzd1.r,rzd1.z,hit2,rzd2.r,rzd2.z,rzd1.r == rzd2.r && rzd1.z == rzd2.z);
	  if( rzd1.r == rzd2.r &&
	      rzd1.z == rzd2.z){
	    hiter1[hit1]=1;hiter2[hit2]=1;
	    nboth++;
	  }
	}
      }
      
      if(debug>0)printf("d %i s1 %i n1 %i m1 %.2f Chi2/ndof %.2e s2 %i n2 %i m2 %.2f Chi2/ndof %.2e nboth %i\n",hsegvec[s1]->d,s1,n1,hsegvec[s1]->m,hsegvec[s1]->Chi2/((float)hsegvec[s1]->Nhits-2.),s2,n2,hsegvec[s2]->m,hsegvec[s2]->Chi2/((float)hsegvec[s2]->Nhits-2.),nboth);
      
      if(n1<=2){keepseg[s1]=0;}
      if(n2<=2){keepseg[s2]=0;}
      if(keepseg[s1]==0||keepseg[s2]==0)continue;
      
      if(n1==n2&&nboth==n2){
	if(debug>0)printf("same segments\n");
	if (hsegvec[s1]->Chi2==0||hsegvec[s1]->Nhits<3)keepseg[s1]=0;
	else if(hsegvec[s2]->Chi2==0.||hsegvec[s2]->Nhits<3)keepseg[s2]=0;
	else if(hsegvec[s1]->Chi2/((float)hsegvec[s1]->Nhits-2.) > hsegvec[s2]->Chi2/((float)hsegvec[s2]->Nhits-2.))keepseg[s1]=0;
	else keepseg[s2]=0;
	continue;
      }
      else if(nboth>1){
	if(debug>0)printf("nboth %i > 1 !\n",nboth);
	float s1_Chi2ndof=hsegvec[s1]->Chi2/((float)hsegvec[s1]->Nhits-2.);
	float s2_Chi2ndof=hsegvec[s2]->Chi2/((float)hsegvec[s2]->Nhits-2.);
	
	if(s2_Chi2ndof==0.)keepseg[s2]=0;
	else if(s1_Chi2ndof==0.)keepseg[s1]=0;
	else if(s2_Chi2ndof>s1_Chi2ndof){
	  if(debug>0)printf("Refit s2\n");
	  for(int i=n2-1;i>=0;i--) if(hiter2[i]==1){
	    hsegvec[s2]->RemoveHit(i);
	  }
	  if(debug>0)printf("refit hit size %i -> %i\n",hsegvec[s2]->Nhits,hsegvec[s2]->Nhits);
	  if(hsegvec[s2]->Nhits>2){
	    TrdHSegmentR *seg=hsegvec[s2]->Refit(debug);
	    if(seg)hsegvec[s2]=seg;
	    else keepseg[s2]=0;
	  }
	  else keepseg[s2]=0;
	}
	else {
	  if(debug)printf("Refit s1\n");
	  for(int i=n1-1;i>=0;i--) if(hiter1[i]==1){
	    hsegvec[s2]->RemoveHit(i);
	  }
	  if(debug>0)printf("refit hit size %i . %i\n",hsegvec[s1]->Nhits,hsegvec[s1]->Nhits);
	  if(hsegvec[s1]->Nhits>2){
	    TrdHSegmentR *seg=hsegvec[s1]->Refit(debug);
	    if(seg) hsegvec[s1]=seg;
	    else keepseg[s1]=0;
	  }
	  else keepseg[s1]=0;
	}
      }      
    }
  } 
  
  TrdHSegmentR* tmparr[nhsegvec];int ntmp=0;
  if(debug)printf("segments size %i\n",nhsegvec);
  for(int it=0;it<nhsegvec;it++){
    if(debug)printf("keep segment %i? %i\n",it,keepseg[it]);
    if(keepseg[it]==1)
      tmparr[ntmp++]=hsegvec[it];
  }
  
  nhsegvec=0;
  for(int i=0;i!=ntmp;i++)
    hsegvec[nhsegvec++]=tmparr[i];
  
  if(debug>0){
    printf ("REMAINING SEGMENTS:\n");
    for(int i=0;i!=nhsegvec;i++){
      printf("d %i s %i n %i m %.2f r %.2f Chi2 %.2f nhits %i \n",hsegvec[i]->d,i,hsegvec[i]->Nhits,hsegvec[i]->m,hsegvec[i]->r,hsegvec[i]->Chi2,hsegvec[i]->Nhits);
    }
    printf("after clean %i\n",nhsegvec);
    printf("Exiting clean_seghsegvec\n");
  } 
  return nhsegvec;
}


bool TrdHReconR::check_hits(int is1,int is2,int debug){

  TrdHTrackR* tr=SegToTrack(hsegvec[is1],hsegvec[is2]);
  if(!tr){
    if(debug>0)printf("No track found / Segments not compatible\n");
    return 0;
  }
  tr->fTrdHSegment[hsegvec[is1]->d]=is1;
  tr->fTrdHSegment[hsegvec[is2]->d]=is2;

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
  return (nhitsok==nref);
}

vector<pair<int,int> > TrdHReconR::check_secondaries(){
  int debug=0;
  vector<pair<int, int> > toReturn;toReturn.clear();

  // rough division of TRD height in dm (0=0-10 cm, 6=60-70 cm)
  int zvec[7];for(int i=0;i!=7;i++)zvec[i]=0;

  for(int s1=0;s1!=nrhits;s1++){
    for(int s2=s1+1;s2!=nrhits;s2++){
      if(hsegvec[s1]->d==hsegvec[s2]->d){

	// check for z of crossing
	float z_cross = (hsegvec[s2]->r - hsegvec[s1]->r - hsegvec[s2]->m * hsegvec[s2]->z + 
			 hsegvec[s1]->m * hsegvec[s1]->z) / ( hsegvec[s1]->m - hsegvec[s2]->m );
	
	if(z_cross>150||z_cross < 80)continue;
	zvec[(int)floor((z_cross-80.)/10.)]++;;
	if(hsegvec[s1]->d==0&&(z_cross>120||z_cross<102))continue;

	float s1zmax=0.,s1zmin=200.;
	for(int hit1=0;hit1!=hsegvec[s1]->Nhits;hit1++){
	  TRDHitRZD rzd1=TRDHitRZD(*hsegvec[s1]->pTrdRawHit(hit1));
	  
	  if(rzd1.z>s1zmax)s1zmax=rzd1.z;
	  if(rzd1.z<s1zmin)s1zmin=rzd1.z;
	}
	float s2zmax=0.,s2zmin=200.;
	for(int hit2=0;hit2!=hsegvec[s2]->Nhits;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(*hsegvec[s2]->pTrdRawHit(hit2));

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

  TrdHTrackR* tr=SegToTrack(hsegvec[is1], hsegvec[is2]);
  if(!tr)return 0;
  tr->fTrdHSegment[hsegvec[is1]->d]=is1;
  tr->fTrdHSegment[hsegvec[is2]->d]=is2;
  for(int i=0;i!=hsegvec[is1]->Nhits;i++){
    TRDHitRZD rzd=TRDHitRZD(*hsegvec[is1]->pTrdRawHit(i));
    float x=0.,y=0.;
    tr->propagateToZ(rzd.z,x,y);
    
    float base=770.+230./(rzd.z-132.);
    float side=320.+100./(rzd.z-132.);
    float line=720.+205./(rzd.z-132.);

    if(fabs(x)>base)return 0;
    if(fabs(y)>base)return 0;
    if(fabs(y)>line)return 0;
    if(fabs(x)+fabs(x)>side)return 0;
  }
  for(int i=0;i!=hsegvec[is2]->Nhits;i++){
    TRDHitRZD rzd=TRDHitRZD(*hsegvec[is2]->pTrdRawHit(i));
    float x=0.,y=0.;
    tr->propagateToZ(rzd.z,x,y);
    
    float base=770.+230./(rzd.z-132.);
    float side=320.+100./(rzd.z-132.);
    float line=720.+205./(rzd.z-132.);

    if(fabs(x)>base)return 0;
    if(fabs(y)>base)return 0;
    if(fabs(y)>line)return 0;
    if(fabs(x)+fabs(x)>side)return 0;
  }

  return 1;
}

int TrdHReconR::combine_segments(int debug){
  bool s_done[nhsegvec];
  bool s_poss[nhsegvec][nhsegvec];

  for(int i=0;i!=nhsegvec;i++){
    s_done[i]=0;
    for(int j=0;j!=nhsegvec;j++){
      if(j==i||hsegvec[i]->d==hsegvec[j]->d){s_poss[i][j]=0;s_poss[j][i]=0;}
      else {s_poss[i][j]=1;s_poss[j][i]=1;}
    }
  }
  
  for(int i=0;i!=nhsegvec;i++){
    for(int j=i+1;j!=nhsegvec;j++){
      if(s_poss[i][j]==0)continue;

      if(hsegvec[i]->Nhits<3){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_geometry(i,j)==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(check_hits(i,j)==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
    }
  }
  
  int n_poss[nhsegvec];
  for(int i=0;i!=nhsegvec;i++){
    if(s_done[i]==1)continue;
    
    n_poss[i]=0;
    for(int j=0;j!=nhsegvec;j++)if(s_poss[i][j]==1)n_poss[i]++;
    
    else if(n_poss[i]==1){
      for(int j=0;j!=nhsegvec;j++){
	if(s_poss[i][j]!=1)continue;
	if(s_done[i]==1||s_done[j]==1)continue;
	TrdHTrackR *tr=SegToTrack(hsegvec[i],hsegvec[j]);

	if(tr){
	  tr->fTrdHSegment[hsegvec[i]->d]=i;
	  tr->fTrdHSegment[hsegvec[j]->d]=j;
	  if(nhtrvec<20){
	    tr->status=1;
	    AddTrack(tr);
	    //	  _htrvec[nhtrvec++]=tr;
	    s_done[i]=1;s_done[j]=1;
	  }
	  else delete tr;
	}
      }
    }
  }

  int n_done=0;
  for(int i=0;i!=nhsegvec;i++)if(s_done[i]==1)n_done++;
  if(nhsegvec==n_done)return nhtrvec;

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
  for(int i=0;i!=nhsegvec;i++)if(s_done[i]==1)n_done++;

  if(debug)printf("%i segment(s) not done after sec. search \n",nhsegvec-n_done);

  return nhtrvec;
}



void TrdHReconR::ReadTRDEvent(vector<TrdRawHitR> r, vector<TrdHSegmentR> s, vector<TrdHTrackR> t){
  nrhits=0;
  nhsegvec=0;
  nhtrvec=0;

  for(int i=0;i!=r.size();i++)if(nrhits<1024)rhits[nrhits++]=new TrdRawHitR(r[i]);
  for(int i=0;i!=s.size();i++)if(nhsegvec<100)hsegvec[nhsegvec++]=new TrdHSegmentR(s[i]);  
  for(int i=0;i!=t.size();i++)if(nhtrvec<20)htrvec[nhtrvec++]=new TrdHTrackR(t[i]);
}

void TrdHReconR::BuildTRDEvent(vector<TrdRawHitR> r){
  clear();
  if(r.size()<3)return;
  VCon* cont=GetVCon()->GetCont("AMSTRDRawHit");
  if(!cont) {
    cerr << "TrRecon::Build  Cant Find AMSTrdRawHit" << endl;
    return ;
  }

  for(int n=0;n<r.size();n++)if(nrhits<1024)AddHit(&r[n]);

  /*  // Check number of TrRawClusters
  int nraw = cont->getnelem();
  printf("passed raw hits %i cont raw hits %i\n",r.size(),nraw);
  for(int n=0;n<r.size();n++){
    printf("passed hit %i\n",n);
    for(int i=0;i!=nraw;i++)if(nrhits<1024){
      printf("cont hit %i\n",i);
      AMSTRDRawHit *thit=(AMSTRDRawHit*)cont->getelem(i);
      printf("nrh %i LLT %02i%02i%02i\n",nrhits,thit->getlayer(),thit->getladder(),thit->gettube());

      rhits[nrhits] = (TrdRawHitR*)cont->getelem(i);
      printf("nrh %i LLT %02i%02i%02i\n",nrhits,rhits[nrhits]->Layer,rhits[nrhits]->Ladder,rhits[nrhits]->Tube);
      if(r[n]==*rhits[nrhits]){
	printf("found iter %i\n",i);
	irhits[nrhits] = i;//cont->getindex((TrElem*)i);
	nrhits++;
      }
    }
  }
*/
  delete cont;

  retrdhevent();

  //AddHit(r[i],i,);
  //rhits[nrhits++]=new TrdRawHitR(r[i]);
  
  //  retrdhevent();
  //  int nhseg=TrdHSegmentR::build();
  //  int nhtr=0;
  //  if(nhseg>1)
  //    nhtr=TrdHTrackR::build();
}

void TrdHReconR::clear(){
  nrhits=0;
  for(int i=0;i<nhsegvec;i++){
    if(hsegvec[i]!=0)delete hsegvec[i];
    hsegvec[i]=0;
  }
  nhsegvec=0;
  for(int i=0;i<nhtrvec;i++){
    if(htrvec[i]!=0)delete htrvec[i];
    htrvec[i]=0;
  }
  nhtrvec=0;


  for(int i=0;i<nrhits;i++){
    if(rhits[i])delete rhits[i];
    rhits[i]=0;
  }

  return;
}
int TrdHReconR::retrdhevent(){
  int debug=0;
  
  // skip arrays for now
  int nhcut=1.e6; //  below 40 nTRDRawHits: vector-histo 
  // above                 array-histo
  
  double Hcut=0.0;
  //if(pev->nTRDRawHit()>500) Hcut=30.0;
  
  if(debug)printf("RawHits %i\n",nrhits);
  if(nrhits<4||nrhits>100) return 0;
  
  //  if(!H2A_mvr[0])H2A_mvr[0] = new TH2A ("ha_mvr_0", "m vs x", 314, -105.0, 105.0, 119, -3.0, 3.0);
  //  if(!H2A_mvr[1])H2A_mvr[1] = new TH2A ("ha_mvr_1", "m vs y", 314, -105.0, 105.0, 119, -3.0, 3.0);
		   
  
  //  if(nrh>nhcut){
  //    H2A_mvr[0]->Reset();
  //    H2A_mvr[1]->Reset();
  //  }else{
  //  H2V_mvr[0]->Reset();
  //  H2V_mvr[1]->Reset();
  //  }
  
  int prefit=DoPrefit(debug);

  if(!prefit)return 0;
  clean_segvec(debug);
  
  if(debug) printf("got %i segment(s)\n",nhsegvec);
  
  int nhseg=nhsegvec;
  if(nhseg==2||nhseg==3){
    int segiter_sel[2]={-1,-1};
    
    for(int s=0;s!=nhseg;s++){
      int d=hsegvec[s]->d;
      float chi2ndofs=1.e6;
      if(hsegvec[s]->nTrdRawHit()>2)
	chi2ndofs=(float) hsegvec[s]->Chi2 / (float) hsegvec[s]->nTrdRawHit();

      if(segiter_sel[d]>-1){
	float chi2ndof=1.e6;
	if(hsegvec[segiter_sel[d]]->nTrdRawHit()>2)
	  chi2ndof=(float) hsegvec[segiter_sel[d]]->Chi2 / (float) hsegvec[segiter_sel[d]]->nTrdRawHit();
	
	if(debug)printf("cand s %.2f < %.2f ?\n",chi2ndof,chi2ndofs);
	if( ( hsegvec[s]->nTrdRawHit() >  hsegvec[segiter_sel[d]]->nTrdRawHit()) ||
	    ((hsegvec[s]->nTrdRawHit() == hsegvec[segiter_sel[d]]->nTrdRawHit()) &&
	     (chi2ndof < chi2ndofs))) segiter_sel[d]=s;
      }
      else if(hsegvec[s]->nTrdRawHit()>0)segiter_sel[d]=s;
      
      if(debug)printf("sel %i d %i nTrdRawHit() %i chi2ndof %.2f\n",s,d,hsegvec[segiter_sel[d]]->nTrdRawHit(),chi2ndofs);
    }

    if(debug) printf("segiter %i %i\n",segiter_sel[0],segiter_sel[1]);
    if(segiter_sel[0]>-1&&segiter_sel[1]>-1){
      TrdHTrackR *tr=SegToTrack(hsegvec[segiter_sel[0]],hsegvec[segiter_sel[1]],debug);
      if(tr&&nhtrvec<20){
	tr->fTrdHSegment[0]=segiter_sel[0];
	tr->fTrdHSegment[1]=segiter_sel[1];

	tr->status=1;
	AddTrack(tr);
      }
      else if (tr)delete tr;
    }
  }
  else if(nhseg>2) combine_segments();

  if(debug)printf("TrdHTrackR::build tracks %i\n",nhtrvec);
  
  return 1;
}

void TrdHReconR::AddTrack(TrdHTrackR* tr){
  VCon* cont2=GetVCon()->GetCont("AMSTRDHTrack");
  if(tr){
    
    for(int i=0;i<20;i++)tr->elayer[i]=0.;
    for(int s=0;s<2;s++){
      for(int h=0;h<tr->segments[s]->Nhits;h++){
	tr->elayer[tr->segments[s]->hits[h]->Layer]+=tr->segments[s]->hits[h]->Amp;
      }
    }

#ifndef __ROOTSHAREDLIBRARY__
    AMSTRDHTrack* trk=new AMSTRDHTrack(tr);
#else
    TrdHTrackR* trk=new TrdHTrackR(tr);
#endif


    cont2->addnext(trk);
    htrvec[nhtrvec] = tr;
    ihtrvec[nhtrvec] = cont2->getindex(trk);
    nhtrvec++;
  }
  delete cont2;
}



void TrdHReconR::AddSegment(TrdHSegmentR* seg){
  VCon* cont2=GetVCon()->GetCont("AMSTRDHSegment");
  if(seg){

#ifndef __ROOTSHAREDLIBRARY__
    AMSTRDHSegment* tmp=new AMSTRDHSegment(seg);
#else
    TrdHSegmentR* tmp=new TrdHSegmentR(seg);
#endif
    cont2->addnext(tmp);
    hsegvec[nhsegvec] = seg;
    ihsegvec[nhsegvec] = cont2->getindex(tmp);
    nhsegvec++;
  }
  delete cont2;
}

void TrdHReconR::AddHit(TrdRawHitR* hit){
  VCon* cont2=GetVCon()->GetCont("AMSTRDRawHit");
  if(hit){

    //#ifndef __ROOTSHAREDLIBRARY__
    //    AMSTRDRawHit* tmp=new AMSTRDRawHit(hit);
    //#else
    TrdRawHitR* tmp=(TrdRawHitR*)hit;
    //#endif
    cont2->addnext(tmp);
    rhits[nrhits] = hit;
    irhits[nrhits] = cont2->getindex(tmp);
    nrhits++;
  }
  delete cont2;
}

