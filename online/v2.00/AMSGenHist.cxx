#include <iostream.h>
#include "AMSGenHist.h"
#include "AMSDisplay.h"
#include <TProfile.h>
#include "AMSNtuple.h"
#include <TPaveText.h>
ClassImp(AMSGenHist)

const   int nids = 19;

unsigned int hids[nids] =
  { 0, 1, 2, 0x200,
    0x1401,0x1441,0x1481,0x14C1,0x1501,0x1541,0x1581,0x15C1,  //  TOF Raw
    0x1680, 0x1740,                                           //  TRK Reduced
    0x1681, 0x1741,                                           //  TRK Raw
    0x168C, 0x174C,                                           //  TRK Mixed
    0x0440                                                    //  Level-3
  };



AMSGenHist::AMSGenHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset, active){
    _Fetch();
    _Fill();    
}

void AMSGenHist::_Fill(){}



void AMSGenHist::_Fetch(){
  char text[80];
    _m2fetched=nids;
    _fetched2= new TH1*[_m2fetched];
    for(int i=0;i<nids;i++)_fetched2[i]=0;
    for(i=0;i<nids;i++){
    int ii = 300000 + hids[i];
    sprintf(text,"h%6d",ii);
      _fetched2[i]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(_fetched2[i]){
        _fetched2[i]->SetXTitle("Bytes*2");
        _fetched2[i]->SetFillColor(41);
      }
    }
}

void AMSGenHist::ShowSet(Int_t Set){
  const int sets = 4;
  gPad->Clear();
  int init[sets+1]    = {0, 4, 12, 14, 18};
  int divideU[sets] = {2 , 4, 1, 2};
  int divideL[sets] = {2 , 2, 2, 2};
  TVirtualPad * gPadSave;

    int ii   = init[Set];
    gPadSave = gPad;
    gPad->Divide(divideU[Set],divideL[Set]);
      for (int i=ii;i<init[Set+1];i++){
	gPad->cd(i-ii+1);
	gPad->SetLogx(gAMSDisplay->IsLogX());
	gPad->SetLogy(gAMSDisplay->IsLogY());
	gPad->SetLogz(gAMSDisplay->IsLogZ());
        if (_fetched2[i]) {
          _fetched2[i]->Draw();
        }
	gPadSave->cd();
      }
}

void AMSGenHist::Fill(AMSNtuple * ntuple){
 
 _Fetch();
 
}





