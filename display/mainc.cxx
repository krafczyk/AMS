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
#include <stdlib.h>
#include "main.h"


void Myapp::HandleIdleTimer(){
SetReturnFromRun(1);
Terminate();
}
extern void InitGui(); // loads the device dependent graphics system
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
int Error; // needed by Motif

TROOT root("AMS", "AMS ROOT", initfuncs);

int main(int argc, char *argv[])
{
// First create application environment. If you replace TApplication
// by TRint (which inherits from TApplication) you will be able
// to execute CINT commands once in the eventloop (via Run()).
Myapp *theApp = new Myapp("App", &argc, argv);
theApp->SetIdleTimer(6,"");
/*
// Do drawing and all everything else you want
TCanvas *c = new TCanvas("Picture", "The Alignment Geometry", 1024, 768);
c->Show();

TLine *l = new TLine(0.1,0.2,0.5,0.9);
l->Draw();

c->Update(); // force primitive drawn after c->Show() to be drawn in canvas
*/

  debugger.Off();
  char *filename="test.root";

  if ( argc > 1 ) {		// now take the file name
    filename = *++argv;
  }

  char *gtv=getenv("AMSEDDataDir");
  if(!gtv){
    cerr <<"amsedc-F-AMSEDDataDir Not defined"<<endl;
    exit(1);
  }
  char fnama[1024]="\0";
  char fnam[1024]="\0";
  char fnami[1024]="\0";
  char fnamo[1024]="\0";
  strcpy(fnama,gtv);
  strcat(fnama,"newroot");
  {  
  ifstream iftxt(fnama,ios::in);
  if(!iftxt){
     cerr <<"amsedc::init-F-ErrorOpenFile "<<fnama<<endl;
     return 1;
  }
  else {
    fnam[0]=0;
    strcpy(fnam,gtv);
    iftxt >> fnami;

    strcat(fnam,fnami);
    iftxt.close();
  }
  }
out:
  {  
   TFile f(fnam);
   TTree * t = (TTree *)f.Get("h1");

   if(!t){
     //if(argc <=1)cout <<"Please type file name as first parameter"<<endl;
    return 1;
   }

   AMSRoot amsroot("AMS", "AMS Display");
   cout <<" "<<endl;
   amsroot.Init(t);
   cout <<" "<<endl;
   cout <<"Open file "<<fnam<<endl;
   amsroot.MakeTree("AMSTree", "AMS Display Tree");
   TFile fgeo("ams_group.root");
   cout <<" "<<endl;
   TGeometry * geo = (TGeometry *)fgeo.Get("ams");
   AMSDisplay display("AMSRoot Event Display", geo);
       display.SetApplication(theApp);
       display.SetView (kTwoView);
       // display.SetView (kTwoView);
      for(int i=0;;i++){
       amsroot.Clear();
       if(!amsroot.GetEvent(i)){
        if(i){
          i=0;
          theApp->SetIdleTimer(30,"");
          continue;
        }
        else break;
       }
       if(!amsroot.IsGolden() && rand()%10 != 3)continue;
          display.DrawEvent();
          display.GetCanvas()->Update();	// force it to draw
          theApp->Run();
                    {
                      ifstream iftxt(fnama,ios::in);
                      if(iftxt){
                       fnamo[0]=0;
                       strcpy(fnamo,gtv);
                       iftxt >> fnami;
                       strcat(fnamo,fnami);
                       iftxt.close();
                       if(strcmp(fnamo,fnam)){
                        strcpy(fnam,fnamo);
                        //                        goto out;
                        goto end;
                       }
                      }
                    }
      }

  }
end:              
      return 0;
  


}


