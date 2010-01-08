#include "analysis.h"
#include <iostream>
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"

// Which part to compute
//#define _RICH_ALIGNMENT_
//#define _MIRROR_ALIGNMENT_
#define _TILES_INDEX_
//#define _CALIBRATE_MAGNET_


// Default RICH alignment
const double ox=2.06e-2-4.49e-2, oy=6.16e-2+5.06e-2, oz=9.05e-2-6.39e-2;

// Default Mirror alingment
const double omx=-0.074-0.0107+1.061e-1, omy=0.092+0.0104+5.15e-2, omz=0;

//ClassImp(Analysis);

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
  RichRing::SetWindow(window_size);   // We double it because we want to callibrate everything, thus we need clusters

  // Use only direct hits to simplify the analysis: this biases the reconstruction unless 
  // events with a enugh fraction of ring are detected, thus we use both direct and reflected
  // to avoid this
  RichRing::UseDirect=true;
  RichRing::UseReflected=true;

  // Initialize the cuts variables
  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

  // The goal are several: find the optimum position of RICH wrt to AMS using only the direct 
  // hits reconstruction
  //  betahit_direct_vs_dx.SetBins(analysis_bins,-1,1,1000,0.95,1.05);
  betahit_direct_vs_dx.SetBins(analysis_bins*6,-3,3,1000,0.95,1.05);
  //  betahit_direct_vs_dy.SetBins(analysis_bins,-1,1,1000,0.95,1.05);
  betahit_direct_vs_dy.SetBins(analysis_bins*6,-3,3,1000,0.95,1.05);
  betahit_direct_vs_dz.SetBins(analysis_bins,-1,1,1000,0.95,1.05);

  // Find the best refractive index for the tiles using only the direct hit reconstruction
  // Beta hit "free" spectrum
  for(int i=0;i<max_tiles;i++) for(int j=0;j<3;j++) betahit_direct_vs_tile[i][j].SetBins(1000,0.95,1.05);

  // Find the optimum mirror position
  betahit_reflected_vs_dx.SetBins(analysis_bins,-1,1,1000,0.95,1.05);
  betahit_reflected_vs_dy.SetBins(analysis_bins,-1,1,1000,0.95,1.05);
  betahit_reflected_vs_dz.SetBins(analysis_bins,-1,1,1000,0.95,1.05);

  /////////////////////////
  betahit_direct_vs_dx_vs_rigidity.SetBins(analysis_bins*6,-3,3,1000,0.95,1.05,50,-10,10);
  betahit_direct_vs_dy_vs_rigidity.SetBins(analysis_bins*6,-3,3,1000,0.95,1.05,50,-10,10);
  betahit_direct_vs_dz_vs_rigidity.SetBins(analysis_bins*6,-3,3,1000,0.95,1.05,50,-10,10);

  betahit_direct.SetBins(1000,0.95,1.05);
  betahit_reflected.SetBins(1000,0.95,1.05);

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
  SELECT("Aerogel track and clean ring",(event->pParticle(0)->pRichRing()->Status&3)==0);

  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);

  RichRingR *ring=event->pParticle(0)->pRichRing();

  /************** These cuts cannot be used for alignment 

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
  *****************************/



  // Select good track and beta=1 (for muons data)
#ifndef _CALIBRATE_MAGNET_  
  SELECT("Beta==1 (trtrack chi2 selection)",log10(event->pParticle(0)->pTrTrack()->Chi2WithoutMS)<-1);  // This cut is not fully understood
#else
  SELECT("Beta==1 (momentum selection)",abs(event->pParticle(0)->pTrTrack()->Rigidity)>100);  // This cut is not fully understood
#endif

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

    RichAlignment::Set(ox,oy,oz,0,0,0);  // Just in case
    RichAlignment::SetMirrorShift(omx,omy,omz);  // Just in case

    // Reconstruct the ring
    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());

    if(!ring) continue;  // Shoul add this to my efficiency counters

    // Select beta=1 events from reconstruction
#ifndef _CALIBRATE_MAGNET_
    if(!(event->pParticle(0)->pRichRing()->Beta<1.003 && event->pParticle(0)->pRichRing()->Beta>0.998 && event->pParticle(0)->pRichRing()->Used>5)){
      delete ring;
      continue;
    }
