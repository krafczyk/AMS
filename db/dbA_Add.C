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
// May    , 1997. ak. 
// June   , 1997. ak. db size limit, navigate through many dbs
//
// last edit June 16, 1997, ak.
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

#include <raweventD.h>
#include <eventTag.h>
#include <mcevent.h>
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
static  integer curContN;
static  integer nTagCont;

static  ooItr(AMSEventTag)     tageventItr;             
static  char                   pred[40];


ooStatus LMS::AddRawEvent(ooHandle(ooContObj)& contH, 
                                    uinteger run, uinteger runAux,
                                    uinteger eventNumber, 
                                    time_t time, 
                                    integer ldata, uint16* data, 
                                    integer WriteStartEnd)
//
// contH         - pointer to the container to place event in
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// status        -
// time          - level-1 trigger time
// data          - subdetector's blocks
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
                          AMSEventTag(run, runAux, eventNumber, time);
    tagcontH -> incNEvents();

    //rstatus = ooUpdateIndexes(tageventH);
    //if (rstatus != oocSuccess) {
    //Warning("AddRawEvent :: ooUpdateIndexes");
    //  rstatus = oocSuccess;
    //}

    eventH = new(contH) AMSraweventD(run, runAux, eventNumber, time, 
                                     ldata, data); 

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
                     tageventH -> SetGeographicCoo(pole, stationT, stationP);
    }

    if (mcevents() || mceventg()) {
     rstatus = AddMCEvent(tageventH, run, eventNumber, time, runtype);
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
    

     //AddAntiCluster
     rstatus = listH -> AddAntiCluster(eventH);
     if (rstatus != oocSuccess) 
                           Fatal("Cannot add TOF Cluster to the container");

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
                           uinteger & eventNumber, time_t & time, 
                           integer & ldata, uint16* data,
                           integer StartCommit)
//
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus = oocError;
    ooMode mrowmode = mrowMode();

    if (!isTagKeyValid(run, eventNumber)) return rstatus;


    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     StartRead(mrowmode);     
     NN_events = 0;
    }

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
    raweventItr -> readEvent  (run, runAux, eventNumber, time, ldata, data);
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
                           uinteger & eventNumber, time_t & time,
                           integer & ldata, uint16* data,
                           integer nevents, integer StartCommit)
