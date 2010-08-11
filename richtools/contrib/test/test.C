#include "test.h"
#include "TF1.h"
#include <iostream>

using namespace std;


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



Analysis::Analysis(TString file){
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
  //  RichRing::UseReflected=false;
  

  // Set the alignment
  RichAlignment::Set(0,0,0,0,0,0);
  RichRing::DeltaHeight=0;
  // FOR REAL DATA ONLY
#undef REALDATA
#ifdef REALDATA
    RichAlignment::Set(0,0.15,0.5,0,0,0);
  //  RichAlignment::Set(-0.03,0.14,0.74,0,0,0);
  RichRing::DeltaHeight=-0.655608413971428550; 
#endif

  summary=TH1F("summ","summ",1000,0.95,1.05);
  summary_used=TH1F("summ used","summ used",1000,0.95,1.05);
  summary_back=TH1F("summ back","summ back",1000,0.95,1.05);
  summary_ref=TH1F("summ ref","summ ref",1000,0.95,1.05);
  summary_ref_used=TH1F("summ ref used","summ ref used",1000,0.95,1.05);
  summary_ref_back=TH1F("summ ref back","summ ref back",1000,0.95,1.05);

  beta=TH2F("Beta","Beta",1000,0.95,1.05,20,0,20);
  wbeta=TH2F("WBeta","WBeta",1000,0.95,1.05,20,0,20);
  
  HotSpotShift=TH2F("HotSpotShift","HotSpotShift",100,-3.4,3.4,100,-3.4,3.4);
  HotSpotLG=TH2F("HotSpotLG","HotSpotLS",100,-3.4,3.4,100,-3.4,3.4);
  ShiftVsTheta=TH2F("ShiftVsTheta","ShiftVsTheta",100,0.85,1.0,100,-3.4,3.4);
  Shift=TH3F("Shift","shift",100,-3.4,3.4,100,-3.4,3.4,100,0.85,1);

  for(int w=0;w<3;w++)
    for(int r=0;r<2;r++){
      beta_win_ref[w][r]=TH1F(
			      Form("beta_win_ref %i %i",r,w),
			      Form("beta_win_ref %i %i",r,w),
			      1000,0.95,1.05);

      beta_win_ref_used[w][r]=TH1F(
				   Form("beta_win_ref_used %i %i",r,w),
				   Form("beta_win_ref_used %i %i",r,w),
				   1000,0.95,1.05);
    }

  for(int i=0;i<121;i++){
    beta_tile[i]=TH1F(Form("beta tile %i",i),
		      Form("beta tile %i",i),
		      1000,0.95,1.05);

    beta_tile_ref[i]=TH1F(Form("beta tile ref %i",i),
			  Form("beta tile ref %i",i),
			  1000,0.95,1.05);

    final_beta[i]=TH1F(Form("Final Beta %i",i),
		       Form("Final Beta %i",i),
		       1000,0.95,1.05);

    final_wbeta[i]=TH1F(Form("Final WBeta %i",i),
		       Form("Final WBeta %i",i),
			1000,0.95,1.05);

  }



  last_cut=0;
  for(int i=0;i<max_cuts;i++) selected[i]=0;

}


#define SELECT(name,cond) {if(cond){                                    \
                                   if(!selected[last_cut])             \
                                     sprintf(cut_name[last_cut],name); \
                                   selected[last_cut++]++;             \
                                   }else return false;}



