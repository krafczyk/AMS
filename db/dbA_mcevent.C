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
// June   , 1997. ak. Geographic coordinates are moved to tag event
//
// last edit June 17, 1997, ak.
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
#include <trigger1D_ref.h>
#include <trigger3D_ref.h>

#include <mcanticlusterD.h>
#include <mcctcclusterD.h>
#include <mceventgD.h>
#include <mctofclusterV.h>
#include <tmcclusterV.h>
#include <trigger1D.h>
#include <trigger3D.h>

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

static  char                   pred[40];
static  integer curContN;
static  integer nTagCont;

static  ooItr(AMSEventTag)     tageventItr;             

ooStatus LMS::AddMCEvent(ooHandle(AMSEventTag)&  tageventH,
                         uinteger run, uinteger eventNumber,
                         time_t time, integer runtype)
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
    eventH = new(listH) AMSmcevent(run, eventNumber, time, runtype);

    rstatus = tageventH -> set_itsMCEvent(eventH);

    if (mceventg()) rstatus = listH -> Addmceventg(eventH);
    if (mcevents())
    {
     rstatus = listH -> Addantimccluster(eventH);
     rstatus = listH -> Addctcmccluster(eventH);
     rstatus = listH -> Addtofmccluster(eventH);
     rstatus = listH -> Addtrmccluster(eventH);
     rstatus = listH -> Addtriggerlvl1(eventH);
     rstatus = listH -> Addtriggerlvl3(eventH);
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
    ooStatus               rstatus = oocError;
    ooMode                 mode     = oocRead;
    ooMode                 mrowmode = mrowMode();;
    integer                do_commit = 0;
    integer                flagNextDB = 0;

    // Start the transaction
    if (StartCommit == 1 || StartCommit == -2) {
     StartRead(mrowmode);     
     NN_events = 0;
     curContN  = 0;
     nTagCont  = ntagconts();
    }

    ooHandle(AMSEventTagList) tagContH;

  newDB:
    do_commit = 0;
    if (StartCommit == 1 || StartCommit == -2 || flagNextDB == 1) { 
     if(flagNextDB != 1) {
      if (run < 1 ) {
        (void) sprintf(pred,"_run>%d",run);
      } else {
       if (run > 0) (void) sprintf(pred,"_run=%d",run,eventn);
       if (eventn > 0) (void)
         sprintf(pred,"_run=%d && _eventNumber >=%d",run,eventn);
      }
     }
     flagNextDB = 0;
     int rc = tagcontN(curContN, tagContH);
     if (rc == 0) {
      Message("ReadMCEvents : All done");
      return oocError;
     }
     if (tagContH == NULL) 
                    Fatal("ReadMCEvents : pointer to tag container is NULL");
     rc = tageventItr.scan(tagContH, mode, oocAll, pred);
     if (rc != oocSuccess) Fatal("ReadMCEvents : tageventItr scan failed");
     cout<<"ReadMCEvents -I- scan event of "<<pred<<endl;
    }

    rstatus = oocSuccess;

    integer               runtype;
    number                pole, stationtheta, stationphi;
    ooItr(AMSmcevent)     eventItr;             
    ooItr(AMSmceventgD)   mceventgItr;          

    if (tageventItr.next() != NULL) {
     tageventItr -> itsMCEvent(eventItr);
     if (eventItr != NULL) {
       tageventItr -> GetGeographicCoo(pole, stationphi, stationtheta);
       eventItr -> readEvent(run, eventn, time, runtype);
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
//
// read triggerlvl1
       integer TofPatt[SCLRS];
       ooItr(Triggerlvl1D) Triggerlvl1Itr;
       eventItr -> pTriggerlvl1(Triggerlvl1Itr, mode);
       if (Triggerlvl1Itr !=NULL) {
        if (dbread_only != 0) {
         Triggerlvl1Itr -> getTofPatt(TofPatt);

         TriggerLVL1* p = new TriggerLVL1(
                                       Triggerlvl1Itr -> getTrigMode(),
                                       Triggerlvl1Itr -> getTofFlag(),
                                       TofPatt,
//                                       Triggerlvl1Itr -> getTofPatt(TofPatt),
//                                       Triggerlvl1Itr -> getTofPatt(),
                                       Triggerlvl1Itr -> getAntiPatt()
                                      );
         AMSEvent::gethead() -> addnext(AMSID("TriggerLVL1",0),p);
         mcevent_size = mcevent_size + sizeof(Triggerlvl1D);
        }
       }
       else{
            Message("ReadMCEvents : no Trigger1 info for this event");
       }

// read triggerlvl3 static members
//       integer TofPatt[SCLRS];
         integer TOFPattern[SCMXBR][SCMXBR];
         integer TOFStatus[SCLRS][SCMXBR];
         integer TrackerStatus[NTRHDRP2];
         integer TrackerAux[NTRHDRP][2];
         integer TOFAux[SCLRS][SCMXBR];
         integer NTOF[SCLRS];
         geant TOFCoo[SCLRS][SCMXBR][3];
         geant TrackerCoo[NTRHDRP][2][3];
         geant TrackerCooZ[nl];
         integer TrackerDRP2Layer[NTRHDRP];

//
 integer nhits[nl];
 integer drp[nl][maxtrpl];
 integer bufdrp[nl*maxtrpl];
 geant   coo[nl][maxtrpl];
 geant   bufcoo[maxtrpl*nl];
 geant   llimitY[nl];
 geant   ulimitY[nl];
 geant   llimitX[nl];
 geant   ulimitX[nl];
 integer Pattern[2];
 number  Residual[2];

//
 integer nbuff0[SCMXBR*SCMXBR];
 integer nbuff1[SCLRS*SCMXBR];
 integer nbuff2[NTRHDRP*2];
 integer nbuff3[NTRHDRP*2];
 integer nbuff4[SCLRS*SCMXBR];
 integer nbuff5[SCLRS];
 geant   nbuff6[SCLRS*SCMXBR*3];
 geant   nbuff7[NTRHDRP*2*3];
 geant   nbuff8[nl];
 integer nbuff9[NTRHDRP];
//
       ooItr(Triggerlvl3D) Triggerlvl3Itr;
       eventItr -> pTriggerlvl3(Triggerlvl3Itr, mode);
       if (Triggerlvl3Itr !=NULL) {
        if (dbread_only != 0) {
        Triggerlvl3Itr -> getPattern(Pattern);
        Triggerlvl3Itr -> getResidual(Residual);
//
         TriggerLVL3* p = new TriggerLVL3(
                                       Triggerlvl3Itr -> getTracTrig(),
                                       Triggerlvl3Itr -> getTofTrig(),
                                       Triggerlvl3Itr -> getAntiTrig(),
                                       Triggerlvl3Itr -> getNTrHits(),
                                       Triggerlvl3Itr -> getNPatFound(),
                                       Pattern,
                                       Residual,
                                       Triggerlvl3Itr -> getTime()
                                      );
         Triggerlvl3Itr -> getnhits(nhits);
         Triggerlvl3Itr -> getdrp(bufdrp);
                           UCOPY(bufdrp,drp,sizeof(integer)*nl*maxtrpl/4);
         Triggerlvl3Itr -> getcoo(bufcoo);
                           UCOPY(bufcoo,coo,sizeof(geant)*nl*maxtrpl/4);

         Triggerlvl3Itr -> getlowlimitY(llimitY);
         Triggerlvl3Itr -> getupperlimitY(ulimitY);
         Triggerlvl3Itr -> getlowlimitX(llimitX);
         Triggerlvl3Itr -> getupperlimitX(ulimitX);
//
         Triggerlvl3Itr -> getTOFPattern(nbuff0);
         Triggerlvl3Itr -> getTOFStatus(nbuff1);
         Triggerlvl3Itr -> getTrackerStatus(nbuff2);
         Triggerlvl3Itr -> getTrackerAux(nbuff3);
         Triggerlvl3Itr -> getTOFAux(nbuff4);
         Triggerlvl3Itr -> getNTOF(nbuff5);
         Triggerlvl3Itr -> getTOFCoo(nbuff6);
         Triggerlvl3Itr -> getTrackerCoo(nbuff7);
         Triggerlvl3Itr -> getTrackerCooZ(nbuff8);
         Triggerlvl3Itr -> getTrackerDRP2Layer(nbuff9);
         Triggerlvl3Itr -> getstripsize();
//

         UCOPY(nbuff0,TOFPattern,sizeof(integer)*SCMXBR*SCMXBR/4);
         UCOPY(nbuff1,TOFStatus,sizeof(integer)*SCLRS*SCMXBR/4);
         UCOPY(nbuff2,TrackerStatus,sizeof(integer)*NTRHDRP2/4);
         UCOPY(nbuff3,TrackerAux,sizeof(integer)*NTRHDRP*2/4);
         UCOPY(nbuff4,TOFAux,sizeof(integer)*SCLRS*SCMXBR/4);
         UCOPY(nbuff5,NTOF,sizeof(integer)*SCLRS/4);
         UCOPY(nbuff6,TOFCoo,sizeof(geant)*SCLRS*SCMXBR*3/4);
         UCOPY(nbuff7,TrackerCoo,sizeof(geant)*NTRHDRP*2*3/4);
         UCOPY(nbuff8,TrackerCooZ,sizeof(geant)*nl/4);
         UCOPY(nbuff9,TrackerDRP2Layer,sizeof(integer)*NTRHDRP/4);

         AMSEvent::gethead() -> addnext(AMSID("TriggerLVL3",0),p);
         mcevent_size = mcevent_size + sizeof(Triggerlvl3D);
        }
       }
       else{
            Message("ReadMCEvents : no Trigger3 info for this event");
       }


//
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
      if (NN_events > nevents) do_commit = 1;
      if (NN_events%NR_commit == 0) do_commit = 1;
     } 
    } else {
     curContN++;
     if (curContN < nTagCont) {
      flagNextDB = 1;
      goto newDB;
     } else {
      Message("ReadMCEvents : All done");
      rstatus = oocError;
      }
    }

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
         Refresh();
        }
        Refresh();
      }
    } else {
     Commit();
    }
   return rstatus;
  }

