// Author V. Choutko 24-may-1996
// TOF parts changed 25-sep-1996 by E.Choumilov.
// add TDV/dbase version October 1, 1997. a.k.
//
// Last Edit : Nov 26, 1997. ak.
//
#include <trrawcluster.h>
#include <typedefs.h> 
#include <tofdbc.h> 
#include <event.h>
#include <cont.h>
#include <commons.h>
#include <amsgobj.h>
#include <trrec.h>
#include <mccluster.h>
#include <tofrec.h>
#include <beta.h>
#include <charge.h>
#include <particle.h>
#include <ctcrec.h>
#include <iostream.h>
#include <fstream.h>
#include <tofsim.h>
#include <stdlib.h>
#include <tofcalib.h>
#include <daqblock.h>
#include <ctccalib.h>
#include <ntuple.h>
#include <ctcdbc.h>
#include <timeid.h>
#include <trcalib.h>
#include <trigger1.h>
#include <trigger3.h>
#include <antirec.h>
#include <ctcsim.h>
#include <user.h>
#include <signal.h>
extern "C" void uglast_();
 
#ifdef __DB__

#include <dbS.h>
extern LMS* lms;

#endif

//
//
integer AMSEvent::debug=0;
AMSEvent* AMSEvent::_Head=0;
AMSNodeMap AMSEvent::EventMap;
integer AMSEvent::SRun=0;
void AMSEvent::_init(){
  // check old run & 
   if(_run!= SRun || !AMSJob::gethead()->isMonitoring())_validate();
  if(_run != SRun){
   cout <<" AMS-I-New Run "<<_run<<endl;
   // get rid of crazy runs
   if(_run<TRMFFKEY.OKAY/10 && AMSJob::gethead()->isRealData()){
     cerr<<"AMSEvent::_init-S-CrazyRunFound "<<_run<<endl;
     raise(SIGTERM);
   }
   DAQEvent::initO(_run);
   
   if(AMSJob::gethead()->isSimulation())_siamsinitrun();
   _reamsinitrun();
  }

  // Initialize containers & aob
  if(AMSJob::gethead()->isSimulation())_siamsinitevent();
  _reamsinitevent();
  if(AMSJob::gethead()->isCalibration())_caamsinitevent();

#ifdef __DB__
  if (_checkUpdate() == 1 || _run != SRun) {
   cout <<"AMSEvent:: -I- UpdateMe is set. Update database and tables. "<<endl;
   int rstatus = lms -> AddAllTDV();
   int n       = AMSJob::gethead()->FillTDVTable();
   lms -> FillTDV(n);
  } else {
    // cout <<"AMSEvent:: -I- UpdateMe != 1. NO UPDATE"<<endl;
  }
  //_validateDB();
  _validate();
#endif



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
    SRun=_run;
#ifndef __DB__
   _validate(1);
#endif
  }

}

void AMSEvent::_siamsinitrun(){
_sitkinitrun();
_sitofinitrun();
_siantiinitrun();
_sictcinitrun();
}

void AMSEvent::_reamsinitrun(){

_retkinitrun();
_retofinitrun();
_reantiinitrun();
_rectcinitrun();
_reaxinitrun();
AMSUser::InitRun();
}


void AMSEvent::_siamsinitevent(){
 _signinitevent();
 _sitkinitevent();
 _sitofinitevent();
 _siantiinitevent();
 _sictcinitevent();
}

void AMSEvent::_reamsinitevent(){
 _redaqinitevent();
 _retkinitevent();
 _retriginitevent();
 _retofinitevent();
 _reantiinitevent();
 _rectcinitevent();
 _reaxinitevent();
}

void AMSEvent::_signinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSmceventg",0),0));
 
}

