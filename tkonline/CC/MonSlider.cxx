#include "MonSlider.h"


ClassImp(MonSlider);


MonSlider::MonSlider(char *name,char *title,int xsize,int ysize) : SliderI(name,title,xsize,ysize){
  canvas->SetFillColor(0);
  rootfile = 0;
  reffile  = 0;  
  graphtype = 1;
  label = 0;
  if (!gROOT->IsBatch()) BuildMenu();
  ladder = TkDBc::Head->FindHwId(0);
  text = new TText();
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
  if (graphmenu!=0) graphmenu->Delete();
  graphmenu = 0;
  if (text!=0) text->Delete();
  text = 0;
}


void MonSlider::BuildMenu() {
  frame->SetWindowName("DataMenu");
  graphmenu = new TGListBox(frame,10);
  int index = 1;
  graphmenu->AddEntry("RawEntries",index++);
  graphmenu->AddEntry("TrackEntries",index++);
  graphmenu->AddEntry("LadderSummary",index++);
  graphmenu->AddEntry("LadderSummary (alt)",index++);
  graphmenu->AddEntry("SizeSummary",index++);
  graphmenu->AddEntry("SizeSummary (alt)",index++);
  graphmenu->AddEntry("DtSummary",index++);
  graphmenu->AddEntry("CrateVsDt",index++);
  graphmenu->AddEntry("Ladder",index++);
  graphmenu->AddEntry("ReconStats",index++);
  graphmenu->AddEntry("Track",index++);
  graphmenu->AddEntry("HitsOnTrack",index++);
  graphmenu->AddEntry("Rigidity",index++);
  graphmenu->AddEntry("SeedOccupancyOnLayer",index++);
  graphmenu->AddEntry("SeedOccupancyGlobal",index++);
  graphmenu->AddEntry("ClustersSummary",index++);
  graphmenu->AddEntry("ClustersSummary (alt)",index++);
  graphmenu->AddEntry("ReconVsTime",index++);
  graphmenu->AddEntry("OrbitFromTime",index++);
  graphmenu->AddEntry("Info",index++);

  graphmenu->Select(graphtype);
  graphmenu->Resize(180,400);
  frame->AddFrame(graphmenu,new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
  frame->MapSubwindows();
  frame->MoveResize(1200,600);
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

  TNtuple* ntuple = (TNtuple*) rootfile->FindObjectAny("timentuple");
  if (ntuple!=0) { 
    // first event time
    TIME_EVENT time_event;
    ntuple->SetBranchAddress("timebranch",&time_event);
    ntuple->GetEntry(0);
    time_t pippo = (time_t) time_event.Time;
    canvas->SetTitle(Form("First event %10d %s",(int)pippo,ctime(&pippo)));
  }
  /*
  if (rootfile->FindObjectAny("TrOnlineMonHeader")!=0) {
    TrHistoManHeader* header = (TrHistoManHeader*) rootfile->FindObjectAny("TrOnlineMonHeader"); 
    if (header->GetNRunNumbers()>0) { 
      char title[100];
      time_t pippo = (time_t) header->GetRunNumber(0);
      sprintf(title,"Data from run %10d %s",header->GetRunNumber(0),ctime(&pippo));
      canvas->SetTitle(title);
    }
  }
  */
  try2Draw(ladder);
}


void MonSlider::Update() {
  // simple check
  if (rootfile==0) {
    printf("MonSlider::Update-W You must select a file before updating it!\n");
    return;
  }
  ifstream ifile(rootfile->GetName());
  if (!ifile) {
    printf("MonSlider::Update-W File doesn't exist, doing nothing!\n");
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


TH1* MonSlider::GetHisto(TFile* file, char* name, char* oldname) {
  TH1* histo = GetHistoFromFile(file,name);
  if (histo==0) histo = GetHistoFromFile(file,oldname);
  return histo;
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


TH1D* MonSlider::GetProfileX(TH2* histo) {
  if (histo==0) { 
    printf("MonSlider::GetProfileX-W the requested histogram (%s) doesn't exist, returning 0.\n",histo->GetName());
    return 0;
  }
  // check-and-release memory
  ClearHistoFromMemory(Form("%s_prof",histo->GetName()));
  ClearHistoFromMemory(Form("%s_hprof",histo->GetName()));
  // write the object in a safe place
  gROOT->cd();
  TProfile* profile = (TProfile*) histo->ProfileX(Form("%s_prof",histo->GetName()));
  int    xbins = profile->GetXaxis()->GetNbins();
  double xmin  = profile->GetXaxis()->GetXmin();
  double xmax  = profile->GetXaxis()->GetXmax();
  TH1D* hprofile = new TH1D(Form("%s_hprof",histo->GetName()),"",xbins,xmin,xmax);
  for (int ibin=1; ibin<=xbins; ibin++) {
    hprofile->SetBinContent(ibin,profile->GetBinContent(ibin));
    hprofile->SetBinError(ibin,profile->GetBinError(ibin));
  }
  // delete tmp profile
  ClearHistoFromMemory(Form("%s_prof",histo->GetName()));
  return hprofile;
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
    DrawRawEntries();
    break; 
  case 2:
    DrawTrackEntries();
    break;
  case 3:
    DrawSignalSummary(0);
    break;
  case 4:
    DrawSignalSummary(1);
    break;
  case 5:
    DrawSizeSummary(0);
    break;
  case 6:
    DrawSizeSummary(1);
    break;
  case 7:
    DrawDtSummary();
    break;    
  case 8:
    DrawCrateVsDt();
    break;
  case 9:
    DrawLadder();
    break;   
  case 10:
    DrawReconStats();
    break;
  case 11:
    DrawTrack();
    break;
  case 12:
    DrawHitsOnTrack();
    break;
  case 13:
    DrawRigidity();
    break;
  case 14:
    DrawSeedOccupancyOnLayer();
    break;
  case 15:
    DrawSeedOccupancyGlobal();
    break;
  case 16:
    DrawClustersSummary(0);
    break;
  case 17:
    DrawClustersSummary(1);
    break;
  case 18:
    DrawReconVsTime();
    break;
  case 19:
    DrawOrbitFromTime();
    break; 
  case 20:
    DrawInfo();
    break;
  }
}


void MonSlider::TestAndDrawText(int side, TH1D* profile, float xmin, float xmax) {
  for (int ibin=1; ibin<=profile->GetNbinsX(); ibin++) {
    int hwid = int((ibin-1)/24)*100 + (ibin-1)%24;
    TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
    if (ladder==0) continue; 
    int tkid = ladder->GetTkId();
    float x = profile->GetBinContent(ibin);
    if ( (x<xmin)||(x>xmax) ) {
      if (side==0) text->SetTextColor(kRed);
      else         text->SetTextColor(kBlue); 
      text->DrawText(ibin,x,Form("%+03d",tkid));
    }
  }
}


void MonSlider::ChangeToLayerIndex(TH1D* histo) {
  if (histo->GetNbinsX()!=192) {
    printf("MonSlider::ChangeToLayerIndex-W the given histo has not the expected format, skipping\n");
    return;
  }
  // storing values
  double value[192];
  double evalue[192];
  for (int ibin=1; ibin<=histo->GetNbinsX(); ibin++) {
    value[ibin-1] = histo->GetBinContent(ibin);
    evalue[ibin-1] = histo->GetBinError(ibin);
  }
  // re-init
  histo->SetBins(450,0.5,9.5);
  histo->Reset();
  for (int ibin=1; ibin<=450; ibin++) { 
    histo->SetBinContent(ibin,-9999); 
    histo->SetBinError(ibin,0); 
  }
  // set
  for (int ibin=1; ibin<=192; ibin++) {
    int hwid = int((ibin-1)/24)*100 + (ibin-1)%24;
    TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
    int index = ladder->GetLayer()*50 + ladder->GetSlot() - 25;
    histo->SetBinContent(index,value[ibin-1]);  
    histo->SetBinError(index,evalue[ibin-1]);
  }
  // setting
  histo->SetStats(kFALSE);
  histo->SetXTitle("iLayer + iSlot/50");
  histo->SetNdivisions(510);
  return;
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


void MonSlider::DrawRawEntries(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(2,1,0.001,0.001);
  text->SetTextColor(kBlack);

  // x-side
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetLogz(kFALSE);
  pad1->SetGridy();
  pad1->SetGridx();
  TH2D* entries_n = (TH2D*) GetHisto(rootfile,"RawEntries_entries_N");
  if (entries_n==0) { canvas->Update(); return; }
  entries_n->SetStats(kFALSE);
  entries_n->Draw("COLZ");
  text->DrawTextNDC(0.1,0.91,"X-Side");

  // y-side
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetLogz(kFALSE);
  pad2->SetGridy();
  pad2->SetGridx();
  TH2D* entries_p = (TH2D*) GetHisto(rootfile,"RawEntries_entries_P");
  if (entries_p==0) { canvas->Update(); return; }
  entries_p->SetStats(kFALSE);
  entries_p->Draw("COLZ");
  text->DrawTextNDC(0.1,0.91,"Y-Side");
  canvas->Update();
}


void MonSlider::DrawTrackEntries(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(2,1,0.001,0.001);
  text->SetTextColor(kBlack);

  // x-side
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetLogz(kFALSE);
  pad1->SetGridy();
  pad1->SetGridx();
  TH2D* entries_n = (TH2D*) GetHisto(rootfile,"TrackEntries_entries_N");
  if (entries_n==0) { canvas->Update(); return; }
  entries_n->SetStats(kFALSE);
  entries_n->Draw("COLZ");
  text->DrawTextNDC(0.1,0.91,"X-Side");

  // y-side
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetLogz(kFALSE);
  pad2->SetGridy();
  pad2->SetGridx();
  TH2D* entries_p = (TH2D*) GetHisto(rootfile,"TrackEntries_entries_P");
  if (entries_p==0) { canvas->Update(); return; }
  entries_p->SetStats(kFALSE);
  entries_p->Draw("COLZ");
  text->DrawTextNDC(0.1,0.91,"Y-Side");
  canvas->Update();
}


void MonSlider::DrawSignalSummary(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(2,1,0.001,0.001);  
  text->SetTextColor(kBlack);

  // signal summary
  TH2D* signal_n = (TH2D*) GetHisto(rootfile,"Signal_vs_Ladder_all_N");
  TH2D* signal_p = (TH2D*) GetHisto(rootfile,"Signal_vs_Ladder_all_P");
  if ( (signal_n==0)||(signal_p==0) ) { canvas->Update(); return; }
  TH1D* signal_n_prof = (TH1D*) GetProfileX(signal_n);
  TH1D* signal_p_prof = (TH1D*) GetProfileX(signal_p);
  if ( (signal_n_prof==0)||(signal_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,signal_n_prof);
  SetHistSideStyle(1,signal_p_prof);
  signal_n_prof->SetYTitle("Average Amplitude (ADC)");
  signal_p_prof->SetYTitle("Average Amplitude (ADC)");
  signal_n_prof->SetXTitle("iCrate*24 + iTDR");
  signal_p_prof->SetXTitle("iCrate*24 + iTDR");
  if (alternative==1) {
    ChangeToLayerIndex(signal_n_prof);
    ChangeToLayerIndex(signal_p_prof);
  }
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1); 
  pad1->SetGridy();
  pad1->SetGridx();
  signal_n_prof->GetYaxis()->SetRangeUser(0.,100.);
  signal_n_prof->Draw();
  signal_p_prof->Draw("SAME");
  if (alternative==0) { 
    TestAndDrawText(0,signal_n_prof,10,60);
    TestAndDrawText(1,signal_p_prof,10,60);
  }

  // width summary 
  TH2D* width_n = (TH2D*) GetHisto(rootfile,"NElement_vs_Ladder_all_N","Width_vs_Ladder_all_N");
  TH2D* width_p = (TH2D*) GetHisto(rootfile,"NElement_vs_Ladder_all_P","Width_vs_Ladder_all_P");
  if ( (width_n==0)||(width_p==0) ) { canvas->Update(); return; }
  TH1D* width_n_prof = (TH1D*) GetProfileX(width_n);
  TH1D* width_p_prof = (TH1D*) GetProfileX(width_p);
  if ( (width_n_prof==0)||(width_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,width_n_prof);
  SetHistSideStyle(1,width_p_prof);
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetLogy(kFALSE);
  pad2->SetGridy();
  pad2->SetGridx();
  width_n_prof->SetYTitle("Average Cluster Width");
  width_p_prof->SetYTitle("Average Cluster Width");
  width_n_prof->SetXTitle("iCrate*24 + iTDR");
  width_p_prof->SetXTitle("iCrate*24 + iTDR");
  if (alternative==1) {
    ChangeToLayerIndex(width_n_prof);
    ChangeToLayerIndex(width_p_prof);
  }
  width_n_prof->GetYaxis()->SetRangeUser(0.,20.);
  width_n_prof->Draw();
  width_p_prof->Draw("SAME");
  if (alternative==0) { 
    TestAndDrawText(0,width_n_prof,1,5);
    TestAndDrawText(1,width_p_prof,1,5);
  }

  canvas->Update();
}


void MonSlider::DrawSizeSummary(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(1,2,0.001,0.001);  
  text->SetTextColor(kBlack);
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->Divide(2,1,0.001,0.001);  

  // tracker size  
  TH2D* size = (TH2D*) GetHisto(rootfile,"Size_all");
  if (size==0) { canvas->Update(); return; }
  size->GetXaxis()->SetRangeUser(0.,3000.); 
  TVirtualPad* subpad1 = (TVirtualPad*) pad1->cd(1);
  subpad1->SetLogy();
  subpad1->SetGridx();
  subpad1->SetGridy();
  size->SetStats(kFALSE);
  size->Draw("HIST");

  // crate size   
  TH2D* sizevscrate = (TH2D*) GetHisto(rootfile,"Size_vs_Crate_all");
  TH1D* sizevscrate_prof = (TH1D*) GetProfileX(sizevscrate);
  if ( (sizevscrate==0)||(sizevscrate_prof==0) ) { canvas->Update(); return; }
  TVirtualPad* subpad2 = (TVirtualPad*) pad1->cd(2);
  subpad2->SetGridy();
  TH1F* frame1 = (TH1F*) subpad2->DrawFrame(-0.5,0.,7.5,500.);
  frame1->SetYTitle("Crate Segment Size (byte)");
  frame1->SetXTitle("Crate");
  sizevscrate_prof->SetMarkerStyle(21);
  sizevscrate_prof->Draw("P SAME");

  // ladder size
  TH2D* size_n = (TH2D*) GetHisto(rootfile,"Size_vs_Ladder_all_N");
  TH2D* size_p = (TH2D*) GetHisto(rootfile,"Size_vs_Ladder_all_P");
  if ( (size_n==0)||(size_p==0) ) { canvas->Update(); return; }
  TH1D* size_n_prof = (TH1D*) GetProfileX(size_n);
  TH1D* size_p_prof = (TH1D*) GetProfileX(size_p);
  if ( (size_n_prof==0)||(size_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,size_n_prof);
  SetHistSideStyle(1,size_p_prof);
  TVirtualPad* pad2 = (TVirtualPad*)canvas->cd(2);
  pad2->SetLogy(kFALSE);
  pad2->SetGridy();
  pad2->SetGridx();
  size_n_prof->SetYTitle("Ladder Size (byte)");
  size_p_prof->SetYTitle("Ladder Size (byte)");
  size_n_prof->SetXTitle("iCrate*24 + iTDR");
  size_p_prof->SetXTitle("iCrate*24 + iTDR");
  if (alternative==1) {
    ChangeToLayerIndex(size_n_prof);
    ChangeToLayerIndex(size_p_prof);
  }
  size_n_prof->GetYaxis()->SetRangeUser(0.,20.);
  size_n_prof->Draw();
  size_p_prof->Draw("SAME");
  if (alternative==0) {
    TestAndDrawText(0,size_n_prof,1,5);
    TestAndDrawText(1,size_p_prof,1,5);
  }
  canvas->Update();
}


void MonSlider::DrawDtSummary(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(1,2,0.001,0.001);  
  text->SetTextColor(kBlack);
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->Divide(2,1,0.001,0.001);

  // interevent time distribution
  TVirtualPad* subpad1 = (TVirtualPad*) pad1->cd(1);
  TH1D* dt = (TH1D*) GetHisto(rootfile,"DT_all");
  if (dt==0) { canvas->Update(); return; }
  dt->Rebin(8);
  subpad1->SetLogy();
  subpad1->SetGridx();
  subpad1->SetGridy();
  TF1 *f1 = new TF1("f1","expo",800,5000);
  dt->Fit("f1","QR");
  float inrate = -1000.*dt->GetFunction("f1")->GetParameter(1);
  float trrate = 1000./dt->GetMean();
  dt->SetStats(kFALSE);
  dt->Draw("l");
  text->DrawTextNDC(0.5,0.80,Form("Est. Event Rate = %7.3f Hz",inrate*1000.));
  text->DrawTextNDC(0.5,0.75,Form("Est. Trigger Rate = %7.3f Hz",trrate*1000.));
  text->DrawTextNDC(0.5,0.70,Form("Est. Lifetime = %7.5f",trrate/inrate));

  // size vs interevent time 
  TVirtualPad* subpad2 = (TVirtualPad*) pad1->cd(2);
  subpad2->SetGridx();
  subpad2->SetGridy();  
  TH2D* sizeVSdt = (TH2D*) GetHisto(rootfile,"Size_vs_DT_all");
  TH1D* sizeVSdt_prof = (TH1D*) GetProfileX(sizeVSdt);
  if ( (sizeVSdt==0)||(sizeVSdt_prof==0) ) { canvas->Update(); return; }
  sizeVSdt_prof->SetStats(kFALSE);
  sizeVSdt_prof->Draw();

  // ladder size at low dt 
  TH2D* sizelowdt_n = (TH2D*) GetHisto(rootfile,"SizeLowDT_vs_Ladder_all_N");
  TH2D* sizelowdt_p = (TH2D*) GetHisto(rootfile,"SizeLowDT_vs_Ladder_all_P");
  if ( (sizelowdt_n==0)||(sizelowdt_p==0) ) { canvas->Update(); return; }
  TH1D* sizelowdt_n_prof = (TH1D*) GetProfileX(sizelowdt_n);
  TH1D* sizelowdt_p_prof = (TH1D*) GetProfileX(sizelowdt_p);
  if ( (sizelowdt_n_prof==0)||(sizelowdt_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,sizelowdt_n_prof);
  SetHistSideStyle(1,sizelowdt_p_prof);
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetGridy();
  TH1F* frame = (TH1F*) pad2->DrawFrame(0.,0.,192.,250.);
  frame->GetXaxis()->SetNdivisions(608,kFALSE);
  frame->SetYTitle("Approx. Mean Ladder Size @ #Deltat<200 #mus (byte)");
  frame->SetXTitle("iCrate*24 + iTDR");
  sizelowdt_n_prof->Draw("SAME");
  sizelowdt_p_prof->Draw("SAME");
  TestAndDrawText(0,sizelowdt_n_prof,0.1,100);
  TestAndDrawText(1,sizelowdt_p_prof,0.1,100);
  canvas->Update();
}


void MonSlider::DrawCrateVsDt(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(4,2,0.001,0.001);

  // crate size vs interevent distribution
  TVirtualPad* pad;
  TH2D* cratevsdt[8]; 
  for (int icrate=0; icrate<8; icrate++) { 
    cratevsdt[icrate] = (TH2D*) GetHisto(rootfile,Form("T%1dSize_vs_DT_all",icrate));
    TH1D* cratevsdt_prof = (TH1D*) GetProfileX(cratevsdt[icrate]);
    pad = canvas->cd(icrate+1);
    pad->SetGridx();
    pad->SetGridy();
    cratevsdt[icrate]->GetYaxis()->SetRangeUser(0.,5000.);
    cratevsdt[icrate]->SetStats(kFALSE);
    cratevsdt[icrate]->SetMarkerStyle(20);
    cratevsdt[icrate]->SetMarkerColor(kGray);
    cratevsdt[icrate]->Draw();
    cratevsdt_prof->SetMarkerStyle(1);
    cratevsdt_prof->SetMarkerColor(kBlack);
    cratevsdt_prof->Draw("SAME");   
    text->DrawTextNDC(0.8,0.8,Form("T%1d",icrate));
  }
  canvas->Update();
}


void MonSlider::DrawLadder(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();  
  canvas->Divide(2,2,0.001,0.001);  
  text->SetTextColor(kBlack); 
  int tkid  = ladder->GetTkId();

  // signal
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetGridx();
  pad1->SetLogy();
  pad1->SetGridy();
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

  // width
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetGridx();
  pad2->SetLogy();
  pad2->SetGridy();
  TH1D* width_n = (TH1D*) GetHisto(rootfile,Form("NElement_%+04d_N",tkid),Form("Width_%+04d_N",tkid));
  TH1D* width_p = (TH1D*) GetHisto(rootfile,Form("NElement_%+04d_P",tkid),Form("Width_%+04d_P",tkid));
  if ( (width_n==0)||(width_p==0) ) { canvas->Update(); return; }
  width_n->SetStats(kFALSE);
  width_n->SetLineWidth(2);
  width_n->SetLineColor(kRed);
  width_p->SetStats(kFALSE);
  width_p->SetLineWidth(2);
  width_p->SetLineColor(kBlue);
  width_p->Draw("HIST");
  width_n->Draw("HIST SAME");

  // occupancy
  TVirtualPad* pad3 = (TVirtualPad*) canvas->cd(3);
  pad3->SetGridx();
  pad3->SetGridy();
  pad3->SetLogy(kFALSE);
  TH1D* occupancy = (TH1D*) GetHisto(rootfile,Form("SeedAddress_%+04d",tkid));
  TH1D* rawcluste = (TH1D*) GetHisto(rootfile,"nRawClusters_all");
  if ( (occupancy==0)||(rawcluste==0) ) { canvas->Update(); return; }
  int nentries = (int) rawcluste->GetEntries();
  occupancy->Scale(100./nentries); 
  occupancy->SetStats(kFALSE);
  occupancy->GetXaxis()->SetNdivisions(516,kFALSE);
  occupancy->SetYTitle("Occupancy (%)");
  occupancy->SetLineWidth(2);
  occupancy->Draw(); 

  // size
  TVirtualPad* pad4 = (TVirtualPad*) canvas->cd(4);
  pad4->SetGridx();
  pad4->SetLogy();
  pad4->SetGridy();
  TH1D* size = (TH1D*) GetHisto(rootfile,Form("Size_%+04d",tkid));
  if (size==0) { canvas->Update(); return; }
  size->SetLineWidth(2);
  size->SetStats(kFALSE);
  size->GetXaxis()->SetRangeUser(7.,400);
  size->Draw("HIST"); 
  canvas->cd(1);

  // name
  canvas->cd(1);
  text->DrawTextNDC(0.1,0.91,Form("Ladder %s  Oct: %s Crate: %d TDR: %d Layer: %d Slot: %d Side: %d ",
     ladder->name,GetOctName(ladder->GetOctant()),
     ladder->GetCrate(),ladder->GetTdr(),ladder->GetLayer(),
     ladder->GetSlot(),ladder->GetSide()));
  canvas->Update();  
}


void MonSlider::DrawReconStats(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(3,2,0.001,0.001);
  text->SetTextColor(kBlack);

  // raw clusters
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1); 
  pad1->SetLogy();
  pad1->SetGridy();
  TH1D* raw = (TH1D*) GetHisto(rootfile,"nRawClusters_all");
  if (raw==0) { canvas->Update(); return; }
  raw->Draw("HIST");

  // clusters
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetLogy();
  pad2->SetGridy();
  TH1D* clu = (TH1D*) GetHisto(rootfile,"nClusters_all");
  if (clu==0) { canvas->Update(); return; }
  clu->Draw("HIST");

  // rec. hits 
  TVirtualPad* pad3 = (TVirtualPad*) canvas->cd(4);
  pad3->SetLogy();
  pad3->SetGridy();
  TH1D* hit = (TH1D*) GetHisto(rootfile,"nRecHits_all");
  if (hit==0) { canvas->Update(); return; }
  hit->Draw("HIST");

  // tracks
  TVirtualPad* pad4 = (TVirtualPad*) canvas->cd(5);
  pad4->SetLogy();
  pad4->SetGridy();
  TH1D* trk = (TH1D*) GetHisto(rootfile,"nTracks_all");		
  if (trk==0) { canvas->Update(); return; }
  trk->Draw("HIST");

  // clusters on track
  TVirtualPad* pad5 = (TVirtualPad*) canvas->cd(3);
  pad5->SetLogy();
  pad5->SetGridy();
  TH1D* cluontrkn = (TH1D*) GetHisto(rootfile,"nClustersOnTrack_all_N");
  TH1D* cluontrkp = (TH1D*) GetHisto(rootfile,"nClustersOnTrack_all_P");
  if ( (cluontrkn==0)||(cluontrkp==0) ) { canvas->Update(); return; } 
  cluontrkp->SetLineColor(kBlue);
  cluontrkn->SetLineColor(kRed);
  cluontrkp->Draw("HIST");
  cluontrkn->Draw("HIST SAME");

  // rec. hits on track
  TVirtualPad* pad6 = (TVirtualPad*) canvas->cd(6);
  pad6->SetLogy();
  pad6->SetGridy();
  TH1D* hitontrk = (TH1D*) GetHisto(rootfile,"nRecHitsOnTrack_all");
  hitontrk->Draw("HIST");
  canvas->Update();
}


void MonSlider::DrawHitsOnTrack(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(3,3,0.001,0.001);
  text->SetTextColor(kBlack);

  // hits on track
  int order[9] = {2,3,4,5,6,7,8,1,9};
  TVirtualPad* pad;
  TH2D* occupancy[9];
  for (int tt=0; tt<TkDBc::Head->nlay(); tt++) {
    occupancy[tt] = (TH2D*) GetHisto(rootfile,Form("Occupancy_layer%1d",tt+1));
    if (occupancy[tt]==0) { canvas->Update(); return; }
    if (TkDBc::Head->GetSetup()==3) pad = canvas->cd(order[tt]);
    else                            pad = canvas->cd(tt+1);
    pad->SetGridx();
    pad->SetGridy();
    occupancy[tt]->SetStats(kFALSE);
    occupancy[tt]->Draw("COLZ");
    text->DrawTextNDC(0.10,0.91,Form("Layer %1d",order[tt]));
  }
  canvas->Update();
}


void MonSlider::DrawTrack(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(2,2,0.001,0.001);
  text->SetTextColor(kBlack);

  // theta vs phi
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetGridy();
  pad1->SetGridx();
  TH2D* theta_vs_phi = (TH2D*) GetHisto(rootfile,"Theta_vs_Phi_all");
  if (theta_vs_phi==0) { canvas->Update(); return; }
  theta_vs_phi->Draw("COLZ");

  // y vs x
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetGridy();
  pad2->SetGridx();
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
  double bins[100];
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
  TVirtualPad* pad3 = (TVirtualPad*) canvas->cd(3);
  pad3->SetLogx();
  pad3->SetLogy();
  pad3->SetGridy();
  pad3->SetGridx(); 
  rigplus2->SetLineColor(kBlue);
  rigminus2->SetLineColor(kRed);
  rigplus2->Draw();
  rigminus2->Draw("SAME");

  // chi-squared plot
  TVirtualPad* pad4 = (TVirtualPad*) canvas->cd(4);
  pad4->SetGridy();
  pad4->SetGridx();
  TH2D* chisq_vs_rig = (TH2D*) GetHisto(rootfile,"logChiSq_vs_logRigidity_all");
  if (chisq_vs_rig==0) { canvas->Update(); return; }
  chisq_vs_rig->Draw("COLZ");  
  canvas->Update();
}


void MonSlider::DrawRigidity(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear(); 
  canvas->Divide(2,1,0.001,0.001);
  text->SetTextColor(kBlack);

  // rigidity
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetGridy();
  pad1->SetGridx();
  TH2D* invrig = (TH2D*) GetHisto(rootfile,"InvRigidity_all");
  if (invrig==0) { canvas->Update(); return; }
  invrig->SetStats(kFALSE);
  invrig->Draw("HIST");

  // error on rigidity
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetGridy();
  pad2->SetGridx();
  TH2D* errinvrig = (TH2D*) GetHisto(rootfile,"ErrInvRigidity_all");
  if (errinvrig==0) { canvas->Update(); return; }
  errinvrig->SetStats(kFALSE);
  errinvrig->GetYaxis()->SetRangeUser(0.,0.8);
  errinvrig->Draw("COLZ");
  canvas->Update();
}


void MonSlider::DrawSeedOccupancyGlobal(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear(); 
  canvas->Divide(2,2,0.001,0.001);
  text->SetTextColor(kBlack);

  // occupancy calculation 
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
  TH1D* occupancy_all = new TH1D("occupancy_all","; Occupancy (%)",210,-0.1,2.0);
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
  TH1D* occupancy_vs_ladder_prof = (TH1D*) GetProfileX(occupancy_vs_ladder);
  if (occupancy_vs_ladder_prof==0) { canvas->Update(); return; }

  // occupancy threshold 
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

  // drawing 
  TVirtualPad* pad = canvas->cd(1);
  pad->SetLogy();
  pad->SetGridx();  
  occupancy_all->Draw();
  canvas->cd(2);
  for (int jj=0; jj<10; jj++) {
    text->DrawTextNDC(0.1,0.8-0.06*jj,Form("Occ. Thr. = %5.2f %%   n Tag. Strips = %10d",100.*pow(2.,jj)/2048.,occu[jj]));
  }
  canvas->cd(3);
  occupancy_summary->SetStats(kFALSE);
  occupancy_summary->Draw();
  canvas->cd(4);
  occupancy_vs_ladder->Draw("COLZ");
  occupancy_vs_ladder_prof->Draw("SAME");
  canvas->Update();
}


void MonSlider::DrawSeedOccupancyOnLayer(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  text->SetTextSize(0.3);
  int layer = ladder->GetLayer(); 
  int nladders = 0; 
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladd = TkDBc::Head->GetEntry(ii);
    if (ladd->GetHwId()<0) continue;
    if (ladd->GetLayer()!=layer) continue;
    nladders++;
  }
  canvas->Divide(2,int((nladders+1)/2),0.0001,0.0001,0);
  TVirtualPad* pad;
  TH1D* occupancy;
  for (int iside=0; iside<2; iside++) {
    int ipad = 0;
    for (int islot=0; islot<=30; islot++) {
      int tkid = layer*100 + islot;
      if (iside==1) tkid = -tkid;
      TkLadder* ladd = TkDBc::Head->FindTkId(tkid);
      if (ladd==0) continue;
      pad = (TVirtualPad*) canvas->cd(1 + 2*ipad + iside*1);
      pad->SetGridx();
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
  text->SetTextSize(0.05);
}


void MonSlider::DrawClustersSummary(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(1,2,0.001,0.001);
  text->SetTextColor(kBlack);

  // number of raw cluster per ladder
  TH2D* nraw_vs_ladder_n = (TH2D*) GetHisto(rootfile,"nRawClusters_vs_Ladder_all_N");
  TH2D* nraw_vs_ladder_p = (TH2D*) GetHisto(rootfile,"nRawClusters_vs_Ladder_all_P");
  if ( (nraw_vs_ladder_n==0)||(nraw_vs_ladder_p==0) ) { canvas->Update(); return; }
  TH1D* nraw_vs_ladder_n_prof = (TH1D*) GetProfileX(nraw_vs_ladder_n);
  TH1D* nraw_vs_ladder_p_prof = (TH1D*) GetProfileX(nraw_vs_ladder_p);
  if ( (nraw_vs_ladder_n_prof==0)||(nraw_vs_ladder_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,nraw_vs_ladder_n_prof);
  SetHistSideStyle(1,nraw_vs_ladder_p_prof);
  nraw_vs_ladder_n_prof->SetYTitle("Average Num. of Raw Clusters");
  nraw_vs_ladder_p_prof->SetYTitle("Average Num. of Raw Clusters");
  nraw_vs_ladder_n_prof->SetXTitle("iCrate*24 + iTDR");
  nraw_vs_ladder_p_prof->SetXTitle("iCrate*24 + iTDR");
  if (alternative==1) {
    ChangeToLayerIndex(nraw_vs_ladder_n_prof);
    ChangeToLayerIndex(nraw_vs_ladder_p_prof);
  }
  TVirtualPad* pad1 = (TVirtualPad*) canvas->cd(1);
  pad1->SetGridy();
  pad1->SetGridx();
  nraw_vs_ladder_n_prof->GetYaxis()->SetRangeUser(0.,0.5);
  nraw_vs_ladder_n_prof->Draw();
  nraw_vs_ladder_p_prof->Draw("SAME");
  if (alternative==0) { 
    TestAndDrawText(0,nraw_vs_ladder_n_prof,0.00001,0.4);
    TestAndDrawText(1,nraw_vs_ladder_p_prof,0.00001,0.4);
  }

  // number of cluster per track on ladder
  TH2D* cluontrk_vs_ladder_n = (TH2D*) GetHisto(rootfile,"nClustersOnTrack_vs_Ladder_all_N");
  TH2D* cluontrk_vs_ladder_p = (TH2D*) GetHisto(rootfile,"nClustersOnTrack_vs_Ladder_all_P");
  if ( (cluontrk_vs_ladder_n==0)||(cluontrk_vs_ladder_p==0) ) { canvas->Update(); return; }
  TH1D* cluontrk_vs_ladder_n_prof = (TH1D*) GetProfileX(cluontrk_vs_ladder_n);
  TH1D* cluontrk_vs_ladder_p_prof = (TH1D*) GetProfileX(cluontrk_vs_ladder_p);
  if ( (cluontrk_vs_ladder_n_prof==0)||(cluontrk_vs_ladder_p_prof==0) ) { canvas->Update(); return; }
  SetHistSideStyle(0,cluontrk_vs_ladder_n_prof);
  SetHistSideStyle(1,cluontrk_vs_ladder_p_prof);
  cluontrk_vs_ladder_n_prof->SetYTitle("Average Num. of Clusters On Track");
  cluontrk_vs_ladder_p_prof->SetYTitle("Average Num. of Clusters On Track");
  cluontrk_vs_ladder_n_prof->SetXTitle("iCrate*24 + iTDR");
  cluontrk_vs_ladder_p_prof->SetXTitle("iCrate*24 + iTDR");
  if (alternative==1) {
    ChangeToLayerIndex(cluontrk_vs_ladder_n_prof);
    ChangeToLayerIndex(cluontrk_vs_ladder_p_prof);
  }
  TVirtualPad* pad2 = (TVirtualPad*) canvas->cd(2);
  pad2->SetGridy();
  pad2->SetGridx();
  cluontrk_vs_ladder_n_prof->GetYaxis()->SetRangeUser(0.,0.2);
  cluontrk_vs_ladder_n_prof->Draw();
  cluontrk_vs_ladder_p_prof->Draw("SAME");
  if (alternative==0) { 
    TestAndDrawText(0,cluontrk_vs_ladder_n_prof,0.00001,0.1);
    TestAndDrawText(1,cluontrk_vs_ladder_p_prof,0.00001,0.1);
  }
  canvas->Update();
}


void MonSlider::DrawReconVsTime(int alternative) {
  // clean
  canvas->Draw();
  canvas->Clear();
  canvas->Divide(1,3,0.001,0.001);
  
  // ntuple check
  rootfile->cd();
  TNtuple* ntuple = (TNtuple*) rootfile->FindObjectAny("timentuple");
  if (ntuple==0) {
    printf("MonSlider::DrawReconVsTime-W No time ntuple found, skipping.\n");
    canvas->Update();
    return;
  }
  float min_time = ntuple->GetMinimum("Time");
  float max_time = ntuple->GetMaximum("Time");

  // clear histograms and define them
  ClearHistoFromMemory("SizeVsTime");
  ClearHistoFromMemory("nRawVsTime");
  ClearHistoFromMemory("nTrkVsTime");
  gROOT->cd();
  TH2D* size_vs_time = new TH2D("SizeVsTime","; Time (sec); Tracker Size (byte)",100,0,max_time-min_time,1000,0,20000);
  TH2D* nraw_vs_time = new TH2D("nRawVsTime","; Time (sec); Number of Raw Clusters",100,0,max_time-min_time,500,-0.5,499.5); 
  TH2D* ntrk_vs_time = new TH2D("nTrkVsTime","; Time (sec); Number of Tracks",100,0,max_time-min_time,10,-0.5,9.5);

  // loop on ntuple for histogram filling
  rootfile->cd();
  TIME_EVENT time_event;
  ntuple->SetBranchAddress("timebranch",&time_event);
  int nentries = ntuple->GetEntries();
  int step = 1 + nentries/10000;
  for (int i=0; i<nentries; i += step) {
    ntuple->GetEntry(i);
    size_vs_time->Fill(time_event.Time - min_time,time_event.Size);
    nraw_vs_time->Fill(time_event.Time - min_time,time_event.nRaw);
    ntrk_vs_time->Fill(time_event.Time - min_time,time_event.nTrk);
  }

  // size vs time
  TVirtualPad* pad1 = canvas->cd(1);
  pad1->SetGridx();
  pad1->SetGridy();
  size_vs_time->SetStats(kFALSE);
  TH1D* size_vs_time_prof = (TH1D*) GetProfileX(size_vs_time);
  if (size_vs_time_prof==0) { canvas->Update(); return; }
  size_vs_time->SetMarkerStyle(20);
  size_vs_time->SetMarkerColor(kGray);
  size_vs_time->Draw();
  size_vs_time_prof->SetMarkerStyle(1);
  size_vs_time_prof->SetMarkerColor(kBlack);
  size_vs_time_prof->Draw("SAME");
   
  // nraw vs time
  TVirtualPad* pad2 = canvas->cd(2);
  pad2->SetGridx();
  pad2->SetGridy();
  nraw_vs_time->SetStats(kFALSE);
  TH1D* nraw_vs_time_prof = (TH1D*) GetProfileX(nraw_vs_time);
  if (nraw_vs_time_prof==0) { canvas->Update(); return; }
  nraw_vs_time->SetYTitle("Number of Raw Clusters");
  nraw_vs_time->SetXTitle("Time (sec)");
  nraw_vs_time->SetMarkerStyle(20);
  nraw_vs_time->SetMarkerColor(kOrange);
  nraw_vs_time->Draw();
  nraw_vs_time_prof->SetMarkerStyle(1);
  nraw_vs_time_prof->SetMarkerColor(kBlack);
  nraw_vs_time_prof->Draw("SAME");

  // ntrack vs time
  TVirtualPad* pad3 = canvas->cd(3);
  pad3->SetGridx();
  pad3->SetGridy();
  ntrk_vs_time->SetStats(kFALSE);
  TH1D* ntrk_vs_time_prof = (TH1D*) GetProfileX(ntrk_vs_time);
  if (ntrk_vs_time_prof==0) { canvas->Update(); return; }
  ntrk_vs_time->SetTitle("");
  ntrk_vs_time->SetYTitle("Number of Tracks");
  ntrk_vs_time->SetXTitle("Time (sec)");
  ntrk_vs_time->SetMarkerStyle(20);
  ntrk_vs_time->SetMarkerColor(kViolet);
  ntrk_vs_time->Draw();
  ntrk_vs_time_prof->SetMarkerStyle(1);
  ntrk_vs_time_prof->SetMarkerColor(kBlack);
  ntrk_vs_time_prof->Draw("SAME");

  canvas->Update();
}


void MonSlider::DrawOrbitFromTime(int alternative) { 
  // clear
  canvas->Clear();
  canvas->cd(0);
  // time event ntuple
  TH2F* trajectory = new TH2F("traj","; latitude; longitude",180,-180,180,161,-80.5,80.5);
  rootfile->cd();
  TNtuple* ntuple = (TNtuple*) rootfile->FindObjectAny("timentuple");
  TIME_EVENT time_event;
  ntuple->SetBranchAddress("timebranch",&time_event);
  int nentries = ntuple->GetEntries();
  int step = 1 + nentries/1000;
  for (int i=0; i<nentries; i += step) {
    ntuple->GetEntry(i);
    double time = time_event.Time + time_event.FineTime;
    GeoCoo pos;
    ISSPosition(time/60/60/24.-3651.,pos);
    double latitude  = pos.Lat; 
    double longitude = pos.Lon;
    trajectory->Fill(longitude,latitude);
  }
  // earth
  TH2F* earth = new TH2F("earth","; latitude; longitude",180,-180,180,161,-80.5,80.5);
  FILE* file = fopen(Form("%s/tkonline/data/earth.dat",getenv("AMSWD")),"read"); 
  if (!file) {
    printf("MonSlider::DrawOrbitFromTime()-W file tkonline/data/earth.dat not present. No Earth drawing!\n");
    trajectory->SetMarkerColor(kViolet);
    trajectory->Draw();
    canvas->Update();
    return;
  }
  while (!feof(file)) {
    float x,y;
    fscanf(file,"%f%f",&x,&y);
    earth->Fill(x,y);
  } 
  earth->SetStats(kFALSE);
  earth->Draw();
  trajectory->SetMarkerColor(kViolet);
  trajectory->Draw("SAME");
  canvas->Update();
}



// DrawRates


void MonSlider::DrawIsGood(int alternative) {
}


void MonSlider::DrawInfo(int alternative) {
  // clear
  canvas->Clear();
  canvas->cd(0);
  text->SetTextColor(kBlack);
  text->SetTextSize(0.03);
  // first-last event
  TNtuple* ntuple = (TNtuple*) rootfile->FindObjectAny("timentuple");
  if (ntuple!=0) { 
    TIME_EVENT time_event;
    ntuple->SetBranchAddress("timebranch",&time_event);
    ntuple->GetEntry(0);
    time_t in = (time_t) time_event.Time;
    ntuple->GetEntry(ntuple->GetEntries());
    time_t fi = (time_t) time_event.Time;
    text->DrawTextNDC(0.1,0.95,Form("First event %10d %s",(int)in,ctime(&in)));
    text->DrawTextNDC(0.1,0.90,Form("Last event %10d %s",(int)fi,ctime(&fi)));
  }  
  // header infos 
  TrHistoManHeader* header = (TrHistoManHeader*) rootfile->FindObjectAny("TrOnlineMonHeader");
  if (header!=0) {
    // list
    text->DrawTextNDC(0.1,0.83,Form("Number of files: %d",header->GetNFileNames()));
    for (int i=0; i<header->GetNFileNames(); i++) {
      text->DrawTextNDC(0.1,0.80-0.03*i,header->GetFileName(i));
    }  
    // list
    text->DrawTextNDC(0.6,0.83,Form("Number of runs: %d",header->GetNRunNumbers()));
    for (int i=0; i<header->GetNRunNumbers(); i++) {
      text->DrawTextNDC(0.6,0.80-0.03*i,Form("Run: %10d",header->GetRunNumber(i)));
    } 
  }
  canvas->Update();
  text->SetTextSize(0.05);
}

