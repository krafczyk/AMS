//  $Id: AMSLVL1Hist.cxx,v 1.3 2001/01/22 17:32:52 choutko Exp $
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
  _m2filled=8+4+2+5;
  _filled2= new TH1*[_m2filled];
  for(int mf=0;mf<_m2filled;mf++)_filled2[mf]=0;
  int i=0;
  _filled2[i++]=new TH1F("LVL1-1OR1"," Lvl1-TOF OR/AND Pattern Plane=1",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1OR2"," Lvl1-TOF OR/AND Pattern Plane=2",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1OR3"," Lvl1-TOF OR/AND Pattern Plane=3",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1OR4"," Lvl1-TOF Or/AND Pattern Plane=4",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1AND1","Lvl1-TOF AND  Pattern Plane=1",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1AND2","Lvl1-TOF AND  Pattern Plane=2",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1AND3","Lvl1-TOF AND  Pattern Plane=3",14,0.5,14.5);
  _filled2[i++]=new TH1F("LVL1-1AND4","Lvl1-TOF AND  Pattern Plane=4",14,0.5,14.5);
  char title[80];
  char name[80];
  for(i=8;i<12;i++){
    sprintf(name,"LVL1-TOFAND%d",i+1-8);
    sprintf(title,"LVL1-TOF vs TOF Plane %d",i+1-8);
    _filled2[i]=new TH2F(name,title,14,0.5,14.5,14,0.5,14.5);
    _filled2[i]->SetXTitle("TOF Paddle No");
    _filled2[i]->SetYTitle("LVL1 Paddle No");
  }
  i=12;
  _filled2[i]=new TH1F("NumberOfHits"," Number OF LVL1 AND Hits",10,-0.5,9.5);
  _filled2[i]->SetXTitle("Number of LVL1 Hits");
  _filled2[i++]->SetFillColor(28);
  _filled2[i]=new TH2F("TriggerPattern","Trigger-TOF Layer1 vs Layer4",14,0.5,14.5,14,0.5,14.5);
  _filled2[i]->SetXTitle("LVL1-TOF Layer 1 Paddle No");
  _filled2[i++]->SetYTitle("LVL1-TOF Layer 4 Paddle No");

  _filled2[i]=new TH1F("LVL1ANTIMXO","Lvl1-anti OR/AND -X",4,-0.5,3.5);
  _filled2[i]->SetXTitle("Number Of Sectors Fired");
  _filled2[i++]->SetFillColor(2);
  _filled2[i]=new TH1F("LVL1ANTIPXO","Lvl1-anti OR/AND +X",4,-0.5,3.5);
  _filled2[i]->SetXTitle("Number Of Sectors Fired");
  _filled2[i++]->SetFillColor(2);
  _filled2[i]=new TH1F("LVL1ANTIMXA","Lvl1-anti AND -X",4,-0.5,3.5);
  _filled2[i]->SetXTitle("Number Of Sectors Fired");
  _filled2[i++]->SetFillColor(8);
  _filled2[i]=new TH1F("LVL1ANTIPXA","Lvl1-anti AND +X",4,-0.5,3.5);
  _filled2[i]->SetXTitle("Number Of Sectors Fired");
  _filled2[i++]->SetFillColor(8);
  _filled2[i]=new TH2F("LVL1VSANTIZ","LVL1-Anti vs Anti",128,0.5,256.5,128,0.5,256.5);  
  _filled2[i]->SetXTitle("Lvl1 Bit Pattern");
  _filled2[i]->SetYTitle("Anti Bit Pattern");
  _filled2[i++]->SetFillColor(30);
 
}





void AMSLVL1Hist::_Fetch(){

}


void AMSLVL1Hist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  switch (Set){
case 0:
  gPad->Divide(2,2);
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
    gPad->Divide(2,2);
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      if(_filled2[i+8]){
	_filled2[i+8]->Draw("LEGO");
      }
      gPadSave->cd();
    }
    break;
  case 2:
    gPad->Divide(1,2);
    gPad->cd(1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(1);
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled2[12]->Draw();
    gPadSave->cd();
    gPad->cd(2);
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled2[13]->Draw("SURF2");
    break;
  case 3:
    gPad->Divide(1,2);
    gPad->cd(1);
    TVirtualPad * gPad2 = gPad;
    gPad->Divide(2,1);
    for(i=0;i<2;i++){
     gPad->cd(i+1);
     gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(1);
      gPad->SetLogz(gAMSDisplay->IsLogZ());
     _filled2[14+i]->Draw();
     _filled2[16+i]->Draw("SAME");
     gPad2->cd();
    }
    gPadSave->cd();
    gPad->cd(2);
    gPad->SetLogz(1);
    _filled2[18]->Draw("SURF2");
    gPadSave->cd();
    break;
  }
}

void AMSLVL1Hist::Fill(AMSNtuple * ntuple){
  int nhit=0;
  for(int i=0;i<4;i++){
    for(int k=0;k<14;k++){
      if(ntuple->_LVL1.TOF[i] & (1<<k))((TH1F*)_filled2[i])->Fill(float(14- k),1.);
      if(ntuple->_LVL1.TOF[i] & (1<<(16+k))){
	((TH1F*)_filled2[i+4])->Fill(float(14-k),1.);
	nhit++;
      }
    }
  }
  _filled2[12]->Fill(nhit,1.);

  for(i=0;i<ntuple->_TOF.NToF;i++){
    if((ntuple->_TOF.Status[i] & 256)==0){
      for(int k=0;k<14;k++){
	if(ntuple->_LVL1.TOF[ntuple->_TOF.Plane[i]-1] & (1<<(16+k)))
	  _filled2[ntuple->_TOF.Plane[i]+7]->Fill((15-ntuple->_TOF.Bar[i]),float(14-k),1.);
      }
      
    }
  }
  for(i=0;i<13;i++){
    for(int k=0;k<13;k++){
      if((ntuple->_LVL1.TOF[0] & (1<<(16+i))) && (ntuple->_LVL1.TOF[3] & (1<<(16+k)))){
	_filled2[13]->Fill(i,k,1.);
      }
    }
  }

  int sim[4]={0,0,0,0};
  int ks;
  for(i=0;i<ntuple->_ANTI.nantiraw;i++){
   if(ntuple->_ANTI.AntirawSector[i] >=5 && ntuple->_ANTI.AntirawSector[i]<=12)ks=0;
   else ks=1;
   sim[1-ntuple->_ANTI.Antirawupdown[i]+2*ks]++;
  }
  for(i=0;i<4;i++){
   if(sim[i]>3)sim[i]=3;
  }
  int off= sim[0] | sim[1]<<2 | sim[2]<<4 | sim[3]<<6;
  //  cout <<off<<" "<<(ntuple->_LVL1.Anti>>16)<<endl;
  _filled2[18]->Fill((ntuple->_LVL1.Anti>>16),off,1.);

   unsigned char pat= ntuple->_LVL1.Anti>>16;
   int minusxor= (pat&3) | ((pat>>2) &3);
   int minusxand= (pat&3) & ((pat>>2) &3);
   int plusxor= ((pat>>4)&3) | ((pat>>6) &3);
   int plusxand=((pat>>4)&3) & ((pat>>6) &3);

   _filled2[14]->Fill(minusxor,1.);
   _filled2[15]->Fill(plusxor,1.);
   _filled2[16]->Fill(minusxand,1.);
   _filled2[17]->Fill(plusxand,1.);


}





