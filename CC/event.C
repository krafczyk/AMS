// Author V. Choutko 24-may-1996
// TOF parts changed 25-sep-1996 by E.Choumilov.
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
#include <ntuple.h>

//
AMSTOFScan scmcscan[SCBTPN];// some "temporary" TOF solution
//
integer AMSEvent::debug=1;
AMSEvent* AMSEvent::_Head=0;
AMSNodeMap AMSEvent::EventMap;
integer AMSEvent::SRun=0;
void AMSEvent::_init(){
  // check old run & 
  if(_run != SRun){
   SRun=_run;
   cout <<" AMS-I-New Run "<<_run<<endl;
   if(AMSJob::gethead()->getjobtype() == AMSFFKEY.Simulation)_siamsinitrun();
   _reamsinitrun();
  }
  init();
}

void AMSEvent::_siamsinitrun(){
_sitkinitrun();
// _sitofinitrun();
_sictcinitrun();
_sitrdinitrun();
}

void AMSEvent::_reamsinitrun(){
_retkinitrun();
_retrdinitrun();
_retofinitrun();
_rectcinitrun();
_reaxinitrun();
}

void AMSEvent::init(){
  // Initialize containers
if(AMSJob::gethead()->getjobtype() == AMSFFKEY.Simulation)_siamsinitevent();
_reamsinitevent();
}

void AMSEvent::_siamsinitevent(){
 _signinitevent();
 _sitkinitevent();
 _sitrdinitevent();
 _sitofinitevent();
 _sictcinitevent();
}

void AMSEvent::_reamsinitevent(){
 _retkinitevent();
 _retrdinitevent();
 _retofinitevent();
 _rectcinitevent();
 _reaxinitevent();
}

void AMSEvent::_signinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSmceventg",0),0));
}

void AMSEvent::_sitkinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}

//====================================================================
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
//    container for photoelectron hits (digi step):
//
  for(il=0;il<SCLRS;il++){
    ptr=AMSEvent::gethead()->add(
        new AMSContainer(AMSID("AMSContainer:AMSTOFPhel",il),0));
  }
//    container for time_over_threshold hits (digi step):
//
  for(il=0;il<SCLRS;il++){
    ptr=AMSEvent::gethead()->add(
        new AMSContainer(AMSID("AMSContainer:AMSTOFTovt",il),0));
  }
//
// container for RawEvent hits(same structure for MC/REAL events) :
//
  ptr=AMSEvent::gethead()->add(
      new AMSContainer(AMSID("AMSContainer:AMSTOFRawEvent",0),0));
//
//--- temporarily !!! here for Anti :
  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
}
//====================================================================

void AMSEvent::_sictcinitevent(){
  AMSNode *ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCTCMCCluster",0),0));

  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
}

void AMSEvent::_sitrdinitevent(){
}

void AMSEvent::_retrdinitevent(){
}
//======================================================================
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
  for( i=0;i<4;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTOFCluster",i),0));
}
//=====================================================================
void AMSEvent::_rectcinitevent(){
  integer i;
  AMSNode *ptr=  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCTCRawCluster",0),0));

  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSCTCCluster",0),0));


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


}

