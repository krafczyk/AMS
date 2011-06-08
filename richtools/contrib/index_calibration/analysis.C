#include "analysis.h"
#include <iostream>
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "time.h"
#include<fstream>

#define REALDATA

using namespace std;
const float window_size=3;
const int analysis_bins=11;

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
  RichRing::DeltaHeight=0;
  RichRing::DeltaHeight=-0.655608413971428550;  // THIS IS FOR REALDATA
  RichAlignment::Set(0,0,0,0,0,0);
  RichAlignment::SetMirrorShift(0,0,0);

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

  gRandom->SetSeed(time(0));

  for(int i=0;i<max_tiles;i++) entries[i]=0;
  total_entries=0;


  for(int i=0;i<trials;i++){
    cout<<"INIT sample "<<i<<endl; 
    for(int j=0;j<max_tiles;j++) betaHitHistograms[j][i].SetBins(1000,0.95,1.05);
  }

  summary=TH1F("summ","summ",1000,0.95,1.05);

  // Initialize the refractive indexes table
  for(int i=0;i<RichRadiatorTileManager::get_number_of_tiles();i++){
    double current=RichRadiatorTileManager::get_refractive_index(i);
    indexes[i][0]=1+(current-1)*0.8;
    indexes[i][1]=current;
    indexes[i][2]=1+(current-1)*1.2;
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

  SELECT("Tracker charge available",event->pCharge(0) && event->pCharge(0)->getSubD("AMSChargeTrackerInner") && event->pCharge(0)->getSubD("AMSChargeTrackerInner")->Q<1.5);
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>60);


  return true;
}

void Analysis::Loop(){
  Init();                       // Initialize everything
  total_entries=0;

  // Counters to artificially look for EOF
  unsigned int event_number=-1;
  unsigned int run_number=-1;

  AMSEventR *event;
  long int prev=-1;

  RichAlignment::Set(			 
		     alignmentParameters[ 0 ],
		     alignmentParameters[ 1 ],
		     alignmentParameters[ 2 ],
		     alignmentParameters[ 3 ],
		     alignmentParameters[ 4 ],
		     alignmentParameters[ 5 ]);

  RichAlignment::SetMirrorShift(0,0,0);  // Just in case
  
  while (event = chain->GetEvent()) {
    
    if((total_entries%10000)==0 && total_entries!=prev) {cout<<endl<<"Number of reconstructions "<<total_entries<<endl;prev=total_entries;}
    if(total_entries>maxReconstructions) break; 
    

    if(event->Event()==event_number && event->Run()==run_number) break;
    event_number=event->Event();
    run_number=event->Run();

    if(!Select(event)) continue;



    int first=1;
    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
    total_entries++;
    if(!ring) continue;
    int tile_index=RichRing::_tile_index;
    //    if(RichRing::_kind_of_tile==naf_kind) tile_index=max_tiles-1;
    entries[tile_index]++;
    delete ring;

    
    for(int i=0;i<trials;i++){
      // Set the refractive indexes
      //      RichRadiatorTileManager::recompute_tables(tile_index,indexes[i]);
      RichRadiatorTileManager::recompute_tables(tile_index,indexes[tile_index][i]);

      // Reconstruct the ring
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      total_entries++;
      if(!ring) continue;  // Should add this to my efficiency counters
      
      for(int j=0;j<ring->_beta_direct.size();j++){
	betaHitHistograms[tile_index][i].Fill(ring->_beta_direct[j]);
	if(i==1) summary.Fill(ring->_beta_direct[j]);
      }
      
      delete ring;
    }

  } // End of event loop
}


