#include "analysis.h"
#include <iostream>
#include <memory>
#include "TF1.h"
#include "TGraphErrors.h"

using namespace std;


// This function fits the beta distribution peak at beta=1 using an iterative
// approach to refine the fit. 
void Fit(TH1 &beta){
  cout<<endl<<"Fitting...";
  // Fit a gaussian to a  histogram
  double xmin=beta.GetXaxis()->GetXmin();
  double xmax=beta.GetXaxis()->GetXmax();

  double prev_norma=-1;
  double norma,mean,sigma;

  if(beta.GetEntries()<100) return;

  for(int i=0;i<10;i++){
    if(xmin>beta.GetXaxis()->GetXmax() || xmax<beta.GetXaxis()->GetXmin() || xmin>=xmax) return;  // Fail

    beta.Fit("gaus","","",xmin,xmax);
    TF1 *func=beta.GetFunction("gaus");
    norma=func->GetParameter(0);
    mean=func->GetParameter(1);
    sigma=fabs(func->GetParameter(2));

    xmin=mean-1.0*sigma;
    xmax=mean+2.0*sigma;
    if(fabs(norma-prev_norma)<1e-3) break;

    prev_norma=norma;

  }
  cout<<"done*****"<<endl;
}


void Analysis::Init(){
  // Init everything
  chain->Rewind();
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
  if(!isMC) RichAlignment::Init(true);
  else RichAlignment::Set(0,0,0,0,0,0); // reset the alignment just in case

  // Compute charge since some paramters used by the
  // selecteion are obtained during that computation
  RichRing::ComputeNpExp=true;

  // Use a big window


  // Use direct and reflected hits
  RichRing::UseDirect=true;
  RichRing::UseReflected=true;

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;
}

Analysis::Analysis(TString file){
  chain=new AMSChain();
  chain->Add(file);

  // Determine if this is data or MC
  isMC=false;
  for(int i=0;i<10;i++)
    if(chain->GetEvent()->nMCEventg())
      {
	isMC=true;
	break;
      }
}

// this macro simplifies the preselecion code
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
  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->iTrTrack()!=-1);
  SELECT("Aerogel track and clean ring",(event->pParticle(0)->pRichRing()->Status&2)==0);

  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);

  RichRingR *ring=event->pParticle(0)->pRichRing();

  // Count used unused hits and position of the hotspot
  float weight=0;
  float x=0;
  float y=0;
  float unused_charge=0;
  int unused_hits=0;
  const int hotspot_flag=(1<<30);
  const int used_flag=(1<<(event->pParticle(0)->iRichRing()));
  for(int i=0;i<event->nRichHit();i++){
    RichHitR *hit=event->pRichHit(i);
    int status=hit->Status;
    if(fabs(hit->Coo[0]-ring->TrPMTPos[0])>3.4/2 && fabs(hit->Coo[1]-ring->TrPMTPos[1])>3.4/2)
      if(!(status&used_flag) && !(status&hotspot_flag)) {unused_charge+=hit->Npe;unused_hits++;}
    if(!(status&hotspot_flag)) continue;
    weight+=hit->Npe;
    x+=hit->Coo[0]*hit->Npe;
    y+=hit->Coo[1]*hit->Npe;
  }

  // This selection decreases the systematic
  if(weight!=0){
    x/=weight;
    y/=weight;
    x-=ring->TrPMTPos[0];
    y-=ring->TrPMTPos[1];
    SELECT("Hot spot associted to track",x*x+y*y<4*0.7*0.7);
  } else SELECT("Hot spot associted to track",1);


  // Select good track and beta=1 (for data)
  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-1);


  return true;
}

const double initial_window=0.5;
const double final_window=6;
const double delta_window=initial_window;


