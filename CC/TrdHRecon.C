#include "TrdHRecon.h"

TrdHReconR trdhrecon;
#pragma omp threadprivate (trdhrecon)

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
  
  if(debug)for(int d=0;d!=2;d++){
    printf("d %i m %.2f em %.2f r %.2f z %.2f\n",d,m[d],em[d],r[d],z[d]);
  }
  // same center of segments
  float zcent=(s1->z * (float) s1->Nhits + s2->z * (float) s2->Nhits) /
    ((float) s1->Nhits + (float) s2->Nhits);
  
  for(int d=0;d!=2;d++)
    r[d]=r[d] + m[d] * ( zcent - z[d] );

  float Dir[3],Coo[3];
  Dir[0]=-m[0];Dir[1]=-m[1],Dir[2]=-1.;
  Coo[0]=r[0];Coo[1]=r[1],Coo[2]=zcent;

  if(debug)printf("built track Coo %.2f %.2f %.2f mx %.2f my %.2f\n",Coo[0],Coo[1],Coo[2],Dir[0]/Dir[2],Dir[1]/Dir[2]);

  TrdHTrackR* tr=new TrdHTrackR(Coo,Dir);
  tr->Chi2=s1->Chi2+s2->Chi2;
  tr->Nhits=s1->nTrdRawHit()+s2->nTrdRawHit();
  tr->SetSegment(s1,s2);

  if(debug)printf("Leaving SegToTrack - return AMSTRDHTrack\n");
  return tr;
}

