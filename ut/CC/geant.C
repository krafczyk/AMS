// Original program by V.Choutko, the date of creation is unknown
//
// Last Edit 
//           May      1996  ak, add OBJY part
//                              session manager is modified
//                              V106
//           July     1996  ak. V109
//                              add event to the database if trig = 1
//                              Read/Write keys
//                              UGLAST definition is added
//                              print ooRunStatus();
//           Aug      1996  ak. V1.24
//                              ReadStartEnd  =  1 start read transaction
//                             (WriteStartEnd)= -1 commit read transaction
//                                            = -2 read/commit
//           Sep      1996 ak.  V1.25, VArray of numbers
//                              call AddList at the beginning of the run
//                              The argument list of AddList is modified
//                              Count number of transactions
//                              Print dbase statistics at the end
//           Oct  02, 1996 ak.  modification in Read and Write cards 
//                              put AMSFFKey.Read/Write interpretation in 
//                              uginit, eventR, eventW
//                              use CCFFKEY.run  
//                              write geometry to dbase before event processing
//           Feb  14, 1997 ak.  AMSmceventD
//           Mar  19, 1997 ak.  see db_comm.h about new eventR/eventW
//                              call dbend from uglast
//                              eventRtype 
//           Oct    , 1997 ak.  TDV dbase version implementation
//           Nov    , 1997 ak.  TKDBc dbase version implementation
//
//  Last Edit: Nov 20, 1997. ak
//

#include <typedefs.h>

#include <cern.h>
#include <mceventg.h>
#include <amsgobj.h>
#include <commons.h>
#include <math.h>
#include <trid.h>
#include <new.h>
#include <limits.h>
#include <extC.h>
#include <job.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <gvolume.h>
#include <gmat.h>
#include <mccluster.h>
#include <event.h>
#include <cont.h>
#include <trrec.h>
#include <daqevt.h>
#include <iostream.h>

#ifdef __DB__

#include <db_comm.h>

integer trigEvents;               // number of events written to the DBase
integer notrigEvents;             // ALL events - trigEvents
integer readEvents;               // events read from the DBase
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
static  integer jobtype;          // AMSJob::jobtype()
#endif

PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)

#ifdef __DB__
  //class AMSEventList;
  //class AMSEventTagList;

#include <dbS.h>

// declaration ooVArray(...)
#include <commonsD_ref.h>     
#include <commonsD.h>

#include <dbcatalog_ref.h>
#include <dbcatalog.h>

#include <ooVArray_uint16.h>

#include <tkdbcV_ref.h>
#include <tkdbcV.h>


#include <gmatD_ref.h>     
#include <gmatD.h>


  //#include <list_ref.h>
  //#include <list.h>
  //
  //#include <listTag_ref.h>
  //#include <listTag.h>

// -
implement (ooVArray, uint16)
implement (ooVArray, geant)   
implement (ooVArray, integer) 
implement (ooVArray, ooRef(ooDBObj)) 
implement (ooVArray, TKDBcD) 
  //implement (ooVArray, ooRef(ooAMSEventList)) 
  //implement (ooVArray, ooRef(ooAMSEventTagList)) 

LMS	               dbout;
LMS*                   lms;

#endif

