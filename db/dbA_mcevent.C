// May  05, 1996. ak. Add methods, objectivity first try
// July 29, 1996. ak. Modify AddEvent, split Add and Fill functions
// Aug    , 1996. ak. All time is lost in commit, so ReadStartEnd flag is added
//                    to CopyMCEvent, CopyEvent 
//                    Add function GetNEvents;
//                    Add WriteStartEnd to AddEvent
// Sep    , 1996. ak. Add Geometry, Materials and TMedia.
//                    remove fRunNumber class member from list.ddl
//                    set map per list, not map per dbase as before
// Nov    , 1996. ak. exit, if Addamsdbc fails
// Dec  16, 1996. ak. oocMROW if mode == oocREAD
// Feb    , 1997. ak. mceventItr, AddTDV
//                    no map anymore, use indexes
// Mar  18, 1997. ak. Getmceventg and GetNEvents are modified
//                    setup moved to AMSsetupDB
// last edit May 9, 1997, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <sys/times.h>
#include <unistd.h>

#include <ooIndex.h>

#include <typedefs.h>
#include <db_comm.h>
#include <dbA.h>

#include <eventTag.h>
#include <mcevent.h>

#include <mcanticlusterD_ref.h>
#include <mcctcclusterD_ref.h>
#include <mceventgD_ref.h>
#include <mctofclusterV_ref.h>
#include <tmcclusterV_ref.h>

#include <mcanticlusterD.h>
#include <mcctcclusterD.h>
#include <mceventgD.h>
#include <mctofclusterV.h>
#include <tmcclusterV.h>

#include <event.h>
#include <job.h>
#include <event.h>
#include <mccluster.h>
#include <mceventg.h>

static  Float_t  StartRealTime;
static  Float_t  EndRealTime;
static  long int gTicks;
static  integer NW_commit = 10;   // commit after each NW_commit events
static  integer NR_commit = 100;  // commit after each NR_commit events
static  integer NN_events;        // number of events written to the database
static  integer checkevent;
static  integer mcevent_size;

static  ooItr(AMSEventTag)     tageventItr;             

ooStatus LMS::AddMCEvent(ooHandle(AMSEventTag)&  tageventH,
                         uinteger run, uinteger eventNumber,
                         time_t time, integer runtype,
                         number pole, number stationT, number stationP)
                         
//
// listH         - pointer to the container to place event in
// run           - unique run number
// eventNumber   - event number
//

  {
    ooStatus rstatus = oocError;

    ooHandle(ooDBObj)  dbH = mcdb();
    if (dbH == NULL) Fatal("AddMCEvent : pointer to event database is NULL");


    ooHandle(AMSMCEventList)listH = mcCont();
    if (listH == NULL) Fatal("AddMCEvent : pointer to container NULL");


    ooHandle(AMSmcevent)     eventH;
    eventH = new(listH) AMSmcevent(run, eventNumber, time, runtype, 
                                   pole, stationT, stationP);

    rstatus = tageventH -> set_itsMCEvent(eventH);

    if (mceventg()) rstatus = listH -> Addmceventg(eventH);
    if (mcevents())
    {
     rstatus = listH -> Addantimccluster(eventH);
     rstatus = listH -> Addctcmccluster(eventH);
     rstatus = listH -> Addtofmccluster(eventH);
     rstatus = listH -> Addtrmccluster(eventH);
    }

    rstatus = oocSuccess;

end:
    return rstatus;
  }
ooStatus LMS::ReadMCEvents(uinteger& run, uinteger& eventn,
                           integer   nevents,
                           time_t& time, 
                           integer StartCommit)
