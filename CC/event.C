// Author V. Choutko 24-may-1996
// TOF parts changed 25-sep-1996 by E.Choumilov.
// add TDV/dbase version October 1, 1997. a.k.
//
// Last Edit : Oct 6, 1997. ak.
//
#include <typedefs.h> 
#include <tofdbc.h> 
#include <event.h>
#include <cont.h>
#include <commons.h>
#include <amsgobj.h>
#include <trrawcluster.h>
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
#include <ntuple.h>
#include <ctcdbc.h>
#include <timeid.h>
#include <trcalib.h>
#include <trigger1.h>
#include <trigger3.h>
#include <antirec.h>
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
  if(_run != SRun){
   cout <<" AMS-I-New Run "<<_run<<endl;
   if(AMSJob::gethead()->isSimulation())_siamsinitrun();
   _reamsinitrun();
  }

  // Initialize containers & aob
  if(AMSJob::gethead()->isSimulation())_siamsinitevent();
  _reamsinitevent();
  if(AMSJob::gethead()->isCalibration())_caamsinitevent();

  if(_run != SRun){
    SRun=_run;
   _validate();
  }
#ifdef __DB__
  _validateDB();
#endif
}

void AMSEvent::_siamsinitrun(){
_sitkinitrun();
_sitofinitrun();
_siantiinitrun();
_sictcinitrun();
_sitrdinitrun();
}

void AMSEvent::_reamsinitrun(){

_retkinitrun();
_retrdinitrun();
_retofinitrun();
_reantiinitrun();
_rectcinitrun();
_reaxinitrun();
}


void AMSEvent::_siamsinitevent(){
 _signinitevent();
 _sitkinitevent();
 _sitrdinitevent();
 _sitofinitevent();
 _siantiinitevent();
 _sictcinitevent();
}

void AMSEvent::_reamsinitevent(){
 _redaqinitevent();
 _retkinitevent();
 _retriginitevent();
 _retrdinitevent();
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
   _time=integer(mktime(&AMSmceventg::Orbit.Begin)+curtime+0.5);
   _NorthPolePhi=pole;
   _StationTheta=theta;
   _StationPhi=fmod(phi+AMSmceventg::Orbit.PhiZero,AMSDBc::twopi);
   GCFLAG.IEVENT=GCFLAG.IEVENT+AMSmceventg::Orbit.Nskip;
   AMSmceventg::Orbit.Nskip=0;        
   AMSmceventg::Orbit.Ntot++;
  }
  else {
   number t2=
   AMSJob::Orbit.AlphaTanThetaMax*AMSJob::Orbit.AlphaTanThetaMax;

   number xsec=difftime(_time,AMSJob::Orbit.Begin);
   _NorthPolePhi=fmod(AMSJob::Orbit.PolePhi+
   AMSJob::Orbit.EarthSpeed*xsec,AMSDBc::twopi);

   number phil=
   atan2(sin(AMSJob::Orbit.PhiI-AMSJob::Orbit.PhiZero)*sqrt(1+t2),
   cos(AMSJob::Orbit.PhiI-AMSJob::Orbit.PhiZero));

   phil=fmod(phil+AMSJob::Orbit.AlphaSpeed*xsec,AMSDBc::twopi);
   number phi=atan2(sin(phil),cos(phil)*sqrt(1+t2));
   if(phi < 0)phi=phi+AMSDBc::twopi;
   _StationTheta=atan(AMSJob::Orbit.AlphaTanThetaMax*sin(phi));
   _StationPhi=fmod(phi+AMSJob::Orbit.PhiZero,AMSDBc::twopi);
  }
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

void AMSEvent::_sitrdinitevent(){
}



void AMSEvent::_retrdinitevent(){
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
}
//=====================================================================
void AMSEvent::_rectcinitevent(){
  integer i;
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCTCRawCluster",0),0));
  for(i=0;i<CTCDBc::getnlay();i++){
   AMSEvent::gethead()->add (
   new AMSContainer(AMSID("AMSContainer:AMSCTCCluster",i),0));
  }

}

