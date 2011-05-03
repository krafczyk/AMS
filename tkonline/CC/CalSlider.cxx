#include "CalSlider.h"
#include "CalPars.h"
#include "TROOT.h"

#include "TNtuple.h"
#include "TMath.h"
#include "TH2F.h"
#include "TLine.h"
#include <vector>
#include <ctime>
#include "amsdbc.h"
#include "timeid.h"
#include "CalList.h"
ClassImp(CalSlider);

char* CalSlider::filename=0;

CalSlider::CalSlider(char *name,char *title,int xsize,int ysize) : SliderI(name,title,xsize,ysize){
  canvas->SetFillColor(0);
  // default graph
  graphtype = 1;
  // build menu
  if (!gROOT->IsBatch()) BuildMenu();
  // init
  rootfile  = NULL;
  fromdb    = 0;
  // default reffile
  char refname[200];
  sprintf(refname,"%s/tkonline/refs/CalDB_1284918006.root",getenv("AMSWD"));
  reffile = new TFile(refname);
  if (!reffile) printf("Warning: No reference file loaded\n");
  label = NULL;
  text = 0;
  // histograms init
  TDV=0;
  Ped   = 0;
  SigR  = 0;
  Sig   = 0;
  for (int ii=0; ii<16; ii++) ST[ii] = 0;
  Cmn   = 0;
  Occ   = 0;
  ped_p = 0;
  ped_n = 0;
  sig_p = 0;
  sig_n = 0;
  sigR_p = 0;
  sigR_n = 0;
  cn_p = 0;
  cn_n = 0;
  cn2_p = 0;
  cn2_n = 0;
  cn3_p = 0;
  cn3_n = 0;
  sta_p = 0;
  sta_n = 0;
  sigR_p_vs_lad = 0;
  sig_p_vs_lad  = 0;
  sigR_n_vs_lad = 0;
  sig_n_vs_lad  = 0;
  bad_p_vs_lad  = 0;
  bad_n_vs_lad  = 0;
  deltaped_vs_lad = 0;
  deltasig_vs_lad = 0;
  deltasigr_vs_lad = 0;
}


