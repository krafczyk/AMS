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
// last edit Apr 10, 1997, ak.
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
#include <raweventD.h>
#include <recevent.h>

#include <event.h>
#include <job.h>


static  Float_t  StartRealTime;
static  Float_t  EndRealTime;
static  long int gTicks;
static  integer NN_events;        // number of events written to the database
static  integer NW_commit = 100;  // commit after each NW_commit events
static  integer NR_commit = 200;  // commit after each NR_commit events
static  integer checkevent;

static  ooItr(AMSEventTag)     tageventItr;             


ooStatus LMS::AddRawEvent(ooHandle(ooContObj)& contH, 
                                    uinteger run, uinteger runAux,
                                    uinteger eventNumber, 
                                    uinteger status, time_t time, 
                                    integer ltrig, integer* trig, 
                                    integer ltracker, integer* tracker,
                                    integer lscint, integer* scint,
                                    integer lanti, integer* anti,
                                    integer lctc, integer* ctc,
                                    integer lslow, integer* slow,
                                    integer WriteStartEnd)
//
// contH         - pointer to the container to place event in
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// status        -
//time           - level-1 trigger time
//others         - subdetector's blocks
// WriteStartEnd - flag;
//                       1 - start transaction
//                      -1 - end transaction
//                      -2 - start&end trasaction
//
  {
    ooStatus rstatus = oocError;
    struct    tms cpt;

    if (Mode() != oocUpdate) {
     Error("AddRawEvent : Cannot add event in non-oocUpdate mode");
     return rstatus;
    }

    if(!isTagKeyValid(run,eventNumber)) return rstatus;

    if (contH == NULL) Fatal("AddRawEvent : pointer to the container is NULL");

    ooHandle(AMSEventTagList) tagcontH = tagCont();
    if (tagcontH == NULL) 
                       Fatal("AddRawEvent : pointer to tag container is NULL");

    // Start the transaction
    if (WriteStartEnd == 1 || WriteStartEnd == -2) {
      StartUpdate();     
      checkevent = FindRun(run);
      StartRealTime = times(&cpt);
    }

    //get pointer to the database
    ooHandle(ooDBObj)  dbH = rawdb();
    if (dbH == NULL) 
                Fatal("AddRawEvent : pointer to raw event database is NULL");
    ooHandle(AMSEventTag)  tageventH;
    ooHandle(AMSraweventD) eventH;
    ooItr(AMSraweventD)    raweventItr;             

    if (checkevent == 1) {
     rstatus = FindTagEvent(run, eventNumber, tageventH);
     if (rstatus == oocSuccess) { // tag event exists
       cout<<"AddrawEvent :: overwrite "; tageventH -> print();
       rstatus = ooDelete(tageventH);
       tagcontH -> decNEvents();
     }
    }

    tageventH = new(tagcontH) 
                          AMSEventTag(run, runAux, eventNumber, time, status);
    tagcontH -> incNEvents();

    //rstatus = ooUpdateIndexes(tageventH);
    //if (rstatus != oocSuccess) {
    //Warning("AddRawEvent :: ooUpdateIndexes");
    //  rstatus = oocSuccess;
    //}

    eventH = new(contH) AMSraweventD(run, runAux, eventNumber, status, time,
                                     ltrig, trig, 
                                     ltracker, tracker, 
                                     lscint, scint,
                                     lanti, anti,
                                     lctc, ctc,
                                     lslow, slow);

    rstatus = tageventH -> set_itsRawEvent(eventH);

end:
    if (rstatus == oocSuccess) {
     NN_events++;
     if (WriteStartEnd == -1 || WriteStartEnd == -2 || 
     NN_events%NW_commit == 0) {
      cout<<"events... "<<NN_events<<endl;
      Commit();
      if (NN_events > 0) {
       if(NN_events%NW_commit == 0 && WriteStartEnd > -1) {
        StartUpdate();
        Refresh();
        StartRealTime = times(&cpt);
       }
      }
     }
    } else {
     checkevent = 0;
     Error(" in AddRawEvent");
     Error(" Transaction is aborted");
     Abort();
     return oocError;
    }
 return rstatus;
  }

ooStatus LMS::AddEvent(uinteger run, uinteger eventNumber, time_t time, 
                       integer runtype, number pole, number stationT, 
                       number stationP, integer WriteStartEnd)
