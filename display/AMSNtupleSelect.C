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
if(ev->nTrTrack() && ev->Particle(0).iTrTrack()>=0){
    TrTrackR & tr=ev->TrTrack( ev->Particle(0).iTrTrack());
    if(fabs(tr.Rigidityf())<20)return false; 
     TrRecHitR *hit1=tr.GetHitLJ(1);
    TrRecHitR *hit9=tr.GetHitLJ(9);
    if(!hit1 || ! hit9)return false;
    if(ev->Particle(0).Charge<3)return false;
    for(int refit=1;refit<6;refit++){
    cout << " refit "<<refit<<endl;
    int rf=refit;
    if(refit>=4){
     rf=3;
    TrClusterR::SetLinearityCorrection();
    //if(refit==5)TkDBc::UseLatest();
    } 
    else  TrClusterR::UnsetLinearityCorrection();
     TrRecHitR *hit1=tr.GetHitLJ(1);
    TrRecHitR *hit9=tr.GetHitLJ(9);
    float q1=0;
    // Get the difference between the two alignments
    AMSPoint diff1,diff9,diff1a,diff9a;
    int lpat=0;
    if(hit1) {
         lpat=1;
          //q1=tr.GetLayerJQ(1);
        cout <<" q1 "<<q1<<endl;
        diff1a=hit1->GetCoord(-1,4);
        cout <<" diff1a "<<diff1a<<endl;
    }
    if(hit9) {
      lpat+=100000000;
        diff9a=hit9->GetCoord(-1,4);
        cout <<" diff9a "<<diff9a<<endl;
    }
       int pattern=tr.Pattern(1111111);
       int it7=tr.iTrTrackPar(1,pattern,rf);  //inner
       pattern=tr.Pattern(11111111);
       int it8=tr.iTrTrackPar(21,pattern,rf); // inner+1
       int it81=tr.iTrTrackPar(1,pattern,rf); // inner+1
       int it82=tr.iTrTrackPar(2,pattern,rf); // inner+1
       int it83=tr.iTrTrackPar(3,pattern,rf); // inner+1
       int it8q=tr.iTrTrackPar(1,5,rf);
       pattern=tr.Pattern(101111111);
       int it91=tr.iTrTrackPar(1,pattern,rf); // inner+1
       int it92=tr.iTrTrackPar(2,pattern,rf); // inner+1
       int it93=tr.iTrTrackPar(3,pattern,rf); // inner+1
       int it9=tr.iTrTrackPar(21,pattern,rf); //inner+9
       pattern=tr.Pattern(111111111);    
       int it10=tr.iTrTrackPar(21,pattern,rf);  //full span
       int it11=tr.iTrTrackPar(1,pattern,10+rf);
       int it11a=tr.iTrTrackPar(1,pattern,rf);
       int it11f=tr.iTrTrackPar(1,0,rf);
       int it12=tr.iTrTrackPar(3,pattern,20+rf);
          int itav=tr.iTrTrackPar(1,pattern,20+rf);
         int nxy=tr.gTrTrackPar(itav).NdofY;;
         cout <<" nxy "<<nxy;
      double rav=itav>=0?tr.gTrTrackPar(itav).Rigidity:-1;
      double rpg=it11a>=0?tr.gTrTrackPar(it11a).Rigidity:-1;
      double rmd=it11>=0?tr.gTrTrackPar(it11).Rigidity:-1;
      double rin=it7>=0?tr.gTrTrackPar(it7).Rigidity:-1;
      double rin1=it8>=0?tr.gTrTrackPar(it8).Rigidity:-1;
      double rin11=it81>=0?tr.gTrTrackPar(it81).Rigidity:-1;
      double rin12=it82>=0?tr.gTrTrackPar(it82).Rigidity:-1;
      double rin13=it83>=0?tr.gTrTrackPar(it83).Rigidity:-1;
      double crin11=it81>=0?tr.gTrTrackPar(it81).Chisq:-1;
      double crin12=it83>=0?tr.gTrTrackPar(it82).Chisq:-1;
      double crin13=it82>=0?tr.gTrTrackPar(it83).Chisq:-1;
      double erin11=it81>=0?tr.gTrTrackPar(it81).ErrRinv:-1;
      double erin12=it83>=0?tr.gTrTrackPar(it82).ErrRinv:-1;
      double erin13=it83>=0?tr.gTrTrackPar(it83).ErrRinv:-1;
      double rin91=it91>=0?tr.gTrTrackPar(it91).Rigidity:-1;
      double rin92=it92>=0?tr.gTrTrackPar(it92).Rigidity:-1;
      double rin93=it93>=0?tr.gTrTrackPar(it93).Rigidity:-1;
      double crin91=it91>=0?tr.gTrTrackPar(it91).Chisq:-1;
      double crin92=it92>=0?tr.gTrTrackPar(it92).Chisq:-1;
      double crin93=it93>=0?tr.gTrTrackPar(it93).Chisq:-1;
      double erin91=it91>=0?tr.gTrTrackPar(it91).ErrRinv:-1;
      double erin92=it92>=0?tr.gTrTrackPar(it92).ErrRinv:-1;
      double erin93=it93>=0?tr.gTrTrackPar(it93).ErrRinv:-1;
      double rin1q=it8q>=0?tr.gTrTrackPar(it8q).Rigidity:-1;
      double rin9=it8>=0?tr.gTrTrackPar(it9).Rigidity:-1;
      double rfull=it11f>=0?tr.gTrTrackPar(it11f).Rigidity:-1;
      double rin19=it10>=0?tr.gTrTrackPar(it9).Rigidity:-1;
      cout <<" r " <<rmd<<" "<<rin<<" "<<rin1<<" "<<rin9<<" "<<rin19<<" "<<rav<<" "<<rin11<<" "<<rin13<<" "<<rin1q<<" "<<rpg<<" "<<rfull<<endl;
      cout <<" in+1 "<<rin11<<" "<<rin12<<" "<<rin13<<endl;
      cout <<" cin+1 "<<crin11<<" "<<crin12<<" "<<crin13<<endl;
      cout <<" ein+1 "<<erin11<<" "<<erin12<<" "<<erin13<<endl;
      cout <<" in+9 "<<rin91<<" "<<rin92<<" "<<rin93<<endl;
      cout <<" cin+9 "<<crin91<<" "<<crin92<<" "<<crin93<<endl;
      cout <<" ein+9 "<<erin91<<" "<<erin92<<" "<<erin93<<endl;
  }
  return true;
}
else return false;
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
