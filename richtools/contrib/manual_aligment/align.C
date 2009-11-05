#include "align.h"
#include "TMinuit.h"

//#define _DEBUG_

char *output="betas.root";
Alignment* Alignment::THIS=0;

Alignment::~Alignment(){
  delete chain;
}

Alignment::Alignment(TString files):e_list(0){
  // Setup the AMS chain
  chain=new AMSChain;
  chain->Add(files);

  // Setup the reconstruction parameters
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
  RichRing::ComputeNpExp=false;
  RichRing::SetWindow(5);

  // Create a pointer to the current alignment
  THIS=this;
}

// The selection function
int Alignment::Select(AMSEventR *event){
  // Cuts of general quality
  if(
     event->nAntiCluster()!=0                             // No Antis
     ) return 0;

  // Cuts of particle
  if(
     event->nParticle()!=1 ||                             // A single particle
     event->pParticle(0)->pTrTrack()==0  
     ) return 0;

  if(
     event->pParticle(0)->pTrTrack()->Chi2WithoutMS>0.1  // Beta=1 (mostly)
     ) return 0;   

  if(
     event->pParticle(0)->RichParticles>1                // Not orphan rich hot spots
     ) return 0;

  
  // Rich ring quality should go to the

  return 1;
}

int Alignment::SelectRing(RichRing *ring){
  // Select the ring properties
  if(!ring) return 0;  // Sometimes it fails
  
  if((ring->_status&2)) return 0;
  //  if(ring->_used!=5) return 0;  // For 5 hits
  if(ring->_used<3) return 0;  // For 3 hits
  return 1;
}



// The loop function
void Alignment::Loop(){
  static int first_call=1;
  AMSEventR *event;

  int event_number=-1;
  int run_number=-1;
  int counter=0;

  // Fill all the histograms
#ifndef _DEBUG_
  const int bins=100;
#else
  const int bins=5;
#endif
  // This scans and returns the best
  const double dx=0.01,dy=0.01,dz=0.1,
    dalpha=0.001,dbeta=0.0001,dgamma=0.0001;

  double Delta[6]={dx,dy,dz,dalpha,dbeta,dgamma};
  double x0[6];
  RichAlignment::_a2rShift.getp(x0[0],x0[1],x0[2]);
  RichAlignment::_a2rRot.GetRotAngles(x0[3],x0[4],x0[5]);

  int monster_number=bins*6;
  TH1F betas[monster_number];
  for(int i=0;i<monster_number;i++)
    betas[i].SetBins(100,0.995,1.005);

  for(int i=0;i<6;i++){
    double minx=x0[i]-(Delta[i]*bins)/2;
    double maxx=x0[i]+(Delta[i]*bins)/2;
    mean_h[i].SetBins(bins,minx,maxx);
    sigma_h[i].SetBins(bins,minx,maxx);
    norma_h[i].SetBins(bins,minx,maxx);
  }

  chain->Rewind();
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();
    counter++;
    if(!Select(event)) continue;

    // Reconstruct
    for(int variable=0;variable<6;variable++){
      for(int shift=1;shift<=bins;shift++){
	double x[6];
	for(int j=0;j<6;j++) x[j]=x0[j];
	x[variable]=mean_h[variable].GetXaxis()->GetBinCenter(shift);
	int beta_histogram_number=(shift-1)*6+variable;
	RichAlignment::Set(x[0],x[1],x[2],x[3],x[4],x[5]);
	RichRing *ring=RichRing::build(event,
				       event->pParticle(0)->pTrTrack());
	
	if(!ring) continue;

	if(!SelectRing(ring)){
	  delete ring;
	  continue;
	}
	
	// Fill the histogram
	betas[beta_histogram_number].Fill(ring->_beta);
	delete ring;
	
      }
    }

  } // END OF LOOP

  // Now it is time to fit everything
  for(int variable=0;variable<6;variable++){
    for(int shift=1;shift<=bins;shift++){
      int beta_histogram_number=(shift-1)*6+variable;
      Fit(betas[beta_histogram_number]);
      TF1 *func=betas[beta_histogram_number].GetFunction("gaus");
      norma_h[variable].SetBinContent(shift,func->GetParameter(0));
      mean_h[variable].SetBinContent(shift,func->GetParameter(1));
      sigma_h[variable].SetBinContent(shift,func->GetParameter(2));

      norma_h[variable].SetBinError(shift,func->GetParError(0));
      mean_h[variable].SetBinError(shift,func->GetParError(1));
      sigma_h[variable].SetBinError(shift,func->GetParError(2));
    }
  }      
}


double Alignment::Fit(TH1F &beta){
  double xmin=beta.GetXaxis()->GetXmin();
  double xmax=beta.GetXaxis()->GetXmax();

  double prev_norma=-1;
  double norma,mean,sigma;


  for(int i=0;i<10;i++){
    if(xmin>beta.GetXaxis()->GetXmax() || xmax<beta.GetXaxis()->GetXmin() || xmin>=xmax) return 0;  // Fail

    //    beta.Fit("gaus","0q","",xmin,xmax);
    beta.Fit("gaus","0q","",xmin,xmax);
    TF1 *func=beta.GetFunction("gaus");
    norma=func->GetParameter(0);
    mean=func->GetParameter(1);
    sigma=fabs(func->GetParameter(2));

    xmin=mean-1.0*sigma;
    xmax=mean+2.0*sigma;
    if(fabs(norma-prev_norma)<1e-3) break;

    prev_norma=norma;

  }

  return -norma;  // This have to be minimized
}


int main(int argc,char **argv){
  if(argc<3){
    cout<<"The first argument should be the set of files"<<endl;
    cout<<"The second should be the initial alignment file "<<endl;
    return 1;
  }


  cout<<"FILES to analyse"<<argv[1]<<endl;
  cout<<"INITIAL ALIGNAMENT "<<argv[2]<<endl;
  // Load the initial alignment
  RichAlignment::LoadFile(argv[2]);


  Alignment analysis(argv[1]);
  analysis.Loop();


  char *vars[6]={"SX","SY","SZ","ALPHA","BETA","GAMMA"};
  char *h_name[3]={"NORMA","MEAN","SIGMA"};



  TFile current(output,"RECREATE");
  
  for(int i=0;i<6;i++){
    analysis.mean_h[i].Write(Form("MEAN %s",vars[i]));
    analysis.norma_h[i].Write(Form("NORMA %s",vars[i]));
    analysis.sigma_h[i].Write(Form("SIGMA %s",vars[i]));
  }


  current.Close();

}