void AMSEvent::SetTimeCoo(){    
  AMSgObj::BookTimer.start("SetTimeCoo");
  // Allocate time & define the geographic coordinates
  if(AMSJob::gethead()->isSimulation()){
    static number dtime=AMSmceventg::Orbit.FlightTime/
      (GCFLAG.NEVENT+1-GCFLAG.IEVENT);
    number t2=
      AMSmceventg::Orbit.AlphaTanThetaMax*AMSmceventg::Orbit.AlphaTanThetaMax;
    static number curtime=0;
    static number theta=AMSmceventg::Orbit.ThetaI;
    static number philocal=
      atan2(sin(AMSmceventg::Orbit.PhiI-AMSmceventg::Orbit.PhiZero)*sqrt(1+t2),
            cos(AMSmceventg::Orbit.PhiI-AMSmceventg::Orbit.PhiZero));
    static number pole=AMSmceventg::Orbit.PolePhi;
    geant dd; 
    int i;
    number xsec=0;
    for(i=0;i<AMSmceventg::Orbit.Nskip+1;i++)
      xsec+=-dtime*log(RNDM(dd)+1.e-30);
    curtime+=xsec;
    pole=fmod(pole+AMSmceventg::Orbit.EarthSpeed*xsec,AMSDBc::twopi);
    philocal=fmod(philocal+AMSmceventg::Orbit.AlphaSpeed*xsec,AMSDBc::twopi);
    number phi=atan2(sin(philocal),cos(philocal)*sqrt(1+t2));
    if(phi < 0)phi=phi+AMSDBc::twopi;
    theta=atan(AMSmceventg::Orbit.AlphaTanThetaMax*
               sin(phi));
    _time=integer(mktime(&AMSmceventg::Orbit.Begin)+curtime);
    _usec=(curtime-integer(curtime))*1000000;
    _NorthPolePhi=pole;
    _StationTheta=theta;
    _StationPhi=fmod(phi+AMSmceventg::Orbit.PhiZero,AMSDBc::twopi);
    GCFLAG.IEVENT=GCFLAG.IEVENT+AMSmceventg::Orbit.Nskip;
    AMSmceventg::Orbit.Nskip=0;        
    AMSmceventg::Orbit.Ntot++;
    _Yaw=0;
    _Roll=0;
    _Pitch=0;
    _StationSpeed=AMSmceventg::Orbit.AlphaSpeed;
    _StationRad=AMSmceventg::Orbit.AlphaAltitude;
    _SunRad=0;
    _SunTheta=0;
    _SunPhi=0;
  }
  else {
    static integer hint=0;
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
      if(hint>=sizeof(Array)/sizeof(Array[0])-1)hint=sizeof(Array)/sizeof(Array[0])-2;
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
      _SunTheta=Array[hint].SunTheta;
      _SunPhi=Array[hint].SunPhi;
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
      _SunTheta=Array[hint+1].SunTheta;
      _SunPhi=Array[hint+1].SunPhi;
    }
    else{
      //interpolation needed
      number xsec=_time-Array[hint].Time;
      number dt=Array[hint+1].Time-Array[hint].Time;
      if(dt){
        _Yaw=Array[hint].StationYaw+xsec/dt*(Array[hint+1].StationYaw-Array[hint].StationYaw);
        _Roll=Array[hint].StationRoll+xsec/dt*(Array[hint+1].StationRoll-Array[hint].StationRoll);
        _Pitch=Array[hint].StationPitch+xsec/dt*(Array[hint+1].StationPitch-Array[hint].StationPitch);
        _StationSpeed=Array[hint].StationSpeed+xsec/dt*(Array[hint+1].StationSpeed-Array[hint].StationSpeed);
        _StationRad=Array[hint].StationR+xsec/dt*(Array[hint+1].StationR-Array[hint].StationR);
        _SunRad=Array[hint].SunR+xsec/dt*(Array[hint+1].SunR-Array[hint].SunR);
        _SunTheta=Array[hint].SunTheta+xsec/dt*(Array[hint+1].SunTheta-Array[hint].SunTheta);
        _SunPhi=Array[hint].SunPhi+xsec/dt*(Array[hint+1].SunPhi-Array[hint].SunPhi);
      }
      else {
        _Yaw=Array[hint].StationYaw;
        _Roll=Array[hint].StationRoll;
        _Pitch=Array[hint].StationPitch;
        _StationSpeed=Array[hint].StationSpeed;
        _StationRad=Array[hint].StationR;
        _SunRad=Array[hint].SunR;
        _SunTheta=Array[hint].SunTheta;
        _SunPhi=Array[hint].SunPhi;
      }
      _NorthPolePhi=fmod(AMSmceventg::Orbit.PolePhiStatic+Array[hint].GrMedPhi+
                         AMSmceventg::Orbit.EarthSpeed*xsec,AMSDBc::twopi);
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
      _StationPhi=fmod(phi+PhiZero,AMSDBc::twopi);


     
    }
  }
  AMSgObj::BookTimer.stop("SetTimeCoo");

}


void AMSEvent::_sitkinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}


void AMSEvent::_retriginitevent(){

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:TriggerLVL1",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:TriggerLVL3",0),0));
}

void AMSEvent::_siantiinitevent(){
 AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
}



void AMSEvent::_sitofinitevent(){
  int il;
  AMSNode *ptr;
//
//           declare some TOF containers for MC:
//
// container for geant hits:
//
  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFMCCluster",0),0));
//
//    container for time_over_threshold hits (digi step):
//
  for(il=0;il<SCLRS;il++){
    ptr=AMSEvent::gethead()->add(
        new AMSContainer(AMSID("AMSContainer:AMSTOFTovt",il),0));
  }
}

void AMSEvent::_sictcinitevent(){
  for(int i=0;i<CTCDBc::getnlay();i++){
   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSCTCMCCluster",i),0));
  }
}






