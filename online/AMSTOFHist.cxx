#include <iostream.h>
#include "AMSTOFHist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
#include <TProfile.h>
ClassImp(AMSTOFHist)

AMSTOFHist::AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSTOFHist::_Fill(){
_m2filled=8+4+2;
_filled2= new TH1*[_m2filled];
int i=0;
_filled2[i]=new TH1F("ToF-1OR1"," TOF OR/AND Pattern Plane=1",14,0.5,14.5);
_filled2[i]->SetXTitle(" Paddle No");
_filled2[i++]->SetFillColor(2);
_filled2[i]=new TH1F("ToF-1OR2"," TOF OR/AND Pattern Plane=2",14,0.5,14.5);
_filled2[i]->SetXTitle(" Paddle No");
_filled2[i++]->SetFillColor(2);
_filled2[i]=new TH1F("ToF-1OR3"," TOF OR/AND Pattern Plane=3",14,0.5,14.5);
_filled2[i]->SetXTitle(" Paddle No");
_filled2[i++]->SetFillColor(2);
_filled2[i]=new TH1F("ToF-1OR4"," TOF Or/AND Pattern Plane=4",14,0.5,14.5);
_filled2[i]->SetXTitle(" Paddle No");
_filled2[i++]->SetFillColor(2);
_filled2[i]=new TH1F("ToF-1AND1","TOF AND  Pattern Plane=1",14,0.5,14.5);
_filled2[i++]->SetFillColor(8);
_filled2[i]=new TH1F("ToF-1AND2","TOF AND  Pattern Plane=2",14,0.5,14.5);
_filled2[i++]->SetFillColor(8);
_filled2[i]=new TH1F("ToF-1AND3","TOF AND  Pattern Plane=3",14,0.5,14.5);
_filled2[i++]->SetFillColor(8);
_filled2[i]=new TH1F("ToF-1AND4","TOF AND  Pattern Plane=4",14,0.5,14.5);
_filled2[i++]->SetFillColor(8);
char name[80];
char title[80];

for(i=8;i<12;i++){
 sprintf(name,"MeanTOF%d",i-7);
 sprintf(title,"TOF%d Mean Energy Loss",i-7);
 _filled2[i]=new TProfile(name,title,14,0.5,14.5,0.,80.);
 _filled2[i]->SetXTitle("Paddle no");
 _filled2[i]->SetYTitle("Mean Amplitude (Mev)");
 _filled2[i]->SetMarkerStyle(24);
 _filled2[i]->SetMarkerSize(0.7);
}
_filled2[i]=new TH1D("NToF","Number Of TOF Clusters",10,-0.5,9.5);
_filled2[i]->SetXTitle(" Number of TOF clusters");
_filled2[i++]->SetFillColor(28);
_filled2[i]=new TH1D("Ampl","TOF Energy Loss Distribution",100,0.,20.);
_filled2[i]->SetXTitle(" TOF Energy Loss (MeV)");
_filled2[i++]->SetFillColor(29);




}



void AMSTOFHist::_Fetch(){

}


void AMSTOFHist::ShowSet(Int_t Set){


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
 _filled2[i+4]->Draw("SAME");
gPadSave->cd();
}
break;
case 1:
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[8+i]->Draw();
gPadSave->cd();
}
break;
case 2:
gPad->Divide(1,2);
for(i=0;i<2;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(1);
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[12+i]->Draw();
gPadSave->cd();
}
}



}

void AMSTOFHist::Fill(AMSNtuple *ntuple){
int i;
for(i=0;i<ntuple->_TOF.NToF;i++){
_filled2[ntuple->_TOF.Plane[i]-1]->Fill(ntuple->_TOF.Bar[i],1.);
if((ntuple->_TOF.Status[i] & 256)==0)_filled2[ntuple->_TOF.Plane[i]+3]->Fill(ntuple->_TOF.Bar[i],1.);
_filled2[ntuple->_TOF.Plane[i]-1+8]->Fill(ntuple->_TOF.Bar[i],ntuple->_TOF.Edep[i],1.);
_filled2[13]->Fill(ntuple->_TOF.Edep[i],1.);
}
_filled2[12]->Fill(ntuple->_TOF.NToF,1.);


}