//
// listH         - pointer to the container to place event in
// run           - unique run number
// eventNumber   - event number
// WriteStartEnd - flag;
//                       1 - start transaction
//                      -1 - end transaction
//                      -2 - start&end trasaction
//

  {
    ooStatus rstatus = oocError;
    struct    tms cpt;
    integer   i;

    if (Mode() != oocUpdate) {
     Error("AddRrcoEvent -E- Cannot add event in non-oocUpdate mode");
     return rstatus;
    }

    if(!isTagKeyValid(run,eventNumber)) return rstatus;
    if (_tagcontH == NULL) 
                       Fatal("AddEvent : pointer to tag container is NULL");

    // Start the transaction
    if (WriteStartEnd == 1 || WriteStartEnd == -2) {
      StartUpdate();     
      checkevent = FindRun(run);
      StartRealTime = times(&cpt);
    }

    //get pointer to the database
    ooHandle(ooDBObj)      dbH;
    ooHandle(AMSEventList) listH;
    if (recoevents()) {
     dbH = recodb();
     if (dbH == NULL) Fatal("AddEvent : pointer to event database is NULL");
     listH = recoCont();
     if (listH == NULL) Fatal("AddEvent : pointer to the container is NULL");
    }  

    ooHandle(AMSEventTag)  tageventH;
    ooHandle(AMSeventD)    eventH;
    ooItr(AMSeventD)       eventItr;             

    if (checkevent == 1) {
     rstatus = FindTagEvent(run, eventNumber, tageventH);
     if (rstatus == oocSuccess) { // tag event exists
      if(recoevents()) {
       tageventH -> itsRecEvent(eventItr);
       if (eventItr != NULL) {
        eventH = eventItr;
        cout <<"overwrite Reco event ";eventH -> print();
        ooDelete(eventH);
        listH -> decNEvents();
       }
      }
      ooHandle(AMSmcevent) mceventH;
      ooItr(AMSmcevent)    mceventItr;

      if( mcevents() || mceventg() ) {
       tageventH -> itsMCEvent(mceventItr);
       if (mceventItr != NULL) {
        mceventH = mceventItr;
        cout <<"overwrite MC ";mceventH -> print();
        ooDelete(mceventH);
        mcCont() -> decNEvents();
       }
      }
     }
    }
    if (tageventH == NULL) {
                     tageventH = new(_tagcontH) AMSEventTag(run, eventNumber);
                     tagCont()  -> incNEvents();
    }

    if (mcevents() || mceventg()) {
     rstatus = AddMCEvent(tageventH, run, eventNumber, time, runtype, 
                         pole, stationT, stationP);
     if (rstatus != oocSuccess) Fatal("Cannot add MC event");
     mcCont() -> incNEvents();
    }

    if (recoevents()) {
     eventH = new(listH) AMSeventD(run, eventNumber, time);
     rstatus = tageventH -> set_itsRecEvent(eventH);
     rstatus = listH -> AddParticleS(eventH);
     if (rstatus != oocSuccess) Fatal("Cannot add ParticleS");
     
     //AddTrRecHits
     for (i=0; i<6; i++) {
      rstatus = listH -> AddTrRecHit(i, eventH);
      if (rstatus != oocSuccess) 
                           Fatal("Cannot add TrRecHit to the  container");
     }
     //AddTrCluster
     for (i=0; i<2; i++) {
      rstatus = listH -> AddTrCluster(i, eventH);
      if (rstatus != oocSuccess) 
                            Fatal("Cannot add TrCluster to the  container");
     }
    
     //AddTrTrack
     rstatus = listH -> AddTrTrack(eventH);
     if (rstatus != oocSuccess) Fatal("Cannot add TrTrack to the  container");

    
     //LinkHitCluster
     for (i=0; i<6; i++) {
      rstatus = listH -> LinkHitClusterD(i, eventH);
      if (rstatus != oocSuccess) Fatal("Cannot link Hit to Cluster");
     }

     //LinkTrackHit
     rstatus = listH -> LinkTrackHitD(eventH);
     if (rstatus != oocSuccess) Fatal("Cannot link Track to hit");
    

     //AddTOFCluster
     for (i=0; i<4; i++) {
      rstatus = listH -> AddTOFCluster(i, eventH);
      if (rstatus != oocSuccess) 
                           Fatal("Cannot add TOF Cluster to the container");
     }

     //AddCTCCluster
     rstatus = listH -> AddCTCCluster(eventH);
     if (rstatus != oocSuccess) 
                           Fatal("Cannot add CTC Cluster to the container");

     //AddBeta
     rstatus = listH -> AddBeta(eventH);
     if (rstatus != oocSuccess) Fatal("Cannot add Beta to the container");

     //AddCharge
     rstatus = listH -> AddCharge(eventH);
     if (rstatus != oocSuccess) Fatal("Cannot add Charge to the  container");

     //AddParticle
      rstatus = listH -> AddParticle(eventH);
      if (rstatus != oocSuccess) 
                              Fatal("Cannot add Particle to the  container");

     recoCont() -> incNEvents();
    } 

    rstatus = oocSuccess;

end:
    if (rstatus == oocSuccess) {
     NN_events++;
     if (WriteStartEnd == -1 || WriteStartEnd == -2 || 
     NN_events%NW_commit == 0) {
      cout<<"events... "<<NN_events<<endl;
      Commit();
      if (NN_events > 0) {
       if(NN_events%NW_commit == 0 && WriteStartEnd > -1) {
        StartUpdate();
        Refresh();
        StartRealTime = times(&cpt);
       }
      }
     }
    } else {
     checkevent = 0;
     Error(" in AddEvent");
     Error(" Transaction is aborted");
     Abort();
     return oocError;
    }
 return rstatus;
  }

