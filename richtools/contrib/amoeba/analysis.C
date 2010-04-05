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


const int nSamples=1000;

void Analysis::Init(){
  // Init everything
  chain->Rewind();
  RichPMTsManager::Init();
  RichRadiatorTileManager::Init();   
  RichAlignment::Init(false);        // We use the MC alignment: no alignment at all  

  // Do not compute charge
  RichRing::ComputeNpExp=false;

  // Use the standard window
  RichRing::SetWindow(window_size);   // We really do not use this thing

  // Use only direct hits to simplify the analysis: this biases the reconstruction unless 
  // events with a enugh fraction of ring are detected, thus we use both direct and reflected
  // to avoid this
  RichRing::UseDirect=true;
  RichRing::UseReflected=false;

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

  gRandom->SetSeed(time(0));

  // Prepare the samples and histograms
  for(int i=0;i<nSamples;i++){
    betaHitHistograms.push_back(TH1F("","",1000,0.95,1.05));
    if(i==0) continue;

    // Alignment parameters
    // Spatial ones
    alignmentParameters[0].push_back(alignmentParameters[ 0 ][0]+gRandom->Gaus()/3);
    alignmentParameters[1].push_back(alignmentParameters[ 1 ][0]+gRandom->Gaus()/3);
    alignmentParameters[2].push_back(alignmentParameters[ 2 ][0]+gRandom->Gaus()/3);

    // Angular ones
    alignmentParameters[3].push_back(alignmentParameters[ 3 ][0]+gRandom->Gaus()/1000);
    alignmentParameters[4].push_back(alignmentParameters[ 4 ][0]+gRandom->Gaus()/1000);
    alignmentParameters[5].push_back(alignmentParameters[ 5 ][0]+gRandom->Gaus()/1000);
  }
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
  //  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-1);  // This cut is not fully understood

  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->pTrTrack()->Rigidity)>100);  // This cut is not fully understood


  return true;
}

void Analysis::Loop(){
  Init();                       // Initialize everything

  // Counter to artificially look for EOF
  unsigned int event_number=-1;
  unsigned int run_number=-1;

  AMSEventR *event;
  while (event = chain->GetEvent()) {
    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();

    if(!Select(event)) continue;

    RichAlignment::Set(0,0,0,0,0,0);  // Just in case
    RichAlignment::SetMirrorShift(0,0,0);  // Just in case


    for(int i=0;i<nSamples;i++){
      int k=0;
      RichAlignment::Set(
			 alignmentParameters[ 0 ][i],
			 alignmentParameters[ 1 ][i],
			 alignmentParameters[ 2 ][i],
			 alignmentParameters[ 3 ][i],
			 alignmentParameters[ 4 ][i],
			 alignmentParameters[ 5 ][i]
			 );

      RichAlignment::SetMirrorShift(0,0,0);
      
      // Reconstruct the ring
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      //      cout<<"RING IS "<<ring<<" event version "<<event->Version()<<" "<<event->pParticle(0)->pRichRing()->pTrTrack()<<endl//;<<"TRACK "<<ring->_ptrack->_r[0]<<" "<<ring->_ptrack->_r[1]<<" "<<ring->_ptrack->_r[2]<<endl
      //	  <<"PARAMETERS ";
      //      k=0;
      //      printf("%g %g %g %g %g %g\n",
      //			 alignmentParameters[k++][i],
      //			 alignmentParameters[k++][i],
      //			 alignmentParameters[k++][i],
      //			 alignmentParameters[k++][i],
      //			 alignmentParameters[k++][i],
      //			 alignmentParameters[k++][i]
      //			 );

      if(!ring) continue;  // Shoul add this to my efficiency counters
      
      for(int j=0;j<ring->_beta_direct.size();j++){
	betaHitHistograms[i].Fill(ring->_beta_direct[j]);
      }
      
      delete ring;
    }

  } // End of event loop
}


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

      proyection->Fit("gaus","","",mean-2.5*sigma,mean+2.5*sigma);
      f=proyection->GetFunction("gaus");
      if(!f) return 0;
      mean=f->GetParameter(1);
      sigma=fabs(f->GetParameter(2));
      mean_error=f->GetParError(1);
      sigma_error=f->GetParError(2);

      return f;
}