void AMSEvent::_reaxinitevent(){
  integer i;
  AMSNode *ptr;
  for( i=0;i<npatb;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSBeta",i),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCharge",0),0));

  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSParticle",0),0));


  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AntiMatter",0),0));





}

void AMSEvent::_retkinitevent(){
  integer i;
  AMSNode *ptr;
  for(i=0;i<2;i++) AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",i),0));

  for( i=0;i<2;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),0));

  for( i=0;i<6;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",i),0));

  for( i=0;i<npat;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack",i),0));
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
   if(AMSJob::gethead()->isSimulation())_siamsevent();
      _reamsevent();
      if(AMSJob::gethead()->isCalibration())_caamsevent();
}
//------------------------------------------------------------------
void AMSEvent::_siamsevent(){
_sitkevent(); 
_sitrdevent();
_sitofevent(); 
_siantievent(); 
_sictcevent(); 
_sitrigevent(); 
if(TOFMCFFKEY.fast==0)_sidaqevent(); //DAQ-simulation only for slow algorithm
}

void AMSEvent::_reamsevent(){
#ifndef __AMSDEBUG__
  if(AMSJob::gethead()->isReconstruction() )_redaqevent();
#else
  if(TOFMCFFKEY.fast==0)_redaqevent();
#endif  
  _retofevent();
  _reantievent();
  _rectcevent(); 
  _retkevent(); 
  _retrdevent(); 
  _reaxevent();
}

void AMSEvent::_caamsinitevent(){
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTRD)_catrdinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CCerenkov)_cactcinitevent();
 if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxinitevent();
}

void AMSEvent::_catkinitevent(){
  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCalibration",0),0));
}

void AMSEvent::_catofinitevent(){
}

void AMSEvent::_catrdinitevent(){
}

void AMSEvent::_caaxinitevent(){
}

void AMSEvent::_cactcinitevent(){
}

void AMSEvent::_caamsevent(){
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTOF)_catofevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CCerenkov)_cactcevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)_catkevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CTRD)_catrdevent();
  if(AMSJob::gethead()->isCalibration() & AMSJob::CAMS)_caaxevent();
}

void AMSEvent::_catkevent(){
  AMSgObj::BookTimer.start("CalTrFill");
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
  AMSgObj::BookTimer.stop("CalTrFill");

}

void AMSEvent::_cactcevent(){
}
//---------------------------------------------------------------------------
void AMSEvent::_catofevent(){
  integer trflag(0);
  TriggerLVL1 *ptr;
//
  ptr=(TriggerLVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr)trflag=ptr->gettoflg();
  if(trflag <= 0)return;// use only H/W-triggered event
  if(TOFRECFFKEY.relogic[0]==2)
           TOFTDIFcalib::select();// event selection for TOF TDIF-calibration
  if(TOFRECFFKEY.relogic[0]==3)
           TOFTZSLcalib::select();// event selection for TOF TZSL-calibration
  if(TOFRECFFKEY.relogic[0]==4)
           TOFAMPLcalib::select();// event selection for TOF AMPL-calibration
}
//---------------------------------------------------------------------------
void AMSEvent::_catrdevent(){
}

void AMSEvent::_caaxevent(){
}

void AMSEvent::_retkevent(integer refit){

AMSgObj::BookTimer.start("RETKEVENT");
AMSgObj::BookTimer.start("TrCluster");
AMSTrCluster::build(refit);
AMSgObj::BookTimer.stop("TrCluster");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrCluster::print();
#endif
AMSgObj::BookTimer.start("TrRecHit");
AMSTrRecHit::build(refit);
AMSgObj::BookTimer.stop("TrRecHit");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrRecHit::print();
#endif
AMSgObj::BookTimer.start("TrTrack");
AMSTrTrack::build(refit);
AMSgObj::BookTimer.stop("TrTrack");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrTrack::print();
#endif
AMSgObj::BookTimer.stop("RETKEVENT");

if(refit==0 && AMSTrTrack::RefitIsNeeded())_retkevent(1);

}

