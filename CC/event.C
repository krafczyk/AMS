//  $Id: event.C,v 1.548 2011/11/26 10:46:56 pzuccon Exp $
// Author V. Choutko 24-may-1996
// TOF parts changed 25-sep-1996 by E.Choumilov.
//  ECAL added 28-sep-1999 by E.Choumilov
// add TDV/dbase version October 1, 1997. a.k.
//

#include "typedefs.h" 
#include <stdexcept>
extern "C" void setbcorr_(float *p);
#ifndef _PGTRACK_
#include "trrawcluster.h"
#include "trmccluster.h"
#include "trcalib.h"
#include "tralig.h"
#include "vtx.h"
#else
#include "MagField.h"
#include "TrRecon.h"
#include "HistoMan.h"
#include "TrExtAlignDB.h"
#endif
extern "C" int ISSGTOD(float *r,float *t,float *p, float *v, float *vt, float *vp, float *grmedphi, double time);

#include "trrec.h"
#include "tofdbc02.h" 
#include "daqecblock.h"
#include "event.h"
#include "cont.h"
#include "commons.h"
#include "amsgobj.h"
#include "tofrec02.h"
#include "mccluster.h"
#include "beta.h"
#include "charge.h"
#include "particle.h"
#include "tofsim02.h"
#include <stdlib.h>
#include "tofcalib02.h"
#include "anticalib02.h"
#include "daqs2block.h"
#include "tofid.h"
#include "ntuple.h"
#include "timeid.h"
#include "trdcalib.h"
#include "trigger102.h"
#include "trigger302.h"
//#include "bcorr.h"
#include "antirec02.h"
#include "user.h"
#include <signal.h>
#include "ecaldbc.h"
#include "ecalrec.h"
#include "ecalcalib.h"
#include "richrec.h"
#include "richdbc.h"
#include "richcal.h"
#include "richradid.h"
#include "geantnamespace.h"
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "status.h"
#include "trdsim.h"
#include "trdrec.h"
#include "trdhrec.h"
#include "TrdHCalib.h"
#ifdef __G4AMS__
#include "g4util.h"
#endif
static geant   Tcpu0 = 0; 
static time_t  T0    = 0;

#ifdef __DB__

#include <dbS.h>
extern LMS* lms;

#endif
#ifdef __CORBA__
#include "producer.h"
#endif
//
//
#include "OrbGen.h"

//#include "HistoMan.h"

bool AMSEvent::_Barrier=false;
integer AMSEvent::debug=0;
uint64 AMSEvent::_RunEv[maxthread]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AMSEvent* AMSEvent::_Head[maxthread]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int AMSEvent::_Wait[maxthread]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uinteger AMSEvent::_Size[maxthread]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AMSNodeMap AMSEvent::EventMap;
integer AMSEvent::SRun=0;
integer AMSEvent::PosInRun=0;
integer AMSEvent::PosGlobal=0;
void AMSEvent::_init(DAQEvent*pdaq){
// gps stuff
 if(pdaq)SetGPSTime(pdaq->_gpsl,pdaq->_gps);

  // Status stuff
  #ifdef __CORBA__
  static bool opendst=false;
  #endif  
  if(AMSFFKEY.Update && AMSStatus::isDBWriteR()  ){
    if(AMSJob::gethead()->getstatustable()->isFull(getrun(),getid(),gettime(),pdaq)){
    AMSEvent::ResetThreadWait(1);
#pragma omp barrier  
if(AMSEvent::get_thread_num()==0){
      AMSJob::gethead()->getstatustable()->Sort();
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVStatus());
      ptdv->UpdateMe()=1;
      
      ptdv->UpdCRC();
      time_t begino,endo,inserto;
      ptdv->gettime(inserto,begino,endo);
      time_t begin,end,insert;
      begin=AMSJob::gethead()->getstatustable()->getbegin();
      end=AMSJob::gethead()->getstatustable()->getend();
      time(&insert);
      ptdv->SetTime(insert,begin,end);
      cout <<" Event Status info  event has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
      cout << " Starting to update "<<*ptdv; 
      bool fail=false;
      if(  !ptdv->write(AMSDATADIR.amsdatabase)){
         cerr <<"AMSEvent::_init-S-ProblemtoUpdate "<<*ptdv;
          fail=true;
      }
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
      cout << "  Status First last "<<first<<" "<<last<<endl;
#ifdef __CORBA__
      cout <<" sending eventtag end "<<endl;
      AMSProducer::gethead()->sendEventTagEnd(ptdv->getname(),p->getrun(),insert,begin,end,first,last,p->getnelem(),fail);       
      opendst=true;
#endif
      ptdv->SetTime(inserto,begino,endo);
      AMSJob::gethead()->getstatustable()->reset();      
    }
#pragma omp barrier 
}
#ifdef __CORBA__
    else if(opendst){
#pragma omp master
{
      opendst=false;
      AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVStatus());
      AMSStatus *p=AMSJob::gethead()->getstatustable();
      uinteger first,last;
      p->getFL(first,last);
      cout <<" sending eventtag begin "<<endl;
     AMSProducer::gethead()->sendEventTagBegin(ptdv->getname(),p->getrun(),first);       
}
    }
#endif
  }
  _init();
}
void AMSEvent::_init(){
  SetTimeCoo(IOPA.mode==1);
   

  // check old run & 
   if(_run!= SRun || !AMSJob::gethead()->isMonitoring())_validate();
  if(_run != SRun){
   AMSEvent::ResetThreadWait(1);
Barrier()=true;
#pragma omp barrier 
//#pragma omp master
//cout <<"AMSAEvent::_init in barrier "<<AMSEvent::get_thread_num()<<endl;
if(AMSEvent::get_thread_num()==0)
{
   // get rid of crazy runs
   if(_run<TRMFFKEY.OKAY/10 && AMSJob::gethead()->isRealData()){
     cerr<<"AMSEvent::_init-S-CrazyRunFound "<<_run<<endl;
   }
   DAQEvent::initO(_run,getid(),gettime());
   if(AMSJob::gethead()->isSimulation())_siamsinitrun();
   _reamsinitrun();
#ifdef _PGTRACK_
   //PZ Book the histos of the Histo facility
   {
     if(IOPA.histoman>0){
       hman.Enable();
       // In case of IOPA.histoman= 2 or 3:
       // Histograms are written in file later when HistoMan::Save is called
       if(IOPA.histoman%10 == 2 || IOPA.histoman%10 == 3) {
	 char rdir[161];
	 UHTOC(IOPA.rfile,40,rdir,160);  

         int offs = 0; 
         for (int i = 0; i < strlen(rdir) && rdir[i] == ' '; i++) offs++;

         int len = strlen(rdir);
         if (rdir[len-5] == '.' && 
             rdir[len-4] == 'r' && rdir[len-3] == 'o' &&
             rdir[len-2] == 'o' && rdir[len-1] == 't') {
           for (int i = len-1; i >= 0; i--)
             if (rdir[i] == '/') {
               rdir[i] = '\0';
               break;
             }
         }

         char hfname[256];
         sprintf(hfname, "%s/Histos_%d.root", &rdir[offs], _run);
         hman.Setname(hfname);
       }
       else if(IOPA.histoman%10 == 1) hman.Setname("");

       int mcmode = (AMSJob::gethead()->isSimulation()) ? 1 : 0;
       if (AMSJob::gethead()->isMCData()) mcmode += 2;
       hman.BookHistos(mcmode);
     }
   }
#endif
   Barrier()=false;


  if(_run != SRun){
    if (AMSFFKEY.Update && !AMSJob::gethead()->isCalibration()){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
       if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
       cerr <<"AMSEvent::_init-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }
    if(SRun){
       AMSNtuple::writeRSetup();
    }
#ifndef _PGTRACK_
    //PZ FIXME CALIB
    if(SRun  && TRCALIB.CalibProcedureNo == 3)AMSTrIdCalib::ntuple(SRun);
    if(SRun  && TRCALIB.CalibProcedureNo == 4){
      if(gettime()>TRCALIB.MultiRun){
       raise(SIGTERM);
      }
      AMSTrIdCalib::addonemorecalib();
    }
#endif
    SRun=_run;
    PosInRun=0;
   _validate(1);
  }


   cout <<" AMS-I-New Run "<<_run<<endl;
        const int size=sizeof(STATUSFFKEY.status)/sizeof(STATUSFFKEY.status[0]);
  if(STATUSFFKEY.status[size-2]){
    cout<<*(AMSJob::gethead()->getstatustable())<<" Triggers "<<GCFLAG.IEVENT<<endl;;
  }
}
#pragma omp barrier 
}
  // Initialize containers & aob
#pragma omp critical (initco)
{
  if(AMSJob::gethead()->isSimulation())_siamsinitevent();
  _reamsinitevent();
  if(AMSJob::gethead()->isCalibration())_caamsinitevent();


  SetTimeCoo(1);
  PosInRun++;
  PosGlobal++;
  Trigger2LVL1::l1trigconf.redefbydc();//MC: redef some lvl1config-pars by data cards
}
}


void AMSEvent::_startofrun() {

}

void AMSEvent::_endofrun() {
}

void AMSEvent::_siamsinitrun(){
for(int i=0;i<8;i++)Trigger2LVL1::PhysBranchCount[i]=0;//reset fired PhysBranches counters
_sitkinitrun();
_sitofinitrun();
_siantiinitrun();
_siecalinitrun();
_sitrdinitrun();
_sirichinitrun();
}


void AMSEvent::_reamsinitrun(){
if(AMSJob::gethead()->isProduction() ){
if(AMSEvent::get_thread_num()==0){
 if(SRun){
  HDELET(0);
 }
  AMSJob::gethead()->uhinit(getrun(),getmiid(),getmitime());
}  


}
else if(AMSJob::gethead()->isMonitoring()){
if(DAQEvent::RootDir()){
  AMSJob::gethead()->urinit(DAQEvent::RootDir());
  DAQEvent::setRootDir();
}
}

if(AMSJob::gethead()->isProduction() && AMSJob::gethead()->isRealData()){
 if(!SRun){
 }
 else{
  _endofrun();
 }
 _startofrun();
}

AMSNtuple::readRSetup(this);


_retkinitrun();
_retofinitrun();
_reantiinitrun();
_reecalinitrun();
_retrdinitrun();
_rerichinitrun();
_reaxinitrun();
AMSUser::InitRun();
}


void AMSEvent::_siamsinitevent(){
 TOF2DBc::debug=0;
 AMSBitstr::setclkphase();//set trig.electronics clock-pulse phases for JLV1 and SPT2
 _sitofinitevent();
 _siantiinitevent();
 _siecalinitevent();
 _sitrdinitevent();
 _sirichinitevent();
}

void AMSEvent::_reamsinitevent(){
#ifdef __CORBA__
 if( !AMSProducer::gethead()->IsSolo() && getrun() != SELECTFFKEY.Run ){
   seterror(2);
   
   throw amsglobalerror("AMSProducer-E-RunIsDifferent ",2);
 }
#endif
 _signinitevent();
 _sitkinitevent();
 _redaqinitevent();
 _retkinitevent();
 _retriginitevent();
 _retof2initevent();
 _reantiinitevent();
 _reecalinitevent();
 _retrdinitevent();
 _rerichinitevent();
 _reaxinitevent();
}

void AMSEvent::_signinitevent(){
  AMSNode *ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSmceventg",0),0));
  add (
  new AMSContainer(AMSID("AMSContainer:AMSmctrack",0),0));
  add (
  new AMSContainer(AMSID("AMSContainer:AMSmctrack",1),0));
}

void AMSEvent::SetTimeCoo(integer rec){
  AMSgObj::BookTimer.start("SetTimeCoo");
  // Allocate time & define the geographic coordinates
  if(AMSJob::gethead()->isSimulation() && !rec){
    static number dtime=AMSmceventg::Orbit.FlightTime/
      (GCFLAG.NEVENT+1);
    static number curtime=0;
    geant dd; 
    int i;
    number xsec=0;
    if(CCFFKEY.low==0&&GMFFKEY.GammaSource==0){ //equispaced events for sources for now
      xsec+=-dtime*(AMSmceventg::Orbit.Nskip+1)*log(RNDM(dd)+1.e-30);
    }
    else xsec+=dtime*(AMSmceventg::Orbit.Nskip+1);
    curtime+=xsec;
    if(curtime>AMSmceventg::Orbit.FlightTime){
      curtime=AMSmceventg::Orbit.FlightTime;
      GCFLAG.IEORUN=1;
    }
    //    cout <<" AMSmceventg::Orbit.FlightTime "<<AMSmceventg::Orbit.FlightTime<<" "<<xsec<<" "<<curtime<<" "<<dtime<< " "<<AMSmceventg::Orbit.Nskip<<endl;
    GCFLAG.IEVENT=GCFLAG.IEVENT+AMSmceventg::Orbit.Nskip;
    //    if(GCFLAG.IEVENT>GCFLAG.NEVENT){
    //      GCFLAG.IEORUN=1;
    //      GCFLAG.IEOTRI=1;
    //      return;
    //    }
    _NorthPolePhi=AMSmceventg::Orbit.PolePhi;
    AMSmceventg::Orbit.UpdateOrbit(curtime,_StationTheta,_StationPhi,_NorthPolePhi,_StationEqAsc,_StationEqDec,_StationGalLat,_StationGalLong,_time);
    
    _usec=(curtime-integer(curtime))*1000000000;  // nsec for mc
    

    AMSmceventg::Orbit.Nskip=0;        
    AMSmceventg::Orbit.Ntot++;
    _Yaw=0;
    _Roll=0;
  _Pitch=0;
  _Alpha=0;
  _B1a=0;
  _B1b=0;
  _B3a=0;
  _B3b=0;
  _StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
  _StationRad=AMSmceventg::Orbit.AlphaAltitude;
  _SunRad=0;
  // get velocity parameters from orbit par
  AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
  AMSDir ax2=AMSmceventg::Orbit.Axis.cross(ax1);
  //cout <<" 2 "<<AMSmceventg::Orbit.Axis<<" "<<ax1.prod(AMSmceventg::Orbit.Axis)<<endl;
  _VelTheta=AMSDBc::pi/2-ax2.gettheta();
  _VelPhi=ax2.getphi();
  
  // Once ISS celestial coo have been calculated, obtain AMS celestial coo
  number raf,decf;
  const float tilt=0.20944; // tilt of about 12 degrees around forward direction 
  
  skyposition forwardpos(_VelTheta,fmod(_VelPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi),_StationRad,_time); // calculate celestial position towards which ISS advances (X_lvlh)
  forwardpos.GetRa(raf);  
  forwardpos.GetDec(decf);  
  
  geant ziss[3],xiss[3],yiss[3],zams[3];
  
  ziss[0] = cos(_StationEqAsc)*cos(_StationEqDec); // ISS pointing direction
  ziss[1] = sin(_StationEqAsc)*cos(_StationEqDec);
  ziss[2] = sin(_StationEqDec);  
  yiss[0] = -cos(raf)*cos(decf); // vector yiss points "backwards"
  yiss[1] = -sin(raf)*cos(decf);
  yiss[2] = -sin(decf);  
  xiss[0] = yiss[1]*ziss[2]-ziss[1]*yiss[2]; // yiss x ziss
  xiss[1] = -(yiss[0]*ziss[2]-ziss[0]*yiss[2]);
  xiss[2] = yiss[0]*ziss[1]-ziss[0]*yiss[1];
  zams[0] = sin(tilt)*xiss[0]+cos(tilt)*ziss[0]; // AMS pointing direction
  zams[1] = sin(tilt)*xiss[1]+cos(tilt)*ziss[1];
  zams[2] = sin(tilt)*xiss[2]+cos(tilt)*ziss[2];
  
  _AMSEqAsc=fmod(atan2(zams[1],zams[0])+AMSDBc::twopi,AMSDBc::twopi);
  _AMSEqDec=asin(zams[2]);
  skyposition amspos(_AMSEqAsc,_AMSEqDec);
  amspos.GetLong(_AMSGalLong);
  amspos.GetLat(_AMSGalLat);

  if(CCFFKEY.low==10){ // realistic orbit generator for plaen1/9 movement studies
    
    OrbGen*orb=OrbGen::GetOrbGen();
    _Yaw=0;
    _Roll=0;
    _Pitch=0;
    _Alpha=0;
    _StationTheta=orb->Lat;
    _StationPhi=orb->Lon;
    _time=orb->Time.Time_s;
    _usec=orb->Time.Time_ns;
  }





  }
  else if(AMSJob::gethead()->isSimulation() && rec){
    if(CCFFKEY.oldformat){
      static number StTheta=0;
      _Yaw=0;
      _Roll=0;
      _Pitch=0;
      _Alpha=0;
      _B1a=0;
      _B1b=0;
      _B3a=0;
    _B3b=0;
    _StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    _StationRad=AMSmceventg::Orbit.AlphaAltitude;
    // get velocity parameters from orbit par
    AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
    AMSDir ax2=AMSmceventg::Orbit.Axis.cross(ax1);
    if(ax1.prod(AMSmceventg::Orbit.Axis)>1e-4){
     cerr<<"AMSEvent::SetTimeCoo-W-RedefinitionOfOrbit.AxisWillBeDone "<<ax1.prod(AMSmceventg::Orbit.Axis)<<endl;
     AMSmceventg::Orbit.UpdateOrbit(_StationTheta,_StationPhi,_StationTheta-StTheta>00?1:-1);
     StTheta=_StationTheta;
    }
    _VelTheta=AMSDBc::pi/2-ax2.gettheta();
    _VelPhi=ax2.getphi();
   }
    _SunRad=0;
  }
  else if((!AMSJob::gethead()->isSimulation() && rec) || (AMSJob::gethead()->isSimulation() && IsTest())){
    static integer hint=0;
#pragma omp threadprivate (hint)
    //get right record
    if( Array[hint].Time<=_time && 
	_time<Array[hint+2>=sizeof(Array)/sizeof(Array[0])?
		    sizeof(Array)/sizeof(Array[0])-1:hint+2].Time){
      // got it
      if(_time>=Array[hint+1].Time)hint++;
    }
    else{
      //find from scratch
      for(hint=1;hint<sizeof(Array)/sizeof(Array[0]);hint++){
        if(Array[hint].Time>_time){
          hint--;
          break;
        }
      }
      if(hint>=sizeof(Array)/sizeof(Array[0]))hint=sizeof(Array)/sizeof(Array[0])-1;
    }
    
    
    if(_time == Array[hint].Time){
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[hint].StationTheta;
      _StationPhi=fmod(Array[hint].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[hint].StationYaw;
      _Roll=Array[hint].StationRoll;
      _Pitch=Array[hint].StationPitch;
      _StationSpeed=Array[hint].StationSpeed;
      _StationRad=Array[hint].StationR;
      _SunRad=Array[hint].SunR;
      _VelTheta=Array[hint].VelTheta;
      _VelPhi=Array[hint].VelPhi;
    }
    else if(_time == Array[hint+1].Time){
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint+1].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[hint+1].StationTheta;
      _StationPhi=fmod(Array[hint+1].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[hint+1].StationYaw;
      _Roll=Array[hint+1].StationRoll;
      _Pitch=Array[hint+1].StationPitch;
      _StationSpeed=Array[hint+1].StationSpeed;
      _StationRad=Array[hint+1].StationR;
      _SunRad=Array[hint+1].SunR;
      _VelTheta=Array[hint+1].VelTheta;
      _VelPhi=Array[hint+1].VelPhi;
    }
    else if(!MISCFFKEY.BeamTest && _StationRad>6.4e8){
      //interpolation needed
      number xsec=_time-Array[hint].Time;
      number dt=Array[hint+1].Time-Array[hint].Time;
      number sYaw,cYaw,sRoll,cRoll,sPitch,cPitch;
      if(dt<xsec){
        sYaw=sin(Array[hint].StationYaw)+xsec/dt*(sin(Array[hint+1].StationYaw)-sin(Array[hint].StationYaw));
        cYaw=cos(Array[hint].StationYaw)+xsec/dt*(cos(Array[hint+1].StationYaw)-cos(Array[hint].StationYaw));
         _Yaw=atan2(sYaw,cYaw);

        sPitch=sin(Array[hint].StationPitch)+xsec/dt*(sin(Array[hint+1].StationPitch)-sin(Array[hint].StationPitch));
        cPitch=cos(Array[hint].StationPitch)+xsec/dt*(cos(Array[hint+1].StationPitch)-cos(Array[hint].StationPitch));
         _Pitch=atan2(sPitch,cPitch);

        sRoll=sin(Array[hint].StationRoll)+xsec/dt*(sin(Array[hint+1].StationRoll)-sin(Array[hint].StationRoll));
        cRoll=cos(Array[hint].StationRoll)+xsec/dt*(cos(Array[hint+1].StationRoll)-cos(Array[hint].StationRoll));
         _Roll=atan2(sRoll,cRoll);

        _StationSpeed=Array[hint].StationSpeed+xsec/dt*(Array[hint+1].StationSpeed-Array[hint].StationSpeed);
        _StationRad=Array[hint].StationR+((Array[hint].StationR>=0)?xsec/dt*(Array[hint+1].StationR-Array[hint].StationR):0);
        _SunRad=Array[hint].SunR+xsec/dt*(Array[hint+1].SunR-Array[hint].SunR);
        _VelTheta=Array[hint].VelTheta;
        _VelPhi=Array[hint].VelPhi;
      }
      else {
        _Yaw=Array[hint].StationYaw;
        _Roll=Array[hint].StationRoll;
        _Pitch=Array[hint].StationPitch;
        _StationSpeed=Array[hint].StationSpeed;
        _StationRad=Array[hint].StationR;
        _SunRad=Array[hint].SunR;
        _VelTheta=Array[hint].VelTheta;
        _VelPhi=Array[hint].VelPhi;
      }
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint].GrMedPhi+
                         AMSmceventg::Orbit.EarthSpeed*xsec,AMSDBc::twopi);
       // getorbit parameters from velocity
        AMSDir ax1(AMSDBc::pi/2-Array[hint].StationTheta,Array[hint].StationPhi);
        AMSDir ax2(AMSDBc::pi/2-_VelTheta,_VelPhi);
        AMSmceventg::Orbit.Axis=ax1.cross(ax2);
        AMSmceventg::Orbit.AlphaTanThetaMax=tan(acos(AMSmceventg::Orbit.Axis[2]));
       number r= tan(Array[hint].StationTheta)/
         AMSmceventg::Orbit.AlphaTanThetaMax;
       if(r > 1 || r < -1){
         cerr <<"AMSEvent::SetTimeCoo-ThetaI too high "<<
           Array[hint].StationTheta*AMSDBc::raddeg<<endl;
           AMSmceventg::Orbit.AlphaTanThetaMax=tan(fabs(Array[hint].StationTheta));
         if(r < 0 )r=-1;
         else r=1;
       }
       number idir=Array[hint+1].StationTheta>Array[hint].StationTheta?1:-1;
       number PhiZero=Array[hint].StationPhi-atan2(r,idir*sqrt(1-r*r));

      number t2=
        AMSmceventg::Orbit.AlphaTanThetaMax*AMSmceventg::Orbit.AlphaTanThetaMax;
      number philocal=
        atan2(sin(Array[hint].StationPhi-PhiZero)*sqrt(1+t2),
              cos(Array[hint].StationPhi-PhiZero));
      philocal=fmod(philocal+_StationSpeed*xsec,AMSDBc::twopi);
      number phi=atan2(sin(philocal),cos(philocal)*sqrt(1+t2));
      if(phi < 0)phi=phi+AMSDBc::twopi;
      _StationTheta=atan(AMSmceventg::Orbit.AlphaTanThetaMax*sin(phi));
      _StationPhi=fmod(phi+PhiZero+AMSDBc::twopi,AMSDBc::twopi);
//      _StationSpeed*=idir;
      // Recalculate VelTheta,VelPhi
{
        AMSDir ax1(AMSDBc::pi/2-_StationTheta,_StationPhi);
        AMSDir ax2= AMSmceventg::Orbit.Axis.cross(ax1);
        _VelPhi=ax2.getphi();
        _VelTheta= AMSDBc::pi/2-ax2.gettheta();
}

     
    }
    else {
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[0].GrMedPhi+AMSDBc::twopi,AMSDBc::twopi);
      _StationTheta=Array[0].StationTheta;
      _StationPhi=fmod(Array[0].StationPhi+AMSDBc::twopi,AMSDBc::twopi);
      _Yaw=Array[0].StationYaw;
      _Roll=Array[0].StationRoll;
      _Pitch=Array[0].StationPitch;
      _StationSpeed=Array[0].StationSpeed;
      _StationRad=Array[0].StationR;
      _SunRad=Array[0].SunR;
      _VelTheta=Array[0].VelTheta;
      _VelPhi=Array[0].VelPhi;
    }
    if(AMSJob::gethead()->isRealData())LoadISS();
  }

  AMSgObj::BookTimer.stop("SetTimeCoo");
}