void AMSEvent::_reantiinitevent(){

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawEvent",0),0));
      
      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",0),0));

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiRawCluster",1),0));

      AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSAntiCluster",0),0));
}
//=====================================================================
void AMSEvent::_retofinitevent(){
  integer i;
  AMSNode *ptr;
//
// container for RawEvent hits(same structure for MC/REAL events) : 
//
  ptr=AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSTOFRawEvent",0),0));
//---
//  container for RawCluster hits :
//
  ptr=  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFRawCluster",0),0));
//---
// container for Cluster hits :
//
  for( i=0;i<SCLRS;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFCluster",i),0));
//
  DAQSBlock::clrtbll();//clear sc.data length
}
//=====================================================================
void AMSEvent::_rectcinitevent(){
  integer i;

  AMSEvent::gethead()->add(
  new AMSContainer(AMSID("AMSContainer:AMSCTCRawEvent",0),0));
      
  for(i=0;i<CTCDBc::getnlay();i++){

   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSCTCRawCluster",i),0));

   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSCTCRawHit",i),0));


   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSCTCCluster",i),0));
  }

}

void AMSEvent::_reaxinitevent(){
  integer i;
  AMSNode *ptr;
  for( i=0;i<npatb;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSBeta",i),&AMSBeta::build,0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCharge",0),&AMSCharge::build,0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",0),&AMSParticle::build,0));


  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AntiMatter",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:HeavyIon",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:Test",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:Dummy",0),0));





}

void AMSEvent::_retkinitevent(){
  integer i;
  AMSNode *ptr;
  for(i=0;i<2;i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",i),0));

  for( i=0;i<2;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),&AMSTrCluster::build,0));
  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrClusterWeak",i),&AMSTrCluster::buildWeak,0));

  for( i=0;i<6;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",i),&AMSTrRecHit::build,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHitWeak",i),&AMSTrRecHit::buildWeak,0));


  for( i=0;i<npat;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack",i),&AMSTrTrack::build,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackWeak",i),&AMSTrTrack::buildWeak,0));

  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseX",i),&AMSTrTrack::buildFalseX,0));
  for( i=0;i<1;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrackFalseTOFX",i),&AMSTrTrack::buildFalseTOFX,0));
}

void  AMSEvent::write(){
  // Sort before by "Used" variable : AMSTrTrack & AMSTrCluster & AMSCTCCl
  AMSEvent::gethead()->getheadC("AMSCTCCluster",0,1); 
  AMSEvent::gethead()->getheadC("AMSCTCCluster",1,1); 

  AMSEvent::gethead()->getheadC("AMSTrCluster",0,1); 
  AMSEvent::gethead()->getheadC("AMSTrCluster",1,1); 
  for(int i=0;i<npat;i++){
   AMSEvent::gethead()->getheadC("AMSTrTrack",i,1); 
  }
 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",0,1); 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",1,1); 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",2,1); 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",3,1); 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",4,1); 
  AMSEvent::gethead()->getheadC("AMSTrRecHit",5,1); 
   
  if(IOPA.hlun){
    AMSJob::gethead()->getntuple()->reset();
    _writeEl();
    AMSNode * cur;
    for (int i=0;;){
      cur=AMSEvent::EventMap.getid(i++);   // get one by one
      if(cur){
        if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
        writeC();
      }
      else break;
    }
    AMSJob::gethead()->getntuple()->write();
  }
}

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
   AMSUser::InitEvent();
   if(AMSJob::gethead()->isSimulation())_siamsevent();
      _reamsevent();
      if(AMSJob::gethead()->isCalibration())_caamsevent();
}
//------------------------------------------------------------------
void AMSEvent::_siamsevent(){
_sitkevent(); 
_sitofevent(); 
_siantievent(); 
_sictcevent(); 
_sitrigevent(); 
 if(TOFMCFFKEY.fast==0)_sidaqevent(); //DAQ-simulation only for slow algorithm
}

void AMSEvent::_reamsevent(){

  geant d;
  if(AMSJob::gethead()->isMonitoring() && RNDM(d)>IOPA.Portion && GCFLAG.NEVENT>100){
    // skip event
     return;    
  }



#ifndef __AMSDEBUG__  
  if(AMSJob::gethead()->isReconstruction() )_redaqevent();
#else
  if(TOFMCFFKEY.fast==0)_redaqevent();
#endif
  // Skip EveryThing 
  if(DAQCFFKEY.NoRecAtAll)return;
  _retofevent();
  _reantievent();
  _rectcevent(); 
if(AMSJob::gethead()->isReconstruction() )_retrigevent();
  _retkevent(); 
  _reaxevent();
   AMSUser::Event();
}

void AMSEvent::_caamsinitevent(){
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CCerenkov)_cactcinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxinitevent();
}

