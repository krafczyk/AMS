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
//                                  1     -  read(write) setup
//                                 10     -              mceventg
//                                100     -              all MC banks
//                               1000     -              all banks
//                              put AMSFFKey.Read/Write interpretation in 
//                              uginit, eventR, eventW
//           Oct  09, 1996 ak.  use CCFFKEY.run  
//           Oct  16, 1996 ak.  write geometry to dbase before event processing
//
//  Last Edit: Oct 23, 1996. ak
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
#include <tofdbc.h>
#include <iostream.h>
#include <tofcalib.h>



#ifdef __DB__
integer trigEvents;               // number of events written to the DBase
integer notrigEvents;             // ALL events - trigEvents
integer readEvents;               // events read from the DBase
integer interactive = 0;          //
static  integer dbg_prtout;       // debug printout flag
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
#endif

PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)

#ifdef __DB__
class AMSEventList;

//#include <typedefs.h>
#include <A_LMS.h>

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;

LMS	               dbout(oocUpdate);

void lmsStart(integer run, integer event, char* jobname, integer WriteStartEnd)
{
  char        eventID[256];
  char        runID[256];
  integer     runD   =  10000;
  integer     eventD = 100000;
  ostrstream  oss, oss1;

  oss <<eventD+event << ends;
  strcpy(eventID,oss.str());
  oss1 <<runD+run << ends;
  strcpy(runID,oss1.str());
  strcat(runID, eventID);

  if(!interactive)dbout.AddEvent
                       (jobname,run,event,runID,WriteStartEnd,eventW);
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
   if (eventW < 1) {
     mode = oocRead;
     mrowmode = oocMROW;
   }
   if (dbg_prtout) cout <<"_uginit -I- LMS init for job "<<jobname<<endl;
   dbout.LMSInit(mode,mrowmode,jobname);
   readGeomDB();
#else
   AMSgmat::amsmat();
   AMSgvolume::amsgeom();
#endif
  AMSJob::gethead()->init();
  AMSJob::map();
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
  try{

  //  Tracker

  if(GCVOLU.nlevel >1 && GCTRAK.destep != 0 && GCTMED.isvol != 0 && 
  GCVOLU.names[1][0]== 'S' &&     GCVOLU.names[1][1]=='T' && 
  GCVOLU.names[1][2]=='K')
     AMSTrMCCluster::sitkhits(GCVOLU.number[GCVOLU.nlevel-1],GCTRAK.vect,
     GCTRAK.destep,GCTRAK.step,GCKINE.ipart);   


//-------------------------------------------
  // ---- TOF ----
  
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
	  geant vect[3],destep; 
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
}

extern "C" void guout_(){

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
#ifdef __AMSDEBUG__
      if(GCFLAG.IEVENT%GCFLAG.ITEST==0)
      AMSEvent::gethead()->printA(AMSEvent::debug);
#endif
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
    if ( eventW > 1) {
     integer run   = AMSEvent::gethead() -> getrun();
     run = CCFFKEY.run;
     char* jobname = AMSJob::gethead()->getname();
     integer event = AMSEvent::gethead() -> getEvent();
     integer WriteStartEnd = 0;
     if (trigEvents == 0 && AMSFFKEY.Read < 10)     WriteStartEnd = 1;
     lmsStart(run, event, jobname, WriteStartEnd);
     trigEvents++;
    } 
    if (dbg_prtout != 0 && eventW < 1) cout <<
         "GUOUT - I - AMSFFKEY.Write = 0, no database action "<<endl;
   } else {
    if (dbg_prtout) cout <<"GUOUT - I - trig =0, no database action "<<endl;
     notrigEvents++;
   }
//-
#endif
   AMSEvent::gethead()->remove();
   AMSEvent::sethead(0);
   UPool.erase(2000000);
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


extern "C" void uglast_(){
#ifdef __DB__
//+
  if (eventR + eventW > 0) {
    integer nST = dbout.getNTransStart();
    integer nCT = dbout.getNTransCommit();
    integer nAT = dbout.getNTransAbort();
    if (nST > nCT + nAT) {
     cout <<"uglast_ -W- Number of started transactions  "<<nST<<endl;
     cout <<"uglast_ -W- Number of commited transactions "<<nCT<<endl;
     cout <<"uglast_ -W- Number of aborted transactions  "<<nAT<<endl;
     //if (dbout.getTransLevel() != 0) {
      cout <<"uglast_ -I- commit an active transaction "<<endl;
      cout <<"uglast_ -I- transaction nesting level "
           <<dbout.getTransLevel()<<endl;
      dbout.Commit();   // Commit transaction
      //}
    }
  }
    ooHandle(AMSEventList) listH;
    char* jobname   = AMSJob::gethead()->getname();
    ooStatus          rstatus;
    rstatus = dbout.Start(oocRead);
    rstatus = dbout.FindEventList(jobname,oocRead,listH);
    if (rstatus == oocSuccess && dbg_prtout !=0 )  
                              listH -> PrintMapStatistics(oocRead);
    rstatus = dbout.Commit();
//-
#endif
       GLAST();
//--------> some TOF stuff :
       TOFJobStat::print(); // Print JOB-TOF statistics
       if(TOFMCFFKEY.mcprtf[2]!=0){ // tempor! print MC-hists
         HPRINT(1050);
         HPRINT(1051);
         HPRINT(1052);
         HPRINT(1053);
         HPRINT(1060);
         HPRINT(1061);
         HPRINT(1062);
         HPRINT(1063);
         HPRINT(1070);
         HPRINT(1071);
         HPRINT(1072);
#ifdef __AMSDEBUG__
#endif
       }
       if(TOFRECFFKEY.reprtf[2]!=0){ // tempor! print RECO-hists
         HPRINT(1100);
         HPRINT(1101);
         HPRINT(1102);
         HPRINT(1526);
         HPRINT(1527);
         HPRINT(1528);
         HPRINT(1529);
         HPRINT(1530);
         HPRINT(1531);
         HPRINT(1532);
         HPRINT(1533);
         HPRINT(1534);
         HPRINT(1535);
         HPRINT(1536);
         HPRINT(1537);
         HPRINT(1538);
         HPRINT(1539);
         HPRINT(1540);
         HPRINT(1541);
         HPRINT(1542);
         if(TOFRECFFKEY.relogic[0]==1){// for calibr. runs
           HPRINT(1500);
           HPRINT(1501);
           HPRINT(1502);
           HPRINT(1503);
           HPRINT(1504);
           HPRINT(1505);
           HPRINT(1506);
           HPRINT(1508);
           HPRINT(1509);
           HPRINT(1510);
           HPRINT(1511);
           HPRINT(1512);
           HPRINT(1513);
           HPRINT(1514);
           HPRINT(1515);
           HPRINT(1516);
           HPRINT(1517);
           HPRINT(1518);
           HPRINT(1519);
           HPRINT(1520);
           HPRINT(1521);
           HPRINT(1522);
           HPRINT(1523);
           HPRINT(1524);
           HPRINT(1525);
           TOFTZSLcalib::mfit();
         }
       }
//--------
       //       HPRINT(0);
       if(IOPA.hlun){
        char hpawc[256]="//PAWC";
        HCDIR (hpawc, " ");
        char houtput[9]="//output";
        HCDIR (houtput, " ");
        integer ICYCL=0;
        HROUT (0, ICYCL, " ");
        HREND ("output");
       }
#ifndef __DB__
       if(AMSJob::debug){
         // AMSJob::gethead()->printN(cout);
          AMSNode* p=AMSJob::gethead()->getnodep(AMSID("Materials:",0));
          assert(p!=NULL);
          p=AMSJob::gethead()->getnodep(AMSID("TrackingMedia:",0));
          assert(p!=NULL);
          AMSgvolume * pg=AMSJob::gethead()->getgeom();
          assert(pg!=NULL);
       }
#endif
#ifdef __DB__
     if (dbg_prtout) ooRunStatus();
#endif
}

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
  char* eventID   = NULL;
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
   if ( (eventR/10)%2 != 1) {
    rstatus = dbout.GetNEvents
            (jobname, eventID, run, eventNu, nevents, mode, mrowmode, eventR);
   } else {
    ReadStartEnd = 0; 
    integer nST = dbout.getNTransStart();
    integer nCT = dbout.getNTransCommit();
    integer nAT = dbout.getNTransAbort();
    if ((readEvents == 0) || (nST == (nCT + nAT))) ReadStartEnd = 1;
    rstatus =dbout.Getmceventg
              (jobname, eventID, run, eventNu, mode, mrowmode, ReadStartEnd);
    readEvents++;
   }
   if (rstatus == oocSuccess) {
    EndRealTime = times(&cpt);
    if (dbg_prtout) cout <<"elapsed time (EndRealTime - StartRealTime) "
         <<(EndRealTime - StartRealTime)<<" "<<gTicks<<" sec."
         <<endl;
     if ((eventR/10)%2 == 1) {
       eventNu++;
       return; }
   } 

   UGLAST();
   //return;
   exit(0);
#endif
}

extern "C" void writeGeomDB(){

#ifdef __DB__
     if ( (AMSFFKEY.Write%2) == 1) {
      char* jobname = AMSJob::gethead()->getname();
      char* setup = AMSJob::gethead() -> getsetup();
      cout <<"uglast_ -I- geometry and setup will be saved to database"<<endl;
      cout <<"            for the job with name "<<jobname<<endl;
      ooHandle(AMSEventList) listH;
      ooStatus rstatus = oocSuccess;
      rstatus = dbout.AddList(jobname,setup,0,listH);
      dbout.AddMaterial(jobname);
      dbout.AddTMedia(jobname);
      dbout.AddGeometry(jobname);
      dbout.Addamsdbc(jobname);
     }
#endif
}

extern "C" void readGeomDB(){

#ifdef __DB__

  char* jobname = AMSJob::gethead()->getname();
  char* setup = AMSJob::gethead() -> getsetup();
  ooHandle(AMSEventList) listH;
  ooStatus rstatus = oocSuccess;
  if (eventW < 1) {
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
   rstatus = dbout.AddList(jobname,setup,0,listH);
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














