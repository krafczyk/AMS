//  $Id: AMSHist.cxx,v 1.7 2003/06/17 07:39:53 choutko Exp $
#include "AMSHist.h"
ClassImp(AMSHist)
Int_t AMSHist::DispatchHist(Int_t cset){
  if(cset>=0){
    ShowSet(cset);
    _cSetl=cset;
    return 0;
  }
  else{
    ShowSet(_cSet);
    _cSetl=_cSet;
    _cSet=(_cSet+1)%SetName.size();
    if(SetName.size()==1)_cSet=0;
    return _cSet;
  }
  
}




AMSHist::~AMSHist(){
}

void AMSHist::Fill(AMSNtupleR *ntuple){
}

void AMSHist::Reset(){
  for(int i=0;i<_filled.size();i++){
   if(_filled[i])_filled[i]->Reset();
  
  }
}
