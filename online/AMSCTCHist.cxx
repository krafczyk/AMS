#include <iostream.h>
#include "AMSCTCHist.h"
#include "AMSDisplay.h"
ClassImp(AMSCTCHist)
AMSCTCHist::AMSCTCHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSCTCHist::_Fill(){
}



void AMSCTCHist::_Fetch(){

}


void AMSCTCHist::ShowSet(Int_t Set){


}

