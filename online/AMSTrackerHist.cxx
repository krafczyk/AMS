#include <iostream.h>
#include "AMSTrackerHist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
AMSTrackerHist::AMSTrackerHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSTrackerHist::_Fill(){
}



void AMSTrackerHist::_Fetch(){


}


void AMSTrackerHist::ShowSet(Int_t Set){


}


void AMSTrackerHist::Fill(AMSNtuple * ntuple){
}
