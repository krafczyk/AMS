#include <iostream.h>
#include "AMSTOFHist.h"
#include "AMSDisplay.h"
ClassImp(AMSTOFHist)

AMSTOFHist::AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSTOFHist::_Fill(){
}



void AMSTOFHist::_Fetch(){

}


void AMSTOFHist::ShowSet(Int_t Set){


}

