//  $Id: main.h,v 1.4 2003/07/08 16:17:34 choutko Exp $
#include <TROOT.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TRootGuiFactory.h>
#include <TApplication.h>
#include <TGX11.h>
#include <TGX11TTF.h>
#include <THistPainter.h>
#include <TFitter.h>
#include <TUtilHist.h>
class Myapp : public TApplication{
protected:
AMSDisplay * fDisplay;
 private:
 THistPainter a;
 TUtilHist aa;
 TFitter b;
public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):TApplication(appClassName,argc, argv),fDisplay(0){
}
 void SetStatic(){
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
  }
 void SetDisplay(AMSDisplay* d){fDisplay=d;} 
};
