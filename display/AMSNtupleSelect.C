
#include "AMSNtupleHelper.h"

static AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;

class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
    static int a=0;
      cout << "qq "<<endl;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise
//   if(ev && (ev->fStatus &3) &&( (ev->fStatus>>4 &1)))return true;
//   else return false;
     if(ev   && ev->nParticle() && ev->nTrTrack() ){
           cout << ev->NTrTrack()  << " "<<ev->nParticle()<<endl;
           return true;
         }
     else return false;
  
  try{
    if(ev && ev->nDaqEvent() && ev->nParticle() &&  !ev->nTrdTrack() && !ev->nTrTrack() &&ev->nEcalShower() ){
       
     if(ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>1 && ev->pDaqEvent(0)->Length<5000){
        cout <<++a<<endl;
return false; 
      }
      else return false;
    if(ev->Beta(0).Pattern==0 && ev->Particle(0).Mass>1.5 && fabs(ev->Particle(0).Momentum)<5 && ev->Particle(0).Beta>0 && ev->Particle(0).Beta<0.92){
cout <<ev->Event()<<endl;
return true;
} 
      if(ev->pLevel1(0)->TrigTime[4]<160 || ev->pDaqEvent(0)->Tdr>9000)return false;
      else if(ev &&ev->nParticle() && ev->nTrTrack() &&ev->nEcalShower() &&ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>0.1)return false;
      else if (ev &&ev->nParticle() && ev->nTrTrack() && ev->Particle(0).Charge>1 && ev->nTofCluster()>2)return false;
       else return false;
    }
    else{
      return false;
    }
  }
  catch (...)
{
return false;
}
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
