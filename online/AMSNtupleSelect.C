#include "AMSNtupleHelper.h"
extern void* gAMSUserFunction;
extern AMSNtupleHelper* AMSNtupleHelper::fgHelper;
class AMSNtupleSelect: public AMSNtupleHelper{
public:
AMSNtupleSelect(){cout <<"hhhhhhh"<<endl;};
bool IsGolden(AMSEventR *ev){
 if(ev && ev->nParticle()>0 && ev->pParticle(0)->pTrTrack()==0 && ev->pParticle(0)->pTrdTrack() && ev->pParticle(0)->pEcalShower()==0){
  double phi=ev->pParticle(0)->Phi*180./3.1415926;
  if(fabs(phi-180)<0.1 ){
      TrdTrackR *ptr= ev->pParticle(0)->pTrdTrack();
    cout <<ptr->Phi<<" "<<ptr->NTrdSegment()<<endl;
    for(int i=0;i<ptr->NTrdSegment();i++){
     TrdSegmentR *ps=ptr->pTrdSegment(i);
     cout <<i<<" "<<ps->Orientation<<" "<<ps->FitPar[0]<<" "<<ps->FitPar[1]<<" "<<ps->Chi2<<" "<<ps->NTrdCluster()<<endl;
     if(ps->Orientation==0){
       for(int j=0;j<ps->NTrdCluster();j++){
        TrdClusterR* pcl=ps->pTrdCluster(j);
        cout <<" cl "<<j<<" "<<pcl->Coo[0]<<" "<<pcl->Coo[1]<<" "<<pcl->Coo[2]<<" "<<endl;
       }
     }
    }
    
    return true;
  }
  else return false;
 }
 else return false;
}
};

class AMSNtupleSelectI{
public:
AMSNtupleSelectI(){
 void ** cp=(void**)0x812e44c;
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<(void*)&AMSNtupleHelper::fgHelper<<" "<<gAMSUserFunction<<endl; 
}
};
AMSNtupleSelectI fgSelect;
bool AMSNtupleHelper::IsGolden(AMSEventR *o){
return true;
}


