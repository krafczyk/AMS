#include <iostream.h>
#include "AMSTrackerHist.h"
#include "AMSDisplay.h"
#include "AMSNtuple.h"
#include <TProfile.h>
AMSTrackerHist::AMSTrackerHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSTrackerHist::_Fill(){
 _m2filled=16+8+4+2;
 _filled2= new TH1*[_m2filled];
 for(int mf=0;mf<_m2filled;mf++)_filled2[mf]=0;
 int crate[2]={32,72};
 char scrate[3]="KS";
 int i=0;
 int k=0;
 char name[80];
 char title[80];
 for(i=0;i<2;i++){
  sprintf(name,"N%c",scrate[i]); 
  sprintf(title ,"Number of Preclusters Side %c",scrate[i]); 
  _filled2[i*2]=new TH1F(name,title,200,-0.5,199.5);
  _filled2[i*2]->SetFillColor(18);
  sprintf(name,"Length%c",scrate[i]); 
  sprintf(title ,"Length (Bytes) Side %c ",scrate[i]); 
  _filled2[i*2+1]=new TH1F(name,title,200,0.,2000.);
  _filled2[i*2+1]->SetFillColor(19);
 }
  sprintf(name,"NumClPerL32K"); 
  sprintf(title ,"Number of K Clusters per Ladder Crate 32"); 
  _filled2[4]=new TProfile(name,title,100,0.5,100.5);
  sprintf(name,"NumClPerL32S"); 
  sprintf(title ,"Number of S Clusters per Ladder Crate 32"); 
  _filled2[5]=new TProfile(name,title,100,0.5,100.5);
  sprintf(name,"NumClPerL72K"); 
  sprintf(title ,"Number of K Clusters per Ladder Crate 72"); 
  _filled2[6]=new TProfile(name,title,100,0.5,100.5);
  sprintf(name,"NumClPerL72S"); 
  sprintf(title ,"Number of S Clusters per Ladder Crate 72"); 
  _filled2[7]=new TProfile(name,title,100,0.5,100.5);
  for(i=4;i<8;i+=2){
   _filled2[i]->SetMarkerStyle(20);
   _filled2[i]->SetMarkerSize(0.7);
   _filled2[i+1]->SetMarkerStyle(21);
   _filled2[i+1]->SetMarkerSize(0.7);
  }
  sprintf(name,"xclampl");
  sprintf(title ,"x Clusters Amplitudes Distribution"); 
  _filled2[8]=new TH1F(name,title,200,-0.5, 199.5);
  _filled2[8]->SetFillColor(20);
  sprintf(name,"yclampl");
  sprintf(title ,"y Clusters Amplitudes Distribution"); 
  _filled2[9]=new TH1F(name,title,200,-0.5, 199.5);
  _filled2[9]->SetFillColor(21);
  sprintf(name,"xclnstr");
  sprintf(title ,"Number of Strips for x Clusters "); 
  _filled2[10]=new TH1F(name,title,6,-0.5 ,5.5);
  _filled2[10]->SetFillColor(22);
  sprintf(name,"yclnstr");
  sprintf(title ,"Number of Strips for y Clusters "); 
  _filled2[11]=new TH1F(name,title,6,-0.5 ,5.5);
  _filled2[11]->SetFillColor(22);

   float bin=8./64.;
  _filled2[28]=new TH1F("s2nK", "Seed signal/noise K side",64,-bin/2,8.-bin/2);
  _filled2[28]->SetXTitle("S/N");
  _filled2[28]->SetFillColor(23);
  _filled2[29]=new TH1F("s2nS", "Seed signal/noise S side",64,-bin/2,8.-bin/2);
  _filled2[29]->SetXTitle("S/N");
  _filled2[29]->SetFillColor(23);
}