void AMSEvent::_retkinitevent(){
  integer i;
  AMSNode *ptr=  AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0));

  for( i=0;i<2;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrCluster",i),0));

  for( i=0;i<6;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",i),0));

  for( i=0;i<npat;i++)  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSTrTrack",i),0));
}

void  AMSEvent::write(){
  // Sort before by "Used" variable : AMSTrTrack & AMSTrCluster
  AMSEvent::gethead()->getheadC("AMSTrCluster",0,1); 
  AMSEvent::gethead()->getheadC("AMSTrCluster",1,1); 
  AMSEvent::gethead()->getheadC("AMSTrTrack",0,1); 
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
   if(strncmp(cur->getname(),"AMSContainer:",13)==0)((AMSContainer*)cur)->
   printC(cout);
 }
 else break;
}
}
else{
_printEl(cerr);
if(debugl==0)return;
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


void AMSEvent::event(){
   if(AMSJob::gethead()->getjobtype() == AMSFFKEY.Simulation)_siamsevent();
      _reamsevent();
}

void AMSEvent::_siamsevent(){
_sitkevent(); 
_sitrdevent(); 
_sitofevent(); 
_sictcevent(); 
}

void AMSEvent::_reamsevent(){
  _retofevent(); 
  _rectcevent(); 
  _retkevent(); 
  _retrdevent(); 
  _reaxevent();
}

void AMSEvent::_retkevent(){

AMSgObj::BookTimer.start("RETKEVENT");
AMSgObj::BookTimer.start("TrCluster");
AMSTrCluster::build();
AMSgObj::BookTimer.stop("TrCluster");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrCluster::print();
#endif
AMSgObj::BookTimer.start("TrRecHit");
AMSTrRecHit::build();
AMSgObj::BookTimer.stop("TrRecHit");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrRecHit::print();
#endif
AMSgObj::BookTimer.start("TrTrack");
AMSTrTrack::build();
AMSgObj::BookTimer.stop("TrTrack");
#ifdef __AMSDEBUG__
if(AMSEvent::debug)AMSTrTrack::print();
#endif
AMSgObj::BookTimer.stop("RETKEVENT");


}

void AMSEvent::_retrdevent(){
}
//========================================================================
void AMSEvent::_retofevent(){
//
AMSgObj::BookTimer.start("RETOFEVENT");
// AMSTOFRawEvent::re_build();// DAQ-->RawEvent
// AMSTOFRawCluster::build(); // RawEvent-->RawClaster
AMSTOFCluster::build();    // RawCluster-->Cluster
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


void AMSEvent::_reaxevent(){
AMSgObj::BookTimer.start("REAXEVENT");
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
AMSgObj::BookTimer.stop("REAXEVENT");
}


void AMSEvent::_sitkinitrun(){
}

void AMSEvent::_sitrdinitrun(){
}
//======================================================================
void AMSEvent::_sitofinitrun(){
 char fname[30];
 char name[10];
 int i,j,isp,ibt;
//  for test :
 number x(-2.5);// impact point
 number scpt[SCANPNT]={-10.,0.,10.};// test scan points
 integer nbt(10);
 number blt(0.);
 number bwt(1.);
 number art[10]={0.,0.,1.,2.,5.,5.,5.,3.,1.,0.};
 number eft[SCANPNT]={0.5,0.7,1.};
 geant dummy(-1), val, rnd;
//
 number scp[SCANPNT];// scan points
 integer lps=1000;
 integer nb;
 number bl;
 number bw;
 number arr[AMSDISL];
 number ef1[SCANPNT],ef2[SCANPNT];
 AMSDistr td1[SCANPNT];
 AMSDistr td2[SCANPNT];
 integer i1,i2;
 number r,eff1,eff2;  
//
//
cout <<"enter in test  " <<'\n';
//
//   <--- loop over bar types and read MC t-scan calibration files
//
  for(ibt=1;ibt<=SCBTPN;ibt++){
    TOFDBc::brfnam(ibt,name);
    strcat(name,".dat");
    strcpy(fname,"/AMS/CALIBR/");
    strcat(fname,name);
    cout<<"Open file : "<<fname<<'\n';
//    ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
//    if(!tcfile){
//      cerr <<"Sitofinitrun(Event.c): Error open file "<<fname<<endl;
//      exit(1);
//    }
// <-- fill errays scp, bl1,bl2,ef1,ef2 from file
// 
    for(isp=0;isp<SCANPNT;isp++){ // loop over sp. points to prepare arr. of t-distr
//      tcfile >> scp[isp];
//      tcfile >> nb;   // for PM-1
//      tcfile >> bl;
//      tcfile >> bw;
//      tcfile >> ef1[i];
      scp[isp]=scpt[isp];
      nb=nbt;
      bl=blt;
      bw=bwt;
      ef1[isp]=eft[isp];
      ef2[isp]=eft[isp];
      for(i=0;i<nb;i++){arr[i]=art[i];}
//      for(i=0;i<nb;i++){arr[i]=0.;}
//      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td1[isp]=AMSDistr(nb,bl,bw,arr);
//      tcfile >> nb;   // for PM-2
//      tcfile >> bl;
//      tcfile >> bw;
//      tcfile >> ef2[i];
//      for(i=0;i<nb;i++){arr[i]=0.;}
//      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td2[isp]=AMSDistr(nb,bl,bw,arr);
    } // <--- end of scan points loop -----
    scmcscan[ibt-1]=AMSTOFScan(scp,ef1,ef2,td1,td2);// create paddle MC-t-scan obj
//    tcfile.close(); // close file
  } //<---------- end of bar types loop ------
//
//        <--- test created objects
//
 ibt=1.;
 scmcscan[ibt].getxbin(x,i1,i2,r);
 cout<<"i1="<<i1<<"  i2="<<i2<<"  r=="<<r<<'\n';
 eff1=scmcscan[ibt].getef1(r,i1,i2);
 eff2=scmcscan[ibt].getef2(r,i1,i2);
 cout<<"eff1="<<eff1<<"  eff2="<<eff2<<'\n';
//
 HBOOK1(1002,"Distribution PM1",20,0.,20.,0.);
 HBOOK1(1003,"Distribution PM2",20,0.,20.,0.);
 for(j=0;j<lps;j++)
  {
    rnd=RNDM(dummy);
    val=scmcscan[ibt].gettm1(rnd,r,i1,i2);
    HF1(1002,val,1.);
    rnd=RNDM(dummy);
    val=scmcscan[ibt].gettm2(rnd,r,i1,i2);
    HF1(1003,val,1.);
  }
 HPRINT(1002);
 HPRINT(1003);
//
//      test bitstream stuff :
 unsigned short int ain[3]={0,0,0};
 unsigned short int bin[3]={0x000F,0xFFFF,0xF000};
 unsigned short int cin[3]={0xF00F,0x0F0F,0xF00F};
 AMSBitstr aa(3);
 aa.setbit(8,23);
 aa.display("aa set");
 aa.clrbit(16,23);
 aa.display("clr-bit aa");
 aa.clrbit(2,1);
 aa.display("clr aa");
 aa.setbit(2,1);
 aa.display("all bits aa");
 AMSBitstr b(3,bin);
 b.display("b");
 AMSBitstr c(3,cin);
 c.display("c");
 AMSBitstr a(c);
 a.display("a(c)");
 aa.clrbit(2,1);
 aa = c;
 aa.display("aa=c");
// AMSBitstr a;
// a.display("cr. a");
 AMSBitstr aaa;
 aaa.clrbit(1,0);
 aaa=b|c;
 aaa.display("aaa=b|c");
 aaa.setbit(1,0);
 aaa.display("aaa all bits");
 aaa=b&c;
 aaa.display("aaa=b&c");
}
//=====================================================================
void AMSEvent::_sictcinitrun(){
}
//=====================================================================
void AMSEvent::_retkinitrun(){
}

void AMSEvent::_retrdinitrun(){
}

void AMSEvent::_retofinitrun(){
}

void AMSEvent::_rectcinitrun(){
}

void AMSEvent::_reaxinitrun(){
}

void AMSEvent:: _sitkevent(){
    AMSTrMCCluster::sitknoise();
  AMSTrMCCluster::sitkcrosstalk();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSTrMCCluster",0);
  if(p && AMSEvent::debug)p->printC(cout);
#endif
    AMSTrRawCluster::sitkdigi();
#ifdef __AMSDEBUG__
  p =getC("AMSTrRawCluster",0);
  if(p && AMSEvent::debug)p->printC(cout);
#endif
}

void AMSEvent:: _sitrdevent(){
}
//===================================================================
void AMSEvent:: _sitofevent(){
  AMSContainer *p;
//  AMSTOFPhel::build(); // Geant_hits-->Phel_hits
//  AMSTOFTovt::build(); // Phel_hits-->Tovt_hits
//  AMSTOFRawEvent::mc_build(); // Tovt_hits-->RawEvent_hits
  AMSTOFRawCluster::sitofdigi();
#ifdef __AMSDEBUG__
  //  p =getC("AMSTOFRawEvent",0);
  //  if(p && AMSEvent::debug)p->printC(cout);
  p =getC("AMSTOFRawCluster",0);
  if(p && AMSEvent::debug)p->printC(cout);
  p=getC("AMSTOFMCCluster",0);
  if(p && AMSEvent::debug)p->printC(cout);
  p=getC("AMSAntiMCCluster",0);
  if(p&& AMSEvent::debug)p->printC(cout);
#endif
}
//===================================================================
void AMSEvent:: _sictcevent(){
   AMSCTCRawCluster::sictcdigi();
#ifdef __AMSDEBUG__
  AMSContainer *p =getC("AMSCTCRawCluster",0);
  if(p && AMSEvent::debug)p->printC(cout);
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
    return p->gethead();
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

void AMSEvent::_copyEl(){
}

void AMSEvent::_printEl(ostream & stream){
 stream << "Run "<<_run<<" "<<getname()<<" "<< getid()<<" Time "<< 
 ctime(&_time)<<endl;
}

void AMSEvent::_writeEl(){

  // fill the ntuple
static integer init=0;
static EventNtuple EN;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"EventH",EN.getaddress(),
  "EventNo:I*4, Run:I*4,  RunType:I*4 ,Time(2):I*4"); 

}
  EN.Event()=_id;
  EN.Run=_run;
  EN.RunType=_runtype;
  UCOPY(&_time,EN.Time,2*sizeof(integer)/4);
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