TF1 *Fita(TH1F *proyection,double &mean,double &mean_error,double &sigma,double &sigma_error){
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



int main(int argc,char **argv){
  if(argc<4){
    cout<<"First argument should be the filename for data"<<endl;
    cout<<"Second argument should be the alignment file"<<endl;
    cout<<"Third argument should be the output file"<<endl;
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
  analysis.alignmentParameters[0]=sx;
  analysis.alignmentParameters[1]=sy;
  analysis.alignmentParameters[2]=sz;
  analysis.alignmentParameters[3]=alpha;
  analysis.alignmentParameters[4]=beta;
  analysis.alignmentParameters[5]=gamma;

  analysis.Loop();

  // Fit them
  double best_tile_value[max_tiles];
  int best_tile_i[max_tiles];
  double winner[max_tiles];
  
  /*
  TFile suma("summary.root","RECREATE");
  for(int i=0;i<nSamples;i++){
    double mean,mean_error,sigma,sigma_error;
    Fita(&analysis.betaHitSummary[i],mean,mean_error,sigma,sigma_error);
    analysis.betaHitSummary[i].Write(Form("Summary %i",i));
  }
  suma.Close();
  */

  double sigmas[max_tiles][trials];
  double means[max_tiles][trials];

  // Fit all distributions
  TFile ffff("index_cal.root","RECREATE");
  for(int j=0;j<max_tiles;j++){    
    winner[j]=-1;
    if(analysis.entries[j]<100){
      cout<<"TILE INDEX "<<j<<" has not enough statistics"<<endl;
      continue;
    }
    int done=0;
    for(int i=0;i<trials;i++){
      sigmas[j][i]=-1;
      double mean,mean_error,sigma,sigma_error;
      
      if(Fita(&analysis.betaHitHistograms[j][i],mean,mean_error,sigma,sigma_error)){
	if(i==1) analysis.betaHitHistograms[j][i].Write();
	sigmas[j][i]=fabs(sigma);
	means[j][i]=mean;
	done++;
      }
    }
    if(done<3) {
      cout<<"TILE INDEX "<<j<<" have some fit problems"<<endl;
      continue;
    }
    // Compute the best index
    double b2=means[j][2];
    double b1=means[j][1];
    double b0=means[j][0];
    /*
    double n2=indexes[2];
    double n1=indexes[1];
    double n0=indexes[0];
    */    
    double n2=indexes[j][2];
    double n1=indexes[j][1];
    double n0=indexes[j][0];

    double n=(1-b1)/(b2-b0)*(n2-n0)+n1;
    //    winner[j]=n;
    //    winner[j]=(n+indexes[j][1])/2; //approac slowly to aim for convergence
    winner[j]=(0.75*n+0.25*indexes[j][1]); //approac slowly to aim for convergence
  }
  

  FILE *fp=fopen(argv[3],"w");
  for(int i=0;i<RichRadiatorTileManager::get_number_of_tiles();i++){
    int tile=i;
    double x=RichRadiatorTileManager::get_tile_x(tile);
    double y=RichRadiatorTileManager::get_tile_y(tile);
    double clarity=RichRadiatorTileManager::get_clarity(tile);
    double height=RichRadiatorTileManager::get_height(tile);
    if(winner[tile]>0){
      cout<<"TILE "<<i<<" "<<x<<" "<<y<<" "
	//	  <<indexes[0]<<" "<<means[i][0]<<" "
	//	  <<indexes[1]<<" "<<means[i][1]<<" "
	//	  <<indexes[2]<<" "<<means[i][2]<<" "
	  <<indexes[i][0]<<" "<<means[i][0]<<" "
	  <<indexes[i][1]<<" "<<means[i][1]<<" "
	  <<indexes[i][2]<<" "<<means[i][2]<<" "
	  <<" -- "<<winner[i]<<endl;
      fprintf(fp,
	      "%g %g %g %g %g\n",
	      x,y,winner[tile],height,clarity);
    }else
      fprintf(fp,
	      "%g %g %g %g %g\n",
	      x,y,RichRadiatorTileManager::get_refractive_index(tile),height,clarity);
    //      fprintf(fp,
    //	      "---- NO FIT ---- %g %g %g %g %g -- tabulated as %g \n",
    //	      x,y,winner[tile],height,clarity,RichRadiatorTileManager::get_refractive_index(tile));
    
  }
  fclose(fp);
  analysis.summary.Write();
  ffff.Close();

  // Output the selection efficiencies
  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }


  return 0;
}
