//#define _PGTRACK_
#include "AMSNtupleHelper.h"
#include "../include/EcalChi2CY.h"
#include "../include/root_setup.h"
#include "../include/DynAlignment.h"
#include "../include/TrInnerDzDB.h"
#include <fstream>
#include "../include/TrdSCalib.h"
#include "../include/TrdKCluster.h"
#include "../include/Tofrec02_ihep.h"
#include "../include/bcorr.h"
 AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;
static ofstream iftxtp("./positrons.txt");
static ofstream iftxte("./electorns.txt");
class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
if(ev && ev->nParticle()){
 ParticleR &part=ev->Particle(0);
       static int clear=0;
       if(!clear++){
       ev->getsetup()->fISSCTRS.clear();
       ev->getsetup()->fISSGTOD.clear();
       }
  AMSSetupR::ISSCTRSR b; 
 AMSSetupR::ISSGTOD ab; 
 AMSPoint d2l;
 d2l[0]=ev->fHeader.RadS*cos(ev->fHeader.ThetaS)*cos(ev->fHeader.PhiS);
 d2l[1]=ev->fHeader.RadS*cos(ev->fHeader.ThetaS)*sin(ev->fHeader.PhiS);
 d2l[2]=ev->fHeader.RadS*sin(ev->fHeader.ThetaS);
 cout <<" tle "<<d2l<<endl;
 double xtime=ev->fHeader.UTCTime(1);
if(!ev->getsetup()->getISSCTRS(b,xtime)){
 AMSPoint dc;
 AMSPoint dcv;
   dc[0]=b.r*cos(b.theta)*cos(b.phi);
   dc[1]=b.r*cos(b.theta)*sin(b.phi);
   dc[2]=b.r*sin(b.theta);
   dcv[0]=b.v*b.r*cos(b.vtheta)*cos(b.vphi);
   dcv[1]=b.v*b.r*cos(b.vtheta)*sin(b.vphi);
   dcv[2]=b.v*b.r*sin(b.vtheta);
   cout << " dc ctrs "<<dc<<endl;
   cout << " dcv ctrs "<<dcv<<endl;
}
 
 if(!ev->getsetup()->getISSGTOD(ab,xtime)){
    AMSPoint dc2;
   dc2[0]=ab.r*cos(ab.theta)*cos(ab.phi);
   dc2[1]=ab.r*cos(ab.theta)*sin(ab.phi);
   dc2[2]=ab.r*sin(ab.theta);
   AMSPoint dc2v;
   dc2v[0]=ab.r*ab.v*cos(ab.vtheta)*cos(ab.vphi);
   dc2v[1]=ab.r*ab.v*cos(ab.vtheta)*sin(ab.vphi);
   dc2v[2]=ab.r*ab.v*sin(ab.vtheta);
   cout << " dc gtod "<<dc2<<endl;
   cout << " dcv gtod "<<dc2v<<endl;
   cout << " gtod tp "<<ab.r<<" "<<ab.theta<<" "<<ab.phi<<endl;
 }
AMSSetupR::GPSWGS84R gps;
if(!ev->getsetup()->getGPSWGS84(gps, xtime)){
cout <<" qqq "<<gps.r<<" "<<gps.v<<" "<<xtime<<" "<<endl;
cout <<" qqq "<<gps.phi<<" "<<gps.theta<<" "<<xtime<<" "<<endl;
    AMSPoint dc3;
   dc3[0]=gps.r*cos(gps.theta)*cos(gps.phi);
   dc3[1]=gps.r*cos(gps.theta)*sin(gps.phi);
   dc3[2]=gps.r*sin(gps.theta);
   AMSPoint  dc3v;
   dc3v[0]=gps.r*gps.v*cos(gps.vtheta)*cos(gps.vphi);
   dc3v[1]=gps.r*gps.v*cos(gps.vtheta)*sin(gps.vphi);
   dc3v[2]=gps.r*gps.v*sin(gps.vtheta);
   cout << " dc gps "<<dc3<<" " <<gps.r*sin(gps.theta)<<gps.r<<" "<<gps.theta<<endl;
   cout << " dcv gps "<<dc3v<<endl;

}
    AMSSetupR::AMSSTK stk;
    if (ev->getsetup() && !ev->getsetup()->getAMSSTK(stk, xtime)) {
      cout <<" stk "<<stk.cam_ra<<" "<<stk.cam_id<<endl;
    }
 return true;
}
else return false;


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
