#include "AMSNtupleHelper.h"
class AMSNtupleSelect: public AMSNtupleHelper{
public:
AMSNtupleSelect(){};
bool IsGolden(AMSEventR *ev){
// 
// This is a user function to be modified
//  return true if event has to be drawn false otherwise
//
 if(ev && ev->nParticle()>0 ){
// if(ev && ev->nParticle()>0 && ev->nRichRing()>0){
   for(int i=0;i<ev->nParticle();i++){
    if(ev->pParticle(i)->Particle==1){
     cout <<" foundd particle "<<ev->pParticle(i)->Particle<<" "<<ev->pParticle(i)->Momentum<<endl;
     return true;  
    }
   }
   return false;
   for(int i=0;i<ev->nRichRing();i++){
    if(ev->pRichRing(i)->UsedM)return true;
   }
   return false;
 }
  else return false;
}
};

//
//  The code below should not be modified
//

extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loaded "<<endl;
}
