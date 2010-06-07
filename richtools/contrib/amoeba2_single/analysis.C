#include "analysis.h"
#include <iostream>
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "time.h"
#include<fstream>

using namespace std;
const float window_size=2.75;
const int analysis_bins=11;

// DEFINE FOR DEBUGGING
#undef _FIX_ANGLES

#undef _CREATE_EVENTLIST
#ifdef _CREATE_EVENTLIST
AMSEventList amsevents;
#endif

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
  RichRing::UseReflected=true;

  RichRing::DeltaHeight=-0.5972871;

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

  gRandom->SetSeed(time(0));

  for(int i=0;i<max_tiles;i++) entries[i]=0;
  total_entries=0;

  // Prepare the samples and histograms
  betaHitSummary.reserve(nSamples);
  for(int j=0;j<max_tiles;j++) betaHitHistograms[j]=new TH1F[nSamples];

  for(int i=0;i<6;i++) alignmentParameters[0].reserve(nSamples);

  TH1F prototype("","",1000,0.95,1.05);
  testigo.SetBins(1000,0.95,1.05);

  for(int i=0;i<nSamples;i++) betaHitSummary.push_back(prototype);
  for(int i=0;i<nSamples;i++){
    cout<<"INIT sample "<<i<<endl; 
    for(int j=0;j<max_tiles;j++) betaHitHistograms[j][i].SetBins(1000,0.95,1.05);

    if(i==0) continue;

    // Alignment parameters
    // Spatial ones
    alignmentParameters[0].push_back(alignmentParameters[ 0 ][0]+gRandom->Gaus()/3/2/3);
    alignmentParameters[1].push_back(alignmentParameters[ 1 ][0]+gRandom->Gaus()/3/2/3);
    alignmentParameters[2].push_back(alignmentParameters[ 2 ][0]+gRandom->Gaus()/3/2);

    // Angular ones
    alignmentParameters[3].push_back(alignmentParameters[ 3 ][0]+gRandom->Gaus()/1000);
    alignmentParameters[4].push_back(alignmentParameters[ 4 ][0]+gRandom->Gaus()/1000/3);
    alignmentParameters[5].push_back(alignmentParameters[ 5 ][0]+gRandom->Gaus()/1000/3);
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
  //  SELECT("With 1 TrTrack",event->nTrTrack()==1);
  SELECT("At most 1 TrdTrack",event->nTrdTrack()<=1);
  SELECT("At most 4 TofClusters",event->nTofCluster()<=4);

  SELECT("With rich particle",event->pParticle(0)->pRichRing());
  SELECT("With track particle",event->pParticle(0)->pRichRing()->pTrTrack());
  SELECT("Clean ring",(event->pParticle(0)->pRichRing()->Status&1)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);
  //  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-1);  // This cut is not fully understood

  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>4);

  SELECT("NaF ring",(event->pParticle(0)->pRichRing()->Status&2)!=0);

#ifdef _CREATE_EVENTLIST
  amsevents.Add(event);
#endif 

  return true;
}

