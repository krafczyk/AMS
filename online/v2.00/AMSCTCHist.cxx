//  $Id: AMSCTCHist.cxx,v 1.2 2001/01/22 17:32:52 choutko Exp $
#include <iostream.h>
#include "AMSCTCHist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
#include <TProfile.h>
ClassImp(AMSCTCHist)
AMSCTCHist::AMSCTCHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSCTCHist::_Fill(){

_m2filled=4;
_filled2= new TH1*[_m2filled];
for(int mf=0;mf<_m2filled;mf++)_filled2[mf]=0;
char name[80];
char title[80];
for(int ilay=0;ilay<2;ilay++){
   sprintf(name,"CTCL%d-AllOcc",ilay+1);
   sprintf(title,"CTC Layer %d Row All Occupancies",ilay+1);
   _filled2[ilay*2]=new TH1D(name,title,44,0.5,44.5);
   _filled2[ilay*2]->SetXTitle("Channel No");
   _filled2[ilay*2]->SetFillColor(32);
   sprintf(name,"CTCL%d-AllA",ilay+1);
   sprintf(title,"CTC Layer %d Row All Mean Amplitudes",ilay+1);
   _filled2[ilay*2+1]=new TProfile(name,title,44,0.5,44.5,0.,200.);
   _filled2[ilay*2+1]->SetMarkerStyle(23);
   _filled2[ilay*2+1]->SetMarkerSize(0.7);
   _filled2[ilay*2+1]->SetXTitle("Channel no");
   _filled2[ilay*2+1]->SetFillColor(31);
   //   _filled2[ilay*2+1]->SetYTitle("Mean Amplitude(p.e.)");


}



}




void AMSCTCHist::_Fetch(){

}


void AMSCTCHist::ShowSet(Int_t Set){


gPad->Clear();
TVirtualPad * gPadSave = gPad;
int i;
switch(Set){
case 0:
gPad->Divide(1,2);
for(i=0;i<2;i++){
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
for(i=0;i<2;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[2+i]->Draw();
gPadSave->cd();
}

}




}

void AMSCTCHist::Fill(AMSNtuple * ntuple){
  for(int i=0;i<ntuple->_CTC.nhit;i++){
    if(ntuple->_CTC.y[i]%2 == 0){
     int ilay=ntuple->_CTC.layer[i]-1;
     int ic=ntuple->_CTC.y[i]/2-1;
     _filled2[ilay*2]->Fill(ntuple->_CTC.x[i]+ic*(10+ilay),1.);
     _filled2[ilay*2+1]->Fill(ntuple->_CTC.x[i]+ic*(10+ilay),ntuple->_CTC.signal[i]*2,1.);
    }
  }


}
