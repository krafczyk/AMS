// By V. Choutko & D. Casadei
// Last changes: 27 Feb 1998 by D.C.
#include <iostream.h>
#include "AMSTOFHist.h"
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>

ClassImp(AMSTOFHist)

AMSTOFHist::AMSTOFHist(Text_t * name, Text_t * title, Int_t maxset, Int_t active):
  AMSHist(name, title, maxset,active){
    _Fetch();
    _Fill();    
}

void AMSTOFHist::_Fill(){ // V. Choutko 25 Feb 1998
  _m2filled=8+4+2;
  _filled2= new TH1*[_m2filled];
for(int mf=0;mf<_m2filled;mf++)_filled2[mf]=0;
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
  _filled2[i]=new TH1D("NToF","Number Of TOF Clusters",16,-0.5,15.5);
  _filled2[i]->SetXTitle(" Number of TOF clusters");
  _filled2[i++]->SetFillColor(28);
  _filled2[i]=new TH1D("Ampl","TOF Energy Loss Distribution",100,0.,20.);
  _filled2[i]->SetXTitle(" TOF Energy Loss (MeV)");
  _filled2[i++]->SetFillColor(29);
  
}

//------------------------------------------------------------------

void AMSTOFHist::_Fetch(){ // D. Casadei 25 Feb 1998
  char text[80],name[80],title[80];
  const int step=30;
  
  _fetched2=new TH1*[54]; 
  // Casadei
  for(int diego=0;diego<8;diego++){
    if(diego%2==0){
      sprintf(name,"Anode %1d-P",(diego/2+1));
      sprintf(title,"Anode charge %1d-P (pC)",(diego/2+1));
    }
    else{
      sprintf(name,"Anode %1d-N",(diego/2+1));
      sprintf(title,"Anode charge %1d-N (pC)",(diego/2+1));
    }
    _fetched2[30+diego]=new TH1F(name,title,14,0.5,14.5);
//     _fetched2[30+diego]->SetFillColor(19);
    _fetched2[30+diego]->SetLabelFont(40,"X");
    _fetched2[30+diego]->SetLabelFont(40,"Y");
    _fetched2[30+diego]->SetLabelSize(0.08,"X");
    _fetched2[30+diego]->SetLabelSize(0.08,"Y");
    _fetched2[30+diego]->SetLineColor(4);
    _fetched2[30+diego]->SetMarkerStyle(8);
    _fetched2[30+diego]->SetMarkerColor(4);
    //
    if(diego%2==0){
      sprintf(name,"Dynode %1d-P",(diego/2+1));
      sprintf(title,"Dynode charge %1d-P (pC)",(diego/2+1));
    }
    else{
      sprintf(name,"Dynode %1d-N",(diego/2+1));
      sprintf(title,"Dynode charge %1d-N (pC)",(diego/2+1));
    }
    _fetched2[38+diego]=new TH1F(name,title,14,0.5,14.5);
//     _fetched2[38+diego]->SetFillColor(19);
    _fetched2[38+diego]->SetLabelFont(40,"X");
    _fetched2[38+diego]->SetLabelFont(40,"Y");
    _fetched2[38+diego]->SetLabelSize(0.08,"X");
    _fetched2[38+diego]->SetLabelSize(0.08,"Y");
    _fetched2[38+diego]->SetLineColor(4);
    _fetched2[38+diego]->SetMarkerStyle(8);
    _fetched2[38+diego]->SetMarkerColor(4);
    //
    if(diego%2==0){
      sprintf(name,"Stretch %1d-P",(diego/2+1));
      sprintf(title,"Time Expansion %1d-P (ns)",(diego/2+1));
    }
    else{
      sprintf(name,"Stretch %1d-N",(diego/2+1));
      sprintf(title,"Time Expansion %1d-N (ns)",(diego/2+1));
    }
    _fetched2[46+diego]=new TH1F(name,title,14,0.5,14.5);
//     _fetched2[46+diego]->SetFillColor(19);
    _fetched2[46+diego]->SetLabelFont(40,"X");
    _fetched2[46+diego]->SetLabelFont(40,"Y");
    _fetched2[46+diego]->SetLabelSize(0.08,"X");
    _fetched2[46+diego]->SetLabelSize(0.08,"Y");
    _fetched2[46+diego]->SetLineColor(4);
    _fetched2[46+diego]->SetMarkerStyle(8);
    _fetched2[46+diego]->SetMarkerColor(4);
  }

  // Choumilov added:
  _fetched2[0]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1544"); // time diff of diffs
  _fetched2[1]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1532"); // time diff 13
  _fetched2[2]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1534"); // time diff 24
  _fetched2[3]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1107"); // little crates size
  _fetched2[4]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1111"); // layers multipl.
  _fetched2[5]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1114"); // "configuration"
                                                        // (ask Choumilov...)
  for(int kk=0;kk<6;kk++)
    if(!_fetched2[kk])cerr<<"AMSTOFHist::_Fetch-E-NoHisFound -- kk="<<kk<<endl;

  int ind=6;

  for(int i=5000;i<5012;i++){ // JL1 hits : index=6..17
    sprintf(text,"h%d",i);
    _fetched2[ind]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
    if(_fetched2[ind]){
    }
    else{
      cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
    }    
    ind++;
  }
  for(i=5015;i<5027;i++){ // hits from data : index=18..29
    sprintf(text,"h%d",i);
    _fetched2[ind]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
    if(_fetched2[ind]){
    }
    else{
      cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
    }    
    ind++;
  }
  TH1*dummy=new TH1();
  dummy=(TH1*)gAMSDisplay->GetRootFile()->Get("h5013");
  if(dummy){
    _norm=dummy->GetEntries();
  }
  else{
    cout<<"I will not normalize histograms!"<<endl;
    _norm=1;
  }
  if(_norm==0)_norm=1;
  for(i=0;i<30;i++)
    if(_fetched2[i])
      _fetched2[i]->Scale(1./_norm);
  delete dummy;

//   int start=5030;  // his edges
//   for(int k=0;k<4;k++){
//     for(i=start;i<start+28;i++){
//       sprintf(text,"h%d",i);
//       _fetched2[i-5000]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
//       if(_fetched2[i-5000]){
//       }
//       else{
// 	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
//       }    
//     }
//     start+=step;
//   }

//   start=5150;  // tdc edges
//   for(int k=0;k<4;k++){
//     for(i=start;i<start+28;i++){
//       sprintf(text,"h%d",i);
//       _fetched2[i-5000]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
//       if(_fetched2[i-5000]){
//       }
//       else{
// 	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
//       }    
//     }
//     start+=step;
//   }

//   start=5270;  // ano edges
//   for(int k=0;k<4;k++){
//     for(i=start;i<start+28;i++){
//       sprintf(text,"h%d",i);
//       _fetched2[i-5000]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
//       if(_fetched2[i-5000]){
//       }
//       else{
// 	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
//       }    
//     }
//     start+=step;
//   }

//   start=5490;  // dyn edges
//   for(int k=0;k<4;k++){
//     for(i=start;i<start+28;i++){
//       sprintf(text,"h%d",i);
//       _fetched2[i-5000]=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
//       if(_fetched2[i-5000]){
//       }
//       else{
// 	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
//       }    
//     }
//     start+=step;
//   }

  int index=0,ntries=1;
  Axis_t _anode[112],_dynode[112],_stretch[112];
  Stat_t _anorms[112],_dynrms[112],_strrms[112];
  TH1* temp;

  int start=5700;  // anode charge
  for(int k=0;k<4;k++){
    for(i=start;i<start+28;i++){
      sprintf(text,"h%d",i);
      temp=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(temp)
	ntries=temp->GetEntries();
      if(ntries==0)ntries=1;
      if(temp){
	_anode[index]=temp->GetMean();
	_anorms[index]=(temp->GetRMS())/sqrt(ntries);
      }
      else{
	_anode[index]=0;
	_anorms[index]=0;
	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
      }    
      index++;
    }
    start+=step;
  }
  index=0;
  start=5830;  // dynode charge
  for(k=0;k<4;k++){
    for(i=start;i<start+28;i++){
      sprintf(text,"h%d",i);
      temp=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(temp)
	ntries=temp->GetEntries();
      if(ntries==0)ntries=1;
      if(temp){
	_dynode[index]=temp->GetMean();
	_dynrms[index]=(temp->GetRMS())/sqrt(ntries);
      }
      else{
	_dynode[index]=0;
	_dynrms[index]=0;
	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
      }    
      index++;
    }
    start+=step;
  }
  index=0;
  start=6000;  // Stretching ratios
  for(k=0;k<4;k++){
    for(i=start;i<start+28;i++){
      sprintf(text,"h%d",i);
      temp=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(temp)
	ntries=temp->GetEntries();
      if(ntries==0)ntries=1;
      if(temp){
	_stretch[index]=temp->GetMean();
	_strrms[index]=(temp->GetRMS())/sqrt(ntries);
      }
      else{
	_stretch[index]=0;
	_strrms[index]=0;
	cerr<<"AMSTOFHist::_Fetch-E-NoHisFound "<<text<<endl;
      }    
      index++;
    }
    start+=step;
  }
  // I go through these vectors for historical reasons...
  Stat_t bin=0;
  for(i=0;i<8;i++){
    for(int j=0; j<14; j++){
      if(_fetched2[30+i]){
	bin=_anode[j+14*i];
	_fetched2[30+i]->SetBinContent(j+1,bin);
	bin=_anorms[j+14*i];
	_fetched2[30+i]->SetBinError(j+1,bin);
      }
      if(_fetched2[38+i]){
	bin=_dynode[j+14*i];
	_fetched2[38+i]->SetBinContent(j+1,bin);
	bin=_dynrms[j+14*i];
	_fetched2[38+i]->SetBinError(j+1,bin);
      }
      if(_fetched2[46+i]){
	bin=_stretch[j+14*i];
	_fetched2[46+i]->SetBinContent(j+1,bin);
	bin=_strrms[j+14*i];
	_fetched2[46+i]->SetBinError(j+1,bin);
      }
    }
  }
  for(i=30;i<54;i++){
    if(_fetched2[i]){
//     _fetched2[i]->SetFillColor(19);
      _fetched2[i]->SetLabelFont(40,"X");
      _fetched2[i]->SetLabelFont(40,"Y");
      _fetched2[i]->SetLabelSize(0.08,"X");
      _fetched2[i]->SetLabelSize(0.08,"Y");
      _fetched2[i]->SetLineColor(4);
      _fetched2[i]->SetMarkerStyle(8);
      _fetched2[i]->SetMarkerColor(4);
    }
  }
}
//------------------------------