void Analysis::Loop(){
  Init();                       // Initialize everything

  // Counters to artificially look for EOF
  unsigned int event_number=-1;
  unsigned int run_number=-1;

  AMSEventR *event;
  long int prev=-1;
  while (event = chain->GetEvent()) {
    if(gRandom->Uniform()<0.5) continue;
    if((total_entries%10000)==0 && total_entries!=prev) {cout<<endl<<"Number of reconstructions "<<total_entries<<endl;prev=total_entries;}
    if(total_entries>maxReconstructions) break; 


    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();

    if(!Select(event)) continue;

    RichAlignment::Set(0,0,0,0,0,0);  // Just in case
    RichAlignment::SetMirrorShift(0,0,0);  // Just in case

    int first=1;
    for(int i=0;i<nSamples;i++){
      int k=0;
      RichAlignment::Set(
#ifndef _FIX_ANGLES
			 alignmentParameters[ 0 ][i],
			 alignmentParameters[ 1 ][i],
			 alignmentParameters[ 2 ][i],
			 alignmentParameters[ 3 ][i],
			 alignmentParameters[ 4 ][i],
			 alignmentParameters[ 5 ][i]
#else
			 0,0,0,
			 0,alignmentParameters[ 4 ][i],0
#endif
			 );

      RichAlignment::SetMirrorShift(0,0,0);
      
      // Reconstruct the ring
      RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      if(!ring) continue;  // Should add this to my efficiency counters
      if(i==0){
	for(int jj=0;jj<ring->_beta_direct.size();jj++)
	  testigo.Fill(ring->_beta_direct[jj]);
      }

      if(first){
	total_entries++;
	first=0;
      }

      // Get the tile
      int tile_number=0;
      entries[tile_number]++;
      for(int j=0;j<ring->_beta_direct.size();j++){
	betaHitHistograms[tile_number][i].Fill(ring->_beta_direct[j]);
	betaHitSummary[i].Fill(ring->_beta_direct[j]);
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

  /**************** WE DO NOT NEED THIS *********************
  // Write all the histograms
  TFile file2("amoeba_fits.root","RECREATE");
  for(int i=0;i<nSamples;i++) 
    for(int j=0;j<max_tiles;j++){
      if(analysis.entries[j]<100) continue;
      analysis.betaHitHistograms[j][i].Write( Form("BetaHit %i Tile %i",i,j) );
    }
  file2.Close();
  */

  // Fit them
  double best_tile_value[max_tiles];
  int best_tile_i[max_tiles];
  
  /*
  TFile suma("summary.root","RECREATE");
  for(int i=0;i<nSamples;i++){
    double mean,mean_error,sigma,sigma_error;
    Fita(&analysis.betaHitSummary[i],mean,mean_error,sigma,sigma_error);
    analysis.betaHitSummary[i].Write(Form("Summary %i",i));
  }
  suma.Close();
  */

  double sigmas[max_tiles][nSamples];
  double means[max_tiles][nSamples];

  // Fit all distributions
  for(int i=0;i<nSamples;i++){
    for(int j=0;j<max_tiles;j++){    
      sigmas[j][i]=-1;
      if(analysis.entries[j]<100){
	cout<<"TILE "<<j<<" ONLY HAS "<<analysis.entries[j]<<" entries"<<endl;
	continue;
      }
      double mean,mean_error,sigma,sigma_error;
      
      if(Fita(&analysis.betaHitHistograms[j][i],mean,mean_error,sigma,sigma_error)){
	sigmas[j][i]=fabs(sigma);
	means[j][i]=mean;
      }
    }
  }

  // THIS IS FOR DEBUGGING
  //  FILE *file_samples=fopen("Samples.txt","w");
  // Find the optimum
  double best=HUGE_VAL;
  int best_i=-1;
  for(int i=0;i<nSamples;i++){
    double sum2=0;
    double total=0;
    for(int j=0;j<max_tiles;j++){
      if(sigmas[j][i]<0) continue;
      double sigma=sigmas[j][i]/means[j][i];  // The mean value should be 1
      sum2+=sigma*sigma*analysis.entries[j];
      total+=analysis.entries[j];
    }
    if(total<0) continue;

    /*
    fprintf(file_samples,
	    "%g %g %g %g %g %g %g\n",
	    analysis.alignmentParameters[0][i],
	    analysis.alignmentParameters[1][i],
	    analysis.alignmentParameters[2][i],
	    analysis.alignmentParameters[3][i],
	    analysis.alignmentParameters[4][i],
	    analysis.alignmentParameters[5][i],
	    sum2<=0?0:sqrt(sum2/total));
    */
    if(sum2/total<best){
      best=sum2/total;
      best_i=i;
    }
  }
  //  fclose(file_samples);


  // WRITE THE RESULT TO THE OUTPUT FILE
  cout<<"BEST I IS "<<best_i<<endl
      <<analysis.alignmentParameters[ 0 ][best_i]<<" "
      <<analysis.alignmentParameters[ 1 ][best_i]<<" "
      <<analysis.alignmentParameters[ 2 ][best_i]<<" "<<best<<endl;
  if(best_i>-1){
    fp=fopen(argv[3],"w");
    fprintf(fp,
	    "%g %g %g\n "
	    "%g %g %g\n "
	    "0 0 0\n"
	    "%g\n",
	    analysis.alignmentParameters[ 0 ][best_i],
	    analysis.alignmentParameters[ 1 ][best_i],
	    analysis.alignmentParameters[ 2 ][best_i],
	    analysis.alignmentParameters[ 3 ][best_i],
	    analysis.alignmentParameters[ 4 ][best_i],
	    analysis.alignmentParameters[ 5 ][best_i],
	    best<=0?0:sqrt(best)
	    );
    fclose(fp);
  }

  /******************** THIS IS ONLY FOR DEBUGGING
  // Write all the histograms
  TFile file("amoeba_fits.root","RECREATE");
  for(int i=0;i<nSamples;i++) for(int j=0;j<max_tiles;j++) {if(analysis.entries[j]<100) continue;if(analysis.betaHitHistograms[j][i].GetFunction("gaus")) analysis.betaHitHistograms[j][i].Write( Form("BetaHit %i Tile %i",i,j) );}
  file.Close();

  TFile fileW("amoeba_winners.root","RECREATE");
  for(int j=0;j<max_tiles;j++) {if(analysis.entries[j]<100) continue;//if(analysis.betaHitHistograms[j][best_tile_i[j]].GetFunction("gaus")) analysis.betaHitHistograms[j][best_tile_i[j]].Write( Form("BetaHit Tile %i",j) );}
    if(best_i>-1 && analysis.betaHitHistograms[j][best_i].GetFunction("gaus")) analysis.betaHitHistograms[j][best_i].Write( Form("BetaHit Tile %i",j) );}
  fileW.Close();
  **********************************************************/


  TFile ffff("testigo.root","RECREATE");
  analysis.testigo.Write();
  ffff.Close();

  // Output the selection efficiencies
  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }


#ifdef _CREATE_EVENTLIST
  TFile evtList("/tmp/mdelgado/evtList.root","RECREATE");
  amsevents.Write(analysis.chain,&evtList);
  evtList.Close();
#endif

  return 0;
}
