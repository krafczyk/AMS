
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
    AMSEventR*pev=ev;
    if(pev && (!pev->NBetaH() && pev->nTrTrack() )){
      cout <<pev->nTrTrack()<<" "<<pev->NBetaH()<<" "<<pev->NTrTrack()<<" "<<pev->NTrTrackG()<<endl;
      return true;
    }
    return false;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise.
    // Example take the even event numbers


   if(ev){
     if(ev->nParticle()){
        if(ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).Charge && fabs(ev->Particle(0).Momentum/ev->Particle(0).Charge)>100){
          TrTrackR tr=ev->TrTrack(ev->Particle(0).iTrTrack());
          TkDBc::UseLatest();
          int it0=tr.iTrTrackPar(1,0,1);
          cout <<" r "<<tr.GetRigidity(it0)<<endl;
          int it1=tr.iTrTrackPar(1,0,1);
          int it3=tr.iTrTrackPar(1,0,3);
           cout <<tr.GetRigidity(it1)<<" "<<tr.GetRigidity(it3)<<endl;
          return true;
        }
    }
    return false;
    bool next=ev->Event()!=oldev;
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