void AMSEvent::_regnevent(){




{
//  Get Correct CCEB block if exists;
    static integer hintc=0;
#pragma omp threadprivate (hintc)




    //get right record
    if( ArrayC[hintc].Time<=_time && 
       _time<ArrayC[hintc+2>=sizeof(ArrayC)/sizeof(ArrayC[0])?
                  sizeof(ArrayC)/sizeof(ArrayC[0])-1:hintc+2].Time){
      // got it
      if(_time>=ArrayC[hintc+1].Time)hintc++;
    }
    else{
      //find from scratch
      for(hintc=1;hintc<sizeof(ArrayC)/sizeof(ArrayC[0]);hintc++){
        if(ArrayC[hintc].Time>_time){
          hintc--;
          break;
        }
      }
      if(hintc>=sizeof(ArrayC)/sizeof(ArrayC[0]))hintc=sizeof(ArrayC)/sizeof(ArrayC[0])-1;
    }
    integer chint;
    if(_time < ArrayC[hintc].Time){
     chint=hintc-1;
     if(chint<0)cerr<<"CCEBPAR-S-LogicError-chint<0 "<<_time<<" "<< ArrayC[hintc].Time<<endl;
     if(chint<0)chint=0;
    }
    else chint=hintc;
    _ccebp=&(ArrayC[chint]);  
#ifndef _PGTRACK_
     geant corr=_ccebp->getBCorr();
     if(corr>0 &&MAGSFFKEY.magstat<=0 && !TRCALIB.LaserRun){
      cerr<<"AMSEvent::_regnevent-E-MAGFileCorrectionandFieldStatusDisageeA "<<corr<<" "<<MAGSFFKEY.magstat<<endl;
       MAGSFFKEY.magstat=1;
     }
     else if(corr==0 &&MAGSFFKEY.magstat>0){
      cerr<<"AMSEvent::_regnevent-E-MAGFileCorrectionandFieldStatusDisageeB "<<corr<<" "<<MAGSFFKEY.magstat<<endl;
       MAGSFFKEY.magstat=-1;
     }
      if(corr>=0 && fabs(corr-1)>0.05){
        static int mess=0;
        // if(mess++<100)cout<<"AMSEvent::_regnevent-I-MagFieldCorrectionApplied "<<corr<<endl;
      }
      else if(corr>=0){
        corr=-2;
      }
      //setbcorr_(&corr);  
#endif
}

//----> Get correct TofSTemp (U&L) blocks if exist:
{
    static integer utofc(0),ltofc(0);
#pragma omp threadprivate (utofc,ltofc)
//-> get pointer to the right UTof-record(for the moment start from scratch because array is short):
    for(utofc=1;utofc<sizeof(UTofTemp)/sizeof(UTofTemp[0]);utofc++){
      if(UTofTemp[utofc].Time>_time){
        utofc--;
        break;
      }
    }
    if(utofc>=sizeof(UTofTemp)/sizeof(UTofTemp[0]))utofc=sizeof(UTofTemp)/sizeof(UTofTemp[0])-1;
    integer utofp;
    if(_time < UTofTemp[utofc].Time){//error
     utofp=utofc-1;
     if(utofp<0)cerr<<"<--- UTofSTemp-S-LogicError- utofp<0 "<<_time<<" "<< UTofTemp[utofc].Time<<endl;
     if(utofp<0)utofp=0;
    }
    else utofp=utofc;
    _Utoftp=&(UTofTemp[utofp]);//set the pointer for later usage in temper.extraction at Tof-validation stage
//  
//-> get pointer to the right LTof-record(for the moment start from scratch because array is short):
    for(ltofc=1;ltofc<sizeof(LTofTemp)/sizeof(LTofTemp[0]);ltofc++){
      if(LTofTemp[ltofc].Time>_time){
        ltofc--;
        break;
      }
    }
    if(ltofc>=sizeof(LTofTemp)/sizeof(LTofTemp[0]))ltofc=sizeof(LTofTemp)/sizeof(LTofTemp[0])-1;
    integer ltofp;
    if(_time < LTofTemp[ltofc].Time){//error
     ltofp=ltofc-1;
     if(ltofp<0)cerr<<"<--- LTofSTemp-S-LogicError- ltofp<0 "<<_time<<" "<< LTofTemp[ltofc].Time<<endl;
     if(ltofp<0)ltofp=0;
    }
    else ltofp=ltofc;
    _Ltoftp=&(LTofTemp[ltofp]);//set the pointer for later usage in temper.extraction at Tof-validation stage
}


//---> Add mceventg if BeamTest
    if(MISCFFKEY.BeamTest>1  ){


    static integer hintb=0;
#pragma omp threadprivate (hintb)
    //get right record
    if( ArrayB[hintb].Time<=_time && 
       _time<ArrayB[hintb+2>=sizeof(ArrayB)/sizeof(ArrayB[0])?
                  sizeof(ArrayB)/sizeof(ArrayB[0])-1:hintb+2].Time){
      // got it
      if(_time>=ArrayB[hintb+1].Time)hintb++;
    }
    else{
      //find from scratch
      for(hintb=1;hintb<sizeof(ArrayB)/sizeof(ArrayB[0]);hintb++){
        if(ArrayB[hintb].Time>_time){
          hintb--;
          break;
        }
      }
      if(hintb>=sizeof(ArrayB)/sizeof(ArrayB[0]))hintb=sizeof(ArrayB)/sizeof(ArrayB[0])-1;
    }
    integer chint;
    if(_time < ArrayB[hintb].Time){
     chint=hintb-1;
     if(chint<0)cerr<<"BeamTime-S-LogicError-chint<0 "<<_time<<" "<< ArrayB[hintb].Time<<endl;
    }
    else chint=hintb;
     // check the runtag
     
     if(ArrayB[chint].RunTag%32768!= getruntype()%32768){
        seterror();
        int chinta, chintb;
        if (chint>0)chinta=chint-1;
        else chinta=chint;
        if (chint<59)chintb=chint+1;
        else chintb=59;
        cerr<<"Event & BeamPar disagree event says runtype = "<<hex<<
        getruntype()%32768<<" BeamPar says "<<hex<<
        ArrayB[chint].RunTag<<" "<<dec<<chint<<" "<<_time <<" "<<ArrayB[chint].Time<<" "<<ArrayB[chinta].Time<<" "<<ArrayB[chintb].Time<< endl;
     }
      
      geant mom(ArrayB[chint].Mom);
      integer part(ArrayB[chint].Pid);
      AMSDir dir(ArrayB[chint].Theta,ArrayB[chint].Phi);
      AMSPoint x(ArrayB[chint].X,ArrayB[chint].Y,ArrayB[chint].Z);
      
//cout << " x "<<x<< " "<<dir<< " "<<ArrayB[chint].Time<<endl;
      AMSmceventg *pgen=new AMSmceventg(part,mom,x,dir,ArrayB[chint].Cond);
      if(pgen->acceptio()){
        addnext(AMSID("AMSmceventg",0),pgen);
      }
  }

}

#ifndef _PGTRACK_
void AMSEvent::_sitkinitevent(){

  add (
  new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
  
}
#endif

void AMSEvent::_retriginitevent(){

   add (
     new AMSContainer(AMSID("AMSContainer:TriggerLVL1",0),0));
   add (
     new AMSContainer(AMSID("AMSContainer:TriggerLVL3",0),0));
}

void AMSEvent::_siantiinitevent(){
 add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
}


void AMSEvent::_siecalinitevent(){
  int i;
  AMSNode *ptr;
//
  for(i=0;i<ECALDBc::slstruc(3);i++){// book containers for EcalMCHit-object
    ptr = add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalMCHit",i),0));
  }
//  AMSEcalRawEvent::init();//reset EC-trig. patts.,flags...
  for(int j=0;j<ecalconst::ECSLMX;j++)AMSEcalMCHit::leadedep[j]=0;
}

void AMSEvent::_sirichinitevent(){
  AMSNode *ptr;
  ptr=add(new AMSContainer(AMSID("AMSContainer:AMSRichMCHit",0),0));
  if(!ptr) cout << "AMSEvent::_sirichinitevent() No container Error" <<endl;
  RICHDB::nphgen=0;
  RICHDB::nphrad=0;
  RICHDB::nphbas=0;
  RICHDB::numrefm=0;
  RICHDB::numrayl=0;
}

void AMSEvent::_sitrdinitevent(){
  add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDMCCluster",0),0));
}

//--------------
void AMSEvent::_sitofinitevent(){
  int il;
  geant dummy(-1);
  AMSNode *ptr;
  integer trpatt[TOF2GC::SCLRS]={0,0,0,0};
//
//           declare some TOF containers for MC:
//
// container for geant hits:
//
  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFMCCluster",0),0));
//
//    container for time_over_threshold hits (digi step):
//
  for(il=0;il<TOF2GC::SCLRS;il++){
    ptr=add(
        new AMSContainer(AMSID("AMSContainer:TOF2Tovt",il),0));
  }
//<--- clear arrays for SumHT(SHT)-channel
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA-1;sf++){
      TOF2Tovt::SumHTh[cr][sf]=0;
      TOF2Tovt::SumSHTh[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX2;ht++){
        TOF2Tovt::SumHTt[cr][sf][ht]=0;
        TOF2Tovt::SumSHTt[cr][sf][ht]=0;
      }
    }
  }
//---> set T0's for SFET(A)'s TDCs (desincr. withing its CC(11bits) ovfl_period = 2048*25ns=51200ns):
    for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
      for(int sf=0;sf<TOF2GC::SCFETA;sf++){
        TOF2Tovt::TofATdcT0[cr][sf]=2048*number(TOF2DBc::tdcbin(3))*number(RNDM(dummy));//ns
      }
    }
//
}




//=====================================================================

void AMSEvent::_reantiinitevent(){

      add(
      new AMSContainer(AMSID("AMSContainer:Anti2RawEvent",0),0));
      
      add(
      new AMSContainer(AMSID("AMSContainer:AntiRawSide",0),0));
      
      add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",0),0));

      add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",1),0));
{
      AMSNode *p =add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiCluster",0),0));
//      cout <<" anti "<<p<<" "<<AMSEvent::gethead()<<" "<<get_thread_num()<<endl;
}
// reset static ev-by-ev vars:
      Anti2RawEvent::setpatt(0);
      Anti2RawEvent::setncoinc(0);
//
}
//=====================================================================
void AMSEvent::_retof2initevent(){
  integer i;
  AMSNode *ptr;
//---
// container for TOF2RawSide hits(same structure for MC/REAL events) : 
//
   ptr=add(
       new AMSContainer(AMSID("AMSContainer:TOF2RawSide",0),0));
   ptr=add(
       new AMSContainer(AMSID("AMSContainer:TOF2RawSide",1),0));
//
//<--- clear static arrays for SumHT(SHT)-channel:
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA-1;sf++){
      TOF2RawSide::SumHTh[cr][sf]=0;
      TOF2RawSide::SumSHTh[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX2;ht++){
        TOF2RawSide::SumHTt[cr][sf][ht]=0;
        TOF2RawSide::SumSHTt[cr][sf][ht]=0;
      }
    }
  }
//<--- clear static arrays for FTtime-channel:
  for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
    for(int sf=0;sf<TOF2GC::SCFETA;sf++){
      TOF2RawSide::FThits[cr][sf]=0;
      TOF2RawSide::FTSchan[cr][sf]=0;
      for(int ht=0;ht<TOF2GC::SCTHMX1;ht++){
        TOF2RawSide::FTtime[cr][sf][ht]=0;
      }
    }
  }
//<--- reset static array for SFET(A) temperatures(event-by-event option):
  TOF2JobStat::resettemp();
//..... for TOF2RawSide::
  TOF2RawSide::settrcode(-1);// reset  TOF-trigger flag
  TOF2RawSide::settrcodez(-1);// reset  TOF-trigger flag
  TOF2RawSide::setcpcode(0);// reset  TOF-trigger flag
  TOF2RawSide::setbzflag(-1);// reset  TOF-trigger flag
  TOF2RawSide::setftpatt(0);// reset glob.FT-trigger pattern
  integer trpatt[TOF2GC::SCLRS]={0,0,0,0};
  TOF2RawSide::setpatt(trpatt);// reset TOF-trigger pattern
  TOF2RawSide::setpattz(trpatt);// reset TOF-trigger pattern
//.... for TOF2RawCluster::
  uinteger trpat[TOF2GC::SCLRS]={0,0,0,0};
  TOF2RawCluster::setpatt(trpat);
  TOF2RawCluster::setpatt1(trpat);
  TOF2RawCluster::settrfl(-1);
  TOF2RawCluster::resetfttime();
//
  DAQS2Block::clrtbll();//reset TOF+ACC daq-event length(16bit words)
//---
//  container for RawCluster hits :
//
   ptr=  add (
      new AMSContainer(AMSID("AMSContainer:TOF2RawCluster",0),0));
//---
// container for Cluster hits :
//
   for( i=0;i<TOF2GC::SCLRS;i++)  ptr = add (
       new AMSContainer(AMSID("AMSContainer:AMSTOFCluster",i),0));
//
//  DAQS2Block::clrtbll();//clear sc.data length
}
//=====================================================================
void AMSEvent::_reecalinitevent(){
  integer i,maxp,maxc;
  AMSNode *ptr;
  maxp=2*ECALDBc::slstruc(3);// max SubCell(pixel)-planes
  maxc=4*ECALDBc::slstruc(3)*ECALDBc::slstruc(4);// max number of SubCell
  for(i=0;i<AMSECIds::ncrates();i++){// <-- book crate type containers for EcalRawEvent
    ptr=add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalRawEvent",i),0));
  }
//
  AMSEcalRawEvent::init();//reset EC-trig. patts.,flags...
//
  for(i=0;i<maxp;i++){// <-- book  SubCell-plane containers for EcalHit
    ptr=add (
      new AMSContainer(AMSID("AMSContainer:AMSEcalHit",i),0));
  }



  for(i=0;i<2;i++){// <-- book  proj
      add (
      new AMSContainer(AMSID("AMSContainer:Ecal1DCluster",i),&Ecal1DCluster::build,0));
 }
    add (
      new AMSContainer(AMSID("AMSContainer:Ecal2DCluster",0),&AMSEcal2DCluster::build,0));

  add (
      new AMSContainer(AMSID("AMSContainer:EcalShower",0),&AMSEcalShower::build,0));
}
void AMSEvent::_retrdinitevent(){

  for(int i=0;i<2*AMSTRDIdSoft::ncrates();i++) add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDRawHit",i),0));

  if(TRDFITFFKEY.FitMethod!=1){
  for(int i=0;i<trdconst::maxlay;i++) add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDCluster",i),&AMSTRDCluster::build,0));

  for(int i=0;i<trdconst::maxseg;i++) add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDSegment",i),&AMSTRDSegment::build,0));

  for(int i=0;i<1;i++) add (
  new AMSContainer(AMSID("AMSContainer:AMSTRDTrack",i),&AMSTRDTrack::build,0));
  }

  if(TRDFITFFKEY.FitMethod!=0){
    AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTRDHSegment",0),0));
    AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTRDHTrack",0),0));
    
    if(TRDFITFFKEY.CalStartVal>0.&&!TrdHCalibR::gethead()->calibrate&&(CALIB.SubDetInCalib/10000)%10>0)
      TrdHCalibR::gethead()->init_calibration(TRDFITFFKEY.CalStartVal);
  }
  
}

void AMSEvent::_rerichinitevent(){
  AMSNode *ptr;
    ptr=add(
     new AMSContainer(AMSID("AMSContainer:AMSRichRawEvent",0),0));
  ptr=add(
    new AMSContainer(AMSID("AMSContainer:AMSRichRing",0),0));
  ptr=add(
    new AMSContainer(AMSID("AMSContainer:AMSRichRingNew",0),0));

  // Update the necessary tables if database have been updated
  if((RICDBFFKEY.dump%10)==0 && 
     !strstr(AMSJob::gethead()->getsetup(),"PreAss"))
    RichRadiatorTileManager::GetFromTDV();

  if(AMSJob::gethead()->isRealData() && 
     ((RICDBFFKEY.dump/10)%10)==0 &&
     !strstr(AMSJob::gethead()->getsetup(),"PreAss"))
    RichAlignment::GetFromTDV();
}

//=====================================================================
void AMSEvent::_reaxinitevent(){
  integer i;
  AMSNode *ptr;
  for( i=0;i<npatb;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSBeta",i),&AMSBeta::build,0));

  for( i=0;i<npatb;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSBetaB",i),&AMSBeta::build,0));

  add (
  new AMSContainer(AMSID("AMSContainer:AMSCharge",0),&AMSCharge::build,0));

  add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",0),&AMSParticle::build,0));
  add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",1),&AMSParticle::build,0));


  add (
  new AMSContainer(AMSID("AMSContainer:AntiMatter",0),0));
  add (
  new AMSContainer(AMSID("AMSContainer:NotAProton",0),0));

  add (
  new AMSContainer(AMSID("AMSContainer:HeavyIon",0),0));

  add (
  new AMSContainer(AMSID("AMSContainer:Test",0),0));

  add (
  new AMSContainer(AMSID("AMSContainer:Dummy",0),0));

  add (
  new AMSContainer(AMSID("AMSContainer:WriteAll",0),0));





}
#ifndef _PGTRACK_
void AMSEvent::_retkinitevent(){



  integer i;
  AMSNode *ptr;
  for(i=0;i<AMSTrIdSoft::ncrates();i++) add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",i),0));

  if( TRCALIB.LaserRun){
  for( i=0;i<2;i++)ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),&AMSTrCluster::buildLaser,0));
   }
   else{
  for( i=0;i<2;i++)ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),&AMSTrCluster::build,0));
  
  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrClusterWeak",i),&AMSTrCluster::buildWeak,0));

  }
  for( i=0;i<TKDBc::nlay();i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",i),&AMSTrRecHit::build,0));

  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHitWeak",i),&AMSTrRecHit::buildWeak,0));


  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack",i),&AMSTrTrack::build,0));

  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackWeak",i),&AMSTrTrack::buildWeak,0));

  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseX",i),&AMSTrTrack::buildFalseX,0));
  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseTOFX",i),&AMSTrTrack::buildFalseTOFX,0));

  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack_PathIntegral",i),&AMSTrTrack::buildPathIntegral,0));
  for( i=0;i<1;i++)  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackWeak_PathIntegral",i),&AMSTrTrack::buildWeakPathIntegral,0));

  ptr = add (
  new AMSContainer(AMSID("AMSContainer:AMSVtx",0),&AMSVtx::build,0));

}
#endif