void AMSTrackerHist::_Fetch(){
 _m2fetched=4+4;
 _fetched2= new TH1*[_m2fetched];
 _fetched2[0]=(TH1*)gAMSDisplay->GetRootFile()->Get("h500001"); 
 _fetched2[1]=(TH1*)gAMSDisplay->GetRootFile()->Get("h500002"); 
 _fetched2[2]=(TH1*)gAMSDisplay->GetRootFile()->Get("h500011"); 
 _fetched2[3]=(TH1*)gAMSDisplay->GetRootFile()->Get("h500012"); 
 for(int i=0;i<4;i++){
 if(_fetched2[i]){
  _fetched2[i]->SetXTitle("ADC ch");
  _fetched2[i]->SetFillColor(25);              
 }
 }
 _fetched2[4]=(TH1*)gAMSDisplay->GetRootFile()->Get("h600001"); 
 _fetched2[5]=(TH1*)gAMSDisplay->GetRootFile()->Get("h600002"); 
 _fetched2[6]=(TH1*)gAMSDisplay->GetRootFile()->Get("h600011"); 
 _fetched2[7]=(TH1*)gAMSDisplay->GetRootFile()->Get("h600012"); 
 for(i=4;i<8;i++){
 if(_fetched2[i]){
  _fetched2[i]->SetXTitle("TDR No");
  _fetched2[i]->SetFillColor(26);              
 }
 }

}


void AMSTrackerHist::ShowSet(Int_t Set){


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
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[4+i]->Draw();
gPadSave->cd();
}
break;
case 2:
gPad->Divide(2,1);
for(i=0;i<2;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 _filled2[28+i]->Draw();
gPadSave->cd();
}
break;
case 3:
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
case 4:
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(1);
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 if(_fetched2[i]){
  _fetched2[i]->Draw();
 }
gPadSave->cd();
}
break;
case 5:
gPad->Divide(2,2);
for(i=0;i<4;i++){
 gPad->cd(i+1);
 gPad->SetLogx(gAMSDisplay->IsLogX());
 gPad->SetLogy(gAMSDisplay->IsLogY());
 gPad->SetLogz(gAMSDisplay->IsLogZ());
 if(_fetched2[i+4]){
  _fetched2[i+4]->Draw();
 }
gPadSave->cd();
}

}



}


void AMSTrackerHist::Fill(AMSNtuple * ntuple){
    int n[2][2]={0,0,0,0};
   int l[2][2]={0,0,0,0};
   int addr[2][2][70];
    int ij;
   for(int lk=0;lk<2;lk++){
    for(int lkk=0;lkk<2;lkk++){
     for(int ij=0;ij<70;ij++)addr[lk][lkk][ij]=0;
    }
   }
  for(int i=0;i<ntuple->_Tracker.ntrraw;i++){
   int lay=   ntuple->_Tracker.rawaddress[i]%10-1;
   int lad=   (ntuple->_Tracker.rawaddress[i]/10)%100;
   int ir=(ntuple->_Tracker.rawaddress[i]/1000)%10;
   int side= ir<2?0:1;
   int crate= (ir==0 || ir==2)?0:1;
   int iaddr=lay*10+lad;
   _filled2[28+side]->Fill(ntuple->_Tracker.s2n[i]); 
    addr[crate][side][iaddr]++;
    //   _filled2[4+crate]->Fill(iaddr,1.,1.);
   n[side][crate]++;
   l[side][crate]+=(ntuple->_Tracker.rawlength[i]+1)*2;
   if(side==1)l[side][crate]+=2;
  }
   for(ij=1;ij<70;ij++)_filled2[4]->Fill(ij,addr[0][0][ij],1.);
   for(ij=1;ij<70;ij++)_filled2[5]->Fill(ij,addr[0][1][ij],1.);
   for(ij=1;ij<70;ij++)_filled2[6]->Fill(ij,addr[1][0][ij],1.);
   for(ij=1;ij<70;ij++)_filled2[7]->Fill(ij,addr[1][1][ij],1.);
  //  cout <<n[0][0]+n[0][1]<<" "<<n[1][0]+n[1][1]<<endl;
  _filled2[0]->Fill(n[0][0]+n[0][1],1.);
  _filled2[2]->Fill(n[1][0]+n[1][1],1.);
  _filled2[1]->Fill(l[0][0]+l[0][1],1.);
  _filled2[3]->Fill(l[1][0]+l[1][1],1.);
  int nc[2]={0,0};
  for(i=0;i<ntuple->_Tracker.ntrcl;i++){
   int lay=   ntuple->_Tracker.idsoft[i]%10-1;
   int lad=   (ntuple->_Tracker.idsoft[i]/10)%100;
   int ir=(ntuple->_Tracker.idsoft[i]/1000)%10;
   int side= ir<2?0:1;
   int crate= (ir==0 || ir==2)?0:1;
   _filled2[8+side]->Fill(ntuple->_Tracker.sumt[i],1.);
   _filled2[10+side]->Fill(ntuple->_Tracker.nelemr[i]-ntuple->_Tracker.neleml[i],1.);
   nc[side]++;
  }
}