void AMSEvent::_retrdevent(){
}
//---------------------------------------------------------------------
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
    AMSAntiRawEvent::validate(stat);// RawEvent-->RawEvent
    if(stat!=0)return;
    ANTIJobStat::addre(1);
//
    AMSAntiRawCluster::build(stat);// RawEvent->RawCluster
    if(stat!=0)return;
    ANTIJobStat::addre(2);
//
  }
//
  AMSAntiCluster::build();// RawCluster->Cluster
  ANTIJobStat::addre(3);
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
    if(trflag<=0)return;// "no h/w TOF-trigger"
    TOFJobStat::addre(1);
//
    if(AMSJob::gethead()->isSimulation() && TOFMCFFKEY.fast==1){
//
//                   ===> reco of events, simulated by fast MC :
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build(stat);    // "RawCluster-->Cluster"
      if(stat!=0)return;
      TOFJobStat::addre(4);
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
    }
    else{
//                   ===> reco of real events or simulated by slow MC:
//
//
      AMSgObj::BookTimer.start("TOF:validation");
      AMSTOFRawEvent::validate(stat);// RawEvent-->RawEvent
      AMSgObj::BookTimer.stop("TOF:validation");
      if(stat!=0)return;
      TOFJobStat::addre(2);
      AMSgObj::BookTimer.start("TOF:RwEv->RwCl");
      AMSTOFRawCluster::build(stat); // RawEvent-->RawCluster
      AMSgObj::BookTimer.stop("TOF:RwEv->RwCl");
      if(stat!=0)return;
      TOFJobStat::addre(3);
//
      AMSgObj::BookTimer.start("TOF:RwCl->Cl");
      AMSTOFCluster::build(stat);    // RawCluster-->Cluster
      AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
      if(stat!=0)return;
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
AMSgObj::BookTimer.start("RECTCEVENT");
AMSCTCCluster::build();
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSCTCCluster::print();
#endif
AMSgObj::BookTimer.stop("RECTCEVENT");
}

//========================================================================
void AMSEvent::_reaxevent(){
AMSgObj::BookTimer.start("REAXEVENT");
//
AMSBeta::build();
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSBeta::print();
#endif
AMSCharge::build();
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSCharge::print();
#endif
AMSParticle::build();
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSParticle::print();
#endif
//
AMSgObj::BookTimer.stop("REAXEVENT");
}


void AMSEvent::_sitkinitrun(){
}

void AMSEvent::_sitrdinitrun(){
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
    "AMSEvent::Retkinitrun-W-TRFITFFKEY.FastTracking selected."<<endl<<
    "Only AntiParticles proceesing will be  possible."<< endl<<
    "Wrong results will be produced if applied to normal particles."<<endl
    <<" "<<endl;
  }
}

