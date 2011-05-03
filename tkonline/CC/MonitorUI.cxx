#include "TkDBc.h"

#include "GenericSlider.h"
#include "MonitorUI.h"

#include <TFile.h>
#include <TrCalDB.h>
#include <TrLadCal.h>
#include <TGClient.h>
#include <TApplication.h>
#include <TGMenu.h>
#include <TGButton.h>
#include <TGFileDialog.h>
#include <TGInputDialog.h>

#define M_FILE_OPEN     0
#define M_FILE_OPEN_REF 1
#define M_CAL_DB        2
#define M_FILE_EXIT     3
#define M_PRINT_THIS    4
#define M_PRINT_ALL     5

ClassImp(MonitorUI);


TStyle* MonitorUI::style = 0;


MonitorUI::MonitorUI(const TGWindow *p,UInt_t w,UInt_t h,char *filename) : TGMainFrame(p,w,h) {
  // DB init
  TkDBc::CreateTkDBc();
  TkDBc::Head->init(3);
  SetStyle();
  // add menu bar
  TGMenuBar *menubar = new TGMenuBar(this,100,20,kHorizontalFrame);
  TGPopupMenu *menufile = new TGPopupMenu();
  menufile->AddEntry("Open &File",M_FILE_OPEN);
  menufile->AddEntry("Open Calibration from &DB",M_CAL_DB);
  menufile->AddEntry("Open &Reference File",M_FILE_OPEN_REF);
  menufile->AddSeparator();
  menufile->AddEntry("&Exit",M_FILE_EXIT);
  menufile->Connect("Activated(Int_t)","MonitorUI",this,"HandleMenu(Int_t)");
  menubar->AddPopup("&File",menufile,new TGLayoutHints(kLHintsTop | kLHintsLeft));
  TGPopupMenu *menuprint = new TGPopupMenu();
  menuprint->AddEntry("Print &This",M_PRINT_THIS);
  menuprint->AddEntry("Print &All",M_PRINT_ALL);
  menuprint->Connect("Activated(Int_t)","MonitorUI",this,"HandleMenu(Int_t)");
  menubar->AddPopup("&Print",menuprint,new TGLayoutHints(kLHintsTop | kLHintsLeft));
  AddFrame(menubar,new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  // build vertical panel
  TGVerticalFrame *vp = new TGVerticalFrame(this);
  // build mainapp to pass pointer to components
  gs = new GenericSlider(vp);
  // add mainapp (canvas,buttons)
  vp->AddFrame(gs,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  // timer
  timer = new TimerPanel(vp,"Timer");
  vp->AddFrame(timer,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
  // interaction (HwId)
  intpan = new InteractionPanel(vp,"HwId",gs);
  vp->AddFrame(intpan,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
  // tracker schematics (TkId)
  track = new TrackerPanel(vp,"TkId",gs);
  vp->AddFrame(track,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
  // search
  search = new SearchPanel(vp,"Search",gs);
  vp->AddFrame(search,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
  AddFrame(vp,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  // connect signals
  timer->Connect("Update()","GenericSlider",gs,"Update()");
  gs->Connect("Updated()","InteractionPanel",intpan,"Update()");
  gs->Connect("Updated()","TrackerPanel",track,"Update()");
  // open file if provided in command line
  if (strlen(filename)!=0) HandleFile(filename,M_FILE_OPEN);
  // finish 
  SetWindowName("MonitorUI");
  MapSubwindows();
  MoveResize(1200,700);
  MapWindow();
}


void MonitorUI::HandleFile(char* filename, int ref) {
  gs->Clear();
  if(strcmp(filename,"-W")==0){    
    gs->addSlider(GenericSlider::kCalSlider);
    gs->setRootFile(filename);
    return;
  }else{
  TFile* file = TFile::Open(filename,"r");
  if (file==0) { 
    printf("MonitorUI::HandleFile-W cannot open the file (%s), please try again using the File::Open menu.\n",filename); 
    return; 
  }
  if ( (ref==M_FILE_OPEN)&&(file->FindObjectAny("TrCalDB")!=0) ) {
    gs->addSlider(GenericSlider::kCalSlider);
    gs->setRootFile(filename);
  }
  if ( (ref==M_FILE_OPEN)&&( (file->FindObjectAny("TrOnlineMon")!=0)||(file->FindObjectAny("Size_all")!=0) ) ) {
    gs->addSlider(GenericSlider::kMonSlider);
    gs->setRootFile(filename);
  }
  if (ref==M_FILE_OPEN_REF) {
    gs->setRefFile(filename);
  }
  file->Close();
  if (gs->getnSlider()==0) {
    printf("MonitorUI::HandleFile-W file not recognized in (%s), please try again using the File::Open menu.\n",filename);
    return;
  }
  }
}


void MonitorUI::HandleMenu(Int_t id){
  switch (id) {
  case M_FILE_OPEN:
  case M_FILE_OPEN_REF:
    {
      const char *ft[] = {"ROOT Files","*.root",0,0};
      TGFileInfo fi;
      fi.fFileTypes = ft;
      fi.fIniDir = NULL;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&fi);
      HandleFile(fi.fFilename,id);
    }
    break;
  case M_CAL_DB:
    {
      //      gs->addSlider(GenericSlider::kCalSlider);
      HandleFile("-W",0);
    }
    break;
  case M_FILE_EXIT:
    gApplication->Terminate(0);
    break;
  case M_PRINT_THIS:
    gs->PrintThis();
    break;
  case M_PRINT_ALL:
    gs->PrintAll();
    break;
  }
}


void MonitorUI::CloseWindow() { 
  gApplication->Terminate(0); 
}


void MonitorUI::SetStyle() {
  // My graphic style 
  if (style==0) style = new TStyle("style","my style");
  // Colors 
  style->SetFillColor(1);
  style->SetFillStyle(1001);
  // Canvas & Pad (Title & Stat & Legend)
  style->SetCanvasColor(kWhite);
  style->SetCanvasBorderMode(0);
  style->SetCanvasBorderSize(1);
  style->SetPadColor(0);
  style->SetPadBorderMode(0);
  style->SetPadBorderSize(1);
  style->SetLegendBorderSize(1);
  style->SetTitleBorderSize(1);
  style->SetTitleFillColor(0);
  style->SetStatBorderSize(1);
  style->SetStatColor(0);
  style->SetTitleBorderSize(1);
  style->SetFrameFillColor(0);
  style->SetFrameFillStyle(0);
  // Position and dimension
  style->SetTitleOffset(0.9,"xyz");
  style->SetTitleSize(0.05,"xyz");
  style->SetTitleSize(0.03," ");
  // Stat option
  style->SetOptStat("ourme");
  // Create linear gradient color table
  TColor* Gradient = new TColor();
  Gradient->InitializeColors();
  const Int_t NRGBs = 5;
  const Int_t NCont = 40;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  Gradient->CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  style->SetNumberContours(NCont);
  // Set
  gROOT->SetStyle("style");
  gROOT->ForceStyle(kTRUE);
}

