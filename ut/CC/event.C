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
#include <tofcalib.h>
#include <ntuple.h>
#include <ctcdbc.h>
#include <timeid.h>
//
AMSTOFScan scmcscan[SCBTPN];// some "temporary" TOF solution
TOFBrcal scbrcal[SCLRS][SCMXBR];// ...................
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
   _validate();
  }
  init();
}

void AMSEvent::_siamsinitrun(){
_sitkinitrun();
_sitofinitrun();
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
//    container for time_over_threshold hits (digi step):
//
  for(il=0;il<SCLRS;il++){
    ptr=AMSEvent::gethead()->add(
        new AMSContainer(AMSID("AMSContainer:AMSTOFTovt",il),0));
  }
//
//--- temporarily !!! here for Anti :
  ptr = AMSEvent::gethead()->add (
  new AMSContainer(AMSID("AMSContainer:AMSAntiMCCluster",0),0));
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
if(IOPA.mode ==2){
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
//========================================================================
void AMSEvent::_retofevent(){
int stat;
//
  AMSgObj::BookTimer.start("RETOFEVENT");
//
  if(TOFMCFFKEY.fast==0){
  TOFJobStat::addre(0);
  if(AMSJob::gethead()->getjobtype() == AMSFFKEY.Reconstruction){
    AMSgObj::BookTimer.start("TOF:DAQ->RwEv");
    AMSTOFRawEvent::re_build(stat);// DAQ-->RawEvent
    AMSgObj::BookTimer.stop("TOF:DAQ->RwEv");
    if(stat!=0)return;
    TOFJobStat::addre(1);
  }
//
  AMSgObj::BookTimer.start("TOF:RwEv->RwCl");
  AMSTOFRawCluster::build(stat); // RawEvent-->RawCluster
  AMSgObj::BookTimer.stop("TOF:RwEv->RwCl");
  if(stat!=0)return;
  TOFJobStat::addre(5);
//
  AMSgObj::BookTimer.start("TOF:RwCl->Cl");
  AMSTOFCluster::build(stat);    // RawCluster-->Cluster
  AMSgObj::BookTimer.stop("TOF:RwCl->Cl");
  if(stat!=0)return;
  TOFJobStat::addre(6);
//
  }
  else AMSTOFCluster::build();
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
if(TOFRECFFKEY.relogic[0]==1)
           TOFTZSLcalib::select();// event selection for TOF TZSL-calibration
//
AMSgObj::BookTimer.stop("REAXEVENT");
}


void AMSEvent::_sitkinitrun(){
}

void AMSEvent::_sitrdinitrun(){
}
//======================================================================
void AMSEvent::_sitofinitrun(){
 char fname[80];
 char name[10];
 int i,j,ila,ibr,ibrm,isp,nsp,ibt;
//
 float scp[SCANPNT];// scan points
 float rlo[SCANPNT];// relat.(to Y=0) light output
 integer lps=1000;
 integer nb;
 float nft,bl,bw;
 float arr[AMSDISL];
 float ef1[SCANPNT],ef2[SCANPNT];
 AMSDistr td1[SCANPNT];
 AMSDistr td2[SCANPNT];
 integer i1,i2;
 float r,eff1,eff2;  
//
//-----------------------------------------------------
// 
 if(AMSJob::gethead()->getjobtype() == AMSFFKEY.Simulation){ // For MC :
//
//   <--- read MC t-scan calibration files :
//
  for(ibt=1;ibt<=SCBTPN;ibt++){ // <-------- loop over bar types
    TOFDBc::brfnam(ibt,name);
    strcat(name,".dat");
    strcpy(fname,AMSDATADIR.amsdatadir);
    strcat(fname,name);
    cout<<"Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
    if(!tcfile){
      cerr <<"Sitofinitrun(Event.c): Error open file "<<fname<<endl;
      //exit(1);
      return;
    }
// <-- fill errays scp, bl1,bl2,ef1,ef2 from file
//
    tcfile >> nsp;// read # of calibr. points
    if(nsp!=SCANPNT){
      cerr<<"sitofinitrun: bad # of MC scan point ! "<<nsp<<'\n';
      exit(1);
    } 
    for(isp=0;isp<SCANPNT;isp++){ // sp. points loop to prepare arr. of t-distr
      tcfile >> scp[isp];
      tcfile >> nft;   // for PM-1
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef1[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td1[isp]=AMSDistr(nb,bl,bw,arr);
      tcfile >> nft;   // for PM-2
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef2[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td2[isp]=AMSDistr(nb,bl,bw,arr);
//
    } // <--- end of scan points loop -----
//
    scmcscan[ibt-1]=AMSTOFScan(scp,ef1,ef2,td1,td2);// create bar MC-t-scan obj
    tcfile.close(); // close file
//
  } //<---------- end of bar types loop ------
//
//----------------------
//
//   Fill bar MC-calibration parameters (TOFBrcal bank) :
//---> static variables :
//
  number slope=3.39;// tempor
//---
  number td2p[2]={16.9,1.8};// tempor timeD->coord conv.factor(cm/ns) and error(cm)
//---
  number logqin[SCACRFP]={ //  Log(inp_charge(mV*ns)) ref.points
     0.,0.98,1.94,2.9,3.86};               // (Log(Q_thresh) is subtracted)
//---
  number tovta[SCACRFP]={ // measured(Tovt) amplitude points
     0.,50.,100.,150.,200.};      //  ("anode" chain, ns)
//---
  number tovtd[SCACRFP]={ // measured(Tovt) amplitude points
     0.,50.,100.,150.,200.};      //  ("dinode" chain, ns)
//---
  number asatl=20.;//(mev,~10MIPs),if E-dinode(1-end) higher - use it instead
//                                 of anode measurements
//
  TOFBrcal::init(slope,td2p,logqin,tovta,tovtd,asatl);// fill static variables
//
//---> bar dependent variables :
//
  int sid,brt;
  number gna[2],gnd[2],qath,qdth,a2dr,tth,strat;
  number fstrd,tzer,mip2q;
  number tzero[SCBTPN]={4.20,5.02,5.57,5.89,6.03};// tempor T0 for bar types
//
  // read from file or DB:
    gna[0]=1.; // tempor
    gna[1]=1.;
    gnd[0]=1.;
    gnd[1]=1.;
    qath=TOFDBc::daqthr(3); // (pC) thresh. at shaper inp.(anode) (may be diff.
//              from TOFDBc::daqthr(3) in reality !!!, but proportional to him)
    qdth=TOFDBc::daqthr(4); // ...................        (dinode) ................
    mip2q=117.;//(pC/mev), dE(mev)_at_counter_center->Q(pC)_at_PM_anode(2x3-sum)
    //                     (depends on bar-type)
    a2dr=1./TOFDBc::di2anr();// anode_to_dinode signal ratio from MC
    tth=TOFDBc::daqthr(0); // (mV), time-discr. thresh.(not needed now)
    strat=TOFDBc::strrat(); // tempor stratcher ratio
    fstrd=TOFDBc::accdel(0)-TOFDBc::accdel(1);//(ns),fast-slow TDC trigger delay
    int mrfp;
    mrfp=SCANPNT/2+1;
    for(isp=0;isp<SCANPNT;isp++){ // fill 2-ends rel. l.output at scan-points
      rlo[isp]=(ef1[isp]+ef2[isp])/(ef1[mrfp]+ef2[mrfp]);
    }
  //
  for(ila=0;ila<SCLRS;ila++){
    ibrm=SCBRS[ila];
    for(ibr=0;ibr<ibrm;ibr++){
      sid=100*(ila+1)+(ibr+1);
      brt=TOFDBc::brtype(ila,ibr);
      tzer=tzero[brt-1];// tempor (later should be red from DB)
      scbrcal[ila][ibr]=TOFBrcal(sid,brt,gna,gnd,qath,qdth,a2dr,tth
                                ,strat,fstrd,tzer,mip2q,scp,rlo);
    }
  }
  //int16u vtes1(0);
  //  cerr<<"*int16u* test :"<<sizeof(vtes1)<<'\n';
  //int vtes2(0);
  //  cerr<<"*int* test :"<<sizeof(vtes2)<<'\n';
  //  integer vtes3(0);
  //  cerr<<"*integer* test :"<<sizeof(vtes3)<<'\n';
  //  float vtes4(0);
  //  cerr<<"*float* test :"<<sizeof(vtes4)<<'\n';
  //  number vtes5(0);
  //  cerr<<"*number* test :"<<sizeof(vtes5)<<'\n';
//
 } // <--- end of MC branch --
//---------------------------------------------------------------------
 else{ //                                                For Real Data :
 }
//----
}
//=====================================================================
void AMSEvent::_sictcinitrun(){
}
//=====================================================================
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

void AMSEvent::_rectcinitrun(){
}

void AMSEvent::_reaxinitrun(){
}

void AMSEvent:: _sitkevent(){
  if(TRMCFFKEY.NoiseOn)AMSTrMCCluster::sitknoise();
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
//
  AMSgObj::BookTimer.start("SITOFDIGI");
  if(TOFMCFFKEY.fast==0){
   AMSgObj::BookTimer.start("TOF:Ghit->Tovt");
   TOFJobStat::addmc(0);
   AMSTOFTovt::build(); // Geant_hits-->Tovt_hits
   AMSgObj::BookTimer.stop("TOF:Ghit->Tovt");
//
   AMSgObj::BookTimer.start("TOF:Tovt->RwEv");
   AMSTOFRawEvent::mc_build(); // Tovt_hits-->RawEvent_hits
   AMSgObj::BookTimer.stop("TOF:Tovt->RwEv");
  }
  else AMSTOFRawCluster::sitofdigi();

  AMSgObj::BookTimer.stop("SITOFDIGI");
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
 ctime(&_time)<<endl;
}

void AMSEvent::_writeEl(){

  // fill the ntuple
static integer init=0;
static EventNtuple EN;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"EventH",EN.getaddress(),
  "EventNo:I*4, Run:I*4,  RunType:I*4 ,Time(2):I*4, Particles:I, Tracks:I, Betas:I, Charges:I ,TrRecHits:I, TrClusters:I, TrMCClusters:I, TOFClusters:I, TOFMCClusters:I, CTCClusters:I, CTCMCClusters:I"); 

}
  EN.Event()=_id;
  EN.Run=_run;
  EN.RunType=_runtype;
  UCOPY(&_time,EN.Time,2*sizeof(integer)/4);
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
  char * tmp =new char[nb];
  assert(tmp !=NULL);
  integer ncp=offspring->CopyOut((void*)tmp);
  ncp=offspring->CopyIn((void*)tmp);
  
  delete[] tmp;
  if(offspring->validate(_time)){
    cout <<"AMSEvent::_validate-I-"<<offspring->getname()<<
      " validated. ("<<ncp<<" bytes )"<<endl;
  }
    else {
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Begin : " <<ctime(&b)<<endl; 
      cerr<<" End : " <<ctime(&e)<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<endl; 
      exit(1);
    }
    offspring=(AMSTimeID*)offspring->next();
}
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
#ifndef __BIGSTACK__
      // line below is very unsafe/risky one.
      // use __BIGSTACK__ together with "limit stack 20000" command to make safe recovery
      // 
      integer n=removeC();
      cerr <<"AMSEvent::Recovery-I- "<<n<<
      " containers succesfully nullified"<<endl;
#endif

      remove();
      cerr <<"AMSEvent::Recovery-I-Event structure removed"<<endl;
      sethead(0);
      UPool.erase(0);
      cerr <<"AMSEvent::Recovery-I-Memory pool released"<<endl;
      UPool.SetLastResort(10000);
      cerr <<"AMSEvent::Recovery-I-Cleanup done"<<endl;

}
