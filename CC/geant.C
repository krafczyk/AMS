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
//           Sep      1996 ak.  AMSFFKEY.Read =  1 read events
//                              AMSFFKEY.Read = 10 read setup
//                              V1.25, VArray of numbers
//                              call AddList at the beginning of the run
//                              The argument list of AddList is modified
//                              Count number of transactions
//                              Print dbase statistics at the end
//           Oct  02, 1996 ak.  modification in Read and Write cards 
//                              put AMSFFKey.Read/Write interpretation in 
//                              uginit, eventR, eventW
//           Oct  09, 1996 ak.  use CCFFKEY.run  
//                              write geometry to dbase before event processing
//           Feb  14, 1997 ak.  AMSmceventD
//           Mar  19, 1997 ak.  see db_comm.h about new eventR/eventW
//                              call dbend from uglast
//                              eventRtype 
//  Last Edit: Mar 24, 1997. ak
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

#include <iostream.h>

#ifdef __DB__

#include <db_comm.h>

integer trigEvents;               // number of events written to the DBase
integer notrigEvents;             // ALL events - trigEvents
integer readEvents;               // events read from the DBase
integer interactive = 0;          //
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
#endif

PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)

#ifdef __DB__
class AMSEventList;

#include <A_LMS.h>

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;

LMS	               dbout(oocUpdate);

void lmsStart(integer run, integer event, char* jobname, integer WriteStartEnd)
{
  if(!interactive)dbout.AddEvent
                       (jobname,run,event,WriteStartEnd,eventW);
  if (interactive) dbout.Interactive();
}
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
   HBOOK1(101,"Number of Nois Hits x",100,-0.5,99.5,0.);
   HBOOK1(102,"Number of Nois Hits y",100,-0.5,99.5,0.);
   HBOOK1(103,"Normal Spectrum  x",200,-50.5,49.5,0.);
   HBOOK1(104,"Normal Spectrum y",200,-50.5,49.5,0.);
   HBOOK1(105,"Above threshold spectrum x",200,-0.5,49.5,0.);
   HBOOK1(106,"Above threshold spectrum y",200,-0.5,49.5,0.);
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
   ooMode   mode;
   ooMode   mrowmode;
   eventR = AMSFFKEY.Read;
   eventW = AMSFFKEY.Write;
   char* jobname = AMSJob::gethead()->getname();
   if (eventW > 0) mode = oocUpdate;
   if (eventW < DBWriteGeom) {
     mode = oocRead;
     mrowmode = oocMROW;
   }
   if (dbg_prtout) cout <<"_uginit -I- LMS init for job "<<jobname<<endl;
   if ((eventW/DBWriteMCEv)%2 == 1 || (eventR/DBWriteMCEv)%2 == 1) 
                              dbout.LMSInitS(mode,mrowmode,jobname);
   if ((eventW/DBWriteMCEv)%2 != 1 && (eventR/DBWriteMCEv)%2 != 1) 
                              dbout.LMSInit(mode,mrowmode,jobname);
   readGeomDB();
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
  writeGeomDB();
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
  geant de,dee,dtr2,div,tof;
  static geant xpr(0.),ypr(0.),zpr(0.),tpr(0.);
  geant trcut2(0.09);// Max. transv.shift (0.3cm)**2
  int i,nd,numv,iprt;
  static int numvo(-999),iprto(-999);
  if(GCVOLU.names[1][0]== 'T' && GCVOLU.names[1][1]=='O' &&
  GCVOLU.names[1][2]=='F' && GCVOLU.names[1][3]=='S'){// in "TOFS"
    iprt=GCKINE.ipart;
    numv=GCVOLU.number[GCVOLU.nlevel-1];
    x=GCTRAK.vect[0];
    y=GCTRAK.vect[1];
    z=GCTRAK.vect[2];
    t=GCTRAK.tofg;
    de=GCTRAK.destep;
    if(GCTRAK.inwvol==1){// new volume or track : stor param.
      iprto=iprt;
      numvo=numv;
      xpr=x;
      ypr=y;
      zpr=z;
      tpr=t;
    }
    else{
      if(iprt==iprto && numv==numvo && de!=0.){// same part. in the same volume
        dtr2=(x-xpr)*(x-xpr)+(y-ypr)*(y-ypr);
        if(dtr2>trcut2){// too big transv. shift: subdivide step
          nd=(integer)sqrt(dtr2/trcut2);
          nd+=1;
	  geant vect[3]; 
          for(i=1;i<=nd;i++){//loop over subdivisions
            div=geant(i)/geant(nd);
            vect[0]=xpr+(x-xpr)*div;
            vect[1]=ypr+(y-ypr)*div;
            vect[2]=zpr+(z-zpr)*div;
            GCTRAK.destep=de/geant(nd);
            tof=tpr+(t-tpr)*div;
            GBIRK(dee);
            AMSTOFMCCluster::sitofhits(numv,vect,dee,tof);
          }
        }
        else{
          GBIRK(dee);
          AMSTOFMCCluster::sitofhits(numv,GCTRAK.vect,dee,t);
        }
      }// end of "same part/vol, de>0"
    }
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
     GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep);

  // CTC Annecy fast

  if(GCVOLU.nlevel >2 && GCKINE.charge != 0  && GCTRAK.destep != 0 && 
  GCTMED.isvol != 0 && 
  ((GCVOLU.names[2][0]== 'A' && GCVOLU.names[2][1]=='G' && 
   GCVOLU.names[2][2]=='L') ||
   (GCVOLU.names[2][0]== 'P' && GCVOLU.names[2][1]=='M' && 
   GCVOLU.names[2][2]=='T' ) ))

     AMSCTCMCCluster::sictchits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     GCKINE.charge,GCTRAK.step, GCTRAK.getot,GCTRAK.destep);


  // ANTI     

  if(GCVOLU.nlevel >1 && GCTRAK.destep != 0  && GCTMED.isvol != 0 && 
  GCVOLU.names[1][0]== 'A' && GCVOLU.names[1][1]=='S' 
  && GCVOLU.names[1][2]=='C'){
     geant dee;
     GBIRK(dee);
     AMSAntiMCCluster::siantihits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     dee,GCTRAK.tofg);
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
     AMSEvent::gethead()->remove();
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
#ifdef __DB__
//+
   if(trig) {
    if ( eventW > DBWriteGeom) {
     integer run   = AMSEvent::gethead() -> getrun();
     run = CCFFKEY.run;
     char* jobname = AMSJob::gethead()->getname();
     integer event = AMSEvent::gethead() -> getEvent();
     integer WriteStartEnd = 0;
     if (trigEvents == 0 && AMSFFKEY.Read < 10)     WriteStartEnd = 1;
     lmsStart(run, event, jobname, WriteStartEnd);
     trigEvents++;
    } 
    if (dbg_prtout != 0 && eventW < DBWriteGeom) cout <<
         "GUOUT - I - AMSFFKEY.Write = 0, no database action "<<endl;
   } else {
     notrigEvents++;
   }
