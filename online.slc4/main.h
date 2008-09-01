//  $Id: main.h,v 1.2 2008/09/01 09:00:02 choutko Exp $
#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TRootGuiFactory.h>
#include <TApplication.h>
#ifdef WIN32
//#include <TTF.h>
#else
#include <TGX11.h>
#include <TGX11TTF.h>
#endif
#include <THistPainter.h>
#include <TFitter.h>
class Myapp : public TApplication{
protected:
AMSOnDisplay * fDisplay;
 private:
#ifndef WIN32
 THistPainter a;
 TFitter b;
#endif
public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):TApplication(appClassName,argc, argv),fDisplay(0){
}
 void SetStatic(){
#ifndef WIN32
       const char *ttpath = gEnv->GetValue("Root.TTFontPath","$(HOME)/ttf/fonts");
   if(ttpath && !getenv("OffmonNoTTF")){
    if(gSystem->Which(ttpath, "arialbd.ttf", kReadPermission)){
     cout << "Using TTF Fonts  from "<<ttpath<<endl;
     gVirtualX=new TGX11TTF(*(TGX11*)gVirtualX);
    }
   }
   else{
     cerr<<"Myapp-ctor-W-TrueTypeFontsWillNotBeUsed"<<endl;
   }
    TVirtualFitter::SetFitter(&b);
     gROOT->GetListOfTypes(true);
#endif
  }
 void SetDisplay(AMSOnDisplay* d){fDisplay=d;} 
};
