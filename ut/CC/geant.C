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
//  Last Edit: Jun 02, 1997. ak
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
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
static  integer jobtype;          // AMSJob::jobtype()
#endif

PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)

#ifdef __DB__
class AMSEventList;

#include <dbA.h>

// declaration ooVArray(...)
#include <commonsD_ref.h>     
#include <commonsD.h>

#include <mctofclusterV_ref.h>
#include <mctofclusterV.h>

#include <tmcclusterV_ref.h>
#include <tmcclusterV.h>

#include <anticlusterV_ref.h>
#include <anticlusterV.h>


//#include <timeidD_ref.h>
//#include <timeidD.h>

// -

implement (ooVArray, geant)   
implement (ooVArray, number)  
implement (ooVArray, integer) 
  //implement (ooVArray, uinteger) 
implement (ooVArray, AMSAntiClusterD) 
implement (ooVArray, AMSTOFMCClusterD) 
implement (ooVArray, AMSTrMCClusterD) 
implement (ooVArray, ParticleS) 

LMS	               dbout;

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
   initDB();
   readSetup();
   if ((AMSFFKEY.Read%2) == 1) dbout.CheckConstants();
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
  geant trcut2(0.09);// Max. transv.shift (0.3cm)**2
  int i,nd,numv,iprt,numl;
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


  // ANTI,  mod. by E.C.
  numl=GCVOLU.nlevel;
//  numv=GCVOLU.number[numl-1];
//  for(i=0;i<4;i++)name[i]=GCVOLU.names[numl-1][i];
//  cerr<<"Volume "<<name<<" number="<<numv<<" level="<<numl<<endl;
//  iprt=GCKINE.ipart;
//  x=GCTRAK.vect[0];
//  y=GCTRAK.vect[1];
//  z=GCTRAK.vect[2];
//  t=GCTRAK.tofg;
//  de=GCTRAK.destep;
//  cerr<<"Part="<<iprt<<" x/y/z="<<x<<" "<<y<<" "<<z<<endl;
//  UHTOC(GCTMED.natmed,4,media,20);
//  cerr<<" Media "<<media<<endl;
  int manti(0);
  if(numl==3 && GCVOLU.names[numl-1][0]== 'A' && GCVOLU.names[numl-1][1]=='N'
                                       && GCVOLU.names[numl-1][2]=='T')manti=1;
  if(GCTRAK.destep != 0  && GCTMED.isvol != 0 && manti==1){
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

#ifdef __DB__
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
#ifdef __AMSDEBUG__
   AMSContainer *p = AMSEvent::gethead()->getC("AMSmceventg",0);
   if(p && AMSEvent::debug)p->printC(cout);
#endif
}

//-----------------------------------------------------------------------------
extern "C" void uglast_(){
       GLAST();
#ifdef __DB__
     dbout.dbend();
#endif
       delete AMSJob::gethead();
}
//------------------------------------------------------------------------------------
extern "C" void readDB(){

#ifdef __DB__

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
      cout <<"            with setup "<<setup<<endl;
      dbout.AddMaterial();
      dbout.AddTMedia();
      dbout.AddGeometry();
      dbout.Addamsdbc();
      dbout.AddTDV();
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
    //dbout.CheckConstants();
   dbout.ReadMaterial();
   GPMATE(0);
   dbout.ReadTMedia();
   GPTMED(0);
   dbout.ReadGeometry();
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
   if (eventW < DBWriteGeom) {
     mode = oocRead;
     mrowmode = oocMROW;
   }
   char* jobname = AMSJob::gethead()-> getname();
   char* setup     = AMSJob::gethead() -> getsetup();
   cout <<"_uginit -I- LMS init for job "<<jobname<<endl;
   cout <<"                       setup "<<setup<<endl;
   dbout.setapplicationName(jobname);
   dbout.setprefix(jobname);
   dbout.setsetup(setup);
   dbout.settypeR(eventR);
   dbout.settypeW(eventW);
   dbout.ClusteringInit(mode,mrowmode);
#endif
}