void uhinit(){
  
  if(IOPA.hlun){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    integer iostat;
    integer rsize=1024;
    HROPEN(IOPA.hlun,"output",hfile,"N",rsize,iostat);
    if(iostat){
     cerr << "Error opening Histo file "<<hfile<<endl;
     exit(1);
    }
    else cout <<"Histo file opened."<<endl;
  }
   HBOOK1(200101,"Number of Nois Hits x",100,-0.5,99.5,0.);
   HBOOK1(200102,"Number of Nois Hits y",100,-0.5,99.5,0.);
   HBOOK1(200103,"Normal Spectrum  x",200,-50.5,49.5,0.);
   HBOOK1(200104,"Normal Spectrum y",200,-50.5,49.5,0.);
   HBOOK1(200105,"Above threshold spectrum x",200,-0.5,49.5,0.);
   HBOOK1(200106,"Above threshold spectrum y",200,-0.5,49.5,0.);
   HBNT(IOPA.ntuple,"Simulation"," ");
}
extern "C" void uginit_(){
  GINIT();
  AMSJob::sethead() = new AMSJob();
  AMSJob::gethead()->data();
  GFFGO();
   AMSJob::gethead()->udata();
  // Geant initialization
  GZINIT();
  GPART();
  GPIONS(4);
#ifdef __DB__
   initDB();
   lms = &dbout;
   readSetup();
   if ((AMSFFKEY.Read%2) == 1) lms -> CheckConstants();
#else
   AMSgmat::amsmat();
   AMSgvolume::amsgeom();
#endif
  AMSJob::map();
  AMSJob::gethead()->init();
  AMSJob::map(1);
#ifndef __BATCH__
GDINIT();
#endif

#ifdef __DB__
  if ((AMSFFKEY.Read%2) == 1) dbout.CheckCommons();
  writeSetup();
  int n = AMSJob::gethead()->FillTDVTable();
  ooStatus rstatus = dbout.FillTDV(n);
  if (rstatus != oocSuccess) cerr<<"uginit_ -W- FillTDV"<<endl;
  AMSJob::gethead() -> seteventRtype(eventR);
#endif

  GPHYSI();
  uhinit();
}


extern "C" void gustep_(){
  //  cout <<" gustep in"<<endl;
  try{

  //  Tracker

  if(GCVOLU.nlevel >1 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
  GCVOLU.names[1][0]== 'S' &&     GCVOLU.names[1][1]=='T' && 
  GCVOLU.names[1][2]=='K')
     AMSTrMCCluster::sitkhits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   

  // TOF

  geant t,x,y,z;
  char name[5]="dumm";
  char media[21]="dummy_media         ";
  geant de,dee,dtr2,div,tof;
  static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
  geant trcut2(0.1);// Max. transv.shift (0.316cm)**2
  geant vect[3],dx,dy,dz,dt; 
  int i,nd,numv,iprt,numl;
  static int numvo(-999),iprto(-999);
  if(GCVOLU.nlevel >1 && GCVOLU.names[1][0]== 'T' && GCVOLU.names[1][1]=='O' &&
  GCVOLU.names[1][2]=='F' && GCVOLU.names[1][3]=='S'){// in "TOFS"
    iprt=GCKINE.ipart;
    numv=GCVOLU.number[GCVOLU.nlevel-1];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
    if(GCTRAK.inwvol==1){// new volume or track : store param.
      iprto=iprt;
      numvo=numv;
      xpr=x;
      ypr=y;
      zpr=z;
      tpr=t;
    }
    else{
      if(iprt==iprto && numv==numvo && de!=0.){// same part. in the same volume
        dx=(x-xpr);
        dy=(y-ypr);
        dz=(z-zpr);
        dt=(t-tpr);
        dtr2=dx*dx+dy*dy;
//
        if(dtr2>trcut2){// too big transv. shift: subdivide step
          nd=integer(sqrt(dtr2/trcut2));
          nd+=1;
          dx=dx/geant(nd);
          dy=dy/geant(nd);
          dz=dz/geant(nd);
          dt=dt/geant(nd);
          GCTRAK.destep=de/geant(nd);
          for(i=1;i<=nd;i++){//loop over subdivisions
            vect[0]=xpr+dx*(i-0.5);
            vect[1]=ypr+dy*(i-0.5);
            vect[2]=zpr+dz*(i-0.5);
            tof=tpr+dt*(i-0.5);
            dee=GCTRAK.destep;
            if(TOFMCFFKEY.birks)GBIRK(dee);
            AMSTOFMCCluster::sitofhits(numv,vect,dee,tof);
          }
        }
        else{
          vect[0]=xpr+0.5*dx;
          vect[1]=ypr+0.5*dy;
          vect[2]=zpr+0.5*dz;
          tof=tpr+0.5*dt;
          dee=GCTRAK.destep;
          if(TOFMCFFKEY.birks)GBIRK(dee);
          AMSTOFMCCluster::sitofhits(numv,vect,dee,tof);
        }// end of "big step" test
//
        xpr=x;
        ypr=y;
        zpr=z;
        tpr=t;
      }// end of "same part/vol, de>0"
    }// end of new volume test
  }// end of "in TOFS"
//-------------------------------------

  // CTC
  if(GCVOLU.nlevel >1 && GCKINE.charge != 0  && GCTRAK.destep != 0 && 
  GCTMED.isvol != 0 && 
  ((GCVOLU.names[1][0]== 'A' && GCVOLU.names[1][1]=='G' && 
   GCVOLU.names[1][2]=='E' && GCVOLU.names[1][3]=='L') ||
   (GCVOLU.names[1][0]== 'W' && GCVOLU.names[1][1]=='L' && 
   GCVOLU.names[1][2]=='S' && GCVOLU.names[1][3]==' ') ))

     AMSCTCMCCluster::sictchits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep, GCTRAK.tofg);

  // CTC Annecy fast

  if(GCVOLU.nlevel >2 && GCKINE.charge != 0  && GCTRAK.destep != 0 && 
  GCTMED.isvol != 0 && 
  ((GCVOLU.names[2][0]== 'A' && GCVOLU.names[2][1]=='G' && 
   GCVOLU.names[2][2]=='L') ||
   (GCVOLU.names[2][0]== 'P' && GCVOLU.names[2][1]=='M' && 
   GCVOLU.names[2][2]=='T' ) ))

     AMSCTCMCCluster::sictchits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep,GCTRAK.tofg);


  // ANTI,  mod. by E.C.
  numl=GCVOLU.nlevel;
  numv=GCVOLU.number[numl-1];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  cout<<"Volume "<<name<<" number="<<numv<<" level="<<numl<<endl;
