#include "AMSNtupleHelper.h"
extern void* gAMSUserFunction;
extern AMSNtupleHelper* AMSNtupleHelper::fgHelper;
class AMSNtupleSelect: public AMSNtupleHelper{
public:
AMSNtupleSelect(){cout <<"hhhhhhh"<<endl;};
bool IsGolden(AMSEventR *ev){
// 
// This is a user function to be modified
//  return true if event has to be drawn false otherwise
//
 if(ev && ev->nParticle()>0 && ev->nAntiCluster()>0){
   for(int i=0;i<ev->nRichRing();i++){
    if(ev->pRichRing(i)->UsedM)return true;
   }
   return false;
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