#endif

    for(int i=0;i<ring->_beta_reflected.size();i++){
      betahit_reflected.Fill(ring->_beta_reflected[i]);
    }
    for(int i=0;i<ring->_beta_direct.size();i++){
      betahit_direct.Fill(ring->_beta_direct[i]);
    }

    int &tile_index=RichRing::_tile_index;
    float tile_radiator_index=RichRadiatorTileManager::get_refractive_index(tile_index);
    

    //////////////////// TILE CALIBRATION ///////////////////////
    double indexes[3];
    indexes[1]=tile_radiator_index;
    indexes[0]=(tile_radiator_index-1)*0.9+1;
    indexes[2]=(tile_radiator_index-1)*1.1+1;


    for(int loop=0;loop<3;loop++){
      // DO NOT CHANGE THE INDEX
      RichRadiatorTileManager::recompute_tables(tile_index,indexes[loop]);
      RichRing *local_ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      if(!local_ring) continue;
      for(int i=0;i<local_ring->_beta_direct.size();i++){
	betahit_direct_vs_tile[tile_index][loop].Fill(local_ring->_beta_direct[i]);
      }
      delete local_ring;
    }
    /*
    for(int i=0;i<ring->_beta_direct.size();i++)
    for(int loop=0;loop<3;loop++)
      betahit_direct_vs_tile[tile_index][loop].Fill(ring->_beta_direct[i]);
    */

    RichRadiatorTileManager::recompute_tables(tile_index,indexes[1]);  // Return to the default index

    delete ring;
#ifdef _MIRROR_ALIGNMENT_
    //////////////////////// MIRROR ALIGNMENT ////////////////////////
    RichAlignment::Set(ox,oy,oz,0,0,0);  // Just in case
    RichAlignment::SetMirrorShift(omx,omy,omz);  // Just in case

      // Now do the same with the beta reflected as a function of the shift
      // Shift in X
    for(int i=1;i<=betahit_reflected_vs_dx.GetNbinsX();i++){
      double dx=betahit_reflected_vs_dx.GetXaxis()->GetBinCenter(i);
      RichAlignment::SetMirrorShift(omx+dx,omy,omz);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_reflected.size();i++){
	betahit_reflected_vs_dx.Fill(dx,ring->_beta_reflected[i]);

      }
      delete ring;
    }
    
    // Shift in Y
    for(int i=1;i<=betahit_reflected_vs_dy.GetNbinsX();i++){
      double dy=betahit_reflected_vs_dy.GetXaxis()->GetBinCenter(i);
      RichAlignment::SetMirrorShift(omx,omy+dy,omz);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_reflected.size();i++){
	betahit_reflected_vs_dy.Fill(dy,ring->_beta_reflected[i]);
      }
      delete ring;
    }
    
    // Shift in Z
    for(int i=1;i<=betahit_reflected_vs_dz.GetNbinsX();i++){
      double dz=betahit_reflected_vs_dz.GetXaxis()->GetBinCenter(i);
      RichAlignment::SetMirrorShift(omx,omy,omz+dz);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_reflected.size();i++){
	betahit_reflected_vs_dz.Fill(dz,ring->_beta_reflected[i]);
      }
      delete ring;
    }
#endif