ooStatus LMS::FindEventList(ooHandle(ooDBObj)& dbH, 
                            char* listName, ooHandle(EventList)& listH)
{
        ooStatus rstatus = oocError;

        if (!listName) return rstatus;

        cout<<"Search for "<<listName<<endl;
        if(listH.exist(dbH, listName, Mode())) {
	 if (dbg_prtout != 0) Message("FindEventList : Event list exists"); 
         rstatus = oocSuccess;
        } else {
	 Warning("FindEventList: cannot find or open event list ");
        }          

        return rstatus;
}


ooStatus LMS::ReadRawEvent(uinteger& run, uinteger & runAux,
                           uinteger & eventNumber, uinteger & status,
                           time_t & time,
                           integer & ltrig, integer* trig,
                           integer & lscint, integer* scint,
                           integer & ltracker, integer* tracker,
                           integer & lanti, integer* anti,
                           integer & lctc, integer* ctc,
                           integer & lslow, integer* slow,
                           integer StartCommit)
//
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus = oocError;
    ooMode mrowmode;

    if (!isTagKeyValid(run, eventNumber)) return rstatus;

    if (tagCont() == NULL) 
                       Fatal("ReadEvent : pointer to tag container is NULL");

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     mrowmode = mrowMode();
     StartRead(mrowmode);     
     NN_events = 0;
    }

    //get pointer to the database
    ooHandle(ooDBObj)  dbH = rawdb();
    if (dbH == NULL) 
                Fatal("ReadEvent : pointer to raw event database is NULL");

    ooHandle(AMSEventTag)  tageventH;
    ooHandle(AMSraweventD) eventH;
    ooItr(AMSraweventD)    raweventItr;             

    rstatus = FindTagEvent(run, eventNumber, tageventH);
    if (rstatus != oocSuccess) { // tag event exists
     cout <<"DbApplication::ReadRawEvent -W- event of run "<<run
          <<", with number "<<eventNumber<<" does not exist "<<endl;
     goto end;
    }

    tageventH -> itsRawEvent(raweventItr);
    if (raweventItr == NULL) {
     cout <<"DbApplication::ReadRawEvent -W- raw event of run "<<run
          <<", with number "<<eventNumber<<" does not exist "<<endl;
     goto end;
    }
    raweventItr -> readEvent  (run, runAux,
                               eventNumber, status, time,
                               ltrig, trig,
                               lscint, scint,
                               ltracker, tracker,
                               lanti, anti,
                               lctc, ctc,
                               lslow, slow);
    rstatus = oocSuccess;

end:
    NN_events++;
    if (rstatus == oocSuccess) {
     if (StartCommit == -1 || StartCommit == -2 || NN_events%NR_commit == 0) 
      {
       Commit();
       if(NN_events%NR_commit ==0 && StartCommit > -1) {
        mrowmode = mrowMode();
        StartRead(mrowmode);
       }
      }
    }
   return rstatus;
  }

