#include <iostream.h>
#include "AMSLVL3Hist.h"
#include "AMSDisplay.h"
ClassImp(AMSLVL3Hist)

AMSLVL3Hist::AMSLVL3Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSLVL3Hist::_Fill(){
}



void AMSLVL3Hist::_Fetch(){

}


void AMSLVL3Hist::ShowSet(Int_t Set){


}

