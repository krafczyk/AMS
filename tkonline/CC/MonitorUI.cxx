#include <TFile.h>
#include <TrCalDB.h>
#include <TrLadCal.h>
#include <TGClient.h>
#include <TApplication.h>
#include <TGMenu.h>
#include <TGButton.h>
#include <TGFileDialog.h>
#include <TGInputDialog.h>

#include "GenericSlider.h"
#include "MonitorUI.h"

#define M_FILE_OPEN_CAL 0
#define M_FILE_OPEN_MON 1
#define M_FILE_OPEN_REF 2
#define M_CAL_DB        3
#define M_FILE_EXIT     4
#define M_PRINT_THIS    5
#define M_PRINT_ALL     6

ClassImp(MonitorUI);


MonitorUI::MonitorUI(const TGWindow *p,UInt_t w,UInt_t h,char *filename) : TGMainFrame(p,w,h) {
  // add menu bar
  TGMenuBar *menubar = new TGMenuBar(this,100,20,kHorizontalFrame);
  TGPopupMenu *menufile = new TGPopupMenu();
  menufile->AddEntry("Open &Calibration File",M_FILE_OPEN_CAL);
  menufile->AddEntry("Open &Monitoring File",M_FILE_OPEN_MON);
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
  if (strlen(filename)!=0) HandleFile(filename);
  // finish 
  SetWindowName("MonitorUI");
  MapSubwindows();
  MoveResize(1200,700);
  MapWindow();
}


void MonitorUI::HandleFile(char* filename, int ref) {
  TFile* file = TFile::Open(filename,"r");
  if (file==0) { 
    printf("MonitorUI::HandleFile-W cannot open the file (%s), please try again using the File::Open menu.\n",filename); 
    return; 
  }
  if ( (ref==0)&&(file->FindObjectAny("TrCalDB")!=0) ) {
    gs->setSlider(GenericSlider::kCalSlider);
    gs->setRootFile(filename);
  }
  else if ( (ref==1)&&( (file->FindObjectAny("TrOnlineMon")!=0)||(file->FindObjectAny("Size_all")!=0) ) ) {
    gs->setSlider(GenericSlider::kMonSlider);
    gs->setRootFile(filename);
  }
  else if (ref==2) {
    gs->setRefFile(filename);
  }
  file->Close();
  if (gs->getSlider()==0) {
    printf("MonitorUI::HandleFile-W file not recognized in (%s), please try again using the File::Open menu.\n",filename);
    return;
  }
}


void MonitorUI::HandleMenu(Int_t id){
  switch (id) {
  case M_FILE_OPEN_CAL:
  case M_FILE_OPEN_MON:
  case M_FILE_OPEN_REF:
    {
      const char *ft[] = {"ROOT Files","*.root",0,0};
      TGFileInfo fi;
      fi.fFileTypes = ft;
      fi.fIniDir = NULL;
      new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&fi);
      if      (id==M_FILE_OPEN_CAL) HandleFile(fi.fFilename,0);
      else if (id==M_FILE_OPEN_MON) HandleFile(fi.fFilename,1);
      else if (id==M_FILE_OPEN_REF) HandleFile(fi.fFilename,2);
    }
    break;
  case M_CAL_DB:
    {
      char rrun[20];
      TGInputDialog* aa = new TGInputDialog(gClient->GetRoot(), 0,"Insert CAL run number","1259011030",rrun);
      printf("Insert here a dialog box for the number\n");
      gs->setCalfromDB(atoi(rrun));
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


int main(int argc, char **argv){
  char filename[200] = "";
  if (argc>=2) strcpy(filename,argv[1]);
  // main application
  TApplication app("App",&argc,argv);
  new MonitorUI(gClient->GetRoot(),200,200,filename);
  app.Run();
  return 0;
}

