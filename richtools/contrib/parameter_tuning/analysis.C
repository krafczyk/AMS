#include "analysis.h"
#include <iostream>
#include "TF1.h"

using namespace std;


TF1 *Fita(TH1F *proyection,double &mean,double &mean_error,double &sigma,double &sigma_error){
  if(proyection->GetEntries()<1000 || proyection->Integral(1,proyection->GetNbinsX())<1000) return 0;
      proyection->Fit("pol0","","",0.962,0.99);
      TF1 *f=proyection->GetFunction("pol0");
      if(!f) return 0;
      Double_t a=f->GetParameter(0);
      Double_t b=0;//f->GetParameter(1);


      for(int bin=1;bin<=proyection->GetNbinsX();bin++){
	double x=proyection->GetXaxis()->GetBinCenter(bin);
	proyection->SetBinError(bin,sqrt(proyection->GetBinContent(bin)));
	if(proyection->GetBinContent(bin)!=0)
	  proyection->SetBinContent(bin,proyection->GetBinContent(bin)-a-b*x);
	if(proyection->GetBinContent(bin)<-3*proyection->GetBinError(bin)){
	  proyection->SetBinContent(bin,0);
	  proyection->SetBinError(bin,0);
	}
      }
      proyection->Fit("gaus","","",0.98,1.02);
      f=proyection->GetFunction("gaus");
      if(!f) return 0;
      mean=f->GetParameter(1);
      sigma=fabs(f->GetParameter(2));
      mean_error=f->GetParError(1);
      sigma_error=f->GetParError(2);

      if(mean<proyection->GetXaxis()->GetXmin() ||
	 mean>proyection->GetXaxis()->GetXmax()) return 0;

      proyection->Fit("gaus","","",mean-1.5*sigma,mean+1.5*sigma);
      f=proyection->GetFunction("gaus");
      if(!f) return 0;
      mean=f->GetParameter(1);
      sigma=fabs(f->GetParameter(2));
      mean_error=f->GetParError(1);
      sigma_error=f->GetParError(2);

      return f;
}

Analysis::Analysis(TString file){
  chain=new AMSChain();
  chain->Add(file);

  // Init everything
  chain->Rewind();
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();   
  RichAlignment::Init(false);

  // Do not compute charge
  RichRing::ComputeNpExp=false;

  // Use the standard window
  RichRing::SetWindow(3); 


  // Use only direct hits to simplify the analysis: this biases the reconstruction unless 
  // events with a enugh fraction of ring are detected, thus we use both direct and reflected
  // to avoid this
  RichRing::UseDirect=true;
  RichRing::UseReflected=false;
  

  /*
  // Set the alignment
  RichRing::DeltaHeight=-0.655608413971428550;  // THIS IS FOR REALDATA
  //  RichAlignment::Set(-0.03,0.14,0.74,0,0,0);
  RichAlignment::Set(0,0.15,0.5,0,0,0);
  //  RichAlignment::SetMirrorShift(0.01,-0.03,-0.46); //From C.D. paper
  */

  for(int i=0;i<20;i++){
    betaVsWindow[i]=TH2F(Form("BetaVsWindow %i",i),Form("BetaVsWindow %i",i),50,0,5,1000,0.95,1.05);
    means[i]=TH1F(Form("Means %i",i),Form("Means %i",i),50,0,5);
    sigmas[i]=TH1F(Form("Sigmas %i",i),Form("Sigmas %i",i),50,0,5);
  }
  betahit=TH1F("betahit","betahit",1000,0.95,1.05);
}


#define SELECT(name,cond) {if(!(cond)) return false;}



bool Analysis::Select(AMSEventR *event){

  SELECT("All events",true);
  SELECT("No antis",event->nAntiCluster()==0);
  SELECT("With 1 particle",event->nParticle()==1);
  //  SELECT("With 1 TrTrack",event->nTrTrack()==1);
  SELECT("At most 1 TrdTrack",event->nTrdTrack()<=1);
  SELECT("At most 4 TofClusters",event->nTofCluster()<=4);

  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->pTrTrack());
  SELECT("Clean ring",(event->pParticle(0)->pRichRing()->Status&1)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);

  //  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>4);
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>10);
  SELECT("Agl ring",(event->pParticle(0)->pRichRing()->Status&2)==0);

  /*
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
  //  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-2);  // This cut is not fully understood
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>4);
  */
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


    for(int i=1;i<=betaVsWindow[0].GetNbinsX();i++){
      RichRing::SetWindow(betaVsWindow[0].GetXaxis()->GetBinCenter(i));      
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      if(!ring) continue;
      
      //      if(ring->_used!=5 && ring->_mused==0) {delete ring;continue;} // Fix the number of hits
      if(ring->_used<20) betaVsWindow[ring->_used].Fill(betaVsWindow[0].GetXaxis()->GetBinCenter(i),ring->_beta);
      delete ring;
    }
    RichRing::SetWindow(3.0);      
    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
    if(!ring) continue;

    for(int i=0;i<ring->_beta_direct.size();i++)
      betahit.Fill(ring->_beta_direct[i]);

    delete ring;
  }
}


int main(int argc,char **argv){
  if(argc<2){
    cout<<"First argument should be the filename"<<endl;
    return 0;
  }


  // Start the Analysis
  Analysis analysis(argv[1]);
  analysis.Loop();

  TFile file("parameter_tuning.root","RECREATE");

  for(int j=2;j<=10;j++)
  for(int i=1;i<=analysis.betaVsWindow[j].GetNbinsX();i++){
    double mean,sigma,mean_error,sigma_error;
    TH1F *histo=(TH1F*)analysis.betaVsWindow[j].ProjectionY(Form("Bin %i",i),i,i);
    if(!histo) continue;
    TF1 *func=Fita(histo,mean,mean_error,sigma,sigma_error);
    analysis.means[j].SetBinContent(i,mean);
    analysis.means[j].SetBinError(i,mean_error);
    analysis.sigmas[j].SetBinContent(i,sigma);
    analysis.sigmas[j].SetBinError(i,sigma_error);
    histo->Write();
  }
    

  for(int i=0;i<20;i++){
    analysis.betaVsWindow[i].Write();
    analysis.means[i].Write();
    analysis.sigmas[i].Write();
  }
  double d;
  //  Fita(&analysis.betahit,d,d,d,d);
  analysis.betahit.Write();
  file.Close();

  return 0;
}
