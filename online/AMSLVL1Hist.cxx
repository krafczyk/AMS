#include <iostream.h>
#include "AMSLVL1Hist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
ClassImp(AMSLVL1Hist)


AMSLVL1Hist::AMSLVL1Hist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSLVL1Hist::_Fill(){
_m2filled=16;
_filled2= new TH1*[_m2filled];
int i=0;
_filled2[i++]=new TH1F("LVL1-1OR1"," Lvl1 OR/AND Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR2"," Lvl1 OR/AND Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR3"," Lvl1 OR/AND Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1OR4"," Lvl1 Or/AND Pattern Plane=4",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND1","Lvl1 AND  Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND2","Lvl1 AND  Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND3","Lvl1 AND  Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("LVL1-1AND4","Lvl1 AND  Pattern Plane=4",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1OR1"," TOF OR/AND Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1OR2"," TOF OR/AND Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1OR3"," TOF OR/AND Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1OR4"," TOF Or/AND Pattern Plane=4",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1AND1","TOF AND  Pattern Plane=1",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1AND2","TOF AND  Pattern Plane=2",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1AND3","TOF AND  Pattern Plane=3",14,0.5,14.5);
_filled2[i++]=new TH1F("TOF-1AND4","TOF AND  Pattern Plane=4",14,0.5,14.5);

}





void AMSLVL1Hist::_Fetch(){

}


void AMSLVL1Hist::ShowSet(Int_t Set){
gPad->Clear();
gPad->Divide(2,2);
TVirtualPad * gPadSave = gPad;
int i;
switch (Set){
case 0:
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 if(_filled2[i]){
   ((TH1*)_filled2[i])->SetXTitle(" Paddle No");
   _filled2[i]->SetFillColor(2);
   _filled2[i]->Draw();
   _filled2[i+4]->SetFillColor(8);
   _filled2[i+4]->Draw("SAME");
 }
gPadSave->cd();
}
break;
case 1:
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 if(_filled2[i]){
   ((TH1*)_filled2[i+8])->SetXTitle(" Paddle No");
   _filled2[i+8]->SetFillColor(2);
   _filled2[i+8]->Draw();
   _filled2[i+12]->SetFillColor(8);
   _filled2[i+12]->Draw("SAME");
 }
gPadSave->cd();
}
break;

}
}

void AMSLVL1Hist::Fill(AMSNtuple * ntuple){

for(int i=0;i<4;i++){
for(int k=0;k<14;k++){
if(ntuple->_LVL1.TOF[i] & (1<<k))((TH1F*)_filled2[i])->Fill(float(k),1.);
if(ntuple->_LVL1.TOF[i] & (1<<(16+k)))((TH1F*)_filled2[i+4])->Fill(float(k),1.);
}
}

for(i=0;i<ntuple->_TOF.NToF;i++){
_filled2[ntuple->_TOF.Plane[i]+8-1]->Fill(ntuple->_TOF.Bar[i],1.);
if((ntuple->_TOF.Status[i] & 256)==0)_filled2[ntuple->_TOF.Plane[i]+11]->Fill(ntuple->_TOF.Bar[i],1.);
}

}

