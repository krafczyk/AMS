#include "AMSNtupleHelper.h"
class AMSNtupleSelect: public AMSNtupleHelper{
public:
AMSNtupleSelect(){};
bool IsGolden(AMSEventR *ev){
// 
// This is a user function to be modified
//  return true if event has to be drawn false otherwise
//
 if(ev && ev->nParticle()>0 && ev->nVertex()>0){
   return true;
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