void AMSEvent::_catkinitevent(){
  if(TRCALIB.CalibProcedureNo == 2){
   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSTrCalibration",0),0));
  }
}

void AMSEvent::_catofinitevent(){
}


void AMSEvent::_cantinitevent(){
}


void AMSEvent::_caaxinitevent(){
}

void AMSEvent::_cactcinitevent(){
}

void AMSEvent::_caamsevent(){
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAnti)_cantievent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CCerenkov)_cactcevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxevent();
}

void AMSEvent::_catkevent(){
  AMSgObj::BookTimer.start("CalTrFill");
  if(TRCALIB.CalibProcedureNo == 1){
    AMSTrIdCalib::check();
  }
  else if(TRCALIB.CalibProcedureNo == 2){
int i,j;
for(i=0;i<2;i++){
  for(j=0;j<tkcalpat;j++){
    if(AMSTrCalibFit::getHead(i,j)->Test()){
     AMSgObj::BookTimer.start("CalTrFit");
     AMSTrCalibFit::getHead(i,j)->Fit();
     AMSgObj::BookTimer.stop("CalTrFit");
    }
  }
}
  }
  AMSgObj::BookTimer.stop("CalTrFill");

}

void AMSEvent::_cactcevent(){
//  CTCGAINcalib::select();// event selection for CTC GAIN-calibration
}
//---------------------------------------------------------------------------
void AMSEvent::_catofevent(){
  integer trflag(0);
  TriggerLVL1 *ptr;
//
  ptr=(TriggerLVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr)trflag=ptr->gettoflg();
//  if(trflag <= 0)return;// use only H/W-triggered event tempor
  if(TOFRECFFKEY.relogic[0]==2)
           TOFTDIFcalib::select();// event selection for TOF TDIF-calibration
  if(TOFRECFFKEY.relogic[0]==3)
           TOFTZSLcalib::select();// event selection for TOF TZSL-calibration
  if(TOFRECFFKEY.relogic[0]==4)
           TOFAMPLcalib::select();// event selection for TOF AMPL-calibration
}
//---------------------------------------------------------------------------

void AMSEvent::_cantievent(){
}

void AMSEvent::_caaxevent(){
}