//=====================================================================================================================
//=====================================================================================================================
void  AMSEvent::write(int trig){
  AMSgObj::BookTimer.start("WriteEvent");
  
  // Sort before by "Used" variable : 
  // AMSTrTrack & AMSTrCluster
#ifndef _PGTRACK_
  //PZ FIXME OBSOLETE
  getheadC("AMSTrCluster",0,2); 
  getheadC("AMSTrCluster",1,2); 

 
  for(int il=0;il<TKDBc::nlay();il++){
    getheadC("AMSTrRecHit",il,2); 
  }
#endif
  getheadC("AMSmceventg",0,2); 
  for(int il=0;il<2*AMSTRDIdSoft::ncrates();il++){
    getheadC("AMSTRDRawHit",il,2); 
  }
  if(TRDFITFFKEY.FitMethod!=1){
  for(int il=0;il<trdconst::maxlay;il++){
    getheadC("AMSTRDCluster",il,2); 
  }

  for(int il=0;il<trdconst::maxseg;il++){
    getheadC("AMSTRDSegment",il,2); 
  }
  }

  if(TRDFITFFKEY.FitMethod!=0){
    for (AMSlink* pptr=getheadC("AMSTRDHSegment",0);pptr!=0;pptr=pptr->next()){
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTRDHSegment*)pptr);
    }
    for (AMSlink* pptr=getheadC("AMSTRDHTrack",0);pptr!=0;pptr=pptr->next()){
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTRDHTrack*)pptr);
    }
  }
  
  for(int il=0;il<2*ECALDBc::slstruc(3);il++){
    getheadC("AMSEcalHit",il,2); 
  }

  for(int il=0;il<2;il++){
    getheadC("Ecal1DCluster",il,2); 
  }

  getheadC("Ecal2DCluster",0,2); 
  getheadC("EcalShower",0,2); 

  //cout <<"  vont ok "<<PosInRun<<" "<<trig<<endl;
  if(trig || PosInRun< (IOPA.WriteAll/1000)*1000){
    DAQEvent * pdaq = (DAQEvent*)getheadC("DAQEvent",0);
    if(pdaq)pdaq->write();
  }   
  if((IOPA.hlun || IOPA.WriteRoot) && AMSJob::gethead()->getntuple()){
    //cout <<"  pntuple write "<<endl;
    AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
    _writeEl();
    AMSNode * cur;
    for (int i=0;;){
      cur=AMSEvent::EventMap.getid(i++);   // get one by one
      if(cur){
	if(strstr("AMSBetaB",cur->getname())!=0)continue;
        if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->writeC();
      }
      else break;
    }
    for( int kk=0;kk<npatb;kk++) 
      for (AMSlink *pptr=getheadC("AMSBetaB",kk);pptr!=0;pptr=pptr->next()){
	//      printf("Adding AMSBetaB to tree\n");
	AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObjectB((AMSBeta*)pptr);
      }
#ifdef _PGTRACK_
    //Fortracker only
    for (AMSlink *pptr=getheadC("AMSTrRawCluster",0);	 pptr!=0;pptr=pptr->next()){
      //      printf("Adding TrRawCluster to tree\n");
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTrRawCluster*)pptr);
    }
    for (AMSlink* pptr=getheadC("AMSTrCluster",0);pptr!=0;pptr=pptr->next()){
      //      printf("Adding TrCluster to tree\n");
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTrCluster*)pptr);
    }
    for (AMSlink *pptr=getheadC("AMSTrRecHit",0);pptr!=0;pptr=pptr->next()){
      //     printf("Adding TrRecHit to tree\n");
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTrRecHit*)pptr);
    }
    for (AMSlink* pptr=getheadC("AMSTrTrack",0);pptr!=0;pptr=pptr->next()){
      //    printf("Adding TrTrack to tree\n");
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTrTrack*)pptr);
    }
    if(!AMSJob::gethead()->isRealData())
      for (AMSlink* pptr=getheadC("AMSTrMCCluster",0);pptr!=0;pptr=pptr->next()){
	//	printf("Adding TrMCCluster to tree\n");
	AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject((AMSTrMCCluster*)pptr);
      }
#endif
    // second pass Root Only
    if (IOPA.WriteRoot) copy();
    //
    if(trig || PosInRun< (IOPA.WriteAll/1000)*1000){
      // if event has been selected write it straight away
      // oh nono check for errors first
      if(HasNoErrors() || (IOPA.WriteAll/100)*100)
	{
	  AMSJob::gethead()->getntuple()->write(1);
	  //#pragma omp critical (writer)
	  AMSJob::gethead()->getntuple()->writeR();
	}
      else
	{
	  AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
	  AMSJob::gethead()->getntuple()->write();
	  //#pragma omp critical (writer)
	  AMSJob::gethead()->getntuple()->writeR();
	}
    }
    else {
      // if event was not selected check if at least header should be written
      // in the ntuples
      if((IOPA.WriteAll/10)%10)
	{
	  AMSJob::gethead()->getntuple()->reset(IOPA.WriteRoot);
	  AMSJob::gethead()->getntuple()->write();
	  //#pragma omp critical (writer)
	  AMSJob::gethead()->getntuple()->writeR();
	}
    }
    // check if one want to close ntuple 
    if( IOPA.MaxNtupleEntries){
      //cout <<"qq "<<AMSJob::gethead()->getntuple()->getentries()<<" "<<IOPA.MaxNtupleEntries<<endl;
      //       cout <<"op "<<AMSJob::gethead()->GetNtupleFileSize()<<" "<<IOPA.MaxFileSize<<endl;
      bool NoMoreSpace=false;
#ifdef __CORBA__
      if(AMSProducer::gethead()->FreeSpace()>=0 && AMSProducer::gethead()->FreeSpace()<IOPA.MaxFileSize/2/1024){
	NoMoreSpace=true;
	if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
      }
#endif
      if(AMSJob::gethead()->getntuple()->getentries()>=IOPA.MaxNtupleEntries || GCFLAG.ITEST<0 || AMSJob::gethead()->GetNtupleFileSize()>IOPA.MaxFileSize
	 || AMSJob::gethead()->GetNtupleFileTime()>IOPA.MaxFileTime || NoMoreSpace)
	{
	  AMSEvent::ResetThreadWait(1);
	  Barrier()=true;
#pragma omp barrier 
	  cout <<"AMSAEvent::writefile in barrier "<<AMSEvent::get_thread_num()<<endl;
	  
	  if(AMSEvent::get_thread_num()==0)
	    {
	      
	      AMSJob::gethead()->uhend();
	      AMSJob::gethead()->uhinit(_run,getmid()+1,getmtime());
	      AMSNtuple::readRSetup(this);
	      Barrier()=false;
	    }
#pragma omp barrier 
	  if(GCFLAG.ITEST<0)GCFLAG.ITEST=-GCFLAG.ITEST;
	  
	}
    }        
  }

  AMSgObj::BookTimer.stop("WriteEvent");

}

//================================================================================================================================
void  AMSEvent::printA(integer debugl){
  if(debugl < 2){
    _printEl(cout);
    if(debugl==0)return;
    AMSNode * cur;
    for (int i=0;;){
      cur=AMSEvent::EventMap.getid(i++);   // get one by one
      if(cur){
	if(strncmp(cur->getname(),"AMSContainer:",13)==0 && strcmp(cur->getname(),"MC")!=0)
	  ((AMSContainer*)cur)->printC(cout);

      }
      else break;
    }
  }
  else{
    _printEl(cerr);
    AMSNode * cur;
    for (int i=0;;){
      cur=AMSEvent::EventMap.getid(i++);   // get one by one
      if(cur){
	if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
	  printC(cerr);
      }
      else break;
    }
  }

}

void AMSEvent::copy(){
  _copyEl();
  AMSNode * cur;
  for (int i=0;;){
    cur=AMSEvent::EventMap.getid(i++);   // get one by one
    if(cur){
      if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
	copyC();
    }
    else break;
  }



}
//------------------------------------------------------------------
void AMSEvent::event(){
  if(GCFLAG.IEVENT==1000){
  }
  addnext(AMSID("WriteAll",0),new Test());
  // First Selected Events
  if(_SelectedEvents){

    if(_SelectedEvents->Run==0){
#pragma omp critical (g4)
      {
	GCFLAG.IEORUN=1;
	GCFLAG.IEOTRI=1;
      }
      return;
    }
    else{
      EventId o(getrun(),getid());
      if(_SelectedEvents->Run > o.Run){
#pragma omp critical (g4)
	{
	  if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
	}
	return;
      }
      else if(*_SelectedEvents < o){
#pragma omp critical (g4)
	{
	  while(*_SelectedEvents<o && _SelectedEvents->Run){
	    if(_SelectedEvents->Event)cerr<<"AMSEvent::event-E-SelectedRunEventNotFound"<<_SelectedEvents->Run<<" "<<_SelectedEvents->Event<<endl;
	    _SelectedEvents++;
	  }
	}
	if(_SelectedEvents->Run!=o.Run){
          return;
	}
      }
      if(_SelectedEvents->Event){
	if(o!=*_SelectedEvents){
#pragma omp critical (g4)
	  {
	    if(! AMSJob::gethead()->getstatustable()->geteventpos(_SelectedEvents->Run,_SelectedEvents->Event,o.Event)){
	      SELECTFFKEY.Run=_SelectedEvents->Run;
	      SELECTFFKEY.Event=_SelectedEvents->Event;
	      DAQEvent::select();
	    }
	  }
	  return;
	}
#pragma omp critical (g4)
	{
	  _SelectedEvents++;
	}
      }       
    }
  }
  AMSgObj::BookTimer.start("EventStatus");
  const int size=sizeof(STATUSFFKEY.status)/sizeof(STATUSFFKEY.status[0]);
  if(STATUSFFKEY.status[size-2]){
    int ok=AMSJob::gethead()->getstatustable()->statusok(getid(),getrun());
    int skipped=0;
    if(!ok){
#pragma omp critical (g4)
      {
	skipped=AMSJob::gethead()->getstatustable()->getnextok();
	PosInRun+=skipped;
      }
    }
    AMSgObj::BookTimer.stop("EventStatus");
#pragma omp critical (g3)
    GCFLAG.IEVENT+=skipped;
    if(!ok)return;
  }
  AMSUser::InitEvent();
  try{
    if(AMSJob::gethead()->isSimulation())_siamsevent();
    AMSmceventg *ptr=(AMSmceventg*)getheadC("AMSmceventg",0);
    

    if(!CCFFKEY.Fast && !(!IOPA.hlun && !IOPA.WriteRoot && (DAQCFFKEY.mode/10)%10)){

      if(_id<=IOPA.skip) return;
      _reamsevent();
      if(AMSJob::gethead()->isCalibration())_caamsevent();

      _trdgain();
      _collectstatus();
    }
  }
  catch (AMSLVL3Error e){
    _collectstatus();
    // No LVL3
    //   if(AMSStatus::isDBWriteR() || AMSStatus::isDBUpdateR()){
    //    setstatus((AMSJob::gethead()->getstatustable()->getstatus(getid(),getrun())).getp());
    //   }
  }
  if(AMSStatus::isDBWriteR()){
    AMSJob::gethead()->getstatustable()->adds(getrun(),getid(),getstatus(),gettime());
  }
  else if(AMSStatus::isDBUpdateR()){
    AMSJob::gethead()->getstatustable()->updates(getrun(),getid(),getstatus(),gettime());
  }
}

//------------------------------------------------------------------
void AMSEvent::_siamsevent(){
  AMSgObj::BookTimer.start("SIAMSEVENT");
  int cftr;  
  AMSmceventg *ptr=(AMSmceventg*)getheadC("AMSmceventg",0);
  if(ptr){
    int iset;
    geant coo[7];
    abinelget_(iset,coo);
    if(iset){
      AMSmceventg* genp=new AMSmceventg(iset+256,coo[6],AMSPoint(coo[0],coo[1],coo[2]),AMSDir(coo[3],coo[4],coo[5]));
      addnext(AMSID("AMSmceventg",0), genp);

    }
  }
#ifdef _PGTRACK_
  if(IOPA.unitimegen){
    unsigned int tt[2];
    OrbGen* orb=OrbGen::GetOrbGen();
    tt[0]=orb->Time.Time_ns; tt[1]=orb->Time.Time_ns;
    TrExtAlignDB::ProduceDisalignment(tt);
  }
#endif
  _siecalevent();
  _sitof2event(cftr);//important to call after _siecalevent to use FT from EC
  //                       (TOF+ECAL)-combined FastTrigger(FT), this flag may be used by other subr.
  _sianti2event();//Anti(as TOF) is digitized only by combined FT (checked inside of subr.!)
  _sitrdevent(); 
  _sirichevent();
  _sitkevent(); 
  _sitrigevent();//create lev1/lev3 trig.object
  _sidaqevent(); //DAQ-simulation 
  Trigger2LVL1 *ptrt=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptrt && CCFFKEY.low==10){ // realistic orbit generator for plaen1/9 movement studies
    OrbGen::GetOrbGen()->NextTime();
  }    

  AMSgObj::BookTimer.stop("SIAMSEVENT");
}
//------------------------------------------------------------------------------------------------------------------------
void AMSEvent::_reamsevent(){
  AMSgObj::BookTimer.start("REAMSEVENT");  
  // get beam par, and other things  if any;
  _regnevent();
  if(AMSJob::gethead()->isReconstruction() && MISCFFKEY.BeamTest>1){
    // skip event if there is no mceventg record
    AMSContainer *p=getC("AMSmceventg",0);
    if(!p || p->getnelem()==0){
      cerr <<"_reamsevent-E-NomceventgRecord" <<getrun()<<endl;
      if(!GCFLAG.IEORUN && MISCFFKEY.BeamTest>1)GCFLAG.IEORUN=2;  //skip entire run
      AMSgObj::BookTimer.stop("REAMSEVENT");  
      return;
    }
  }
#ifdef __AMSDEBUG__
  _redaqevent();//create subdetectors RawEvent-Objects
#else
  if(AMSJob::gethead()->isReconstruction() ){
    _redaqevent();//create subdetectors RawEvent-Objects
  }
#endif
  geant d;
  if(AMSJob::gethead()->isMonitoring() && RNDM(d)>IOPA.Portion && GCFLAG.NEVENT>100){
    // skip event
    for(int i=0;;i++){
      AMSContainer *pctr=getC("TriggerLVL1",i);
      if(pctr)pctr->eraseC();
      else break ;
    }

    AMSgObj::BookTimer.stop("REAMSEVENT");  
    return;    
  }



  if(AMSJob::gethead()->isReconstruction() )_retrigevent();//attach needed subdets parts to existing lvl1-obj
  // copy some subdet-related info from lvl1 to subdet-objects(for example AntiRawEvent)
  if(DAQCFFKEY.SkipRec && AMSJob::gethead()->isSimulation()){
    static int ist=0;
    if(ist++<10)
      cout <<"AMSEvent::_reamsevent-W-ReconstructionWillNotBeDoneBecauseofDAQCCFFKEY.SkipRecRequired"<<endl;
    return;
  }
  //
  //----> below is a tempor.solution to speedup ped-type calibrations for tof/acc/ecal:
  bool calltrk(true),calltrd(true),callrich(true),callax(true),callecal(true),calluser(true);
  bool ecpedcal=((AMSJob::gethead()->isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]==5);
  bool tftdccal=((AMSJob::gethead()->isCalibration() & AMSJob::CTOF) && TFREFFKEY.relogic[0]==1);
  bool tfcal=((AMSJob::gethead()->isCalibration() & AMSJob::CTOF) && TFREFFKEY.relogic[0]>0);
  bool tfpedcal=((AMSJob::gethead()->isCalibration() & AMSJob::CTOF) && TFREFFKEY.relogic[0]==6);
  //
  calltrk  = (!(ecpedcal || tftdccal || tfpedcal));
  calltrd  = (!(ecpedcal || tftdccal || tfpedcal));
  callrich = (!(ecpedcal || tftdccal || tfpedcal));
  callax   = (!(ecpedcal || tftdccal || tfpedcal));
  calluser = (!(ecpedcal || tftdccal || tfpedcal));
  callecal = (!tfcal);
  if((ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2) && (AMSEvent::gethead()->getid())>90000)return;
  //



  if(getC("TriggerLVL1",0)->getnelem() ){
    try{
      _retof2event();
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _retof2event"<<endl;
      seterror(2);
    }
    try{
      _reanti2event();
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _reantievent"<<endl;
      seterror(2);
    }

    try{
      if(calltrd)_retrdevent();
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _retrdevent"<<endl;
      seterror(2);
    }
    
    if(calltrk){
      try{
	_retkevent();
      }
      catch (std::out_of_range &re){
	static int nerr=0;
	if(nerr++<100)cerr <<"_retkevent range_error exception catched "<<re.what()<<endl;
	seterror(2);
	
      }

      catch(std::bad_alloc a){
	cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _retof2event"<<endl;
	seterror(2);
      }
    } 
    try{
      if(callrich)_rerichevent();
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _rerichevent"<<endl;
      seterror(2);
    }
    try{
      if(callecal)_reecalevent();
    }
    catch(std::bad_alloc a){
      cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _reecalevent"<<endl;
      seterror(2);
    }
  }
  try{
    if(callax)_reaxevent();
  }
  catch(std::bad_alloc a){
    cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" _reaxevent"<<endl;
    seterror(2);
  }
  try{
    if(calluser)AMSUser::Event();
  }
  catch(std::bad_alloc a){
    cerr<<" AMSEvent::_reamsevent-E-BadALLOC in "<<getrun()<<" "<<getid()<<" AMSUser::Event"<<endl;
    seterror(2);
  }
#ifndef _PGTRACK_
  if(calltrk)AMSTrTrack::cleanup(); 
#endif

  AMSgObj::BookTimer.stop("REAMSEVENT");  
}

//------------------------------------------------------------------------------------------------------------------------



void AMSEvent::_caamsinitevent(){
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTRD)_catrdinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CRICH)_carichinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxinitevent();
}

void AMSEvent::_catrdinitevent(){
}

#ifndef _PGTRACK_
void AMSEvent::_catkinitevent(){

  if(TRCALIB.CalibProcedureNo == 2){
   add (
   new AMSContainer(AMSID("AMSContainer:AMSTrCalibration",0),0));
  }
}
#endif
void AMSEvent::_catofinitevent(){
}


void AMSEvent::_cantinitevent(){
}


void AMSEvent::_carichinitevent(){
}


void AMSEvent::_caaxinitevent(){
}


void AMSEvent::_caamsevent(){
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantievent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CEcal)_caecevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTRD)_catrdevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CRICH)_carichevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxevent();
}
#ifndef _PGTRACK_
void AMSEvent::_catkevent(){
  AMSgObj::BookTimer.start("CalTrFill");
    if(TRALIG.UpdateDB){
      if(!TRALIG.GlobalFit)AMSTrAligFit::Test();
      else AMSTrAligFit::Testgl();
    }
  if(TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check();
  }
  else if(TRCALIB.CalibProcedureNo == 2){
int i,j;
for(i=0;i<nalg;i++){
  if(TRCALIB.Method==0 || TRCALIB.Method==i){
   int ps=TRCALIB.PatStart;
   int pe=tkcalpat;
   if(TRCALIB.MultiRun)pe=ps+2;
   for(j=ps;j<pe;j++){
     if(AMSTrCalibFit::getHead(i,j)->Test()){
      AMSgObj::BookTimer.start("CalTrFit");
      AMSTrCalibFit::getHead(i,j)->Fit();
      AMSgObj::BookTimer.stop("CalTrFit");
     }
  }
 }
}
  }
  AMSgObj::BookTimer.stop("CalTrFill");

}
#endif

