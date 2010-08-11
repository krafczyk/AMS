#include "analysis.h"
#include <iostream>
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "time.h"
#include<fstream>

using namespace std;
const float window_size=3;
const int analysis_bins=11;


const int nSamples=100*2;
//const int nSamples=500;
const long int maxReconstructions=230000/2;

AMSEventList amsevents;

void Analysis::Init(){
  // Init everything
  chain->Rewind();
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();   
  RichAlignment::Init(false);        // We use the MC alignment: no alignment at all  

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;
}

Analysis::Analysis(TString file){
  chain=new AMSChain();
  chain->Add(file);
}

#define SELECT(name,cond) {if(cond){                                    \
                                   if(!selected[last_cut])             \
                                     sprintf(cut_name[last_cut],name); \
                                   selected[last_cut++]++;             \
                                   }else return false;}



bool Analysis::Select(AMSEventR *event){
  last_cut=0;

  SELECT("All events",true);
  SELECT("No antis",event->nAntiCluster()==0);
  SELECT("With 1 particle",event->nParticle()==1);
  SELECT("With 1 TrTrack",event->nTrTrack()==1);
  SELECT("At most 1 TrdTrack",event->nTrdTrack()<=1);
  SELECT("At most 4 TofClusters",event->nTofCluster()<=4);

  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->iTrTrack()!=-1);
  SELECT("Aerogel track and clean ring",(event->pParticle(0)->pRichRing()->Status&3)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);
  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-1);  // This cut is not fully understood

  //  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>20);


  amsevents.Add(event);
  return true;
}

void Analysis::Loop(){
  Init();                       // Initialize everything

  // Counter to artificially look for EOF
  unsigned int event_number=-1;
  unsigned int run_number=-1;

  AMSEventR *event;
  long int prev=-1;
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();
    Select(event);
  } // End of event loop
}



int main(int argc,char **argv){
  if(argc<3){
    cout<<"First argument should be the filename for data"<<endl;
    cout<<"Last argument should be the filename for output"<<endl;
    return 0;
  }

  Analysis analysis;
  for(int i=1;i<argc-1;i++) analysis.chain->Add(argv[i]);

  analysis.Loop();

  TFile evtList(argv[argc-1],"RECREATE");
  amsevents.Write(analysis.chain,&evtList);
  evtList.Close();


  // Output the selection efficiencies

  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }

  return 0;
}
