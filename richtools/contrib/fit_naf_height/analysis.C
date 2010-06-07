#include "analysis.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include <iostream>

using namespace std;


TF1 *Fita(TH1D *proyection,double &mean,double &mean_error,double &sigma,double &sigma_error){
  if(proyection->GetEntries()<50 || proyection->Integral(1,proyection->GetNbinsX())<50) return 0;
  if(proyection->Integral(
			  proyection->GetXaxis()->FindFixBin(0.962),
			  proyection->GetXaxis()->FindFixBin(0.99))<50) return 0;

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

      //      proyection->Fit("gaus","","",mean-2.5*sigma,mean+2.5*sigma);
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
  ///////////////////////////////////
  // CHAIN AND REC. INITIALIZATION //
  ///////////////////////////////////

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
  RichRing::SetWindow(2.75);

  // Use only direct hits to simplify the analysis: this biases the reconstruction unless 
  // events with a enugh fraction of ring are detected, thus we use both direct and reflected
  // to avoid this
  RichRing::UseDirect=true;
  RichRing::UseReflected=true;
  

  // Set the alignment: not done until expansion length is corrected 
  RichAlignment::Set(0,0,0,
		     0,0,0);
  RichAlignment::SetMirrorShift(0,0,0);
  RichRing::DeltaHeight=0;

  // Histograms
  beta_hit_direct=   TH2F("Beta Hit Direct",   "Beta Hit Direct",   31,-1,1,1000,0.95,1.05);
  beta_hit_reflected=TH2F("Beta Hit Reflected","Beta Hit Reflected",31,-1,1,1000,0.95,1.05);


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
  SELECT("At most 1 TrdTrack",event->nTrdTrack()<=1);
  SELECT("At most 4 TofClusters",event->nTofCluster()<=4);

  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->pTrTrack());
  SELECT("Clean ring",(event->pParticle(0)->pRichRing()->Status&1)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>5);

  // NaF
  SELECT("NaF",(event->pParticle(0)->pRichRing()->Status&2)!=0);

  return true;
}





void Analysis::Loop(){
  AMSEventR *event;
  unsigned int event_number=-1;
  unsigned int run_number=-1;
  int i=0;

  while (event = chain->GetEvent()) {

    // Checks to exit the loop
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();

    // Event selection
    if(!Select(event)) continue;

    // Reconstruction
    for(int i=1;i<=beta_hit_direct.GetNbinsX();i++){
      RichRing::DeltaHeight=beta_hit_direct.GetXaxis()->GetBinCenter(i);
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      if(!ring) continue;
      if(RichRing::_kind_of_tile==naf_kind){ // Further check
	for(int j=0;j<ring->_hit_used.size();j++){
	  beta_hit_direct.Fill(RichRing::DeltaHeight,ring->_beta_direct[j]);
	  beta_hit_reflected.Fill(RichRing::DeltaHeight,ring->_beta_reflected[j]);
	}
      }
      delete ring;
    }
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


  TFile output("fit_naf_height.root","RECREATE");

  TGraphErrors mean_direct(analysis.beta_hit_direct.GetNbinsX());
  TGraphErrors sigma_direct(analysis.beta_hit_direct.GetNbinsX());
  TGraphErrors mean_reflected(analysis.beta_hit_direct.GetNbinsX());
  TGraphErrors sigma_reflected(analysis.beta_hit_direct.GetNbinsX());

  for(int i=1;i<=analysis.beta_hit_direct.GetNbinsX();i++){
    double mean,emean,sigma,esigma;

    // Deal with each histogram
    TH1D *proy=analysis.beta_hit_direct.ProjectionY("",i,i);
    

    if(proy && Fita(proy,mean,emean,sigma,esigma)){
      mean_direct.SetPoint(i,analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i),mean);
      sigma_direct.SetPoint(i,analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i),sigma);
      mean_direct.SetPointError(i,0,emean);
      sigma_direct.SetPointError(i,0,esigma);
      proy->Write(Form("Direct %g",analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i)));
    }

    proy=analysis.beta_hit_reflected.ProjectionY("",i,i);

    if(proy && Fita(proy,mean,emean,sigma,esigma)){
      mean_reflected.SetPoint(i,analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i),mean);
      sigma_reflected.SetPoint(i,analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i),sigma);
      mean_reflected.SetPointError(i,0,emean);
      sigma_reflected.SetPointError(i,0,esigma);
      proy->Write(Form("Reflected %g",analysis.beta_hit_direct.GetXaxis()->GetBinCenter(i)));
    }

  }

  mean_direct.Write    ("Mean Direct");
  sigma_direct.Write   ("Sigma Direct");
  mean_reflected.Write ("Mean Reflected");
  sigma_reflected.Write("Sigma Reflected");
  

  // Output the selection efficiencies
  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }


  return 0;
}
