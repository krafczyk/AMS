#include "MonSlider.h"

ClassImp(MonSlider);


MonSlider::MonSlider(char *name,char *title,int xsize,int ysize) : SliderI(name,title,xsize,ysize){
  canvas->SetFillColor(0);
  rootfile = 0;
  reffile  = 0;  
  graphtype = 1;
  label = 0;
  BuildMenu();
  ladder = TkDBc::Head->FindHwId(0);
}


MonSlider::~MonSlider() {
  if (rootfile!=0) rootfile->Close();
  rootfile = 0;
  if (reffile!=0) reffile->Close();
  reffile = 0;
  graphtype = 0;
  ladder = 0;
  if (label!=0) label->Delete();
  label = 0;
  if (graphmenu!=0) delete graphmenu;
  graphmenu = 0;
}


void MonSlider::BuildMenu() {
  frame->SetWindowName("DataMenu");
  graphmenu = new TGListBox(frame,10);
  graphmenu->AddEntry("LadderSummary",1);
  graphmenu->AddEntry("SizeSummary",2);
  graphmenu->AddEntry("DtSummary",3);
  graphmenu->AddEntry("CrateVsDt",4);
  graphmenu->AddEntry("Ladder",5);
  graphmenu->AddEntry("ReconStats",6);
  graphmenu->AddEntry("Track",7);
  graphmenu->AddEntry("HitsOnTrack",8);
  graphmenu->AddEntry("Rigidity",9);
  graphmenu->AddEntry("SeedOccupancyOnLayer",10);
  graphmenu->AddEntry("SeedOccupancyGlobal",11);
  graphmenu->AddEntry("ClustersSummary",12);
  graphmenu->Select(graphtype);
  graphmenu->Resize(180,220);
  frame->AddFrame(graphmenu,new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
  frame->MapSubwindows();
  frame->MoveResize(1200,800);
  frame->MapWindow();
  graphmenu->Connect("Selected(Int_t,Int_t)","MonSlider",this,"selectGraph(Int_t,Int_t)");
}


void MonSlider::selectGraph(Int_t w, Int_t id){
  graphtype = id;
  Draw();
}


void MonSlider::setRootFile(char *filename){
  // closing
  if (rootfile!=0) { 
    rootfile->Close(); 
    rootfile = 0; 
  }
  // open
  rootfile = new TFile(filename,"read");
  if (rootfile->IsZombie()) {
    printf("MonSlider::setRootFile-W %s is not a valid filename.\n",filename);
    return; 
  }
  if (rootfile->FindObjectAny("TrOnlineMonHeader")!=0) {
    TrHistoManHeader* header = (TrHistoManHeader*) rootfile->FindObjectAny("TrOnlineMonHeader"); 
    if (header->GetNRunNumber()>0) { 
      char title[100];
      time_t pippo = (time_t) header->GetRunNumber(0);
      sprintf(title,"Data from run %10d %s",header->GetRunNumber(0),ctime(&pippo));
      canvas->SetTitle(title);
    }
  }
  try2Draw(ladder);
}


void MonSlider::Update() {
  // simple check
  if (rootfile==0) {
    printf("MonSlider::Update-W you must select a file before updating it!\n");
    return;
  }
  // re-open
  char oldfilename[200];
  sprintf(oldfilename,"%s",rootfile->GetName());
  setRootFile(oldfilename);
}


TH1* MonSlider::GetHistoFromFile(TFile* file, char* name) {
  if (file==0) return 0;
  file->cd();
  TObject* obj = file->FindObjectAny(name);
  // cout << "calling GetHistoFromFile: " << name << endl;
  if (obj==0) { 
    // printf("MonSlider::GetHistoFromFile-W the requested histogram (%s) doesn't exist, returning 0.\n",name);
    return 0;
  }
  if (!obj->InheritsFrom("TH1")) {
    printf("MonSlider::GetHistoFromFile-W the object requested (%s) is not an histogram, returning 0.\n",name);
    return 0; 
  }
  return (TH1*) obj;
}


TH1* MonSlider::GetHistoFromMemory(char* name) {
  gROOT->cd();
  TObject* obj = gROOT->FindObjectAny(name);
  // cout << "calling GetHistoFromMemory: " << name << endl;
  if (obj==0) {  
    // printf("MonSlider::GetHistoFromMemory-W the requested histogram (%s) doesn't exist, returning 0.\n",name);
    return 0;
  }
  if (!obj->InheritsFrom("TH1")) {
    printf("MonSlider::GetHistoFromMemory-W the object requested (%s) is not an histogram, returning 0.\n",name);
    return 0;
  }
  return (TH1*) obj;
}


TH1* MonSlider::GetHisto(TFile* file, char* name) {
  TH1* histo = GetHistoFromFile(file,name);
  TH1* newhisto = 0;
  if (histo!=0) { 
    // release memory 
    ClearHistoFromMemory(name);
    // write the object in a safe place 
    gROOT->cd();
    newhisto = (TH1*) histo->Clone(Form("%s_saved",name));
    // histo->SetDirectory(0);
    // newhisto = histo;
  }
  else {
    // search in memory
    newhisto = GetHistoFromMemory(name); 
  }
  return newhisto;
}


void MonSlider::ClearHistoFromMemory(char* name) {
  gROOT->cd();
  TObject* obj = gROOT->FindObjectAny(name);
  if (obj==0) {
    // printf("MonSlider::ClearHistoFromMemory-V the requested histogram (%s) doesn't exist, skipping.\n",name);
    return;
  }
  obj->Delete(); 
  obj = 0; 
  return;
}


TProfile* MonSlider::GetProfileX(TH2D* histo) {
  if (histo==0) { 
    printf("MonSlider::GetProfileX-W the requested histogram (%s) doesn't exist, returning 0.\n",histo->GetName());
    return 0;
  }
  // release memory
  ClearHistoFromMemory(Form("%s_prof",histo->GetName()));
  // write the object in a safe place
  gROOT->cd();
  TProfile* profile = (TProfile*) histo->ProfileX(Form("%s_prof",histo->GetName()));
  return profile;
}


void MonSlider::showNext(){
  if(rootfile==NULL) return;
  int entry = ladder->GetCrate()*24 + ladder->GetTdr();
  entry++;
  if (entry>191) entry = 0;
  int hwid = int(entry/24)*100 + entry%24;
  ladder = TkDBc::Head->FindHwId(hwid);
  try2Draw(ladder);
}


void MonSlider::showPrev(){
  if(rootfile==NULL) return;
  int entry = ladder->GetCrate()*24 + ladder->GetTdr();
  entry--;
  if (entry<0) entry = 191;
  int hwid = int(entry/24)*100 + entry%24;
  ladder = TkDBc::Head->FindHwId(hwid);
  try2Draw(ladder);
}


void MonSlider::PrintThis(char* name){
  char filename[200];
  sprintf(filename,"%s_%s.png",rootfile->GetName(),name);
  printf("Writing plot %s\n",filename);
  canvas->Print(filename);
}


void MonSlider::PrintThis(){
  char filename[200];
  sprintf(filename,"%s.png",rootfile->GetName());
  printf("Writing plot %s\n",filename);
  canvas->Print(filename);
}


void MonSlider::PrintAll(){
  canvas->Print("Mon_All.ps[","ps");
  canvas->Print("Mon_All.ps]","ps");
}


int MonSlider::try2Draw(TkLadder *lad){
  if (lad) {
    ladder = lad; 
    Draw();
    return 0;
  } 
  else {
    canvas->Clear();
    if(!label) label = new TPaveLabel(0.3,0.35,0.7,0.65,"Not found");
    label->SetFillColor(0);
    label->Draw();
    canvas->Update();
  }
  return 1;
}


void MonSlider::Draw() {
  switch(graphtype) {
  case 1:
    DrawSignalSummary();
    break;
  case 2:
    DrawSizeSummary();
    break;
  case 3:
    DrawDtSummary();
    break;    
  case 4:
    DrawCrateVsDt();
    break;
  case 5:
    DrawLadder();
    break;   
  case 6:
    DrawReconStats();
    break;
  case 7:
    DrawTrack();
    break;
  case 8:
    DrawHitsOnTrack();
    break;
  case 9:
    DrawRigidity();
    break;
  case 10:
    DrawSeedOccupancyOnLayer();
    break;
  case 11:
    DrawSeedOccupancyGlobal();
    break;
  case 12:
    DrawClustersSummary();
    break;
  }
}


void MonSlider::DrawSignalSummary() {
  // Prepare canvas
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(2,1,0.001,0.001);  
  TText* text = new TText();
  // Signal summary
  TH2D* signal_n = (TH2D*) GetHisto(rootfile,"Signal_vs_Ladder_all_N");
  TH2D* signal_p = (TH2D*) GetHisto(rootfile,"Signal_vs_Ladder_all_P");
  if ( (signal_n==0)||(signal_p==0) ) { canvas->Update(); return; }
  TProfile* signal_n_prof = (TProfile*) GetProfileX(signal_n);;
  TProfile* signal_p_prof = (TProfile*) GetProfileX(signal_p);
  if ( (signal_n_prof==0)||(signal_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,signal_n_prof);
  SetHistSideStyle(1,signal_p_prof);
  // Draw
  TVirtualPad* cc = canvas->cd(1);
  cc->SetGridy();
  cc->SetGridx();
  TH1F* frame = cc->DrawFrame(0.,0.,192.,100.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Average Amplitude (ADC)");
  frame->SetXTitle("iCrate*24 + iTDR");
  SetHistSideStyle(0,frame);
  signal_n_prof->Draw("SAME");
  signal_p_prof->Draw("SAME");
  // Test
  char testo[100];
  for (int ii=1; ii<=signal_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId(); 
    float mean_p = signal_p_prof->GetBinContent(ii);
    float mean_n = signal_n_prof->GetBinContent(ii);
    if ( (mean_n< 5.)||(mean_n>40.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if ( (mean_p< 5.)||(mean_p>40.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }
  // Width Summary (all ladders)
  TH2D* width_n = (TH2D*) GetHisto(rootfile,"Width_vs_Ladder_all_N");
  TH2D* width_p = (TH2D*) GetHisto(rootfile,"Width_vs_Ladder_all_P");
  if ( (width_n==0)||(width_p==0) ) { canvas->Update(); return; }
  TProfile* width_n_prof = (TProfile*) GetProfileX(width_n);
  TProfile* width_p_prof = (TProfile*) GetProfileX(width_p);
  if ( (width_n_prof==0)||(width_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,width_n_prof);
  SetHistSideStyle(1,width_p_prof);
  // Draw
  cc = canvas->cd(2);
  cc->SetLogy(kFALSE);
  cc->SetGridy();
  cc->SetGridx();
  frame = cc->DrawFrame(0.,0.,192.,10.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Average Cluster Width");
  frame->SetXTitle("iCrate*24 + iTDR");
  SetHistSideStyle(0,frame);
  width_n_prof->Draw("SAME");
  width_p_prof->Draw("SAME");
  // Test
  for (int ii=1; ii<=width_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = width_p_prof->GetBinContent(ii);
    float mean_n = width_n_prof->GetBinContent(ii);
    if ( (mean_n< 1.)||(mean_n>4.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if ( (mean_p< 1.)||(mean_p>4.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }
  canvas->Update();
}


void MonSlider::DrawSizeSummary() {
  // prepare canvas
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(1,2,0.001,0.001);  
  TVirtualPad* cc = canvas->cd(1);
  cc->Divide(2,1,0.001,0.001);  
  TLatex* text = new TLatex();
  char testo[100];
  // Tracker size  
  TH2D* size = (TH2D*) GetHisto(rootfile,"Size_all");
  if (size==0) { canvas->Update(); return; }
  size->GetXaxis()->SetRangeUser(0.,3000.); 
  TVirtualPad* cc1 = cc->cd(1);
  cc1->SetLogy();
  cc1->SetGridx();
  cc1->SetGridy();
  size->SetStats(kFALSE);
  size->Draw("HIST");
  // Crate size   
  TH2D* sizevscrate = (TH2D*) GetHisto(rootfile,"Size_vs_Crate_all");
  TProfile* sizevscrate_prof = (TProfile*) GetProfileX(sizevscrate);
  if ( (sizevscrate==0)||(sizevscrate_prof==0) ) { canvas->Update(); return; }
  cc1 = cc->cd(2);
  cc1->SetGridy();
  TH1F* frame = cc1->DrawFrame(-0.5,0.,7.5,500.);
  frame->SetYTitle("Crate Segment Size (byte)");
  frame->SetXTitle("Crate");
  sizevscrate_prof->Draw("SAME");
  // Event Size Summary (all ladders separated by side)
  TH2D* size_n = (TH2D*) GetHisto(rootfile,"Size_vs_Ladder_all_N");
  TH2D* size_p = (TH2D*) GetHisto(rootfile,"Size_vs_Ladder_all_P");
  if ( (size_n==0)||(size_p==0) ) { canvas->Update(); return; }
  TProfile* size_n_prof = (TProfile*) GetProfileX(size_n);
  TProfile* size_p_prof = (TProfile*) GetProfileX(size_p);
  if ( (size_n_prof==0)||(size_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,size_n_prof);
  SetHistSideStyle(1,size_p_prof);
  cc1 = canvas->cd(2);
  cc1->SetGridy();
  frame = cc->DrawFrame(0.,0.,192.,70.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Approx. Mean Ladder Size (byte)");
  frame->SetXTitle("iCrate*24 + iTDR");
  size_n_prof->Draw("SAME");
  size_p_prof->Draw("SAME");
  for (int ii=1; ii<=size_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = size_p_prof->GetBinContent(ii);
    float mean_n = size_n_prof->GetBinContent(ii);
    if ( (mean_n< 2.)||(mean_n>6.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawLatex(ii,mean_n,testo);
    }
    if ( (mean_p< 2.)||(mean_p>6.) ) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawLatex(ii,mean_p,testo);
    }
  }
  canvas->Update();
}


void MonSlider::DrawDtSummary() {
  char testo[100];
  TText* text = new TText();
  // prepare canvas
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(1,2,0.001,0.001);  
  TVirtualPad* canvas2 = canvas->cd(1);
  canvas2->Divide(2,1,0.001,0.001);
  // Delta t
  TVirtualPad* cc = canvas2->cd(1);
  TH1D* dt = (TH1D*) GetHisto(rootfile,"DT_all");
  if (dt==0) { canvas->Update(); return; }
  dt->Rebin(8);
  cc->SetLogy();
  cc->SetGridx();
  cc->SetGridy();
  TF1 *f1 = new TF1("f1","expo",800,5000);
  dt->Fit("f1","QR");
  float inrate = -1000.*dt->GetFunction("f1")->GetParameter(1);
  float trrate = 1000./dt->GetMean();
  dt->SetStats(kFALSE);
  dt->Draw("l");
  sprintf(testo,"Est. Event Rate = %7.3f Hz",inrate*1000.);
  text->DrawTextNDC(0.5,0.80,testo);
  sprintf(testo,"Est. Trigger Rate = %7.3f Hz",trrate*1000.);
  text->DrawTextNDC(0.5,0.75,testo);
  sprintf(testo,"Est. Lifetime = %7.5f",trrate/inrate);
  text->DrawTextNDC(0.5,0.70,testo);
  // Size vs Dt
  cc = canvas2->cd(2);
  cc->SetGridx();
  cc->SetGridy();  
  TH2D* sizeVSdt = (TH2D*) GetHisto(rootfile,"Size_vs_DT_all");
  TProfile* sizeVSdt_prof = (TProfile*) GetProfileX(sizeVSdt);
  if ( (sizeVSdt==0)||(sizeVSdt_prof==0) ) { canvas->Update(); return; }
  sizeVSdt_prof->SetStats(kFALSE);
  sizeVSdt_prof->Draw();
  // Ladder sizes at low Dt
  // Event Size Summary (all ladders separated by side)
  TH2D* sizelowdt_n = (TH2D*) GetHisto(rootfile,"SizeLowDT_vs_Ladder_all_N");
  TH2D* sizelowdt_p = (TH2D*) GetHisto(rootfile,"SizeLowDT_vs_Ladder_all_P");
  if ( (sizelowdt_n==0)||(sizelowdt_p==0) ) { canvas->Update(); return; }
  TProfile* sizelowdt_n_prof = (TProfile*) GetProfileX(sizelowdt_n);
  TProfile* sizelowdt_p_prof = (TProfile*) GetProfileX(sizelowdt_p);
  if ( (sizelowdt_n_prof==0)||(sizelowdt_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,sizelowdt_n_prof);
  SetHistSideStyle(1,sizelowdt_p_prof);
  cc = canvas->cd(2);
  cc->SetGridy();
  TH1F* frame = cc->DrawFrame(0.,0.,192.,250.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Approx. Mean Ladder Size @ #Deltat<200 #mus (byte)");
  frame->SetXTitle("iCrate*24 + iTDR");
  sizelowdt_n_prof->Draw("SAME");
  sizelowdt_p_prof->Draw("SAME");
  for (int ii=1; ii<=sizelowdt_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = sizelowdt_p_prof->GetBinContent(ii);
    float mean_n = sizelowdt_n_prof->GetBinContent(ii);
    if (mean_n>100.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if (mean_p>100.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }
  canvas->Update();
}


void MonSlider::DrawCrateVsDt() {
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(4,2,0.001,0.001);
  TText* text = new TText();
  TVirtualPad* cc;
  char histoname[100];
  TH2D* cratevsdt[8]; 
  for (int icrate=0; icrate<8; icrate++) { 
    sprintf(histoname,"T%1dSize_vs_DT_all",icrate);
    cratevsdt[icrate] = (TH2D*) GetHisto(rootfile,histoname);
    sprintf(histoname,"T%1dSize_vs_DT_all_prof",icrate);
    TProfile* cratevsdt_prof = (TProfile*) GetProfileX(cratevsdt[icrate]);
    cc = canvas->cd(icrate+1);
    cc->SetGridx();
    cc->SetGridy();
    cratevsdt[icrate]->GetYaxis()->SetRangeUser(0.,5000.);
    cratevsdt[icrate]->SetStats(kFALSE);
    cratevsdt[icrate]->SetMarkerStyle(20);
    cratevsdt[icrate]->SetMarkerColor(kGray);
    cratevsdt[icrate]->Draw();
    cratevsdt_prof->SetMarkerStyle(1);
    cratevsdt_prof->SetMarkerColor(kBlack);
    cratevsdt_prof->Draw("SAME");   
    sprintf(histoname,"T%1d",icrate);
    text->DrawTextNDC(0.8,0.8,histoname);
  }
  canvas->Update();
}


void MonSlider::DrawLadder() {
  TText* text = new TText();
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(2,2,0.001,0.001);  
  int tkid  = ladder->GetTkId();
  // Signal
  TVirtualPad* cc = canvas->cd(1);
  cc->SetGridx();
  cc->SetLogy();
  cc->SetGridy();
  TH1D* signal_n = (TH1D*) GetHisto(rootfile,Form("Signal_%+04d_N",tkid));
  TH1D* signal_p = (TH1D*) GetHisto(rootfile,Form("Signal_%+04d_P",tkid));
  if ( (signal_n==0)||(signal_p==0) ) { canvas->Update(); return; }  
  signal_n->SetStats(kFALSE);
  signal_n->SetLineWidth(2);
  signal_n->SetLineColor(kRed);
  signal_p->SetStats(kFALSE);
  signal_p->SetLineWidth(2);
  signal_p->SetLineColor(kBlue);
  signal_p->Draw("HIST");
  signal_n->Draw("HIST SAME");
  // Width
  cc = canvas->cd(2);
  cc->SetGridx();
  cc->SetLogy();
  cc->SetGridy();
  TH1D* width_n = (TH1D*) GetHisto(rootfile,Form("Width_%+04d_N",tkid));
  TH1D* width_p = (TH1D*) GetHisto(rootfile,Form("Width_%+04d_P",tkid));
  if ( (width_n==0)||(width_p==0) ) { canvas->Update(); return; }
  width_n->SetStats(kFALSE);
  width_n->SetLineWidth(2);
  width_n->SetLineColor(kRed);
  width_p->SetStats(kFALSE);
  width_p->SetLineWidth(2);
  width_p->SetLineColor(kBlue);
  width_p->Draw("HIST");
  width_n->Draw("HIST SAME");
  // Occupancy
  cc = canvas->cd(3);
  cc->SetGridx();
  cc->SetGridy();
  cc->SetLogy(kFALSE);
  TH1D* occupancy = (TH1D*) GetHisto(rootfile,Form("SeedAddress_%+04d",tkid));
  TH1D* rawcluste = (TH1D*) GetHisto(rootfile,"nRawClusters_all");
  if ( (occupancy==0)||(rawcluste==0) ) { canvas->Update(); return; }
  int nentries = (int) rawcluste->GetEntries();
  occupancy->Scale(100./nentries); 
  // occupancy->SetFillColor(kRed);
  occupancy->SetStats(kFALSE);
  occupancy->GetXaxis()->SetNdivisions(516,kFALSE);
  occupancy->SetYTitle("Occupancy (%)");
  occupancy->SetLineWidth(2);
  occupancy->Draw("l"); 
  // Size
  cc = canvas->cd(4);
  cc->SetGridx();
  cc->SetLogy();
  cc->SetGridy();
  TH1D* size = (TH1D*) GetHisto(rootfile,Form("Size_%+04d",tkid));
  if (size==0) { canvas->Update(); return; }
  size->SetLineWidth(2);
  size->SetStats(kFALSE);
  size->GetXaxis()->SetRangeUser(7.,400);
  size->Draw("HIST"); 
  canvas->cd(1);
  // Name
  char namein[100];
  sprintf(namein,"Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
          ladder->name,GetOctName(ladder->GetOctant()),ladder->GetCrate(),ladder->GetTdr(),ladder->GetLayer(),ladder->GetSlot(),ladder->GetSide());
  text->DrawTextNDC(0.1,0.91,namein);
  canvas->Update();  
}


void MonSlider::DrawReconStats() {
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(3,2,0.001,0.001);
  TVirtualPad* cc0 = canvas->cd(1); 
  cc0->SetLogy();
  TH1D* raw = (TH1D*) GetHisto(rootfile,"nRawClusters_all");
  if (raw==0) { canvas->Update(); return; }
  raw->Draw("HIST");
  cc0 = canvas->cd(2);
  cc0->SetLogy();
  TH1D* clu = (TH1D*) GetHisto(rootfile,"nClusters_all");
  if (clu==0) { canvas->Update(); return; }
  clu->Draw("HIST");
  cc0 = canvas->cd(4);
  cc0->SetLogy();
  TH1D* hit = (TH1D*) GetHisto(rootfile,"nRecHits_all");
  if (hit==0) { canvas->Update(); return; }
  hit->Draw("HIST");
  cc0 = canvas->cd(5);
  cc0->SetLogy();
  TH1D* trk = (TH1D*) GetHisto(rootfile,"nTracks_all");		
  if (trk==0) { canvas->Update(); return; }
  trk->Draw("HIST");
  cc0 = canvas->cd(3);
  cc0->SetLogy();
  TH1D* cluontrkn = (TH1D*) GetHisto(rootfile,"nClustersOnTrack_all_N");
  TH1D* cluontrkp = (TH1D*) GetHisto(rootfile,"nClustersOnTrack_all_P");
  if ( (cluontrkn==0)||(cluontrkp==0) ) { canvas->Update(); return; } 
  cluontrkp->SetLineColor(kBlue);
  cluontrkn->SetLineColor(kRed);
  cluontrkp->Draw("HIST");
  cluontrkn->Draw("HIST SAME");
  cc0 = canvas->cd(6);
  cc0->SetLogy();
  TH1D* hitontrk = (TH1D*) GetHisto(rootfile,"nRecHitsOnTrack_all");
  hitontrk->Draw("HIST");
  canvas->Update();
}


void MonSlider::DrawHitsOnTrack() {
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(3,3,0.001,0.001);
  TText* text = new TText();
  int order[9] = {2,3,4,5,6,7,8,1,9};
  TVirtualPad* cc;
  TH2D* occupancy[9];
  for (int tt=0; tt<TkDBc::Head->nlay(); tt++) {
    occupancy[tt] = (TH2D*) GetHisto(rootfile,Form("Occupancy_layer%1d",tt+1));
    if (occupancy[tt]==0) { canvas->Update(); return; }
    if (TkDBc::Head->GetSetup()==3) cc = canvas->cd(order[tt]);
    else                            cc = canvas->cd(tt+1);
    cc->SetGridx();
    cc->SetGridy();
    occupancy[tt]->SetStats(kFALSE);
    occupancy[tt]->Draw("COLZ");
    text->DrawTextNDC(0.10,0.91,Form("Layer %1d",tt+1));
    canvas->Update();
  }
  canvas->Update();
}


void MonSlider::SetHistSideStyle(int side, TH1* histo) {
  int col[2] = {kRed, kBlue}; 
  int mark[2] = {20,21};
  histo->GetXaxis()->SetLabelSize(.04);
  histo->GetYaxis()->SetLabelSize(.04);
  histo->GetYaxis()->SetTitleOffset(1.3);
  histo->GetYaxis()->SetTitleSize(0.04);
  histo->GetXaxis()->SetTitleSize(0.04);
  histo->SetMarkerStyle(mark[side]);
  histo->SetMarkerColor(col[side]);
  histo->GetXaxis()->SetNdivisions(608,kFALSE);
  return;
}


void MonSlider::DrawTrack() {
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);
  TVirtualPad* cc = canvas->cd(1);
  cc->SetGridy();
  cc->SetGridx();
  TH2D* theta_vs_phi = (TH2D*) GetHisto(rootfile,"Theta_vs_Phi_all");
  if (theta_vs_phi==0) { canvas->Update(); return; }
  theta_vs_phi->Draw("COLZ");
  cc = canvas->cd(2);
  cc->SetGridy();
  cc->SetGridx();
  TH2D* y_vs_x = (TH2D*) GetHisto(rootfile,"Y_vs_X_all"); 
  if (y_vs_x==0) { canvas->Update(); return; }
  y_vs_x->Draw("COLZ");
  // rigidity histogram
  TH1D* rigplus  = (TH1D*) GetHisto(rootfile,"logRigidityPlus_all");
  TH1D* rigminus = (TH1D*) GetHisto(rootfile,"logRigidityMinus_all");
  if ( (rigplus==0)||(rigminus==0) ) { canvas->Update(); return; }
  // prepare histograms with log scale
  float  logmin = rigplus->GetXaxis()->GetXmin();
  float  logmax = rigplus->GetXaxis()->GetXmax();
  int    nbins  = rigplus->GetXaxis()->GetNbins();
  float  step   = (logmax-logmin)/nbins;
  Double_t bins[100];
  for (int ii=0; ii<=nbins; ii++) bins[ii] = pow(10,logmin) * pow(10,step*ii);
  ClearHistoFromMemory("RigidityPlus_all");
  ClearHistoFromMemory("RigidityMinus_all");
  gROOT->cd();
  TH1D* rigplus2  = new TH1D("RigidityPlus_all","; Rigidity (GV/c)",nbins,bins);
  TH1D* rigminus2 = new TH1D("RigidityMinus_all","; Rigidity (GV/c)",nbins,bins);
  for (int ii=0; ii<nbins; ii++) { 
    rigplus2->SetBinContent(ii+1,rigplus->GetBinContent(ii+1));
    rigminus2->SetBinContent(ii+1,rigminus->GetBinContent(ii+1));
  }
  cc = canvas->cd(3);
  cc->SetLogx();
  cc->SetLogy();
  cc->SetGridy();
  cc->SetGridx(); 
  rigplus2->SetLineColor(kBlue);
  rigminus2->SetLineColor(kRed);
  rigplus2->Draw();
  rigminus2->Draw("SAME");
  // chisqared plot
  cc = canvas->cd(4);
  cc->SetGridy();
  cc->SetGridx();
  TH2D* chisq_vs_rig = (TH2D*) GetHisto(rootfile,"logChiSq_vs_logRigidity_all");
  if (chisq_vs_rig==0) { canvas->Update(); return; }
  chisq_vs_rig->Draw("COLZ");  
  canvas->Update();
}


void MonSlider::DrawRigidity() {
  canvas->Clear();
  canvas->Divide(1,2,0.001,0.001);
  TVirtualPad* cc = canvas->cd(1);
  cc->SetGridy();
  cc->SetGridx();
  TH2D* invrig = (TH2D*) GetHisto(rootfile,"InvRigidity_all");
  if (invrig==0) { canvas->Update(); return; }
  invrig->SetStats(kFALSE);
  invrig->Draw("HIST");
  cc = canvas->cd(2);
  cc->SetGridy();
  cc->SetGridx();
  TH2D* errinvrig = (TH2D*) rootfile->Get("ErrInvRigidity_all");
  if (errinvrig==0) { canvas->Update(); return; }
  errinvrig->SetStats(kFALSE);
  errinvrig->Draw("HIST");
  canvas->Update();
}


void MonSlider::DrawSeedOccupancyGlobal() {
  /* Drawing */
  canvas->cd(0);
  canvas->Clear(); 
  canvas->Divide(2,2,0.001,0.001);
  TText* text = new TText();
  text->SetTextSize(0.05);
  char testo[100];
  /* Occupancy Calculation */
  TH1D* rawcluste = (TH1D*) GetHisto(rootfile,"nRawClusters_all");
  if (rawcluste==0) { canvas->Update(); return; }
  int nentries = (int) rawcluste->GetEntries();
  if (nentries<1) {
    printf("MonSlider::DrawOccupancy-Warning no entries! Skipping\n");
    return;
  }
  char name[200];
  ClearHistoFromMemory("occupancy_all");
  ClearHistoFromMemory("occupancy_vs_ladder");
  ClearHistoFromMemory("occupancy_summary");
  gROOT->cd();
  TH1D* occupancy_all = new TH1D("occupancy_all","; Occupancy (%)",630,-0.1,2.0);
  TH2D* occupancy_vs_ladder = new TH2D("occupancy_vs_ladder","; iCrate*24 + iTdr; Occupancy (%)",192,0,192,105,-0.1,2.0);
  TH1D* occupancy_summary = new TH1D("occupancy_summary","; iCrate*24 + iTdr; number of strips",192,0,192);
  int occu[10] = {0,0,0,0,0,0,0,0,0,0};  
  int n[192];
  double mean[192];
  double rms[192];
  for (int ii=0; ii<192; ii++) {
    mean[ii] = 0;
    rms[ii] = 0;
    n[ii] = 0;
  } 
  for (int ii=0; ii<192; ii++) {
    int hwid = int(ii/24)*100 + ii%24; 
    TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
    int entry = ladder->GetCrate()*24 + ladder->GetTdr();
    int tkid  = ladder->GetTkId();
    int layer = ladder->GetLayer();    
    TH1D* occupancy = (TH1D*) GetHisto(rootfile,Form("SeedAddress_%+03d",tkid));
    if (occupancy==0) { canvas->Update(); return; }
    for (int bin=1; bin<=1024; bin++) {
      float occupancyvalue = 100.*occupancy->GetBinContent(bin)/nentries;
      occupancy_all->Fill(occupancyvalue);       
      if (occupancyvalue>1e-6) { 
        occupancy_vs_ladder->Fill(entry,occupancyvalue);
        mean[ii] += occupancyvalue;
        rms[ii] += occupancyvalue*occupancyvalue;
        n[ii]++;
      }
      for (int jj=0; jj<10; jj++)  
        if (occupancyvalue>(100*pow(2.,jj)/2048.)) occu[jj]++; 
    }
  }
  for (int ii=0; ii<192; ii++) {
    mean[ii] /= n[ii];
    rms[ii] /= n[ii];
    rms[ii] = sqrt(fabs(mean[ii]*mean[ii] - rms[ii]));
  }
  TProfile* occupancy_vs_ladder_prof = (TProfile*) GetProfileX(occupancy_vs_ladder);
  if (occupancy_vs_ladder_prof==0) { canvas->Update(); return; }
  /* Occupancy Threshold */
  int ntagged = 0;
  for (int ii=0; ii<192; ii++) {
    int hwid = int(ii/24)*100 + ii%24;
    TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
    int entry = ladder->GetCrate()*24 + ladder->GetTdr();
    int tkid  = ladder->GetTkId();
    int layer = ladder->GetLayer();
    TH1D* occupancy = (TH1D*) GetHisto(rootfile,Form("SeedAddress_%+03d",tkid));
    if (occupancy==0) { canvas->Update(); return; }
    for (int bin=1; bin<=1024; bin++) {
      if ( (tkid==+405)&&(bin>640) ) continue;
      float occupancyvalue = 100.*occupancy->GetBinContent(bin)/nentries;
      if ( (occupancyvalue>0.2) ) { 
        ntagged++;
        occupancy_summary->Fill(entry);
      }
    }
  }
  // printf("n. of occupancy strips: %6d\n",ntagged);
  /* Drawing */
  TVirtualPad* cc = canvas->cd(1);
  cc->SetLogy();
  cc->SetGridx();  
  // occupancy_all->SetStats(kFALSE);
  occupancy_all->Draw();
  cc = canvas->cd(2);
  for (int jj=0; jj<10; jj++) {
    sprintf(testo,"Occ. Thr. = %5.2f %%   n Tag. Strips = %10d",100.*pow(2.,jj)/2048.,occu[jj]);
    text->DrawTextNDC(0.1,0.8-0.06*jj,testo);
  }
  canvas->cd(3);
  occupancy_summary->SetStats(kFALSE);
  occupancy_summary->Draw();
  canvas->cd(4);
  occupancy_vs_ladder->Draw("COLZ");
  occupancy_vs_ladder_prof->Draw("SAME");
  canvas->Update();
}


void MonSlider::DrawSeedOccupancyOnLayer() {
  canvas->Clear();
  TText* text = new TText();
  text->SetTextSize(0.5);
  text->SetTextColor(kRed);
  int layer = ladder->GetLayer(); 
  int nladders = 0; 
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladd = TkDBc::Head->GetEntry(ii);
    if (ladd->GetHwId()<0) continue;
    if (ladd->GetLayer()!=layer) continue;
    nladders++;
  }
  canvas->Divide(2,int((nladders+1)/2),0.0001,0.0001,0);
  TVirtualPad* cc;
  TH1D* occupancy;
  for (int iside=0; iside<2; iside++) {
    int ipad = 0;
    for (int islot=0; islot<=30; islot++) {
      int tkid = layer*100 + islot;
      if (iside==1) tkid = -tkid;
      TkLadder* ladd = TkDBc::Head->FindTkId(tkid);
      if (ladd==0) continue;
      cc = canvas->cd(1 + 2*ipad + iside*1);
      cc->SetGridx();
      occupancy = (TH1D*) GetHisto(rootfile,Form("SeedAddress_%+04d",tkid));
      if (occupancy==0) continue;
      occupancy->SetStats(kFALSE);
      occupancy->GetXaxis()->SetNdivisions(516,kFALSE);
      occupancy->SetYTitle("");
      occupancy->Draw("HIST");  
      text->DrawTextNDC(0.,0.6,Form("%+04d",tkid));
      ipad++;
    }
  }
  canvas->Update();
}


void MonSlider::DrawHits() {
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(1,2,0.001,0.001);
  TH1D* nhits_vs_events = (TH1D*) GetHisto(rootfile,"nRecHits_vs_Events_all");
  if (nhits_vs_events==0) { canvas->Update(); return; }
  TVirtualPad* cc = canvas->cd(1);
  nhits_vs_events->Draw("COLZ");
  TH1D* nhitsontrack_vs_events = (TH1D*) GetHisto(rootfile,"nRecHitsOnTrack_vs_Events_all");
  if (nhitsontrack_vs_events==0) { canvas->Update(); return; }
  cc = canvas->cd(2);
  nhitsontrack_vs_events->Draw("COLZ");
  canvas->Update();
}


void MonSlider::DrawClustersSummary() {
  // prepare canvas
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(1,2,0.001,0.001);
  TText* text = new TText();
  TH2D* nraw_vs_ladder_n = (TH2D*) GetHisto(rootfile,"nRawClusters_vs_Ladder_all_N");
  TH2D* nraw_vs_ladder_p = (TH2D*) GetHisto(rootfile,"nRawClusters_vs_Ladder_all_P");
  if ( (nraw_vs_ladder_n==0)||(nraw_vs_ladder_p==0) ) { canvas->Update(); return; }
  TProfile* nraw_vs_ladder_n_prof = (TProfile*) GetProfileX(nraw_vs_ladder_n);
  TProfile* nraw_vs_ladder_p_prof = (TProfile*) GetProfileX(nraw_vs_ladder_p);
  if ( (nraw_vs_ladder_n_prof==0)||(nraw_vs_ladder_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,nraw_vs_ladder_n_prof);
  SetHistSideStyle(1,nraw_vs_ladder_p_prof);
  TVirtualPad* cc = canvas->cd(1);
  cc->SetGridy();
  cc->SetGridx();
  TH1F* frame = cc->DrawFrame(-0.05,0.,192.,1.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Average Num. of RawClusters");
  frame->SetXTitle("iCrate*24 + iTDR");
  SetHistSideStyle(0,frame);
  nraw_vs_ladder_n_prof->Draw("SAME");
  nraw_vs_ladder_p_prof->Draw("SAME");
  char testo[100];
  for (int ii=1; ii<=nraw_vs_ladder_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = nraw_vs_ladder_p_prof->GetBinContent(ii);
    float mean_n = nraw_vs_ladder_n_prof->GetBinContent(ii);
    if (mean_n>5.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if (mean_n>5.) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }
  TH2D* cluontrk_vs_ladder_n = (TH2D*) GetHisto(rootfile,"nClustersOnTrack_vs_Ladder_all_N");
  TH2D* cluontrk_vs_ladder_p = (TH2D*) GetHisto(rootfile,"nClustersOnTrack_vs_Ladder_all_P");
  if ( (cluontrk_vs_ladder_n==0)||(cluontrk_vs_ladder_p==0) ) { canvas->Update(); return; }
  TProfile* cluontrk_vs_ladder_n_prof = (TProfile*) GetProfileX(cluontrk_vs_ladder_n);
  TProfile* cluontrk_vs_ladder_p_prof = (TProfile*) GetProfileX(cluontrk_vs_ladder_p);
  if ( (cluontrk_vs_ladder_n_prof==0)||(cluontrk_vs_ladder_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,cluontrk_vs_ladder_n_prof);
  SetHistSideStyle(1,cluontrk_vs_ladder_p_prof);
  cc = canvas->cd(2);
  cc->SetGridy();
  cc->SetGridx();
  frame = cc->DrawFrame(-0.05,0.,192.,0.1);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Average Num. of Clusters On Track");
  frame->SetXTitle("iCrate*24 + iTDR");
  SetHistSideStyle(0,frame);
  cluontrk_vs_ladder_n_prof->Draw("SAME");
  cluontrk_vs_ladder_p_prof->Draw("SAME");
  for (int ii=1; ii<=cluontrk_vs_ladder_n_prof->GetNbinsX(); ii++) {
    int   tkid   = (TkDBc::Head->FindHwId(int((ii-1)/24)*100 + (ii-1)%24))->GetTkId();
    float mean_p = cluontrk_vs_ladder_p_prof->GetBinContent(ii);
    float mean_n = cluontrk_vs_ladder_n_prof->GetBinContent(ii);
    if (mean_n<0.00005) {
      sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kRed);
      text->DrawText(ii,mean_n,testo);
    }
    if (mean_p<0.00005) {
    sprintf(testo,"%+03d",tkid);
      text->SetTextColor(kBlue);
      text->DrawText(ii,mean_p,testo);
    }
  }
  canvas->Update();
}


