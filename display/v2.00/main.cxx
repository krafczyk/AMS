
//--------------------------------------------------------------------------
//
// Usage:
//
//        amsed [-h] [-d -D] [-t seconds] [filename]
//  -h  =  print the usage above
//  -d  =  enable debugger.printing
//  -D  =  same as -d
//  -t  =  follows the IdleTimer(d=5) in seconds, Idle_cmd is IdleHandle.C(0)
//  filename = root filename produced via "h2root" from converted-ntuple
//
//--------------------------------------------------------------------------

#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeometry.h>
#include "AMSR_Root.h"
#include "AMSR_Display.h"
#include "Debugger.h"
#include <iostream.h>
#include <fstream.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>


extern void InitGui(); // loads the device dependent graphics system
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
int Error; // needed by Motif

TROOT root("AMS", "AMS ROOT", initfuncs);

main(int argc, char *argv[])
{
//=============> Processing options first  ===================================
  //
  //  Turn off the debugger printings by default
  //
  debugger.Off();

  Int_t idleSec = 5;  //default IdleTimer in seconds

  //
  // Turn on the debugger printings with option="-d" or "-D"
  //    and print the help on usage for option="-h"
  //
  char *pchar=0;
  if (argc>1) {
     while ( pchar =argv[1],*pchar=='-' ) {
        argv++;
        argc--;
        pchar++;
        if (*pchar=='d' || *pchar=='D') debugger.On();
        else if (*pchar=='h') {   // print the help on usage
          cout << "\nUsage: amsed [-h] [-d -D] [-t seconds] [filename]\n"
            << "  -h  =  print the usage above\n"
            << "  -d  =  enable debugger.printing\n"
            << "  -D  =  same as -d\n"
            << "  -t  =  follows the IdleTimer(d=5) in secconds, Idle_cmd is IdleHandle.C(0)\n"
            << "  filename = root filename produced via \"h2root\" from converted-ntuple\n"
            << endl;
           return 1;
        } else if (*pchar=='t') {  // follows IdleTimer in seconds
          idleSec = atoi(*++argv); argc--;
          cout << "IdleTimer set to " << idleSec << endl;
        } else cout << "Unknown option =" << *--pchar << endl;
     }
  }

//=============> Processing actions then ======================================

  // First create application environment. If you replace TApplication
  // by TRint (which inherits from TApplication) you will be able
  // to execute CINT commands once in the eventloop (via Run()).
  int nargs=0;
  TApplication *theApp = new TApplication("App", &nargs, 0);
  //  TApplication *theApp = new Myapp("App", &argc, argv);

/*
  // Do drawing and all everything else you want
  TCanvas *c = new TCanvas("Picture", "The Alignment Geometry", 1024, 768);
  c->Show();

  TLine *l = new TLine(0.1,0.2,0.5,0.9);
  l->Draw();

  c->Update(); // force primitive drawn after c->Show() to be drawn in canvas
*/

//  debugger.Off();
  
  //
  //AMSR_Root
  //-------
  AMSR_Root amsroot("AMS", "AMS Display");

  //
  //AMSR_Display
  //----------
  TFile fgeo("ams_group.root");
  TGeometry * geo = (TGeometry *)fgeo.Get("ams");
  AMSR_Display display("AMSR_Root Event Display", geo,1024,768);
  display.SetNextView (kTwoView);

  //
  // Load "IdleHandle()"
  //
  theApp->ProcessLine(".L IdleHandle.C");

  //
  //Open data file
  //
  char *filename = 0;

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;
    amsroot.OpenDataFile(filename);
    amsroot.GetEvent(0);
    display.ShowNextEvent(0);
    display.GetCanvas()->Update();        // force it to draw
  } else {
    display.GetCanvas()->OpenFileCB();
  }

  //
  // Initialize "IdleHandle()"
  //
  theApp->ProcessLine("IdleHandle(-1)");


  //
  // display next event automatically after each idleSec(d=5) seconds
  //
  display.SetIdleTimer(idleSec,"IdleHandle(0)");
//  display.SetIdleTimer(5,"gAMSR_Display->ShowNextEvent(1)");

  //
  // Enter event loop
  //
  theApp->Run(1);

  // End "IdleHandle()"
  theApp->ProcessLine("IdleHandle(1)");

//  delete theApp;

  return 3;
  
}
//---------------------------------------