void AMSEvent::_trdgain(){

  if(!TrdHCalibR::gethead()->calibrate)return;
  if(TrdHReconR::gethead(AMSEvent::get_thread_num())->SelectEvent())
    if(TrdHReconR::gethead(AMSEvent::get_thread_num())->htrvec.size()==1 &&
       TrdHReconR::gethead(AMSEvent::get_thread_num())->SelectTrack(0) &&
       TrdHReconR::gethead(AMSEvent::get_thread_num())->hsegvec.size()==2){


      // get event beta and charge
      AMSParticle *ptr=(AMSParticle*)getheadC("AMSParticle",0);
      AMSParticle *ptr1=(AMSParticle*)getheadC("AMSParticle",1);
      int npart=0;
      AMSContainer *ptrc;
      if(ptr){
	ptrc=getC("AMSParticle",0);
      }
      else if(ptr1){
	ptr=ptr1;     
	ptrc=getC("AMSParticle",1);
      }

      if(ptr){
	npart=ptrc->getnelem();
	if(npart==1&&ptr->getpbeta()){
	  float beta=ptr->getpbeta()->getbeta();
	  

	  if(beta>0.98){
	    geant elosc[TOF2GC::SCLRS];
	    for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (TofClus containers) ---
	      AMSTOFCluster* ptrc=(AMSTOFCluster*)AMSEvent::gethead()->
		getheadC("AMSTOFCluster",ilay,0);
	      while(ptrc){ // <--- loop over AMSTofCluster hits in L=ilay
		elosc[ilay]=ptrc->getedep();//continious
		ptrc=ptrc->next();
	      }
	    }

	    geant *pntr[TOF2GC::SCLRS];
	    geant avera[4];
	    for(int il=0;il<TOF2GC::SCLRS;il++)pntr[il]=&elosc[il];//pointers to layer edep's 
	    AMSsortNAG(pntr,TOF2GC::SCLRS);//sort in increasing order
	    avera[0]=(*pntr[0]);// lowest
	    avera[1]=avera[0]+(*pntr[1]);// sum of 2 lowest
	    avera[2]=avera[1]+(*pntr[2]);//        3 lowest
	    avera[3]=avera[2]+(*pntr[3]);// average
	    avera[1]/=2.;
	    avera[2]/=3.;
	    avera[3]/=4.;
	    
	    // select charge 1
	    if(avera[2]<3){
	      //calibrate trd here
#pragma omp critical (trd)
	      TrdHCalibR::gethead()->update_medians(&TrdHReconR::gethead(AMSEvent::get_thread_num())->htrvec[0]);
	    }
	  }
	}
      }
    }
}  

void AMSEvent::_catrdevent(){
  AMSgObj::BookTimer.start("CalTRDFill");
  
  
  if(TRDCALIB.CalibProcedureNo == 1){
    AMSTRDIdCalib::check();
  }
  AMSgObj::BookTimer.stop("CalTRDFill");

}

//---------------------------------------------------------------------------
void AMSEvent::_catofevent(){
  bool tofft(0);
  integer cmode=TFREFFKEY.relogic[0];
  Trigger2LVL1 *ptr2;
//
    ptr2=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    if(ptr2)tofft=ptr2->TofFasTrigOK();
    if(!tofft)return;// use only TOF-triggered event
//
    if(cmode==2 || cmode==3 || cmode==4 || cmode==23 || cmode==234 || cmode==34){
      TofTmAmCalib::select();//event selection for TOF Tdelv/Tzslw/Ampl-calibration
    }
//
}
//---------------------------------------------------------------------------

void AMSEvent::_cantievent(){
  bool globft(0);
  Trigger2LVL1 *ptr2;
//
    ptr2=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    if(ptr2)globft=ptr2->GlobFasTrigOK();
    if(!globft)return;// use only H/W-triggered event
    if(ATREFFKEY.relogic==1)AntiCalib::select();
}
//--------------------------------------------------------------------------
void AMSEvent::_caecevent(){
  bool globft(false);
  Trigger2LVL1 *ptr;
//
    ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    if(ptr){
      globft=ptr->GlobFasTrigOK();
    }
    if(!globft)return;// use only H/W-triggered event
    if(ECREFFKEY.relogic[1]>0){
#pragma omp critical (eccal_anor)
{
      if(ECREFFKEY.relogic[1]<=2)ECREUNcalib::select();// RLGA/FIAT part of REUN-calibration
      if(ECREFFKEY.relogic[1]==3)ECREUNcalib::selecte();// ANOR part of REUN-calibration
// PedCal mode has no special select-routine( select/fill is done at validate stages)
}
    }
}

//---------------------------------------------------------------------------
void AMSEvent::_carichevent(){
#pragma omp critical (richcalvent)
  AMSRichCal::process_event();
}
//--------------------------------------------------------------------------
void AMSEvent::_caaxevent(){
}
//============================================================================
#ifndef _PGTRACK_
void AMSEvent::_retkevent(integer refit){

// do not reconstruct events without lvl3 if  LVL3FFKEY.Accept
 
    AMSlink *ptr=getheadC("TriggerLVL3",0);
    AMSlink *ptr1=getheadC("TriggerLVL1",0);

    TriggerLVL302 *ptr302=dynamic_cast<TriggerLVL302*>(ptr);
     
if(ptr1 && (!LVL3FFKEY.Accept ||  (ptr1 && ptr && (ptr302 && ptr302->LVL3OK())))){//tempor
 AMSgObj::BookTimer.start("RETKEVENT");
  AMSgObj::BookTimer.start("TrCluster");
  buildC("AMSTrCluster",refit);
  AMSgObj::BookTimer.stop("TrCluster");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrCluster::print();
#endif
  AMSgObj::BookTimer.start("TrRecHit");
  buildC("AMSTrRecHit",refit);
  AMSgObj::BookTimer.stop("TrRecHit");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrRecHit::print();
#endif
  AMSgObj::BookTimer.start("TrTrack");
  

  bool veto=true;
  bool hmul=false;
  for( AMSTRDTrack *ptrd=(AMSTRDTrack*)getheadC("AMSTRDTrack",0);ptrd;ptrd=ptrd->next()){
    if(ptrd->IsHighGammaTrack()){
     hmul=true;
     break;
    }
    else if(!(veto=ptrd->Veto(trdconst::maxlad-1))){
     break;
    }
  }
//   cout <<" ptrd "<<hmul<<"  "<<veto<<endl;
  Trigger2LVL1 * ptr12= dynamic_cast<Trigger2LVL1*>(ptr1);
  if(ptr12 && (ptr12->IsECHighEnergy() || ptr12->IsECEMagEnergy() || TRFITFFKEY.LowMargin || hmul  || veto)){
    AMSTrTrack::setMargin(1);
  }
  else{
   AMSTrTrack::setMargin(0);
  }
  integer itrk=1;

   if(TRFITFFKEY.OldTracking ){
     //  Old Stuff
//     cout <<" old stuff "<<TRFITFFKEY.OldTracking<<endl; 
  // Default reconstruction: 4S + 4K or more
  if(TRFITFFKEY.FalseXTracking && !TRFITFFKEY.FastTracking)
    itrk = buildC("AMSTrTrackFalseX",TKDBc::nlay());
  if(itrk>0)itrk=buildC("AMSTrTrack",refit);
  // Reconstruction with looser cuts on the K side
  if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.WeakTracking ){
    buildC("AMSTrClusterWeak",refit);
    buildC("AMSTrRecHitWeak",refit);
    itrk = buildC("AMSTrTrackWeak",refit);
  }

  if(TRFITFFKEY.FastTracking){
    // Reconstruction of 4S + 3K
    if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.FalseXTracking ){
      itrk=buildC("AMSTrTrackFalseX",TKDBc::nlay()-3);
      if(itrk>0) itrk=buildC("AMSTrTrack",refit);
#ifdef __AMSDEBUG__
      if(itrk>0)cout << "FalseX - Track found "<<itrk<<endl; 
#endif
    }
  }
  // Reconstruction of 4S + TOF
  int flag =    (itrk<=0 && TRFITFFKEY.FalseTOFXTracking)
             || (TRFITFFKEY.FullReco && TRFITFFKEY.FalseTOFXTracking)
             || TRFITFFKEY.ForceFalseTOFX;
  if ( flag) {
    itrk=buildC("AMSTrTrackFalseTOFX",refit);
#ifdef __AMSDEBUG__
    if (itrk>0) cout << "FalseTOFX - Track found "<< itrk << endl;
#endif
  }
 }
 else{
  //
  // New (JA) Stuff
  //  Different logic too
  //
//     cout <<" new stuff "<<TRFITFFKEY.OldTracking<<endl; 
    // Default reconstruction: 4S + 4K or more
  if(TRFITFFKEY.FalseXTracking && !TRFITFFKEY.FastTracking)
    itrk = buildC("AMSTrTrackFalseX",TKDBc::nlay());
       AMSgObj::BookTimer.start("TrTrackPathI");
  itrk=buildC("AMSTrTrack_PathIntegral",refit);
       AMSgObj::BookTimer.stop("TrTrackPathI");
  // Reconstruction with looser cuts on the K side
  if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.WeakTracking ){
    buildC("AMSTrClusterWeak",refit);
    buildC("AMSTrRecHitWeak",refit);
       AMSgObj::BookTimer.start("TrTrackPathI");
    itrk = buildC("AMSTrTrackWeak_PathIntegral",refit);
       AMSgObj::BookTimer.stop("TrTrackPathI");
  }

  if(TRFITFFKEY.FastTracking){
    // Reconstruction of 4S + 3K
    if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.FalseXTracking ){
      itrk=buildC("AMSTrTrackFalseX",TKDBc::nlay()-3);
       AMSgObj::BookTimer.start("TrTrackPathI");
      if(itrk>0){
        itrk=buildC("AMSTrTrack_PathIntegral",refit);
      }
       AMSgObj::BookTimer.stop("TrTrackPathI");
#ifdef __AMSDEBUG__
      if(itrk>0)cout << "FalseX - Track found "<<itrk<<endl; 
#endif
    }
  }
  // Reconstruction of 4S + TOF
  int flag =    (itrk<=0 && TRFITFFKEY.FalseTOFXTracking)
             || (TRFITFFKEY.FullReco && TRFITFFKEY.FalseTOFXTracking)
             || TRFITFFKEY.ForceFalseTOFX;
  if ( flag) {
       AMSgObj::BookTimer.start("TrTrackFalseTOFX");
    itrk=buildC("AMSTrTrackFalseTOFX",refit);
       AMSgObj::BookTimer.stop("TrTrackFalseTOFX");
#ifdef __AMSDEBUG__
    if (itrk>0) cout << "FalseTOFX - Track found "<< itrk << endl;
#endif
  }
  
 }
  
  AMSgObj::BookTimer.stop("TrTrack");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrTrack::print();
#endif
  
 AMSgObj::BookTimer.stop("RETKEVENT");
}
 else throw AMSLVL3Error("LVL3NotCreated");  
}

#endif 

//----------------------------------------
void AMSEvent::_reanti2event(){
  int stat;
  bool tofftok(false),ecalftok(false),extrigok(false);
//
//
  AMSgObj::BookTimer.start("REANTIEVENT");
//
  ANTI2JobStat::addre(0);
//
//
//
    Trigger2LVL1 *ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    if(ptr){
      tofftok=ptr->TofFasTrigOK();
      ecalftok=ptr->EcalFasTrigOK();
      extrigok=ptr->ExternTrigOK();
    }
    if(!(tofftok || ecalftok || extrigok)){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;// "no TOF/EC/Ext in LVL1-trigger"
    }
    ANTI2JobStat::addre(1);
    if(tofftok)ANTI2JobStat::addre(5);
    else if(ecalftok)ANTI2JobStat::addre(6);
//
    Anti2RawEvent::validate(stat);// RawEvent-->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTI2JobStat::addre(2);
//
//
    AMSAntiCluster::build2(stat);// RawEvent->Cluster
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTI2JobStat::addre(3);
// 
  #ifdef __AMSDEBUG__
   if(AMSEvent::debug)AMSAntiCluster::print();
  #endif
//
  AMSgObj::BookTimer.stop("REANTIEVENT");
}
//===============================================================================
void AMSEvent::_retof2event(){
int stat;
static int firstev(0);
#pragma omp threadprivate(firstev)
uinteger runtyp=AMSEvent::getruntype();
bool tofftok(false),ecalftok(false),extrigok(false);
//
//
  AMSgObj::BookTimer.start("RETOFEVENT");
    TOF2JobStat::addre(0);
    Trigger2LVL1 *ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
    if(ptr){
      tofftok=ptr->TofFasTrigOK();
      ecalftok=ptr->EcalFasTrigOK();
      extrigok=ptr->ExternTrigOK();
    }
    if(!(tofftok || ecalftok || extrigok)){
      AMSgObj::BookTimer.stop("RETOFEVENT");
      if(firstev==0){
        cout<<"<======== Warning-Retofevent: no TOF,ECAL or EXTERN trigger, runtype="<<runtyp<<endl;
        firstev=1;
      }
      return;// "no TOF/EC/Ext in LVL1-trigger"
    }   
    TOF2JobStat::addre(1);
    if(tofftok)TOF2JobStat::addre(5);
    else if(ecalftok)TOF2JobStat::addre(6);
//
//                   ===> reco of real or MC events :
//
//
      AMSgObj::BookTimer.start("TOF:validation");
      TOF2RawSide::validate(stat);// RawEvent-->RawEvent
      AMSgObj::BookTimer.stop("TOF:validation");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(2);
//
      AMSgObj::BookTimer.start("TOF:RwEv->RwCl");
      TOF2RawCluster::build(stat); // RawEvent-->RawCluster
      AMSgObj::BookTimer.stop("TOF:RwEv->RwCl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(3);
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build2(stat); // RawCluster-->Cluster
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOF2JobStat::addre(4);
//
  #ifdef __AMSDEBUG__
  if(AMSEvent::debug){
    AMSTOFCluster::print();
  }
  #endif
  AMSgObj::BookTimer.stop("RETOFEVENT");
}
//========================================================================
void AMSEvent::_reecalevent(){
  Trigger2LVL1 *ptr;
  int stat(0),nsuc(0);
  bool tofftok(false),ecalftok(false),extrigok(false);
//
  AMSgObj::BookTimer.start("REECALEVENT");
//
  EcalJobStat::addre(0);
  ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptr){
    tofftok=ptr->TofFasTrigOK();
    ecalftok=ptr->EcalFasTrigOK();
    extrigok=ptr->ExternTrigOK();
  }
  if(!(tofftok || ecalftok || extrigok)){
    AMSgObj::BookTimer.stop("REECALEVENT");
    return;// "no FT/Ext-trig in LVL1"   
  }
//  if((ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5) && ecalftok)return;//if PedCalib->remove events with EcalFT IN
// !!! not needed to my mind, Truncation+SpikeSuppression do the job (need to recheck with real data !) 
  EcalJobStat::addre(1);
  if(ecalftok)EcalJobStat::addre(2);
  if(tofftok && ecalftok)EcalJobStat::addre(3);
//
  AMSEcalRawEvent::validate(stat);// EcalRawEvent->EcalRawEvent
  if(stat!=0){
    AMSgObj::BookTimer.stop("REECALEVENT");
    return;
  }
  EcalJobStat::addre(4);
//
  AMSEcalHit::build(stat);// EcalRawEvent->EcalHit
  if(stat!=0){
    AMSgObj::BookTimer.stop("REECALEVENT");
    return;
  }
  EcalJobStat::addre(5);
//
      
       AMSgObj::BookTimer.start("ReEcalShowerFit");
       //two phase run due to att corr 
      for(int i=0;i<2;i++){
       buildC("Ecal1DCluster",i);
       buildC("Ecal2DCluster",i);
       int suc=buildC("EcalShower",i);
//        cout <<" succ*** "<<i<<" "<<suc<<endl;
       if(!suc)break;
       nsuc+=1;
      }
      if(nsuc==2)EcalJobStat::addre(6);

       AMSgObj::BookTimer.stop("ReEcalShowerFit");

//
//
//
//
  AMSgObj::BookTimer.stop("REECALEVENT");
}
//========================================================================
void AMSEvent::_retrdevent(){
//
  AMSgObj::BookTimer.start("RETRDEVENT");
  buildC("AMSTRDCluster");
#ifdef __AMSDEBUG__
  for(int i=0;i<trdconst::maxlay;i++){
   AMSContainer *p =getC("AMSTRDCluster",i);
   if(p && AMSEvent::debug)p->printC(cout);
//   if(p )p->printC(cout);
  }
#endif

  int nseg=buildC("AMSTRDSegment");
#ifdef __AMSDEBUG__
  for(int i=0;i<trdconst::maxseg;i++){
   AMSContainer *p =getC("AMSTRDSegment",i);
   if(p && AMSEvent::debug)p->printC(cout);
  }
#endif
  buildC("AMSTRDTrack");   
#ifdef __AMSDEBUG__
  for(int i=0;i<1;i++){
   AMSContainer *p =getC("AMSTRDTrack",i);
   if(p && AMSEvent::debug)p->printC(cout);
  }
#endif
  AMSgObj::BookTimer.stop("RETRDEVENT");

  if(TRDFITFFKEY.FitMethod!=0){
    AMSgObj::BookTimer.start("RETRDHEVENT");

#ifdef __MLD__
    cout << " trd before "<<endl;
    TrdRawHitR::print();
    TrdHSegmentR::print();
    TrdHTrackR::print();
#endif
    
    TrdHReconR::gethead(AMSEvent::get_thread_num())->clear();
    TrdHReconR::gethead(AMSEvent::get_thread_num())->build();

#ifdef __MLD__
    cout << " trd after "<<endl;
    TrdRawHitR::print();
    TrdHSegmentR::print();
    TrdHTrackR::print();
#endif

    AMSgObj::BookTimer.stop("RETRDHEVENT");
  }
  
}
void AMSEvent::_rerichevent(){
  Trigger2LVL1 *ptr;
  int stat;
  bool globftok(0),extrigok(0);
//
  AMSgObj::BookTimer.start("RERICH");

  AMSRichRawEvent::build(); // Preprocess the hits

//
  ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptr){
    globftok=ptr->GlobFasTrigOK();
    extrigok=ptr->ExternTrigOK();
  }
  if(!globftok && !extrigok){
    AMSgObj::BookTimer.stop("RERICH");
    return;// "no FT/Ext-trig in LVL1 trigger"   
  }
  // Reconstruction CJD
  if(RICRECFFKEY.recon[0]%10) {
    AMSRichRing::build();
  }
  // LIP reconstruction
  if((RICRECFFKEY.recon[0]/10)%10)

    //temporary critical statement
    //while debug of parallelized LIP code is in progress
// #pragma omp critical (lipreconst)
 
{
    AMSgObj::BookTimer.start("RERICHLIP");
    AMSRichRingNewSet NewRings;
    NewRings.init();
    NewRings.build();
    //cout << "------------------------------ LIP results " << endl;
    //cout << "Number of LIP Rings = " << NewRings.NumberOfRings() << endl;
    NewRings.reset();
    AMSgObj::BookTimer.stop("RERICHLIP");
  }

  AMSgObj::BookTimer.stop("RERICH");
}
//========================================================================
void AMSEvent::_reaxevent(){
  AMSgObj::BookTimer.start("REAXEVENT");



  buildC("AMSBeta");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSBeta::print();
#endif
  buildC("AMSCharge");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSCharge::print();
#endif

  // Flag golden tracks here
#ifndef _PGTRACK_
  //PZ FIXME FOR VERTEX
  AMSTrTrack::flag_golden_tracks();
#endif

  // Vertexing
  AMSgObj::BookTimer.start("Vtx");
#ifdef _PGTRACK_
  //PZ FIXME DEBUG 
  //   TrRecon::gethead()->BuildVertex();
#else
  buildC("AMSVtx");
#endif
  AMSgObj::BookTimer.stop("Vtx");

  AMSgObj::BookTimer.start("REAXPART");
  buildC("AMSParticle");
  AMSgObj::BookTimer.stop("REAXPART");

#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSParticle::print();
#endif


  AMSgObj::BookTimer.stop("REAXEVENT");
}

#ifndef _PGTRACK_
void AMSEvent::_sitkinitrun(){
     int l;
     for(l=0;l<2;l++){
       for (int i=0;i<TKDBc::nlay();i++){
         for (int j=0;j<TKDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
             id.upd(k);
             geant d;
             id.setindnoise()=oldone+
             AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
             oldone=id.getindnoise();
            }
           }
         }
       }
     }
     // Fill the histos
     for(l=0;l<2;l++){
      geant p0=AMSTrMCCluster::sitknoiseprobU(TRMCFFKEY.thr1R[l],AMSTrMCCluster::step());
      for(int i=0;i<AMSTrMCCluster::ncha();i++){
        geant x=TRMCFFKEY.thr1R[l]+i*AMSTrMCCluster::step();
        geant prob=AMSTrMCCluster::sitknoiseprobU(x,AMSTrMCCluster::step());
        HF1(AMSTrMCCluster::hid(l),x,prob/p0);
      }
#ifdef __AMSDEBUG__
      HPRINT(AMSTrMCCluster::hid(l));
#endif
#ifdef __G4AMS__
        if(MISCFFKEY.G4On)AMSRandGeneral::book(AMSTrMCCluster::hid(l));
#endif
     }
}

#endif


void AMSEvent::_sitofinitrun(){
}

void AMSEvent::_siantiinitrun(){
}


void AMSEvent::_siecalinitrun(){
}
void AMSEvent::_sitrdinitrun(){
}
void AMSEvent::_sirichinitrun(){
}

#ifndef _PGTRACK_
void AMSEvent::_retkinitrun(){
  // Warning if TRFITFFKEY.FastTracking is on...
  if(TRFITFFKEY.FastTracking){
    cout <<" "<<endl<<
    "AMSEvent::Retkinitrun-W-TRFITFFKEY.FastTracking selected. "<<TRFITFFKEY.FastTracking<<endl;
  }
  for (int half=0;half<2;half++){
    for(int side=0;side<2;side++){
      cout <<"AMSEvent::_retkinitrrun-I-"<<AMSTrIdCalib::CalcBadCh(half,side)<<
      " bad channels found for half "<<half<<" and side "<<side<<endl;
    }
  }
}
#endif
//----
void AMSEvent::_retofinitrun(){
}
//----
void AMSEvent::_reantiinitrun(){
  static bool accswap(false);
  if(getrun()<1211886677 && AMSJob::gethead()->isRealData()){
    if(!accswap){
      cout<<"======================================================"<<endl;
      cout<<"========> ACC de-swapping is ON starting from run "<<getrun()<<endl;
      cout<<"======================================================"<<endl;
    }
    accswap=true;
  }
  else{
    if(accswap){
      cout<<"======================================================"<<endl;
      cout<<"========> ACC de-swapping is OFF starting from run "<<getrun()<<endl;
      cout<<"======================================================"<<endl;
    }
    accswap=false;
  }
}