#ifdef _RICH_ALIGNMENT_
    //////////////////// RICH ALIGNMENT ////////////////////
    RichAlignment::Set(ox,oy,oz,0,0,0);  // Just in case
    RichAlignment::SetMirrorShift(omx,omy,omz);  // Just in case

    for(int i=1;i<=betahit_direct_vs_dx.GetNbinsX();i++){
      double dx=betahit_direct_vs_dx.GetXaxis()->GetBinCenter(i);
      RichAlignment::Set(ox+dx,oy,oz,0,0,0);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_direct.size();i++){
	betahit_direct_vs_dx.Fill(dx,ring->_beta_direct[i]);
	betahit_direct_vs_dx_vs_rigidity.Fill(dx,ring->_beta_reflected[i],1.0/event->pParticle(0)->pRichRing()->pTrTrack()->Rigidity);
      }
      delete ring;
    }
    
    // Shift in Y
    for(int i=1;i<=betahit_direct_vs_dy.GetNbinsX();i++){
      double dy=betahit_direct_vs_dy.GetXaxis()->GetBinCenter(i);
      RichAlignment::Set(ox,oy+dy,oz,0,0,0);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_direct.size();i++){
	betahit_direct_vs_dy.Fill(dy,ring->_beta_direct[i]);
	betahit_direct_vs_dy_vs_rigidity.Fill(dy,ring->_beta_reflected[i],1.0/event->pParticle(0)->pRichRing()->pTrTrack()->Rigidity);
      }
      delete ring;
    }
    
    // Shift in Z
    for(int i=1;i<=betahit_direct_vs_dz.GetNbinsX();i++){
      double dz=betahit_direct_vs_dz.GetXaxis()->GetBinCenter(i);
      RichAlignment::Set(ox,oy,oz+dz,0,0,0);
      ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
      
      if(!ring) continue;
      for(int i=0;i<ring->_beta_direct.size();i++){
	betahit_direct_vs_dz.Fill(dz,ring->_beta_direct[i]);
	betahit_direct_vs_dz_vs_rigidity.Fill(dz,ring->_beta_reflected[i],1.0/event->pParticle(0)->pRichRing()->pTrTrack()->Rigidity);
      }
      delete ring;
    }
#endif    
  } // End of event loop
}


TF1 *Fita(TH1D *proyection,double &mean,double &mean_error,double &sigma,double &sigma_error){
  if(proyection->Integral()<1000) return 0;
      proyection->Fit("pol1","","",0.962,0.99);
      TF1 *f=proyection->GetFunction("pol1");
      if(!f) return 0;
      Double_t a=f->GetParameter(0);
      Double_t b=f->GetParameter(1);

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
      sigma=f->GetParameter(2);
      mean_error=f->GetParError(1);
      sigma_error=f->GetParError(2);
      return f;
}



