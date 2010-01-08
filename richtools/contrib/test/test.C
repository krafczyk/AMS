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


// Default RICH alignment
const double ox=2.06e-2, oy=6.16e-2, oz=9.05e-2-6.39e-2;

// Default Mirror alingment
const double omx=-0.074-0.0107, omy=0.092+0.0104, omz=0;


void Analysis::Loop(){
  AMSEventR *event;
  unsigned int event_number=-1;
  int i=0;
  RichAlignment::Set(ox,oy,oz,0,0,0);  // Just in case
  RichAlignment::SetMirrorShift(omx,omy,omz);  // Just in case
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number) break;
    event_number=event->Event();
    cout<<"NEW EVENT "<<i++<<endl;
    if(!Select(event)) continue;

    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());


    double best_beta=-1;
    double best_size=-1;
    for(double dy=-3.0;dy<=3.0;dy+=0.1){
      double beta,rms,size,p;
      RichAlignment::Set(ox,oy+dy,oz,0,0,0);
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      if(!ring) continue;
      ring->buildEM(beta,rms,p,size);
      if(size>best_size){
	best_beta=beta;
	best_size=size;
      }
      delete ring;
    }
    
    
    RichRingR *ring_ntuple=event->pParticle(0)->pRichRing();

    cout<<"BETAS "<<ring_ntuple->Beta<<" "<<best_beta<<endl;

    if(ring){
      //      cout<<"Betas "<<ring_ntuple->Beta<<" "<<ring->getbeta()<<endl;
      //      cout<<"Used "<<ring_ntuple->Used<<" "<<ring->_used<<endl;
      //      cout<<"MUsed "<<ring_ntuple->UsedM<<" "<<ring->_mused<<endl;

      //      double new_beta=ring->buildEM();

      //      cout<<endl<<"BETAS "<<ring_ntuple->Beta<<" "<<ring->getbeta()<<" "<<new_beta<<endl;

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
  //  RichRing::SetWindow(5);
  RichRing::SetWindow(3);

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