//  iprt=GCKINE.ipart;
//  x=GCTRAK.vect[0];
//  y=GCTRAK.vect[1];
//  z=GCTRAK.vect[2];
//  t=GCTRAK.tofg;
//  de=GCTRAK.destep;
//  cout<<"Part="<<iprt<<" x/y/z="<<x<<" "<<y<<" "<<z<<endl;
//  UHTOC(GCTMED.natmed,4,media,20);
//  cout<<" Media "<<media<<endl;
  int manti(0);
  if(numl==3 && GCVOLU.names[numl-1][0]== 'A' && GCVOLU.names[numl-1][1]=='N'
                                       && GCVOLU.names[numl-1][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
     GBIRK(dee);
     AMSAntiMCCluster::siantihits(numv,GCTRAK.vect,dee,GCTRAK.tofg);
  }
  GSKING(0);
#ifndef __BATCH__
  GSXYZ();
#endif
  }
  
   catch (AMSuPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
     AMSEvent::gethead()->Recovery();
     return;
   }
   catch (AMSaPoolError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
    AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
    cerr << "GUSTEP  "<< e.getmessage();
    GCTRAK.istop =1;
   }
   //  cout <<" gustep out"<<endl;
}
//-----------------------------------------------------------------------
extern "C" void guout_(){

#ifdef __DB__
   if (dbg_prtout != 0 && eventR > DBWriteGeom) {
     cout <<"guout_: read event of type "<<AMSJob::gethead() -> eventRtype()
          <<" from dbase"<<endl;
    if (AMSJob::gethead() -> isMCBanks()) cout <<"guout_: MCBanks exist"<<endl;
    if (AMSJob::gethead() -> isRecoBanks()) 
                                        cout <<"guout_: RecoBanks exist"<<endl;
   }
#endif

   if(AMSJob::gethead()->isSimulation())
   AMSgObj::BookTimer.stop("GEANTTRACKING");

   try{
          AMSEvent::gethead()->event();
   }
   catch (AMSuPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSaPoolError e){
     cerr << e.getmessage()<<endl;
     AMSEvent::gethead()->Recovery();
      return;
   }
   catch (AMSTrTrackError e){
     cerr << e.getmessage()<<endl;
     cerr <<"Event dump follows"<<endl;
     AMSEvent::gethead()->printA(0);
     UPool.Release(0);
     AMSEvent::gethead()->remove();
     UPool.Release(1);
     AMSEvent::sethead(0);
      UPool.erase(0);
      return;
   }
      if(GCFLAG.IEVENT%GCFLAG.ITEST==0 ||     GCFLAG.IEORUN || GCFLAG.IEOTRI || 
         GCFLAG.IEVENT==GCFLAG.NEVENT)
      AMSEvent::gethead()->printA(AMSEvent::debug);
     integer trig;
     if(AMSJob::gethead()->gettriggerOr()){
      trig=0;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trig+=p->getnelem();
         else break;
        }
       }
     }
     else{
      trig=1;
     integer ntrig=AMSJob::gethead()->gettriggerN();
       for(int n=0;n<ntrig;n++){
        integer trigl=0;
        for(int i=0; ;i++){
         AMSContainer *p=AMSEvent::gethead()->
         getC(AMSJob::gethead()->gettriggerC(n),i);
         if(p)trigl+=p->getnelem();
         else break;
        }
        if(trigl==0)trig=0;
       }
     }

   if(trig ){ 
     AMSEvent::gethead()->write();
     AMSEvent::gethead()->copy();
   }