void Analysis::Loop(){
  Init();                       // Initialize everything

  // Counter to artificially look for EOF due to the bug in AMSRoot
  unsigned int event_number=-1;
  unsigned int run_number=-1;

  AMSEventR *event;
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();


    if(!Select(event)) continue;

    int counter=-1;
    for(double window_size=initial_window;window_size<=final_window;window_size+=delta_window){
      counter++;

      RichRing::SetWindow(window_size);  

      // Reconstruct the ring
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack()); // Be careful: this reserves the memory, thus it should be freed in some place 
    
      if(!ring) continue;

      // The next macro takes care of freeing the ring memory
#define next {delete ring;continue;}
      // Some standard quality cuts
      if(ring->_used<3)        next;
      if(ring->_probkl<1e-1)   next;
      if(ring->_npexp<3)       next;
      if((ring->_status&1)!=0) next;//continue;  // no noisy 
#undef next
      
      // Fill histograms
      if(beta_vs_used[counter].GetNbinsX()<20){
	beta_vs_used[counter].SetBins(20,0,20,200,0.99,1.01);
	beta_vs_used[counter].SetName(Form("Beta vs Used for %g",window_size));
      }

      beta_vs_used[counter].Fill(ring->_used,ring->_beta);

      delete ring;  // free the memory      
    }
  } // End of event loop
}


int main(int argc,char **argv){
  if(argc<3){
    cout<<"First argument should be the filename for data"<<endl;
    cout<<"Second argument should be the filename for MC"<<endl;
    return 0;
  }

  // Run the event loop
  Analysis analysis_data(argv[1]);
  analysis_data.Loop();

  Analysis analysis_mc(argv[2]);
  analysis_mc.Loop();

  
  // Create all the required histograms
  TFile f("Results_optimization.root","RECREATE");
    int counter=-1;
    for(double window_size=initial_window;window_size<=final_window;window_size+=delta_window){
      counter++;
      if(analysis_data.beta_vs_used[counter].GetNbinsX()<20 ||
	 analysis_mc.beta_vs_used[counter].GetNbinsX()<20) continue;

      TGraphErrors sigma_beta_data(analysis_data.beta_vs_used[counter].GetNbinsX());
      TGraphErrors sigma_beta_mc(analysis_data.beta_vs_used[counter].GetNbinsX());
      TGraphErrors mean_beta_data(analysis_data.beta_vs_used[counter].GetNbinsX());
      TGraphErrors mean_beta_mc(analysis_data.beta_vs_used[counter].GetNbinsX());


      for(int i=1;i<=analysis_data.beta_vs_used[counter].GetNbinsX();i++){
	int n_used=i-1;
	TH1D* current=analysis_data.beta_vs_used[counter].ProjectionY(Form("Data beta_used=%i window=%f",n_used,window_size),i,i);
	Fit(*current);
	if(!current->GetFunction("gaus")) continue;
	mean_beta_data.SetPoint(n_used,n_used,current->GetFunction("gaus")->GetParameter(1));
	mean_beta_data.SetPointError(n_used,0,current->GetFunction("gaus")->GetParError(1));
	sigma_beta_data.SetPoint(n_used,n_used,current->GetFunction("gaus")->GetParameter(2));
	sigma_beta_data.SetPointError(n_used,0,current->GetFunction("gaus")->GetParError(2));
	
	current=analysis_mc.beta_vs_used[counter].ProjectionY(Form("MC beta_used=%i window=%f",n_used,window_size),i,i);
	Fit(*current);
	if(!current->GetFunction("gaus")) continue;
	mean_beta_mc.SetPoint(n_used,n_used,current->GetFunction("gaus")->GetParameter(1));
	mean_beta_mc.SetPointError(n_used,0,current->GetFunction("gaus")->GetParError(1));
	sigma_beta_mc.SetPoint(n_used,n_used,current->GetFunction("gaus")->GetParameter(2));
	sigma_beta_mc.SetPointError(n_used,0,current->GetFunction("gaus")->GetParError(2));
      }

      sigma_beta_data.Write(Form("Data Sigma Beta window=%f",window_size));
      mean_beta_data.Write(Form("Data Mean Beta window=%f",window_size));
      sigma_beta_mc.Write(Form("MC Sigma Beta window=%f",window_size));
      mean_beta_mc.Write(Form("MC Mean Beta window=%f",window_size));
      
    }

  f.Close();


  // Output the selection efficiencies

  for(int i=1;analysis_data.selected[i]>0;i++){
    cout<<"CUT "<<analysis_data.cut_name[i]
	<<" DATA ENTRIES "<<analysis_data.selected[i]<<" DATA EFF "<<double(analysis_data.selected[i])/analysis_data.selected[i-1]
	<<" MC ENTRIES "<<analysis_mc.selected[i]<<" MC EFF "<<double(analysis_mc.selected[i])/analysis_mc.selected[i-1]<<endl;
  }


  return 0;

}      