void CalSlider::BuildMenu() {
  frame->SetWindowName("CalMenu");
  graphmenu = new TGListBox(frame,10);
  graphmenu->AddEntry("CalSummary1",1);
  graphmenu->AddEntry("CalSummary2",9);
  graphmenu->AddEntry("CalSummary3",2);
  graphmenu->AddEntry("Calibration1",3);
  graphmenu->AddEntry("Calibration2",4);
  graphmenu->AddEntry("CalDiffSummary",5);
  graphmenu->AddEntry("CalDiff",6);
  graphmenu->AddEntry("CalFinder",7);
  graphmenu->AddEntry("CalFindSummary",8);
  graphmenu->Select(graphtype);
  graphmenu->Resize(180,300);
  frame->AddFrame(graphmenu,new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
  frame->MapSubwindows();
  frame->MoveResize(1200,600);
  frame->MapWindow();
  graphmenu->Connect("Selected(Int_t,Int_t)","CalSlider",this,"selectGraph(Int_t,Int_t)");
}


int CalSlider::GetTkId(){
  if(!lad) return 0;
  return lad->GetTkId();
}


int CalSlider::GetHwId(){
  if(!lad) return 0;
  return lad->GetHwId();
}


void CalSlider::selectGraph(Int_t w, Int_t id){
  graphtype = id;
  Draw();
}


int CalSlider::try2Draw(TrLadCal *cc){
  if(cc && cc->IsFilled()){
    cal = cc;
    lad = TkDBc::Head->FindHwId(cal->HwId);
    Draw();
    /* update my pointer now...*/
    for (calit_cur=calit_begin; calit_cur->second!=cc; calit_cur++);
    return 0;
  } else {
    canvas->Clear();
    if(!label)
      label = new TPaveLabel(0.3,0.35,0.7,0.65,"Not found");
    label->SetFillColor(0);
    label->Draw();
    canvas->Update();
  }
  return 1;
}


void CalSlider::setRootFile(char *filename){
  if(strcmp(filename,"-W")==0){    
    if(TrCalDB::Head==0||caldb==0){
      caldb=new TrCalDB();
      TrCalDB::Head=0;
      caldb->init();
    }
    char rrun[20];
    if(!TDV) OpenTDV();
    //    AMSTimeID* tt= ((CalSlider*)gs->slider)->TDV;
    CalList* bb=new CalList(TDV,gClient->GetRoot(),0, 200, 200); 
    rootfile=0;
  }else{
    rootfile = TFile::Open(filename,"read");
    caldb = (TrCalDB*) rootfile->Get("TrCalDB");
  }
  // canvas title
  char title[100];
  time_t pippo = (time_t)caldb->GetRun();
  sprintf(title,"Calibration %10d %s",caldb->GetRun(),ctime(&pippo));
  canvas->SetTitle(title);
  // pointers
  calit_begin = caldb->trcal_hwidmap.begin();
  calit_end = caldb->trcal_hwidmap.end();
  calit_cur = calit_begin;
  if(calit_cur->second->IsFilled())
    try2Draw(calit_cur->second);
  else
    showNext();
}


void CalSlider::Update() {
  // simple check
  if (rootfile==0) {
    printf("CalSlider::Update-W You must select a file before updating it!\n");
    return;
  }
  ifstream ifile(rootfile->GetName());
  if (!ifile) {
    printf("CalSlider::Update-W File doesn't exist, doing nothing!\n");
    return;
  }
  // histogram could be appended to the current rootfile
  // so is better to clear now the histogram table 
  ClearHistograms(); 
  // mark settings
  char oldfilename[200];
  int  currentHwId = calit_cur->second->GetHwId(); 
  sprintf(oldfilename,"%s",rootfile->GetName());
  rootfile->Close();
  // define again
  rootfile = TFile::Open(oldfilename,"read");
  caldb = (TrCalDB*) rootfile->Get("TrCalDB");
  calit_begin = caldb->trcal_hwidmap.begin();
  calit_end = caldb->trcal_hwidmap.end();
  for (calit_cur=calit_begin; calit_cur->second->GetHwId()!=currentHwId; calit_cur++);
  // canvas title
  char title[100];
  time_t pippo = (time_t)caldb->GetRun();
  sprintf(title,"Calibration %10d %s",caldb->GetRun(),ctime(&pippo));
  canvas->SetTitle(title);
  // update drawing 
  try2Draw(calit_cur->second);
}


void CalSlider::setCalfromDB(time_t run){
  caldb = new TrCalDB();
  caldb->init();
  calit_begin = caldb->trcal_hwidmap.begin();
  calit_end = caldb->trcal_hwidmap.end();
  calit_cur = calit_begin;
  ReadCalFromDB(run); 
  fromdb=1;
  if(calit_cur->second->IsFilled())
    try2Draw(calit_cur->second);
  else
    showNext();
}


void CalSlider::setRefFile(char *filename){
  reffile = new TFile(filename);
  graphtype = 5; 
  graphmenu->Select(graphtype);
  try2Draw(calit_cur->second);
}


void CalSlider::showNext(){
  if(rootfile == NULL && fromdb==0) return;
  trcalIT calit_save = calit_cur;
  do{
    calit_cur++;
    if(calit_cur == calit_end) calit_cur = calit_begin;
  }while(!(calit_cur->second->IsFilled()) || (calit_cur == calit_save));
  try2Draw(calit_cur->second);
}


void CalSlider::showPrev(){
  if(rootfile == NULL&& fromdb==0) return;
  trcalIT calit_save = calit_cur;
  do {
    if(calit_cur == calit_begin) calit_cur = calit_end;
    calit_cur--;
  } while (!(calit_cur->second->IsFilled()) || (calit_cur == calit_save));
  try2Draw(calit_cur->second);
}


int CalSlider::showTkId(int tkid){
  if(rootfile == NULL && fromdb==0) return 2;
  return try2Draw(caldb->FindCal_TkId(tkid));
}


int CalSlider::showHwId(int hwid){
  if(rootfile == NULL&& fromdb==0) return 2;
  return try2Draw(caldb->FindCal_HwId(hwid));
}


void CalSlider::PrintThis(){
  char filename[20];
  /* Cal_TkId+210.ps */
  sprintf(filename,"Cal_TkId%+d.ps",lad->GetTkId());
  canvas->Print(filename,"ps");
}

void CalSlider::PrintAll(){
  canvas->Print("Cal_All.ps[","ps");
  for(calit_cur=calit_begin;calit_cur!=calit_end;calit_cur++){
    if(!calit_cur->second->IsFilled()) continue;
    try2Draw(calit_cur->second);
    canvas->Print("Cal_All.ps","ps");
  }
  canvas->Print("Cal_All.ps]","ps");
}

void CalSlider::CloseFile(){
  //  char ntemp[90];
  //  TCanvas* temp=new TCanvas("temp","temp");
  //  sprintf(ntemp,"%s]",filename);
  //  temp->Print(ntemp,"ps");
  //  Opened=0;
  //  delete temp;
}

void CalSlider::OpenFile(){
  //  char ntemp[90];
  //  sprintf(ntemp,"%s[",filename);
  //  Print(ntemp,"ps");
  //  Opened=1;
}

CalSlider::~CalSlider(){
  ClearHistograms();
  if (rootfile!=0) rootfile->Close();
  rootfile = 0;
  if (reffile!=0) reffile->Close();
  reffile = 0;
  if (graphmenu!=0) delete graphmenu;
  graphmenu = 0;
}

void CalSlider::ClearHistograms(){
  gROOT->cd();
  if (Ped!=0) delete Ped;
  if (Sig!=0) delete Sig;
  if (SigR!=0) delete SigR;
  for (int ii=0; ii<16; ii++) if (ST[ii]!=0) delete ST[ii];
  if (Cmn!=0) delete Cmn;  
  if (Occ!=0) delete Occ;  
  if (ped_p!=0) delete ped_p;
  if (ped_n!=0) delete ped_n;
  if (sig_p!=0) delete sig_p;
  if (sig_n!=0) delete sig_n;
  if (sigR_p!=0) delete sigR_p;
  if (sigR_n!=0) delete sigR_n;
  if (cn_p!=0) delete cn_p;
  if (cn_n!=0) delete cn_n;
  if (cn2_p!=0) delete cn2_p;
  if (cn2_n!=0) delete cn2_n;
  if (cn3_p!=0) delete cn3_p;
  if (cn3_n!=0) delete cn3_n;
  if (sta_p!=0) delete sta_p;
  if (sta_n!=0) delete sta_n;
  if (sigR_p_vs_lad!=0) delete sigR_p_vs_lad;
  if (sig_p_vs_lad!=0) delete sig_p_vs_lad; 
  if (sigR_n_vs_lad!=0) delete sigR_n_vs_lad; 
  if (sig_n_vs_lad!=0) delete sig_n_vs_lad;  
  if (bad_p_vs_lad!=0) delete bad_p_vs_lad;
  if (bad_n_vs_lad!=0) delete bad_n_vs_lad;
  if (deltaped_vs_lad!=0) delete deltaped_vs_lad; 
  if (deltasig_vs_lad!=0) delete deltasig_vs_lad; 
  if (deltasigr_vs_lad!=0) delete deltasigr_vs_lad;
  Ped   = 0;
  SigR  = 0;
  Sig   = 0;
  for (int ii=0; ii<16; ii++) ST[ii] = 0;
  Cmn   = 0;
  Occ   = 0;
  ped_p = 0;
  ped_n = 0;
  sig_p = 0;
  sig_n = 0;
  sigR_p = 0;
  sigR_n = 0;
  cn_p = 0;
  cn_n = 0;
  cn2_p = 0;
  cn2_n = 0;
  cn3_p = 0;
  cn3_n = 0;
  sta_p = 0;
  sta_n = 0;
  sigR_p_vs_lad = 0;
  sig_p_vs_lad  = 0;
  sigR_n_vs_lad = 0;
  sig_n_vs_lad  = 0;
  bad_p_vs_lad  = 0;
  bad_n_vs_lad  = 0;
  deltaped_vs_lad = 0;
  deltasig_vs_lad = 0;
  deltasigr_vs_lad = 0;
}


/* Drawing management */
void CalSlider::Draw(const Option_t *aa,int flag){
  switch(graphtype){
  case 1:
    Draw1(aa,flag);
    break;
  case 2:
    Draw2(aa,flag);
    break;
  case 3:
    Draw3(aa,flag);
    break;
  case 4:
    Draw4(aa,flag);
    break;
  case 5:
    Draw5(aa,flag);
    break;
  case 6:
    Draw6(aa,flag);
    break;
  case 7:
    Draw7(aa,flag);
    break;
  case 9:
    Draw9(aa,flag);
  }
}

void CalSlider::FillLadderHistos() {
  gROOT->cd();
  // Definitions
  if(Ped==0)  Ped  = new TH1F("Pedestal","; channel; amplitude (ADC)",1024,0,1024);
  if(SigR==0) SigR = new TH1F("SigmaRaw","; channel; #sigma_{raw} (ADC)",1024,0,1024);
  if(Sig==0)  Sig  = new TH1F("Sigma","; channel; #sigma (ADC)",1024,0,1024);
  char STname[80];
  for (int ii=0; ii<16; ii++) {
    sprintf(STname,"StatusBit_%02d",ii);
    if(ST[ii]==0) ST[ii] = new TH1F(STname,"; channel",1024,0,1024); 
  }
  if(Cmn==0)  Cmn  = new TH1F("CommonNoise","; VA number; common noise (ADC)",16,0,16);
  if(Occ==0)  Occ  = new TH1F("Occupancy","; channel; occupancy",1024,0,1024); 

  // Fill
  float bitvalue[16] = {-0.50,-0.75,-1.00,-1.25,-1.50,-2,-1,-2,-1,-1.75,-1,-2,-1,-2,-1,-2.00};
  for (int ii=0;ii<1024;ii++){
    Ped->SetBinContent(ii+1,cal->Pedestal(ii));
    Sig->SetBinContent(ii+1,cal->Sigma(ii));
    SigR->SetBinContent(ii+1,cal->SigmaRaw(ii));
    Occ->SetBinContent(ii+1,cal->Occupancy(ii));
    for (int bb=0; bb<16; bb++) { 
      if (TESTBIT(cal->Status(ii),bb)==0) ST[bb]->SetBinContent(ii+1,0); 
      else                                ST[bb]->SetBinContent(ii+1,bitvalue[bb]);
    }
  }
  for(int ii=0;ii<16;ii++){
    Cmn->SetBinContent(ii+1,cal->GetCNmean(ii));
    Cmn->SetBinError(ii+1,cal->GetCNrms(ii));
  }  
}



void CalSlider::Draw5(const Option_t* aa,int flag){
  gROOT->cd();

  // Drawing settings
  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);
  
  if (deltaped_vs_lad==0)  deltaped_vs_lad  = new TH1F("deltaped_vs_lad","; ladder (iCrate*24 + iTDR); average #DeltaPedestal (ADC)",192,0,192);
  if (deltasig_vs_lad==0)  deltasig_vs_lad  = new TH1F("deltasig_vs_lad","; ladder (iCrate*24 + iTDR); average #Delta#sigma (ADC)",192,0,192);
  if (deltasigr_vs_lad==0) deltasigr_vs_lad = new TH1F("deltasigr_vs_lad","; ladder (iCrate*24 + iTDR); average #Delta#sigma_{raw} (ADC)",192,0,192);

  float ped1[192];
  float sig1[192];
  float sigr1[192];
  caldb = (TrCalDB*) reffile->Get("TrCalDB");
  for (int ii=0; ii<caldb->GetEntries() ; ii++){
    ped1[ii]  = 0.;
    sig1[ii]  = 0.;
    sigr1[ii] = 0.;
    TrLadCal* ladcal = caldb->GetEntry(ii);
    for (int jj=0; jj<1024; jj++) {
      ped1[ii]  += ladcal->Pedestal(jj);
      sig1[ii]  += ladcal->Sigma(jj);
      sigr1[ii] += ladcal->SigmaRaw(jj); 
    }
    ped1[ii]  /= 1024.;
    sig1[ii]  /= 1024.;
    sigr1[ii] /= 1024.;
  }


  float ped2[192];
  float sig2[192];
  float sigr2[192];
  caldb = (TrCalDB*) rootfile->Get("TrCalDB");
  for (int ii=0; ii<caldb->GetEntries() ; ii++){
    ped2[ii]  = 0.;
    sig2[ii]  = 0.;
    sigr2[ii] = 0.;
    TrLadCal* ladcal = caldb->GetEntry(ii);
    for (int jj=0; jj<1024; jj++) {
      ped2[ii]  += ladcal->Pedestal(jj);
      sig2[ii]  += ladcal->Sigma(jj);
      sigr2[ii] += ladcal->SigmaRaw(jj);
    }
    ped2[ii]  /= 1024.;
    sig2[ii]  /= 1024.;
    sigr2[ii] /= 1024.;
  }

  for (int ii=0; ii<caldb->GetEntries(); ii++){
    deltaped_vs_lad->SetBinContent(ii+1,ped1[ii] - ped2[ii]);
    deltasig_vs_lad->SetBinContent(ii+1,sig1[ii] - sig2[ii]);
    deltasigr_vs_lad->SetBinContent(ii+1,sigr1[ii] - sigr2[ii]);
  }

  TVirtualPad* cc0 = canvas->cd(1);
  deltaped_vs_lad->Draw("l");

  cc0 = canvas->cd(2);
  deltasigr_vs_lad->Draw("l");

  cc0 = canvas->cd(4);
  deltasig_vs_lad->Draw("l");

  canvas->Update();
}


