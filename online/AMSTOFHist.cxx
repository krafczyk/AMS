#include <iostream.h>
#include "AMSTOFHist.h"
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>

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


//------------------------------
void AMSTOFHist::_Fetch(){
  char text[80];
  const int step=30;

  _fetched2=new TH1*[30]; 

  // Choumilov added:
  _fetched2[0]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1544"); // time diff of diffs
  _fetched2[1]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1532"); // time diff 13
  _fetched2[2]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1534"); // time diff 24
  _fetched2[3]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1107"); // little crates size
  _fetched2[4]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1111"); // layers multipl.
  _fetched2[5]=(TH1*)gAMSDisplay->GetRootFile()->Get("h1114"); // "configuration"
                                                        // (ask Choumilov...)
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


  int index=0;
  int start=5700;  // anode charge
  TH1* temp;
  for(int k=0;k<4;k++){
    for(i=start;i<start+28;i++){
      sprintf(text,"h%d",i);
      temp=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(temp){
      _anode[index]=temp->GetMean();
      _anorms[index]=temp->GetRMS();
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
      if(temp){
      _dynode[index]=temp->GetMean();
      _dynrms[index]=temp->GetRMS();
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
  start=6000;  // stretcher ratio
  for(k=0;k<4;k++){
    for(i=start;i<start+28;i++){
      sprintf(text,"h%d",i);
      temp=(TH1*)gAMSDisplay->GetRootFile()->Get(text);
      if(temp){
      _stretch[index]=temp->GetMean();
      _strrms[index]=temp->GetRMS();
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
  delete temp;

}
//------------------------------

void AMSTOFHist::ShowSet(Int_t Set){
  int i;
  gPad->Clear();
  char text[80];
  Float_t cpos[14]={1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.};
  Float_t epos[14]={.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01,.01};

  Width_t thick=2*TAttLine::GetLineWidth();

  if(Set==3){//Sides hits histograms
    Float_t y[14],ey[14];
    gPad->Divide(2,4);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if((i%2)==0){
	sprintf(text,"Plane %1d-P hits",(i/2+1));
	_fetched2[6+i/2]->SetLabelSize(0.1,"X"); // JL1 
	_fetched2[6+i/2]->SetLabelSize(0.1,"Y"); // JL1 
	_fetched2[6+i/2]->SetLineColor(3); // green
	_fetched2[6+i/2]->Draw(); // JL1 
	((TH1*)_fetched2[6+i/2])->SetTitle("");
	_fetched2[18+i/2]->SetLineColor(4); // blue
	_fetched2[18+i/2]->Draw("same"); // data
      }
      else{
	sprintf(text,"Plane %1d-N hits",(i/2+1));
	_fetched2[10+i/2]->SetLabelSize(0.1,"X"); // JL1 
	_fetched2[10+i/2]->SetLabelSize(0.1,"Y"); // JL1 
	_fetched2[10+i/2]->Draw(); // JL1 
	_fetched2[10+i/2]->SetLineColor(3); // JL1 
	((TH1*)_fetched2[10+i/2])->SetTitle("");
	_fetched2[22+i/2]->SetLineColor(4); // data
	_fetched2[22+i/2]->Draw("same"); // data
      }
      TPaveText* box = new TPaveText(.25,.86,.75,1.,"NDC");
      box->AddText(text);
      //      box->SetMargin(0.005);
      box->Draw();
      gPad->Update();
      gPadSave->cd();
    }
  }
  else if(Set==4){//AND hits
    gPad->Divide(2,2);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      if(_fetched2[14+i]){
	sprintf(text,"Plane %1d hits",(i+1));
	((TH1*)_fetched2[14+i])->SetXTitle(text);
	_fetched2[14+i]->SetLabelSize(0.05,"XY"); // JL1 
	_fetched2[14+i]->SetLineColor(3); // JL1 
	_fetched2[14+i]->Draw(); // JL1 
	_fetched2[26+i]->SetLineColor(4); // data
	_fetched2[26+i]->Draw("same"); // data
      }
      gPadSave->cd();
    }
  }
  else if(Set==5){//Choumilov hists
    gPad->Divide(2,3);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<6;i++){
      gPad->cd(i+1);
      if(_fetched2[i]){
	if(i==0)
	  sprintf(text,"Tdiff(1,3) - Tdiff(2,4)");
	else if(i==1)
	  sprintf(text,"Tdiff(1,3)");
	else if(i==2)
	  sprintf(text,"Tdiff(2,4)");
	else if(i==3)
	  sprintf(text,"Little crates data size");
	else if(i==4)
	  sprintf(text,"Layers hits");
	else if(i==5)
	  sprintf(text,"Configuration");
	((TH1*)_fetched2[i])->SetXTitle(text);
	_fetched2[i]->Draw(); 
      }
      gPadSave->cd();
    }
  }
  else if(Set==6){//Anode charge
    Float_t y[14],ey[14];
    gPad->Divide(2,4);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if((i%2)==0)
	sprintf(text,"Anode charge %1d-P (pC)",(i/2+1));
      else
	sprintf(text,"Anode charge %1d-N (pC)",(i/2+1));
      for(int k=0;k<14;k++){
	y[k] =_dynode[k+14*i];
	ey[k]= _dynrms[k+14*i];
      }
      TGraphErrors *gr=new TGraphErrors(14,cpos,y,epos,ey);
      gr->SetTitle("");
      gr->SetMarkerColor(4);
      gr->SetMarkerStyle(21);
      gr->Draw("ALP");
      TPaveText* box = new TPaveText(.25,.86,.75,1.,"NDC");
      box->AddText(text);
      box->Draw();
      gPad->Update();
      gPadSave->cd();
    }
  }
  else if(Set==7){//Dynode charge
    Float_t y[14],ey[14];
    gPad->Divide(2,4);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if((i%2)==0)
	sprintf(text,"Dynode charge %1d-P (pC)",(i/2+1));
      else
	sprintf(text,"Dynode charge %1d-N (pC)",(i/2+1));
      for(int k=0;k<14;k++){
	y[k] =_dynode[k+14*i];
	ey[k]= _dynrms[k+14*i];
      }
      TGraphErrors *gr=new TGraphErrors(14,cpos,y,epos,ey);
      gr->SetTitle("");
      gr->SetMarkerColor(4);
      gr->SetMarkerStyle(21);
      gr->Draw("ALP");
      TPaveText* box = new TPaveText(.25,.86,.75,1.,"NDC");
      box->AddText(text);
      box->Draw();
      gPad->Update();
      gPadSave->cd();
    }
  }
  else if(Set==8){//Stretching Ratio
    Float_t y[14],ey[14];
    gPad->Divide(2,4);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<8;i++){
      gPad->cd(i+1);
      if((i%2)==0)
	sprintf(text,"Time Expansion Factor: %1d-P",(i/2+1));
      else
	sprintf(text,"Time Expansion Factor: %1d-N",(i/2+1));
      for(int k=0;k<14;k++){
	y[k] =_stretch[k+14*i];
	ey[k]= _strrms[k+14*i];
      }
      TGraphErrors *gr=new TGraphErrors(14,cpos,y,epos,ey);
      gr->SetTitle("");
      gr->SetMarkerColor(4);
      gr->SetMarkerStyle(21);
      //    (TH1*((TGraph*)gr->GetHistogram()))->GetXaxis->SetLabelSize(0.1,"X");
      gr->Draw("ALP");
      TPaveText* box = new TPaveText(.2,.86,.8,1.,"NDC");
      box->AddText(text);
      box->Draw();
      gPad->Update();
      gPadSave->cd();
    }
  }
  else if(Set==0){// three windows by Choutko
    gPad->Divide(2,2);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled2[i]->SetFillColor(3);
      _filled2[i]->Draw();
      _filled2[i+4]->SetFillColor(4);
      _filled2[i+4]->Draw("SAME");
      gPadSave->cd();
    }
  }
  else if(Set==1){
    gPad->Divide(2,2);
    TVirtualPad * gPadSave = gPad;
    for(i=0;i<4;i++){
      gPad->cd(i+1);
      gPad->SetLogx(gAMSDisplay->IsLogX());
      gPad->SetLogy(gAMSDisplay->IsLogY());
      gPad->SetLogz(gAMSDisplay->IsLogZ());
      _filled2[8+i]->Draw();
      gPadSave->cd();
    }
  }
  else if(Set==2){
    gPad->Divide(1,2);
    TVirtualPad * gPadSave = gPad;
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

