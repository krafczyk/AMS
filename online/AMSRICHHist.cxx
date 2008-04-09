//  $Id: AMSRICHHist.cxx,v 1.5 2008/04/09 13:35:23 choutko Exp $
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




void AMSRICHHist::Book(){ 

AddSet("RICH Raw");       // About occupancy and so on

  _filled.push_back(new TH1F("RICHOccupancy","RICH Occupancy",16*680,0,16*680));
  _filled[_filled.size()-1]->SetXTitle("Rich hits");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(41);



  _filled.push_back(new TH1F("RICHOccupancy2","RICH Occupancy2",16*680,0,16*680));
  _filled[_filled.size()-1]->SetXTitle("Rich soft Channel");
  _filled[_filled.size()-1]->SetYTitle("Number of hits");
  _filled[_filled.size()-1]->SetFillColor(41);


  AddSet("RICH Hits");      // About hits information (signals and so on)

  _filled.push_back(new TH1F("RICHSignal","RICH Signal",500,0,100));
  _filled[_filled.size()-1]->SetXTitle("Number of photoelectrons");
  _filled[_filled.size()-1]->SetFillColor(41);


  _filled.push_back(new TH1F("RICHADC1","RICH ADC gainx1",1024,0,4096));
  _filled[_filled.size()-1]->SetXTitle("ADC_{#times 1} count");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHADC5","RICH ADC gainx5",1024,0,4096));
  _filled[_filled.size()-1]->SetXTitle("ADC_{#times 5} count");
  _filled[_filled.size()-1]->SetFillColor(41);


  AddSet("RICH Rings");     // About reconstructed rings
  
  _filled.push_back(new TH1F("RICHRings","RICH Rings",30,0,30));
  _filled[_filled.size()-1]->SetXTitle("RICH rings");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHRingProb","RICH Ring probability",50,0.,1.));
  _filled[_filled.size()-1]->SetXTitle("Ring probability");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHBeta","RICH Beta ",50,0.97,1.01));
  _filled[_filled.size()-1]->SetXTitle("#beta_{RICH}(prob>0.001)");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TH1F("RICHCharge","RICH Charge ",100,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("#Z_{RICH}(prob>0.001)");
  _filled[_filled.size()-1]->SetFillColor(41);

  AddSet("RICH alignment");     // About RICH/Tracker alignment
  
  _filled.push_back(new TProfile("TrXvsRichX","Tracker X vs Rich X",70,-70,70));
  _filled[_filled.size()-1]->SetXTitle("Tracker X vs Rich X");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TProfile("TrYvsRichY","Tracker Y vs Rich Y",70,-70,70));
  _filled[_filled.size()-1]->SetXTitle("Tracker Y vs Rich Y");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TProfile("TrX-RichX","Tracker X vs Rich X vs cosine track",100,0,1));
  _filled[_filled.size()-1]->SetXTitle("Tracker X-Rich X vs cos #theta");
  _filled[_filled.size()-1]->SetFillColor(41);

  _filled.push_back(new TProfile("TrY-RichY","Tracker Y vs Rich Y vs cosine track",100,0,1));
  _filled[_filled.size()-1]->SetXTitle("Tracker Y-Rich Y vs cos #theta");
  _filled[_filled.size()-1]->SetFillColor(41);



  //  AddSet("RICH/TRACKER"); // Mass reconstruction

  //  _filled.push_back(new TH1F("RICHMass","RICH Mass ",100,0.,10.));
  //  _filled[_filled.size()-1]->SetXTitle("Mass (prob>0.01)");
  //  _filled[_filled.size()-1]->SetFillColor(41);

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
    _filled[0]->Fill(ntuple->nRichHit(),1.);
    for(int i=0;i<ntuple->nRichHit();i++){
      RichHitR current=ntuple->RichHit(i);
      _filled[1]->Fill(current.Channel,1.);

      _filled[2]->Fill(current.Npe,1.);
      int gain_mask=1<<29;
      if(! (current.Status&gain_mask))
	_filled[3]->Fill(current.Counts,1.);
      else
	_filled[4]->Fill(current.Counts,1.);
    }
    _filled[5]->Fill(ntuple->nRichRing(),1.);
    if(ntuple->nParticle()==1){
      ParticleR current=ntuple->Particle(0);
      if(current.pRichRing())
	{
	  _filled[6]->Fill(current.pRichRing()->Prob,1.);
	  if(current.pRichRing()->Prob>1e-3){
	    _filled[7]->Fill(current.pRichRing()->BetaRefit,1.);
	    if(current.pRichRing()->NpExp>0)
	      _filled[8]->Fill(sqrt((current.pRichRing()->NpCol)/
				    (current.pRichRing()->NpExp)),1.);

	    // Compute preliminary alignment histograms: search a crossed PMT
	    int ring_number=current.iRichRing();
	    float *crossingpoint=current.pRichRing()->TrPMTPos;

	    
	    static float pmtx[680],pmty[680],entries[680];
	    memset(pmtx,0,sizeof(pmtx));
	    memset(pmtx,0,sizeof(pmty));
	    memset(pmtx,0,sizeof(entries));

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
		

	      ((TProfile*)_filled[9])->Fill(pmtx[winner],crossingpoint[0]);
	      ((TProfile*)_filled[10])->Fill(pmty[winner],crossingpoint[1]);
	      ((TProfile*)_filled[11])->Fill(crossingpoint[0]-pmtx[winner],cosine);
	      ((TProfile*)_filled[12])->Fill(crossingpoint[1]-pmty[winner],cosine);
	    }
	    


	  }
	}
      
    }
    
}







