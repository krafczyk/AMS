#include "AMSHist.h"
ClassImp(AMSHist)
Int_t AMSHist::DispatchHist(Int_t cset){
  if(cset>=0){
   ShowSet(cset);
   return 1;
  }
  else{
   ShowSet(_cSet);
   _cSet=(_cSet+1)%_mSet;
   return _cSet;
  }

}




 AMSHist::~AMSHist(){
 if(_fetched2)delete[] _fetched2;
 if(_filled2)delete[] _filled2;
}

void AMSHist::Fill(AMSNtuple *ntuple){
}