//----
void AMSEvent::_reecalinitrun(){
/*
  if((AMSJob::gethead()->isCalibration() & AMSJob::CEcal) && ECREFFKEY.relogic[1]<=0){
    if(AMSECIdCalib::Run()!=0){
     AMSECIdCalib::getaverage();
     AMSECIdCalib::clear();
   }
   AMSECIdCalib::Run()=getrun();
   AMSECIdCalib::Time()=gettime();   
  }





  if(AMSJob::gethead()->isRealData()){
geant gains[18][14];
ifstream fbin;
fbin.open("gains.gains");
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
   for(int j=0;j<14;j++)gains[i][j]=1;
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

   
   for(int i=0;i<ecalconst::ECSLMX;i++){
       for(int j=0;j<7;j++){
              ECcalib::ecpmcal[i][j].pmrgain()=1;
         for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<endl;             
              ECcalib::ecpmcal[i][j].pmscgain(k)=gains[ids.getlayer()][ids.getcell()];
           cout <<"  new gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}
*/
}
//----
void AMSEvent::_retrdinitrun(){
   for (int i=0;i<AMSTRDIdSoft::ncrates();i++){
      cout <<"AMSEvent::_retrdevent-I-"<<AMSTRDIdSoft::CalcBadCh(i)<<
      " bad channels found for crate "<<i<<endl;
}
//
}
void AMSEvent::_rerichinitrun(){
}

void AMSEvent::_reaxinitrun(){
}


#ifndef _PGTRACK_

void AMSEvent:: _sitkevent(){
bool fastrigger= TOF2RawSide::GlobFasTrigOK();
  if(TRMCFFKEY.NoiseOn && fastrigger )AMSTrMCCluster::sitknoise();
  AMSTrMCCluster::sitkcrosstalk();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTrMCCluster",0);
//  if(p && AMSEvent::debug>1 )p->printC(cout);
   if(p && AMSEvent::debug )p->printC(cout);
#endif
    AMSTrRawCluster::sitkdigi();
#ifdef __AMSDEBUG__
  p =getC("AMSTrRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p =getC("AMSTrRawCluster",1);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}
#endif
//----------------------------------------------------------------
void AMSEvent:: _sianti2event(){
  int stat;
  AMSgObj::BookTimer.start("SIANTIEVENT");
  ANTI2JobStat::addmc(0);
//
//
    Anti2RawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("SIANTIEVENT");
      return;// 
    }
    ANTI2JobStat::addmc(6);
//
//
#ifdef __AMSDEBUG__
  AMSContainer *p;
  p=getC("AMSAntiMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SIANTIEVENT");
}
//----------------------------------------------------------------
void AMSEvent:: _sitrdevent(){

  AMSgObj::BookTimer.start("SITRDDigi");
    AMSTRDRawHit::sitrddigi();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTRDRawHit",0);
  if(p && AMSEvent::debug )p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SITRDDigi");

}
//----------------------------------------------------------------
void AMSEvent:: _siecalevent(){
  int stat;
//
/*
   int16u tesarr[200];
   int16u *p;
   char fname[80];
   strcpy(fname,"/f2users/choumilo/ams02wrk/ecalcftes1.dat");
   cout<<"Opening RawFormat test file: "<<fname<<'\n';
   ifstream stfile(fname,ios::in|ios::binary); // open file for reading
   if(!stfile){
     cout <<"siecinitevent(): missing Ecal RawFormat test-file "<<fname<<endl;
     exit(1);
   }
   stfile >>hex>> tesarr[0];//read 1st block length
   integer leng1=tesarr[0];
   cout<<"siecinitevent(): 1st block has length="<<dec<<leng1<<endl;
   if(leng1>0 && (leng1+1)<200){
     for(int i=0;i<leng1;i++){
       stfile >>hex>> tesarr[i+1];
     }
     cout<<dec<<endl;
     p=&tesarr[0];
     DAQECBlock::buildraw(leng1, p);
     cout<<"siecinitevent(): 1st block was processed !"<<endl;
//     exit(10);
   }
   else{
     cout <<"siecinitevent(): 1st block length 0 or too big - exit ! "<<endl;
     exit(1);
   }
   stfile >>hex>> tesarr[leng1+1];//read 2nd block length
   integer leng2=tesarr[leng1+1];
   cout<<"siecinitevent(): 2nd block has length="<<dec<<leng2<<endl;
   if(leng2>0 && (leng1+leng2+2)<200){
     for(int i=0;i<leng2;i++){
       stfile >>hex>> tesarr[i+leng1+2];
     }
     cout<<dec<<endl;
     p=&tesarr[leng1+1];
     DAQECBlock::buildraw(leng2, p);
     cout<<"siecinitevent(): 2nd block was processed !"<<endl;
//     exit(10);
   }
   else{
     cout <<"siecinitevent(): 2nd block length 0 or too big "<<endl;
     exit(1);
   }
*/
//
  AMSgObj::BookTimer.start("SIECALEVENT");
  EcalJobStat::addmc(0);
//
  if(ECMCFFKEY.fastsim==0){//           ===> slow algorithm:
//
    AMSEcalRawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("SIECALEVENT");
      return;
    }
    EcalJobStat::addmc(1);
//
  }
  else{    //                         ===> fast algorithm:
//
//    to be done
  }
//
#ifdef __AMSDEBUG__
  AMSContainer *p;
  p=getC("AMSEcalMCHit",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SIECALEVENT");
}
//----------------------------------------------------------------


void AMSEvent::_sirichevent(){
  AMSgObj::BookTimer.start("SIRICH");
  AMSRichRawEvent::mc_build();
  AMSPoint r(0,0,0);
  AMSPoint u(0,0,0);
  addnext(AMSID("AMSRichMCHit",0),
                               new AMSRichMCHit(Noise,0,0,r,u,Status_Fake));
  AMSgObj::BookTimer.stop("SIRICH");
}

//---------------------------------------------------------------
void AMSEvent:: _sitrigevent(){
    Trigger2LVL1::build();//build complete(!) lvl1-obj 
    TriggerLVL302::build();
//below is for LVL1 builddaq-test:
/*
  int16u tesar[53];
  tesar[0]=52;
  int16u * p2wr=&tesar[1];
  Trigger2LVL1 *ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptr){
  integer nblocks=Trigger2LVL1::getmaxblocks();
  cout<<"-----> nblocks="<<nblocks<<endl;
  integer leng=Trigger2LVL1::calcdaqlength(0);
  cout<<"       daqleng="<<leng<<endl;
  Trigger2LVL1::builddaq(0, leng, p2wr);
  Trigger2LVL1::EventBitDump(leng,tesar,"MC DAQ test-array done:");
  Trigger2LVL1::buildraw(leng, p2wr);
  exit(11);
  }
*/
//below is for Tof/Acc builddaq-test:
/*
  int16u tesar[1001];
  integer nblocks,leng;
  int16u * p2wr=&tesar[1];
  Trigger2LVL1 *ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptr){
  nblocks=DAQS2Block::getmaxblocks();
  cout<<"-----> Total nblocks="<<nblocks<<endl;
  for(int ibl=0;ibl<nblocks;ibl++){
    leng=DAQS2Block::calcblocklength(ibl);//<0 due to VC
    leng=-leng;
    tesar[0]=int16(leng);
    cout<<"<----- CalcBlockLeng for crate="<<ibl+1<<" gives blockleng="<<leng<<endl<<endl;
    if(leng>1000){
      cout<<"<----- Length too big !!!"<<endl;
      exit(10);
    }
    DAQS2Block::buildblock(ibl,leng,p2wr);
    DAQS2Block::EventBitDump(leng,tesar,"MC TofAcc-Daq test-array done:");
    DAQS2Block::buildraw(leng, p2wr);
  }
  exit(11);
  }
*/
//below is for ECAL builddaq-test:
/*
  integer id,icr;
  geant padc[3];
  AMSEcalRawEvent * ptr;
//
  int16u tesar[2001];
  integer nblocks,leng(0);
  int16u * p2wr=&tesar[1];
  Trigger2LVL1 *ptrt=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptrt){
  nblocks=DAQECBlock::getmaxblocks();
  cout<<"-----> Total nblocks="<<nblocks<<endl;
//----
  for(int ibl=0;ibl<nblocks;ibl++){
    leng=DAQECBlock::calcblocklength(ibl);//<0 due to VC
//
//
    leng=-leng;
    tesar[0]=int16(leng);
    cout<<"<----- CalculBlockLeng for crate="<<ibl+1<<" gives blockleng="<<leng<<endl<<endl;
    if(leng>2000){
      cout<<"<----- Length too big !!!"<<endl;
      exit(10);
    }
    DAQECBlock::buildblock(ibl,leng,p2wr);
//    DAQECBlock::EventBitDump(leng,tesar,"MC Ecal-Daq test-array done:");
    DAQECBlock::buildraw(leng, p2wr);
//
  }
  exit(11);
  }
*/
}

//---------------------------------------------------------------
void AMSEvent:: _retrigevent(){
//Add missing info to HW-created(in redaqevent) lvl1-obj; can "simulate" trigger 1 & 3 for rec data
   Trigger2LVL1::build();
   if( LVL3FFKEY.RebuildLVL3){
         TriggerLVL302::build();
    }
}


//===============================================================
void AMSEvent:: _sitof2event(int &cftr){
  AMSContainer *p;
  int stat,i;
//
  AMSgObj::BookTimer.start("SITOFDIGI");
   AMSgObj::BookTimer.start("TOF:Ghit->Tovt");
   TOF2JobStat::addmc(0);
//
//   cout << "before build "<<endl;
//   AMSmceventg::PrintSeeds(cout);
   TOF2Tovt::build();// Ghits->TovT-hits
//    cout <<"after build "<<endl;
   AMSgObj::BookTimer.stop("TOF:Ghit->Tovt");
   TOF2JobStat::addmc(1);
//
   AMSgObj::BookTimer.start("TOF:Tovt->RwEv");
   TOF2RawSide::mc_build(stat);//Tovt_hit->RawSide_hit
   AMSgObj::BookTimer.stop("TOF:Tovt->RwEv");
   if(stat!=0){
     cftr=0;//no (TOF+ECAL)-combined fast trigger
     AMSgObj::BookTimer.stop("SITOFDIGI");
     return; // no FTrigger from TOF or ECAL
   }
   cftr=1;//found (TOF+ECAL)-combined fast trigger
   TOF2JobStat::addmc(2);
  AMSgObj::BookTimer.stop("SITOFDIGI");
//
#ifdef __AMSDEBUG__
  p=getC("AMSTOFMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p =getC("TOF2RawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}
//=============================================================

void AMSEvent::_findC(AMSID & id){

  static char names[1024]="AMSContainer:";
  #pragma omp threadprivate (names)
  names[13]='\0';
  if(id.getname())strcat(names,id.getname());
  id.setname(names); 


}


integer AMSEvent::setbuilderC(const char name[], pBuilder pb){
  AMSID id(name,0);
  _findC(id);
  AMSContainer *p = (AMSContainer*)getp(id);
  if(p){
   p->setbuilder(pb);
   return 1;
  }
  else return 0;

}

integer AMSEvent::buildC(const char name[], integer par){
   AMSID id(name,0);
   _findC(id);
   AMSContainer *p = (AMSContainer*)getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 
  
}

integer AMSEvent::rebuildC(const char name[], integer par){
   AMSID id(name,0);
   _findC(id);
  for(int i=0;;i++){
   id.setid(i);
   AMSContainer *p = (AMSContainer*)getp(id);
   if(p){
     p->eraseC();
   }
   else break;
  }
   id.setid(0);
   AMSContainer *p = (AMSContainer*)getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 


}


AMSlink * AMSEvent::_getheadC( AMSID id, integer sorted){
_findC(id);
  AMSContainer *p = (AMSContainer*)getp(id);
  if(p){
    if(sorted)p->sort(sorted-1);
    // Reset global ref if any
    if(p->gethead())(p->gethead())->resethash(id.getid(),p->gethead());
    return p->gethead();
  }
  else return 0;
}
AMSlink * AMSEvent::_getlastC( AMSID id){
_findC(id);
  AMSContainer *p = (AMSContainer*)getp(id);
  if(p){
    return p->getlast();
  }
  else return 0;
}

integer AMSEvent::_setheadC( AMSID id, AMSlink *head){
_findC(id);
  AMSContainer *p = (AMSContainer*)getp(id);
  if(p && head){
   p->sethead(head);
   return 1;
  }
  else return 0;
}

AMSContainer * AMSEvent::_getC( AMSID  id){
_findC(id);  
  AMSContainer *p = dynamic_cast<AMSContainer*>(getp(id));
  return p;
}

integer AMSEvent::getnC(const char n[]){
  AMSID id;
  _findC(id);
  AMSContainer *p;
  for(int i=0;;i++){
   id.setid(i);
   p = (AMSContainer*)getp(id);
   if(p==0) return i;
  }
}

void AMSEvent::_copyEl(){
}

void AMSEvent::_printEl(ostream & stream){
  stream <<endl<< "======>>> Run:"<<_run<<" "<<getname()<<" "<< getid()<<" Time: " 
   <<ctime(&_time)<<"."<<_usec<<" Station R:"<<_StationRad<<" Theta:"<<_StationTheta*AMSDBc::raddeg
   <<" Phi:"<<_StationPhi*AMSDBc::raddeg<<" Speed:"<<_StationSpeed
   <<" Pole:"<<_NorthPolePhi*AMSDBc::raddeg<<endl;
  stream <<"   TOFSlot1Temper(4crates):"<<TOF2JobStat::gettemp(0,0)<<","<<TOF2JobStat::gettemp(1,0)
                                          <<","<<TOF2JobStat::gettemp(2,0)<<","<<TOF2JobStat::gettemp(3,0);
  Trigger2LVL1 *ptr=(Trigger2LVL1*)getheadC("TriggerLVL1",0);
  if(ptr){
    stream <<" FastTrigRate/LiveTime:"<<Trigger2LVL1::scalmon.FTrate()<<"/"<<ptr->getlivetime();
  }
  //PZ FIXME OBSOLETE stream <<" AverMagnetTemper:"<<MagnetVarp::getmeanmagnetmtemp()<<endl;
 stream <<" Thread "<<AMSEvent::get_thread_num()<<endl;
}
//=====================================================================
void AMSEvent::_writeEl(){
// Get event length
  DAQEvent *myp=(DAQEvent*)getheadC("DAQEvent",0);
  int nws=myp?myp->getlength():0;
// Fill the ntuple
  EventNtuple02* EN = AMSJob::gethead()->getntuple()->Get_event02();
#ifndef _PGTRACK_
  EN->BAv=_ccebp?_ccebp->getBAv()*(_ccebp->BOK()?1:0):-1;
#else
  EN->BAv=_ccebp?_ccebp->getBAv():0;
#endif
  EN->TempTracker=_ccebp?_ccebp->getTAv():-273;
  EN->TrStat= trstat;
  EN->EventStatus[0]=getstatus()[0];
  EN->EventStatus[1]=getstatus()[1];
  EN->Eventno=_id;
  EN->RawWords=nws<(1<<18)?nws:((1<<18)-1);
  EN->RawWords+=(AMSCommonsI::getosno())<<18;
  EN->RawWords+=(AMSCommonsI::getbuildno())<<22;
  EN->Run=_run;
  EN->RunType=_runtype;
  EN->Time[0]=_time;
  EN->Time[1]=AMSJob::gethead()->isRealData()?_usec:_usec/1000;
   if(!AMSJob::gethead()->isRealData()){
     AMSmceventg *p =(AMSmceventg*)getheadC("AMSmceventg",0);
     if(p){
      EN->RunType=p->getseed(0);
      EN->RNDMSeed[0]=p->getseed(0);
      EN->RNDMSeed[1]=p->getseed(1);
     }
     
   }
  //EN->GrMedPhi=_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic;;
  EN->ThetaS=_StationTheta;
  EN->PhiS=fmod(_StationPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);
  EN->RadS=_StationRad;
  EN->Yaw=_Yaw;
  EN->Pitch=_Pitch;
  EN->Roll=_Roll;
  for(int k=0;k<sizeof(_GPS)/sizeof(_GPS[0]);k++)EN->GPS[k]=0;
  EN->GPSL=_GPSL;
  if(EN->GPSL>=  sizeof(_GPS)/sizeof(_GPS[0]))EN->GPSL=sizeof(_GPS)/sizeof(_GPS[0]);
  for(int k=0;k<EN->GPSL;k++)EN->GPS[k]=_GPS[k];
  
  EN->Alpha=_Alpha;
  EN->B1a=_B1a;
  EN->B3a=_B3a;
  EN->B1b=_B1b;
  EN->B3b=_B3b;
  EN->VelocityS=_StationSpeed;
  EN->VelTheta=_VelTheta;
  EN->VelPhi=fmod(_VelPhi-(_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);
  EN->ISSEqAsc=_StationEqAsc*(360/AMSDBc::twopi);// ISN 
  EN->ISSEqDec=_StationEqDec*(360/AMSDBc::twopi); // ISN 
  EN->ISSGalLat=_StationGalLat*(360/AMSDBc::twopi); //ISN 
  EN->ISSGalLong=_StationGalLong*(360/AMSDBc::twopi);//ISN 
  EN->AMSEqAsc=_AMSEqAsc*(360/AMSDBc::twopi);// ISN 
  EN->AMSEqDec=_AMSEqDec*(360/AMSDBc::twopi); // ISN 
  EN->AMSGalLat=_AMSGalLat*(360/AMSDBc::twopi); //ISN 
  EN->AMSGalLong=_AMSGalLong*(360/AMSDBc::twopi);//ISN     
 /*
 integer  i,nc;
  AMSContainer *p;
//  EN->Particles=0;
  EN->Tracks=0;
//  EN->Betas=0;
//  EN->Charges=0;
  EN->TrRecHits=0;
  EN->TrClusters=0;
  EN->TrRawClusters=0;
  EN->TrMCClusters=0;
  EN->TRDMCClusters=0;
  EN->TRDClusters=0;
  EN->TRDRawHits=0;
  EN->TRDSegments=0;
  EN->TRDTracks=0;
  EN->TOFClusters=0;
  EN->TOFMCClusters=0;
  EN->AntiClusters=0;
  EN->AntiMCClusters=0;
  EN->EcalClusters=0;
  EN->EcalHits=0;
  EN->RICMCClusters=0;//CJM
  EN->RICHits=0;//CJM
*/
  getmag(EN->ThetaM,EN->PhiM);
/*
  for(i=0;;i++){
   p=getC("AMSTrTrack",i);
   if(p) EN->Tracks+=p->getnelem();
   else break;
  }



  for(i=0;;i++){
   p=getC("AMSTrRecHit",i);
   if(p) EN->TrRecHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=getC("AMSTrCluster",i);
   if(p) EN->TrClusters+=p->getnelem();
   else break;
  }
  for(i=0;;i++){
   p=getC("AMSTrRawCluster",i);
   if(p) EN->TrRawClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=getC("AMSTrMCCluster",i);
   if(p) EN->TrMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSTRDMCCluster",i);
   if(p) EN->TRDMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSTRDCluster",i);
   if(p) EN->TRDClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSTRDRawHit",i);
   if(p) EN->TRDRawHits+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSTRDTrack",i);
   if(p) EN->TRDTracks+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=getC("AMSTRDSegment",i);
   if(p) EN->TRDSegments+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSTOFCluster",i);
   if(p) EN->TOFClusters+=p->getnelem();
   else break;
  }
  for(i=0;;i++){
   p=getC("AMSTOFMCCluster",i);
   if(p) EN->TOFMCClusters+=p->getnelem();
   else break;
  }


  for(i=0;;i++){
   p=getC("AMSAntiCluster",i);
   if(p) EN->AntiClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=getC("AMSAntiMCCluster",i);
   if(p) EN->AntiMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("Ecal1DCluster",i);
   if(p) EN->EcalClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=getC("AMSEcalHit",i);
   if(p) EN->EcalHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){//CJM
   p=getC("AMSRichMCHit",i);
   if(p) EN->RICMCClusters+=p->getnelem()-1; //Due to fake hit
   else break;
  }

  for(i=0;;i++){//CJM
   p=getC("AMSRichRawEvent",i);
   if(p) EN->RICHits+=p->getnelem();
   else break;
  }
*/
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->fHeader.Set(EN);
  AMSJob::gethead()->getntuple()->Get_evroot02()->fStatus=getstatus64();
  //cout <<" status nah "<<_id<<" "<<_status[0]<<" "<<_status[1]<<" "<<getstatus64()<<" "<<((_status[1]>>5)&3)<<" "<<bits<<endl;
#endif
  
}
//====================================================================

AMSlink * AMSEvent::addnext(AMSID id, AMSlink *p){
 AMSContainer * ptr= getC(id);
   if(ptr){
     ptr->addnext(p);
     return p;
   }
   else {
    cerr<<"AMSEvent::addnext-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
   }
}

integer AMSEvent::replace(AMSID id, AMSlink *p, AMSlink *prev){
 AMSContainer * ptr= getC(id);
   if(ptr){
     ptr->replaceEl(p,prev);
     return 1;
   }
   else {
    cerr<<"AMSEvent::replace-S-NoContainer "<<id<<endl;
#ifdef __AMSDEBUG__
    exit(1);
#endif
    return 0;
   }
}

void AMSEvent::_validate(int print){
AMSgObj::BookTimer.start("TDV");

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=dynamic_cast<AMSTimeID*>(ptid->down());
while(offspring){
  if(!offspring->Verify()|| offspring->UpdateMe()){
    offspring=(AMSTimeID*)offspring->next();
    continue;
  }
  integer nb=offspring->GetNbytes();
#ifdef __AMSDEBUG__
  //          char * tmp =new char[nb];
  //          assert(tmp !=NULL);
  //          integer ncp=offspring->CopyOut((uinteger*)tmp);
  //          ncp=offspring->CopyIn((uinteger*)tmp);
  //          delete[] tmp;
#endif
  if(offspring->validate(_time)){
#ifdef __AMSDEBUG__
    if(print)cout <<"AMSEvent::_validate-I-"<<offspring->getname()<<" "<<offspring->getid()<<
      " validated. ("<<nb-sizeof(uinteger)<<" bytes ) CRC = "<<
      offspring->getCRC()<<endl;                                              
#endif
   }
    else 
#pragma omp critical (tdvnotvalidated)
{
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated. "<<AMSEvent::get_thread_num()<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Time: "<<ctime(&_time)<<" "<<_time<<endl;
      cerr<<" Begin : " <<ctime(&b)<<" "<<b<<endl; 
      cerr<<" End : " <<ctime(&e)<<" "<<e<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<" "<<i<<endl; 
#ifdef __CORBA__
//cerr<<"  updating map "<<endl;
//offspring->updatemap(AMSDATADIR.amsdatabase,true);
#endif
      throw amsglobalerror("TDV Not    Validated ",3);
 
    }
    offspring=(AMSTimeID*)offspring->next();
  }
     AMSgObj::BookTimer.stop("TDV");

}

void AMSEvent::_validateDB()
//
// compare TDV in memory with ones in DBASE
// read from DBase and update memory tables is necessary
//
{
#ifdef __DB__
AMSgObj::BookTimer.start("TDVdb");
AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
    if (AMSJob::gethead()->isReadSetup()) {
     char *name=offspring -> getname();
     int  id   =offspring -> getid();
     time_t I, B, E;
     time_t i, b, e;
     integer S;
     int rstat = AMSJob::FindTheBestTDV(name, id, _time, S, I, B, E); 
     if (rstat == 1) {
       offspring -> gettime(i,b,e);
       if (i==I && b==B && e==E) {
       } else {
         cout<<"AMSEvent::_validate -I- "<<name
             <<", i,b,e need to be updated "<<endl;
#ifdef __AMSDEBUG__
         cout<<"Insert "<<ctime(&i);
         cout<<"       "<<ctime(&I);
         cout<<"Begin  "<<ctime(&b);
         cout<<"       "<<ctime(&B);
         cout<<"End    "<<ctime(&e);
         cout<<"       "<<ctime(&E)<<endl;
#endif 

        uinteger* buff = new uinteger[S];

#ifdef __AMSDEBUG__
         cout<<"EventTime "<<ctime(&_time)<<endl;
#endif
         int rstatus = lms -> ReadTDV(name, id, I, B, E, buff);
         if (rstatus == oocSuccess) {
           offspring -> CopyIn((uinteger*)buff);
           offspring -> SetTime(I,B,E);
         } 
         delete [] buff;
       }
     } 
    }
    offspring=(AMSTimeID*)offspring->next();
 }
 AMSgObj::BookTimer.stop("TDVdb");
#endif
}


integer AMSEvent::removeC(){
  // find and nullify _Head in all containers in case of memory chock;

AMSNode * cur;
int i,n=0;
for (i=0;;){
  cur=AMSEvent::EventMap.getid(i++);   // get one by one
 if(cur){
   if(strncmp(cur->getname(),"AMSContainer:",13)==0){
   n++;
   ((AMSContainer*)cur)->sethead(0);
   }
 }
 else break;
}
return n;
}

void AMSEvent::Recovery(bool finish){
      cerr <<"AMSEvent::Recovery-I-Event dump follows"<<endl;
      printA(0);
      cerr <<"AMSEvent::Recovery-I-Cleanup started"<<endl;
      UPool.ReleaseLastResort();
      cerr << "AMSEvent::Recovery-I-Last resort released"<<endl;
      //#ifndef __BIGSTACK__
      //      // line below is very unsafe/risky one.
      //      // use __BIGSTACK__ together with "limit stack 20000" 
      //      command to make safe recovery
      //       integer n=removeC();
      //      cerr <<"AMSEvent::Recovery-I- "<<n<<
      //      " containers succesfully nullified"<<endl;
      //#endif
      UPool.Release(0);
      cerr << "AMSEvent::Recovery-I-Release0"<<endl;
      remove();
      cerr << "AMSEvent::Recovery-I-remove"<<endl;
      UPool.Release(1);
      cerr <<"AMSEvent::Recovery-I-Event structure removed"<<endl;
      sethead(0);
      UPool.erase(0);
      cerr <<"AMSEvent::Recovery-I-Memory pool released"<<endl;
      int mresort=160000;
      int ntry=0;
      const int maxtry=10;
      if(AMSJob::gethead()->isSimulation()){
        ntry=maxtry;
         mresort=1;
       } 
      if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
agains:
      try{
       if(!finish)UPool.SetLastResort(mresort);
      }
    catch (std::bad_alloc a){  
      cerr<<"AMSEvent::Recovery-E-UnableToSetLastResortTo "<<mresort<<" try "<<ntry<<endl;
      ntry++;
      if(ntry<maxtry){
      sleep(2);
      goto agains;
      }
      else{
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      AMSFFKEY.CpuLimit=10;
      cerr<<" AMSEvent::Recovery-S-TryingToTerminate "<<endl;
      }
    }
      cerr <<"AMSEvent::Recovery-I-Cleanup done"<<endl;

}


void AMSEvent::_redaqinitevent(){
  add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",0),0));
  add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",4),0));
  add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",6),0));
  add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",27),0));
  add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",896),0));
}

void AMSEvent::_redaqevent(){
  //  Add Dummy for Copying ....
  addnext(AMSID("Dummy",0),new Test());

  // Erase some containers if in debug mode
#ifdef __AMSDEBUG__
  int i;
  for(i=0;;i++){
    AMSContainer *pctr=getC("AMSTrRawCluster",i);
      if(pctr)pctr->eraseC();
      else break ;
  }
  for(i=0;;i++){
    AMSContainer *pctr=getC("AMSTRDRawHit",i);
      if(pctr)pctr->eraseC();
      else break ;
  }
#endif


  AMSgObj::BookTimer.start("REDAQ");

   DAQEvent * pdaq = (DAQEvent*)
   getheadC("DAQEvent",0);
   if(pdaq)pdaq->buildRawStructures();
   pdaq = (DAQEvent*)
   getheadC("DAQEvent",6);
   if(pdaq)pdaq->buildRawStructures();
   pdaq = (DAQEvent*)
   getheadC("DAQEvent",27);
   if(pdaq)pdaq->buildRawStructures();
   pdaq = (DAQEvent*)
   getheadC("DAQEvent",896);
   if(pdaq)pdaq->buildRawStructures();
  AMSgObj::BookTimer.stop("REDAQ");
}


void AMSEvent::_sidaqevent(){
  AMSgObj::BookTimer.start("SIDAQ");

DAQEvent *  pdaq = new DAQEvent();
addnext(AMSID("DAQEvent",0), pdaq);      
pdaq->buildDAQ();

// H/K simulation 

//pdaq=new DAQEvent();
//addnext(AMSID("DAQEvent",4), pdaq);      
//pdaq->buildDAQ(4);


  AMSgObj::BookTimer.stop("SIDAQ");
}







void AMSEvent::builddaq(integer i, integer length, int16u *p){

*p=getdaqid() | (1<<15);
*(p+4)=int16u(_Head[get_thread_num()]->_run&65535);
*(p+3)=int16u((_Head[get_thread_num()]->_run>>16)&65535);
*(p+2)=int16u(_Head[get_thread_num()]->_runtype&65535);
*(p+1)=int16u(_Head[get_thread_num()]->_runtype>>16&65535);
uinteger _event=uinteger(_Head[get_thread_num()]->_id);
*(p+6)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+8)=int16u(_Head[get_thread_num()]->_time&65535);
*(p+7)=int16u((_Head[get_thread_num()]->_time>>16)&65535);
*(p+10)=int16u(_Head[get_thread_num()]->_usec&65535);
*(p+9)=int16u((_Head[get_thread_num()]->_usec>>16)&65535);
*(p+11)=0;
}





void AMSEvent::builddaqSh(integer i, integer length, int16u *p){

p--;
uinteger tmp;
memcpy(&tmp,&_Head[get_thread_num()]->_StationRad,sizeof(tmp));
*(p+2)=int16u(tmp&65535);
*(p+1)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head[get_thread_num()]->_StationTheta,sizeof(tmp));
*(p+4)=int16u(tmp&65535);
*(p+3)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head[get_thread_num()]->_StationPhi,sizeof(tmp));
*(p+6)=int16u(tmp&65535);
*(p+5)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head[get_thread_num()]->_Yaw,sizeof(tmp));
*(p+8)=int16u(tmp&65535);
*(p+7)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head[get_thread_num()]->_Pitch,sizeof(tmp));
*(p+10)=int16u(tmp&65535);
*(p+9)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head[get_thread_num()]->_Roll,sizeof(tmp));
*(p+12)=int16u(tmp&65535);
*(p+11)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head[get_thread_num()]->_StationSpeed,sizeof(tmp));
*(p+14)=int16u(tmp&65535);
*(p+13)=int16u((tmp>>16)&65535);
memcpy(&tmp,&_Head[get_thread_num()]->_VelTheta,sizeof(tmp));
*(p+16)=int16u(tmp&65535);
*(p+15)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head[get_thread_num()]->_VelPhi,sizeof(tmp));
*(p+18)=int16u(tmp&65535);
*(p+17)=int16u((tmp>>16)&65535);