void AMSEvent::_retkevent(integer refit){



AMSgObj::BookTimer.start("RETKEVENT");

// do not reconstruct events without lvl3 if 
// LVL3FFKEY.Accept 
TriggerLVL3 *ptr=(TriggerLVL3*)getheadC("TriggerLVL3",0);

if(!LVL3FFKEY.Accept || (ptr && ptr->LVL3OK())){
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
  
  integer itrk=1;
  
  // Default reconstruction: 4S + 4K or more
  if(TRFITFFKEY.FalseXTracking && !TRFITFFKEY.FastTracking)
    itrk = buildC("AMSTrTrackFalseX",0);
  if(itrk)itrk=buildC("AMSTrTrack",refit);
  // Reconstruction with looser cuts on the K side
  if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.WeakTracking ){
    buildC("AMSTrClusterWeak",refit);
    buildC("AMSTrRecHitWeak",refit);
    itrk = buildC("AMSTrTrackWeak",refit);
  }

  if(TRFITFFKEY.FastTracking){
    // Reconstruction of 4S + 3K
    if ( (itrk<=0 || TRFITFFKEY.FullReco) && TRFITFFKEY.FalseXTracking ){
      itrk=buildC("AMSTrTrackFalseX",22);
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
  
  AMSgObj::BookTimer.stop("TrTrack");
#ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTrTrack::print();
#endif
  
  //if(refit==0 && AMSTrTrack::RefitIsNeeded())_retkevent(1);
}  
  AMSgObj::BookTimer.stop("RETKEVENT");
}

void AMSEvent::_reantievent(){
  integer trflag(0);
  TriggerLVL1 *ptr;
  int stat;
//
  AMSgObj::BookTimer.start("REANTIEVENT");
//
  ANTIJobStat::addre(0);
//
  if(AMSJob::gethead()->isSimulation() && TOFMCFFKEY.fast==1){ // fast algorithm
  }
//
  else{                                                        // slow algorithm
//
    ptr=(TriggerLVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr)trflag=ptr->gettoflg();
    if(trflag<=0){
//      AMSgObj::BookTimer.stop("REANTIEVENT");
//      return;// "no h/w TOF-trigger"  tempor commented
    }
    ANTIJobStat::addre(1);
//
    AMSAntiRawEvent::validate(stat);// RawEvent-->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTIJobStat::addre(2);
//
    AMSAntiRawCluster::build(stat);// RawEvent->RawCluster
    if(stat!=0){
      AMSgObj::BookTimer.stop("REANTIEVENT");
      return;
    }
    ANTIJobStat::addre(3);
//
  }
//
  AMSAntiCluster::build();// RawCluster->Cluster
  ANTIJobStat::addre(4);
// 
  #ifdef __AMSDEBUG__
   if(AMSEvent::debug)AMSAntiCluster::print();
  #endif
//
  AMSgObj::BookTimer.stop("REANTIEVENT");
}
//---------------------------------------------------------------------
void AMSEvent::_retofevent(){
integer trflag(0);
int stat;
TriggerLVL1 *ptr;
//
  AMSgObj::BookTimer.start("RETOFEVENT");
    TOFJobStat::addre(0);
    ptr=(TriggerLVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
    if(ptr)trflag=ptr->gettoflg();
    if(trflag<=0){
//      AMSgObj::BookTimer.stop("RETOFEVENT");
//      return;// "no h/w TOF-trigger"   tempor commented
    }
    TOFJobStat::addre(1);
//
    if(AMSJob::gethead()->isSimulation() && TOFMCFFKEY.fast==1){
//
//                   ===> reco of events, simulated by fast MC :
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build(stat);    // "RawCluster-->Cluster"
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOFJobStat::addre(4);
    }
    else{
//                   ===> reco of real events or simulated by slow MC:
//
//
      AMSgObj::BookTimer.start("TOF:validation");
      AMSTOFRawEvent::validate(stat);// RawEvent-->RawEvent
      AMSgObj::BookTimer.stop("TOF:validation");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOFJobStat::addre(2);
      AMSgObj::BookTimer.start("TOF:RwEv->RwCl");
      AMSTOFRawCluster::build(stat); // RawEvent-->RawCluster
      AMSgObj::BookTimer.stop("TOF:RwEv->RwCl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOFJobStat::addre(3);
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build(stat);    // RawCluster-->Cluster
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
      if(stat!=0){
        AMSgObj::BookTimer.stop("RETOFEVENT");
        return;
      }
      TOFJobStat::addre(4);
    }
//
  #ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSTOFCluster::print();
  #endif
  AMSgObj::BookTimer.stop("RETOFEVENT");
}
//========================================================================
void AMSEvent::_rectcevent(){
  integer trflag(0);
  int stat;
  TriggerLVL1 *ptr;
  AMSgObj::BookTimer.start("RECTCEVENT");
//
  CTCJobStat::addre(0);
//
  ptr=(TriggerLVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr)trflag=ptr->gettoflg();
  if(trflag<=0){
//    AMSgObj::BookTimer.stop("RECTCEVENT");
//    return;// "no h/w TOF-trigger"          // tempor commented
  }
//
  CTCJobStat::addre(1);
  AMSCTCRawEvent::validate(stat);// RawEvent-->RawEvent
  if(stat!=0){
    AMSgObj::BookTimer.stop("RECTCEVENT");
    return;
  }
  CTCJobStat::addre(2);
//
  AMSCTCRawHit::build(stat);// RawEvent-->RawHit
  if(stat!=0){
    AMSgObj::BookTimer.stop("RECTCEVENT");
    return;
  }
  CTCJobStat::addre(3);
//
  AMSCTCCluster::build();// RawHit-->Cluster
  CTCJobStat::addre(4);
//
  #ifdef __AMSDEBUG__
  if(AMSEvent::debug)AMSCTCCluster::print();
  #endif
  AMSgObj::BookTimer.stop("RECTCEVENT");
}

//========================================================================
void AMSEvent::_reaxevent(){
AMSgObj::BookTimer.start("REAXEVENT");
//
buildC("AMSBeta");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSBeta::print();
#endif
buildC("AMSCharge");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSCharge::print();
#endif
buildC("AMSParticle");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSParticle::print();
#endif
//
AMSgObj::BookTimer.stop("REAXEVENT");
}


void AMSEvent::_sitkinitrun(){

     for(int l=0;l<2;l++){
       for (int i=0;i<AMSDBc::nlay();i++){
         for (int j=0;j<AMSDBc::nlad(i+1);j++){
           for (int s=0;s<2;s++){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
            if(id.dead())continue;
            number oldone=0;
            for(int k=0;k<AMSDBc::NStripsDrp(i+1,l);k++){
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
}



void AMSEvent::_sitofinitrun(){
}

void AMSEvent::_siantiinitrun(){
}

void AMSEvent::_sictcinitrun(){
}

void AMSEvent::_retkinitrun(){
  // Warning if TRFITFFKEY.FastTracking is on...
  if(TRFITFFKEY.FastTracking){
    cout <<" "<<endl<<
    "AMSEvent::Retkinitrun-W-TRFITFFKEY.FastTracking selected."<<endl;
  }
  for (int half=0;half<2;half++){
    for(int side=0;side<2;side++){
      cout <<"AMSEvent::_retkevent-I-"<<AMSTrIdCalib::CalcBadCh(half,side)<<
      " bad channels found for half "<<half<<" and side "<<side<<endl;
    }
  }
}


void AMSEvent::_retofinitrun(){
}

void AMSEvent::_reantiinitrun(){
}

void AMSEvent::_rectcinitrun(){
}

void AMSEvent::_reaxinitrun(){
}

void AMSEvent:: _sitkevent(){
  if(TRMCFFKEY.NoiseOn)AMSTrMCCluster::sitknoise();
  AMSTrMCCluster::sitkcrosstalk();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTrMCCluster",0);
  if(p && AMSEvent::debug>1 )p->printC(cout);
#endif
    AMSTrRawCluster::sitkdigi();
#ifdef __AMSDEBUG__
  p =getC("AMSTrRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p =getC("AMSTrRawCluster",1);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}

void AMSEvent:: _siantievent(){
  int stat;
  AMSgObj::BookTimer.start("SIANTIEVENT");
  ANTIJobStat::addmc(0);
//
  if(TOFMCFFKEY.fast==0){//           ===> slow algorithm:
//
    AMSAntiRawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0){
      AMSgObj::BookTimer.stop("SIANTIEVENT");
      return;// no FTrigger from TOF - skip the rest
    }
    ANTIJobStat::addmc(1);
//
  }
  else{    //                         ===> fast algorithm:
//
    AMSAntiRawCluster::siantidigi();//Geant_hit->RawCluster
  }
//
#ifdef __AMSDEBUG__
  AMSContainer *p;
  p=getC("AMSAntiMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SIANTIEVENT");
}
//----------------------------------------------------------------

void AMSEvent:: _sitrigevent(){

  TriggerLVL1::build();
  TriggerLVL3::build();

}


void AMSEvent:: _retrigevent(){
  // Backup solution to "simulate" trigger 1 & 3 for rec data
  
   if(LVL1FFKEY.RebuildLVL1)TriggerLVL1::build();
   if(LVL3FFKEY.RebuildLVL3)TriggerLVL3::build();
}


//---------------------------------------------------------------
void AMSEvent:: _sitofevent(){
  AMSContainer *p;
  int stat;
//
  AMSgObj::BookTimer.start("SITOFDIGI");
  if(TOFMCFFKEY.fast==0){//           ===> slow algorithm:
   AMSgObj::BookTimer.start("TOF:Ghit->Tovt");
   TOFJobStat::addmc(0);
   AMSTOFTovt::build(); // Geant_hits-->Tovt_hits
   AMSgObj::BookTimer.stop("TOF:Ghit->Tovt");
//
   AMSgObj::BookTimer.start("TOF:Tovt->RwEv");
   AMSTOFRawEvent::mc_build(stat); // Tovt_hits-->RawEvent_hits
   AMSgObj::BookTimer.stop("TOF:Tovt->RwEv");
   if(stat!=0){
     AMSgObj::BookTimer.stop("SITOFDIGI");
     return; // no MC-trigger
   }
   TOFJobStat::addmc(1);
  }
  else{    //                         ===> fast algorithm:
    TOFJobStat::addmc(0);
    AMSTOFRawCluster::sitofdigi(stat);//Geant_hit->RawCluster
    if(stat!=0){
      AMSgObj::BookTimer.stop("SITOFDIGI");
      return; // no MC-trigger
    }
    TOFJobStat::addmc(2);
  }
  AMSgObj::BookTimer.stop("SITOFDIGI");
//
#ifdef __AMSDEBUG__
  //  p =getC("AMSTOFRawEvent",0);
  //  if(p && AMSEvent::debug)p->printC(cout);
  p =getC("AMSTOFRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p=getC("AMSTOFMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}

//=============================================================
void AMSEvent:: _sictcevent(){
  int stat(0);
//
//  if((AMSEvent::gethead()->getid())==56)CTCRECFFKEY.reprtf[1]=2;//tempor
//  else CTCRECFFKEY.reprtf[1]=0;
//
  AMSgObj::BookTimer.start("SICTCEVENT");
  CTCJobStat::addmc(0);
  AMSCTCRawHit::sictcdigi();// Ghits-->RawHit
  CTCJobStat::addmc(1);
//
  AMSCTCRawEvent::mc_build(stat);// RawHit-->RawEvent
  if(stat!=0){// no FTrigger from TOF - skip the rest
    AMSgObj::BookTimer.stop("SICTCEVENT");
    return; // no MC-TOFtrigger
  }
  CTCJobStat::addmc(2);
//
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSCTCRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
  AMSgObj::BookTimer.stop("SICTCEVENT");
}
//=============================================================

void AMSEvent::_findC(AMSID & id){

  static char names[1024]="AMSContainer:";
  static char * name=names;
#ifdef __AMSDEBUG__
  int nc=0; 
  if(13+strlen(id.getname())+1  > 1024){
   name=new char[13+strlen(id.getname())+1];
   nc=1;
   name[0]='\0';
   strcat(name,"AMSContainer:");
  }
#endif
  name[13]='\0';
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
#ifdef __AMSDEBUG__
  if(nc)delete[] name;
#endif


}


integer AMSEvent::setbuilderC(char name[], pBuilder pb){
  AMSID id(name,0);
  _findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
   p->setbuilder(pb);
   return 1;
  }
  else return 0;

}

integer AMSEvent::buildC(char name[], integer par){
   AMSID id(name,0);
   _findC(id);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 
  
}

integer AMSEvent::rebuildC(char name[], integer par){
   AMSID id(name,0);
   _findC(id);
  for(int i=0;;i++){
   id.setid(i);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->eraseC();
   }
   else break;
  }
   id.setid(0);
   AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p){
     p->runbuilder(par);
     return p->buildOK();
   }
   else return 0; 


}


AMSlink * AMSEvent::_getheadC( AMSID id, integer sorted){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    if(sorted)p->sort();
    // Reset global ref if any
    if(p->gethead())(p->gethead())->resethash(id.getid(),p->gethead());
    return p->gethead();
  }
  else return 0;
}
AMSlink * AMSEvent::_getlastC( AMSID id){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    return p->getlast();
  }
  else return 0;
}

integer AMSEvent::_setheadC( AMSID id, AMSlink *head){
_findC(id);
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p && head){
   p->sethead(head);
   return 1;
  }
  else return 0;
}

AMSContainer * AMSEvent::_getC( AMSID  id){
_findC(id);  
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  return p;
}

integer AMSEvent::getnC(char n[]){
  AMSID id;
  _findC(id);
  AMSContainer *p;
  for(int i=0;;i++){
   id.setid(i);
   p = (AMSContainer*)AMSEvent::gethead()->getp(id);
   if(p==0) return i;
  }
}

void AMSEvent::_copyEl(){
}

void AMSEvent::_printEl(ostream & stream){
 stream << "Run "<<_run<<" "<<getname()<<" "<< getid()<<" Time "<< 
   ctime(&_time)<<"."<<_usec<<" R "<<_StationRad<<" Theta "<<_StationTheta*AMSDBc::raddeg<<" Phi "<<_StationPhi*AMSDBc::raddeg<<" Speed "<<_StationSpeed<<
   " Pole "<<_NorthPolePhi*AMSDBc::raddeg<<endl;
}

void AMSEvent::_writeEl(){

// Fill the ntuple
  EventNtuple* EN = AMSJob::gethead()->getntuple()->Get_event();

  EN->Eventno=_id;
  EN->Run=_run;
  EN->RunType=_runtype;
  UCOPY(&_time,EN->Time,2*sizeof(integer)/4);
  EN->GrMedPhi=_NorthPolePhi-AMSmceventg::Orbit.PolePhiStatic;;
  EN->ThetaS=_StationTheta;
  EN->PhiS=_StationPhi;
  EN->RadS=_StationRad;
  EN->Yaw=_Yaw;
  EN->Pitch=_Pitch;
  EN->Roll=_Roll;
  EN->VelocityS=_StationSpeed;
  integer  i,nc;
  AMSContainer *p;
  EN->Particles=0;
  EN->Tracks=0;
  EN->Betas=0;
  EN->Charges=0;
  EN->TrRecHits=0;
  EN->TrClusters=0;
  EN->TrRawClusters=0;
  EN->TrMCClusters=0;
  EN->TOFClusters=0;
  EN->TOFMCClusters=0;
  EN->CTCClusters=0;
  EN->CTCMCClusters=0;
  EN->AntiClusters=0;
  EN->AntiMCClusters=0;

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSParticle",i);
   if(p) EN->Particles+=p->getnelem();
   else break;
  }  

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrTrack",i);
   if(p) EN->Tracks+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSBeta",i);
   if(p) EN->Betas+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCharge",i);
   if(p) EN->Charges+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrRecHit",i);
   if(p) EN->TrRecHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrCluster",i);
   if(p) EN->TrClusters+=p->getnelem();
   else break;
  }
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrRawCluster",i);
   if(p) EN->TrRawClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrMCCluster",i);
   if(p) EN->TrMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFCluster",i);
   if(p) EN->TOFClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFMCCluster",i);
   if(p) EN->TOFMCClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCTCCluster",i);
   if(p) EN->CTCClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCTCMCCluster",i);
   if(p) EN->CTCMCClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiCluster",i);
   if(p) EN->AntiClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiMCCluster",i);
   if(p) EN->AntiMCClusters+=p->getnelem();
   else break;
  }

}


integer AMSEvent::addnext(AMSID id, AMSlink *p){
 AMSContainer * ptr= AMSEvent::gethead()->getC(id);
   if(ptr){
     ptr->addnext(p);
     return 1;
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
 AMSContainer * ptr= AMSEvent::gethead()->getC(id);
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

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
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
    else {
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Time: "<<ctime(&_time)<<endl;
      cerr<<" Begin : " <<ctime(&b)<<endl; 
      cerr<<" End : " <<ctime(&e)<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<endl; 
      uglast_();
      exit(1);
    }
    offspring=(AMSTimeID*)offspring->next();
  }
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

void AMSEvent::Recovery(){
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
      remove();
      UPool.Release(1);
      cerr <<"AMSEvent::Recovery-I-Event structure removed"<<endl;
      sethead(0);
      UPool.erase(0);
      cerr <<"AMSEvent::Recovery-I-Memory pool released"<<endl;
      UPool.SetLastResort(10000);
      cerr <<"AMSEvent::Recovery-I-Cleanup done"<<endl;

}


void AMSEvent::_redaqinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",0),0));
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:DAQEvent",4),0));
}

void AMSEvent::_redaqevent(){
  //  Add Dummy for Copying ....
  addnext(AMSID("Dummy",0),new Test());

  // Erase some containers if in debug mode
#ifdef __AMSDEBUG__
  int i;
  for(i=0;;i++){
    AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrRawCluster",i);
      if(pctr)pctr->eraseC();
      else break ;
  }
#endif


  AMSgObj::BookTimer.start("REDAQ");

   DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->
   getheadC("DAQEvent",0);
   if(pdaq)pdaq->buildRawStructures();
   pdaq = (DAQEvent*)AMSEvent::gethead()->
   getheadC("DAQEvent",4);
   if(pdaq)pdaq->buildRawStructures();
  AMSgObj::BookTimer.stop("REDAQ");
}


void AMSEvent::_sidaqevent(){
  AMSgObj::BookTimer.start("SIDAQ");

DAQEvent *  pdaq = new DAQEvent();
AMSEvent::gethead()->addnext(AMSID("DAQEvent",0), pdaq);      
pdaq->buildDAQ();

// H/K simulation 

pdaq=new DAQEvent();
AMSEvent::gethead()->addnext(AMSID("DAQEvent",4), pdaq);      
pdaq->buildDAQ(4);


  AMSgObj::BookTimer.stop("SIDAQ");
}







void AMSEvent::builddaq(integer i, integer length, int16u *p){

*p= getdaqid(0);
*(p+2)=int16u(_Head->_run&65535);
*(p+1)=int16u((_Head->_run>>16)&65535);
*(p+4)=int16u(_Head->_runtype&65535);
*(p+3)=int16u(_Head->_runtype>>16&65535);
uinteger _event=uinteger(_Head->_id);
*(p+6)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+8)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+10)=int16u(_Head->_usec&65535);
*(p+9)=int16u((_Head->_usec>>16)&65535);
}
void AMSEvent::buildTrackerHKdaq(integer i, integer length, int16u *p){

*p= getdaqid(4);
*(p+2)=int16u(_Head->_run&65535);
*(p+1)=int16u((_Head->_run>>16)&65535);
*(p+4)=int16u(_Head->_runtype&65535);
*(p+3)=int16u(_Head->_runtype>>16&65535);
uinteger _event=uinteger(_Head->_id);
*(p+6)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+8)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+10)=int16u(_Head->_usec&65535);
*(p+9)=int16u((_Head->_usec>>16)&65535);
}


