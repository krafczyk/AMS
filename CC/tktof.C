#include "TrTrack.h"
#include "TrRecon.h"
#include "root.h"
#include "TFitResult.h"


bool TkTOFMatch2(TrTrackR* ptrack,int select_tag) {  
  class mtof_hitH{
  public:
    TofClusterHR * phit;
    AMSPoint min_d;
    number sleng;
    bool Xmatch;
    bool Ymatch;
    bool Tmatch;
    bool Lmatch;
    AMSPoint checker;
    mtof_hitH():phit(0),min_d(AMSPoint(0,0,0)),sleng(0),
		Xmatch(false),Ymatch(false),Tmatch(false),Lmatch(false)
    {
      //                    Transversal              Longitudinal  
      checker=AMSPoint(3.,3.,3.);
    }
    mtof_hitH(mtof_hitH &orig):phit(orig.phit),min_d(orig.min_d),sleng(orig.sleng),
			       Xmatch(orig.Xmatch),Ymatch(orig.Ymatch),
			       Tmatch(orig.Tmatch),Lmatch(orig.Lmatch),checker(orig.checker){}
    ~mtof_hitH(){phit=0;}
    number distT() {return (phit->Layer==0 ||phit->Layer==3)? min_d[1]:min_d[0];}
    void MatchCheck(){
      if(!phit) return;
      bool YY=(phit->Layer==0 ||phit->Layer==3);
      if (YY){
	Xmatch= (  min_d[0]   <=  checker[1]);
	Ymatch= (  min_d[1]   <=  checker[0]);
	Tmatch=Ymatch; 
	Lmatch=Xmatch;
      }
      else   { 
	Xmatch= (  min_d[0]   <=  checker[0]);
	Ymatch= (  min_d[1]   <=  checker[1]);
	Tmatch=Xmatch; 
	Lmatch=Ymatch;
      }
      return;
    }
    
  }; 
  
  
  
  mtof_hitH tmhit[4];  
  
  
  // 0 -- Search for the geometrically matching TOF Hits  

  //-----new cont
  VCon* tcont = GetVCon()->GetCont("AMSTOFClusterH");
  
  for (int ii=0;ii<4;ii++)
    tmhit[ii].min_d= AMSPoint(1000,1000,1000);
 
  for (int ii=0;ii<tcont->getnelem();ii++){
    TofClusterHR* tofhit=(TofClusterHR*)tcont->getelem(ii);
    int  TOFlay=tofhit->Layer;
    // 1 -- FIND on each TOF layer the hit closer to the track
    if(!(tofhit->IsGoodTime())) continue;
    
    AMSDir dir(0,0,1.);
    AMSPoint outp,dst;
    AMSPoint ecoo(tofhit->ECoo);
    AMSPoint coo(tofhit->Coo);
    number theta,phi,sleng;   
        ptrack->interpolate(coo,dir,outp,theta,phi,sleng);
    if(ptrack->IsFake()){
      ecoo[0]+=1.;  // add one cm fot trd/tof
      ecoo[1]+=1.;  // add one cm fot trd/tof
    }
    
    dst=((outp-coo)/ecoo).abs();
    
    // search the closest along Y only
    if (fabs(dst[1]) <= tmhit[TOFlay].min_d[1]){
      //cout<<" The cluster "<<clnum-1<<" is good"<<endl; 
      tmhit[TOFlay].min_d = dst.abs(); 
      tmhit[TOFlay].phit  = tofhit;
      tmhit[TOFlay].sleng = sleng;
    }
  }
  delete tcont;
  // 2 -- CHECK if the hit is at a resonable distance if the selected hit has a resonable distance count it!
  for (int ii=0;ii<4;ii++) tmhit[ii].MatchCheck();
 
  bool g_YMatch=tmhit[0].Ymatch && tmhit[3].Ymatch;
  bool g_XMatch=tmhit[1].Xmatch || tmhit[2].Xmatch;
  if ( ( (g_YMatch)&&(!g_XMatch) )||
       ( (g_YMatch)&&(log10(ptrack->GetNormChisqX(ptrack->Gettrdefaultfit()))>TRCLFFKEY.logChisqXmax) ) 
       ) { // try to move 

    AMSPoint p0(tmhit[0].phit->Coo);
    AMSPoint p1(tmhit[3].phit->Coo);
    AMSDir dir(p0-p1);
    return TrRecon::MoveTrTrack(ptrack, p0, dir,  3.8, select_tag);
    
  }
  return false;

}
 