void CalSlider::Draw6(const Option_t* aa,int flag){
  gROOT->cd();

  // Drawing settings
  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);

  if (reffile==NULL) {
    printf("WARNING: No Reference Calibration File Selected!!!\n");
    return;
  }
  
  // printf("Reference Calibration File: %s\n",reffile->GetName());

  FillLadderHistos();

  TH1F* PedDiff  = (TH1F*) Ped->Clone("PedDiff");
  TH1F* SigRNeg  = (TH1F*) SigR->Clone("SigmaRawDiff");
  TH1F* SigRDiff = (TH1F*) SigR->Clone("SigmaRawNeg");
  TH1F* SigNeg   = (TH1F*) Sig->Clone("SigmaNeg"); 
  TH1F* SigDiff  = (TH1F*) Sig->Clone("SigmaDiff"); 

  TrCalDB* calsave=caldb;
  // reference calibration  
  caldb = (TrCalDB*) reffile->Get("TrCalDB");
  TrLadCal* calref = caldb->FindCal_HwId(lad->GetHwId());
  for (int ii=0;ii<1024;ii++){
    PedDiff->SetBinContent(ii+1,calref->Pedestal(ii) - Ped->GetBinContent(ii+1));
    SigNeg->SetBinContent(ii+1,-calref->Sigma(ii));
    SigDiff->SetBinContent(ii+1,calref->Sigma(ii) - Sig->GetBinContent(ii+1));
    SigRNeg->SetBinContent(ii+1,-calref->SigmaRaw(ii));
    SigRDiff->SetBinContent(ii+1,calref->SigmaRaw(ii) - SigR->GetBinContent(ii+1));
  }
  Color_t col[5]={kGreen,kRed,kGreen,kBlue,kBlack};

  // Drawing pedestals 
  TVirtualPad* cc0 = canvas->cd(1);  
  SetHistStyle1(Ped);
  cc0->SetGrid(1,0);
  SetHistStyle1(PedDiff);
  PedDiff->SetYTitle("#DeltaPedestal (ADC)");
  PedDiff->Draw("l");
  gPad->Update();
  Double_t xmin,ymin,xmax,ymax;
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  TLine *line1=new TLine();
  line1->SetLineColor(kBlue);
  line1->SetLineStyle(5);
  TLine *line2=new TLine();
  line2->SetLineColor(kRed);
  line2->SetLineStyle(5);
  for (int i=0; i<1024; i++) {
    if (cal->Status(i)!=0 && calref->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,ymax);
    if (calref->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,ymin);
  }

  // Drawing sigma  
  cc0 = canvas->cd(4);
  SetHistStyle1(Sig);
  SetHistStyle1(SigNeg);
  cc0->SetGrid(1,0);
  Sig->SetFillStyle(3004);
  Sig->SetFillColor(col[4]);
  Sig->GetYaxis()->SetRangeUser(-10,10);
  Sig->Draw("l");
  SigNeg->SetFillStyle(3004);
  SigNeg->SetFillColor(col[1]);
  SigNeg->GetYaxis()->SetRangeUser(-10,10);
  SigNeg->Draw("l SAME");
  gPad->Update();
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  line1->SetLineStyle(1);
  line1->SetLineColor(kMagenta);
  line2->SetLineStyle(1);
  line2->SetLineColor(kMagenta);
  for (int i=0; i<1024; i++) {
    if (cal->Status(i)!=0 && calref->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->Sigma(i))?cal->Sigma(i):ymax);
    if (calref->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-calref->Sigma(i))?-calref->Sigma(i):ymin);
  }

  // Drawing sigma raw   
  cc0 = canvas->cd(2);
  SetHistStyle1(SigR);
  SetHistStyle1(SigRNeg);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  SigR->GetYaxis()->SetRangeUser(-18,18);
  SigR->Draw("l");
  SigRNeg->SetFillColor(col[1]);
  SigRNeg->SetFillStyle(3004);
  SigRNeg->GetYaxis()->SetRangeUser(-18,18);
  SigRNeg->Draw("l SAME");
  gPad->Update();
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  for (int i=0; i<1024; i++) {
    if (cal->Status(i)!=0 && calref->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->SigmaRaw(i))?cal->SigmaRaw(i):ymax);
    if (calref->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-calref->SigmaRaw(i))?-calref->SigmaRaw(i):ymin);
  }

  // Drawing sigma difference   
  cc0 = canvas->cd(3);
  SetHistStyle1(SigDiff);
  cc0->SetGrid(1,0);
  SigDiff->SetYTitle("#Delta#sigma (ADC)");
  SigDiff->SetFillColor(col[4]);
  SigDiff->GetYaxis()->SetRangeUser(-18,18);
  SigDiff->Draw("l");

  canvas->Update();
  // resetta il calDB
  caldb=calsave;
  //caldb = (TrCalDB*) rootfile->Get("TrCalDB"); // do not know why, but this one does not work if you directly click on the Hwid buttons (loop in try2Daw just runs infinitely: the pointer is not the right one
} 

