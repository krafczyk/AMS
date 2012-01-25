//#define _PGTRACK_
#include "AMSNtupleHelper.h"
#include "../include/root_setup.h"
static AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;

class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0){
 TrTrackR tr=ev->TrTrack(ev->Particle(0).iTrTrack());
   cout <<" ok "<<endl;
  int k=tr.iTrTrackPar(1,0,5);
vector<float>value;
 int s=ev->GetSlowControlData("L03T+01X+08 / L06T+02Y-03",value,1);
 cout <<" res "<<s<<" "<<value.size()<<" value "<<value[0]<<endl;
int s2=  AMSSetupR::gethead()->fSlowControl.GetData("L03T+01X+08 / L06T+02Y-03",ev->UTime(),100.,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<value[0]<<endl;

AMSSetupR::gethead()->fSlowControl.printElementNames("L03T+01X+08 / L06T+02Y-03");

 AMSSetupR::DynAlignment_m &pars=AMSSetupR::gethead()->fDynAlignment;
 cout <<" size "<<pars[1].FitParameters.size()<<" "<<pars[9].FitParameters.size()<<endl;
cout <<k<<" "<<ev->Particle(0).TRDCoo[0][2]<<" "<<ev->Particle(0).TRDCoo[1][2]<<endl;
return true;
}else return false;
if(ev && ev->nEcalShower() &&   ev->EcalShower(0).EnergyC>180 &&    ev->EcalShower(0).EnergyC<220 && ev->EcalShower(0).EnergyC/fabs(ev->Particle(0).Momentum)>1 ){
      cout <<ev->Event()<<" "<<ev->EcalShower(0).EnergyC<<endl;
return true;
}
else return false;
  if(ev && ev->nParticle()){
    cout <<"0 "<<ev->Particle(0).TRDCoo[0][2]<<endl;
    cout <<"1 "<<ev->Particle(0).TRDCoo[1][2]<<endl;
    for(int k=0;k<9;k++)cout <<k<<" "<<ev->Particle(0).TrCoo[k][1]<<" "<<ev->Particle(0).TrCoo[k][2]<<endl;

    return 1;
  }
  
 //cout<<ev->getsetup()->fScalers[1]._LiveTime[0]<<" "<<ev->getsetup()->fScalers[1]._LiveTime[1]<<endl;
// cout <<ev->fHeader.Roll<<" "<<ev->fHeader.Pitch<<" "<<ev->fHeader.Yaw<<endl;
 unsigned int gps,gpsn;
 //int ret=ev->GetGPSTime(gps,gpsn);
 //cout <<"  ret "<<endl;
 //for(int k=0;k<ev->fHeader.GPSTime.size();k++){
 //  cout <<" GPS "<<k<<" "<<ev->fHeader.GPSTime[k]<<endl;
// }
 //cout <<" gps "<<ret<<" "<<gps<<" "<<gpsn<<" "<<ev->UTime()<<" "<<ev->fHeader.Time[1]<<endl; 
//return true;
   if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->nTrdTrack() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>200 ){

//   if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->nTrdTrack() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>20 && ev->EcalShower(0).S13R!=0 && ev->EcalShower(0).RearLeak<0.99 ){
   //if(ev && ev->nLevel1()   && ev->nParticle()  && ev->Particle(0).Beta>0 && ev->nTrTrack()  && ev->Particle(0).Charge>1 && ev->Particle(0).iTrTrack()>=0 &&  ev->Particle(0).iTrdTrack()<0){
//   cout<<ev->EcalShower(0).AttLeak<<" "<<ev->EcalShower(0).S13Leak<<" "<<ev->EcalShower(0).S13R<<endl;
  cout <<ev->fHeader.Roll<<" "<<ev->fHeader.Pitch<<" "<<ev->fHeader.Yaw<<endl;
  return true;
  }
  else return false;
    static int a=0;
    cout <<ev->getsetup()<<endl;
    cout<<ev->getsetup()->fScalers[0]._LiveTime[0]<<endl;
    vector<float> aa;
    int k=ev->getsetup()->fSlowControl.GetData("B-Field P0 Component X",ev->UTime(),ev->Frac(),aa);
    cout <<"size "<<aa.size()<<" res "<<k<<endl;
     if(aa.size())cout <<"  ee "<<aa[0]<<endl;
    k=ev->getsetup()->fSlowControl.GetData("Zenith PT1000 4-3B (outlet)",ev->UTime(),ev->Frac(),aa);
 //   return true;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise
//   if(ev && (ev->fStatus &3) &&( (ev->fStatus>>4 &1)))return true;
//   else return false;
     if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->Particle(0).iTrTrack()>=0 && ev->nBeta() && fabs(ev->Beta(0).Beta)==1)return true;
else return false;
if(ev && ev->nLevel1()){
  Level1R l1=ev->Level1(0);
    if(1 || (l1.EcalFlag/10)>=2 || (l1.EcalFlag%10)>1){
      cout << "EFlag "<<l1.EcalFlag<<endl;
      cout <<ev->Run()<<" "<<ev->Event()<<endl;
      for (int i=0;i<16;i++){
        cout <<i<<" "<<(((l1.JMembPatt)>>i)&1)<<endl;
}
       return true;
    }
    else return false;
}
      if(ev){
   a++;
        if(ev->nMCTrack()){
           double step=0;
           for(int i=0;i<ev->nMCTrack();i++){
            MCTrackR tr= ev->MCTrack(i);
             step+=tr.RadL;
             if(strstr(tr.VolName,"STK6"))cout <<tr.VolName<<" "<<tr.Pos[1]<<" "<<tr.Pos[2]<<" "<<tr.RadL<<endl;
          }
         cout <<" got "<<step<<" "<<a<<endl;
         return true;
        }     
  }
  return false;
     
}
/*
    int          membpat=ev->pLevel1(0)->JMembPatt;
    int b15=(membpat>>15)&1;
    int b14=(membpat>>14)&1;
         if(!ev->nEcalShower() && ev->nEcalCluster()){
           cout<<" "<<ev->nEcalCluster()<<" "<<ev->nEcal2DCluster()<<" "<<ev->nEcalHit()<<endl;
           for(int i=0;i<ev->nEcalCluster();i++){
                EcalClusterR ec=ev->EcalCluster(i);
                cout <<" i "<<ec.Status<<" "<<ec.Plane<<" "<<ec.Coo[0]<<" "<<ec.Coo[1]<<" "<<ec.Coo[2]<<" "<<ec.Edep<<endl;
            }
           return true;
        }
           else return false;
    //      if(! (b14 && b15))return false; 
          cout <<ev->nTrTrack()<<" "<<ev->NTrTrack()<<endl;
           TrTrackR tr=ev->TrTrack(ev->Particle(0).iTrTrack());
           cout <<" q "<<tr.NTrRecHit()<<endl;
           return true;
           for(int i=0;i<tr.NTrRecHit();i++){
              TrRecHitR rh=ev->TrRecHit(tr.iTrRecHit(i));
              bool ok=rh.lay()==3 && rh.half()==0 && rh.lad()==2;
//              ok =ok || (rh.lay()==5 && rh.half()==1 && rh.lad()==3);
              if(ok)return true;
            }
            return false;           
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
*/
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
