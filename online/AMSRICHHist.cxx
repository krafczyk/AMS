//  $Id: AMSRICHHist.cxx,v 1.10 2011/03/30 14:53:25 mdelgado Exp $
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSRICHHist.h"
#include <TProfile2D.h>
#include <algorithm>

//#define MUONS


// Macros for selection
#define END };}
#define EVERY(_counter)  {\
static int _ANONYMOUS_=-1;\
_ANONYMOUS_++;\
if(_ANONYMOUS_==(_counter)-1)_ANONYMOUS_=0;\
if(_ANONYMOUS_==0){

// Macros for histograms
#define NewSet(_name) {sets[_name]=sets_number++;AddSet(_name);}
#define pushH1(_name,_title,_n,_min,_max) _filled.push_back(new TH1F(#_name,#_title,_n,_min,_max))
#define pushH2(_name,_title,_nx,_minx,_maxx,_ny,_miny,_maxy) _filled.push_back(new TH2F(#_name,#_title,_nx,_minx,_maxx,_ny,_miny,_maxy))
#define pushProf(_name,_title,_n,_min,_max) _filled.push_back(new TProfile(#_name,#_title,_n,_min,_max))
#define pushProf2(_name,_title,_nx,_minx,_maxx,_ny,_miny,_maxy) _filled.push_back(new TProfile2D(#_name,#_title,_nx,_minx,_maxx,_ny,_miny,_maxy))
#define _LAST _filled[_filled.size()-1]
#define setAxisNames(_xtitle,_ytitle,_fillcolor) {_LAST->SetXTitle(#_xtitle);_LAST->SetYTitle(#_ytitle);_LAST->SetFillColor(_fillcolor);}
#define binName(_bin,_name) {_LAST->GetXaxis()->SetBinLabel(_bin,#_name);}
#define get(_name) _filled[FindName(#_name)]





int AMSRICHHist::FindName(TString name){
  for(int i=0;i<_filled.size();i++) if(name==_filled[i]->GetName()) return i;
  return -1;
}

int AMSRICHHist::init(){
  sets_number=0;
  last_run=-1;
  min_run=-1;
  max_run=-1;
}


void AMSRICHHist::Book(){ 
  // Macros to avoid writing too much

  //////////////////////////////////////////////////////////////////////////////////////////////
  NewSet("RICH Raw");       // About occupancy and so on without refering to recsontrcution   //
  //////////////////////////////////////////////////////////////////////////////////////////////

  pushH1(RICHOccupancy,RICH Number of hits,16*680,0,16*680);
  setAxisNames(Rich hits,Number of events,41);

  pushH1(RICHOccupancy2,RICH Occupancy per channel,16*680,0,16*680);
  setAxisNames(Rich Channel,Number of hits,41);
  
  //////////////////////////////////////////////////////////////////////////////////////////////
  NewSet("RICH Hits");      // About hits information (signals and so on)
  //////////////////////////////////////////////////////////////////////////////////////////////


  pushH1(RICHSignal,RICH Hit Charge,500,0,100);
  setAxisNames(Number of photoelectrons,,41);

  pushH1(RICHADC1,RICH ADC gainx1,1024,0,4096);
  setAxisNames(ADC_{#times 1} count,,41);


  pushH1(RICHADC5,RICH ADC gainx5,1024,0,4096);
  setAxisNames(ADC_{#times 5} count,,41); 

  NewSet("RICH Rings");     // About reconstructed rings

  pushH1(RICHRings,RICH Nb. of Rings,30,0,30);
  setAxisNames(RICH rings,,41);

  pushH1(RICHRingProb,RICH Ring probability,50,0.,1.);
  setAxisNames(Ring probability,,41);

  pushH1(RICHBeta,RICH Beta spectrum,100,0.95,1.01);
  setAxisNames(#beta_{RICH}(prob>0.001),,41);

  pushH1(RICHCharge,RICH Charge spectrum,100,0.,10.);
  setAxisNames(#Z_{RICH}(prob>0.001),,41);

  //////////////////////////////////////////////////////////////////////////////////////////////
  NewSet("RICH alignment");     // About RICH/Tracker alignment
  //////////////////////////////////////////////////////////////////////////////////////////////
  

  pushProf(TrXvsRichX,Tracker X vs Rich X,70,-70,70);
  setAxisNames(Tracker X (cm),RICH X-Tracker X (cm),41);

  pushProf(TrYvsRichY,Tracker Y vs Rich Y,70,-70,70);
  setAxisNames(Tracker Y (cm),RICH Y-Tracker Y (cm),41);

  pushH1(TrX-RichX,Tracker X-Rich X,200,-12,12);
  setAxisNames(Tracker X-Rich X (cm),,41);

  pushH1(TrY-RichY,Tracker Y-Rich,200,-12,12);
  setAxisNames(Tracker Y-Rich Y (cm),,41);


  /////////////////////////////////////////
  NewSet("RICH calibration events");
  pushH1(SP0,Single Photoelectron Response grid 0,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP1,Single Photoelectron Response grid 1,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP2,Single Photoelectron Response grid 2,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP3,Single Photoelectron Response grid 3,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP4,Single Photoelectron Response grid 4,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP5,Single Photoelectron Response grid 5,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP6,Single Photoelectron Response grid 6,100,0,4);
  setAxisNames(SPE response,Events,41);
  pushH1(SP7,Single Photoelectron Response grid 7,100,0,4);
  setAxisNames(SPE response,Events,41);

  pushH1(BetaNaF,Rec. Beta,1,0,1);
  setAxisNames(Beta,,41);
  pushH1(BetaAgl,Rec. Beta,100,0.95,1.005);
  setAxisNames(Beta,,41);

  //////////////////////////////////////

  NewSet("RICH calibration events evolution");
  pushProf(SP0T,Single Photoelectron Response grid 0,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP1T,Single Photoelectron Response grid 1,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP2T,Single Photoelectron Response grid 2,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP3T,Single Photoelectron Response grid 3,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP4T,Single Photoelectron Response grid 4,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP5T,Single Photoelectron Response grid 5,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP6T,Single Photoelectron Response grid 6,1,0,4);
  setAxisNames(Time,SPE response,41);
  pushProf(SP7T,Single Photoelectron Response grid 7,1,0,4);
  setAxisNames(Time,SPE response,41);

  pushProf(BetaNaFT,Beta NaF,1,0,1);
  setAxisNames(Time,Beta,41);
  pushProf(BetaAglT,Beta Agl,1,0.95,1.005);
  setAxisNames(Time,Beta,41);


  ///////////////////////////////////////////

  NewSet("RICH radiator");
  const int grid_side_length=15;
  const int n_tiles=grid_side_length*grid_side_length;
  const double tile_width=0.1+11.5; 
  const double limit=grid_side_length*0.5*tile_width;
  gStyle->SetPalette(1);

  pushH2(TileCalOcc,"Tile Occupancy for calibration events",
	 2*grid_side_length,-limit,limit,
	 2*grid_side_length,-limit,limit);

  setAxisNames(X (cm),Y(cm),41);

  pushProf2(TileCalHits,"Tile Hits for calibration events",
	    2*grid_side_length,-limit,limit,
	    2*grid_side_length,-limit,limit);

  setAxisNames(X (cm),Y(cm),41);

  NewSet("Mass Plots");
  pushH1(massSpectrumZ1,"Agl Mass Spectrum for RICH Z=1",100,-2,1);
  setAxisNames(log_{10}  Reconstructed Mass (log_{10} GeV),,41);

  pushH1(massSpectrumZ2,"Agl Mass Spectrum for RICH Z>1",100,-2,2);
  setAxisNames(log_{10} Reconstructed Mass (log_{10} GeV),,41);



  NewSet("RICH Efficiencies");
  pushProf(eff,RICH reconstruction efficiencies,8,1.,8.001);
  setAxisNames(,efficiency,44);
  binName(1,LvL1);
  binName(2,Particle);
  binName(3,nParticle=1);
  binName(4,TrTrack);
  binName(5,RichRing);
  binName(6,RingClean);
  binName(7,AglRing);
  binName(8,NaFRing);

  NewSet("RICH Sel. Efficiencies");
  pushProf(eff2,RICH efficiencies,7,1.,7.001); 
  setAxisNames(,"efficiency",44);
  binName(1,AglRing);
  binName(2,NaFRing);
  binName(9-6,Cal_{rad});
  binName(10-6,Cal_{PMT});
  binName(11-6,Prob>0.01);
  binName(12-6,Width<2.5);
  binName(13-6,Z_{RICH}>1);

  
}

void AMSRICHHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave=gPad;
  for(int i=0;i<_filled.size();i++) _filled[i]->SetStats(false);

  if(Set==sets["RICH Raw"]){
    gPad->Divide(1,2);
    for(int i=0;i<2;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i]->Draw();
      gPadSave->cd();
    }
  }

  if(Set==sets["RICH Hits"]){
    gPad->Divide(1,2);
    gPad->cd(2);
    gPad->Divide(2,1);
    gPadSave->cd();
    gPad->cd(1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[2]->Draw();
    gPadSave->cd();
    for(int i=0;i<2;i++){
      gPad->cd(2);
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i+3]->Draw();
      gPadSave->cd();
    }
  }

  if(Set==sets["RICH Rings"]){
    gPad->Divide(2,2);
    for(int i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i+5]->Draw();
      gPadSave->cd();
    }
  }
    
  if(Set==sets["RICH alignment"]){
    gPad->Divide(2,2);
    for(int i=0;i<4;i++){
      gPad->cd(i+1);
      _filled[i+9]->Draw();
      gPadSave->cd();
    }
  }


  if(Set==sets["RICH calibration events"]){
    gPad->Divide(3,3);
    const int goTo[8]={8,7,4,1,2,3,6,9};

    for(int i=0;i<8;i++){
      gPad->cd(goTo[i]);
      _filled[FindName(Form("SP%i",i))]->Draw();
      gPadSave->cd();
    }
    

    gPad->cd(5);
    get(BetaAgl)->Draw();
    get(BetaNaF)->SetFillColor(0);
    get(BetaNaF)->SetLineColor(2);
    get(BetaNaF)->Draw("same");
    gPadSave->cd();
  }

  if(Set==sets["RICH calibration events evolution"]){
    gPad->Divide(3,3);
    const int goTo[8]={8,7,4,1,2,3,6,9};

    for(int i=0;i<8;i++){
      gPad->cd(goTo[i]);
      _filled[FindName(Form("SP%iT",i))]->SetMarkerStyle(20);
      _filled[FindName(Form("SP%iT",i))]->Draw();
      gPadSave->cd();
    }
    

    gPad->cd(5);
    get(BetaAglT)->SetMarkerStyle(20);
    get(BetaAglT)->SetMarkerColor(1);
    get(BetaAglT)->Draw();
    get(BetaNaFT)->SetMarkerStyle(25);
    get(BetaNaFT)->SetMarkerColor(2);
    get(BetaNaFT)->Draw("same");
    gPadSave->cd();
  }


  if(Set==sets["RICH radiator"]){
    gPad->Divide(2,1);
    gPad->cd(1);
    get(TileCalOcc)->Draw("colz");
    gPadSave->cd();
    gPad->cd(2);
    get(TileCalHits)->Draw("colz");
    gPadSave->cd();
  }


  if(Set==sets["Mass Plots"]){
    gPad->Divide(2,1);
    gPad->cd(1);
    get(massSpectrumZ1)->Draw();
    gPadSave->cd();
    gPad->cd(2);
    get(massSpectrumZ2)->Draw();
    gPadSave->cd();
    //    gPad->cd(3);
    //    gPadSave->cd();
  }


  if(Set==sets["RICH Efficiencies"]){
    //    gPad->Divide(2,1);
    //    gPad->cd(1);
    get(eff)->Draw("hbar2");
    gPadSave->cd();
    //    gPad->cd(2);
    //    get(eff2)->Draw("hbar2");
    //    gPadSave->cd();
  }

  if(Set==sets["RICH Sel. Efficiencies"]){
    //    gPad->Divide(2,1);
    //    gPad->cd(1);
    //    get(eff)->Draw("hbar2");
    //    gPadSave->cd();
    //    gPad->cd(2);
    get(eff2)->Draw("hbar2");
    gPadSave->cd();
  }

}


bool AMSRICHHist::Select(AMSNtupleR *ntuple){
  if(!ntuple->nLevel1()) return false;
  return true;
}



void AMSRICHHist::Fill(AMSNtupleR *ntuple){ 
  if(!Select(ntuple)) return;


  // Fill efficiencies
  int values[21]={0,0,0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,0,0,
		  0};
  
  values[0]=1;

  if(ntuple->nParticle())                               values[1]=1;
  if(ntuple->nParticle()==1)                            values[2]=1;
  if(values[2] && ntuple->pParticle(0)->pTrTrack())     values[3]=1;
  if(values[3] && ntuple->pParticle(0)->pRichRing())    values[4]=1;
  if(values[4]){
    RichRingR *ring=ntuple->pParticle(0)->pRichRing();
    if(ring->IsClean()) values[5]=1;
    if(values[5] && !ring->IsNaF()) values[6]=1;
    if(values[5] && ring->IsNaF()) values[7]=1;

    // Deal with calibrati
    if(values[6] && ring->getCharge2Estimate()<1.5*1.5){
      // Possibly a calibratoin event
#ifndef MUONS
      const float betaCalThreshold=50;
      const float gainCalThreshold=10;
#else
      const float betaCalThreshold=10;
      const float gainCalThreshold=2;
#endif
      if(fabs(ntuple->pParticle(0)->Momentum)>betaCalThreshold) values[8]=1;
      if(fabs(ntuple->pParticle(0)->Momentum)>gainCalThreshold) values[9]=1;
    }
   
    if(ring->Prob>1e-2)    values[10]=1;
    if(ring->getWidth()<2.5) values[11]=1;
    if(ring->getCharge2Estimate()>1.5*1.5) values[12]=1;
  }
  for(int i=1;i<=8;i++) get(eff)->Fill(i,values[i-1]);
  if(values[5])  for(int i=0;i<7;i++) get(eff2)->Fill(i+1,values[6+i]);


#pragma omp critical (croqueta)
  FillTemp(ntuple);

  ////////////////////////////////// FIRST AND SECOND SET 

  get(RICHOccupancy)->Fill(ntuple->nRichHit());

  for(int i=0;i<ntuple->nRichHit();i++){
    RichHitR current=ntuple->RichHit(i);
    get(RICHOccupancy2)->Fill(current.Channel);
    
    get(RICHSignal)->Fill(current.Npe);
    int gain_mask=1<<29;
    if(! (current.Status&gain_mask))
      get(RICHADC1)->Fill(current.Counts);
    else
      get(RICHADC5)->Fill(current.Counts);
  }
  
  get(RICHRings)->Fill(ntuple->nRichRing(),1.);

  /////////////////////////////////////////////////////////////////

  //////////////////////// THIRD SET


  if(ntuple->nParticle()==1 && ntuple->pParticle(0)->pTrTrack()){
    ParticleR current=ntuple->Particle(0);
    if(current.pRichRing() && current.pRichRing()->IsClean())
      {
	get(RICHRingProb)->Fill(current.pRichRing()->Prob);
	
	if(current.pRichRing()->Prob>1e-3){
	  get(RICHBeta)->Fill(current.pRichRing()->getBeta());
	  if(current.pRichRing()->NpExp>0){
	    double collected=current.pRichRing()->getPhotoElectrons();
	    //	    if(current.pRichRing()->NpColLkh>0) collected=current.pRichRing()->NpColLkh;
	    get(RICHCharge)->Fill(sqrt(collected/
				       (current.pRichRing()->NpExp)));
	  }	

	  double x,y;
	  if(!GetCrossing(ntuple,x,y)) return;
	  float *crossingpoint=current.pRichRing()->TrPMTPos;
	  
	  float theta=current.pRichRing()->getTrackTheta();
	  float cosine=fabs(cos(theta));
	  get(TrXvsRichX)->Fill(crossingpoint[0],x-crossingpoint[0]);
	  get(TrYvsRichY)->Fill(crossingpoint[1],y-crossingpoint[1]);
	  get(TrX-RichX)->Fill(crossingpoint[0]-x);
	  get(TrY-RichY)->Fill(crossingpoint[1]-y);
	}
      }
    
  }

  // Fill the mass plots
  if(ntuple->nParticle()==1 && ntuple->pParticle(0)->pTrTrack() && ntuple->pParticle(0)->pRichRing()){  
    RichRingR &ring=*ntuple->pParticle(0)->pRichRing();  
    
    // Reasonable ring
    if(ring.IsClean() && !ring.IsNaF() && ring.Used>3 &&
       ring.NpExp>2 && ring.Prob>1e-2 && ring.getWidth()<2.5){
      double beta=ring.getBeta();
      double gamma2=1/(1-beta*beta);
      double p=fabs(ntuple->pParticle(0)->Momentum);
      double mass2=p*p*(1/beta/beta-1);

      if(gamma2>0 && gamma2<50 && mass2>0){
	// OK, it seems that it is well reconstructed
	// Mass spectrum
	if(ring.getCharge2Estimate()<1.5*1.5){
	  get(massSpectrumZ1)->Fill(0.5*log10(mass2));
	}else{
	  get(massSpectrumZ2)->Fill(0.5*log10(mass2));
	}
   
      }
      
    }


  }

  
}








bool  AMSRICHHist::GetCrossing(AMSNtupleR *ntuple,double &x,double &y){
  ParticleR current=ntuple->Particle(0);
  float *crossingpoint=current.pRichRing()->TrPMTPos;

  static float pmtx[680],pmty[680],entries[680];
  memset(pmtx,0,sizeof(pmtx));
  memset(pmty,0,sizeof(pmty));
  memset(entries,0,sizeof(entries));
  
  int mask=1<<30;                               // Mask of crossed channels
  
  // Search of crossed pmts
  for(int i=0;i<ntuple->nRichHit();i++){
    RichHitR &hit=ntuple->RichHit(i);
    if(!(hit.Status&mask)) continue;
    
    int pmt=hit.Channel/16;
    entries[pmt]+=hit.Npe;
    pmtx[pmt]+=hit.Coo[0]*hit.Npe; 
    pmty[pmt]+=hit.Coo[1]*hit.Npe; 
  }
    
  int winner=-1;
  float winnerdist=1e6;
  
  for(int i=0;i<680;i++){
    if(entries[i]==0) continue;
    pmtx[i]/=entries[i];
    pmty[i]/=entries[i];
    
    float dist=(pmtx[i]-crossingpoint[0])*(pmtx[i]-crossingpoint[0])+(pmty[i]-crossingpoint[1])*(pmty[i]-crossingpoint[1]);
      
    if(dist<winnerdist)
      {
	winnerdist=dist;
	winner=i;
      }
    
  }
  
  if(winner==-1) return false;

  x=pmtx[winner];
  y=pmty[winner];
  
  return true;
}



#define P(x) {cerr<<(#x)<<endl;}

void AMSRICHHist::FillTemp(AMSNtupleR *ntuple){
  const int range=3600*8;   // 8 hours in 5 minutes bin
  const int binwidth=60*30; // 10 minutes


#ifndef MUONS
  const float betaCalThreshold=50;
  const float gainCalThreshold=10;
#else
  const float betaCalThreshold=10;
  const float gainCalThreshold=2;
#endif

  int now=ntuple->fHeader.Time[0];
  // Initialize the histograms if needed
  if(get(BetaAglT)->GetNbinsX()==1){
    // Initialize the 24 hours profile
    int normalized_now=now-(now%binwidth);
    const int twenty4=24*60*60;

    get(BetaAglT)->SetBins((twenty4+6*60*60)/binwidth,normalized_now-6*60*60,normalized_now+twenty4);
    get(BetaAglT)->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
    get(BetaAglT)->GetXaxis()->SetTimeDisplay(1);
    get(BetaAglT)->GetXaxis()->SetTimeFormat("%H/%d/%m %F1970-01-01 00:00:00");
    get(BetaAglT)->GetXaxis()->SetNdivisions(3,kFALSE);
    get(BetaAglT)->GetYaxis()->SetRangeUser(0.97,1.03);
    get(BetaNaFT)->SetBins((twenty4+6*60*60)/binwidth,normalized_now-6*60*60,normalized_now+twenty4);
    get(BetaNaFT)->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
    get(BetaNaFT)->GetXaxis()->SetTimeDisplay(1);
    get(BetaNaFT)->GetXaxis()->SetTimeFormat("%H/%d/%m %F1970-01-01 00:00:00");
    get(BetaNaFT)->GetXaxis()->SetNdivisions(3,kFALSE);
    get(BetaNaFT)->GetYaxis()->SetRangeUser(0.97,1.03);
    for(int i=0;i<8;i++){
      _filled[FindName(Form("SP%iT",i))]->SetBins((twenty4+6*60*60)/binwidth,normalized_now-6*60*60,normalized_now+twenty4);
      _filled[FindName(Form("SP%iT",i))]->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
      _filled[FindName(Form("SP%iT",i))]->GetYaxis()->SetRangeUser(0.90,1.1);
      _filled[FindName(Form("SP%iT",i))]->GetXaxis()->SetTimeDisplay(1);
      _filled[FindName(Form("SP%iT",i))]->GetXaxis()->SetTimeFormat("%H/%d/%m %F1970-01-01 00:00:00");
      _filled[FindName(Form("SP%iT",i))]->GetXaxis()->SetNdivisions(3,kFALSE);
    }
  }
  

  // Adapt the current range
  if(get(BetaAglT)->GetXaxis()->GetBinUpEdge((get(BetaAglT)->GetXaxis()->GetLast()))<=now){
    get(BetaAglT)->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
    get(BetaNaFT)->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
    
    for(int i=0;i<8;i++){
      _filled[FindName(Form("SP%iT",i))]->GetXaxis()->SetRangeUser(now-6*60*60,now+2*60*60);
    }    
  }



  if(ntuple->nParticle()==1 && ntuple->pParticle(0)->pTrTrack()) 1; else return;
  RichRingR *ring=ntuple->pParticle(0)->pRichRing();
  if(!ring) return;
  if(!ring->IsClean()) return;
  
  if(ring->getCharge2Estimate()>1.5*1.5) return;
  
  if(fabs(ntuple->pParticle(0)->Momentum)>betaCalThreshold){
    if(!ring->IsNaF()) {get(BetaAgl)->Fill(ring->Beta);if(ring->Beta>0.99) get(BetaAglT)->Fill(now,ring->Beta);}
    if(ring->IsNaF()) {get(BetaNaF)->Fill(ring->Beta);if(ring->Beta>0.97) get(BetaNaFT)->Fill(now,ring->Beta);}
    float *xpoint=ntuple->pParticle(0)->pRichRing()->TrPMTPos;
    get(TileCalOcc)->Fill(xpoint[0],xpoint[1]);
    ((TProfile2D*)get(TileCalHits))->Fill(xpoint[0],xpoint[1],ring->Used);
  }

  if(fabs(ntuple->pParticle(0)->Momentum)>gainCalThreshold && !ring->IsNaF()){
    for(int i=0;i<ring->Used;i++){
      RichHitR *hit=ntuple->pRichHit(ring->iRichHit(i));
      if(!hit) continue;
      if(hit->Npe>4) continue;

      int channel=hit->Channel/16; 
      int grid=-1;
      if(channel>-1 && channel<=142) grid=0;
      if(channel>142 && channel<=169) grid=1;
      if(channel>169 && channel<=312) grid=2;
      if(channel>312 && channel<=339) grid=3;
      if(channel>339 && channel<=482) grid=4;
      if(channel>482 && channel<=509) grid=5;
      if(channel>509 && channel<=651) grid=6;
      if(channel>651 && channel<=679) grid=7;
      if(grid==-1) continue;
      _filled[FindName(Form("SP%i",grid))]->Fill(hit->Npe);
      _filled[FindName(Form("SP%iT",grid))]->Fill(now,hit->Npe);
    }
  }

}