bool TkTOFMatch3(TrTrackR* ptrack,int select_tag,int flag) {  
  class mtof_hitH{
  public:
    TofClusterHR * phit;
    AMSPoint min_d;
    number sleng;
    bool Xmatch;
    bool Ymatch;
    bool Tmatch;
    bool Lmatch;
    AMSPoint checker;
    mtof_hitH():phit(0),min_d(AMSPoint(0,0,0)),sleng(0),
		Xmatch(false),Ymatch(false),Tmatch(false),Lmatch(false)
    {
      //                    Transversal              Longitudinal  
      checker=AMSPoint(4.,4.,3.);
    }
    mtof_hitH(mtof_hitH &orig):phit(orig.phit),min_d(orig.min_d),sleng(orig.sleng),
			       Xmatch(orig.Xmatch),Ymatch(orig.Ymatch),
			       Tmatch(orig.Tmatch),Lmatch(orig.Lmatch),checker(orig.checker){}
    ~mtof_hitH(){phit=0;}
    number distT() {return (phit->Layer==0 ||phit->Layer==3)? min_d[1]:min_d[0];}
    void MatchCheck(){
      if(!phit) return;
      bool YY=(phit->Layer==0 ||phit->Layer==3);
      if (YY){
	Xmatch= (  min_d[0]   <=  checker[1]);
	Ymatch= (  min_d[1]   <=  checker[0]);
	Tmatch=Ymatch; 
	Lmatch=Xmatch;
      }
      else   { 
	Xmatch= (  min_d[0]   <=  checker[0]);
	Ymatch= (  min_d[1]   <=  checker[1]);
	Tmatch=Xmatch; 
	Lmatch=Ymatch;
      }
      return;
    }
    void Print(){
      printf("Coo:  %f %f %f   XM: %d YM: %d TM: %d LM: %d \n",
	     phit->Coo[0],
	     phit->Coo[1],
	     phit->Coo[2],
	     Xmatch,Ymatch,Tmatch,Lmatch
	     );
      min_d.Print();
    }
    
  }; 
  
  
  
  mtof_hitH tmhit[4];  
  
  
  // 0 -- Search for the geometrically matching TOF Hits  

  //-----new cont
  VCon* tcont = GetVCon()->GetCont("AMSTOFClusterH");
  
  for (int ii=0;ii<4;ii++)
    tmhit[ii].min_d= AMSPoint(1000,1000,1000);
 
  for (int ii=0;ii<tcont->getnelem();ii++){
    TofClusterHR* tofhit=(TofClusterHR*)tcont->getelem(ii);
    int  TOFlay=tofhit->Layer;
    // 1 -- FIND on each TOF layer the hit closer to the track
    if(!(tofhit->IsGoodTime())) continue;
    
    AMSDir dir(0,0,1.);
    AMSPoint outp,dst;
    AMSPoint ecoo(tofhit->ECoo);
    AMSPoint coo(tofhit->Coo);
    
    number theta,phi,sleng;   
    if(flag%10){
        ptrack->interpolate(coo,dir,outp,theta,phi,sleng);
    }else{
      AMSPoint p0=ptrack->GetP0();
      AMSPoint d0=ptrack->GetDir();
     sleng=0;
      outp[2]=coo[2];
      outp[0]=d0[0]/d0[2]*(outp[2]-p0[2])+p0[0];
      outp[1]=d0[1]/d0[2]*(outp[2]-p0[2])+p0[1];
    }
    if(ptrack->IsFake()){
      ecoo[0]+=1.;  // add one cm fot trd/tof
      ecoo[1]+=1.;  // add one cm fot trd/tof
    }
    
    dst=((outp-coo)/ecoo).abs();
    
    // search the closest along Y only
    if (fabs(dst[1]) <= tmhit[TOFlay].min_d[1]){
      // cout<<" The cluster "<<ii<<" is good"<<endl; 
      tmhit[TOFlay].min_d = dst.abs(); 
      tmhit[TOFlay].phit  = tofhit;
      tmhit[TOFlay].sleng = sleng;
    }
    // else cout<<" The cluster "<<ii<<" is bad"<<endl; 

  }
  delete tcont;
  // 2 -- CHECK if the hit is at a resonable distance if the selected hit has a resonable distance count it!
  for (int ii=0;ii<4;ii++) {
    tmhit[ii].MatchCheck();
    // tmhit[ii].Print();
  }
  int ymatch=0;
  for (int ii=0;ii<4;ii++) if (tmhit[ii].Ymatch) ymatch++;
 
  bool g_YMatch=tmhit[0].Ymatch && tmhit[3].Ymatch;
  g_YMatch=(ymatch>=2);
  bool g_XMatch=tmhit[1].Xmatch || tmhit[2].Xmatch;
  if ( ( (g_YMatch)&&(!g_XMatch) )||
       ((g_YMatch)&& (log10(ptrack->SimpleChi2X())>TRCLFFKEY.logChisqXmax) ) 
       ) { // try to move 

    double sx=0;
    double sx2=0;
    double sy=0;
    double sxy=0;
    double ssy=0;
    double ssxy=0;
    int    N=0;
    double x[4],y[4],ex[4],ey[4];
    for (int ii=0;ii<4;ii++){
      if (tmhit[ii].Ymatch){
	x[N]=tmhit[ii].phit->Coo[2];
	ex[N]=0;
	y[N]=tmhit[ii].phit->Coo[0];
	if(flag/10)ey[N]=tmhit[ii].phit->ECoo[0];
	else ey[N]=0;
	sx+=tmhit[ii].phit->Coo[2];
	sx2+=tmhit[ii].phit->Coo[2]*tmhit[ii].phit->Coo[2];
	sy+=tmhit[ii].phit->Coo[0];
	sxy=tmhit[ii].phit->Coo[0]*tmhit[ii].phit->Coo[2];

	ssy+=tmhit[ii].phit->Coo[1];
	ssxy=tmhit[ii].phit->Coo[1]*tmhit[ii].phit->Coo[2];
	N++;
      }
    }
    double Delta=N*sx2-sx*sx;
    double qx=1/Delta*(sx2*sy - sx *sxy);
    double mx=1/Delta*(N*sxy - sx *sy);

    TGraphErrors gr(N,x,y,ex,ey);
    TFitResultPtr res=gr.Fit("pol1","QS");
    
    qx=(*res).Parameter(0);
    mx=(*res).Parameter(1);

     double qy=1/Delta*(sx2*ssy - sx *ssxy);
     //double my=1/Delta*(N*ssxy - sx *ssy);
     
    AMSPoint p0(qx,qy,0);

    AMSDir dir(mx,0,1);
    //  printf("-----------_>>>>>Moving Track");p0.Print();dir.Print();
    return TrRecon::MoveTrTrack(ptrack, p0, dir,  4., select_tag);
    
  }
  return false;

  
}
 