#ifdef __DB_All__
//+
   if(trig) {
    if ( eventW > DBWriteGeom) {
     integer run   = AMSEvent::gethead() -> getrun();
     char* jobname = AMSJob::gethead()->getname();
     integer event = AMSEvent::gethead() -> getEvent();
     time_t  time  = AMSEvent::gethead() -> gettime();
     integer rtype = AMSEvent::gethead() -> getruntype();
     number pole, stationT, stationP;
     AMSEvent::gethead() -> GetGeographicCoo(pole, stationT, stationP);
     integer WriteStartEnd = 0;
     if (trigEvents == 0 && AMSFFKEY.Read < 10)     WriteStartEnd = 1;
      if(dbout.recoevents() || dbout.mcevents() || dbout.mceventg())
        dbout.AddEvent(run, event, time, rtype, pole, stationT, stationP, 
                       WriteStartEnd);
      trigEvents++;
    } 
    if (dbg_prtout != 0 && eventW < DBWriteGeom) cout <<
         "GUOUT - I - AMSFFKEY.Write = 0, no database action "<<endl;
   } else {
     notrigEvents++;
   }
//-
#endif

     UPool.Release(0);
   AMSEvent::gethead()->remove();
     UPool.Release(1);
   AMSEvent::sethead(0);
   UPool.erase(2000000);
}

extern "C" void gukine_(){

static integer event=0;



#ifdef __DB_All__
  if (AMSFFKEY.Read > 1) {
    readDB();
     return;
  }
#endif
// create new event & initialize it
  if(AMSJob::gethead()->isSimulation()){
    AMSgObj::BookTimer.start("GEANTTRACKING");
   if(IOPA.mode !=1 ){
    AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
    new AMSEvent(AMSID("Event",GCFLAG.IEVENT),CCFFKEY.run,0)));
    for(integer i=0;i<CCFFKEY.npat;i++){
    AMSmceventg* genp=new AMSmceventg(GCFLAG.NRNDM);
    if(genp){
     AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
     genp->run(GCKINE.ikine);
    }
    }
   }
   else {
    AMSIO io;
    if(io.read()){
     AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
     new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime(),
     io.getpolephi(),io.getstheta(),io.getsphi())));
     AMSmceventg* genp=new AMSmceventg(io);
     if(genp){
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
      genp->run();
     }
    }
    else{
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return;
    }   
   }
  }
  else {
    //
    // read daq    
    //
    DAQEvent * pdaq=0;
    for(;;){
      pdaq = new DAQEvent();
      if(!(pdaq->read()))break;
      AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
      new AMSEvent(AMSID("Event",pdaq->eventno()),pdaq->runno(),
      pdaq->runtype(),pdaq->time())));
      AMSEvent::gethead()->addnext(AMSID("DAQEvent",pdaq->GetBlType()), pdaq);
      guout_();
      if(GCFLAG.IEOTRI || GCFLAG.IEVENT >= GCFLAG.NEVENT)break;
      GCFLAG.IEVENT++;
    }
     GCFLAG.IEORUN=1;
     GCFLAG.IEOTRI=1;
     return; 
  }   
}