int main(int argc,char **argv){
  if(argc<4){
    cout<<"First argument should be the filename for data"<<endl;
    cout<<"Second argument should be the initial alignment"<<endl;
    cout<<"Third argument should be the output alignment"<<endl;
    return 0;
  }


  


  // Load the alignment
  double sx,sy,sz;
  double alpha,beta,gamma;
  double mx,my,mz;
  {
    fstream stream;
    const int ver=1;
    TString filename=argv[2];
    stream.open(filename,fstream::in);
    if(!stream.is_open() || stream.fail()){
      cout<<"Alignment::LoadFile -- problem opening file "<<filename<<" ... Exiting."<<endl;
      return 0;
    }
    
    cout<<"RichAlignment::LoadFile -- loading "<<filename<<endl;
    
    // Read the following components of the a2r matrixes

    
    stream>>sx>>sy>>sz;
    
    if(stream.eof()){
      cout<<"RichAlignment::LoadFile -- truncated... Ingnoring "<<filename<<endl;
    }
    
    stream>>alpha>>beta>>gamma;
    
    
    if(stream.eof()){
      cout<<"RichAlignment::LoadFile -- truncated... Ingnoring "<<filename<<endl;
    }
    
    if(ver==1){
      stream>>mx>>my>>mz;
      if(stream.eof()){
	cout<<"RichAlignment::LoadFile -- truncated... Ingnoring "<<filename<<endl;
      }
    }
    
    stream.close();
  }

    

  // Fill histograms
  Analysis analysis(argv[1]);  
  analysis.alignmentParameters[0].push_back(sx);
  analysis.alignmentParameters[1].push_back(sy);
  analysis.alignmentParameters[2].push_back(sz);
  analysis.alignmentParameters[3].push_back(alpha);
  analysis.alignmentParameters[4].push_back(beta);
  analysis.alignmentParameters[5].push_back(gamma);

  analysis.Loop();
  FILE *fp=fopen("amoeba_results.txt","w");

  // Write all the histograms
  TFile file2("amoeba_fits.root","RECREATE");
  for(int i=0;i<nSamples;i++) analysis.betaHitHistograms[i].Write( Form("BetaHit %i",i) );
  file2.Close();

  // Fit them
  int best_i=0;
  double best_value=HUGE_VAL;
  for(int i=0;i<nSamples;i++){
    double mean,mean_error,sigma,sigma_error;
    cout<<"FITTING "<<i<<endl;
    if(Fita(&analysis.betaHitHistograms[i],mean,mean_error,sigma,sigma_error)){
      int k=0;
      fprintf(fp,
	      "%g %g %g "
	      "%g %g %g "
	      "%g %g "
	      "%g %g\n",
	      analysis.alignmentParameters[ 0 ][i],
	      analysis.alignmentParameters[ 1 ][i],
	      analysis.alignmentParameters[ 2 ][i],
	      analysis.alignmentParameters[ 3 ][i],
	      analysis.alignmentParameters[ 4 ][i],
	      analysis.alignmentParameters[ 5 ][i],
	      mean,mean_error,sigma,sigma_error
	      );
      if(sigma<best_value){
	best_value=sigma;
	best_i=i;
      }
      cout<<"DONE"<<endl;
    }else{
      cout<<"FAILED"<<endl;
    }
  }
  fclose(fp);

  // Write the output with the best alignment
  fp=fopen(argv[3],"w");
  fprintf(fp,
	  "%g %g %g\n"
	  "%g %g %g\n"
	  "0 0 0\n\n\n"
	  "BEST VALUE: %g",
	  analysis.alignmentParameters[0][best_i],
	  analysis.alignmentParameters[1][best_i],
	  analysis.alignmentParameters[2][best_i],
	  analysis.alignmentParameters[3][best_i],
	  analysis.alignmentParameters[4][best_i],
	  analysis.alignmentParameters[5][best_i],
	  best_value);
  fclose(fp);

  
  // Write all the histograms
  TFile file("amoeba_fits.root","RECREATE");
  for(int i=0;i<nSamples;i++) if(analysis.betaHitHistograms[i].GetFunction("gaus")) analysis.betaHitHistograms[i].Write( Form("BetaHit %i",i) );
  file.Close();




  // Output the selection efficiencies

  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }


  return 0;
}
