#include <iostream.h>
#include "AMSLVL1Hist.h"
#include "AMSDisplay.h"
ClassImp(AMSLVL1Hist)


AMSLVL1Hist::AMSLVL1Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSLVL1Hist::_Fill(){
_m2filled=8;
_filled2= new TH1*[_m2filled];
int i=0;
_filled2[i++]=new TH1F("LVL1-1OR1"," Lvl1 OR Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR2"," Lvl1 OR Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR3"," Lvl1 OR Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR4"," Lvl1 Or Pattern Plane=4",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND1","Lvl1 AND  Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND2","Lvl1 AND  Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND3","Lvl1 AND  Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND4","Lvl1 AND  Pattern Plane=4",14,0.5,14.5);

}





void AMSLVL1Hist::_Fetch(){

}


void AMSLVL1Hist::ShowSet(Int_t Set){
gPad->Clear();
gPad->Divide(2,2);
TVirtualPad * gPadSave = gPad;
for(int i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 if(_filled2[i]){
   ((TH1*)_filled2[i])->SetXTitle(" Paddle No");
   _filled2[i]->Draw();
   _filled2[i]->SetFillColor(2);
   _filled2[i+4]->Draw("SAME");
   _filled2[i]->SetFillColor(42);
 }
gPadSave->cd();
}
}

void AMSLVL1Hist::Fill(AMSNtuple * ntuple){

for(int i=0;i<4;i++){
for(int k=0;k<14;k++){
if(ntuple->_LVL1.TOF[i] & (1<<k))((TH1F*)_filled2[i])->Fill(float(k),1.);
if(ntuple->_LVL1.TOF[i] & (1<<(16+k)))((TH1F*)_filled2[i+4])->Fill(float(k),1.);
}
}

}

