#include "align.h"
#include "DEMinimizer.h"
#include "TMinuit.h"

char *output="betas.root";

//#define _CACHE_EVENTS_
//#define _USE_PROBKL_

Alignment* Alignment::THIS=0;

Alignment::~Alignment(){
  delete chain;
#ifdef _CACHE_EVENTS_
  delete disk_chain;
#endif
  if(e_list) delete e_list;
}

Alignment::Alignment(TString files):e_list(0){
  // Setup the AMS chain
#ifdef _CACHE_EVENTS_
  disk_chain=new AMSChain;
  disk_chain->Add(files);

  chain=(AMSChain*)disk_chain->CloneTree(0);
  chain->SetDirectory(0);

  int event_number=-1;
  AMSEventR *event;
  while(event=disk_chain->GetEvent()) {
    if(event->Event()==event_number) break;
    event_number=event->Event();
    if(!Select(event)) continue;

    event->GetAllContents();
    chain->Fill();
  }
  chain->Rewind();

#else
  chain=new AMSChain;
  chain->Add(files);
#endif

  // Setup the reconstruction parameters
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
#ifdef _USE_PROBKL
  RichRing::ComputeNpExp=true;
#else
  RichRing::ComputeNpExp=false;
#endif
  RichRing::SetWindow(5);
  RichRing::UseReflected=false;   // Use only direct hits

  // Create a pointer to the current alignment
  THIS=this;

  // Prepare the histograms
  h_beta.SetBins(100,0.995,1.005);
  
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
#ifdef _USE_PROBKL_
  if(ring->_probkl<1e-1) return 0; // This is not working
  if(ring->_npexp<3) return 0;
#endif
  if(ring->_used!=5) return 0;  // For 5 hits
  return 1;
}


double rnd(double mean){
  return (2*drand48()-1)*mean;
}


double gaus(double mean,double width){
  static int status=0;
  static double y1,y2;

  if(status==0){
    double x1, x2, w;
 
    do {
      x1 = 2.0 * drand48() - 1.0;
      x2 = 2.0 * drand48() - 1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    status=1-status;
    return y1*width+mean;
  }else{
    status=1-status;
    return y2*width+mean;
  }
}



// The loop function
void Alignment::Loop(double &_x,double &_y,double &_z,
		     double &_alpha,double &_beta,double &_gamma){
  static int first_call=1;
  AMSEventR *event;

  int event_number=-1;
  int counter=0;


  // Create a vector with all the parameters
  //  const int max_samples=50;
  const int max_samples=10;
  double parameters[max_samples][6];
  // This scans and returns the best
  const double dx=0.01,dy=0.01,dz=0.01,
    dalpha=0.0001,dbeta=0.0001,dgamma=0.0001;
  const double lx=0.2,ly=0.2,lz=0.2,
    lalpha=0.002,lbeta=0.002,lgamma=0.002;


  // Set the best
  parameters[0][0]=_x;
  parameters[0][1]=_y;
  parameters[0][2]=_z;
  parameters[0][3]=_alpha;
  parameters[0][4]=_beta;
  parameters[0][5]=_gamma;

  if(first_call){
    parameters[1][0]=-0.1591858;
    parameters[1][1]=0.0013612;
    parameters[1][2]=-0.208455;
    parameters[1][3]=0;
    parameters[1][4]=0;
    parameters[1][5]=-0.0009641;
  }

  for(int i=first_call?2:1;i<max_samples;i++){
    parameters[i][0]=_x+gaus(0,lx/3);
    parameters[i][1]=_y+gaus(0,ly/3);
    parameters[i][2]=_z+gaus(0,lz/3);
    parameters[i][3]=_alpha+gaus(0,lalpha/3);
    parameters[i][4]=_beta+gaus(0,lbeta/3);
    parameters[i][5]=_gamma+gaus(0,lgamma/3);
  }

  first_call=0;

  TH1F betas[max_samples];
  for(int i=0;i<max_samples;i++){
    betas[i].SetBins(100,0.995,1.005);
  }


  chain->Rewind();
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number) break;
    event_number=event->Event();
    counter++;
    if(!Select(event)) continue;
    // Reconstruct

    for(int i=0;i<max_samples;i++){
      /*
      cout<<"SETTING PARAMETERS "
	<<parameters[i][0]
	<<parameters[i][1]
	<<parameters[i][2]
	<<parameters[i][3]
	<<parameters[i][4]
	  <<parameters[i][5]<<endl;
      */

      RichAlignment::Set(parameters[i][0],
			 parameters[i][1],
			 parameters[i][2],
			 parameters[i][3],
			 parameters[i][4],
			 parameters[i][5]);

      RichRing *ring=RichRing::build(event,
				     event->pParticle(0)->pTrTrack());

      if(!SelectRing(ring)){
	delete ring;
	continue;
      }

      // Fill the histogram
      betas[i].Fill(ring->_beta);
      delete ring;
    }
  }
  // Finished with all the events: fit all the histos
  double current_best=HUGE_VAL;
  int best_index=-1;
  for(int i=0;i<max_samples;i++){
    double value=Fit(betas[i]);

    cout<<"SETTING PARAMETERS "
	<<parameters[i][0]
	<<parameters[i][1]
	<<parameters[i][2]
	<<parameters[i][3]
	<<parameters[i][4]
	<<parameters[i][5]<<endl;

    cout<<"VALUE "<<value<<endl;
    cout<<"BEST VALUE "<<current_best<<endl<<endl<<endl;


    if(value<current_best){
      current_best=value;
      best_index=i;
    }
  }

  h_beta.Reset();
  if(best_index>=0){
    h_beta=betas[best_index];
    Fit(h_beta);
    _x=parameters[best_index][0];
    _y=parameters[best_index][1];
    _z=parameters[best_index][2];
    _alpha=parameters[best_index][3];
    _beta=parameters[best_index][4];
    _gamma=parameters[best_index][5];
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
  if(argc<2){
    cout<<"The first argument should be the set of files"<<endl;
    return 1;
  }


  cout<<"FILES to analyse"<<argv[1]<<endl;
  double pars[6]={0,0,0,0,0,0};

  Alignment analysis(argv[1]);
  for(int i=0;i<50;i++){
    analysis.Loop(pars[0],pars[1],pars[2],
		  pars[3],pars[4],pars[5]);
    TFile current(output,"UPDATE");
    analysis.h_beta.Write("Beta");
    current.Close();

    cout<<"Current minimum "<<endl
	<<"    Shift parameters: "<<pars[0]<<" "<<pars[1]<<" "<<pars[2]<<endl
	<<"    Rot. parameters:  "<<pars[3]<<" "<<pars[4]<<" "<<pars[5]<<endl;
    cout<<"Function value "<<analysis.Fit(analysis.h_beta)<<endl;
    cout<<endl<<endl<<endl;
  }

  cout<<"Final minimum "<<endl
      <<"    Shift parameters: "<<pars[0]<<" "<<pars[1]<<" "<<pars[2]<<endl
      <<"    Rot. parameters:  "<<pars[3]<<" "<<pars[4]<<" "<<pars[5]<<endl;
  cout<<"Function value "<<analysis.Fit(analysis.h_beta)<<endl;
  cout<<endl<<endl<<endl;

}