memcpy(&tmp,&_Head[get_thread_num()]->_NorthPolePhi,sizeof(tmp));
*(p+20)=int16u(tmp&65535);
*(p+19)=int16u((tmp>>16)&65535);
*(p+21)=getdaqidSh() ;
}



void AMSEvent::buildraw(
              integer length, int16u *p, uinteger & run, uinteger &id,
              uinteger &runtype, time_t & time, uinteger &usec){
  //  run=(*(p+1)) |  (*(p+2))<<16;
  //  runtype=(*(p+3)) |  (*(p+4))<<16;
  //  id=(*(p+5)) |  (*(p+6))<<16;
  //  time=(*(p+7)) |  (*(p+8))<<16;
  //  usec=(*(p+9)) |  (*(p+10))<<16;

  //!   Swap
//    run=(*(p+2)) |  (*(p+1))<<16;
//    runtype=(*(p+4)) |  (*(p+3))<<16;
    runtype=(*(p+2)) |  (*(p+1))<<16;
    run=(*(p+4)) |  (*(p+3))<<16;
    id=(*(p+6)) |  (*(p+5))<<16;
    time=(*(p+8)) |  (*(p+7))<<16;
    usec=(*(p+10)) |  (*(p+9))<<16;
    const uinteger _OffsetT=0x12d53d80;
    if(run<1000000000 && AMSJob::gethead()->isRealData())time+=_OffsetT;
}



void AMSEvent::buildraw2009(
              integer type, int16u *p, uinteger & run, uinteger &id,
              uinteger &runtype, time_t & time, uinteger &usec, int16u lvl3[], unsigned int _GPS[], unsigned int &_GPSL){


    id=(*(p+6)) |  (*(p+5))<<16;
    usec=*(p+1)*1000;
    runtype=*(p+2);
    run=(*(p+4)) |  (*(p+3))<<16;
    id=(*(p+6)) |  (*(p+5))<<16;
    const uinteger _OffsetT=0x12d53d80;
    if(run<1000000000 && AMSJob::gethead()->isRealData()){
        run+=_OffsetT;
    }
    
   if(time<1000000000 && AMSJob::gethead()->isRealData()){
        time+=_OffsetT;
    }
    for(int k=0;k<sizeof(_GPS)/sizeof(_GPS[0]);k++)_GPS[k]=0;
    _GPSL=0;
    // lvl3
    for(int k=0;k<5;k++)lvl3[k]=0;
    int nbytes=0;
    if(type==0x11 || type==0x13){
     nbytes=( (*(p+7)>>8)&31);
    }
       static int print=0;
    if(nbytes>0){
     for(int k=0;k<2;k++)lvl3[k]=type==0x11?*(p+7+k):0;
    }
    if(nbytes==5){
       if(print++<10)cerr<<" AMSEvent::buildraw2009-E-Brokenlvl3formatsuspected "<<run<<endl;
       nbytes=3;
    }
    else if (nbytes==9){
     for(int k=0;k<3;k++)lvl3[k+2]=*(p+7+2+k);
    }
    else{
       if(print++<10)cerr<<" AMSEvent::buildraw2009-E-unknownlvl3format datected "<<run<<" "<<nbytes<<endl;
    }
    if(type== 0x12 || type== 0x13){
     if(nbytes>0 && nbytes%2){
      nbytes++;
      int nbytesgps=( (*(p+7+nbytes/2)>>8)&31);
      if(nbytesgps%2){
         nbytesgps++;
         if(nbytesgps%4){
       if(print++<10)cerr<<" AMSEvent::buildraw2009-E-gpsnbytes++ error "<<run<<" "<<nbytes<<" "<<nbytesgps<<endl;
         }
         else{
          _GPSL=0;
          for(int k=0;k<nbytesgps/2;k+=2){
           unsigned int a= *(p+7+nbytes/2+k)<<16;
           a+=*(p+nbytes/2+7+k+1);
            _GPS[_GPSL++]=a; 
          }
        }
       }
      else{
       if(print++<10)cerr<<" AMSEvent::buildraw2009-E-gpsnbytes error "<<run<<" "<<nbytes<<" "<<nbytesgps<<endl;
      }
     }
    }

}
    


void AMSEvent::buildrawSh(integer length, int16u *p){
    p--;
    uinteger tmp=(*(p+2)) |  (*(p+1))<<16;
    memcpy(&_Head[get_thread_num()]->_StationRad,&tmp,sizeof(tmp));
     tmp=(*(p+4)) |  (*(p+3))<<16;
    memcpy(&_Head[get_thread_num()]->_StationTheta,&tmp,sizeof(tmp));
     tmp=(*(p+6)) |  (*(p+5))<<16;
    memcpy(&_Head[get_thread_num()]->_StationPhi,&tmp,sizeof(tmp));
     tmp=(*(p+8)) |  (*(p+7))<<16;
    memcpy(&_Head[get_thread_num()]->_Yaw,&tmp,sizeof(tmp));
     tmp=(*(p+10)) |  (*(p+9))<<16;
    memcpy(&_Head[get_thread_num()]->_Pitch,&tmp,sizeof(tmp));
     tmp=(*(p+12)) |  (*(p+11))<<16;
    memcpy(&_Head[get_thread_num()]->_Roll,&tmp,sizeof(tmp));
     tmp=(*(p+14)) |  (*(p+13))<<16;
    memcpy(&_Head[get_thread_num()]->_StationSpeed,&tmp,sizeof(tmp));
     tmp=(*(p+16)) |  (*(p+15))<<16;
    memcpy(&_Head[get_thread_num()]->_VelTheta,&tmp,sizeof(tmp));
     tmp=(*(p+18)) |  (*(p+17))<<16;
    memcpy(&_Head[get_thread_num()]->_VelPhi,&tmp,sizeof(tmp));
     tmp=(*(p+20)) |  (*(p+19))<<16;
    memcpy(&_Head[get_thread_num()]->_NorthPolePhi,&tmp,sizeof(tmp));


}


integer AMSEvent::checkdaqid(int16u id){
int16u type=id&31;
int16u node=(id>>5)&1023;
if((type==5 || type==7) && node<16)return node+1;
else return 0;

}


integer AMSEvent::checkdaqid2009(int16u id){

int16u type=id&31;
int16u node=(id>>5)&1023;
if(type>=16 && type <=19 && node<16){

   return type;
}
else return 0;
}



integer AMSEvent::checkdaqidSh(int16u id){
 if((id&31)==getdaqidSh())return 1;
 else return 0;
}

integer AMSEvent::calcTrackerHKl(integer i){
#ifdef _PGTRACK_
  // FIXME Tracker House keeping
  return 0;
#else
static integer init =0;
if(!TRMCFFKEY.WriteHK || abs(++init-TRMCFFKEY.WriteHK-1) >1)return 0;
return 1+2+2+2+4;
#endif
}

integer AMSEvent::_checkUpdate(){

integer True  = 1;
integer False = 0;

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
  if(offspring->UpdateMe()) return True;
  offspring=(AMSTimeID*)offspring->next();
}
return False;
}

void AMSEvent::SetCCEBPar(){
//create default cceb record;
  for(int i=0;i<sizeof(ArrayC)/sizeof(ArrayC[0]);i++){
   for(int j=0;j<4;j++){
    ArrayC[i].B[0][j]=j==0?-1:0;
    ArrayC[i].B[1][j]=0;
    ArrayC[i].B[2][j]=0;
    ArrayC[i].T[j]=-273;
    ArrayC[i].Time=mktime(&AMSmceventg::Orbit.Begin);
     
   }  
 }

} 

void AMSEvent::SetShuttlePar(){
  for(int i=0;i<sizeof(Array)/sizeof(Array[0]);i++){
    Array[i].StationR=AMSmceventg::Orbit.AlphaAltitude;
    Array[i].StationTheta=AMSmceventg::Orbit.ThetaI;  
    Array[i].StationPhi=AMSmceventg::Orbit.PhiI;
    Array[i].GrMedPhi=AMSmceventg::Orbit.PolePhi-AMSmceventg::Orbit.PolePhiStatic;
    Array[i].StationYaw=0;
    Array[i].StationPitch=0;
    Array[i].StationRoll=0;
    Array[i].StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    Array[i].SunR=0;
    Array[i].VelTheta=0;
    Array[i].VelPhi=0;
    Array[i].Time=mktime(&AMSmceventg::Orbit.Begin);
  }
}

AMSEvent::ShuttlePar AMSEvent::Array[60];
AMSEvent::BeamPar AMSEvent::ArrayB[60];
AMSEvent::CCEBPar AMSEvent::ArrayC[64];
AMSEvent::TofSTemp AMSEvent::UTofTemp[32];
AMSEvent::TofSTemp AMSEvent::LTofTemp[32];


AMSID AMSEvent::getTDVStatus(){
  return AMSID(AMSJob::gethead()->getstatustable()->getname(),AMSJob::gethead()->isRealData());
}


