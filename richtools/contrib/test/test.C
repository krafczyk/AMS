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

    delete ring;
  }
}


int main(int argc,char **argv){
  if(argc<2){
    cout<<"First argument should be the filename"<<endl;
    return 0;
  }


  // Initalize the helper classes
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
  RichAlignment::Init(false);

  // Do not compute charge (this saves the initialization time)
  RichRing::ComputeNpExp=false;

  // Set the window size
  RichRing::SetWindow(5);

  // Check if the tree is MC data
  AMSChain chain;
  chain.Add(argv[1]);
  bool isMC=false;
  for(int i=0;i<10;i++)
    if(chain.GetEvent()->nMCEventg())
      {
	isMC=true;
	break;
      }

  // If not MC 
  if(!isMC) RichAlignment::Init(true);

  // Start the Analysis
  Analysis analysis(argv[1]);
  analysis.Loop();

  return 0;
}