ooStatus LMS::ReadRawEvents(uinteger& run, uinteger & runAux,
                           uinteger & eventNumber, uinteger & status,
                           time_t & time,
                           integer & ltrig, integer* trig,
                           integer & lscint, integer* scint,
                           integer & ltracker, integer* tracker,
                           integer & lanti, integer* anti,
                           integer & lctc, integer* ctc,
                           integer & lslow, integer* slow,
                           integer nevents, integer StartCommit)
//
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// nevents       - number of events to read
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus = oocError;
    ooMode   mrowmode;
    integer                do_commit = 0;
    char                   pred[40];
    ooItr(AMSraweventD)    raweventItr;             

    if (!isTagKeyValid(run, eventNumber)) return rstatus;

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     mrowmode = mrowMode();
     StartRead(mrowmode);     
     NN_events = 0;
    }
    ooMode mode     = Mode();

    //get pointer to the database
    ooHandle(ooContObj)  tagContH = tagCont();
    if (tagContH == NULL) 
                   Fatal("ReadRawEvents : pointer to tag container is NULL");

    if (StartCommit == 1 || StartCommit == -2) { //init Itr if Start
      if (run < 1) {
        //(void) sprintf(pred,"_runUni>%d",run);
        (void) sprintf(pred,"_run>%d",run);
      } else {
        //if (run > 0) (void) sprintf(pred,"_runUni=%d",run);
       if (run > 0) (void) sprintf(pred,"_run=%d",run);
       if (eventNumber > 0) (void)
        //sprintf(pred,"_runUni=%d && _eventNumber >=%d",run,eventNumber);
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventNumber);
      }
      tageventItr.scan(tagContH, mode, oocAll, pred);
      cout<<"ReadRawEvents -I- scan event of "<<pred<<endl;
    }
    rstatus = oocSuccess;
    if (tageventItr.next() != NULL) {
     tageventItr -> itsRawEvent(raweventItr);
     if (raweventItr != NULL) {
      raweventItr -> readEvent(run, runAux,
                               eventNumber, status, time,
                               ltrig, trig,
                               lscint, scint,
                               ltracker, tracker,
                               lanti, anti,
                               lctc, ctc,
                               lslow, slow);
       NN_events++;
     } 
     if (NN_events > nevents) do_commit = 1;
     if (NN_events%NR_commit == 0) {
       //do_commit = 1;
     }
    } else {
      do_commit = 1;
      if (NN_events == 0) 
                     Warning("ReadRawEvents: no events match to the query ");
    }

end:
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

ooStatus LMS::ReadEvents(uinteger& run, uinteger& eventNumber, 
                         integer   nevents,
                         time_t& time, 
                         integer StartCommit)
//
// run           - unique run number
// eventNumber   - event number
// nevents       - number of events to read
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus = oocError;
    ooMode   mrowmode;
    integer                do_commit = 0;
    char                   pred[40];
    ooItr(AMSeventD)       eventItr;             

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     mrowmode = mrowMode();
     StartRead(mrowmode);     
     NN_events = 0;
    }

    ooMode mode     = Mode();

    //get pointer to the database
    ooHandle(ooContObj)  tagContH = tagCont();
    if (tagContH == NULL) 
                   Fatal("ReadRawEvents : pointer to tag container is NULL");

    if (StartCommit == 1 || StartCommit == -2) { //init Itr if Start
      if (run < 1) {
        //(void) sprintf(pred,"_runUni>%d",run);
        (void) sprintf(pred,"_run>%d",run);
      } else {
        //if (run > 0) (void) sprintf(pred,"_runUni=%d",run);
       if (run > 0) (void) sprintf(pred,"_run=%d",run);
       if (eventNumber > 0) (void)
        //sprintf(pred,"_runUni=%d && _eventNumber >=%d",run,eventNumber);
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventNumber);
      }
      tageventItr.scan(tagContH, mode, oocAll, pred);
      cout<<"ReadEvents -I- scan event of "<<pred<<endl;
    }

    rstatus = oocSuccess;
    if (tageventItr.next() != NULL) {
     tageventItr -> itsRecEvent(eventItr);
     if (eventItr != NULL) {
       eventItr -> readEvent(run, eventNumber, time);
       NN_events++;
     } 
     if (NN_events > nevents) do_commit = 1;
     if (NN_events%NR_commit == 0) {
       //do_commit = 1;
     }
    } else {
      do_commit = 1;
      if (NN_events == 0) Warning("ReadEvents: no events match to the query ");
    }

end:
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