void AMSEvent::_collectstatus(){



//  if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
//      uinteger __status=0;
//
//  {
//    TriggerLVL3 *ptr=dynamic_cast<TriggerLVL3*>(getheadC("TriggerLVL3",0));
//    if(ptr){
//      integer lvl3o=ptr->TrackerTrigger();
//      integer prescale = lvl3o>=32?1:0;
//      lvl3o=lvl3o%32;
//      __status=__status | lvl3o | (prescale<<4);
      
//    }
//   }
//  {
//    AMSParticle *ptr=(AMSParticle*)getheadC("AMSParticle",0);
//    AMSParticle *ptr1=(AMSParticle*)getheadC("AMSParticle",1);
//    int npart=0;
//    AMSContainer *ptrc;
//    if(ptr){
//      ptrc=getC("AMSParticle",0);
//    }
//    else if(ptr1){
//      ptr=ptr1;     
//      ptrc=getC("AMSParticle",1);
//    }
//    if(ptr){
//      npart=ptrc->getnelem();
//      if(npart>3)npart=3;
//      __status= __status | (npart<<21);
//      integer charge=ptr->getcharge()-1;
//      if(charge>7)charge=7;
//      __status=__status | (charge<<5);
//      number pmom=ptr->getmomentum();
//      integer sign=pmom<0?0:1;
//      __status=__status | (sign<<8);
//      sign=(ptr->getpbeta())->getbeta()<0?0:1;
//      __status=__status | (sign<<9);
//      integer pat=(ptr->getptrack())->getpattern();
//      if(pat>31)pat=31;
//      if(pat<0)pat=31;
//      __status=__status | (pat<<10);
//       pat=(ptr->getpbeta())->getpattern();
//      uinteger spat=TKDBc::nlay()-TKDBc::patpoints((ptr->getptrack())->getpattern());
//      if(spat>3)spat=3;
//      __status=__status | (spat<<15);       
//      number rig=fabs(pmom)/(charge+1);
//      uinteger srig;
//      if(rig<2)srig=0;
//      else if(rig<8)srig=1;
//      else if(rig<20)srig=2;
//      else srig=3;
//      __status=__status | (srig<<23);
//      uinteger trquality;
//      if((ptr->getptrack())->checkstatus(AMSDBc::FalseTOFX))trquality=3;
//      else if( (ptr->getptrack())->checkstatus(AMSDBc::FalseX))trquality=2;
//      else if((ptr->getptrack())->checkstatus(AMSDBc::WEAK) )trquality=1;
//      else trquality=0;   
//      __status=__status | (trquality<<25);
//       uinteger localdb=0;
//       if((ptr->getptrack())->checkstatus(AMSDBc::LocalDB))localdb=1;
//      __status=__status | (localdb<<29);
//   }
//  }
//  {
//      AMSContainer *ptr=getC("AMSAntiCluster",0);
//      integer ncl=ptr->getnelem();
//      if(ncl>3)ncl=3;
//      __status=__status | (ncl<<19);
//  }
     
  // Now collect geomag latitude 
//     geant thetam,phim;
//     getmag(thetam,phim);
//     number cosgm=fabs(sin(thetam));
//     integer icos;
//     if(cosgm<0.1736)icos=0;
//     else if(cosgm<0.5)icos=1;
//     else if(cosgm<0.766)icos=2;
//     else icos=3;
//      __status=__status | (icos<<27);
  // Now Set Event Error
//     if(_Error==1){
//      __status=__status | (1<<30);
//     }
//      _status[0]=__status;
//      _status[1]=0;
//}
//else{
 uinteger __status=0;
 uinteger __status1=0;
  // Now Set Event Error
     if(_Error==1){
      __status=__status | (1<<30);
     }   

//   0-1    number of particles
//   2-7    presence of (TRD,ToF,Tracker,RICH,Vertex in particle)
//   8-9    number of TRD Tr
//   10-12   -----    TOF Clusters
//     13-14   --------  Tracker Tr
//     15-16   --------  Rich Rings
//     17-18   --------- EcalShowaers
//     19-20      --------- Vertex
//     21-22   -------   Anti
//     23-25   --------- Charge Mag
//     26-27   -------   lvl1 z=1 xof 4
//     28-29   --------  lvl1 z>1 x of 4
//     30      --------  error

    AMSParticle *ptr=(AMSParticle*)getheadC("AMSParticle",0);
    AMSParticle *ptr1=(AMSParticle*)getheadC("AMSParticle",1);
    int npart=0;
    AMSContainer *ptrc;
    if(ptr){
      ptrc=getC("AMSParticle",0);
    }
    else if(ptr1){
      ptr=ptr1;     
      ptrc=getC("AMSParticle",1);
    }
    if(ptr){
      npart=ptrc->getnelem();
      if(npart>3)npart=3;
      __status= __status | (npart);
       if(ptr->_ptrd)__status|=(1<<2);
       if(ptr->_pbeta)__status|=(1<<3);
       if(ptr->_ptrack && getheadC("AMSParticle",0))__status|=(1<<4);
       if(ptr->_prich)__status|=(1<<5);
       if(ptr->_pShower)__status|=(1<<6);
       if(ptr->_pvert)__status|=(1<<7);
    }
       ptrc=getC("AMSTRDTrack",0);
       int ntrd=ptrc?ptrc->getnelem():0;
       if(ntrd>3)ntrd=3;
       __status|=(ntrd<<8);
       int ntof =0;
       for(int i=0;i<4;i++)ntof+=getC("AMSTOFCluster",i)->getnelem();
       if(ntof>7)ntof=7;
       __status|=(ntof<<10);
       int ntr=getC("AMSTrTrack",0)->getnelem();
       if(ntr>3)ntr=3;   
       __status|=(ntr<<13);

        int nrich=getC("AMSRichRing",0)->getnelem();
        if(nrich>3)nrich=3;
       __status|=(nrich<<15);
        int necal=getC("EcalShower",0)->getnelem();
        if(necal>3)necal=3;
       __status|=(necal<<17);

        int nvtx=getC("AMSVtx",0)->getnelem();
        if(nvtx>3)nvtx=3;
       __status|=(nvtx<<19);
        int nanti=getC("AMSAntiCluster",0)->getnelem();
        if(nanti>3)nanti=3;
       __status|=(nanti<<21);
       if(ptr){
        integer charge=ptr->getcharge();
        if(charge>7)charge=7;
       __status|=(charge<<23);
       }
        Trigger2LVL1 *ptrt= dynamic_cast<Trigger2LVL1*>(getheadC("TriggerLVL1",0));         
             if(ptrt){
               int z1=0;
               if(ptrt->gettoflag1()>=0){
               if(ptrt->gettoflag1()==0)z1=3;
               else if(ptrt->gettoflag1()<5)z1=2;
               else if(ptrt->gettoflag1()<11)z1=1;
               }
               __status|=(z1<<26);
               int z2=0;
                if(ptrt->gettoflag1()>=0){
               if(ptrt->gettoflag2()==0)z2=3;
               else if(ptrt->gettoflag2()<5)z2=2;
               else if(ptrt->gettoflag2()<11)z2=1;
               }
               __status|=(z2<<28);


             }



//        TriggerLVL302 *ptr3= dynamic_cast<TriggerLVL302*>(getheadC("TriggerLVL3",0));         
//        if(ptr3){
//         int z1=ptr3->Prescaled()?1:0;
//         __status1|=z1;
//        }
          if(ptrt){
           int z1=(ptrt->JMembPattBitSet(0) ||ptrt->JMembPattBitSet(11) )?1:0;
           __status1|=z1;
         }              
  

        
        if(ptr){
         if(ptr->getpbeta()){
          int z1=ptr->getpbeta()->getbeta()>0?1:0;
         __status1|=(z1<<1);
        }          
        int z1=ptr->getmomentum()>0?1:0;
         __status1|=(z1<<2);
        
       }
       
       if(ptr ){
        integer charge=ptr->getcharge();
	geant rig;
	if (charge==0)
	  rig=fabs(ptr->getmomentum());
	 else 
	   rig=fabs(ptr->getmomentum())/fabs(charge);
        int z1=0;
        if(rig<8)z1=0;        
        else if(rig<32)z1=1;        
        else if(rig<128)z1=2;        
        else z1=3;
         __status1|=(z1<<3);
       }
          

       if(ptr){
        int z1=0;
        if(ptr->getpshower()){
        number en=ptr->getpshower()->getEnergy()/1000.;

        if(!isnan(en)&&!isinf(en)){
	  if(en<2)z1=0;        
	  else if(en<8)z1=1;        
	  else if(en<32)z1=2;        
	  else z1=3;        
	}
         //cout <<" en "<<en<<" "<<z1<<endl;
        }
         __status1|=(z1<<5);
       }
if(strstr(AMSJob::gethead()->getsetup(),"AMS02P")){
         int iz1=0;
       if( ptr &&ptr->_ptrack ){
         iz1=ptr->_ptrack->HasExtLayers();
        if(iz1>3){
         cerr<<"AMSEvent::_collectstatus::-S-TooManyTrackerExtLayersFound "<<iz1<<endl;
        iz1=0;
        }
       }
         __status1|=(iz1<<7);
         


}
else{          
       if(_ccebp){
         if(MAGSFFKEY.magstat>0){
          geant b=_ccebp->getBAv();
          int iz1=0;
          if(b<6)iz1=1;
          else if(b<9.5)iz1=2;
          else iz1=3;
         __status1|=(iz1<<7);
               
         }   
       }
}

       if(ptrt){
         geant rate=ptrt->gettrrates(5);
         int z1=0;
         if(rate<500)z1=0;
         else if(rate<1000)z1=1;
         else if(rate<2000)z1=2;
         else z1=3; 
         __status1|=(z1<<9);
       }


       if(ptr){
        int z1=0;
        if(ptr->getptrd()){
         int nh=0;
         for(int k=0;k<ptr->getptrd()->_Base._NHits;k++){
          if(ptr->getptrd()->_Base._PCluster[k]->gethmult()>0)nh++;
         }
         if(nh==0)z1=0;
         else if(nh<2)z1=1;
         else if(nh<4)z1=2;
         else z1=3;
        }
         __status1|=(z1<<11);
       }


//
//
//   geomag here
//
  // Now collect geomag latitude 
     geant thetam,phim;
     getmag(thetam,phim);
     number cosgm=fabs(sin(thetam));
     integer icos;
     if(cosgm<0.1736)icos=0;
     else if(cosgm<0.5)icos=1;
     else if(cosgm<0.766)icos=2;
     else icos=3;
      __status1|=(icos<<13);




       if(ptr){
        if(ptr->_prich){
          float charge=ptr->_prich->getcollnpe()/(ptr->_prich->getnpexp()+1.e-10);
          charge=sqrt(fabs(charge));
          int z1=(charge+1./8)*4;
          if(z1>31)z1=31;
         __status1|=(z1<<15);
        }       
        }

        if(ptr){
         int z1=0;
         const float cc=11.3*3./2;
         if(fabs(ptr->_RichCoo[0][0])<cc && fabs(ptr->_RichCoo[0][1])<cc)z1=1;
         __status1|=(z1<<20);
        }


       if(ptr){
         int z1=0;
         if(ptr->getpbeta()){
           if(ptr->getpbeta()->getpattern()==0)z1=3;
           else if(ptr->getpbeta()->getpattern()<4)z1=2;
            else z1=1;
         }
         __status1|=(z1<<21);
       }
      if(ptr){ 
               integer charge=ptr->getcharge();
               float mass=ptr->getmass();
               float moc=mass;
               if(charge==0)moc=mass;
               else moc=mass/charge;
               moc=fabs(moc);
               int z1=0;
               if(moc<0.5)z1=0;
               else if(moc<1.5)z1=1;
               else if(moc<3.0)z1=2;
               else z1=3;
              __status1|=(z1<<23);

      }


    if(ptr){
              int z1=0;
              if(ptr->getptrack()){
               int nht=ptr->getptrack()->getnhits(); 
               if(nht<4)z1=0;
               else if(nht<5)z1=1;
               else if(nht<6)z1=2;
               else if(nht<7)z1=3;
               else if(nht<8)z1=4;
               else if(nht<9)z1=5;
               else z1=6; 
              } 
             __status1|=(z1<<25);
      }

       if(ptr){
        if(ptr->_ptrd){
          float charge=ptr->_ptrd->_Charge.Q;
          int z1=(charge+1./4.)*2;
          if(z1>15)z1=15;
         __status1|=(z1<<28);
        }       
        }



       _status[0]=__status;
       _status[1]=__status1;

//}
}



integer AMSEvent::IsTest(){
 
 if(MISCFFKEY.BeamTest || AMSmceventg::fixeddir() || AMSmceventg::fixedmom() )return 1;
 else return 0;   
}

AMSEvent::EventId * AMSEvent::_SelectedEvents=0;

void AMSEvent::setfile(const char file[]){
  if(strlen(file)){
    ifstream ifile;
    ifile.open(file);
    if(ifile){
     // get total number of lines;
     int nline=0;
     EventId o;
     while(1){
      ifile >>o.Run >>o.Event;
      ifile.ignore(INT_MAX,'\n');        
      if(ifile.eof())break;
      if(o.Run )nline++;
     }    
     if(nline>0){ 
       int nar=nline+1;
       _SelectedEvents=new EventId[nar];
       if(_SelectedEvents){
         cout << " AMSEvent::setfile-I-SelectedEventsFound "<<nline<<endl;
         ifile.clear();
         ifile.close();
         ifile.open(file);
         nline=0;
         if(ifile){ 
          while(1){
                ifile >>o.Run >> o.Event;
                ifile.ignore(INT_MAX,'\n');        
                if(ifile.eof())break;
                if(o.Run )_SelectedEvents[nline++]=o;
          }
          if(nar!=nline+1){
           cerr<<"AMSEvent::setfile-F-LogicError "<<nar<< " "<<nline+1<<endl;
           exit(1);
          }
          // order
          AMSsortNAGa(_SelectedEvents,nline);
          _SelectedEvents[nline].Run=0;
          _SelectedEvents[nline].Event=0;
          SELECTFFKEY.Run=_SelectedEvents[0].Run;
          SELECTFFKEY.Event=-1;
          // check for same events;
          int nlineo=nline;
          for( int nev=nline-1;nev>0;nev--){
            if(_SelectedEvents[nev].Run==_SelectedEvents[nev-1].Run &&
               _SelectedEvents[nev].Event==_SelectedEvents[nev-1].Event){
              // squeeze
              cerr<< " AMSEvent::setfile-W-DuplicatedEventFound "<<_SelectedEvents[nev-1].Run<<" "<<_SelectedEvents[nev-1].Event<<endl;
             for(int isq=nev;isq<nline;isq++){
               _SelectedEvents[isq]=_SelectedEvents[isq+1];    
             }
             nline--;
            }
          }
          if(nlineo!=nline){
            cout <<"AMSEvent::setfile-I-"<<nline<<" events will be processed"<<endl;
          }
         }
       }
       else cerr<<" AMSEvent::setfile-E-UnableToAllMemory for "<<nline<< 
       "events"<<endl;
     }
    }
    else{
      cerr<<"AMSEvent::setfile-E-UnableToOpenFile" <<file<<endl;
    }
  }
}

// extern "C" void geocoor_(const number & gt, const number &gp, const number &gr, geant &mt, geant &mp, geant &mr);

void AMSEvent::getmag(float & thetam, float & phim){
  number DipoleR      =AMSmceventg::Orbit.DipoleR;
  number DipoleTheta  =AMSmceventg::Orbit.DipoleTheta;
  number DipolePhi    =AMSmceventg::Orbit.DipolePhi;
  number PoleTheta    =AMSmceventg::Orbit.PoleTheta;
  number PolePhiStatic=AMSmceventg::Orbit.PolePhiStatic;

//  geant drad;
//  geocoor_(_StationTheta,_StationPhi,_StationRad/100000.,thetam,phim,drad);

  number zero=0, one=1;
  number StationTheta=_StationTheta;
  number StationPhi=_StationPhi-(_NorthPolePhi-PolePhiStatic);
  number StationRad=_StationRad;

// Station GTOD coordinates referred to dipole center
  AMSPoint StationGOTD(cos(StationTheta)*cos(StationPhi),
                       cos(StationTheta)*sin(StationPhi),sin(StationTheta));
  AMSPoint DipoleGTOD(cos(DipoleTheta)*cos(DipolePhi),
                      cos(DipoleTheta)*sin(DipolePhi),sin(DipoleTheta));
  AMSDir StationRedDirGTOD=AMSDir(StationGOTD*StationRad-DipoleGTOD*DipoleR);

// Dipole direction
  AMSDir DipoleDirGTOD=AMSDir(cos(PoleTheta)*cos(PolePhiStatic),
                              cos(PoleTheta)*sin(PolePhiStatic),sin(PoleTheta));

// For sake of clearness GEOM z axis is reversed, i.e. z = -DipoleDirGTOD, y= z x S
   AMSDir GTODs=AMSDir(zero,zero,-one);

//   AMSDir GEOMz=DipoleDirGTOD;       // conventional GEOM z coordinate definition
   AMSDir GEOMz=DipoleDirGTOD*-one;  // unconventional GEOM z coordinate definition
   AMSDir GEOMy=GEOMz.cross(GTODs);
   AMSDir GEOMx=GEOMy.cross(GEOMz);

   AMSDir StationGEOM(GEOMx.prod(StationRedDirGTOD),GEOMy.prod(StationRedDirGTOD),GEOMz.prod(StationRedDirGTOD));

   thetam=(float)(AMSDBc::pi/2-StationGEOM.gettheta());
   phim=(float)StationGEOM.getphi();

}

integer AMSEvent::getselrun(integer i){
 if(_SelectedEvents)return _SelectedEvents[i].Run;
 else return -1;
}

uint64 AMSEvent::getrunev()const {
    uint64 runev=_run; 
   return ((getid()) | runev<<32);
}

uinteger AMSEvent::getmid()const{
#ifdef _OPENMP
uinteger evt=0;
for(int i=0;i<get_num_threads();i++){
 if(_Head[i] && _Head[i]->getid()>evt)evt=_Head[i]->getid();
}
return evt;
#else
return getid();
#endif
}


uinteger AMSEvent::getmiid(){
#ifdef _OPENMP
uinteger evt=INT_MAX;
for(int i=0;i<get_num_threads();i++){
 if(_Head[i] && _Head[i]->getid()<evt && _Head[i]->getid()>0)evt=_Head[i]->getid();
}
return evt;
#else
return getid();
#endif
}




time_t AMSEvent::getmtime()const{
#ifdef _OPENMP
time_t evt=0;
for(int i=0;i<get_num_threads();i++){
 if(_Head[i] && _Head[i]->gettime()>evt)evt=_Head[i]->gettime();
}
return evt;
#else
return gettime();
#endif
}


time_t AMSEvent::getmitime()const{
#ifdef _OPENMP
time_t evt=INT_MAX;
for(int i=0;i<get_num_threads();i++){
 if(_Head[i] && _Head[i]->gettime()<=evt && _Head[i]->getid()>0)evt=_Head[i]->gettime();
}
return evt;
#else
return gettime();
#endif
}


geant AMSEvent::CCEBPar::getBAv()const{
geant av=0;
for(int i=0;i<4;i++){
av+=i<2?B[0][i]:-B[0][i];
}
return av/4;
}

geant AMSEvent::CCEBPar::getBCorr()const{
if(BOK()){
      geant corr=getBAv()/5.47/MAGSFFKEY.fscale; // Only valid for 230A map file to be f
      return corr;     
}
else return -3;
}

geant AMSEvent::CCEBPar::getTAv()const{
geant av=0;
for(int i=0;i<4;i++){
av+=T[i];
}
return av/4;
}

geant AMSEvent::CCEBPar::getBSi()const{
number av=0;
number av2=0;
for(int i=0;i<4;i++){
av+=i<2?B[0][i]:-B[0][i];
av2+=B[0][i]*B[0][i];
}
av/=4;
av2/=4;
av2-=av*av;
if(av2<0)av2=0;
return sqrt(av2);
}



geant AMSEvent::CCEBPar::getTSi()const{
number av=0;
number av2=0;
for(int i=0;i<4;i++){
av+=T[i];
av2+=T[i]*T[i];
}
av/=4;
av2/=4;
av2-=av*av;
if(av2<0)av2=0;
return sqrt(av2);
}


bool AMSEvent::CCEBPar::BOK()const{
bool ok= getBAv()>=0 && getBSi()<0.1*getBAv();
static int berr=0;
if(!ok && berr++<100){
 cerr<<" AMSEvent::CCEBPar::BOK-E-BSensorsProblem "<<getBAv()<<" "<<getBSi();
 for(int i=0;i<4;i++)cerr<<" "<<B[0][i];
 cerr<<endl;
}
return ok;
}


bool AMSEvent::CCEBPar::TOK()const{
bool ok= getTAv()>-273 && getTSi()<0.1*fabs(getTAv());
return ok;
}
void AMSEvent::buildcceb(integer n, int16u* p){
    static int rec=0;
    int len=n&65535;
    if(len!=72){
//     cerr<<"AMSEvent::buildcceb-E-WromngLength "<<len<<endl;;
     return ;
    }



struct _CCBT {
  char *nam;
  float A;
  float B;
};
static struct _CCBT CCBT_FM[32][2] = {                 // ===>>> CALIBRATED UR AS 20090707 <<<===
    {{"PT01", 0.2022/3.85,  0.7/3.85}, {"PT01", 0.2025/3.85,  5.4/3.85}},  //  0
    {{"PT02", 0.2030/3.85, -0.4/3.85}, {"PT02", 0.2028/3.85, -1.6/3.85}},  //  1
    {{"PT03", 0.2031/3.85,  0.0/3.85}, {"PT03", 0.2034/3.85,  6.0/3.85}},  //  2
    {{"PT04", 0.2032/3.85, -3.1/3.85}, {"PT04", 0.2033/3.85, -0.2/3.85}},  //  3
    {{"PT05", 0.2041/3.85,  8.4/3.85}, {"PT05", 0.2030/3.85, -0.8/3.85}},  //  4
    {{"PT06", 0.2029/3.85,  1.9/3.85}, {"PT06", 0.2032/3.85,  1.7/3.85}},  //  5
    {{"PT07", 0.2030/3.85,  2.2/3.85}, {"PT07", 0.2029/3.85,  0.4/3.85}},  //  6
    {{"PT08", 0.2028/3.85,  9.9/3.85}, {"PT08", 0.2036/3.85,  1.7/3.85}},  //  7
    {{"PT09", 0.2039/3.85,  6.5/3.85}, {"PT09", 0.2033/3.85,  7.8/3.85}},  //  8
    {{"PT10", 0.2037/3.85,  3.5/3.85}, {"PT10", 0.2033/3.85,  1.6/3.85}},  //  9
    {{"PT11", 0.2035/3.85,  2.4/3.85}, {"PT11", 0.2028/3.85, -0.3/3.85}},  // 10
    {{"PT12", 0.2024/3.85, -1.2/3.85}, {"PT12", 0.2033/3.85,  2.2/3.85}},  // 11

    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 12
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 13
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 14
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 15

    // from  BH703 datasheet  AS 20091126 (unit kG; X,Y,Z in AMS coordinates)
    // B_0:Sensor#747 / B_1:Sensor#751 / B_2:Sensor#752 / B_3:Sensor#750
    {{"BX_0",  1/25.50/7.937,  0.021/7.937}, {"BX_2",  1/25.50/7.950, -0.012/7.950}},  // 16
    {{"BZ_0",  1/112.0/7.942, -0.045/7.942}, {"BZ_2",  1/112.0/7.947, -0.008/7.947}},  // 17
    {{"BY_0", -1/112.0/7.947, +0.028/7.947}, {"BY_2", -1/112.0/7.976, -0.005/7.976}},  // 18
    {{"Tmp0", 0.2030/3.85,  1.6/3.85}, {"Tmp2", 0.2023/3.85,  0.2/3.85}},  // 19

    {{"BX_1",  1/25.50/7.511,  0.006/7.511}, {"BX_3",  1/25.50/7.822,  0.014/7.822}},  // 20
    {{"BZ_1",  1/112.0/7.527, -0.011/7.527}, {"BZ_3",  1/112.0/7.816,  0.009/7.816}},  // 21
    {{"BY_1", -1/112.0/7.529, -0.037/7.529}, {"BY_3", -1/112.0/7.823, -0.008/7.823}},  // 22
    {{"Tmp1", 0.2014/3.85,  0.1/3.85}, {"Tmp3", 0.2028/3.85,  3.9/3.85}},  // 23

    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 24
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 25
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 26
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 27
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 28
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 29
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}},  // 30
    {{"GND ", 0.1,    0.0}, {"GND ", 0.1,    0.0}}}; // 31


                float bt[4][4];
                short ibt[4][4];
                for(int k=0;k<4;k++){
                 ibt[0][k]=(short)(*(p+20+k)&4095)-2048;
                 ibt[1][k]=(short)(*(p+24+k)&4095)-2048;
                 ibt[2][k]=(short)(*(p+20+k+34)&4095)-2048;
                 ibt[3][k]=(short)(*(p+24+k+34)&4095)-2048;
                 bt[0][k]=ibt[0][k]*CCBT_FM[16+k][0].A+CCBT_FM[16+k][0].B;
                 bt[1][k]=ibt[1][k]*CCBT_FM[20+k][0].A+CCBT_FM[20+k][0].B;
                 bt[2][k]=ibt[2][k]*CCBT_FM[16+k][1].A+CCBT_FM[16+k][1].B;
                 bt[3][k]=ibt[3][k]*CCBT_FM[20+k][1].A+CCBT_FM[20+k][1].B;
                }