//
// run           - unique run number
// eventn        - event number
// nevents       - number of events to read
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus = oocError;
    ooMode   mrowmode;
    integer                do_commit = 0;
    char                   pred[40];
    ooMode mode     = oocRead;


    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     mrowmode = mrowMode();
     StartRead(mrowmode);     
     NN_events = 0;
    }

    //get pointer to the database
    ooHandle(ooContObj)  tagContH = tagCont();
    if (tagContH == NULL) 
                   Fatal("ReadMCEvents : pointer to tag container is NULL");

    if (StartCommit == 1 || StartCommit == -2) { //init Itr if Start
      if (run < 1 ) {
        //(void) sprintf(pred,"_runUni>%d",run);
        (void) sprintf(pred,"_run>%d",run);
      } else {
        //if (run > 0) (void) sprintf(pred,"_runUni=%d",run,eventn);
       if (run > 0) (void) sprintf(pred,"_run=%d",run,eventn);
       if (eventn > 0) (void)
        // sprintf(pred,"_runUni=%d && _eventNumber >=%d",run,eventn);
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventn);
      }
      tageventItr.scan(tagContH, mode, oocAll, pred);
      cout<<"ReadMCEvents -I- scan event of "<<pred<<endl;
    }

    integer               runtype;
    number                pole, stationtheta, stationphi;
    ooItr(AMSmcevent)     eventItr;             
    ooItr(AMSmceventgD)   mceventgItr;          

    if (tageventItr.next() != NULL) {
     tageventItr -> itsMCEvent(eventItr);
     if (eventItr != NULL) {
       eventItr -> readEvent(run, eventn, time, runtype, pole,
                             stationphi, stationtheta);
       eventItr -> print();
       //create event
       AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
                    new AMSEvent(AMSID("Event",eventn),run,runtype,time,
                                 pole, stationtheta, stationphi)));
       if(mceventg()) {
       eventItr -> pmcEventg(mceventgItr);
       // read mceventg
       if (mceventgItr != NULL) {
        if (dbread_only != 0) {
         AMSmceventg* p = new AMSmceventg();
         mceventgItr -> copy(p);
         AMSEvent::gethead() -> addnext(AMSID("AMSmceventg",0),p);
         p -> InitSeed();
         p->run();
        }
         mcevent_size = mcevent_size + sizeof(AMSmceventgD);
        } else {
         Warning("ReadMCEvents : ptr to mceventg is NULL");
         rstatus = oocError;
         goto end;
        }
       }

       if(mcevents()) {
       // read trmccluster
        ooItr(AMSTrMCClusterV) trmcclusterItr;          
        eventItr -> pMCCluster(trmcclusterItr, mode);
        integer imcs=0;
        if (trmcclusterItr != NULL) {
         if (dbread_only != 0) {
          integer nelem = trmcclusterItr -> getnelem();
          if (nelem > 0) {
           for (int j = 0; j <nelem; j++) {
            AMSTrMCClusterD trmcclusterD = trmcclusterItr -> get(j);
            AMSTrMCClusterD* pD;
            pD = & trmcclusterD;
            AMSTrMCCluster* p = new AMSTrMCCluster(
                                                   pD -> getid(),
                                                   pD -> getxca(),
                                                   pD -> getxcb(),
                                                   pD -> getxgl(),
                                                   pD -> getsum(),
                                                   pD -> getitra()
                                                   );
            AMSEvent::gethead() -> addnext(AMSID("AMSTrMCCluster",0),p);
            mcevent_size = mcevent_size + sizeof(AMSTrMCClusterD);
           }
          }
         }
        } else {
         if (dbg_prtout == 1) 
            Message("ReadMCEvents : no Tracker MC clusters for this event");
        }
       // read tofmccluster
       ooItr(AMSTOFMCClusterV) tofmcclusterItr;        
       eventItr -> pTOFMCCluster(tofmcclusterItr, mode);
       if (tofmcclusterItr != NULL) {
        if (dbread_only != 0) {
         integer nelem = tofmcclusterItr -> getnelem();
         if (nelem > 0) {
          for (int j=0; j<nelem; j++) {
           AMSTOFMCClusterD tofmcclusterD = tofmcclusterItr -> get(j);
           AMSTOFMCClusterD* pD = & tofmcclusterD;
           AMSTOFMCCluster* p = new AMSTOFMCCluster(
                                                    pD -> getid(),
                                                    pD -> getxcoo(),
                                                    pD -> getedep(),
                                                    pD -> gettof()
                                                    );
            AMSEvent::gethead() -> addnext(AMSID("AMSTOFMCCluster",0),p);
            mcevent_size = mcevent_size + sizeof(AMSTOFMCClusterD);
           }
          }
         }
        } else {
         if (dbg_prtout == 1) 
            Message("ReadMCEvents : no TOF MC clusters for this event");
        }
       // read antimccluster
       ooItr(AMSAntiMCClusterD) AntiMCClusterItr;        
       eventItr -> pAntiMCCluster(AntiMCClusterItr, mode);
       imcs = 0;
       while (AntiMCClusterItr.next()) {
        if (dbread_only != 0) {
         AMSAntiMCCluster* p = new AMSAntiMCCluster(
                                       AntiMCClusterItr -> getid(),
                                       AntiMCClusterItr -> getcoo(),
                                       AntiMCClusterItr -> getedep(),
                                       AntiMCClusterItr -> gettof()
                                      );
         AMSEvent::gethead() -> addnext(AMSID("AMSAntiMCCluster",0),p);
        }
          imcs++;
       }
        mcevent_size = mcevent_size + sizeof(AMSAntiMCClusterD)*imcs;
        if (imcs == 0) 
            Message("ReadMCEvents : no Anti MC clusters for this event");
       // read ctcmccluster
       ooItr(AMSCTCMCClusterD) CTCMCClusterItr;        
       imcs=0;
       eventItr -> pCTCMCCluster(CTCMCClusterItr, mode);
       while (CTCMCClusterItr.next()) {
        if (dbread_only != 0) {
         AMSCTCMCCluster* p = new AMSCTCMCCluster(
                                               CTCMCClusterItr -> getid(),
                                               CTCMCClusterItr -> getxcoo(),
                                               CTCMCClusterItr -> getxdir(),
                                               CTCMCClusterItr -> getcharge(),
                                               CTCMCClusterItr -> getstep(),
                                               CTCMCClusterItr -> getbeta(),
                                               CTCMCClusterItr -> getedep()
                                               );
           AMSEvent::gethead() -> addnext(AMSID("AMSCTCMCCluster",0),p);
           imcs++;
        }
       }
       mcevent_size = mcevent_size + sizeof(AMSCTCMCClusterD)*imcs;
       if (imcs == 0) 
            Message("ReadMCEvents : no CTC MC clusters for this event");
       }
      if (dbg_prtout == 1)cout<<"Readmcevent:  MC event(s) size "
                               << mcevent_size<<endl;
      NN_events++;
     } else {
      Warning("ReadMCEvents : ptr to mcevent is NULL");
      rstatus = oocError;
      goto end;
     } 
     if (NN_events > nevents) do_commit = 1;
     if (NN_events%NR_commit == 0) {
       //do_commit = 1;
     }
    } else {
      do_commit = 1;
      if (NN_events == 0) 
                     Warning("ReadMCEvents: no events match to the query ");
    }

    rstatus = oocSuccess;

end:
    if (NN_events == 0) rstatus = oocError;
    if (rstatus == oocSuccess) {
     if (StartCommit == -1 || StartCommit == -2 || do_commit == 1) 
      {
       cout<<"NN_events "<<NN_events<<endl;
       Commit();
        if(NN_events%NR_commit == 0 && StartCommit > -1) {
         mrowmode = mrowMode();
         StartRead(mrowmode);
        }
      }
    } else {
     Commit();
    }
   return rstatus;
  }

