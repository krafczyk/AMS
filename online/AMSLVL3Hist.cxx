#include <iostream.h>
#include "AMSLVL3Hist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
ClassImp(AMSLVL3Hist)

AMSLVL3Hist::AMSLVL3Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSLVL3Hist::_Fill(){
_m2filled=6;
_filled2= new TH1*[_m2filled];
int i=0;
_filled2[i]=new TH1F("LVL3Output","LVL3 Output",17,-0.5,16.5);
_filled2[i]->SetXTitle("LVL3 Output");
_filled2[i++]->SetFillColor(41);
_filled2[i]=new TH1F("LVL3Hits","LVL3 Hits",60,-0.5,59.5);
_filled2[i]->SetXTitle("Number Of Tracker Hits");
_filled2[i++]->SetFillColor(43);
_filled2[i]=new TH1F("LVL3Residual","LVL3 Residual",50,-2.,2.);
_filled2[i]->SetXTitle("Average Residual (mm)");
_filled2[i++]->SetFillColor(42);
_filled2[i]=new TH1F("LVL3ELoss","LVL3 ELoss",500,0.,5000.);
_filled2[i]->SetXTitle("Tracker Energy Loss (ADC ch *8)");
_filled2[i++]->SetFillColor(44);
_filled2[i]=new TH1F("LVL3Output1","LVL3 Output Positive Rigidity",17,-0.5,16.5);
_filled2[i]->SetXTitle("LVL3 Output");
_filled2[i++]->SetFillColor(41);
_filled2[i]=new TH1F("LVL3Output2","LVL3 Output Negative Rigidity",17,-0.5,16.5);
_filled2[i]->SetXTitle("LVL3 Output");
_filled2[i++]->SetFillColor(42);

}



void AMSLVL3Hist::_Fetch(){

}


void AMSLVL3Hist::ShowSet(Int_t Set){
gPad->Clear();
TVirtualPad * gPadSave = gPad;
int i;
switch(Set){
case 0:
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[i]->Draw();
gPadSave->cd();
}
break;
case 1:
gPad->Divide(1,2);
for(i=4;i<6;i++){
 gPad->cd(i+1-4);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[i]->Draw();
gPadSave->cd();
}

}


}

void AMSLVL3Hist::Fill(AMSNtuple * ntuple){
  if(ntuple->_LVL3.nlvl3 ){
    _filled2[0]->Fill(ntuple->_LVL3.TrackerTr,1.);
    if(ntuple->_LVL3.TOFTr){
     _filled2[1]->Fill(ntuple->_LVL3.NtrHits,1.);
     if(ntuple->_LVL3.NPat){
      _filled2[2]->Fill(ntuple->_LVL3.Residual[0],1.);
      _filled2[3]->Fill(ntuple->_LVL3.ELoss,1.);
        } 
    }
    if(ntuple->_AxAMS.npart){
     if(ntuple->_AxAMS.pmom[0] > 0)_filled2[4]->Fill(ntuple->_LVL3.TrackerTr,1.);
     else _filled2[5]->Fill(ntuple->_LVL3.TrackerTr,1.);
    }
  }
}
