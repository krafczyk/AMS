//  $Id: AMSHist.cxx,v 1.6 2001/01/22 17:32:52 choutko Exp $
#include "AMSHist.h"
ClassImp(AMSHist)
Int_t AMSHist::DispatchHist(Int_t cset){
  if(cset>=0){
    ShowSet(cset);
    return 0;
  }
  else{
    ShowSet(_cSet);
    _cSet=(_cSet+1)%_mSet;
    if(_mSet==1)_cSet=0;
    return _cSet;
  }
  
}




AMSHist::~AMSHist(){
  if(_fetched2)delete[] _fetched2;
  if(_filled2)delete[] _filled2;
}

void AMSHist::Fill(AMSNtuple *ntuple){
}

void AMSHist::Reset(){
  for(int i=0;i<_m2filled;i++){
   if(_filled2[i])_filled2[i]->Reset();
  }
}