void AMSTOFHist::ShowSet(Int_t Set){// D. Casadei 26 Feb 1998
  int i;
  gPad->Clear();
  char text[80];

  Float_t cpos[14]={1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.};
  Float_t epos[14]={.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01};
  Double_t weight[14]={1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};

  Int_t stat;
  stat=gStyle->GetOptStat();
  gStyle->SetOptStat(0);


// First three windows by Choutko
  if(Set==0){//Sides hits histograms
    Float_t y[14],ey[14];
    gPad->Divide(2,4,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if(((i%2)==0)&&_fetched2[6+i/2]&&_fetched2[18+i/2]){
	sprintf(text,"Plane %1d Side P hits",(i/2+1));
	_fetched2[6+i/2]->SetLabelSize(0.08,"X"); // JL1 
	_fetched2[6+i/2]->SetLabelSize(0.08,"Y"); // JL1 
	_fetched2[6+i/2]->SetLineColor(8); // green
	_fetched2[6+i/2]->SetLabelFont(40,"X");
	_fetched2[6+i/2]->SetLabelFont(40,"Y");
	_fetched2[6+i/2]->Draw(); // JL1 
	((TH1*)_fetched2[6+i/2])->SetTitle();
	_fetched2[18+i/2]->SetLineColor(4); // blue
	_fetched2[18+i/2]->Draw("same"); // data
      }
      else if(_fetched2[10+i/2]&&_fetched2[22+i/2]){
	sprintf(text,"Plane %1d Side N hits",(i/2+1));
	_fetched2[10+i/2]->SetLabelSize(0.08,"X"); // JL1 
	_fetched2[10+i/2]->SetLabelSize(0.08,"Y"); // JL1 
	_fetched2[10+i/2]->SetLabelFont(40,"X");
	_fetched2[10+i/2]->SetLabelFont(40,"Y");
	_fetched2[10+i/2]->Draw(); // JL1 
	_fetched2[10+i/2]->SetLineColor(8); // JL1 
	((TH1*)_fetched2[10+i/2])->SetTitle("");
	_fetched2[22+i/2]->SetLineColor(4); // data
	_fetched2[22+i/2]->Draw("same"); // data
      }
      TPaveText* box1 = new TPaveText(.43,.14,.57,.44,"NDC");
      TText *tJL1, *tTOF;
      tJL1=box1->AddText("JL1");
      tTOF=box1->AddText("TOF");
      tJL1->SetTextColor(8);
      tTOF->SetTextColor(4);
//       box1->SetFillColor(42);
      box1->Draw();
      TPaveText* box = new TPaveText(.25,.9,.75,1.,"NDC");
      box->AddText(text);
      box->Draw();
      gPad->Update();
      gPadSave->cd();
    }
  }
  else if(Set==1){//AND hits
    gPad->Divide(2,2,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      if(_fetched2[14+i]){
	sprintf(text,"Plane %1d. AND of sides",(i+1));
	((TH1*)_fetched2[14+i])->SetTitle();
	((TH1*)_fetched2[14+i])->SetXTitle("Counter Number");
	_fetched2[14+i]->SetLineColor(8);
	_fetched2[14+i]->SetLabelFont(40,"X");
	_fetched2[14+i]->SetLabelFont(40,"Y");
	_fetched2[14+i]->Draw(); // JL1 
	_fetched2[26+i]->SetLineColor(4); // data
	_fetched2[26+i]->Draw("same");
	TPaveText* box1 = new TPaveText(.42,.14,.58,.3,"NDC");
	TText *tJL1, *tTOF;
	tJL1=box1->AddText("JL1");
	tTOF=box1->AddText("TOF");
	tJL1->SetTextColor(8);
	tTOF->SetTextColor(4);
	//       box1->SetFillColor(43);
	box1->Draw();
	TPaveText* box = new TPaveText(.25,.93,.75,1.,"NDC");
	box->AddText(text);
	box->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==2){ // JL1 OR, AND of sides
    gPad->Divide(2,2,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      if(_filled2[i]){
	gPad->SetLogx(gAMSDisplay->IsLogX());
	gPad->SetLogy(gAMSDisplay->IsLogY());
	gPad->SetLogz(gAMSDisplay->IsLogZ());
	((TH1*)_filled2[i])->SetTitle();
	((TH1*)_filled2[i])->SetXTitle("Counter Number");
	_filled2[i]->SetLabelFont(40,"X");
	_filled2[i]->SetLabelFont(40,"Y");
	_filled2[i]->SetFillColor(3);
	_filled2[i]->Draw();
	_filled2[i+4]->SetFillColor(4);
	_filled2[i+4]->Draw("SAME");
	TPaveText* box = new TPaveText(.43,.14,.57,.3,"NDC");
	TText *tOR, *tAND;
	tOR=box->AddText("OR");
	tAND=box->AddText("AND");
	tOR->SetTextColor(3);
	tAND->SetTextColor(4);
//       box->SetFillColor(43);
	box->Draw();
	TPaveText* title = new TPaveText(.3,.93,.7,1.,"NDC");
	sprintf(text,"Plane %1d hits",(i+1));
	title->AddText(text);
	title->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==3){//Choumilov hists
    gPad->Divide(2,3,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    Float_t x1, x2;
    for(i=0;i<6;i++){
      gPad->cd(i+1);
      if(_fetched2[i]){
	if(i==0){
	  sprintf(text,"TOF(1,3) - TOF(2,4) [ns]");
	  x1=.25;
	  x2=.75;
	}
	else if(i==1){
	  sprintf(text,"TOF(1,3) [ns]");
	  x1=.35;
	  x2=.65;
	}
	else if(i==2){
	  sprintf(text,"TOF(2,4) [ns]");
	  x1=.35;
	  x2=.65;	  
	}
	else if(i==3){
	  sprintf(text,"Little crates data size [words]");
	  x1=.24;
	  x2=.76;
	}
	else if(i==4){
// 	  gPad->RangeAxis(0.5,0.8,4.5,1.0);
// 	  gPad->Update();
	  sprintf(text,"Layers Frequency");
	  x1=.25;
	  x2=.75;	  
	}
	else if(i==5){
	  sprintf(text,"Configuration");
	  x1=.35;
	  x2=.65;	  
	}
	((TH1*)_fetched2[i])->SetTitle();
	((TH1*)_fetched2[i])->SetFillColor(42);
	((TH1*)_fetched2[i])->SetLabelFont(40,"X");
	((TH1*)_fetched2[i])->SetLabelFont(40,"Y");
	((TH1*)_fetched2[i])->SetLabelSize(0.08,"X");
	((TH1*)_fetched2[i])->SetLabelSize(0.08,"Y");
	((TH1*)_fetched2[i])->SetLineColor(4);
	_fetched2[i]->Draw(); 
	TPaveText* box = new TPaveText(x1,.9,x2,1.,"NDC");
	box->AddText(text);
	box->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==4){ // TOF clusters, total Eloss
    gPad->Divide(1,2,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<2;i++){
      gPad->cd(i+1);
      if(_filled2[12+i]){
	gPad->SetLogx(gAMSDisplay->IsLogX());
	gPad->SetLogy(1);
	gPad->SetLogz(gAMSDisplay->IsLogZ());
	((TH1*)_filled2[12+i])->SetTitle();
	((TH1*)_filled2[12+i])->SetFillColor(42);
	((TH1*)_filled2[12+i])->SetLineColor(4);
	_filled2[12+i]->SetLabelFont(40,"X");
	_filled2[12+i]->SetLabelFont(40,"Y");
	_filled2[12+i]->Draw();
	TPaveText* title = new TPaveText(.375,.93,.625,1.,"NDC");
	if(i==0)
	  sprintf(text,"TOF Clusters");
	else
	  sprintf(text,"TOF Energy Loss");
	title->AddText(text);
	title->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==5){ // Eloss/counter
    gPad->Divide(2,2,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      if(_filled2[8+i]){
	gPad->SetLogx(gAMSDisplay->IsLogX());
	gPad->SetLogy(gAMSDisplay->IsLogY());
	gPad->SetLogz(gAMSDisplay->IsLogZ());
	((TH1*)_filled2[8+i])->SetTitle();
	((TH1*)_filled2[8+i])->SetXTitle("Counter Number");
	((TH1*)_filled2[8+i])->SetLineColor(4);
	_filled2[8+i]->SetLabelFont(40,"X");
	_filled2[8+i]->SetLabelFont(40,"Y");
	_filled2[8+i]->Draw();
	TPaveText* title = new TPaveText(.25,.93,.75,1.,"NDC");
	sprintf(text,"Plane %1d Energy Loss",(i+1));
	title->AddText(text);
	title->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==6){//Anode charge
    Float_t y[14],ey[14];
    gPad->Divide(2,4,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if(_fetched2[30+i]){
	if((i%2)==0)
	  sprintf(text,"Anode charge %1d-P (pC)",(i/2+1));
	else
	  sprintf(text,"Anode charge %1d-N (pC)",(i/2+1));
	_fetched2[30+i]->SetTitle();
	_fetched2[30+i]->Draw("PE");
	TPaveText* box = new TPaveText(.25,.9,.75,1.,"NDC");
	box->AddText(text);
	box->Draw();
      }
      gPadSave->cd();
    }
  }
  else if(Set==7){//Dynode charge
    Float_t y[14],ey[14];
    gPad->Divide(2,4,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if(_fetched2[38+i]){
	if((i%2)==0)
	  sprintf(text,"Dynode charge %1d-P (pC)",(i/2+1));
	else
	  sprintf(text,"Dynode charge %1d-N (pC)",(i/2+1));
	_fetched2[38+i]->SetTitle();
	_fetched2[38+i]->Draw("PE");
	TPaveText* box = new TPaveText(.25,.9,.75,1.,"NDC");
	box->AddText(text);
	box->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  else if(Set==8){//Stretching Ratio
    Float_t y[14],ey[14];
    gPad->Divide(2,4,0.001,0.001,10);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if(_fetched2[46+i]){
	if((i%2)==0)
	  sprintf(text,"Time Expansion Factor: %1d-P",(i/2+1));
	else
	  sprintf(text,"Time Expansion Factor: %1d-N",(i/2+1));
	_fetched2[46+i]->SetTitle();
	_fetched2[46+i]->Draw("PE");
	TPaveText* box = new TPaveText(.2,.9,.8,1.,"NDC");
	box->AddText(text);
	box->Draw();
	gPad->Update();
      }
      gPadSave->cd();
    }
  }
  gStyle->SetOptStat(stat);
}

//------------------------------------------------------------------------------


void AMSTOFHist::Fill(AMSNtuple *ntuple){ // V. Choutko 25 Feb 1998
  int i;

  for(i=0;i<ntuple->_TOF.NToF;i++){
    _filled2[ntuple->_TOF.Plane[i]-1]->Fill((15-ntuple->_TOF.Bar[i]),1.);
    if((ntuple->_TOF.Status[i] & 256)==0)
      _filled2[ntuple->_TOF.Plane[i]+3]->Fill((15-ntuple->_TOF.Bar[i]),1.);
    _filled2[ntuple->_TOF.Plane[i]-1+8]->Fill((15-ntuple->_TOF.Bar[i]),ntuple->_TOF.Edep[i],1.);
    _filled2[13]->Fill(ntuple->_TOF.Edep[i],1.);
  }
  _filled2[12]->Fill(ntuple->_TOF.NToF,1.);
}