//
// run           - unique run number
// runAux        - auxillary run number
// eventNumber   - event number
// nevents       - number of events to read
// StartCommit   - start/commit transaction flag
//
  {
    ooStatus rstatus  = oocError;
    ooMode   mode     = Mode();
    ooMode   mrowmode = mrowMode();
    ooHandle(AMSEventTagList) contH;
    ooItr(AMSraweventD)       raweventItr;             
    integer                   do_commit  = 0;
    integer                   flagNextDB = 0;

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     StartRead(mrowmode);     
     NN_events = 0;
     curContN  = 0;
     nTagCont  = ntagconts();
    }

    ooHandle(AMSEventTagList)  tagContH;

  newDB:
    do_commit = 0;
    if (StartCommit == 1 || StartCommit == -2 || flagNextDB == 1) { 
     if (flagNextDB != 1) {
      if (run < 1) {                                   //init Itr if Start
        (void) sprintf(pred,"_run>%d",run);
      } else {
       if (run > 0) (void) sprintf(pred,"_run=%d",run);
       if (eventNumber > 0) (void)
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventNumber);
      }
     }

     flagNextDB = 0;
     int rc = tagcontN(curContN, tagContH);
     if (rc == 0) {
      Message("ReadRawEvents : All done ");
      return oocError;
     }
     if (tagContH == NULL) 
                    Fatal("ReadRawEvents : pointer to tag container is NULL");
     rc = tageventItr.scan(tagContH, mode, oocAll, pred);
     if (rc != oocSuccess) Fatal("ReadRawEvents : tageventItr scan failed");
     cout<<"ReadRawEvents -I- scan event of "<<pred<<endl;
    }

    rstatus = oocSuccess;

    if (tageventItr.next() != NULL) {
     tageventItr -> itsRawEvent(raweventItr);
     if (raweventItr != NULL) {
      raweventItr -> readEvent(run, runAux,eventNumber, time, ldata, data);
       NN_events++;
     } 
     if (NN_events > nevents) do_commit = 1;
     if (NN_events%NR_commit == 0) {
       //do_commit = 1;
     }
    } else {
     curContN++;
     if(curContN < nTagCont) {
      flagNextDB = 1;
      goto newDB;
     }
      Message("ReadRawEvents: all done");
      rstatus = oocError; // don't call me anymore
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
    integer                do_commit;
    ooItr(AMSeventD)       eventItr;             
    integer                flagNextDB = 0;

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     mrowmode = mrowMode();
     StartRead(mrowmode);     
     NN_events = 0;
     curContN = 0;
     nTagCont = ntagconts();
    }

    ooMode mode     = Mode();

    // get number of containers 
    ooHandle(AMSEventTagList)  tagContH;
  newDB:
    do_commit = 0;
    if (StartCommit == 1 || StartCommit == -2 || flagNextDB == 1) { 
     if(flagNextDB != 1) {
      if (run < 1) {  //init Itr if Start or new dbase
        (void) sprintf(pred,"_run>%d",run);
      } else {
       if (run > 0) (void) sprintf(pred,"_run=%d",run);
       if (eventNumber > 0) (void)
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventNumber);
      }
     }
      flagNextDB = 0;
      int rc = tagcontN(curContN, tagContH); 
      if (rc == 0) { 
        Message("ReadEvents : All done. ");
        return oocError;
      }
      if (tagContH == NULL) 
                       Fatal("ReadEvents : pointer to tag container is NULL");
      rc = tageventItr.scan(tagContH, mode, oocAll, pred);
      if (rc != oocSuccess) Fatal("ReadEvents: tageventItr scan failed");
      cout<<"ReadEvents -I- scan event of "<<pred<<endl;
    }

    rstatus = oocSuccess;
    number                pole, stationtheta, stationphi;
    integer               runtype = 0;
    ooHandle(ooDBObj)      dbH;
    ooHandle(AMSEventList) listH;
    ooHandle(AMSeventD)    eventH;

    if (tageventItr.next() != NULL) {
     tageventItr -> itsRecEvent(eventItr);
     if (eventItr != NULL) {
      tageventItr -> GetGeographicCoo(pole, stationphi, stationtheta);
      eventItr -> readEvent(run, eventNumber, time);
      eventItr -> print();
      if(dbread_only != 0)  //create event
       AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
                    new AMSEvent(AMSID("Event",eventNumber),run,runtype,time,
                                 pole, stationtheta, stationphi)));
     eventH = eventItr;
     if(recoevents()) { // read full reconstructed event from dbase
      dbH = recodb();
      if (dbH == NULL) Fatal("ReadEvents : pointer to event database is NULL");
      listH = recoCont();
      if (listH == NULL) Fatal("ReadEvents: pointer to the container is NULL");

       rstatus = listH -> CopyTrCluster(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read TrCluster");

       rstatus = listH -> CopyTrRecHit(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read TrRecHit");

       rstatus = listH -> CopyTrTrack(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read TrTrack");

       rstatus = listH -> CopyTOFCluster(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read TOFCluster");

       rstatus = listH -> CopyCTCCluster(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read CTCCluster");

       rstatus = listH -> CopyAntiCluster(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read AntiCluster");

       rstatus = listH -> CopyBeta(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read Beta");

       rstatus = listH -> CopyCharge(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read Charge");

       rstatus = listH -> CopyParticle(eventH);
       if(rstatus != oocSuccess) Fatal("ReadEvents: cannot read Particle");
     }
      NN_events++;
      if (NN_events > nevents) do_commit = 1;
      if (NN_events%NR_commit == 0) {
       //do_commit = 1;
      }
     } 
   } else {
      curContN++;
      if (curContN < nTagCont) {
       flagNextDB = 1;
       goto newDB;
      } else {
        Message("ReadEvents: All done");
        rstatus = oocError; // don't call me anymore
      }
    }

end:
    if (rstatus == oocSuccess) {
     if (StartCommit == -1 || StartCommit == -2 || do_commit == 1) 
      {
       cout<<"NN_events "<<NN_events<<endl;
       cout<<"StartCommit, do_commit "<<StartCommit<<", "<<do_commit<<endl;
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

ooStatus LMS::AddRawEvent(ooHandle(ooContObj)& contH, 
                          DAQEvent *pdaq, integer WriteStartEnd)
//
// contH         - pointer to the container to place event in
// pdaq          - pointer to DAQ event (V.Shoutko version)
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

    if (!pdaq) Fatal("AddRawEvent : pointer to DAQ event is NULL");

    if (contH == NULL) Fatal("AddRawEvent : pointer to the container is NULL");
      

    integer     eventlength = pdaq -> eventlength();
    if (eventlength < 1) {
      Error("AddRawEvent : invalid event length");
      return rstatus;
    }

    integer     run = pdaq -> runno();
    integer     eventNumber = pdaq -> eventno();
    if(!isTagKeyValid(run,eventNumber)) return rstatus;
    time_t      time = pdaq -> time();
    integer     status = 0;
    integer     runAux = 0;

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

    tageventH = new(tagcontH) AMSEventTag(run, runAux, eventNumber, time);
    tagcontH -> incNEvents();

    eventH = new(contH) AMSraweventD(run, eventNumber, time, pdaq);

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

