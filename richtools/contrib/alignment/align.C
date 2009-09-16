#include "align.h"
#include "DEMinimizer.h"
#include "TMinuit.h"

char *output="betas.root";

//#define _SIMPLE_FUNC_
//#define _USE_MINUIT_

Alignment* Alignment::THIS=0;

Alignment::~Alignment(){
  delete chain;
  delete disk_chain;
  if(e_list) delete e_list;
}

Alignment::Alignment(TString files):e_list(0){
  // Setup the AMS chain
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

  // Setup the reconstruction parameters
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();
  RichRing::ComputeNpExp=false;
  RichRing::SetWindow(5);

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
     event->pParticle(0)->pRichRing()==0 ||               // A ring...
     event->pParticle(0)->pRichRing()->pTrTrack()==0      // ..associated to a tracker track    
     ) return 0;

  if(
     (event->pParticle(0)->pRichRing()->Status&2)!=0     // Crossing the arogel
     ) return 0;

  if(
     event->pParticle(0)->pRichRing()->pTrTrack()->Chi2WithoutMS>0.1  // Beta=1 (mostly)
     ) return 0;   

  if(
     event->pParticle(0)->RichParticles>1                // Not orphan rich hot spots
     ) return 0;

  
  if(
     event->pParticle(0)->pRichRing()->Prob<0.1 ||       // Rich ring quality (in terms of expected and hot spots)
     event->pParticle(0)->pRichRing()->NpExp<3
     ) return 0;

  // Rich ring quality should go to the

  return 1;
}


// The loop function
void Alignment::Loop(bool fill){
  AMSEventR *event;

  if(fill && !e_list)
    e_list=new AMSEventList;

  int event_number=-1;
  int counter=0;

  chain->Rewind();
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number) break;
    event_number=event->Event();
    counter++;
    if(!Select(event)) continue;
    if(fill) e_list->Add(event);
    // Reconstruct
    RichRing::UseReflected=false;   // Use only direct hits
    RichRing *ring=RichRing::build(event,
				   event->pParticle(0)->pRichRing()->pTrTrack());


    if(!ring) continue;  // Sometimes it fails
    if(ring->_used!=5) continue;  // For 5 hits

    // Fill the histogram
    h_beta.Fill(ring->getbeta());

    // Cleanup used memory
    delete ring;
  }
}


