#include <iostream.h>
#include "AMSAxAMSHist.h"
#include "AMSDisplay.h"
AMSAxAMSHist::AMSAxAMSHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSAxAMSHist::_Fill(){
}



void AMSAxAMSHist::_Fetch(){

}


void AMSAxAMSHist::ShowSet(Int_t Set){


}

