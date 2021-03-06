
#include "AMSNtupleHelper.h"

static AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;

class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
    static int a=0;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise
    //a
    if(ev && ev->Event()%2==0) return true;
    else return false;
    if(ev &&   ev->nTrTrack()){
      a++;
      cout <<ev->Event()<<endl;
      cerr<< " Number "<<a<<endl;
      return true;
      
    }
    else{
      return false;
    }
    //if(ev && ev->nRichRing() && ev->nEcalShower() && ev->nTrTrack())return true;
    //else return false;
    //if(ev && ev->nTrTrack())return true;
    //else return false;
    if(ev && ev->nParticle()>0){
      if(ev->nEcalShower()>0 && ev->nTrdTrack()>0 && ev->nTrTrack()&&ev->nRichRing()>0 && ev->Particle(0).iTrdTrack()>=0 && ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>5.)return true;
      else return false;    
      if((ev->Particle(0)).Phi>3.14 &&  (ev->Particle(0)).Phi<3.16 )return true;
      else return false;
    }
    else return false;
    
    /*
      if(ev &&  ev->nTrdTrack()==1 &&(ev->TrdTrack(0)).NTrdSegment()>2 ){
      cout <<"working "<<(ev->TrdTrack(0)).NTrdSegment()<<endl;
      int ib=0;
      TrdTrackR trd=ev->TrdTrack(0);   
      for(int i=0;i<trd.NTrdSegment();i++){
      TrdSegmentR s=ev->TrdSegment(trd.iTrdSegment(i));
      for (int j=0;j<s.NTrdCluster();j++){
      TrdClusterR c=ev->TrdCluster(s.iTrdCluster(j));
      TrdRawHitR h=ev->TrdRawHit(c.iTrdRawHit());
      if(h.Layer<4 and h.Ladder==10){
      cout <<c.Coo[0]<<" "<<c.Coo[1]<<" "<<c.Coo[2]<<" "<<c.Layer<<endl;
      ib++;
      break;
      //return true;
      }
      }
      for (int j=0;j<s.NTrdCluster();j++){
      TrdClusterR c=ev->TrdCluster(s.iTrdCluster(j));
      TrdRawHitR h=ev->TrdRawHit(c.iTrdRawHit());
      if(h.Layer>15 and h.Ladder==8){
      cout <<c.Coo[0]<<" "<<c.Coo[1]<<" "<<c.Coo[2]<<" "<<c.Layer<<endl;
      if(ib>0)return true;
      }
      }
      }
      return false;
      }
    */
    /*
      if(ev && ev->nParticle()>0){
      //return true;
      }
      if(ev && (ev->nTrTrack()>0 || ev->nRichRing()>0)){
      return true;
      }
      if(ev && ev->nAntiCluster()>0){
      //return true;
      }
      if(ev &&  ev->nTrdTrack()==1 &&(ev->TrdTrack(0)).NTrdSegment()>3 ){
      int count[20];
      for(int k=0;k<20;k++)count[k]=0;
      for(int i=0;i<(ev->TrdTrack(0)).NTrdSegment();i++){
      TrdSegmentR s=ev->TrdSegment((ev->TrdTrack(0)).iTrdSegment(i));
      for (int j=0;j<s.NTrdCluster();j++){
      TrdClusterR c=ev->TrdCluster(s.iTrdCluster(j));
      TrdRawHitR h=ev->TrdRawHit(c.iTrdRawHit());
      int id=10000+h.Layer*100+h.Ladder;
      for(int k=0;k<20;k++){
      float thr=2+float(k)/10.;
      if(c.EDep>thr){
      count[k]++;
      }
      }
      }
      }
      //    if(count[19]>10)return true;
      //    else return false;
      int nmemb=0;
      int nlay=0;
      for (int i=0;i<ev->nTofCluster();i++){
      if(ev->TofCluster(i).Layer<3){
      nlay+=ev->TofCluster(i).Layer;
      if(ev->TofCluster(i).NTofRawCluster()==2)nmemb+=ev->TofCluster(i).Layer;
      }
      }
      if(nlay==3 && nmemb==3)return true;
      else return false;     
      TrdTrackR trd=ev->TrdTrack(0);
      double nx=sin(trd.Theta)*cos(trd.Phi);
      double ny=sin(trd.Theta)*sin(trd.Phi);
      double nz=cos(trd.Theta);
      for (int i=0;i<ev->nTofCluster();i++){
      if(ev->TofCluster(i).Layer==1 ){
      number yc=ev->TofCluster(i).Coo[1];
      number zc=ev->TofCluster(i).Coo[2];
      
      number cross=trd.Coo[1]+(-trd.Coo[2]+zc)*ny/nz;
      if(fabs(cross-yc)>10){
      cout <<" "<<cross<<" "<<ev->TofCluster(i).Coo[1]<<" "<<trd.Coo[2]<<" "<<trd.Coo[1]<<" "<<ny<<" "<<nz<<" "<<zc<<endl;
      return true;
      }
      }
      else if(ev->TofCluster(i).Layer==2){
      number xc=ev->TofCluster(i).Coo[0];
      number zc=ev->TofCluster(i).Coo[2];
      number cross=trd.Coo[0]+(zc-trd.Coo[2])*nx/nz;
      //if(fabs(cross-xc)>7)return true;
      }
      }
      return false;
      }     
      else return false;
    */
    if(ev && ev->nParticle()>0 && ev->nTrTrack()>0){
      TrTrackR tr=ev->TrTrack(0);
      const int nbadl=12;
      int blay[12]={1, 1, 1, 1, 2 ,  2,   4,   6,   7,   7,   8,   8};
      int blad[12]={1,   1,  15,  15,   1,  14,   7,  12,   1,  14,   7,  15};
      int bhalf[12]={0 ,  1,   0,   1,   0,   1,   1,   1,   1,   0,   0,   1};
      for(int i=0;i<tr.NTrRecHit();i++){
	TrRecHitR rh=ev->TrRecHit(tr.iTrRecHit(i));
	int lay,lad,half,stripx,stripy;
	rh.Geom(lay,lad,half,stripx,stripy);
        for(int k=0;k<nbadl;k++){
	  if(lay==blay[k] && lad==blad[k] && half==bhalf[k]){
	    return true;
	  }
	}
      }
    }
    return false;
  }
};

//
//  The code below should not be modified
//
#ifndef WIN32
extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loadedd "<<endl;
}

#else
#include <windows.h>
BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle
	     DWORD fdwReason,              // reason called 
	     LPVOID lpvReserved)           // reserved 
{ 
 
  switch (fdwReason) 
    { 
      // The DLL is loading due to process 
      // initialization or a call to LoadLibrary. 
 
    case DLL_PROCESS_ATTACH: 
      fgHelper=new AMSNtupleSelect();
      cout <<"  Handle Loadedd "<<fgHelper<<endl;
      break;

    case DLL_PROCESS_DETACH: 
      if(fgHelper){
	delete fgHelper;
	fgHelper=0;
      } 
 
      break; 
 
    default: 
      break; 
    } 
  return TRUE;
}
extern "C" AMSNtupleHelper * gethelper(){return fgHelper;}

#endif