void AMSEvent::_retrdinitrun(){
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

void AMSEvent:: _sitrdevent(){
}

void AMSEvent:: _siantievent(){
  int stat;
  AMSgObj::BookTimer.start("SIANTIEVENT");
  ANTIJobStat::addmc(0);
//
  if(TOFMCFFKEY.fast==0){//           ===> slow algorithm:
//
    AMSAntiRawEvent::mc_build(stat);// Geant_hit->RawEvent
    if(stat!=0)return;// no FTrigger from TOF - skip the rest
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


void AMSEvent:: _sitrigevent(){

  TriggerLVL1::build();
  TriggerLVL3::build();

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
   if(stat!=0)return; // no MC-trigger
   TOFJobStat::addmc(1);
  }
  else{    //                         ===> fast algorithm:
    TOFJobStat::addmc(0);
    AMSTOFRawCluster::sitofdigi(stat);//Geant_hit->RawCluster
    if(stat!=0)return; // no MC-trigger
    TOFJobStat::addmc(2);
  }

  AMSgObj::BookTimer.stop("SITOFDIGI");
#ifdef __AMSDEBUG__
  //  p =getC("AMSTOFRawEvent",0);
  //  if(p && AMSEvent::debug)p->printC(cout);
  p =getC("AMSTOFRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
  p=getC("AMSTOFMCCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}


void AMSEvent:: _sictcevent(){
   AMSCTCRawCluster::sictcdigi();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSCTCRawCluster",0);
  if(p && AMSEvent::debug>1)p->printC(cout);
#endif
}




AMSlink * AMSEvent::_getheadC( AMSID id, integer sorted){
  char *name=new char[13+strlen(id.getname())+1];
  name[0]='\0';
  strcat(name,"AMSContainer:");
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
  delete[] name;
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
  char *name=new char[13+strlen(id.getname())+1];
  name[0]='\0';
  strcat(name,"AMSContainer:");
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
  delete[] name;
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p){
    return p->getlast();
  }
  else return 0;
}

integer AMSEvent::_setheadC( AMSID id, AMSlink *head){
  char *name=new char[13+strlen(id.getname())+1];
  name[0]='\0';
  strcat(name,"AMSContainer:");
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
  delete[] name;
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  if(p && head){
   p->sethead(head);
   return 1;
  }
  else return 0;
}

AMSContainer * AMSEvent::_getC( AMSID  id){
  char *name=new char[13+strlen(id.getname())+1];
  name[0]='\0';
  strcat(name,"AMSContainer:");
  if(id.getname())strcat(name,id.getname());
  id.setname(name); 
  delete[] name;
  
  AMSContainer *p = (AMSContainer*)AMSEvent::gethead()->getp(id);
  return p;
}

integer AMSEvent::getnC(char n[]){
  AMSID id;
  char *name=new char[13+strlen(n)+1];
  strcpy(name,"AMSContainer:");
  if(n)strcat(name,n);
  id.setname(name); 
  delete[] name;
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
   ctime(&_time)<<" Theta "<<_StationTheta*AMSDBc::raddeg<<" Phi "<<_StationPhi*AMSDBc::raddeg<<
   " Pole "<<_NorthPolePhi*AMSDBc::raddeg<<endl;
}

void AMSEvent::_writeEl(){

  // fill the ntuple
static integer init=0;
static EventNtuple EN;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"EventH",EN.getaddress(),
  "EventNo:I*4, Run:I*4,  RunType:I*4 ,Time(2):I*4, PolePhi:R, ThetaS:R, PhiS:R, Particles:I, Tracks:I, Betas:I, Charges:I ,TrRecHits:I, TrClusters:I, TrMCClusters:I, TOFClusters:I, TOFMCClusters:I, CTCClusters:I, CTCMCClusters:I, AntiMCClusters:I, AntiClusters:I"); 

}
  EN.Event()=_id;
  EN.Run=_run;
  EN.RunType=_runtype;
  UCOPY(&_time,EN.Time,2*sizeof(integer)/4);
  EN.PolePhi=_NorthPolePhi;
  EN.ThetaS=_StationTheta;
  EN.PhiS=_StationPhi;
  integer  i,nc;
  AMSContainer *p;
  EN.Particles=0;
  EN.Tracks=0;
  EN.Betas=0;
  EN.Charges=0;
  EN.TrRecHits=0;
  EN.TrClusters=0;
  EN.TrMCClusters=0;
  EN.TOFClusters=0;
  EN.TOFMCClusters=0;
  EN.CTCClusters=0;
  EN.CTCMCClusters=0;
  EN.AntiClusters=0;
  EN.AntiMCClusters=0;

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSParticle",i);
   if(p) EN.Particles+=p->getnelem();
   else break;
  }  

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrTrack",i);
   if(p) EN.Tracks+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSBeta",i);
   if(p) EN.Betas+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCharge",i);
   if(p) EN.Charges+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrRecHit",i);
   if(p) EN.TrRecHits+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrCluster",i);
   if(p) EN.TrClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTrMCCluster",i);
   if(p) EN.TrMCClusters+=p->getnelem();
   else break;
  }
 
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFCluster",i);
   if(p) EN.TOFClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSTOFMCCluster",i);
   if(p) EN.TOFMCClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCTCCluster",i);
   if(p) EN.CTCClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSCTCMCCluster",i);
   if(p) EN.CTCMCClusters+=p->getnelem();
   else break;
  }

  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiCluster",i);
   if(p) EN.AntiClusters+=p->getnelem();
   else break;
  }
  
  for(i=0;;i++){
   p=AMSEvent::gethead()->getC("AMSAntiMCCluster",i);
   if(p) EN.AntiMCClusters+=p->getnelem();
   else break;
  }

  HFNTB(IOPA.ntuple,"EventH");

 
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

