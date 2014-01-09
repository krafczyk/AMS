#include "TrTrack.h"
#include "TrRecon.h"
#include "root.h"


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
  
  
  
  int mfit = TrTrackR::kSimple;
  mtof_hitH tmhit[4];  
  
  
  // 0 -- Search for the geometrically matching TOF Hits  
  int tofcoo[4]={1,0,0,1};

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
 
