#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeometry.h>
#include "AMSRoot.h"
#include "AMSDisplay.h"
#include "Debugger.h"
#include <iostream.h>
#include <fstream.h>
#include <sys/stat.h>
#include <sys/file.h>

extern void InitGui(); // loads the device dependent graphics system
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
int Error; // needed by Motif

TROOT root("AMS", "AMS ROOT", initfuncs);

void main(int argc, char *argv[])
{
// First create application environment. If you replace TApplication
// by TRint (which inherits from TApplication) you will be able
// to execute CINT commands once in the eventloop (via Run()).
TApplication *theApp = new TApplication("App", &argc, argv);

/*
// Do drawing and all everything else you want
TCanvas *c = new TCanvas("Picture", "The Alignment Geometry", 400, 400);
c->Show();

TLine *l = new TLine(0.1,0.2,0.5,0.9);
l->Draw();

c->Update(); // force primitive drawn after c->Show() to be drawn in canvas
*/

  debugger.Off();
  
  char * filename = "mc.root";		// default file name

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;
  }

  printf("opening file %s...\n", filename);
   TFile f(filename);
   TTree * t = (TTree *)f.Get("h1");
   if(!t){
    if(argc <=1)cout <<"Please type file name as first parameter"<<endl;
    return;
   }
   AMSRoot amsroot("AMS", "AMS Display");
   amsroot.Init(t);
   amsroot.MakeTree("AMSTree", "AMS Display Tree");
   TFile fgeo("ams_group.root");
   TGeometry * geo = (TGeometry *)fgeo.Get("ams");
   AMSDisplay display("AMSRoot Event Display", geo);
   amsroot.GetEvent(0);
   //   display.SetView (kTwoView);
      display.ShowNextEvent(-1);
      display.GetCanvas()->Update();	// force it to draw
   

  


// Enter event loop
  theApp->Run();

  delete theApp;
}
//---------------------------------------



