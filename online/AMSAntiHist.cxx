#include <iostream.h>
#include "AMSAntiHist.h"
#include "AMSDisplay.h"
ClassImp(AMSAntiHist)

AMSAntiHist::AMSAntiHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset, active){
    _Fetch();
    _Fill();    
}

void AMSAntiHist::_Fill(){
}



void AMSAntiHist::_Fetch(){
char text[80];
_m2fetched=16;
_fetched2= new TH1*[_m2fetched];
for(int i=0;i<16;i++){
  sprintf(text,"h1%02d3",i+1);
  _fetched2[i]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
  if(_fetched2[i]){
  }
  else{
    cerr<<"AMSAntiHist::_Fetch-E-NoHisFound "<<text<<endl;
  }    
}

}


void AMSAntiHist::ShowSet(Int_t Set){
gPad->Clear();
int init=Set*4;  
gPad->Divide(2,2);
 TVirtualPad * gPadSave = gPad;
for(int i=init;i<init+4;i++){
gPad->cd(i-init+1);
gPad->SetLogx(gAMSDisplay->IsLogX());
gPad->SetLogy(gAMSDisplay->IsLogY());
gPad->SetLogz(gAMSDisplay->IsLogZ());
if(_fetched2[i]){
   ((TH1*)_fetched2[i])->SetXTitle("Up counter Amplitude (MeV)");
  ((TH1*)_fetched2[i])->SetYTitle("Down Counter Amplitude (MeV)");
 _fetched2[i]->Draw();
    char text[80];
    sprintf(text,"Sector %02d UP vs DOWN",i+1);
gPad->cd(i-init+1);
   TPaveLabel *lf  = new TPaveLabel(30.,45.,49.,49.,text);
   lf->SetFillColor(41);
   lf->Draw();

}
gPadSave->cd();
}


}

