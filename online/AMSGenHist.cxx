//  $Id: AMSGenHist.cxx,v 1.10 2011/05/24 13:14:46 choutko Exp $
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSGenHist.h"



void AMSGenHist::Book(){ 
  AddSet("EventLengths");
  _filled.push_back(new TH1F("tofl","TOF Length bytes",200,0.,2000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("trkl","TRK Length bytes",500,0.,25000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("trdl","TRD Length bytes",200,0.,2000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("richl","RICH Length bytes",200,0.,4000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("ecall","ECAL Length bytes",200,0.,4000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("lvl1l","LVL1 Length bytes",200,0.,2000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("lvl3l","LVL3 Length bytes",200,0.,2000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");
  _filled.push_back(new TH1F("total","Event Length bytes",400,0.,40000.));
_filled[_filled.size()-1]->SetXTitle("Length (Bytes)");

  AddSet("EventLengthsCorrelations");
  _filled.push_back(new TProfile("toflvsd","TOFLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  _filled.push_back(new TProfile("trklvsd","TRKLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  _filled.push_back(new TProfile("trdlvsd","TRDLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  _filled.push_back(new TProfile("richlvsd","RICHLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  _filled.push_back(new TProfile("ecallvsd","ECALLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  _filled.push_back(new TProfile("totallvsd","TotalLength vs TimeD msec",1000,0.001,25.001));
_filled[_filled.size()-1]->SetXTitle("Time Difference(msec)");
_filled[_filled.size()-1]->SetYTitle("Length (bytes)");
  AddSet("Misc Info");
  _filled.push_back(new TH1F("ThetaM","ThetaM",400,-3.1415926/2,3.1415926/2));
_filled[_filled.size()-1]->SetXTitle("ThetaM (Rad)");

}



void AMSGenHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
switch(Set){
case 0:
gPad->Divide(4,2);
for(int i=0;i<8;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[i]->Draw();
 gPadSave->cd();
}
break;
case 1:
gPad->Divide(3,2);
for(int i=0;i<6;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[i+8]->Draw();
 gPadSave->cd();
}
break;
case 2:
gPad->Divide(1,1);
 gPad->cd(1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled[8+6]->Draw();
break;
}
}



void AMSGenHist::Fill(AMSNtupleR *ntuple){ 
  static int evno=0;
  if(ntuple->nDaqEvent()){
    _filled[0]->Fill(ntuple->pDaqEvent(0)->Sdr,1);
    _filled[1]->Fill(ntuple->pDaqEvent(0)->Tdr,1);
    _filled[2]->Fill(ntuple->pDaqEvent(0)->Udr,1);
    _filled[3]->Fill(ntuple->pDaqEvent(0)->Rdr,1);
    _filled[4]->Fill(ntuple->pDaqEvent(0)->Edr,1);
    _filled[5]->Fill(ntuple->pDaqEvent(0)->L1dr,1);
    _filled[6]->Fill(ntuple->pDaqEvent(0)->L3dr,1);
    _filled[7]->Fill(ntuple->pDaqEvent(0)->Length,1);
    if(ntuple->nLevel1() && (1 || ntuple->Event()-evno==1)){
      float xtime=ntuple->pLevel1(0)->TrigTime[4]/1000.;
//      cout << " xtime "<<xtime<<endl;
      ((TProfile*)_filled[8])->Fill(xtime,ntuple->pDaqEvent(0)->Sdr);  
      ((TProfile*)_filled[9])->Fill(xtime,ntuple->pDaqEvent(0)->Tdr);  
      ((TProfile*)_filled[10])->Fill(xtime,ntuple->pDaqEvent(0)->Udr);  
      ((TProfile*)_filled[11])->Fill(xtime,ntuple->pDaqEvent(0)->Rdr);  
      ((TProfile*)_filled[12])->Fill(xtime,ntuple->pDaqEvent(0)->Edr);  
      ((TProfile*)_filled[13])->Fill(xtime,ntuple->pDaqEvent(0)->Length);  
    }
   }
   //cout <<ntuple->fHeader.ThetaM<<endl;
   _filled[14]->Fill(ntuple->fHeader.ThetaM,1.);
   evno=ntuple->Event();
     Float_t xm=0;
//    if(ntuple->nMCEventg()>0){		
//     MCEventgR mc_ev=ntuple->MCEventg(0);
//      xm = log(mc_ev.Momentum);
//     } 
}


