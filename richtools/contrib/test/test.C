#include "test.h"
#include <iostream>

using namespace std;


Analysis::Analysis(TString file){
  chain=new AMSChain();
  chain->Add(file);
  RichPMTsManager::Init();
}


bool Analysis::Select(AMSEventR *event){
  if(
     event->nParticle()!=1 ||
     event->pParticle(0)->pRichRing()==0 ||
     event->pParticle(0)->pRichRing()->pTrTrack()==0
     ) return false;
  return true;
}



void Analysis::Loop(){
  AMSEventR *event;
  unsigned int event_number=-1;
  int i=0;
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number) break;
    event_number=event->Event();
    cout<<"NEW EVENT "<<i++<<endl;
    if(!Select(event)) continue;

    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
    RichRingR *ring_ntuple=event->pParticle(0)->pRichRing();

    if(ring){
      cout<<"Betas "<<ring_ntuple->Beta<<" "<<ring->getbeta()<<endl;
      cout<<"Used "<<ring_ntuple->Used<<" "<<ring->_used<<endl;
      cout<<"MUsed "<<ring_ntuple->UsedM<<" "<<ring->_mused<<endl;
    }


  }
}


int main(){
  // Initalize the helper classes
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
  RichAlignment::Init(false);

  // Do not compute charge (this saves the initialization time)
  RichRing::ComputeNpExp=false;

  // Set the window size
  RichRing::SetWindow(5);

  // Start the MC analysis
  Analysis analysis("/f2users/mdelgado/production/mc/97674199702889435885.root");
  analysis.Loop();

  // Start the data analysis
  RichAlignment::Init(true);
  Analysis analysisD("/f2users/mdelgado/production/data/1210846309.*");
  analysisD.Loop();

  return 0;
}
