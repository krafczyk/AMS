//  $Id: main.h,v 1.8 2008/09/02 14:34:47 choutko Exp $
#include <TASImage.h>
#include <THistPainter.h>
#include <TGraphPainter.h>
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
  TASImage a;
  TVirtualHistPainter::fgPainter=(TClass*)new THistPainter() ;
  TVirtualGraphPainter::fgPainter= new TGraphPainter() ;
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
