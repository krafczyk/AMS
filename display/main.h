//  $Id$
#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TRootGuiFactory.h>
#include <TApplication.h>
#ifdef WIN32
//#include <TGWin32.h>
//#include <TTF.h>
#else
#ifndef __APPLE__
#include <TGX11.h>
#include <TGX11TTF.h>
#endif
#endif
#include <THistPainter.h>
#include <TFitter.h>
class Myapp : public TApplication{
protected:
AMSDisplay * fDisplay;
 private:
// THistPainter a;
// TFitter b;
public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):TApplication(appClassName,argc, argv),fDisplay(0){
}
 void SetStatic(){
     if(gROOT->IsBatch()){
       cerr<<"MyApp::SetStatic-F-RootIsInBatchMode,quitting"<<endl;
       exit(1);
     }
#ifndef WIN32
#ifndef __APPLE__
       const char *ttpath = gEnv->GetValue("Root.TTFontPath","$(HOME)/ttf/fonts");
   if(ttpath && !getenv("OffmonNoTTF")){
    char file[512];
    strcpy(file,ttpath);
    strcat(file,"/arialbd.ttf");
    if(!access(file,kReadPermission)){
     cout << "Using TTF Fonts  from "<<ttpath<<endl;
     gVirtualX=new TGX11TTF(*(TGX11*)gVirtualX);
    }
   }
   else{
     cerr<<"Myapp-ctor-W-TrueTypeFontsWillNotBeUsed"<<endl;
   }
#endif
#endif
//    TVirtualFitter::SetFitter(&b);
//     gROOT->GetListOfTypes(true);
  }
 void SetDisplay(AMSDisplay* d){fDisplay=d;} 
};