/* Draw Summary Plots 2 */
void CalSlider::Draw2(const Option_t* aa,int flag){
  gROOT->cd();

  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,1,0.001,0.001);  

  if (sigR_p_vs_lad==0) sigR_p_vs_lad = new TH1F("sigR_p_vs_lad","; ladder (iCrate*24 + iTDR); average raw noise (ADC)",192,0,192); 
  if (sigR_n_vs_lad==0) sigR_n_vs_lad = new TH1F("sigR_n_vs_lad","; ladder (iCrate*24 + iTDR); average raw noise (ADC)",192,0,192); 
  if (sig_p_vs_lad==0)  sig_p_vs_lad  = new TH1F("sig_p_vs_lad","; ladder (iCrate*24 + iTDR); average noise (ADC)",192,0,192); 
  if (sig_n_vs_lad==0)  sig_n_vs_lad  = new TH1F("sig_n_vs_lad","; ladder (iCrate*24 + iTDR); average noise (ADC)",192,0,192); 
  if (bad_p_vs_lad==0)  bad_p_vs_lad  = new TH1F("bad_p_vs_lad","; ladder (iCrate*24 + iTDR); bad strips (%)",192,0,192);
  if (bad_n_vs_lad==0)  bad_n_vs_lad  = new TH1F("bad_n_vs_lad","; ladder (iCrate*24 + iTDR); bad strips (%)",192,0,192);
 
  for (int ii=0; ii<caldb->GetEntries(); ii++){
    TrLadCal* ladcal = caldb->GetEntry(ii);
    TkLadder* ladder = TkDBc::Head->FindHwId(ladcal->GetHwId());    
    if ( (ladcal)&&(ladcal->IsFilled()) ) {
      float meanR = 0;
      float mean  = 0;
      float rmsR  = 0;
      float rms   = 0;
      float nbad  = 0;
      for (int jj=0; jj<640; jj++) {
	meanR += ladcal->SigmaRaw(jj);
	rmsR  += pow((float)ladcal->SigmaRaw(jj),2);
	mean  += ladcal->Sigma(jj);
	rms   += pow((float)ladcal->Sigma(jj),2);
        if (ladcal->Status(jj)!=0) nbad++; 
      }
      rmsR  = sqrt(fabs(rmsR - meanR*meanR/640./640.)/640.);
      meanR = meanR/640.;
      rms   = sqrt(fabs(rms - mean*mean/640./640.)/640.);
      mean  = mean/640.;
      sigR_p_vs_lad->SetBinContent(ii+1,meanR);
      sig_p_vs_lad->SetBinContent(ii+1,mean);  
      // sigR_p_vs_lad->SetBinError(ii+1,rmsR);
      // sig_p_vs_lad->SetBinError(ii+1,rms);  
      bad_p_vs_lad->SetBinContent(ii+1,100.*nbad/640.);

      meanR = 0;
      mean  = 0;
      rmsR  = 0;
      rms   = 0;
      nbad  = 0;
      for (int jj=640; jj<1024; jj++) {
	meanR += ladcal->SigmaRaw(jj);
	rmsR  += pow((float)ladcal->SigmaRaw(jj),2);
	mean  += ladcal->Sigma(jj);
	rms   += pow((float)ladcal->Sigma(jj),2);	
        if (ladcal->Status(jj)!=0) nbad++;
      }
      rmsR  = sqrt(fabs(rmsR - meanR*meanR/384./384.)/384.);
      meanR = meanR/384.;
      rms   = sqrt(fabs(rms - mean*mean/384./384.)/384.);
      mean  = mean/384.;
      sigR_n_vs_lad->SetBinContent(ii+1,meanR); 
      sig_n_vs_lad->SetBinContent(ii+1,mean);   
      // sigR_n_vs_lad->SetBinError(ii+1,rmsR);
      // sig_n_vs_lad->SetBinError(ii+1,rms);  
      bad_n_vs_lad->SetBinContent(ii+1,100.*nbad/384.);
    }
    else {
      sigR_p_vs_lad->SetBinContent(ii+1,0);
      sigR_n_vs_lad->SetBinContent(ii+1,0); 
      sig_p_vs_lad->SetBinContent(ii+1,0);  
      sig_n_vs_lad->SetBinContent(ii+1,0);   
      bad_p_vs_lad->SetBinContent(ii+1,0);
      bad_n_vs_lad->SetBinContent(ii+1,0);
    }
  }
 
  char testo[100];
 
  // Bad strips
  TVirtualPad* cc0 = canvas->cd(1);  
  cc0->SetGridy();
  cc0->SetGridx();
  TH1F* frame = cc0->DrawFrame(0,-5,192,105);
  frame->SetXTitle("iCrate*24 + iTdr");
  frame->SetYTitle("Bad Strips (%)");
  // SetHistStyle3(bad_p_vs_lad);
  // SetHistStyle3(bad_n_vs_lad);
  bad_p_vs_lad->SetMarkerStyle(24);
  bad_n_vs_lad->SetMarkerStyle(25);
  bad_p_vs_lad->SetMarkerColor(kBlue);
  bad_n_vs_lad->SetMarkerColor(kRed);
  bad_p_vs_lad->Draw("p SAME");
  bad_n_vs_lad->Draw("p SAME");
  for (int ii=1; ii<=192; ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = bad_p_vs_lad->GetBinContent(ii);
    float mean_n = bad_n_vs_lad->GetBinContent(ii);
    if (mean_n>20.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if (mean_p>20.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }

  // Average noise
  cc0 = canvas->cd(2);
  cc0->SetGridy();  
  cc0->SetGridx();  
  frame = cc0->DrawFrame(0,-1,192,10);
  frame->SetXTitle("iCrate*24 + iTdr");
  frame->SetYTitle("Average Noise (ADC)");
  // SetHistStyle3(sig_p_vs_lad);
  // SetHistStyle3(sig_n_vs_lad);
  sig_p_vs_lad->SetMarkerStyle(24);
  sig_n_vs_lad->SetMarkerStyle(25);
  sig_p_vs_lad->SetMarkerColor(kBlue);
  sig_n_vs_lad->SetMarkerColor(kRed);
  sig_p_vs_lad->Draw("p SAME");
  sig_n_vs_lad->Draw("p SAME");
  for (int ii=1; ii<=192; ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = sig_p_vs_lad->GetBinContent(ii);
    float mean_n = sig_n_vs_lad->GetBinContent(ii);
    if ( (mean_n<2)||(mean_n>5.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if ( (mean_p<2)||(mean_p>5.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }

  canvas->Update();
}


/* Draw Summary Plots 1 */
void CalSlider::Draw1(const Option_t* aa,int flag){
  gROOT->cd();

  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(3,2,0.001,0.001);  

  if (ped_p==0) ped_p = new TH1F("ped_p","; pedestal (ADC)",100,0.,1000.);
  else          ped_p->Reset();  
  ped_p->SetLineColor(4);
  ped_p->SetFillColor(4);
  ped_p->SetFillStyle(3002);
  ped_p->SetStats(kTRUE);
  if (ped_n==0) ped_n = new TH1F("ped_n","; pedestal (ADC)",100,0.,1000.);
  else          ped_n->Reset();
  ped_n->SetLineColor(2);
  ped_n->SetFillColor(2);
  ped_n->SetFillStyle(3002);
  ped_n->SetStats(kTRUE);
  if (sig_p==0) sig_p = new TH1F("sig_p","; noise (ADC)",400,0.,50.);
  else          sig_p->Reset();
  sig_p->SetLineColor(4);
  sig_p->SetFillColor(4);
  sig_p->SetFillStyle(3002);
  sig_p->SetStats(kTRUE);
  if (sig_n==0) sig_n = new TH1F("sig_n","; noise (ADC)",400,0.,50.);
  else          sig_n->Reset();
  sig_n->SetLineColor(2);
  sig_n->SetFillColor(2);
  sig_n->SetFillStyle(3002);
  sig_n->SetStats(kTRUE);
  if (sta_p==0) sta_p = new TH1F("sta_p","; status bit",16,0.,16.);
  else          sta_p->Reset();
  sta_p->SetLineColor(4);
  sta_p->SetFillColor(4);
  sta_p->SetFillStyle(3002);
  sta_p->SetStats(kFALSE);
  if (sta_n==0) sta_n = new TH1F("sta_n","; status bit",16,0.,16.);
  else          sta_n->Reset();
  sta_n->SetLineColor(2);
  sta_n->SetFillColor(2);
  sta_n->SetFillStyle(3002);
  sta_n->SetStats(kFALSE);

  // Active ladders counter
  int   nladders = 0;

  // Histos for Calibration Parameters
  int nout_p = 0; 
  int nout_n = 0; 
  int nded_p = 0; 
  int nded_n = 0; 

  for (int ii=0; ii<caldb->GetEntries(); ii++){
    TrLadCal* ladcal = caldb->GetEntry(ii);
    TkLadder* ladder = TkDBc::Head->FindHwId(ladcal->GetHwId());
    if ( (ladcal)&&(ladcal->IsFilled()) ) {
      nladders++;
      for (int jj=0; jj<1024; jj++) {
        int status = ladcal->GetStatus(jj);
        int side = 1-(int)jj/640;
        // if ( (status>>0&1) || (status>>2&1) ) dead[side]++;
        // if ( (status>>1&1) || (status>>3&1) ) noisy[side]++;
        // if ( (status>>4&1) )                  gauss[side]++;
        // if (status!=0) continue;
        for (int bb=0; bb<16; bb++) {
          int bit = status>>bb&1;
          if (bit!=0) {
            if (jj>=640) sta_n->Fill(bb);
            else         sta_p->Fill(bb);
          }
        } 
        if (side==1) { 
          ped_p->Fill(ladcal->GetPedestal(jj)); 
          sig_p->Fill(ladcal->GetSigma(jj)); 
          if (ladcal->GetSigma(jj)>10.) nout_p++; 
          if (ladcal->GetSigma(jj)<0.2) nded_p++; 
        } 
        if (side==0) { 
          ped_n->Fill(ladcal->GetPedestal(jj)); 
          sig_n->Fill(ladcal->GetSigma(jj)); 
          if (ladcal->GetSigma(jj)>10.) nout_n++; 
          if (ladcal->GetSigma(jj)<0.2) nded_n++; 
        } 
      }
    }
  }

  TVirtualPad* cc0 = canvas->cd(1);  
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  ped_p->Draw();
  cc0 = canvas->cd(4);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  ped_n->Draw();
  cc0 = canvas->cd(2);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sig_p->Draw();
  cc0 = canvas->cd(5);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sig_n->Draw();
  cc0 = canvas->cd(3);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sta_p->Draw();
  char testo[100];
  if (text==0) text = new TText();
  text->SetTextColor(kBlack);
  for (int bb=0; bb<16; bb++) {
    int value = (int) sta_p->GetBinContent(bb+1);
    sprintf(testo,"%d",value);
    if (value>0) text->DrawText(bb+0.1,value*1.1,testo); 	 		 
  } 
  cc0 = canvas->cd(6);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sta_n->Draw();
  for (int bb=0; bb<16; bb++) {
    int value = (int) sta_n->GetBinContent(bb+1);
    sprintf(testo,"%d",value);
    if (value>0) text->DrawText(bb+0.1,value*1.1,testo); 	 		 
  } 
  canvas->Update();
}

/* Draw Summary Plots 9 */
void CalSlider::Draw9(const Option_t* aa,int flag){
  gROOT->cd();

  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(3,2,0.001,0.001);  

  if (sig_p==0) sig_p = new TH1F("sig_p","; noise (ADC)",400,0.,50.);
  else          sig_p->Reset();
  sig_p->SetLineColor(kBlue);
  sig_p->SetFillColor(kBlue);
  sig_p->SetFillStyle(3002);
  sig_p->SetStats(kTRUE);
  if (sig_n==0) sig_n = new TH1F("sig_n","; noise (ADC)",400,0.,50.);
  else          sig_n->Reset();
  sig_n->SetLineColor(kRed);
  sig_n->SetFillColor(kRed);
  sig_n->SetFillStyle(3002);
  sig_n->SetStats(kTRUE);

  if (sigR_p==0) sigR_p = new TH1F("sigR_p","; raw noise (ADC)",400,0.,50.);
  else          sigR_p->Reset();
  sigR_p->SetLineColor(kAzure-4);
  sigR_p->SetFillColor(kAzure-4);
  sigR_p->SetFillStyle(3002);
  sigR_p->SetStats(kTRUE);
  if (sigR_n==0) sigR_n = new TH1F("sigR_n","; raw noise (ADC)",400,0.,50.);
  else          sigR_n->Reset();
  sigR_n->SetLineColor(kViolet);
  sigR_n->SetFillColor(kViolet);
  sigR_n->SetFillStyle(3002);
  sigR_n->SetStats(kTRUE);

  if (cn_p==0) cn_p = new TH1F("cn_p","; common noise sigma (ADC)",100,0.,20.);
  else          cn_p->Reset();
  cn_p->SetLineColor(kBlue);
  cn_p->SetFillColor(kBlue);
  cn_p->SetFillStyle(3002);
  cn_p->SetStats(kTRUE);
  if (cn_n==0) cn_n = new TH1F("cn_n","; common noise sigma (ADC)",100,0.,20.);
  else          cn_n->Reset();
  cn_n->SetLineColor(kRed);
  cn_n->SetFillColor(kRed);
  cn_n->SetFillStyle(3002);
  cn_n->SetStats(kTRUE);

  if (cn3_p==0) cn3_p = new TH1F("cn3_p","; common noise sigma (ADC)",100,0.,20.);
  else          cn3_p->Reset();
  cn3_p->SetLineColor(kAzure-4);
  cn3_p->SetFillColor(kAzure-4);
  cn3_p->SetFillStyle(3002);
  cn3_p->SetStats(kTRUE);
  if (cn3_n==0) cn3_n = new TH1F("cn3_n","; common noise sigma (ADC)",100,0.,20.);
  else          cn3_n->Reset();
  cn3_n->SetLineColor(kViolet);
  cn3_n->SetFillColor(kViolet);
  cn3_n->SetFillStyle(3002);
  cn3_n->SetStats(kTRUE);

  if (cn2_p==0) cn2_p = new TH1F("cn2_p","; common noise mean (ADC)",100,-10.,10.);
  else          cn2_p->Reset();
  cn2_p->SetLineColor(kBlue);
  cn2_p->SetFillColor(kBlue);
  cn2_p->SetFillStyle(3002);
  cn2_p->SetStats(kTRUE);
  if (cn2_n==0) cn2_n = new TH1F("cn2_n","; common noise mean (ADC)",100,-10.,10.);
  else          cn2_n->Reset();
  cn2_n->SetLineColor(kRed);
  cn2_n->SetFillColor(kRed);
  cn2_n->SetFillStyle(3002);
  cn2_n->SetStats(kTRUE);

  // Active ladders counter
  int   nladders = 0;

  // Histos for Calibration Parameters
  int nout_p = 0; 
  int nout_n = 0; 
  int nded_p = 0; 
  int nded_n = 0; 

  vector<int>   badcn_p;
  vector<float> badcn_p_value;
  vector<int>   badcn_n;
  vector<float> badcn_n_value;

  for (int ii=0; ii<caldb->GetEntries(); ii++){
    TrLadCal* ladcal = caldb->GetEntry(ii);
    TkLadder* ladder = TkDBc::Head->FindHwId(ladcal->GetHwId());
    if ( (ladcal)&&(ladcal->IsFilled()) ) {
      nladders++;
      for(int kk=0;kk<10;kk++){
	cn2_p->Fill(ladcal->GetCNmean(kk));
	if (fabs(ladcal->GetCNmean(kk))>1) {
	  //	  printf("%f\n", ladcal->GetCNmean(kk));
	  badcn_p.push_back((int)ladder->GetHwId());
	  badcn_p_value.push_back((float)ladcal->GetCNmean(kk));
	}
	cn3_p->Fill(ladcal->GetCNrms(kk));
      }
      for(int kk=10;kk<16;kk++){
	cn2_n->Fill(ladcal->GetCNmean(kk));
	if (fabs(ladcal->GetCNmean(kk))>1) {
	  //	  printf("%f\n", ladcal->GetCNmean(kk));
	  badcn_n.push_back((int)ladder->GetHwId());
	  badcn_n_value.push_back((float)ladcal->GetCNmean(kk));
	}
	cn3_n->Fill(ladcal->GetCNrms(kk));
      }
      double cncomputed_p;
      double cncomputed_n;
      for (int jj=0; jj<1024; jj++) {
        int status = ladcal->GetStatus(jj);
        int side = 1-(int)jj/640;
        if (side==1) { 
          sig_p->Fill(ladcal->GetSigma(jj)); 
          sigR_p->Fill(ladcal->GetSigmaRaw(jj));
	  cncomputed_p+=fabs(ladcal->GetSigmaRaw(jj)*ladcal->GetSigmaRaw(jj) - ladcal->GetSigma(jj)*ladcal->GetSigma(jj));
	  //	  cncomputed_p+=fabs(ladcal->GetSigmaRaw(jj) - ladcal->GetSigma(jj));
	  if ((jj+1)%64==0) {
	    //	    printf("jj = %d\n", jj);
	    cn_p->Fill(TMath::Sqrt(cncomputed_p/64));
	    cncomputed_p=0;
	  }
          if (ladcal->GetSigma(jj)>10.) nout_p++; 
          if (ladcal->GetSigma(jj)<0.2) nded_p++; 
        }
        else if (side==0) { 
          sig_n->Fill(ladcal->GetSigma(jj)); 
          sigR_n->Fill(ladcal->GetSigmaRaw(jj));
	  cncomputed_n+=fabs(ladcal->GetSigmaRaw(jj)*ladcal->GetSigmaRaw(jj) - ladcal->GetSigma(jj)*ladcal->GetSigma(jj));
	  //	  cncomputed_n+=fabs(ladcal->GetSigmaRaw(jj) - ladcal->GetSigma(jj));
	  if ((jj+1)%64==0) {
	    //	    printf("jj = %d\n", jj);
	    cn_n->Fill(TMath::Sqrt(cncomputed_n/64));
	    cncomputed_n=0;
	  }
          if (ladcal->GetSigma(jj)>10.) nout_n++; 
          if (ladcal->GetSigma(jj)<0.2) nded_n++; 
        } 
      }
    }
  }

  TVirtualPad* cc0 = canvas->cd(1);  
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sig_p->Draw();
  sigR_p->Draw("SAME");
  cc0 = canvas->cd(4);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  sig_n->Draw();
  sigR_n->Draw("SAME");
  cc0 = canvas->cd(2);  
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  cn3_p->Draw();
  cn_p->Draw("SAME");
  cc0 = canvas->cd(5);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  cn3_n->Draw();
  cn_n->Draw("SAME");
  cc0 = canvas->cd(3);  
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  cn2_p->Draw();
  int shift[9]= {1,2,5,10,20,50,100,200,500};
  int shift1=0;
  int shift2=0;
  int shift3=0;
  int shift4=0;
  char testo[100];
  if (text==0) text = new TText();
  text->SetTextColor(kBlack);
  int size=(int)(badcn_p.size());
  //  printf("Size p %d\n", size);
  for (int bb=0; bb<size; bb++) {
    int hwid = (int) badcn_p.at(bb);
    //    printf("HwId %03d\n",hwid);
    sprintf(testo,"%03d",hwid);
    if (badcn_p_value.at(bb)>0) {
      text->DrawText(badcn_p_value.at(bb)-0.5,1+shift[shift1],testo);
      shift1++;
    }
    else {
      text->DrawText(badcn_p_value.at(bb)-1.5,1+shift[shift2],testo);
      shift2++;
    }
  }
  cc0 = canvas->cd(6);
  cc0->SetLogy();
  cc0->SetGridy();
  cc0->SetGridx();
  cn2_n->Draw();
  if (text==0) text = new TText();
  text->SetTextColor(kBlack);
  size=(int)(badcn_n.size());
  //  printf("Size n %d\n", size);
  for (int bb=0; bb<size; bb++) {
    int hwid = (int) badcn_n.at(bb);
    //    printf("HwId %03d\n",hwid);
    sprintf(testo,"%03d",hwid);
    if (badcn_n_value.at(bb)>0) {
      text->DrawText(badcn_n_value.at(bb)-0.5,1+shift[shift3],testo);
      shift3++;
    }
    else {
      text->DrawText(badcn_n_value.at(bb)-1.5,1+shift[shift4],testo);
      shift4++;
    }
  }

  canvas->Update();
}


/* See Standard Calibration */
void CalSlider::Draw3(const Option_t* aa,int flag){
  gROOT->cd();
  
  // Drawing settings
  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);  

  FillLadderHistos();

  int dead_p = 0;
  int dead_n = 0;
  int noisy_p = 0;
  int noisy_n = 0;
  int gauss_p = 0;
  int gauss_n = 0;
  int pass2_p = 0;
  int pass2_n = 0;
  int perm_p = 0;
  int perm_n = 0;
  for (int ii=0;ii<1024;ii++){
    if ( (ii< 640)&&( (TESTBIT(cal->Status(ii),0)!=0)||(TESTBIT(cal->Status(ii),2)!=0) ) ) dead_p++;
    if ( (ii< 640)&&( (TESTBIT(cal->Status(ii),1)!=0)||(TESTBIT(cal->Status(ii),3)!=0) ) ) noisy_p++;
    if ( (ii< 640)&&(TESTBIT(cal->Status(ii),4)!=0) )                                      gauss_p++;
    if ( (ii< 640)&&(TESTBIT(cal->Status(ii),9)!=0) )                                      pass2_p++;
    if ( (ii< 640)&&(TESTBIT(cal->Status(ii),15)!=0) )                                     perm_p++;
    if ( (ii>=640)&&( (TESTBIT(cal->Status(ii),0)!=0)||(TESTBIT(cal->Status(ii),2)!=0) ) ) dead_n++;
    if ( (ii>=640)&&( (TESTBIT(cal->Status(ii),1)!=0)||(TESTBIT(cal->Status(ii),3)!=0) ) ) noisy_n++;
    if ( (ii>=640)&&(TESTBIT(cal->Status(ii),4)!=0) )                                      gauss_n++;
    if ( (ii>=640)&&(TESTBIT(cal->Status(ii),9)!=0) )                                      pass2_n++;
    if ( (ii>=640)&&(TESTBIT(cal->Status(ii),15)!=0) )                                     perm_n++;
  }
  
  /* Drawing pedestals (top, left)*/
  TVirtualPad*cc0 = canvas->cd(1);  
  SetHistStyle1(Ped);
  cc0->SetGrid(1,0);
  Ped->Draw("l"); // "l" to avoid annoying white strips on the display, temporary hopefully

  Color_t col[6]={kGreen,kRed,kGreen,kBlue,kBlack,kViolet};

  /* Drawing sigma (bottom, right),
     dead/noisy channels for sigma
     and not-gaussian strips 
     [+ VA sigma mean * 1.5 and *4.] */
  cc0 = canvas->cd(4);  
  SetHistStyle1(Sig);
  cc0->SetGrid(1,0);
  Sig->SetFillStyle(3004);
  Sig->SetFillColor(col[4]);
  Sig->GetYaxis()->SetRangeUser(-2,10);
  Sig->Draw("l"); // "l" temporary solution for white strips problem on display
  // all the strips!!!
  ST[0]->SetFillColor(col[2]);
  ST[0]->SetFillStyle(3004);
  ST[0]->SetLineColor(col[2]);
  ST[0]->Draw("same");
  ST[1]->SetFillColor(col[3]);
  ST[1]->SetFillStyle(3005);
  ST[1]->SetLineColor(col[3]);
  ST[1]->Draw("same");
  ST[2]->SetFillColor(col[2]);
  ST[2]->SetFillStyle(3005);
  ST[2]->SetLineColor(col[2]);
  ST[2]->Draw("same");
  ST[3]->SetFillColor(col[3]);
  ST[3]->SetFillStyle(3004);
  ST[3]->SetLineColor(col[3]);
  ST[3]->Draw("same");
  ST[4]->SetFillColor(col[1]);
  ST[4]->SetFillStyle(3005);
  ST[4]->SetLineColor(col[1]);
  ST[4]->Draw("same");
  ST[9]->SetFillColor(col[5]);
  ST[9]->SetLineColor(col[5]);
  ST[9]->SetFillStyle(3004);
  ST[9]->Draw("same");
  ST[15]->SetFillColor(col[4]);
  ST[15]->SetLineColor(col[4]);
  ST[15]->SetFillStyle(3004);
  ST[15]->Draw("same");

  /* Drawing sigma raw (top, right) 
     and noisy/dead channels for sigma raw 
     [+ VA sigma raw mean * 1.5 and *4.] */
  cc0 = canvas->cd(2);  
  SetHistStyle1(SigR);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  SigR->GetYaxis()->SetRangeUser(-2,18);
  SigR->Draw("l"); // "l" temporary solution for white strips problem on display
  ST[0]->SetFillColor(col[2]);
  ST[0]->SetFillStyle(3004);
  ST[0]->SetLineColor(col[2]);
  ST[0]->Draw("same");
  ST[1]->SetFillColor(col[3]);
  ST[1]->SetFillStyle(3005);
  ST[1]->SetLineColor(col[3]);
  ST[1]->Draw("same");
  

  /* Parameters summary*/
  cc0=canvas->cd(3);  
  char testo[100];
  if (text==0) text = new TText();
  text->SetTextColor(kBlack);
  text->DrawTextNDC(0.15,0.85,"Calibration parameters:"); 	 		 
  sprintf(testo,"HwId = %03d  TkId = %+03d",cal->HwId,(TkDBc::Head->FindHwId(cal->HwId))->GetTkId());	
  text->DrawTextNDC(0.15,0.75,testo); 	 		 
  sprintf(testo,"dspver = %hx",cal->dspver);	 
  text->DrawTextNDC(0.15,0.70,testo);  
  sprintf(testo,"S1_lowthres = %7.3f",cal->S1_lowthres);	 
  text->DrawTextNDC(0.15,0.65,testo);  
  sprintf(testo,"S1_highthres = %7.3f",cal->S1_highthres);	 	 
  text->DrawTextNDC(0.15,0.60,testo);  
  sprintf(testo,"S2_lowthres = %7.3f",cal->S2_lowthres);	 
  text->DrawTextNDC(0.15,0.55,testo);  
  sprintf(testo,"S2_highthres = %7.3f",cal->S2_highthres);	 	 
  text->DrawTextNDC(0.15,0.50,testo);  
  sprintf(testo,"K_lowthres = %7.3f",cal->K_lowthres);	 
  text->DrawTextNDC(0.15,0.45,testo);  
  sprintf(testo,"K_highthres = %7.3f",cal->K_highthres);	 
  text->DrawTextNDC(0.15,0.40,testo);  
  sprintf(testo,"sigrawthres = %7.3f",cal->sigrawthres);	 
  text->DrawTextNDC(0.15,0.35,testo);  
  sprintf(testo,"calstatus = %d",cal->calstatus);	 
  text->DrawTextNDC(0.15,0.30,testo);  
  sprintf(testo,"Power_failureS = %d",cal->Power_failureS);
  text->DrawTextNDC(0.15,0.25,testo);  
  sprintf(testo,"Power_failureK = %d",cal->Power_failureK);
  text->DrawTextNDC(0.15,0.20,testo);  

  text->DrawTextNDC(0.45,0.85,"Calibration statistics:"); 	 		 
  sprintf(testo,"P-Side dead strips = %3d",dead_p);	
  text->DrawTextNDC(0.45,0.75,testo); 
  sprintf(testo,"N-Side dead strips = %3d",dead_n);	
  text->DrawTextNDC(0.45,0.70,testo); 
  sprintf(testo,"P-Side noisy strips = %3d",noisy_p);	
  text->DrawTextNDC(0.45,0.65,testo); 
  sprintf(testo,"N-Side noisy strips = %3d",noisy_n);	
  text->DrawTextNDC(0.45,0.60,testo); 
  sprintf(testo,"P-Side non-gaussian strips = %3d",gauss_p);
  text->DrawTextNDC(0.45,0.55,testo);
  sprintf(testo,"N-Side non-gaussian strips = %3d",gauss_n);
  text->DrawTextNDC(0.45,0.50,testo);
  sprintf(testo,"P-Side bad strips from 2nd pass = %3d",pass2_p);
  text->DrawTextNDC(0.45,0.45,testo);
  sprintf(testo,"N-Side bad strips from 2nd pass = %3d",pass2_n);
  text->DrawTextNDC(0.45,0.40,testo);
  sprintf(testo,"P-Side permanent strips = %3d",perm_p);
  text->DrawTextNDC(0.45,0.35,testo);
  sprintf(testo,"N-Side permanent trips = %3d",perm_n);
  text->DrawTextNDC(0.45,0.30,testo);

  canvas->cd(1);
  char namein[100];
  sprintf(namein,"Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
          lad->name,GetOctName(lad->GetOctant()),lad->GetCrate(),lad->GetTdr(),lad->GetLayer(),lad->GetSlot(),lad->GetSide());
  text->DrawTextNDC(0.1,0.91,namein);

  canvas->Update();
}


void CalSlider::SetHistStyle1(TH1* hh){
  hh->SetStats(kFALSE);
  hh->GetXaxis()->SetLabelSize(.05);
  hh->GetYaxis()->SetLabelSize(.05);
  hh->GetXaxis()->SetNdivisions(516,0);
  hh->GetYaxis()->SetTitleOffset(0.5);
  hh->GetYaxis()->SetTitleSize(0.06);
  hh->GetXaxis()->SetTitleSize(0.05);
  hh->SetLineStyle(1); 
  hh->SetFillStyle(0);
}

void CalSlider::SetHistStyle2(TH1* hh){
  hh->SetStats(kFALSE);
  hh->GetXaxis()->SetLabelSize(.05);
  hh->GetYaxis()->SetLabelSize(.05);
  hh->GetXaxis()->SetNdivisions(516,0);
  hh->GetYaxis()->SetTitleOffset(0.5);
  hh->GetYaxis()->SetTitleSize(0.06);
  hh->GetXaxis()->SetTitleSize(0.05);
  hh->SetMarkerStyle(20);
  hh->SetLineStyle(1); 
  hh->SetFillStyle(0);
}

void CalSlider::SetHistStyle3(TH1* hh){
  hh->SetStats(kFALSE);
  hh->GetXaxis()->SetLabelSize(.04);
  hh->GetYaxis()->SetLabelSize(.04);
  hh->GetXaxis()->SetNdivisions(508,0);
  hh->GetXaxis()->SetTitleSize(0.04);
  hh->GetYaxis()->SetTitleSize(0.04);
  hh->SetMarkerStyle(20);
  hh->SetLineStyle(1); 
  hh->SetFillStyle(0);
}

char * GetOctName(int oct){
  static char name[10];
  sprintf(name," ");
  if(oct==1) sprintf(name,"WAKE++");
  if(oct==1) sprintf(name,"WAKE-+");
  if(oct==3) sprintf(name,"RAM ++");
  if(oct==4) sprintf(name,"RAM -+");
  if(oct==5) sprintf(name,"WAKE+-");
  if(oct==6) sprintf(name,"WAKE--");
  if(oct==7) sprintf(name,"RAM +-");
  if(oct==8) sprintf(name,"RAM --");
  return name;
}




/* Draw second pass calibration and other informations (CMN, ...) */
void CalSlider::Draw4(const Option_t* aa,int flag){
  gROOT->cd();

  // Drawing settings
  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);  

  FillLadderHistos();

  Color_t col[5]={kGreen,kRed,kGreen,kBlue,kBlack};

  // Common noise
  TVirtualPad* cc0 = canvas->cd(1);  
  SetHistStyle2(Cmn);
  cc0->SetGrid(1,1);
  Cmn->Draw();
  
  // Sigma
  cc0 = canvas->cd(4);  
  SetHistStyle1(Sig);
  cc0->SetGrid(1,0);
  Sig->SetFillColor(col[4]);
  Sig->SetFillStyle(3004);
  Sig->GetYaxis()->SetRangeUser(-2,10);
  Sig->Draw("l"); // "l": trick to avoid white strips on the display - temporary(?)
  ST[2]->SetFillColor(col[2]);
  ST[2]->SetFillStyle(3004);
  ST[2]->SetLineColor(col[2]);
  ST[2]->Draw("same");
  ST[3]->SetFillColor(col[3]);
  ST[3]->SetFillStyle(3005);
  ST[3]->SetLineColor(col[3]);
  ST[3]->Draw("same");
  ST[4]->SetFillColor(col[1]);
  ST[4]->SetFillStyle(3005);
  ST[4]->SetLineColor(col[1]);
  ST[4]->Draw("same");
  ST[9]->SetFillColor(kViolet);
  ST[9]->SetLineColor(kViolet);
  ST[9]->SetFillStyle(3004);
  ST[9]->Draw("same");
  ST[15]->SetFillColor(kBlack);
  ST[15]->SetFillStyle(3004);
  ST[15]->Draw("same");

  // Sigma raw
  cc0 = canvas->cd(2);  
  SetHistStyle1(SigR);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  SigR->GetYaxis()->SetRangeUser(-2,18);
  SigR->Draw("l"); // "l": trick to avoid white strips on the display - temporary(?)
  ST[0]->SetFillColor(col[2]);
  ST[0]->SetFillStyle(3004);
  ST[0]->SetLineColor(col[2]);
  ST[0]->Draw("same");
  ST[1]->SetFillColor(col[3]);
  ST[1]->SetFillStyle(3005);
  ST[1]->SetLineColor(col[3]);
  ST[1]->Draw("same");
  ST[4]->SetFillColor(col[2]);
  ST[4]->SetFillStyle(3005);
  ST[4]->SetLineColor(col[2]);
  ST[4]->Draw("same");

  // Occupancy table (non-gaussianity step) 
  cc0 = canvas->cd(3);  
  SetHistStyle1(Occ);
  cc0->SetGrid(1,0);
  // Occ->GetYaxis()->SetRangeUser(-0.1,1.1);
  Occ->Draw("l"); // "l": trick to avoid white strips on the display - temporary(?)
  ST[4]->SetFillColor(col[1]);
  ST[4]->SetFillStyle(3005);
  ST[4]->SetLineColor(col[1]);
  ST[4]->Draw("same");
  ST[9]->SetFillColor(kViolet);
  ST[9]->SetLineColor(kViolet);
  ST[9]->SetFillStyle(3004);
  ST[9]->Draw("same");
  ST[15]->SetFillColor(kBlack);
  ST[15]->SetFillStyle(3004);
  ST[15]->Draw("same");

  canvas->cd(1);
  char namein[100];
  sprintf(namein,"Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
          lad->name,GetOctName(lad->GetOctant()),lad->GetCrate(),lad->GetTdr(),lad->GetLayer(),lad->GetSlot(),lad->GetSide());
  text->DrawTextNDC(0.1,0.91,namein);

  canvas->Update();
}


/* find the geographic position of a ladder and return the hwid of the current geometry database */
void CalSlider::Draw7(const Option_t* aa,int flag){
  gROOT->cd();

  // Drawing settings
  canvas->Draw(aa);
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);

  FillLadderHistos();

  if (reffile==NULL) {
    printf("WARNING: No Reference Calibration File Selected!!!\n");
    return;
  }

  // Save CalDB
  TrCalDB* calsave = caldb;

  // Save pedestal array
  float pedestal[1024];
  for (int ii=0;ii<1024;ii++) pedestal[ii] = cal->Pedestal(ii);
  Int_t currenthwid = cal->GetHwId();
  Int_t currenttkid = ((TkLadder*)TkDBc::Head->FindHwId(currenthwid))->GetTkId(); 

  // Reference calibration  
  caldb = (TrCalDB*) reffile->Get("TrCalDB");

  // Find the TkId of the ladders
  Int_t    ncha[2]    = {  384,   640};
  Int_t    tkid[2]    = {    0,     0};
  Int_t    oldhwid[2] = {    0,     0};
  Double_t chi2min[2] = {1e+50, 1e+50};
  for (int iside=0; iside<2; iside++) {
    for (int ientry=0; ientry<caldb->GetEntries(); ientry++) {      
      TrLadCal* calref = caldb->GetEntry(ientry);
      Double_t  chi2 = 0;
      int       rej = 0;
      for (int ichan=0; ichan<ncha[iside]; ichan++) {
        int ichan2 = (1-iside)*ncha[iside] + ichan;
        if (calref->GetPedestal(ichan2)<1.) {
          rej++; 
          continue;
        }
	chi2 += TMath::Min(1e+6,pow(calref->GetPedestal(ichan2)-pedestal[ichan2],2)/(calref->GetPedestal(ichan2)+0.0001)); // avoid errors
      }
      chi2 /= (ncha[iside]-rej);
      // printf("side=%1d oldhwid=%03d oldtkid=%+4d chi2=%f\n",iside,calref->GetHwId(),((TkLadder*)TkDBc::Head->FindHwId(calref->GetHwId()))->GetTkId(),chi2);
      if (chi2<chi2min[iside]) { 
	chi2min[iside] = chi2;
 	oldhwid[iside] = calref->GetHwId();
	tkid[iside] = ((TkLadder*)TkDBc::Head->FindHwId(calref->GetHwId()))->GetTkId(); 
      }
    }
  }

  // if ( (oldhwid[0]==oldhwid[1])&&(currenthwid!=oldhwid[0]) ) printf("|%s|%03d|%03d|%+04d||\n",TkDBc::Head->FindTkId(tkid[0])->name,currenthwid,oldhwid[0],tkid[0]); 

  // Difference histos
  TH1F* PedOther = (TH1F*) Ped->Clone("PedOther");
  TH1F* SigRNeg  = (TH1F*) SigR->Clone("SigmaRawDiff");
  TH1F* SigRDiff = (TH1F*) SigR->Clone("SigmaRawNeg");
  TH1F* SigNeg   = (TH1F*) Sig->Clone("SigmaNeg");
  TH1F* SigDiff  = (TH1F*) Sig->Clone("SigmaDiff");
  for (int ientry=0; ientry<caldb->GetEntries(); ientry++) {      
    TrLadCal* calref = caldb->FindCal_HwId(oldhwid[1]);
    for (int ii=0; ii<640; ii++) { 
      PedOther->SetBinContent(ii+1,calref->Pedestal(ii));
      SigNeg->SetBinContent(ii+1,-calref->Sigma(ii));
      SigDiff->SetBinContent(ii+1,calref->Sigma(ii) - Sig->GetBinContent(ii+1));
      SigRNeg->SetBinContent(ii+1,-calref->SigmaRaw(ii));
      SigRDiff->SetBinContent(ii+1,calref->SigmaRaw(ii) - SigR->GetBinContent(ii+1));
    }
    calref = caldb->FindCal_HwId(oldhwid[0]);
    for (int ii=640; ii<1024; ii++) { 
      PedOther->SetBinContent(ii+1,calref->Pedestal(ii));
      SigNeg->SetBinContent(ii+1,-calref->Sigma(ii));
      SigDiff->SetBinContent(ii+1,calref->Sigma(ii) - Sig->GetBinContent(ii+1));
      SigRNeg->SetBinContent(ii+1,-calref->SigmaRaw(ii));
      SigRDiff->SetBinContent(ii+1,calref->SigmaRaw(ii) - SigR->GetBinContent(ii+1));
    }
  }
  Color_t col[5]={kGreen,kRed,kGreen,kBlue,kBlack};
  Double_t xmin,ymin,xmax,ymax;
  TLine *line1=new TLine();
  line1->SetLineColor(kBlue);
  line1->SetLineStyle(5);
  TLine *line2=new TLine();
  line2->SetLineColor(kRed);
  line2->SetLineStyle(5);

  // Draw text
  canvas->cd(1);
  char testo[100];
  if (text==0) text = new TText();
  text->SetTextColor(kBlack);
  sprintf(testo,"Tracker Cabling Used: %s",TkDBc::Head->GetSetupName());
  text->DrawTextNDC(0.15,0.85,testo); 	 		 
  sprintf(testo,"Current HwId: %03d (old-TkId: %+03d)",currenthwid,currenttkid);
  text->DrawTextNDC(0.15,0.75,testo); 	 		 
  sprintf(testo,"N-Side found TkId: %+03d (old-HwId: %03d)",tkid[0],oldhwid[0]);
  text->DrawTextNDC(0.15,0.65,testo); 	 		 
  sprintf(testo,"P-Side found TkId: %+03d (old-HwId: %03d)",tkid[1],oldhwid[1]);
  text->DrawTextNDC(0.15,0.55,testo); 	 		 
  TkLadder* lad0 = TkDBc::Head->FindTkId(tkid[0]);
  TkLadder* lad1 = TkDBc::Head->FindTkId(tkid[1]);
  sprintf(testo,"Ladder %s (%s) is connected to HwId: %03d",lad0->name,lad1->name,currenthwid);
  text->DrawTextNDC(0.15,0.45,testo);
  if (currenthwid!=oldhwid[0]) {
    sprintf(testo,"N-Side: HwId Change for TkId %+03d from %03d to %03d",tkid[0],oldhwid[0],currenthwid);
    text->SetTextColor(kRed);
    text->DrawTextNDC(0.15,0.35,testo); 	 		 
  }
  if (currenthwid!=oldhwid[1]) {
    text->SetTextColor(kRed);
    sprintf(testo,"P-Side: HwId Change for TkId %+03d from %03d to %03d",tkid[1],oldhwid[1],currenthwid);
    if (currenthwid!=oldhwid[0]) text->DrawTextNDC(0.15,0.25,testo); 	 		 
    else                         text->DrawTextNDC(0.15,0.35,testo);               
  }
  text->SetTextColor(kBlack);

  // Drawing pedestals
  TVirtualPad* cc0 = canvas->cd(3);
  SetHistStyle1(Ped);
  cc0->SetGrid(1,0);
  Ped->Draw("l"); 
  SetHistStyle1(PedOther);
  PedOther->SetLineColor(kRed);
  PedOther->Draw("l SAME"); 
 
  // Drawing sigma  
  cc0 = canvas->cd(4);
  SetHistStyle1(Sig);
  SetHistStyle1(SigNeg);
  cc0->SetGrid(1,0);
  Sig->SetFillStyle(3004);
  Sig->SetFillColor(col[4]);
  Sig->GetYaxis()->SetRangeUser(-10,10);
  Sig->Draw("l");
  SigNeg->SetFillStyle(3004);
  SigNeg->SetFillColor(col[1]);
  SigNeg->GetYaxis()->SetRangeUser(-10,10);
  SigNeg->Draw("l SAME");
  gPad->Update();
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  line1->SetLineStyle(1);
  line1->SetLineColor(kMagenta);
  line2->SetLineStyle(1);
  line2->SetLineColor(kMagenta);
  for (int i=0; i<640; i++) {
    if (cal->Status(i)!=0 && caldb->FindCal_HwId(oldhwid[1])->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->Sigma(i))?cal->Sigma(i):ymax);
    if (caldb->FindCal_HwId(oldhwid[1])->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-caldb->FindCal_HwId(oldhwid[1])->Sigma(i))?-caldb->FindCal_HwId(oldhwid[1])->Sigma(i):ymin);
  }
  for (int i=640; i<1024; i++) {
    if (cal->Status(i)!=0 && caldb->FindCal_HwId(oldhwid[1])->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->Sigma(i))?cal->Sigma(i):ymax);
    if (caldb->FindCal_HwId(oldhwid[0])->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-caldb->FindCal_HwId(oldhwid[0])->Sigma(i))?-caldb->FindCal_HwId(oldhwid[0])->Sigma(i):ymin);
  }


  // Drawing sigma raw   
  cc0 = canvas->cd(2);
  SetHistStyle1(SigR);
  SetHistStyle1(SigRNeg);
  cc0->SetGrid(1,0);
  SigR->SetFillColor(col[4]);
  SigR->SetFillStyle(3004);
  SigR->GetYaxis()->SetRangeUser(-18,18);
  SigR->Draw("l");
  SigRNeg->SetFillColor(col[1]);
  SigRNeg->SetFillStyle(3004);
  SigRNeg->GetYaxis()->SetRangeUser(-18,18);
  SigRNeg->Draw("l SAME");
  gPad->Update();
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  for (int i=0; i<640; i++) {
    if (cal->Status(i)!=0 && caldb->FindCal_HwId(oldhwid[1])->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->SigmaRaw(i))?cal->SigmaRaw(i):ymax);
    if (caldb->FindCal_HwId(oldhwid[1])->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-caldb->FindCal_HwId(oldhwid[1])->SigmaRaw(i))?-caldb->FindCal_HwId(oldhwid[1])->SigmaRaw(i):ymin);
  }
  for (int i=640; i<1024; i++) {
    if (cal->Status(i)!=0 && caldb->FindCal_HwId(oldhwid[1])->Status(i)==0) line1->DrawLine(i+0.5,0,i+0.5,(ymax>cal->SigmaRaw(i))?cal->SigmaRaw(i):ymax);
    if (caldb->FindCal_HwId(oldhwid[0])->Status(i)!=0 && cal->Status(i)==0) line2->DrawLine(i+0.5,0,i+0.5,(ymin<-caldb->FindCal_HwId(oldhwid[0])->SigmaRaw(i))?-caldb->FindCal_HwId(oldhwid[0])->SigmaRaw(i):ymin);
  }

  // Drawing 
  canvas->Update();

  // CalDB reset 
  caldb = calsave;
}

