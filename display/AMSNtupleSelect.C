
#include "AMSNtupleHelper.h"

static AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;

class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
    static set <unsigned int> fmap;
    static int a=0;
    static int dupl=0;
    static unsigned int oldev=0;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise.
    // Example take the even event numbers
   if(ev){
    cout<<" ev "<<ev->NAntiCluster()<<endl;
    return true;
    bool next=ev->Event()!=oldev;
    bool breaks=ev->Event()-oldev!=1;
    if(breaks){
       cout<<" break "<<ev->Event()<<" "<<oldev<<endl;
    }
    oldev=ev->Event();
   if(fmap.find(ev->Event())!=fmap.end()){
         cout<< " dupl event found "<<ev->Event()<<" "<<++dupl<<endl;
       }
       else fmap.insert(ev->Event());
       if(next)return false;
       else return true;
   }
   return false;
}
};
//
//  The code below should not be modified
//
#ifndef WIN32
extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loadedd "<<endl;
}

#else
#include <windows.h>
BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle
             DWORD fdwReason,              // reason called 
             LPVOID lpvReserved)           // reserved 
{ 
 
  switch (fdwReason) 
    { 
      // The DLL is loading due to process 
      // initialization or a call to LoadLibrary. 
 
    case DLL_PROCESS_ATTACH: 
      fgHelper=new AMSNtupleSelect();
      cout <<"  Handle Loadedd "<<fgHelper<<endl;
      break;

    case DLL_PROCESS_DETACH: 
      if(fgHelper){
        delete fgHelper;
        fgHelper=0;
      } 
 
      break; 
 
    default: 
      break; 
    } 
  return TRUE;
}
extern "C" AMSNtupleHelper * gethelper(){return fgHelper;}

#endif
