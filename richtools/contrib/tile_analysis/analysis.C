#include "test.h"
#include "TF1.h"
#include <iostream>

using namespace std;


Analysis::Analysis(TString file){
  chain=new AMSChain();
  chain->Add(file);

  // Init everything
  chain->Rewind();
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();   
  RichAlignment::Init(false);        // We use the MC alignment: no alignment at all  

  // Do not compute charge
  RichRing::ComputeNpExp=false;

  // Use the standard window
  //  RichRing::SetWindow(2.75);
  RichRing::SetWindow(5.0);

  // Use only direct hits to simplify the analysis: this biases the reconstruction unless 
  // events with a enugh fraction of ring are detected, thus we use both direct and reflected
  // to avoid this
  RichRing::UseDirect=true;
  RichRing::UseReflected=false;
  

  // Set the alignment
  RichAlignment::Set(0,0,0,0,0,0);
  RichRing::DeltaHeight=0;
  // FOR REAL DATA ONLY
#define REALDATA
#ifdef REALDATA
  RichAlignment::Set(0,0.15,0.5,0,0,0);
  RichRing::DeltaHeight=-0.655608413971428550; 
#endif

  for(int i=0;i<121;i++){
    beta_tile[i]=TH1F(Form("beta tile %i",i),
		      Form("beta tile %i",i),
		      1000/8,0.95,1.05);

    final_beta[i]=TH1F(Form("Final Beta %i",i),
		       Form("Final Beta %i",i),
		       1000/4,0.95,1.05);

    final_beta_vs_used[i]=TH2F(Form("Final Beta vs used %i",i),
			       Form("Final Beta vs used %i",i),
			       1000/4,0.95,1.05,20,1,21);

    beta_used[i]=TH1F(Form("beta used %i",i),
		      Form("beta used %i",i),
		      1000/8,0.95,1.05);

    beta_vs_beta_used[i]=TH2F(Form("beta vs beta used %i",i),
			      Form("beta vs beta used %i",i),
			      1000/4,0.95,1.05,
			      1000/8,0.95,1.05);

  }



  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

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
  //  SELECT("With 1 TrTrack",event->nTrTrack()==1);
  SELECT("At most 1 TrdTrack",event->nTrdTrack()<=1);
  SELECT("At most 4 TofClusters",event->nTofCluster()<=4);

  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->pTrTrack());
  //  SELECT("Aerogel track and clean ring",(event->pParticle(0)->pRichRing()->Status&3)==0);
  SELECT("Clean ring",(event->pParticle(0)->pRichRing()->Status&1)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>4);
  //  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>10);
  RichRingR *ring=event->pParticle(0)->pRichRing();




  return true;
}





void Analysis::Loop(){
  AMSEventR *event;
  unsigned int event_number=-1;
  unsigned int run_number=-1;
  int i=0;

  while (event = chain->GetEvent()) {
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();

    if(!Select(event)) continue;


    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
    if(!ring) continue;
    
    
    if(ring->_used==4) final_beta[RichRing::_tile_index].Fill(ring->_beta);
    final_beta_vs_used[RichRing::_tile_index].Fill(ring->_beta,ring->_used);

    // fill per window excluding the reflected ones
    for(int j=0;j<ring->_hit_used.size();j++){
      if(ring->_hit_used[j]==1) continue;
      if(ring->_hit_used[j]==0/* && ring->_used==4*/){
	beta_used[RichRing::_tile_index].Fill(ring->_beta_direct[j]);
	beta_vs_beta_used[RichRing::_tile_index].Fill(ring->_beta,ring->_beta_direct[j]);
      }
      beta_tile[RichRing::_tile_index].Fill(ring->_beta_direct[j]);
    }
    delete ring;
  }

}


int main(int argc,char **argv){
  if(argc<2){
    cout<<"First argument should be the filename for data"<<endl;
    return 0;
  }
  // Start the Analysis
  Analysis analysis(argv[1]);
  analysis.isMC=false;
  analysis.Loop();


  TFile file("tile_histos.root","RECREATE");
  
  for(int i=0;i<121;i++){
    if(analysis.beta_tile[i].GetEntries()<100) continue;
    analysis.beta_tile[i].Write();
    analysis.final_beta[i].Write();
    analysis.final_beta_vs_used[i].Write();
    analysis.beta_used[i].Write();
    analysis.beta_vs_beta_used[i].Write();
  }
  

  file.Close();

  // Output the selection efficiencies
  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }




  return 0;
}