int TrdHReconR::DoPrefit(vector<PeakXYZW> &seg_x, vector<PeakXYZW> &seg_y){
  int debug=0;

  for(int d=0;d!=2;d++){
    if(!H2V_mvr[d])H2V_mvr[d]=new TH2V ("", ";offet [cm];slope", 314,-105,105, 119, -3, 3);
    H2V_mvr[d]->Reset();
    
    // ARRAY should be included
    
  }
  int nrh=rhits.size();

  int pairs_x=0,pairs_y=0;;
  for(int i=0;i!=nrh;i++){
    TRDHitRZD rzdi=TRDHitRZD(rhits[i]);
    for(int j=i+1;j!=nrh;j++){
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

  
vector<TrdHSegmentR> TrdHReconR::DoLinReg(vector<PeakXYZW> *segvec_x,vector<PeakXYZW> *segvec_y,int debug){
  if(debug>0)printf("Entering DoLinReg\n");

  int nrh=rhits.size();
  if(debug>0)printf("Total number of hits %i\n",nrh);

  vector<TrdHSegmentR> segments;
  for(int d=0;d!=2;d++){
    vector<PeakXYZW> *segvec=NULL;
    if(d==0) segvec=segvec_x;
    if(d==1) segvec=segvec_y;
    for(int i=0;i!=segvec->size();i++){
      TrdHSegmentR *seg=new TrdHSegmentR(d,segvec->at(i).y,0.,segvec->at(i).x,0.,segvec->at(i).z,segvec->at(i).w);
      seg->em=0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
      
      for(int h=0;h!=nrh;h++){	
	TRDHitRZD rzd=TRDHitRZD(rhits[h]);
	if(rzd.d!=d)continue;
	
	float dz=rzd.z-seg->z;
	float expos=seg->r+ seg->m*dz;//*cos(atan(seg->m));
	float resid=rzd.r - expos;
	float maxresid=0.6+fabs(dz)* seg->em;
	if(debug>0)printf("d %i hit %i r %.2f expos %.2f resid %f < %f ok %i\n",d,h,rzd.r,expos,resid,maxresid,fabs(resid)<maxresid);
	
	if(fabs(resid)<maxresid)
	  seg->fTrdRawHit.push_back(h);
      }
      seg->Nhits=seg->nTrdRawHit();
      
      if(debug>0)printf("seg %i m %.2f r %.2f z %.2f hits %i\n",i,seg->m,seg->r,seg->z,seg->Nhits);
      
      int lr=1;
      while(lr>=0){
	lr=seg->LinReg();
	int n0=seg->nTrdRawHit();
	seg->fTrdRawHit.clear();

	for(int h=0;h!=nrh;h++){	
	  TRDHitRZD rzd=TRDHitRZD(rhits[h]);
	  if(rzd.d!=d)continue;
	  
	  float dz=rzd.z-seg->z;
	  float resid=(rzd.r - (seg->r+ seg->m*dz));//*cos(atan(seg->m));
	  float maxresid=0.6+fabs(dz)*seg->em;//0.6/(segvec->at(i).zmax-segvec->at(i).zmin);
	  
	  if(debug>0)printf("Lin Reg residuals %.2f < %.2f %i (dz %.2f em %.2e)\n",resid,maxresid,fabs(resid)<maxresid,rzd.z-seg->z,seg->em);
	  if(fabs(resid)<maxresid)
	    seg->fTrdRawHit.push_back(h);
	  
	}
	
	if(n0>=seg->nTrdRawHit()){
	  if(debug)printf("no more hits - breaking loop\n");
	  break;
	}
	if(debug)printf("d %i n0 %i n1 %i - refitting\n",d,n0,seg->nTrdRawHit());
      }
      
      seg->Nhits=seg->nTrdRawHit();
      seg->calChi2();
      
      if(debug>0){
	printf("d %i after linreg loop: segment %i hits %i\n",d,i,seg->Nhits);
	printf("After linreg: seg %i d %i m %.2f r %.2f z %.2f hits %i\n",i,d,seg->m,seg->r,seg->z,seg->Nhits);
      }
      segments.push_back(*seg);
    }
  }
  return segments;
}

 
vector<TrdHSegmentR> TrdHReconR::clean_segvec(vector<TrdHSegmentR> vec,int debug){
  
  //MM I hope this issue is solved
  //PZ FPE bugfix 
  // this function contains broken code 
  // it should be rewritten
  // for the moment really-force FPE masking

  /*    int env=0;
#ifndef __DARWIN__
    env=fegetexcept();
    if(MISCFFKEY.RaiseFPE<=2){
      fedisableexcept(FE_OVERFLOW);
      fedisableexcept(FE_INVALID);
      fedisableexcept(FE_DIVBYZERO);
    }
#endif    
  */
  if(debug>0){
    printf("Enter clean_segvec\n");
    printf("before clean %i\n",vec.size());
    for(int s1=0;s1!=vec.size();s1++){
      printf("segment %i d %i n %i m %.2f r %.2f z %.2f Chi2 %.2e\n",
	     s1,vec[s1].d,vec[s1].Nhits,vec[s1].m,vec[s1].r,vec[s1].z,vec[s1].Chi2);
    }
  }

  bool keepseg[vec.size()];
  for(int i=0;i!=vec.size();i++)keepseg[i]=1;

  for(int s1=0;s1!=vec.size();s1++){
    if(vec.size()>2&&vec[s1].Chi2/((float)vec[s1].Nhits-2)>6){
      if(debug>0)printf("removing segment %i because of high Chi2/ndof %f\n",s1,vec[s1].Chi2/((float)vec[s1].Nhits-2));
      keepseg[s1]=0;
    }
    if(keepseg[s1]==0)continue;
    
    for(int s2=s1+1;s2!=vec.size();s2++){
      if(vec[s1].d!=vec[s2].d)continue;
      if(keepseg[s2]==0)continue;
      
      int nboth=0;
      int n1=vec[s1].nTrdRawHit(),n2=vec[s2].nTrdRawHit();
      
      int hiter1[n1],hiter2[n2];
      for(int hit1=0;hit1!=n1;hit1++)hiter1[hit1]=0;
      for(int hit2=0;hit2!=n2;hit2++)hiter2[hit2]=0;
      
      for(int hit1=0;hit1!=n1;hit1++){
	TRDHitRZD rzd1=TRDHitRZD(*vec[s1].pTrdRawHit(hit1)); 
	for(int hit2=0;hit2!=n2;hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(*vec[s2].pTrdRawHit(hit2)); 

	  if(debug>1) printf("hit1 %i r %.2f z %.2f hit2 %i r %.2f z %.2f both %i\n",hit1,rzd1.r,rzd1.z,hit2,rzd2.r,rzd2.z,rzd1.r == rzd2.r && rzd1.z == rzd2.z);
	  if( rzd1.r == rzd2.r &&
	      rzd1.z == rzd2.z){
	    hiter1[hit1]=1;hiter2[hit2]=1;
	    nboth++;
	  }
	}
      }
      
      if(debug>0)printf("d %i s1 %i n1 %i m1 %.2f Chi2/ndof %.2e s2 %i n2 %i m2 %.2f Chi2/ndof %.2e nboth %i\n",vec[s1].d,s1,n1,vec[s1].m,vec[s1].Chi2/((float)vec[s1].nTrdRawHit()-2.),s2,n2,vec[s2].m,vec[s2].Chi2/((float)vec[s2].nTrdRawHit()-2.),nboth);
      
      if(n1<=2){keepseg[s1]=0;}
      if(n2<=2){keepseg[s2]=0;}
      if(keepseg[s1]==0||keepseg[s2]==0)continue;
      
      if(n1==n2&&nboth==n2){
	if(debug>0)printf("same segments\n");
	if (vec[s1].Chi2==0||vec[s1].Nhits<3)keepseg[s1]=0;
	else if(vec[s2].Chi2==0.||vec[s2].Nhits<3)keepseg[s2]=0;
	else if(vec[s1].Chi2/((float)vec[s1].nTrdRawHit()-2.) > vec[s2].Chi2/((float)vec[s2].nTrdRawHit()-2.))keepseg[s1]=0;
	else keepseg[s2]=0;
	continue;
      }
      else if(nboth>1){
	if(debug>0)printf("nboth %i > 1 !\n",nboth);
	float s1_Chi2ndof=vec[s1].Chi2/((float)vec[s1].nTrdRawHit()-2.);
	float s2_Chi2ndof=vec[s2].Chi2/((float)vec[s2].nTrdRawHit()-2.);
	
	if(s2_Chi2ndof==0.)keepseg[s2]=0;
	else if(s1_Chi2ndof==0.)keepseg[s1]=0;
	else if(s2_Chi2ndof>s1_Chi2ndof){
	  if(debug>0)printf("Refit s2\n");
	  for(int i=n2-1;i>=0;i--) if(hiter2[i]==1) vec[s2].fTrdRawHit.erase(vec[s2].fTrdRawHit.begin()+i);
	  if(debug>0)printf("refit hit size %i -> %i\n",vec[s2].Nhits,vec[s2].nTrdRawHit());
	  if(vec[s2].nTrdRawHit()>2){
	    TrdHSegmentR *seg=vec[s2].Refit(debug);
	    if(seg)vec[s2]=*seg;
	    else keepseg[s2]=0;
	  }
	  else keepseg[s2]=0;
	}
	else {
	  if(debug)printf("Refit s1\n");
	  for(int i=n1-1;i>=0;i--) if(hiter1[i]==1) vec[s1].fTrdRawHit.erase(vec[s1].fTrdRawHit.begin()+i);
	  if(debug>0)printf("refit hit size %i . %i\n",vec[s1].Nhits,vec[s1].nTrdRawHit());
	  if(vec[s1].nTrdRawHit()>2){
	    TrdHSegmentR *seg=vec[s1].Refit(debug);
	    if(seg) vec[s1]=seg;
	    else keepseg[s1]=0;
	  }
	  else keepseg[s1]=0;
	}
      }      
    }
  } 
  
  if(debug)printf("segments size %i\n",vec.size());
  for(int it=vec.size()-1;it>=0;it--){
    if(debug)printf("keep segment %i? %i\n",it,keepseg[it]);
    if(keepseg[it]==0)vec.erase(vec.begin()+it);
  }
  //  sort(vec.begin(),vec.end());

  int n0=0,n1=0;
  for(int i=0;i!=vec.size();i++){
    if(vec[i].d==0)n0++;
    if(vec[i].d==1)n1++;
  }
  for(int i=vec.size()-1;i>=0;i--){
    if(vec[i].d==0&&n0>5){vec.erase(vec.begin()+i);n0--;}
    if(vec[i].d==1&&n1>5){vec.erase(vec.begin()+i);n1--;}
  }



  if(debug>0){
    printf ("REMAINING SEGMENTS:\n");
    for(int i=0;i!=vec.size();i++){
      printf("d %i s %i n %i m %.2f r %.2f Chi2/ndof %.2e \n",vec[i].d,i,vec[i].Nhits,vec[i].m,vec[i].r,vec[i].Chi2/((float)vec[i].Nhits-2.));
    }
    printf("after clean %i\n",vec.size());
    printf("Exiting clean_segvec\n");
  } 
  /*
#ifndef __DARWIN__
    feclearexcept(FE_OVERFLOW);
    feclearexcept(FE_INVALID);
    feclearexcept(FE_DIVBYZERO);
    if(env){
      feenableexcept(env);        
    }
#endif    
  */
  return vec;
}


bool TrdHReconR::check_hits(TrdHSegmentR* s1,TrdHSegmentR* s2,int debug){
  TrdHTrackR* tr=trdhrecon.SegToTrack(s1,s2);
  if(!tr){
    if(debug>0)printf("No track found / Segments not compatible\n");
    return 0;
  }
  int hit_ok[refhits.size()];
  int nhitsok=0;
  for(int i=0;i!=refhits.size();i++){
    hit_ok[i]=0;
    
    float dz=(refhits[i].z() - tr->Coo[2]);

    float pos_x = tr->Coo[0]+tr->Dir[0]/tr->Dir[2]*dz;
    float pos_y = tr->Coo[1]+tr->Dir[1]/tr->Dir[2]*dz;

    float ex   = fabs(dz)*tr->emx();
    float ey   = fabs(dz)*tr->emy(1);

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

  if(debug)printf("return - all hits found? %i\n",nhitsok==refhits.size());
  return (nhitsok==refhits.size());
}

vector<pair<int,int> > TrdHReconR::check_secondaries(){
  int debug=0;
  vector<pair<int, int> > toReturn;toReturn.clear();

  // rough division of TRD height in dm (0=0-10 cm, 6=60-70 cm)
  int zvec[7];for(int i=0;i!=7;i++)zvec[i]=0;

  for(int s1=0;s1!=trdhrecon.rhits.size();s1++){
    for(int s2=s1+1;s2!=trdhrecon.rhits.size();s2++){
      if(trdhrecon.hsegvec[s1].d==trdhrecon.hsegvec[s2].d){

	// check for z of crossing
	float z_cross = (trdhrecon.hsegvec[s2].r - trdhrecon.hsegvec[s1].r - trdhrecon.hsegvec[s2].m * trdhrecon.hsegvec[s2].z + 
			 trdhrecon.hsegvec[s1].m * trdhrecon.hsegvec[s1].z) / ( trdhrecon.hsegvec[s1].m - trdhrecon.hsegvec[s2].m );
	
	if(z_cross>150||z_cross < 80)continue;
	zvec[(int)floor((z_cross-80.)/10.)]++;;
	if(trdhrecon.hsegvec[s1].d==0&&(z_cross>120||z_cross<102))continue;

	float s1zmax=0.,s1zmin=200.;
	for(int hit1=0;hit1!=trdhrecon.hsegvec[s1].nTrdRawHit();hit1++){
	  TRDHitRZD rzd1=TRDHitRZD(*trdhrecon.hsegvec[s1].pTrdRawHit(hit1));
	  if(rzd1.z>s1zmax)s1zmax=rzd1.z;
	  if(rzd1.z<s1zmin)s1zmin=rzd1.z;
	}
	float s2zmax=0.,s2zmin=200.;
	for(int hit2=0;hit2!=trdhrecon.hsegvec[s2].nTrdRawHit();hit2++){
	  TRDHitRZD rzd2=TRDHitRZD(*trdhrecon.hsegvec[s2].pTrdRawHit(hit2));
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

bool TrdHReconR::check_geometry(TrdHSegmentR *s1, TrdHSegmentR *s2){
  // idea is to calculate if track candidate leaves TRD but has hits "outside" TRD volume
  // for this define TRD contours as a function of z and compare to highest/ lowest hitz

  TrdHTrackR* tr=trdhrecon.SegToTrack(s1, s2);
  if(!tr)return 0;
  for(int i=0;i!=s1->Nhits;i++){
    TRDHitRZD rzd=TRDHitRZD(*s1->pTrdRawHit(i));
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
  for(int i=0;i!=s2->Nhits;i++){
    TRDHitRZD rzd=TRDHitRZD(*s2->pTrdRawHit(i));
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
  bool s_done[trdhrecon.hsegvec.size()];
  bool s_poss[trdhrecon.hsegvec.size()][trdhrecon.hsegvec.size()];

  for(int i=0;i!=trdhrecon.hsegvec.size();i++){
    s_done[i]=0;
    for(int j=0;j!=trdhrecon.hsegvec.size();j++){
      if(j==i||trdhrecon.hsegvec[i].d==trdhrecon.hsegvec[j].d){s_poss[i][j]=0;s_poss[j][i]=0;}
      else {s_poss[i][j]=1;s_poss[j][i]=1;}
    }
  }
  
  for(int i=0;i!=trdhrecon.hsegvec.size();i++){
    for(int j=i+1;j!=trdhrecon.hsegvec.size();j++){
      if(s_poss[i][j]==0)continue;

      if(trdhrecon.hsegvec[i].nTrdRawHit()<3){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(trdhrecon.check_geometry(&trdhrecon.hsegvec[i],&trdhrecon.hsegvec[j])==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
      if(trdhrecon.check_hits(&trdhrecon.hsegvec[i],&trdhrecon.hsegvec[j])==0){s_poss[i][j]=0;s_poss[j][i]=0;continue;}
    }
  }
  
  int n_poss[trdhrecon.hsegvec.size()];
  for(int i=0;i!=trdhrecon.hsegvec.size();i++){
    if(s_done[i]==1)continue;
    
    n_poss[i]=0;
    for(int j=0;j!=trdhrecon.hsegvec.size();j++)if(s_poss[i][j]==1)n_poss[i]++;
    
    else if(n_poss[i]==1){
      for(int j=0;j!=trdhrecon.hsegvec.size();j++){
	if(s_poss[i][j]!=1)continue;
	if(s_done[i]==1||s_done[j]==1)continue;
	TrdHTrackR *tr=trdhrecon.SegToTrack(&trdhrecon.hsegvec[i],&trdhrecon.hsegvec[j]);
	tr->status=1;
	if(tr){trdhrecon.htrvec.push_back(*tr);
	  s_done[i]=1;s_done[j]=1;}
      }
    }
  }

  int n_done=0;
  for(int i=0;i!=trdhrecon.hsegvec.size();i++)if(s_done[i]==1)n_done++;
  if(trdhrecon.hsegvec.size()==n_done)return trdhrecon.htrvec.size();

  // check for vertices
  vector<pair<int,int> > check=trdhrecon.check_secondaries();

  if(check.size()>0){
    // skip secondary vertices for now
    for(int i=0;i!=check.size();i++){
      int s=abs(check[i].second);
      s_done[s]=1;
      if(check[i].second==99&&check[i].first==99){
	if(debug) printf("\nHADRONIC INTERACTION IN TRD\n");
	//	save_ev=1;
	return trdhrecon.htrvec.size();
      }
    }
  }

  n_done=0;
  for(int i=0;i!=trdhrecon.hsegvec.size();i++)if(s_done[i]==1)n_done++;

  if(debug)printf("%i segment(s) not done after sec. search \n",trdhrecon.hsegvec.size()-n_done);

  return trdhrecon.htrvec.size();
}



void TrdHReconR::ReadTRDEvent(vector<TrdRawHitR> r, vector<TrdHSegmentR> s, vector<TrdHTrackR> t){
  trdhrecon.rhits.clear();
  trdhrecon.hsegvec.clear();
  trdhrecon.htrvec.clear();
  for(int i=0;i!=r.size();i++)trdhrecon.rhits.push_back(r[i]);
  for(int i=0;i!=s.size();i++)trdhrecon.hsegvec.push_back(s[i]);  
  for(int i=0;i!=t.size();i++)trdhrecon.htrvec.push_back(t[i]);
}

void TrdHReconR::BuildTRDEvent(vector<TrdRawHitR> r){
  trdhrecon.rhits.clear();
  trdhrecon.hsegvec.clear();
  trdhrecon.htrvec.clear();
  for(int i=0;i!=r.size();i++)trdhrecon.rhits.push_back(r[i]);
  
  int nhseg=TrdHSegmentR::build();
  int nhtr=0;
  if(nhseg>1)
    nhtr=TrdHTrackR::build();
}