extern "C" void uglast_(){
       GLAST();
       if( CCFFKEY.low ==2)CMEND();
#ifdef __DB__
     dbout.dbend();
#endif
       delete AMSJob::gethead();
}
//------------------------------------------------------------------------------------
extern "C" void readDB(){

#ifdef __DB_All__

ooHandle(AMSEventList) listH;
ooStatus               rstatus;

  integer nevents = GCFLAG.NEVENT;
  char* jobname   = AMSJob::gethead()->getname();
  char* setup     = AMSJob::gethead() -> getsetup();
  uinteger run    = SELECTFFKEY.Run;         // run number to read
  integer  eventF = SELECTFFKEY.Event;       // event number to start from
  uinteger eventn;
  time_t   time;

   if (eventF > 0 ) {
     eventn = eventF;
   } else {
    eventn       = 0;
   }
   if (run < 0)     run = 0;

    ReadStartEnd = 0; 
    integer nST = dbout.nTransStart();
    integer nCT = dbout.nTransCommit();
    integer nAT = dbout.nTransAbort();
    number  pole, stationT, stationP;
    integer rtype;
    if (readEvents == 0) AMSJob::gethead() -> seteventRtype(eventR);
    if ((readEvents == 0) || (nST == (nCT + nAT))) ReadStartEnd = 1;
    if (dbout.recoevents()) {
     for (;;) {
      rstatus = dbout.ReadEvents(run, eventn, nevents, time, ReadStartEnd);
      if (rstatus != oocSuccess) break;
      ReadStartEnd = 0;
     }
    }
    if (dbout.mcevents() || dbout.mceventg())
      rstatus =dbout.ReadMCEvents(run, eventn, nevents, time, ReadStartEnd);
   if (rstatus == oocSuccess) {
    readEvents++;
    if ((eventR < DBWriteMC) || dbout.mceventg()) {
       eventNu++;
       return; }
   }
   UGLAST();
   exit(0);
#endif
}

extern "C" void writeSetup(){

#ifdef __DB__
     if ( (AMSFFKEY.Write%2) == 1) {
      integer eventW = AMSFFKEY.Write;
      char* jobname = AMSJob::gethead()->getname();
      char* setup = AMSJob::gethead() -> getsetup();
      cout <<"uglast -I- geometry and setup will be written to database"<<endl;
      cout <<"           for the job "<<jobname<<endl;
      cout <<"           with setup  "<<setup<<endl;
      dbout.AddMaterial();
      dbout.AddTMedia();
      dbout.AddGeometry();
      dbout.Addamsdbc();
      dbout.AddAllTDV();
     }
#endif
}

extern "C" void readSetup(){

#ifdef __DB__

  if ( (AMSFFKEY.Read%2) == 0) {
   AMSgmat::amsmat();
   AMSgvolume::amsgeom();
  }  
  if ((AMSFFKEY.Read%2) == 1)   {
   dbout.ReadMaterial();
   GPMATE(0);
   dbout.ReadTMedia();
   GPTMED(0);
   //AMSgmat::amsmat();
   //AMSgvolume::amsgeom();
   dbout.ReadGeometry();
   ooStatus rstatus = dbout.ReadTKDBc();
   if (rstatus != oocSuccess) {
     cerr<<"readSetup -E- Error in ReadTKDBc"<<endl;
     exit(0);
   }
  }
#endif
}

extern "C" void initDB() 
{
#ifdef __DB__
   ooMode   mode;
   ooMode   mrowmode;
   eventR = AMSFFKEY.Read;
   eventW = AMSFFKEY.Write;
   if (eventW > 0) mode = oocUpdate;
   if (eventW < 1) {
     mode = oocRead;
     mrowmode = oocMROW;
   }
   char* jobname = AMSJob::gethead()-> getname();
   char* setup     = AMSJob::gethead() -> getsetup();
   int   jobtype   = AMSJob::gethead() -> AMSJob::jobtype();
   cout <<"_uginit -I- LMS init for job     "<<jobname<<endl;
   cout <<"                         setup   "<<setup<<endl;
   cout <<"                         jobtype "<<jobtype<<endl;
   dbout.setapplicationName(jobname);
   dbout.setprefix(jobname);
   dbout.setsetup(setup);
   dbout.setjobtype(jobtype);
   dbout.settypeR(eventR);
   dbout.settypeW(eventW);
   dbout.ClusteringInit(mode,mrowmode);
#endif
}
