//  $Id: AMSAntiHist.cxx,v 1.4 2001/01/22 17:32:52 choutko Exp $
#include <iostream.h>
#include "AMSAntiHist.h"
#include "AMSDisplay.h"
#include <TProfile.h>
#include "AMSNtuple.h"
#include <TPaveText.h>
ClassImp(AMSAntiHist)

AMSAntiHist::AMSAntiHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset, active){
    _Fetch();
    _Fill();    
}

void AMSAntiHist::_Fill(){

  _m2filled=16+4;
  _filled2= new TH1*[_m2filled];
for(int mf=0;mf<_m2filled;mf++)_filled2[mf]=0;
  char name[80];
  char text[80];
  for(int i=0;i<16;i++){
    sprintf(name,"AntiS_%2d",i+1);
    sprintf(text," Anti Sector %2d up vs down",i+1);
    _filled2[i]=new TH2F(name,text,50,0.,20.,50,0.,20.);
    _filled2[i]->Fill(0.,0.,1.);  
  }
  
  i=16;
  _filled2[i]=new TH1D("AntiOccUp","Anti Sectors Occupancy up",16,0.5,16.5);
  _filled2[i++]->SetFillColor(42);
  _filled2[i]=new TH1D("AntiOccDown","Anti Sectors Occupancy down",16,0.5,16.5);
  _filled2[i++]->SetFillColor(42);
  _filled2[i++]=new TProfile("AntiProfileUp","Anti Sectors Profile Up",16,0.5,16.5);
  _filled2[i++]=new TProfile("AntiProfileDown","Anti Sectors Profile Down",16,0.5,16.5);
  
  
  for(int k=0;k<2;k++) {
    _filled2[18+k]->SetMarkerStyle(21);
    _filled2[18+k]->SetMarkerSize(0.7);
  }
  _filled2[16]->SetXTitle("Up Counter No");
  _filled2[16]->SetYTitle("Occupancy");
  _filled2[17]->SetXTitle("Down Counter No");
  _filled2[17]->SetYTitle("Occupancy");
  _filled2[18]->SetXTitle("Up Counter No");
  _filled2[18]->SetYTitle("Mean Amplitude (MeV)");
  _filled2[19]->SetXTitle("Down Counter No");
  _filled2[19]->SetYTitle("Mean Amplitude (MeV)");
  
}



void AMSAntiHist::_Fetch(){
  char text[80];
    _m2fetched=16;
    _fetched2= new TH1*[_m2fetched];
    for(int i=0;i<16;i++)_fetched2[i]=0;
  //  for(i=0;i<16;i++){
  //    sprintf(text,"h1%02d3",i+1);
  //    _fetched2[i]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
  //    if(_fetched2[i]){
  //      _fetched2[i]->SetXTitle("Up counter Amplitude (MeV)");
  //      _fetched2[i]->SetYTitle("Down Counter Amplitude (MeV)");
  //    }
  //  }
}

void AMSAntiHist::ShowSet(Int_t Set){
  gPad->Clear();
  int i;
  TVirtualPad * gPadSave;
  switch(Set){
  case 0:
    gPad->Divide(2,2);
    gPadSave = gPad;
    if(_filled2[16] && _filled2[17] && _filled2[18] && _filled2[19]){
      for (int i=0;i<4;i++){
	gPad->cd(i+1);
	gPad->SetLogx(gAMSDisplay->IsLogX());
	gPad->SetLogy(gAMSDisplay->IsLogY());
	gPad->SetLogz(gAMSDisplay->IsLogZ());
	if(i<2)_filled2[16+i]->Draw();
	if(i>=2) _filled2[16+i]->Draw();
	gPadSave->cd();
      }
    }
    break;
  case 1: case 2: case 3: case 4:
    int init=(Set-1)*4;  
    gPad->Divide(2,2);
    gPadSave = gPad;
    for(i=init;i<init+4;i++){
      gPad->cd(i-init+1);
      //gPad->SetLogx(gAMSDisplay->IsLogX());
      //gPad->SetLogy(gAMSDisplay->IsLogY());
      //gPad->SetLogz(gAMSDisplay->IsLogZ());
      if(_fetched2[i]){
	_fetched2[i]->Draw();
	char text[80];
	sprintf(text,"Sector %02d UP vs DOWN",i+1);
	gPad->cd(i-init+1);
	TPaveLabel *lf=new TPaveLabel(30.,45.,49.,49.,text);
	lf->SetFillColor(41);
	lf->Draw();
	
      }
      else{
	if(_filled2[i]){
	  _filled2[i]->SetXTitle("Up counter Amplitude (MeV)");
	  _filled2[i]->SetYTitle("Down Counter Amplitude (MeV)");
	  _filled2[i]->Draw();
	  //    char text[80];
	  //    sprintf(text,"Sector %02d UP vs DOWN",i+1);
	  //    TPaveText * lf =new TPaveText(0.8,0.8,0.9,0.9,"NDC");
	  //    lf->AddText(text);
	  //    lf->SetFillColor(41);
	  //    lf->Draw();
	  
	}
      }
      gPadSave->cd();
    }
    break;
    
  }
}

void AMSAntiHist::Fill(AMSNtuple * ntuple){
  for(int i=0;i<ntuple->_ANTI.nantiraw;i++){
    int sector=ntuple->_ANTI.AntirawSector[i]-1;   
    int updown=ntuple->_ANTI.Antirawupdown[i];
    _filled2[16+updown]->Fill(float(sector+1),1.);
    _filled2[18+updown]->Fill(float(sector+1),ntuple->_ANTI.AntirawSignal[i],1.);
    for(int j=0;j<ntuple->_ANTI.nantiraw;j++){
      if(sector == ntuple->_ANTI.AntirawSector[j] &&
	 updown !=ntuple->_ANTI.Antirawupdown[j]){
        if(updown==0)((TH2F*)_filled2[sector])->Fill(ntuple->_ANTI.AntirawSignal[i],
						     ntuple->_ANTI.AntirawSignal[j],1.);       
        else ((TH2F*)_filled2[sector])->Fill(ntuple->_ANTI.AntirawSignal[j],
					     ntuple->_ANTI.AntirawSignal[i],1.);       
      }
    }
  }
  
}