bool Analysis::HotSpotPos(AMSEventR *event,double &x,double &y,double &lx,double &ly){

  RichRingR *ring=event->pParticle(0)->pRichRing();
  if(!ring) return false;

  // Search the pmt with hot-spots compatible with the track
  double x_pmt[680];
  double y_pmt[680];
  double counts[680];
  for(int i=0;i<680;i++) x_pmt[i]=y_pmt[i]=counts[i]=0;
  
  for(int i=0;i<event->nRichHit();i++){
    RichHitR *hit=event->pRichHit(i);
    if(!(hit->Status&(1<<30)))  continue;
    
    int pmt=hit->Channel/16;
    
    //    cout<<"PMT "<<pmt<<" hit "<<i<<endl;

    double weight=hit->Npe;
    counts[pmt]+=weight;
    x_pmt[pmt]+=weight*hit->Coo[0];
    y_pmt[pmt]+=weight*hit->Coo[1];
  }

  


  double best=0;
  int winner=-1;
  for(int i=0;i<680;i++){
    if(counts[i]==0) continue;
    //    cout<<i<<"....";
    if(counts[i]>best){
      best=counts[i];
      x=x_pmt[i]/counts[i];
      y=y_pmt[i]/counts[i];
      winner=i;
      //      cout<<x<<" "<<y<<endl;
    }
    //    cout<<endl;
  }

  /*

  
  double best=-HUGE_VAL;
  int winner=-1;
  x=0;
  y=0;
  RichRingR *ring=event->pParticle(0)->pRichRing();
  if(!ring) return false;
  for(int i=0;i<event->nRichHit();i++){
    RichHitR *hit=event->pRichHit(i);
    if(!(hit->Status&(1<<30)) || hit->Npe<best) continue;
    if(fabs(hit->Coo[0]-ring->TrPMTPos[0])>3.4 || fabs(hit->Coo[1]-ring->TrPMTPos[1])>3.4) continue;
    winner=hit->Channel/16;
    best=hit->Npe;
    x=hit->Coo[0];
    y=hit->Coo[1];
  }
  */
  

  if(best<=0) return false;

  lx=x-RichPMTsManager::GetRichPMTPos(winner,0);
  ly=y-RichPMTsManager::GetRichPMTPos(winner,1);


  // Compute the position of the track ignoring all the used alignment
  double dx=event->pParticle(0)->RichCoo[1][0]-event->pParticle(0)->RichCoo[0][0];
  double dy=event->pParticle(0)->RichCoo[1][1]-event->pParticle(0)->RichCoo[0][1];
  double dz=event->pParticle(0)->RichCoo[1][2]-event->pParticle(0)->RichCoo[0][2];
  
  double px=event->pParticle(0)->RichCoo[1][0]+dx/dz*(RICHDB::cato_pos()+RICHDB::pmtb_height()/2.);
  double py=event->pParticle(0)->RichCoo[1][1]+dy/dz*(RICHDB::cato_pos()+RICHDB::pmtb_height()/2.);


  x-=px;
  y-=py;


  return true;


}

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
  //  SELECT("Aerogel track and clean ring",(event->pParticle(0)->pRichRing()->Status&3)==0);
  SELECT("Clean ring",(event->pParticle(0)->pRichRing()->Status&1)==0);
  SELECT("At most 1 hot spot",event->pParticle(0)->RichParticles<=1);
  //  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>4);
  SELECT("Beta==1 (momentum selection)",fabs(event->pParticle(0)->Momentum)>10);
  RichRingR *ring=event->pParticle(0)->pRichRing();
  /*
  SELECT("VERTICAL TRACKS ",
	 fabs((ring->TrPMTPos[0]-ring->TrRadPos[0])/(ring->TrPMTPos[2]-ring->TrRadPos[2]))*3<3.4/4 &&
	 fabs((ring->TrPMTPos[1]-ring->TrRadPos[1])/(ring->TrPMTPos[2]-ring->TrRadPos[2]))*3<3.4/4);
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


    // Get the hotspot position
    double x,y,lx,ly;
    if(HotSpotPos(event,x,y,lx,ly)){
      HotSpotShift.Fill(x,y);
      HotSpotLG.Fill(lx,ly);
      ShiftVsTheta.Fill(fabs(cos(event->pParticle(0)->Theta)),x*cos(event->pParticle(0)->Phi)+y*sin(event->pParticle(0)->Phi));
      Shift.Fill(x*cos(event->pParticle(0)->Phi)+y*sin(event->pParticle(0)->Phi),y*cos(event->pParticle(0)->Phi)-x*sin(event->pParticle(0)->Phi),fabs(cos(event->pParticle(0)->Theta)));
    }

    RichRing *ring=RichRing::build(event,event->pParticle(0)->pRichRing()->pTrTrack());
    if(!ring) continue;
    
    
    if(ring->_used==5){
      final_beta[RichRing::_tile_index].Fill(ring->_beta);
      final_wbeta[RichRing::_tile_index].Fill(ring->_wbeta);
    }
    


    /*
    if(RichRing::_kind_of_tile==naf_kind){
      // Fill the histogram 120
      for(int j=0;j<ring->_hit_used.size();j++){
	beta_tile[120].Fill(ring->_beta_direct[j]);
	beta_tile_ref[120].Fill(ring->_beta_reflected[j]);
      }
    }else{
      // Fill the histogram 120
      for(int j=0;j<ring->_hit_used.size();j++){
	beta_tile[119].Fill(ring->_beta_direct[j]);
	beta_tile_ref[119].Fill(ring->_beta_reflected[j]);
      }
    }
    */

    beta.Fill(ring->_beta,ring->_used);
    wbeta.Fill(ring->_wbeta,ring->_used);
    //    beta.Fill(event->pParticle(0)->pRichRing()->Beta,event->pParticle(0)->pRichRing()->Used);
    
    // fill per window and per reflected or not (for the used only)
    for(int j=0;j<ring->_hit_used.size();j++){
      summary.Fill(ring->_beta_direct[j]);
      if(ring->_hit_used[j]==0) summary_used.Fill(ring->_beta_direct[j]);
      else summary_back.Fill(ring->_beta_direct[j]);

      summary_ref.Fill(ring->_beta_reflected[j]);
      if(ring->_hit_used[j]==1) summary_ref_used.Fill(ring->_beta_reflected[j]);       else summary_ref_back.Fill(ring->_beta_reflected[j]);

      if(ring->_hit_used[j]==0)
	beta_tile[RichRing::_tile_index].Fill(ring->_beta_direct[j]);
      if(ring->_hit_used[j]==1)
      beta_tile_ref[RichRing::_tile_index].Fill(ring->_beta_reflected[j]);

      int window=(event->pRichHit(ring->_hit_pointer[j])->Channel%16);
      int kind=1;
      if(window==5 || window==6 || window==9 || window==10) kind=0;
      if(window==0 || window==3 || window==12 || window==15) kind=2;
      beta_win_ref[kind][0].Fill(ring->_beta_direct[j]);
      beta_win_ref[kind][1].Fill(ring->_beta_reflected[j]);

      if(ring->_hit_used[j]!=0 && ring->_hit_used[j]!=1) continue;
      double beta=ring->_hit_used[j]==0?ring->_beta_direct[j]:ring->_beta_reflected[j];
      beta_win_ref_used[kind][ring->_hit_used[j]].Fill(beta);

    }

    delete ring;
  }

  for(int i=0;i<RichRadiatorTileManager::get_number_of_tiles();i++){
    double current=RichRadiatorTileManager::get_refractive_index(i);
    double x=RichRadiatorTileManager::get_tile_x(i);
    double y=RichRadiatorTileManager::get_tile_y(i);
    cout<<i<<" "<<x<<" "<<y<<" "<<current<<endl;

  }

}