#pragma omp critical (rec)
{               
                for (int k=sizeof(ArrayC)/sizeof(ArrayC[0])-1;k>=rec;k--){
                //cout <<gettime()<<" "<<bt[0][0]<<endl;
                if(AMSEvent::gethead())ArrayC[k].Time=AMSEvent::gethead()->gettime();
                else ArrayC[k].Time=(*(p-1)) |  (*(p))<<16;     //tbfixed
                for(int i=0;i<4;i++){
                  ArrayC[k].T[i]=bt[i][3];
                  ArrayC[k].B[0][i]=bt[i][0];
                  ArrayC[k].B[1][i]=bt[i][2];
                  ArrayC[k].B[2][i]=bt[i][1];
                }
                 
                }
                cout <<"AMSEvent::buildcceb-I-RecordFilled "<<rec<<" "<<ArrayC[rec].Time<<endl;
                rec=(rec+1)%(sizeof(ArrayC)/sizeof(ArrayC[0]));

                if(rec==0){
                  AMSTimeID * ptdv=AMSJob::gethead()->gettimestructure(AMSID("CCEBPar",AMSJob::gethead()->isRealData()));
                int diff=ArrayC[sizeof(ArrayC)/sizeof(ArrayC[0])-1].Time-ArrayC[0].Time;
                bool ok=diff>0 && diff<864000;
    for(int i=0;i<sizeof(ArrayC)/sizeof(ArrayC[0]);i++){
       cout<<" Time " <<i<< " "<<ArrayC[i].Time<<endl;
                }
                if(ptdv && ok){
                  ptdv->UpdateMe()=1;
                  ptdv->UpdCRC();
   time_t begin,end,insert;
   time(&insert);
   if(CALIB.InsertTimeProc)insert=ArrayC[0].Time;
   
   ptdv->SetTime(insert,ArrayC[0].Time,ArrayC[sizeof(ArrayC)/sizeof(ArrayC[0])-1].Time+3600);
   cout <<" CCEB H/K  info has been read for "<<*ptdv;
   ptdv->gettime(insert,begin,end);
   cout <<" Time Insert "<<ctime(&insert);
   cout <<" Time Begin "<<ctime(&begin);
   cout <<" Time End "<<ctime(&end);
   if(IOPA.hlun){
   CALIB.Ntuple++;
   HBNT(CALIB.Ntuple,"CCEB"," ");
   static CCEBPar TRCALIB;
   HBNAME(CALIB.Ntuple,"CCEB",(int*)(&TRCALIB),"Time:I,B(4,3):R,T(4):R");
               for(int i=0;i<sizeof(ArrayC)/sizeof(ArrayC[0]);i++){
                ucopy_(&ArrayC[i],&TRCALIB,sizeof(TRCALIB)/sizeof(integer));
                HFNT(CALIB.Ntuple);
               }
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("CCEB");

           }
       }
               else cerr<<"AMSEvent::buildcceb-E-NoTDVFoundOrBadRecord "<<AMSID("CCEBPar",AMSJob::gethead()->isRealData())<<" "<<diff<<endl;
}
}
}


integer AMSEvent::checkccebid(int16u id){
  char sstr[128];
  char nn[]="ABPS";
 for(int i=0;i<4;i++){
  strcpy(sstr,"CCEB-");
  strncat(sstr,nn+i,1);
  if(DAQEvent::ismynode(id,sstr))return i+1; 
 }
 return 0;
}

//-------- TofSlowTemp codes ---------
//
integer AMSEvent::checktofstid(int16u id){
  int valid(0);
  int16u addr=((id>>5)&(0x1FF));
  int16u dtyp=(id&0x1F);
//  cout<<"---> In AMSEvent::checktofstid:id="<<hex<<id<<dec<<" addr="<<addr<<" dtyp="<<dtyp<<endl;
  if(!(dtyp==24 || dtyp==25))return 0;
  char sstr[128];
  char abps[]="ABPS";
  for(int i=0;i<4;i++){
    strcpy(sstr,"MPD-");
    strncat(sstr,abps+i,1);
    if(DAQEvent::ismynode(id,sstr)){//UTof
      valid=i+1;
      break;
    } 
  }
  if(valid==0){
    for(int i=0;i<4;i++){
      strcpy(sstr,"JPD-");
      strncat(sstr,abps+i,1);
      if(DAQEvent::ismynode(id,sstr)){//LTof
        valid=10+i+1;
        break;
      } 
    }
  }
  if(valid!=0 && dtyp==25)valid+=100;
//  cout<<"  return val="<<valid<<endl;
  return valid;//Lftt=1|Ltof=1|Rdcy=1-4(a,b,p,s)
//
}
void AMSEvent::buildtofst(integer leng, int16u* p){
  int16u len,uls,abps,rdcy,side,bus,stat;
  int16u word1,word2,word3,ams,als,age;
  int16u tms,tls,tmp;
  geant arr[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int msta[2][2]={0,0,0,0};
  int navp[2][2]={0,0,0,0};
  geant avrp[2][2]={0,0,0,0};// aver. pm-temp [il][is]
  geant avrc[2][2]={0,0,0,0};// aver. sfec-temp [il][is]
  int id,il,ill,ib,is,ul; 
  time_t dtst;
  uinteger htime;
  geant temp;
  char ddate[30];
  uchar tmps;
  int16u* p2b=p+2;
  int npairs,nwords;
  int shift,itemp;
  bool UTof(0);
  bool LTof(0);
  bool Lfmt(0);
  bool sfec(0);
  geant wtemp[2][4]={0,0,0,0,0,0,0,0};
  int wstat[2][4]={0,0,0,0,0,0,0,0};
//
  static int recU=0;
  static int recL=0;
//
  len=int16u(leng&(0xFFFFL));//block's length in 16b-words(not including length word itself)
  uls=int16u((leng>>16)&(0x3FFFL));//U(L)|S(U=0,L=1|S=1-4=>a,b,p,s) as return by my "checkblockid"-1
  Lfmt=((uls+1)/100==1);//long temper.format
  UTof=(((uls+1)%100)/10==0);
  LTof=(((uls+1)%100)/10==1);
  rdcy=((uls+1)%10);//1/2/3/4->a,b,p,s
  side=(rdcy+1)%2;//0/1->a(p)/b(s)
  bus=(*(p+2)>>8);
  stat=(*(p-1)>>12);
  htime=(uinteger(*p)<<16 | uinteger(*(p+1)));
  dtst=time_t(htime);
  strcpy(ddate,asctime(localtime(&dtst)));
  if(bus!=2)goto BadExit;
  TOF2JobStat::addtofst(0);//entr(bus ok)
  
  cout<<"<------ Date of the USCM-reply : "<<ddate<<endl;
//  cout<<"p-3="<<hex<<*(p-3)<<" p-2="<<*(p-2)<<"  p-1="<<*(p-1)<<"  p="<<*p<<" p+1="<<*(p+1)<<" p+2="<<*(p+2)<<dec<<endl;
//  cout<<"   size(bytes)="<<*(p-3)<<" "<<*(p-4)<<endl;
//  cout<<"   U/Ltof="<<UTof<<"/"<<LTof<<" side="<<side<<" Lfmt="<<Lfmt<<" length="<<len<<" bus="<<bus<<" stat="<<stat<<endl;
  
  if(stat!=0)goto BadExit;
  TOF2JobStat::addtofst(1);//bus+stat ok
  
  TOF2JobStat::addtofst(2+20*int(LTof));//U/LTof entries
  
  if(UTof){
    if(!Lfmt)TOF2JobStat::addtofst(3);//UTof-Sfmt
    else TOF2JobStat::addtofst(4);//UTof-Lfmt
  }
  if(LTof){
    if(!Lfmt)TOF2JobStat::addtofst(23);//LTof-Sfmt
    else TOF2JobStat::addtofst(24);//LTof-Lfmt
  }
  
  if((Lfmt && len==37) || (!Lfmt && len==13))TOF2JobStat::addtofst(5+20*int(LTof));//block length ok
  else goto BadExit;
  
  if(side==0)TOF2JobStat::addtofst(6+20*int(LTof));//sideA(P)
  else TOF2JobStat::addtofst(7+20*int(LTof));//sideB(S)
//
if(UTof)cout<<"---> Decoding UTof:"<<endl;
if(LTof)cout<<"---> Decoding LTof:"<<endl;
  if(Lfmt){//decode Lfmt
    npairs=floor((len-4)/2);//temp+age pairs (-4 for header words)
    shift=0;
    itemp=0;
    for(int ip=0;ip<npairs;ip++){
      word1=*(p2b+shift);
      word2=*(p2b+1+shift);
      word3=*(p2b+2+shift);
      tms=((word1&0xFF)<<8);//t-msb
      tls=((word2&0xFF00)>>8);//t-lsb
      ams=((word2&0xFF)<<8);//a-msb
      als=((word3&0xFF00)>>8);//a-lsb
      age=(ams|als);//int16u
      tmp=(tms|tls);//int16u
      if(tmp!=0x8000 && age<300){//ok (age<5min, otherwise too old = bad)
        temp=float(int16(tmp))/256-0.25;//float
      }
      else temp=-273;
      arr[itemp]=temp;
      itemp+=1;
      cout<<"LFmtIt="<<itemp<<"  temp="<<temp<<" age="<<age<<" tmp="<<tmp<<endl;
      shift+=2;
    }
  }
  else{
    nwords=len-4;//9
    itemp=0;
    for(int ip=0;ip<nwords;ip++){
      word1=*(p2b+ip);
      if(ip<(nwords-1)){//lsb
        tmps=uchar(word1&(0xFF));
        if(tmps!=0x80){//ok 
          temp=float(char(tmps))/2.;//float
        }
        else temp=-273;
        arr[itemp]=temp;
        itemp+=1;
	cout<<"It="<<itemp<<"  temp="<<temp<<endl;
      }
//
      if(ip>0){
	tmps=uchar(word1>>8);//msb
        if(tmps!=0x80){//ok 
          temp=float(char(tmps))/2.;//float
        }
        else temp=-273;
        arr[itemp]=temp;
        itemp+=1;
        cout<<"SFmtIt="<<itemp<<"  temp="<<temp<<endl;
      }
    }
  }
//  cout<<"<--- found Ntemps="<<itemp<<endl;
  
//--- make sens.average per /layer/side:
  if(UTof)ul=0;
  else ul=1;
  for(int sen=0;sen<16;sen++){
    id=AMSSCIds::getenvsensid(ul,int(side),sen);
    sfec=!((id/1000)>0);
    if((id/1000)>0){
      il=(id/1000);//1-8(10)
      ib=(id%1000)/10;
    }
    else il=id/10;
    is=id%10;//1,2
    ill=(il-1)%2;//0/1->L1(3)/L2(4)
    cout<<"  sensID:"<<id<<" ul="<<ul<<" scec? "<<sfec<<" il/ib/is="<<il<<" "<<ib<<" "<<is<<" ill="<<ill<<" arr="<<arr[sen]<<endl;
    if(sfec)avrc[ill][is-1]=arr[sen];// sfec (may be -273 if bad)
    else{// pmt
      if(arr[sen]>-273){
        avrp[ill][is-1]+=arr[sen];
        navp[ill][is-1]+=1;
      }
    }
  }
  
  for(int i=0;i<2;i++){
    for(int j=0;j<2;j++){
      if(navp[i][j]>0)avrp[i][j]/=navp[i][j];
      else avrp[i][j]=-273;//for bad average
    }
  }
//--- fill TofSTemp-object(UTof or LTof):
cout<<"SFEC-temp, L1(S1/S2)="<<avrc[0][0]<<"/"<<avrc[0][1]<<" L2(S1/S2)="<<avrc[1][0]<<"/"<<avrc[1][1]<<endl; 
cout<<"PMTS-temp, L1(S1/S2)="<<avrp[0][0]<<"/"<<avrp[0][1]<<" L2(S1/S2)="<<avrp[1][0]<<"/"<<avrp[1][1]<<endl; 
#pragma omp critical (recU)
{
  if(UTof){//<--- UTof               
  for(int k=sizeof(UTofTemp)/sizeof(UTofTemp[0])-1;k>=recU;k--){
    if(AMSEvent::gethead())UTofTemp[k].Time=AMSEvent::gethead()->gettime();
    else UTofTemp[k].Time=htime;//tbfixed
    UTofTemp[k].chain=side;
    for(int i=0;i<2;i++){//layer
    for(int j=0;j<2;j++){//tof-side
      UTofTemp[k].temp[i][2*j]=avrc[i][j];//sfec
      UTofTemp[k].temp[i][2*j+1]=avrp[i][j];//pmt
    }
    }             
  }
  cout <<"AMSEvent::buildtofst-I- UTof RecordFilled "<<recU<<" "<<UTofTemp[recU].Time<<endl;
  recU=(recU+1)%(sizeof(UTofTemp)/sizeof(UTofTemp[0]));
//
  if(recU==0){
    AMSTimeID * ptdv=AMSJob::gethead()->gettimestructure(AMSID("UTofSTempPar",AMSJob::gethead()->isRealData()));
    int diff=UTofTemp[sizeof(UTofTemp)/sizeof(UTofTemp[0])-1].Time-UTofTemp[0].Time;
    bool ok=diff>0 && diff<86400;
    for(int i=0;i<sizeof(UTofTemp)/sizeof(UTofTemp[0]);i++){
       cout<<"Static array UTof Time " <<i<< " "<<UTofTemp[i].Time<<endl;
    }
    if(ptdv && ok){
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time_t begin,end,insert;
      time(&insert);
      if(CALIB.InsertTimeProc)insert=UTofTemp[0].Time;
   
      ptdv->SetTime(insert,UTofTemp[0].Time,UTofTemp[sizeof(UTofTemp)/sizeof(UTofTemp[0])-1].Time+3600);
      cout <<" TofSTemp info has been read for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
    }
    else cerr<<"AMSEvent::buildtofst-E-NoTDVFoundOrBadRecord "<<AMSID("UTofSTempPar",AMSJob::gethead()->isRealData())<<" "<<diff<<endl;
  }
  }//--->endof UTof
//--
  if(LTof){//<--- LTof               
  for(int k=sizeof(LTofTemp)/sizeof(LTofTemp[0])-1;k>=recL;k--){
    if(AMSEvent::gethead())LTofTemp[k].Time=AMSEvent::gethead()->gettime();
    else LTofTemp[k].Time=htime;//tbfixed
    LTofTemp[k].chain=side;
    for(int i=0;i<2;i++){//layer
    for(int j=0;j<2;j++){//tof-side
      LTofTemp[k].temp[i][2*j]=avrc[i][j];//sfec
      LTofTemp[k].temp[i][2*j+1]=avrp[i][j];//pmt
    }
    }             
  }
  cout <<"AMSEvent::buildtofst-I- LTof RecordFilled "<<recL<<" "<<LTofTemp[recL].Time<<endl;
  recL=(recL+1)%(sizeof(LTofTemp)/sizeof(LTofTemp[0]));
//
  if(recL==0){
    AMSTimeID * ptdv=AMSJob::gethead()->gettimestructure(AMSID("LTofSTempPar",AMSJob::gethead()->isRealData()));
    int diff=LTofTemp[sizeof(LTofTemp)/sizeof(LTofTemp[0])-1].Time-LTofTemp[0].Time;
    bool ok=diff>0 && diff<86400;
    for(int i=0;i<sizeof(LTofTemp)/sizeof(LTofTemp[0]);i++){
       cout<<"Static array LTof Time " <<i<< " "<<LTofTemp[i].Time<<endl;
    }
    if(ptdv && ok){
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time_t begin,end,insert;
      time(&insert);
      if(CALIB.InsertTimeProc)insert=LTofTemp[0].Time;
   
      ptdv->SetTime(insert,LTofTemp[0].Time,LTofTemp[sizeof(LTofTemp)/sizeof(LTofTemp[0])-1].Time+3600);
      cout <<" LTofSTemp info has been read for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
    }
    else cerr<<"AMSEvent::buildtofst-E-NoTDVFoundOrBadRecord "<<AMSID("LTofSTempPar",AMSJob::gethead()->isRealData())<<" "<<diff<<endl;
  }
  }//--->endof LTof
}
//--- 
BadExit:
  return;
}
//-----
geant AMSEvent::TofSTemp::gettempC(int lay, int side){
  geant atemp=temp[lay][2*side];
  return atemp;
}
geant AMSEvent::TofSTemp::gettempP(int lay, int side){
  geant atemp=temp[lay][2*side+1];
  return atemp;
}  
void AMSEvent::SetTofSTemp(){
//create default TofSTemp records, called in _timeinitjob().
  char frdate[30];
  time_t data;
  for(int i=0;i<sizeof(UTofTemp)/sizeof(UTofTemp[0]);i++){
    for(int j=0;j<2;j++){//lay loop
      for(int k=0;k<4;k++){
        UTofTemp[i].temp[j][k]=1;//degr.C
      }
    }  
    UTofTemp[i].Time=mktime(&AMSmceventg::Orbit.Begin);
    data=mktime(&AMSmceventg::Orbit.Begin);
    strcpy(frdate,asctime(localtime(&data)));
//cout<<"<--- UTime="<<mktime(&AMSmceventg::Orbit.Begin)<<"  "<<frdate<<endl;
    UTofTemp[i].chain=0;
  }
  for(int i=0;i<sizeof(LTofTemp)/sizeof(LTofTemp[0]);i++){
    for(int j=0;j<2;j++){//lay loop
      for(int k=0;k<4;k++){
        LTofTemp[i].temp[j][k]=-1;//degr.C
      }
    }  
    LTofTemp[i].Time=mktime(&AMSmceventg::Orbit.Begin);
    LTofTemp[i].chain=0;
  }
} 
//------------------------------------


void AMSEvent::LoadISS(){
unsigned int gpsdiff=15;
if(AMSNtuple::LoadISS(_time)){
ISSGTOD(&_StationRad,&_StationTheta,&_StationPhi,&_StationSpeed,&_VelTheta,&_VelPhi,&_NorthPolePhi,double(_time)+_usec/1.e6-gpsdiff);
//double v_earth=2*3.1415926/24/3600;
//double vz=fabs(_StationSpeed*sin(_VelTheta));
//double vr=fabs(_StationSpeed*cos(_VelTheta));
//vr-=v_earth*sin(atan(AMSmceventg::Orbit.AlphaTanThetaMax));
//double vgtod=sqrt(vz*vz+vr*vr);
_NorthPolePhi+=AMSmceventg::Orbit.PolePhiStatic;
}
int ret=AMSNtuple::ISSAtt(_Roll,_Pitch,_Yaw,double(_time)+_usec/1.e6-gpsdiff);
static int print=0;
if( ret && print++<100)cerr<<" AMSEvent::LoadISS-E-ISSAtt Returns "<<ret<<" "<<_Roll<<" "<<_Pitch<<" "<<_Yaw<<" "<<_time<<endl;

ret=AMSNtuple::ISSSA(_Alpha,_B1a,_B3a,_B1b,_B3b,double(_time)+_usec/1.e6-gpsdiff);
if( ret && print++<100)cerr<<" AMSEvent::LoadISS-E-ISSSA Returns "<<ret<<" "<<_Alpha<<" "<<_B1a<<" "<<_B3a<<" "<<_time<<endl;

float r,phi,theta,v,vphi,vtheta;
ret=AMSNtuple::ISSCTRS(r,theta,phi,v,vtheta,vphi,double(_time)+_usec/1.e6-gpsdiff);
if( ret && print++<100)cerr<<" AMSEvent::LoadISS-E-ISSCTRS Returns "<<ret<<" "<<r<<" "<<theta<<" "<<phi<<" "<<v<<" "<<_time<<endl;
if(!ret){
// replace?
//cout<< "tbd "<<endl;

}

}


#ifdef _PGTRACK_
#include "event_tk.C"
#endif



