#include <iostream.h>
#include "AMSLVL1Hist.h"
#include "AMSDisplay.h"
ClassImp(AMSLVL1Hist)


AMSLVL1Hist::AMSLVL1Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSLVL1Hist::_Fill(){
}



void AMSLVL1Hist::_Fetch(){

}


void AMSLVL1Hist::ShowSet(Int_t Set){


}

