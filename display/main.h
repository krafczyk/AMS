//  $Id: main.h,v 1.5 2003/09/26 11:06:53 choutko Exp $
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

    TVirtualFitter::SetFitter(&b);
     gROOT->GetListOfTypes(true);
  }
 void SetDisplay(AMSDisplay* d){fDisplay=d;} 
};