void AMSEvent::_validate(){

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
  integer nb=offspring->GetNbytes();
#ifdef __AMSDEBUG__
          char * tmp =new char[nb];
          assert(tmp !=NULL);
          integer ncp=offspring->CopyOut((uinteger*)tmp);
          ncp=offspring->CopyIn((uinteger*)tmp);
          delete[] tmp;
#endif
  if(offspring->validate(_time)){
    cout <<"AMSEvent::_validate-I-"<<offspring->getname()<<
      " validated. ("<<nb-sizeof(uinteger)<<" bytes ) CRC = "<<
      offspring->getCRC()<<endl;                                              
   }
    else {
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
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

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=(AMSTimeID*)ptid->down();
while(offspring){
    if (AMSJob::gethead()->isReadSetup()) {
     char *name=offspring -> getname();
     time_t I, B, E;
     time_t i, b, e;
     integer S;
     int rstat = AMSJob::FindTheBestTDV(name, _time, S, I, B, E); 
     if (rstat) {
       offspring -> gettime(i,b,e);
       if (i==I && b==B && e==E) {
#ifdef __AMSDEBUG__
         cout<<"AMSEvent::_validate-I- i,b,e are up to date"<<endl;
#endif
       } else {
         cerr<<"AMSEvent::_validate-W- i,b,e need to be updated "<<endl;
         cerr<<"Insert "<<ctime(&i);
         cerr<<"       "<<ctime(&I);
         cerr<<"Begin  "<<ctime(&b);
         cerr<<"       "<<ctime(&B);
         cerr<<"End    "<<ctime(&e);
         cerr<<"       "<<ctime(&E)<<endl;
         uinteger* buff = new uinteger[S];
         int rstatus = lms -> ReadTDV(name, I, B, E, buff);
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
*(p+1)=int16u(_Head->_run&65535);
*(p+2)=int16u((_Head->_run>>16)&65535);
*(p+3)=int16u(_Head->_runtype&65535);
uinteger _event=uinteger(_Head->_id);
*(p+4)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+6)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+8)=0;
}
void AMSEvent::buildTrackerHKdaq(integer i, integer length, int16u *p){

*p= getdaqid(4);
*(p+1)=int16u((_Head->_run+1)&65535);
*(p+2)=int16u(((_Head->_run+1)>>16)&65535);
*(p+3)=int16u(4&65535);
uinteger _event=uinteger(_Head->_id);
*(p+4)=int16u(_event&65535);
*(p+5)=int16u((_event>>16)&65535);
*(p+6)=int16u(_Head->_time&65535);
*(p+7)=int16u((_Head->_time>>16)&65535);
*(p+8)=0;
}


void AMSEvent::buildraw(
              integer length, int16u *p, uinteger & run, uinteger &id,
              uinteger &runtype, time_t & time){
  run=(*(p+1)) |  (*(p+2))<<16;
  runtype=*(p+3);
  id=(*(p+4)) |  (*(p+5))<<16;
  time=(*(p+6)) |  (*(p+7))<<16;

}


integer AMSEvent::checkdaqid(int16u id){
if(id==getdaqid(0))return 1;
else if(id==getdaqid(4))return 5 ;
else return 0;


}

integer AMSEvent::calcTrackerHKl(integer i){
static integer init =0;
if(!TRMCFFKEY.WriteHK || abs(++init-TRMCFFKEY.WriteHK-1) >1)return 0;
return 1+2+1+2+3;
}
