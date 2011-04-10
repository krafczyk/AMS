#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TrHistoMan.h"
#include "TkDBc.h"

#include "MonitorUI.h"
#include "GenericSlider.h"
#include "MonSlider.h"
#include "CalSlider.h"

#include "TFile.h"
#include "TTree.h"

#include <cstring>

int main(int argc, char **argv){
  if (argc<2) {
    printf("Usage: %s <File> [All/Cal/Mon=0/1/2] [GraphNumber] [HwId]\n",argv[0]);
    return 0;
  }
  // type
  int type = 0;
  if (argc>2) type = atoi(argv[2]);
  // graphtype
  int graphtype = 1;
  if (argc>3) graphtype = atoi(argv[3]);
  // hwid
  int hwid = 0;
  if (argc>4) hwid = atoi(argv[4]);
  // create database
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  // set style
  MonitorUI::SetStyle(); 
  // open root file 
  TFile* file = new TFile(argv[1],"read");
  if ( (file->FindObjectAny("TrCalDB")!=0)&&( (type==0)||(type==1) ) ) {
    CalSlider* calslider = new CalSlider("CalSlider","CalSlider");
    calslider->setRootFile(argv[1]);
    calslider->selectGraph(0,graphtype);
    TrCalDB* trcaldb = (TrCalDB*) file->FindObjectAny("TrCalDB");
    calslider->showHwId(hwid);
    calslider->GetCanvas()->Print(Form("Cal_%010d_%02d_%03d.png",trcaldb->GetRun(),graphtype,hwid));
  }
  if ( (file->FindObjectAny("TrOnlineMon")!=0)&&(file->FindObjectAny("timentuple")!=0)&&( (type==0)||(type==2) ) ) {
    MonSlider* monslider = new MonSlider("MonSlider","MonSlider");
    monslider->setRootFile(argv[1]);
    monslider->selectGraph(0,graphtype);    
    monslider->showHwId(hwid);
    TNtuple* ntuple = (TNtuple*) file->FindObjectAny("timentuple");
    TrHistoManHeader* header = (TrHistoManHeader*) file->FindObjectAny("TrOnlineMonHeader");
    if ( (header->GetNRunNumbers()>0)&&(header->GetNFileNames()>0) ) {
      // first event time
      TIME_EVENT time_event;
      ntuple->SetBranchAddress("timebranch",&time_event);
      ntuple->GetEntry(0);
      int first_time = (int) time_event.Time;
      // first run number 
      int run_number = header->GetRunNumber(0);
      // parse file name to find block numbers
      const char* pointer1 = strrchr(header->GetFileName(0),'/')-4; 
      char copy1[5];
      strncpy(copy1,pointer1,4);
      copy1[4] = '\0';
      int dir = atoi(copy1);
      const char* pointer2 = strrchr(header->GetFileName(0),'/')+1;
      char copy2[4];
      strncpy(copy2,pointer2,3);
      copy2[3] = '\0';
      int block = atoi(copy2);
      // print
      monslider->GetCanvas()->Print(Form("Mon_%04d%03d_%010d_%010d_%02d_%03d.png",dir,block,run_number,first_time,graphtype,hwid));
    } 
  }
  return 0;
}

