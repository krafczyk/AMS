
//--------------------------------------------------------------------------
//
// Usage:
//
//        amsed [-h] [-d/D] [-tseconds] [-intupleID] [filename [filetype]]
//  -h  =  print the usage above
//  -d  =  enable debugger.printing
//  -D  =  same as -d
//  -t  =  follows the IdleTimer(d=5) in seconds, Idle_cmd is IdleHandle.C(0)
//  -i  =  set ntupleID, 1 by default
//  filename = if no filetype provided,
//               file extension "root*" for ROOT data type,
//               file extension "ntp*" or "hbk*" for ntuple data type
//               others for Objectivity data type
//  filetype = 0 or "root"   for ROOT data type
//           = 1 or "db"     for Objectivity data type
//           = 2 or "ntp"    for ntuple data type
//--------------------------------------------------------------------------

#include <TROOT.h>
#include <TApplication.h>
#include <TFile.h>
//#include <TTree.h>
#include <TGeometry.h>
#include "AMSR_Root.h"
#include "AMSR_Ntuple.h"
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
  Int_t idNull = -1;
  Int_t ntupleID = idNull; //initial value

  //
  // Turn on the debugger printings with option="-d" or "-D"
  //    and print the help on usage for option="-h"
  //

  for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--))
  {
    pchar++;
    if (*pchar=='d' || *pchar=='D') debugger.On();
    else if (*pchar=='h') {   // print the help on usage
      cout << "\nUsage: amsed [-h] [-d/D] [-tseconds] [-intupleID] [filename [filetype]]\n"
        << "  -h  =  print the usage above\n"
        << "  -d  =  enable debugger.printing\n"
        << "  -D  =  same as -d\n"
        << "  -t  =  follows the IdleTimer(d=5) in secconds, Idle_cmd is IdleHandle.C(0)\n"
        << " filename = if no filetype provided,\n"
        << "               file extension \"root*\" for ROOT data type,\n"
        << "               file extension \"ntp*\" or \"hbk*\" for ntuple data type\n"
        << "               others for Objectivity data type\n"
        << " filetype = 0 or \"root\"   for ROOT data type\n"
        << "          = 1 or \"db\"     for Objectivity data typ\n"
        << "          = 2 or \"ntp\"    for ntuple data type\n"
        << endl;
       return 1;
    } else if (*pchar=='t') {  // follows IdleTimer in seconds
      idleSec = atoi(++pchar);
      cout << "IdleTimer set to " << idleSec << endl;
    } else if (*pchar=='i') {  // follows NtupleID
      ntupleID = atoi(++pchar);
      cout << "NtupleID set to " << ntupleID << endl;
    } else cout << "Unknown option =" << *--pchar << endl;
  }

//=============> Processing actions then ======================================

  // First create application environment. If you replace TApplication
  // by TRint (which inherits from TApplication) you will be able
  // to execute CINT commands once in the eventloop (via Run()).
  int nargs=0;
  TApplication *theApp = new TApplication("App", &nargs, 0);
//  TApplication *theApp = new TApplication("App", &argc, argv);

  //
  //AMSR_Root
  //-------
  AMSR_Root amsroot("AMS", "AMS Display");

  //
  //Get the Geometry from the ROOT file
  //        --------
  char *geo_dir=getenv("AMSGeoDir");
  if ( !geo_dir || strlen(geo_dir) == 0 ) {
    cerr <<"AMSR_Display-F-AMSGeoDir Not Defined. Exit(1)"<<endl;
    exit(1);
  }

  char geoFile[256];
  strcpy(geoFile,geo_dir);

  int version = gROOT->GetVersionInt();
  char *geoFile_old = "/ams_group.root2.08";
  char *geoFile_new = "/ams_group.root";

  if ( version < 20009 ) strcat(geoFile,geoFile_old);
  else strcat(geoFile,geoFile_new);;
  TFile fgeo(geoFile);

  TGeometry *geo = (TGeometry *)fgeo.Get("ams");

  //
  //AMSR_Display
  //----------
  AMSR_Display display("AMSR_Root Event Display", geo,1024,768);
  display.SetNextView (kTwoView);

  //
  // Load "IdleHandle()"
  //
//  theApp->ProcessLine(".L IdleHandle.C");
  gROOT->LoadMacro("IdleHandle.C");
//  char idleHandle[256];
//  strcpy(idleHandle,geo_dir);
//  strcat(idleHandle,"/IdleHandle.C");
//  gROOT->LoadMacro(idleHandle);

  //
  // Set ntupleID if necessary
  //
  if (ntupleID != idNull && ntupleID > 0)
     amsroot.GetNtuple()->SetNtupleID(ntupleID);

  //
  // display next event automatically after each idleSec(d=5) seconds
  //
  display.SetIdleTimer(idleSec,"IdleHandle(0)");

  //
  //Open data file
  //
  char *filename;
  char *ctype;
  EDataFileType type = kUnknown;

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;

    if ( argc == 2 ) {        // one arguement, data type got from filename
      type = kUnknown;
    } else {
      ctype = *++argv;
      if ( strcmp(ctype,"0")==0 || strcmp(ctype,"root")==0) type = kRootFile;
      else if ( strcmp(ctype,"1")==0 || strcmp(ctype,"db")==0) type = kObjectivityFile;
      else if ( strcmp(ctype,"2")==0 || strcmp(ctype,"ntp")==0) type = kNtupleFile;
      else {
        cerr << "unknown file type =" << ctype << "!" << endl;
        return 3;
      }
    }

    amsroot.OpenDataFile(filename, type);
//    amsroot.GetEvent(0);
    if ( display.IdleTime() <= 0 || display.IdleCommand() == 0 )
      display.ShowNextEvent(0);
    display.DrawEvent();
    display.GetCanvas()->Update();        // force it to draw
  } else {
    display.GetCanvas()->OpenFileCB();
  }

  //
  // Initialize "IdleHandle()"
  //
  theApp->ProcessLine("IdleHandle(-1)");

  //
  // Enter event loop
  //
  theApp->Run();

  // End "IdleHandle()"
  theApp->ProcessLine("IdleHandle(1)");

  delete theApp;

  return 3;
  
}
//---------------------------------------

