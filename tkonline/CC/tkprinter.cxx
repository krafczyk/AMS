#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TkDBc.h"

#include "MonitorUI.h"
#include "GenericSlider.h"
#include "MonSlider.h"
#include "CalSlider.h"

#include "TFile.h"

int main(int argc, char **argv){
  if (argc<2) {
    printf("Usage: %s <File> [GraphNumber]\n",argv[0]);
    return 0;
  }
  // create database
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  // set style
  MonitorUI::SetStyle(); 
  // open root file 
  TFile* file = new TFile(argv[1],"read");
  if (file->FindObjectAny("TrCalDB")!=0) {
    CalSlider* calslider = new CalSlider("CalSlider","CalSlider");
    calslider->setRootFile(argv[1]);
    if (argc>2) calslider->selectGraph(0,atoi(argv[2]));
    calslider->GetCanvas()->Print(Form("%s_cal.png",argv[1]));
  }
  if (file->FindObjectAny("TrOnlineMon")!=0) {
    MonSlider* monslider = new MonSlider("MonSlider","MonSlider");
    monslider->setRootFile(argv[1]);
    if (argc>2) monslider->selectGraph(0,atoi(argv[2]));    
    monslider->GetCanvas()->Print(Form("%s_mon.png",argv[1]));
  }
  return 0;
}