void AMSEvent::buildraw(
              integer length, int16u *p, uinteger & run, uinteger &id,
              uinteger &runtype, time_t & time, uinteger &usec){
  //  run=(*(p+1)) |  (*(p+2))<<16;
  //  runtype=(*(p+3)) |  (*(p+4))<<16;
  //  id=(*(p+5)) |  (*(p+6))<<16;
  //  time=(*(p+7)) |  (*(p+8))<<16;
  //  usec=(*(p+9)) |  (*(p+10))<<16;

    run=(*(p+2)) |  (*(p+1))<<16;
    runtype=(*(p+4)) |  (*(p+3))<<16;
    id=(*(p+6)) |  (*(p+5))<<16;
    time=(*(p+8)) |  (*(p+7))<<16;
    usec=(*(p+10)) |  (*(p+9))<<16;


}


integer AMSEvent::checkdaqid(int16u id){
if(id==getdaqid(0))return 1;
else if(id==getdaqid(4))return 5 ;
else return 0;


}

integer AMSEvent::calcTrackerHKl(integer i){
static integer init =0;
if(!TRMCFFKEY.WriteHK || abs(++init-TRMCFFKEY.WriteHK-1) >1)return 0;
return 1+2+2+2+4;
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
    Array[i].SunTheta=0;
    Array[i].SunPhi=0;
    Array[i].Time=mktime(&AMSmceventg::Orbit.Begin);
  }
}

AMSEvent::ShuttlePar AMSEvent::Array[60];