//-
#endif
   AMSEvent::gethead()->remove();
   AMSEvent::sethead(0);
   UPool.erase(2000000);
   //   cout <<" guout out "<<endl; 
}

extern "C" void gukine_(){

static integer event=0;
time_t         Time;

   time(&Time);

   AMSgObj::BookTimer.start("GEANTTRACKING");

#ifdef __DB__
  if (AMSFFKEY.Read > 1) {
    readDB();
     return;
  }
#endif
// create new event & initialize it
  if(IOPA.mode !=1){
  AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
  new AMSEvent(AMSID("Event",++event),CCFFKEY.run,0,Time)));
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
    new AMSEvent(AMSID("Event",io.getevent()),io.getrun(),0,io.gettime())));
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
#ifdef __AMSDEBUG__
   AMSContainer *p = AMSEvent::gethead()->getC("AMSmceventg",0);
   if(p && AMSEvent::debug)p->printC(cout);
#endif
}

//-----------------------------------------------------------------------------
extern "C" void uglast_(){
       GLAST();
#ifdef __DB__
     dbout.dbend(eventR, eventW);
#endif
       delete AMSJob::gethead();
}
//------------------------------------------------------------------------------------
extern "C" void readDB(){

#ifdef __DB__

ooHandle(AMSEventList) listH;
ooStatus               rstatus;
struct       tms       cpt;
Float_t                StartRealTime;
Float_t                EndRealTime;

  integer nevents = GCFLAG.NEVENT;
  ooMode   mode     = oocRead;
  ooMode   mrowmode = oocMROW;
  char* jobname   = AMSJob::gethead()->getname();
  char* setup     = AMSJob::gethead() -> getsetup();
  long int gTicks = sysconf(_SC_CLK_TCK);
  integer run     = SELECTFFKEY.Run;         // run number to read
  integer eventF  = SELECTFFKEY.Event;       // event number to start from
  if (AMSFFKEY.Write) mode = oocUpdate;
   StartRealTime = times(&cpt);
   if (eventF > 0 ) {
     eventNu = eventF;
   } else {
    eventNu       = 0;
   }
   if (run < 0)     run = 0;
   if ( (eventR > DBWriteMCEg+1)  && (eventR/DBWriteMCEv)%2 != 1) {
    AMSJob::gethead() -> seteventRtype(eventR);
    rstatus = dbout.GetNEvents
            (jobname, run, eventNu, nevents, mode, mrowmode, eventR);
   } else {
    ReadStartEnd = 0; 
    integer nST = dbout.getNTransStart();
    integer nCT = dbout.getNTransCommit();
    integer nAT = dbout.getNTransAbort();

    if (readEvents == 0) AMSJob::gethead() -> seteventRtype(eventR);
    if ((readEvents == 0) || (nST == (nCT + nAT))) ReadStartEnd = 1;
    if ( (eventR/DBWriteMCEg)%2 == 1)
    rstatus =dbout.Getmceventg
              (jobname, run, eventNu, mode, mrowmode, ReadStartEnd);
    if ( (eventR/DBWriteMCEv)%2 == 1)
    rstatus =dbout.GetmceventD
              (jobname, run, eventNu, mode, mrowmode, ReadStartEnd);
    readEvents++;
   }
   if (rstatus == oocSuccess) {
    EndRealTime = times(&cpt);
    if (dbg_prtout) cout <<"elapsed time (EndRealTime - StartRealTime) "
         <<(EndRealTime - StartRealTime)<<" "<<gTicks<<" sec."
         <<endl;
     if ((eventR < DBWriteMC) || (eventR/DBWriteMCEv)%2 == 1) {
       eventNu++;
       return; }
   } 

   UGLAST();
   exit(0);
#endif
}