double Alignment::Fit(){
  double xmin=h_beta.GetXaxis()->GetXmin();
  double xmax=h_beta.GetXaxis()->GetXmax();

  double prev_norma=-1;
  double norma,mean,sigma;

#ifdef _SIMPLE_FUNC_
  return h_beta.GetEntries()?h_beta.GetRMS()/h_beta.GetEntries():HUGE_VAL;
#endif


  for(int i=0;i<10;i++){
    if(xmin>h_beta.GetXaxis()->GetXmax() || xmax<h_beta.GetXaxis()->GetXmin() || xmin>=xmax) return 0;  // Fail

    h_beta.Fit("gaus","0q","",xmin,xmax);
    TF1 *func=h_beta.GetFunction("gaus");
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

// This is the function generating a sample
inline double get_random(double min,double max){
  return (max-min)*drand48()+min;
}

double *sample(){
  static int current=0;
  double *a=new double[6];

  /*
  if(current==0){
    // No aligned
    current++;
    for(int i=0;i<6;i++) a[i]=0;
    return a;
  }

  if(current==1){
    // Current alignment
    current++;
    int i=0;
    a[i++]=-0.1591858;
    a[i++]=0.0013612;
    a[i++]=-0.2084552;
    a[i++]=0.0;
    a[i++]=0.0;
    a[i++]=-0.0009641;
    return a;
  }


  // Shifts (in cm)
  int i=0;
  a[i++]=get_random(-0.2,0.2);
  a[i++]=get_random(-0.2,0.2);
  a[i++]=get_random(-0.2,0.2);

  // Rotations (in rads)
  a[i++]=get_random(-1e-3,1e-3);
  a[i++]=get_random(-1e-3,1e-3);
  a[i++]=get_random(-1e-3,1e-3);
  */

  // Shifts (in cm)
  int i=0;
  a[i++]=get_random(-0.5,0.5);
  a[i++]=get_random(-0.5,0.5);
  a[i++]=get_random(-0.5,0.5);

  // Rotations (in rads)
  a[i++]=get_random(-0.5e-2,0.5e-2);
  a[i++]=get_random(-0.5e-2,0.5e-2);
  a[i++]=get_random(-0.5e-2,0.5e-2);

  return a;
}


// This is the function to normalize
double func2minimize(double *pars){
  assert(Alignment::THIS);
  Alignment &analysis=*Alignment::THIS;

  // Set current alignment
  RichAlignment::Set(pars[0],pars[1],pars[2],   
		     pars[3],pars[4],pars[5]);

  analysis.h_beta.Reset(); // Reset histogram
  analysis.Loop();         // Fill it  

  cout<<"EVALUTING ";
  for(int i=0;i<6;i++) cout<<pars[i]<<" ";
  cout<<endl;
  double value=analysis.Fit();  // Get the resolution

  cout<<"     = "<<value;
  return value;
  //  return analysis.Fit();  // Get the resolution
}


// Wrapper for minuit
void func4minuit(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
  f=func2minimize(par);
}





int main(int argc,char **argv){
  if(argc<2){
    cout<<"The first argument should be the set of files"<<endl;
    return 1;
  }


  cout<<"FILES to analyse"<<argv[1]<<endl;
  
  Alignment analysis(argv[1]);
  analysis.Loop();
  TFile current(output,"RECREATE");
  analysis.h_beta.Write("Beta");
  current.Close();

#ifdef _USE_MINUIT_
  TMinuit *gMinuit = new TMinuit(6);
  gMinuit->SetFCN(func4minuit);

  // Initial parameters
  Double_t arglist[10];
  Int_t ierflg = 0;
  arglist[0] = 1;

  //static Double_t vstart[6] = {0,0,0,0,0,0};
  static Double_t vstart[6] = {
    -0.1591858,
    0.0013612,
    -0.2084552,
    0.0,
    0.0,
    -0.0009641};

  static Double_t step[6] = {1e-1 , 1e-1 , 1e-1 ,1e-3,1e-3,1e-3};
  for(int i=0;i<6;i++)
    gMinuit->mnparm(i, Form("a%i",i), vstart[i], step[i], 0,0,ierflg);

  // Minimize
  arglist[0]=3;
  gMinuit->mnexcm("SET PRI",arglist,1,ierflg);

  arglist[0] = 500;
  arglist[1] = 1.;
  

  gMinuit->mnexcm("SIMPLEX", arglist ,2,ierflg);
  gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);


  // Print results
  Double_t amin,edm,errdef;
  Int_t nvpar,nparx,icstat;
  gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  gMinuit->mnprin(3,amin);

#else
  // Start the minimization
  DEMinimizer minimizer(6,                 // 6 parameters
			6*3,                // Population
			&func2minimize,
			&sample
			);


  // Start minimization
  do{
    minimizer.Step();
    double *p=minimizer.fBest;
    cout<<"Current minimum "<<endl
	<<"    Shift parameters: "<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl
	<<"    Rot. parameters:  "<<p[3]<<" "<<p[4]<<" "<<p[5]<<endl;
    cout<<"Function value "<<minimizer.fBestValue<<endl;
    cout<<endl<<endl<<endl;

    // Save current best
    RichAlignment::Set(p[0],p[1],p[2],
		       p[3],p[4],p[5]);
    analysis.h_beta.Reset();
    analysis.Loop();
    TFile log(output,"UPDATE");
    analysis.h_beta.Write("Beta");
    log.Close();

  }while(minimizer.fAge<30);
#endif

  // Finish
  double *p=minimizer.fBest;
  cout<<"Current minimum "<<endl
      <<"    Shift parameters: "<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl
      <<"    Rot. parameters:  "<<p[3]<<" "<<p[4]<<" "<<p[5]<<endl;
  cout<<"Function value "<<minimizer.fBestValue<<endl;
  cout<<endl<<endl<<endl;




  TFile output("alignment_histograms.root","UPDATE");
  analysis.h_beta.Write("Beta");
  output.Close();

}