int main(int argc,char **argv){
  if(argc<2){
    cout<<"First argument should be the filename"<<endl;
    return 0;
  }


  double sigma_tile[121],mean_tile[121];
  double sigma_tile_ref[121],mean_tile_ref[121];
  TH1F sigmas("Sigmas","Sigmas",100,2e-3,3e-3);
  TH1F means("Means","Means",1000,0.998,1.002);
  TH1F sigmas_ref("SigmasRef","SigmasRef",100,2e-3,3e-3);
  TH1F means_ref("MeansRef","MeansRef",1000,0.998,1.002);

  // Start the Analysis
  Analysis analysis(argv[1]);
  analysis.Loop();

  TFile file("test.root","RECREATE");
  analysis.beta.Write();
  analysis.wbeta.Write();
  analysis.summary.Write();
  analysis.summary_used.Write();
  analysis.summary_ref.Write();
  analysis.summary_ref_used.Write();
  analysis.summary_back.Write();
  analysis.summary_ref_back.Write();
  for(int i=0;i<3;i++) for(int j=0;j<2;j++) {analysis.beta_win_ref[i][j].Write();analysis.beta_win_ref_used[i][j].Write();}
  for(int i=0;i<119;i++){
    sigma_tile[i]=mean_tile[i]=-1;
    double mean,mean_error,sigma,sigma_error;
    TF1 *f=Fita(&analysis.beta_tile[i],mean,mean_error,sigma,sigma_error);
    analysis.beta_tile[i].Write();
    if(!f) continue;
    sigmas.Fill(sigma);
    means.Fill(mean);
    sigma_tile[i]=sigma;
    mean_tile[i]=mean;
  }


  sigmas.Write();
  means.Write();

  for(int i=0;i<121;i++){
    analysis.final_beta[i].Write();
    analysis.final_wbeta[i].Write();
    sigma_tile_ref[i]=mean_tile_ref[i]=-1;
    double mean,mean_error,sigma,sigma_error;
    TF1 *f=Fita(&analysis.beta_tile_ref[i],mean,mean_error,sigma,sigma_error);
    analysis.beta_tile_ref[i].Write();
    if(!f) continue;
    sigmas_ref.Fill(sigma);
    means_ref.Fill(mean);
    sigma_tile_ref[i]=sigma;
    mean_tile_ref[i]=mean;
  }

  analysis.beta_tile[119].Write();
  analysis.beta_tile[120].Write();
  analysis.beta_tile[121].Write();

  analysis.HotSpotShift.Write();
  analysis.HotSpotLG.Write();
  analysis.ShiftVsTheta.Write();
  analysis.Shift.Write();

  file.Close();




  /*
  for(int i=0;i<120;i++){
    if(mean_tile[i]<0) continue;
    int &tile=i;
    double x=RichRadiatorTileManager::get_tile_x(tile);
    double y=RichRadiatorTileManager::get_tile_y(tile);
    double clarity=RichRadiatorTileManager::get_clarity(tile);
    double height=RichRadiatorTileManager::get_height(tile);
    double index=RichRadiatorTileManager::get_refractive_index(tile);
    printf("%i %g %g %g %g %g %g %g %g %g %g %g\n",
	   i,x,y,index,height,clarity,mean_tile[i],sigma_tile[i],analysis.beta_tile[i].Integral(),mean_tile_ref[i],sigma_tile_ref[i],analysis.beta_tile_ref[i].Integral());
  }
  */
  // Output the selection efficiencies
  for(int i=1;analysis.selected[i]>0;i++){
    cout<<"CUT "<<analysis.cut_name[i]
	<<" DATA ENTRIES "<<analysis.selected[i]<<" DATA EFF "<<double(analysis.selected[i])/analysis.selected[i-1]<<endl;
  }




  return 0;
}
