#include <iostream.h>
#include "AMSAxAMSHist.h"
#include "AMSNtuple.h"
#include "AMSDisplay.h"
#include <math.h>
#include <TPaveText.h>
#include <TProfile.h>
AMSAxAMSHist::AMSAxAMSHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSAxAMSHist::_Fill(){
_m2filled=5+1;
_filled2= new TH1*[_m2filled];
int i=0;
int color=18;
_filled2[i]=new TH1F("Momentum","Momentum",200,-20.,20.);
_filled2[i]->SetXTitle("Momentum (GeV)");
_filled2[i++]->SetFillColor(color++);
_filled2[i]=new TH1F("Abs(Momentum)","Abs(Momentum)",100,0.,20.);
_filled2[i]->SetXTitle("Abs(Momentum) (GeV)");
_filled2[i++]->SetFillColor(color++);
_filled2[i]=new TH1F("Charge","Charge",20,0.5,10.5);
_filled2[i]->SetXTitle("Charge");
_filled2[i++]->SetFillColor(color++);
_filled2[i]=new TH1F("Velocity","Velocity",100,0.2,1.2);
_filled2[i]->SetXTitle("Velocity");
_filled2[i++]->SetFillColor(color++);
_filled2[i]=new TH1F("Mass","Mass",200,0.01,10.01);
_filled2[i]->SetXTitle("Mass");
_filled2[i++]->SetFillColor(color++);
_filled2[i]=new TH2F("Log(Momentum) vs Velocity","Momentum vs Velocity",100,-5.,5.,
100,0.6,1.1);
_filled2[i]->SetXTitle("Log(Momentum) Log(GeV)");
_filled2[i++]->SetYTitle("Velocity");

_filled2[i]=new TProfile("TrackerRes","Tracker delta_p/p vs p",100,0.,20.,0.,1000.);
_filled2[i]->SetXTitle("Momentum (GeV)");
_filled2[i]->SetYTitle("Resolution dp/p (%)");
_filled2[i++]->SetFillColor(color++);
}



void AMSAxAMSHist::_Fetch(){

}


void AMSAxAMSHist::ShowSet(Int_t Set){
   TF1 f1("myg","[0]*exp(-(x-[1])/[2]*(x-[1])/[2]/2)",0,2);
   f1.SetParameter(1,1.);
   f1.SetParameter(2,0.05);
   _filled2[3]->Fit("myg","Q");
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
 if(i==3){
    char text[80];
    TPaveText *lf=new TPaveText(0.2,0.5,0.4,0.8,"NDC");
    lf->SetFillColor(18);
    sprintf(text,"Mean %f",f1.GetParameter(1));    
    lf->AddText(text);
    sprintf(text,"Sigma %f",f1.GetParameter(2));    
    lf->AddText(text);
    lf->Draw();
 }
gPadSave->cd();
}

break;
case 1:
gPad->Divide(1,2);
 gPad->cd(1);
TVirtualPad * gp1=gPad; 
 gPad->Divide(2,1);
 gPad->cd(1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[4]->Draw();
 gp1->cd();
 gPad->cd(2);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[6]->Draw();
 gPadSave->cd();
 gPad->cd(2);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[5]->Draw();
}


}

void AMSAxAMSHist::Fill(AMSNtuple * ntuple){
  if(ntuple->_AxAMS.npart){
    _filled2[0]->Fill(ntuple->_AxAMS.pmom[0],1.);
    _filled2[1]->Fill(fabs(ntuple->_AxAMS.pmom[0]),1.);
    _filled2[2]->Fill(ntuple->_AxAMS.pcharge[0],1.);
    _filled2[3]->Fill(ntuple->_AxAMS.beta[ntuple->_AxAMS.pbetap[0]-1],1.);
    _filled2[4]->Fill(ntuple->_AxAMS.pmass[0],1.);
    _filled2[5]->Fill(log(fabs(ntuple->_AxAMS.pmom[0])),ntuple->_AxAMS.beta[ntuple->_AxAMS.pbetap[0]-1],1.);
    float err=ntuple->_AxAMS.perrmom[0];
    if(err<100000){
      err=err*100;
     if (fabs(ntuple->_AxAMS.pmom[0]) !=0)err=err/fabs(ntuple->_AxAMS.pmom[0]);
    }
    _filled2[6]->Fill(fabs(ntuple->_AxAMS.pmom[0]),err,1.);
  }
}