int main(int argc,char **argv){
  if(argc<3){
    cout<<"First argument should be the filename for data"<<endl;
    cout<<"Second argument should be the filename for MC"<<endl;
    return 0;
  }


  Analysis analysis_data(argv[1]);
  analysis_data.Loop();

  Analysis analysis_mc(argv[2]);
  analysis_mc.Loop();

  Analysis *pointer[2]={&analysis_data,&analysis_mc};
  char *prefix[2]={"Data","MC"};


  // Perform the analysis: for each histogram fit a line+a gaussian
  TF1 func("func","pol1+gaus(2)");
  func.SetParameters(10,0,10,1.0,3*2.5e-3);
  
  
  TH2F tile_mean_h[2];
  TH2F tile_sigma_h[2];
  double tile_mean[2][max_tiles];
  double tile_sigma[2][max_tiles];


  //////////////// RADIATOR TILES MAP
  for(int i=0;i<2;i++){
    tile_mean_h[i].SetBins(300,-70,70,300,-70,70);
    tile_sigma_h[i].SetBins(300,-70,70,300,-70,70);
    for(int loop=0;loop<3;loop++)
    for(int j=0;j<max_tiles;j++){
      if(pointer[i]->betahit_direct_vs_tile[j][loop].Integral()<1000) continue;
      if(RichRadiatorTileManager::get_tile_kind(j)!=agl_kind) continue;
      func.SetParameters(0,0,10,1.0,3*2.5e-3);
      pointer[i]->betahit_direct_vs_tile[j][loop].Fit("func","","",0.962,1.01);
      Double_t a=pointer[i]->betahit_direct_vs_tile[j][loop].GetFunction("func")->GetParameter(0);
      Double_t b=pointer[i]->betahit_direct_vs_tile[j][loop].GetFunction("func")->GetParameter(1);

      for(int bin=1;bin<= pointer[i]->betahit_direct_vs_tile[j][loop].GetNbinsX();bin++){
	double x=pointer[i]->betahit_direct_vs_tile[j][loop].GetXaxis()->GetBinCenter(bin);
	pointer[i]->betahit_direct_vs_tile[j][loop].SetBinError(bin,sqrt(pointer[i]->betahit_direct_vs_tile[j][loop].GetBinContent(bin)));
	if(pointer[i]->betahit_direct_vs_tile[j][loop].GetBinContent(bin)!=0)
	  pointer[i]->betahit_direct_vs_tile[j][loop].SetBinContent(bin,pointer[i]->betahit_direct_vs_tile[j][loop].GetBinContent(bin)-a-b*x);
      }
      pointer[i]->betahit_direct_vs_tile[j][loop].Fit("gaus","","",0.98,1.02);
      if(loop==1){
	tile_mean[i][j]=pointer[i]->betahit_direct_vs_tile[j][loop].GetFunction("gaus")->GetParameter(1);
	tile_sigma[i][j]=pointer[i]->betahit_direct_vs_tile[j][loop].GetFunction("gaus")->GetParameter(2);
      }
    }
  
    for(int bx=1;bx<=tile_mean_h[i].GetNbinsX();bx++)
      for(int by=1;by<=tile_mean_h[i].GetNbinsY();by++){
	float x=tile_mean_h[i].GetXaxis()->GetBinCenter(bx);
	float y=tile_mean_h[i].GetYaxis()->GetBinCenter(by);
	int tile_number=RichRadiatorTileManager::get_tile_number(x,y);
	if(tile_number<0 || tile_number>max_tiles) continue;
	if(tile_mean[tile_number]==0) continue;
	tile_mean_h[i].SetBinContent(bx,by,tile_mean[i][tile_number]);
	tile_sigma_h[i].SetBinContent(bx,by,tile_sigma[i][tile_number]);
      }

  }

#ifdef _TILES_INDEX_
  // Dump a file with all the tiles information
  FILE *fp=fopen("radiator_fits.txt","w");

  for(int i=0;i<RichRadiatorTileManager::get_number_of_tiles();i++){
    if(pointer[0]->betahit_direct_vs_tile[i][1].Integral()<1000) continue;  // DATA
    //    if(pointer[1]->betahit_direct_vs_tile[i][1].Integral()<1000) continue;  // MC

    int tile=i;
    double x=RichRadiatorTileManager::get_tile_x(tile);
    double y=RichRadiatorTileManager::get_tile_y(tile);
    float tile_radiator_index=RichRadiatorTileManager::get_refractive_index(tile);
    double indexes[3];
    indexes[1]=tile_radiator_index;
    indexes[0]=(tile_radiator_index-1)*0.9+1;
    indexes[2]=(tile_radiator_index-1)*1.1+1;
    double beta[3];

    //    cout<<"--------> "<<indexes[0]<<" "<<indexes[1]<<" "<<indexes[2]<<endl;

    //    double goal=pointer[0]->betahit_direct_vs_tile[i][1].GetFunction("gaus")->GetParameter(1);
    double goal=1.0;
    for(int which=0;which<3;which++){
      TF1 *func=pointer[0]->betahit_direct_vs_tile[i][which].GetFunction("gaus");
      if(!func){beta[which]=0;continue;}
      beta[which]=func->GetParameter(1);
    }

    fprintf(fp,
	    "%i %g %g "
	    "%g %g %g "
	    "%g %g %g "
	    "%g\n",
	    tile,x,y,
	    indexes[0],indexes[1],indexes[2],
	    beta[0],beta[1],beta[2],
	    goal);

  }  

  fclose(fp);
#endif



  //////////////// ALIGNMENT ANALYSIS
  TGraphErrors beta_vs_rich_dx[2];
  TGraphErrors beta_vs_rich_dy[2];
  TGraphErrors beta_vs_rich_dz[2];

  TGraphErrors sigma_vs_rich_dx[2];
  TGraphErrors sigma_vs_rich_dy[2];
  TGraphErrors sigma_vs_rich_dz[2];




#ifdef _RICH_ALIGNMENT_
  TFile ff("RichAlignment.root","RECREATE");
  for(int i=0;i<2;i++){

    TH2F *arrays[3]={&(pointer[i]->betahit_direct_vs_dx),&(pointer[i]->betahit_direct_vs_dy),&(pointer[i]->betahit_direct_vs_dz)};
    TGraphErrors *mean_graphs[3]={&(beta_vs_rich_dx[i]),&(beta_vs_rich_dy[i]),&(beta_vs_rich_dz[i])};
    TGraphErrors *sigma_graphs[3]={&(sigma_vs_rich_dx[i]),&(sigma_vs_rich_dy[i]),&(sigma_vs_rich_dz[i])};

    
    for(int which=0;which<3;which++){
      mean_graphs[which]->Set(arrays[which]->GetNbinsX());
      sigma_graphs[which]->Set(arrays[which]->GetNbinsX());
      for(int bin=1;bin<=arrays[which]->GetNbinsX();bin++){
	TH1D *proyection=arrays[which]->ProjectionY("_py",bin,bin);
	
	double mean,mean_error,sigma,sigma_error;
	TF1 *f=Fita(proyection,mean,mean_error,sigma,sigma_error);
	if(!f) continue;
	cout<<"FILLING "<<bin-1<<" "<<mean<<" "<<mean_error<<" "<<sigma<<" "<<sigma_error<<endl;
	mean_graphs[which]->SetPoint(bin-1,arrays[which]->GetXaxis()->GetBinCenter(bin),mean);
	mean_graphs[which]->SetPointError(bin-1,0,mean_error);
	sigma_graphs[which]->SetPoint(bin-1,arrays[which]->GetXaxis()->GetBinCenter(bin),sigma);
	sigma_graphs[which]->SetPointError(bin-1,0,sigma_error);
	proyection->SetName(Form("Rich_%i_%i_%i",i,which,bin));
	proyection->Write();
	//	delete proyection;
      }
    }

  }
  ff.Close();
#endif


  //////////////// MIRROR ALIGNMENT ANALYSIS
  TGraphErrors beta_vs_mirror_dx[2];
  TGraphErrors beta_vs_mirror_dy[2];
  TGraphErrors beta_vs_mirror_dz[2];

  TGraphErrors sigma_vs_mirror_dx[2];
  TGraphErrors sigma_vs_mirror_dy[2];
  TGraphErrors sigma_vs_mirror_dz[2];

#ifdef _MIRROR_ALIGNMENT_
  TFile fff("MirrorAlignment.root","RECREATE");
  for(int i=0;i<2;i++){

    TH2F *arrays[3]={&(pointer[i]->betahit_reflected_vs_dx),&(pointer[i]->betahit_reflected_vs_dy),&(pointer[i]->betahit_reflected_vs_dz)};
    TGraphErrors *mean_graphs[3]={&(beta_vs_mirror_dx[i]),&(beta_vs_mirror_dy[i]),&(beta_vs_mirror_dz[i])};
    TGraphErrors *sigma_graphs[3]={&(sigma_vs_mirror_dx[i]),&(sigma_vs_mirror_dy[i]),&(sigma_vs_mirror_dz[i])};

    
    for(int which=0;which<3;which++){
      mean_graphs[which]->Set(arrays[which]->GetNbinsX());
      sigma_graphs[which]->Set(arrays[which]->GetNbinsX());
      for(int bin=1;bin<=arrays[which]->GetNbinsX();bin++){
	TH1D *proyection=arrays[which]->ProjectionY("_py",bin,bin);
	
	double mean,mean_error,sigma,sigma_error;
	TF1 *f=Fita(proyection,mean,mean_error,sigma,sigma_error);
	if(!f) continue;
	cout<<"FILLING "<<bin-1<<" "<<mean<<" "<<mean_error<<" "<<sigma<<" "<<sigma_error<<endl;
	mean_graphs[which]->SetPoint(bin-1,arrays[which]->GetXaxis()->GetBinCenter(bin),mean);
	mean_graphs[which]->SetPointError(bin-1,0,mean_error);
	sigma_graphs[which]->SetPoint(bin-1,arrays[which]->GetXaxis()->GetBinCenter(bin),sigma);
	sigma_graphs[which]->SetPointError(bin-1,0,sigma_error);
	proyection->SetName(Form("Mirror_%i_%i_%i",i,which,bin));
	proyection->Write();
	//	delete proyection;
      }
    }

  }
  fff.Close();
#endif

  // Write

  TFile f("Results_betahitspectrum.root","RECREATE");
  // Create a directory structure
  gDirectory->mkdir("MC");
  gDirectory->mkdir("Data");
  gDirectory->cd("MC");
  gDirectory->mkdir("Tiles");
  gDirectory->mkdir("RichAlignment");
  gDirectory->mkdir("MirrorAlignment");
  f.cd();
  gDirectory->cd("Data");
  gDirectory->mkdir("Tiles");
  gDirectory->mkdir("TilesFit");
  gDirectory->mkdir("RichAlignment");
  gDirectory->mkdir("MirrorAlignment");

  f.cd();
  gDirectory->cd("Data");
  tile_mean_h[0].Write("Tiles mean");
  tile_sigma_h[0].Write("Tiles sigma");
  pointer[0]->betahit_direct.Write("Beta direct");
  pointer[0]->betahit_reflected.Write("Beta reflected");
  f.cd();
  gDirectory->cd("MC");
  tile_mean_h[1].Write("Tiles mean");
  tile_sigma_h[1].Write("Tiles sigma");
  pointer[1]->betahit_direct.Write("Beta direct");
  pointer[1]->betahit_reflected.Write("Beta reflected");

  for(int i=0;i<2;i++){
    f.cd();
    gDirectory->cd(Form("%s/Tiles",prefix[i]));

    for(int loop=0;loop<3;loop++)
    for(int j=0;j<max_tiles;j++){
      if(RichRadiatorTileManager::get_tile_kind(j)!=agl_kind) continue;
      pointer[i]->betahit_direct_vs_tile[j][loop].Write(Form("betahit_direct_vs_tile %i %i",j,loop));
    }

    f.cd();
    gDirectory->cd(Form("%s/MirrorAlignment",prefix[i]));
    
    pointer[i]->betahit_reflected_vs_dx.Write(Form("betahit_reflected_vs_dx",prefix[i]));
    pointer[i]->betahit_reflected_vs_dy.Write(Form("betahit_reflected_vs_dy",prefix[i]));
    pointer[i]->betahit_reflected_vs_dz.Write(Form("betahit_reflected_vs_dz",prefix[i]));

    f.cd();
    gDirectory->cd(Form("%s/RichAlignment",prefix[i]));
    pointer[i]->betahit_direct_vs_dx.Write("betahit_direct_vs_dx");
    pointer[i]->betahit_direct_vs_dy.Write("betahit_direct_vs_dy");
    pointer[i]->betahit_direct_vs_dz.Write("betahit_direct_vs_dz");

    pointer[i]->betahit_direct_vs_dx_vs_rigidity.Write("betahit_direct_vs_dx_vs_rigidity");
    pointer[i]->betahit_direct_vs_dy_vs_rigidity.Write("betahit_direct_vs_dy_vs_rigidity");
    pointer[i]->betahit_direct_vs_dz_vs_rigidity.Write("betahit_direct_vs_dz_vs_rigidity");



    f.cd();


    f.cd();
    gDirectory->cd(Form("%s",prefix[i]));
    beta_vs_rich_dx[i].Write("Beta Rich alignment X");
    beta_vs_rich_dy[i].Write("Beta Rich alignment Y");
    beta_vs_rich_dz[i].Write("Beta Rich alignment Z");
    sigma_vs_rich_dx[i].Write("Sigma Rich alignment X");
    sigma_vs_rich_dy[i].Write("Sigma Rich alignment Y");
    sigma_vs_rich_dz[i].Write("Sigma Rich alignment Z");

    f.cd();
    gDirectory->cd(Form("%s",prefix[i]));
    beta_vs_mirror_dx[i].Write("Beta Mirror alignment X");
    beta_vs_mirror_dy[i].Write("Beta Mirror alignment Y");
    beta_vs_mirror_dz[i].Write("Beta Mirror alignment Z");
    sigma_vs_mirror_dx[i].Write("Sigma Mirror alignment X");
    sigma_vs_mirror_dy[i].Write("Sigma Mirror alignment Y");
    sigma_vs_mirror_dz[i].Write("Sigma Mirror alignment Z");
  }

  for(int i=0;i<2;i++)
    for(int which=0;which<3;which++){
      for(int bin=1;bin<=analysis_bins;bin++){
	TH1D *pr=(TH1D*)gROOT->FindObjectAny(Form("Rich_%i_%i_%i",i,which,bin));
	if(pr){
	  f.cd();
	  gDirectory->cd(Form("%s/RichAlignment",prefix[i]));
	  pr->Write(pr->GetName());
	}
      
	pr=(TH1D*)gROOT->FindObjectAny(Form("Mirror_%i_%i_%i",i,which,bin));
	if(pr){
	  f.cd();
	  gDirectory->cd(Form("%s/MirrorAlignment",prefix[i]));
	  pr->Write(pr->GetName());
	}
      }
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