extern "C" void writeGeomDB(){

#ifdef __DB__
     if ( (AMSFFKEY.Write%2) == 1) {
      integer eventW = AMSFFKEY.Write;
      char* jobname = AMSJob::gethead()->getname();
      char* setup = AMSJob::gethead() -> getsetup();
      cout <<"uglast_ -I- geometry and setup will be saved to database"<<endl;
      cout <<"            for the job with name "<<jobname<<endl;
      ooHandle(AMSEventList) listH;
      ooStatus rstatus = oocSuccess;
      rstatus = dbout.AddList(jobname,eventW,listH);
      dbout.AddMaterial(jobname);
      dbout.AddTMedia(jobname);
      dbout.AddGeometry(jobname);
      dbout.Addamsdbc(jobname);
      dbout.AddTDV(jobname);
     }
#endif
}

extern "C" void readGeomDB(){

#ifdef __DB__

  char* jobname = AMSJob::gethead()->getname();
  char* setup = AMSJob::gethead() -> getsetup();
  ooHandle(AMSEventList) listH;
  ooStatus rstatus = oocSuccess;
  if (eventW < DBWriteGeom) {
   if (eventR) {
    rstatus = dbout.Start(oocRead);
    rstatus = dbout.FindEventList(jobname,oocRead,listH);
    if (rstatus != oocSuccess) {
     rstatus = dbout.Commit();
      cout <<"uginit_ -E- cannot open list with name "<<jobname<<endl;
       exit(1);
    } 
      integer nn = listH -> getNEvents();
      if (nn < GCFLAG.NEVENT && eventR > 1) {
        cout <<"uginit_ -W- there are "<<nn<<" events in the list "
             <<jobname<<endl;
        cout<<"uginit_ -W- GCFLAG.NEVENT will be set to "<<nn<<endl;
      }
    rstatus = dbout.Commit();
   }
  } else {
   rstatus = dbout.AddList(jobname,eventW,listH);
  }

  if ( (AMSFFKEY.Read%2) == 0) {
    cout <<"uginit_ -I- geometry and setup will be created now "<<endl;
    cout <<"            for the job with name "<<jobname<<endl;
    cout <<"jobname length "<<strlen(jobname)<<endl;
   AMSgmat::amsmat();
   AMSgvolume::amsgeom();
  } 
  if ((AMSFFKEY.Read%2) == 1)   {
   dbout.CheckConstants();
   cout <<"uginit_ -I- geometry and setup will be read from database"<<endl;
   cout <<"            for the job with name "<<jobname<<endl;
   dbout.FillMaterial(jobname);
   GPMATE(0);
   dbout.FillTMedia(jobname);
   GPTMED(0);
   dbout.FillGeometry(jobname);
  }
#endif
}
