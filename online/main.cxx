#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeometry.h>
#include "AMSDisplay.h"
#include "Debugger.h"
#include <iostream.h>
#include <fstream.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include "main.h"
#include "AMSAntiHist.h"
#include "AMSTrackerHist.h"
#include "AMSTOFHist.h"
#include "AMSLVL1Hist.h"
#include "AMSLVL3Hist.h"
#include "AMSCTCHist.h"
#include "AMSAxAMSHist.h"
#include "AMSGenHist.h"

void Myapp::HandleIdleTimer(){
  SetReturnFromRun(1);
  Terminate();
}
extern void InitGui(); // loads the device dependent graphics system
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
int Error; // needed by Motif

TROOT root("AMS", "AMS ROOT", initfuncs);

void main(int argc, char *argv[])
{
// First create application environment. If you replace TApplication
// by TRint (which inherits from TApplication) you will be able
// to execute CINT commands once in the eventloop (via Run()).
  Myapp *theApp = new Myapp("App", &argc, argv);
  theApp->SetIdleTimer(15,"");

  debugger.Off();
  
  char * filename = "test.root";		// default file name

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;
  }

  printf("opening file %s...\n", filename);
  TFile f(filename);
  AMSOnDisplay * amd= new AMSOnDisplay("AMSRoot Online Display",&f);
  AMSAntiHist  antih("ANTI","Anti counter Hists",1,1);
  amd->AddSubDet(antih);
  AMSTrackerHist  trackerh("Tracker","Tracker  Hists",5,1);
  amd->AddSubDet(trackerh);
  AMSLVL1Hist  LVL1h("LVL1","LVL1  Hists",4,1);
  amd->AddSubDet(LVL1h);
  AMSLVL3Hist  LVL3h("LVL3","LVL3  Hists",2,1);
  amd->AddSubDet(LVL3h);
  AMSTOFHist  TOFh("TOF","TOF  Hists",9,1);
  amd->AddSubDet(TOFh);
  AMSCTCHist  CTCh("CTC","CTC  Hists",2,1);
  amd->AddSubDet(CTCh);
  AMSAxAMSHist  AxAMSh("AxAMS","AxAMS  Hists",2,1);

  AMSGenHist genh("General", "General Hists",1,1);
  amd->AddSubDet(genh);

  amd->AddSubDet(AxAMSh);
  amd->Init();
  amd->SetApplication(theApp);
  amd->Begin()=0;
  amd->Sample()=900;
  int Sample=1000;
      for(;;){
        amd->Fill();
        amd->DispatchProcesses();  
        theApp->Run();
        
      }        
      return ;
  


}
//---------------------------------------



