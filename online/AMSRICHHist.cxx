//  $Id: AMSRICHHist.cxx,v 1.9 2011/03/24 15:26:41 mdelgado Exp $
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


int AMSRICHHist::FindName(TString name){
  for(int i=0;i<_filled.size();i++) if(name==_filled[i]->GetName()) return i;
  return -1;
}


void AMSRICHHist::Book(){ 

  AddSet("RICH Raw");       // About occupancy and so on without refering to recsontrcution

  // Macros to avoid writing too much
#define pushH1(_name,_title,_n,_min,_max) _filled.push_back(new TH1F(#_name,#_title,_n,_min,_max))
#define pushProf(_name,_title,_n,_min,_max) _filled.push_back(new TProfile(#_name,#_title,_n,_min,_max))
#define _LAST _filled[_filled.size()-1]
#define setAxisNames(_xtitle,_ytitle,_fillcolor) {_LAST->SetXTitle(#_xtitle);_LAST->SetYTitle(#_ytitle);_LAST->SetFillColor(_fillcolor);}
#define get(_name) _filled[FindName(#_name)]

  pushH1(RICHOccupancy,RICH Number of hits,16*680,0,16*680);
  setAxisNames(Rich hits,Number of events,41);

  pushH1(RICHOccupancy2,RICH Occupancy per channel,16*680,0,16*680);
  setAxisNames(Rich Channel,Number of hits,41);
  
  AddSet("RICH Hits");      // About hits information (signals and so on)

  pushH1(RICHSignal,RICH Hit Charge,500,0,100);
  setAxisNames(Number of photoelectrons,,41);

  pushH1(RICHADC1,RICH ADC gainx1,1024,0,4096);
  setAxisNames(ADC_{#times 1} count,,41);


  pushH1(RICHADC5,RICH ADC gainx5,1024,0,4096);
  setAxisNames(ADC_{#times 5} count,,41); 

  AddSet("RICH Rings");     // About reconstructed rings

  pushH1(RICHRings,RICH Nb. of Rings,30,0,30);
  setAxisNames(RICH rings,,41);

  pushH1(RICHRingProb,RICH Ring probability,50,0.,1.);
  setAxisNames(Ring probability,,41);

  pushH1(RICHBeta,RICH Beta spectrum,50,0.97,1.01);
  setAxisNames(#beta_{RICH}(prob>0.001),,41);

  pushH1(RICHCharge,RICH Charge spectrum,100,0.,10.);
  setAxisNames(#Z_{RICH}(prob>0.001),,41);

  AddSet("RICH alignment");     // About RICH/Tracker alignment
  

  pushProf(TrXvsRichX,Tracker X vs Rich X,70,-70,70);
  setAxisNames(Tracker X (cm),RICH X-Tracker X (cm),41);

  pushProf(TrYvsRichY,Tracker Y vs Rich Y,70,-70,70);
  setAxisNames(Tracker Y (cm),RICH Y-Tracker Y (cm),41);

  pushH1(TrX-RichX,Tracker X-Rich X,200,-12,12);
  setAxisNames(Tracker X-Rich X (cm),,41);

  pushH1(TrY-RichY,Tracker Y-Rich,200,-12,12);
  setAxisNames(Tracker Y-Rich Y (cm),,41);
}


void AMSRICHHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave=gPad;
  switch(Set){
  case 0:
    gPad->Divide(1,2);
    for(int i=0;i<2;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i]->Draw();
      gPadSave->cd();
    }
    break;
  case 1:
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
    break;
  case 2:
    gPad->Divide(2,2);
    for(int i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled[i+5]->Draw();
      gPadSave->cd();
    }
    break;
  case 3:
    gPad->Divide(2,2);
    for(int i=0;i<4;i++){
      gPad->cd(i+1);
      _filled[i+9]->Draw();
      gPadSave->cd();
    }
    //  case 3:
    //    for(int i=0;i<1;i++){
    //      gPad->cd(i+1);
    //      gPad->SetLogx(gAMSDisplay->IsLogX());
    //      gPad->SetLogy(gAMSDisplay->IsLogY());
    //      gPad->SetLogz(gAMSDisplay->IsLogZ());
    //      _filled[i+9]->Draw();
    //      gPadSave->cd();
    //  }
    //    break;
  }
}



void AMSRICHHist::Fill(AMSNtupleR *ntuple){ 
  if(ntuple->nLevel1()){
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

    if(ntuple->nParticle()==1){
      ParticleR current=ntuple->Particle(0);
      if(current.pRichRing())
	{
	  get(RICHRingProb)->Fill(current.pRichRing()->Prob);

	  if(current.pRichRing()->Prob>1e-3){
	    get(RICHBeta)->Fill(current.pRichRing()->getBeta());
	    if(current.pRichRing()->NpExp>0)
	      get(RICHCharge)->Fill(sqrt((current.pRichRing()->getPhotoElectrons())/
					 (current.pRichRing()->NpExp)));
	    
	    // Compute preliminary alignment histograms: search a crossed PMT
	    int ring_number=current.iRichRing();
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
	     
	    if(winner!=-1){
	      // We have a candidate: fill the histograms
	      float cosine=(current.pRichRing()->TrPMTPos[0]-current.pRichRing()->TrRadPos[0])*(current.pRichRing()->TrPMTPos[0]-current.pRichRing()->TrRadPos[0])+
		(current.pRichRing()->TrPMTPos[1]-current.pRichRing()->TrRadPos[1])*(current.pRichRing()->TrPMTPos[1]-current.pRichRing()->TrRadPos[1])+
		(current.pRichRing()->TrPMTPos[2]-current.pRichRing()->TrRadPos[2])*(current.pRichRing()->TrPMTPos[2]-current.pRichRing()->TrRadPos[2]);
	      cosine=sqrt((current.pRichRing()->TrPMTPos[2]-current.pRichRing()->TrRadPos[2])*(current.pRichRing()->TrPMTPos[2]-current.pRichRing()->TrRadPos[2])/cosine);
	      
	      
	      get(TrXvsRichX)->Fill(crossingpoint[0],pmtx[winner]-crossingpoint[0]);
	      get(TrYvsRichY)->Fill(crossingpoint[1],pmty[winner]-crossingpoint[1]);
	      get(TrX-RichX)->Fill(crossingpoint[0]-pmtx[winner]);
	      get(TrY-RichY)->Fill(crossingpoint[1]-pmty[winner]);
	    }
	    


	  }
	}
      
    }
  }
}