void CalSlider::Draw8(const Option_t *aa,int flag) {
  gROOT->cd();
  printf("DrawFindSummary\n");
  // find 
  //

}

#include "timeid.h"

int CalSlider::OpenTDV(){
  char * dd=getenv("AMSDataDir");
  char nn[100];
  sprintf(nn,"%s/DataBase/",dd);
  AMSDBc::amsdatabase=new char[strlen(nn)+1];
  strcpy(AMSDBc::amsdatabase,nn);
  if(TrCalDB::Head==0){
    TrCalDB* db= new TrCalDB();
    db->init();
  }
  TrCalDB::Head->CreateLinear();
  TrCalDB::Head->Clear();

  tm begin;
  tm end;
  begin.tm_isdst=0;
  end.tm_isdst=0;    
  begin.tm_sec  =0;
  begin.tm_min  =0;
  begin.tm_hour =0;
  begin.tm_mday =0;
  begin.tm_mon  =0;
  begin.tm_year =0;
  
  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;
  int need=1;
  AMSTimeID::CType server=AMSTimeID::Standalone;
  TDV= new AMSTimeID(AMSID("TrackerCals",1),begin,end,
                               TrCalDB::GetLinearSize(),
                               TrCalDB::linear,
                               server,need,SLin2CalDB);
 
  return 1;
}
int CalSlider::ReadCalFromDB(time_t run){
  if (!TDV) OpenTDV();
  time_t bb=run+10;
  TDV->validate(bb);
  char title[140];
  time_t pippo = (time_t)caldb->GetRun();
  sprintf(title,"Calibration %10d %s",caldb->GetRun(),ctime(&pippo));
  canvas->SetTitle(title);

}
