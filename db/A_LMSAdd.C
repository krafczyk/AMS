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
// last edit Mar 25, 1997, ak.
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
#include <A_LMS.h>

#include <event.h>
#include <job.h>

static  Float_t  StartRealTime;
static  Float_t  EndRealTime;
static  long int gTicks;
static  integer NN_events;        // number of events written to the database
static  integer NW_commit = 10;   // commit after each NW_commit events
static  ooItr(AMSEventD)       mceventgItr;
static  ooItr(AMSmceventD)     mceventItr;


ooStatus   LMS::AddEvent(char* listName, 
                         integer runNumber, uinteger eventNumber, 
                         integer WriteStartEnd, integer eventW)
  //
  // listName      - name of list (container) to place event
  // runNumber     - run number
  // eventNumber   - event number
  // WriteStartEnd - flag; 
  //                       1 - start transaction
  //                      -1 - end transaction
  //                      -2 - start&end trasaction
  // eventW        - flag, has the same meaning as AMSFFKEY.Write datacard
  //
{
	ooStatus 	     rstatus = oocError;	// Return status
        integer              i;
        ooMode               mode = oocUpdate;
        char                 err_mess[80];
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;

        struct    tms cpt;

        gTicks = sysconf(_SC_CLK_TCK);

//
	strcpy(err_mess, "error");
        if (runNumber < 1 || eventNumber < 1) {
         cerr << "AddEvent -E- invalid run/event number, run # "
              <<runNumber<<", event # "<<eventNumber<<endl;
          return oocError;
        }
        if (eventW < 10) {
         cerr << "AddEvent -W- no action, eventW "<<eventW<<endl;
          return oocError;
        }
// Start the transaction
        if (WriteStartEnd == 1 || WriteStartEnd == -2) {
         rstatus = Start(oocUpdate);
         if (rstatus != oocSuccess) return rstatus;
         StartRealTime = times(&cpt);
        }
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
// Check for the list
        rstatus = FindEventList(listName, oocUpdate, listH);
        if (rstatus == oocSuccess) {

        ooMode  lmode = listH.openMode();
        if(lmode == oocNoOpen) {
         if(!listH.open(_databaseH, listName, oocUpdate)) {
	  strcpy(err_mess, "Cannot open the list in oocUpdate mode"); 
           goto error;}
        } 
        if(lmode == oocRead) {
         strcpy(err_mess,"list already opened in ooRead mode "); goto error;}
        if(lmode == oocUpdate && dbg_prtout != 0) {
          cout<<"LMS::AddEvent -I- do nothing, list already opened "<<endl;
        }
    
        rstatus = listH -> FindEvent(runNumber, eventNumber, mode, eventH);
        if (rstatus == oocSuccess) {
          cout <<"LMS::AddEvent -W- event of run "<<runNumber<<", with number "
               <<eventNumber<<" will be overwritten "<<endl;
          rstatus = ooDelete(eventH);
          listH -> decNEvents();
        }

         rstatus = listH -> AddEvent(runNumber, eventNumber, eventW);
          if (rstatus != oocSuccess) {
           strcpy(err_mess, "Cannot add the event to the list");
          } else {
           cout <<"AddEvent -I- event "<<eventNumber<<", run "<<runNumber
               <<" added to the list "<<endl;
          }
         } else {
          cout <<"AddEvent -E- cannot find list "<<listName<<endl;
          rstatus = oocError;
         }
        } else {
	 strcpy(err_mess, "_databaseH == NULL");
         rstatus = oocError;
        }

error:
        if (rstatus == oocSuccess) {
         NN_events++;
         if (WriteStartEnd == -1 || WriteStartEnd == -2 || 
             NN_events%NW_commit == 0) {
              rstatus = Commit();
              //cout <<"LMS::AddEvent -I- CommitTransaction after "
              //     << NN_events<<" events"<<endl;
        EndRealTime = times(&cpt);
        if (gTicks != 0)
          //         cout <<"elapsed time (EndRealTime - StartRealTime)/event "
          //  <<(EndRealTime - StartRealTime)/gTicks<<" sec."<<endl;
        if(NN_events%NW_commit ==0 && WriteStartEnd > -1) {
         rstatus = Start(oocUpdate);
         if (rstatus == oocSuccess) {
           //cout <<"AddEvent:: -I- StartTransaction, WriteStartEnd "
           //   <<WriteStartEnd<<endl;
          StartRealTime = times(&cpt);
          }
          }
         }
        } else {
          cout <<"LMS::AddEvent -E- "<<err_mess<<endl;
          cout <<"LMS::AddEvent -E- Transaction is aborted"<<endl;
          rstatus = Abort();
          return oocError;
        }

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus	LMS::AddList(char* listName, 
                                integer eventW, ooHandle(AMSEventList)& listH)
  // listName - name of container to store events
  // eventW   - AMSFFKEY.Write
  // listH    - pointer to the created container
{
	ooStatus	rstatus = oocError;	// Return status

        if (!listName) return rstatus;

        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;

       _databaseH = _session -> DefaultDatabase();

       if (listH.exist(_databaseH, listName, _openMode)) {
         listH -> SetContainersNames();
         rstatus = Commit();
          return oocError;
       }
        // Create the list, if it doesn't exist
        char* setup = AMSJob:: gethead() -> getsetup();
        listH = new(listName,1,0,0,_databaseH) AMSEventList(listName, setup);

        if (setup) listH -> setsetup(setup); 
        integer type = AMSJob:: gethead() -> jobtype();
        listH -> setlisttype(type); 
        listH -> setEventType(eventW);

        // Create key
        ooHandle(ooKeyField) keyFieldH;
        ooHandle(ooKeyDesc)  keyDescH;

        if ((eventW/DBWriteMCEv)%2 != 1) {
         keyDescH  = new(listH) ooKeyDesc(ooTypeN(AMSEventD),oocTrue);
         keyFieldH = new(keyDescH) ooKeyField(ooTypeN(AMSEventD),"_runNumber");
         keyDescH  -> addField(keyFieldH);
         keyFieldH = 
                  new(keyDescH) ooKeyField(ooTypeN(AMSEventD),"_eventNumber");
         if (keyDescH  -> addField(keyFieldH) != oocSuccess) 
          {
            cerr<<"AddList -E- error creating key description"<<endl;
            rstatus = Commit();
            return oocError;
          }
        else if (keyDescH -> createIndex(listH) != oocSuccess) 
          {
            cerr<<"AddList -E- error creating index"<<endl;
            rstatus = Commit();
            return oocError;
          }
        } else {
         keyDescH  = new(listH) ooKeyDesc(ooTypeN(AMSmceventD),oocTrue);
         keyFieldH = new(keyDescH) 
                       ooKeyField(ooTypeN(AMSmceventD),"_runNumber");
         keyDescH  -> addField(keyFieldH);
         keyFieldH = new(keyDescH) 
                       ooKeyField(ooTypeN(AMSmceventD),"_eventNumber");
         if (keyDescH  -> addField(keyFieldH) != oocSuccess)
          {
            cerr<<"AddList -E- error creating key description"<<endl;
            rstatus = Commit();
            return oocError;
           }
         if (keyDescH -> createIndex(listH) != oocSuccess) 
          {
            cerr<<"AddList -E- error creating index"<<endl;
            rstatus = Commit();
            return oocError;
          }
        }

       // commit the transaction
        rstatus = Commit();

        return rstatus;
}


void	LMS::SetCurrentList(char* listName)
{
	// Delete old value (if it exists)
	if (fListName) delete fListName;

	// Set the new value
	fListName = new char[strlen(listName) + 1];
	strcpy(fListName, listName);
}

ooStatus LMS::FindEventList
                (char* listName, ooMode mode, ooHandle(AMSEventList)& listH)
{
        ooStatus rstatus = oocError;

        if (!listName) return rstatus;

        if((listH.exist(_session -> DefaultDatabase(), listName, mode))) {
	 if (dbg_prtout != 0) cout << "LMS::FindEventList: - I - Event list " 
                                   << listName << " exists" << endl;
         rstatus = oocSuccess;
        } else {
	 cout << "LMS::FindEventList: - W - cannot find or open event list "
              << " with name "<< listName<<", mode "<<mode<<endl;
        }          

        return rstatus;
}


ooStatus   LMS::AddGeometry(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
	  strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, oocUpdate, listH); // Check list
         if (rstatus == oocSuccess) {
          if(!listH -> AddGeometry(dbH)) {
 	   strcpy(err_mess, "Cannot add geometry"); goto error; }
         } else {
 	   strcpy(err_mess, "listH pointer = NULL"); goto error; }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"AddGeometry:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         return oocError;
        }

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus   LMS::AddMaterial(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
	  strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, oocUpdate, listH); // Check list
         if (rstatus == oocSuccess) {
          if(!listH -> AddMaterial(dbH)) {
 	   strcpy(err_mess, "Cannot add materials"); goto error; }
          } else {
 	   strcpy(err_mess, "listH pointer = NULL"); goto error; }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"AddMaterial:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         return oocError;
        }
	return rstatus;
}

ooStatus   LMS::AddTMedia(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        rstatus = Start(oocUpdate); // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase(); // Get pointer to default 
        if (_databaseH != NULL) {                   // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
	  strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, oocUpdate, listH); // Check list
         if (rstatus == oocSuccess) {
          if (listH != NULL) {
           rstatus = listH -> AddTMedia(dbH);
           if(rstatus != oocSuccess) {
 	    strcpy(err_mess, "Cannot add tmedia"); goto error; }
           } else {
 	   strcpy(err_mess, "listH pointer = NULL"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"AddTMedia:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         exit(1);
        }

	return rstatus;
}

ooStatus LMS::GetNEvents
            (char* listName, integer run, integer eventN, 
             integer N, ooMode mode, ooMode mrowmode, integer eventR)
//+
//
// listName  - name of List
// eventN    - number of event to start from
// N         - number of events to read from dbase
// eventR    - type of event to read (see db_comm.h)
//-
{
	ooStatus	       rstatus;             // Return status
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;
        ooItr(AMSEventD)       eventItr;
        integer                ii=0;
        char                   pred[32];

        if (run < 0 || eventN < 0) {
          cerr <<"LMS::GetNEvent -E- invalid run/event number "<<run
               <<", "<<eventN<<endl;
          return oocError;
        }

        if (eventR < DBWriteMCEg) {
         cerr<<"LMS::GetNEvent -W- do nothing, eventR "<<eventR<<endl;
          return oocError;
        }

        if (mode == oocRead)
          rstatus = Start(mode,mrowmode);
        else 
          rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;
        cout <<"LMS::GetNEvents -I- StartTransaction"<<endl;

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         listH -> SetContainersNames();
         if (eventN > 0) {
           rstatus = listH -> FindEvent(run, eventN, mode, eventH);
           if (rstatus != oocSuccess) {
            cout <<"LMS::GetNEvents: error in FindEvent "<<endl;
            goto error;
           }
         }

         if (run > 0) {
            if (eventN > 0) (void) 
             sprintf(pred,"_eventNumber>=%d && _runNumber=%d",eventN,run);
            if (eventN < 1) (void) sprintf(pred,"_runNumber=%d",run);
           } else {
             (void) sprintf(pred,"_runNumber>%d",run);
           }

         rstatus = oocError;
         integer nevents = listH -> getNEvents();
         if (nevents > 0) {
          cout<<"LMS::GetNEvents -I- read "<<N<<endl;
          eventItr.scan(listH, mode, oocAll, pred);
          if (dbg_prtout == 1) 
           cout<<"LMS::GetNEvent-I- scan event of "<<pred<<endl;
          while (eventItr.next()) 
          {
           rstatus = listH -> CopyEventHeader(eventItr, mode);
           if ( (eventR/DBWriteMC)%2 == 1)
               rstatus = listH -> CopyMCEvent(eventItr, mode);
           if ( (eventR/DBWriteRecE)%2 == 1)
               rstatus = listH -> CopyEvent(eventItr, mode);
           ii++;
           if (rstatus == oocSuccess) {
             guout_();
           } else {
             break;
           }
          if (ii == N || ii == nevents) break;
          }
        } else {
         cerr<<"LMS::GetNEvent -W- quit, there are  "<<nevents
             <<" in the list"<<endl;
         }
        } else {
          cerr <<"LMS::GetNEvents-E- cannot find Event List "<<listName<<endl;
        }

error:
        if (rstatus == oocSuccess) {
         rstatus = Commit();
         cout <<"LMS::GetNEvents -I- CommitTransaction"<<endl;
        } else {
          cout <<"LMS::GetNEvents -E- Transaction is aborted after "
               <<ii<<" events"<<endl;
          rstatus = Abort();
          return oocError;
        }

	return rstatus;
}

ooStatus LMS::Getmceventg
            (char* listName, integer run, integer eventN, 
             ooMode mode, ooMode mrowmode, integer ReadStartEnd)
//+
//
// listName     - name of List
// run          - run number
// eventN       - number of event to start from
// ReadStartEnd - flag, 1 - Start a transaction, -1 - Commit a transaction
//                     -2 - start and commit a tranaction
//
//-
{
	ooStatus	       rstatus;             // Return status
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;
        ooItr(AMSEventD)       eventItr;
        integer                N;
        integer                nevents;
        integer                do_abort = 0;
        char                   pred[32];
        integer                do_commit = 0;

        if (run < 0 || eventN < 0) {
          cerr <<"LMS::Getmceventg -E- invalid run/event number "<<run
               <<", "<<eventN<<endl;
          return oocError;
        }

        if (ReadStartEnd == 1 || ReadStartEnd == -2) {
         if (mode == oocRead)
          rstatus = Start(mode,mrowmode);
         else
         rstatus = Start(mode);
         if (rstatus != oocSuccess) return rstatus;
         cout <<"LMS::Getmceventg: -I- StartTransaction"<<endl;
        }

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         listH -> SetContainersNames();
         if (eventN > 0) {
          if (ReadStartEnd == 1 || ReadStartEnd == -2) {
           if (dbg_prtout) cout<<"Getmceventg -I- search event "<<eventN
                               <<", run "<<run<<endl;
           rstatus = listH -> FindEvent(run, eventN, mode, eventH);
           if (rstatus != oocSuccess) {
            cout <<"LMS::Getmceventg -E- error in FindEvent "<<endl;
            do_abort = 1;
            goto error;
           }
          }
         }

         if (ReadStartEnd == 1 || ReadStartEnd == -2) { //init Itr if Start
           if (run > 0) {
            if (eventN > 0) (void) 
              sprintf(pred,"_eventNumber>=%d && _runNumber=%d",eventN,run);
            if (eventN < 1) 
             (void) sprintf(pred,"_runNumber=%d",run);
           } else {
              (void) sprintf(pred,"_runNumber>%d",run);
           }
          mceventgItr.scan(listH, mode, oocAll, pred);
          cout<<"LMS::Getmceventg-I- scan event of "<<pred<<endl;
         }
         rstatus = oocError;
         nevents = listH -> getNEvents();
         if (nevents != 0) {
          if (mceventgItr.next() != NULL) {
           rstatus = listH -> CopyEventHeader(mceventgItr, mode);
           rstatus = listH -> CopyMCeventg(mceventgItr, mode);
          }
         } else {
          do_commit = 1;
         } 
error:
        if (do_abort == 1) {
         rstatus = Abort();
         return oocError;
        }
        if (rstatus != oocSuccess) {
         if (ReadStartEnd == -1 || ReadStartEnd == -2 || do_commit == 1) {
           rstatus = Commit();
         } else {         
           rstatus = Abort();
         }
         return oocError;
         }
        }
	return rstatus;
}
ooStatus   LMS::Addamsdbc(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
        rstatus = Start(oocUpdate); // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase();  // Get pointer to default 
        if (_databaseH != NULL) {                    // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
	  strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, oocUpdate, listH); // Check list
         if (rstatus == oocSuccess) {
           if(!listH -> Addamsdbc(dbH)) {
            rstatus = oocError;
 	    strcpy(err_mess, "Cannot add amsdbc"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"Addamsdbc:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         exit (1);
        }
	return rstatus;
}

ooStatus LMS::GetmceventD(char* listName, integer run, integer eventN, 
                          ooMode mode, ooMode mrowmode, integer ReadStartEnd)
//+
//
// listName     - name of List
// run          - run number
// eventN       - event Number to start from, if 0, then first event of run
// ReadStartEnd - flag, 1 - Start a transaction, -1 - Commit a transaction
//                     -2 - start and commit a tranaction
//
//-
{
	ooStatus	       rstatus;             // Return status
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;
        ooHandle(AMSmceventD)  eventH;
        integer                nevents;
        integer                do_abort = 0;
        char                   pred[32];
        integer                do_commit = 0;

        static integer oldrun = 0;

        if (run < 0 || eventN < 0) {
          cerr<<"LMS::GetmceventD: -E- invalid run/event number "
              <<run<<", "<<eventN<<endl;
          return oocError;
        }

        if (ReadStartEnd == 1 || ReadStartEnd == -2) {
         if (mode == oocRead)
          rstatus = Start(mode,mrowmode);
         else
          rstatus = Start(mode);
          if (rstatus != oocSuccess) return rstatus;
          if (dbg_prtout) cout<<"LMS::GetmceventD: -I- StartTransaction"<<endl;
        }

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
           cerr<<"LMS::GetmceventD -E- Cannot open setup dbase in "
               <<mode<<" mode";}
         if (ReadStartEnd == 1 || ReadStartEnd == -2) { //init Itr if Start
          if (run > 0 && eventN > 0) (void) 
             sprintf(pred,"_eventNumber>=%d && _runNumber=%d",eventN,run);
          if (run > 0 && eventN < 1) (void) sprintf(pred,"_runNumber=%d",run);
          if (run < 1 && eventN < 1) (void) sprintf(pred,"_runNumber>%d",0);
          mceventItr.scan(listH, mode, oocAll, pred);
          if (dbg_prtout) cout<<"LMS::Getmcevent-I- scan events with "
                              <<pred<<endl;
         }
         rstatus = oocError;
         nevents = listH -> getNEvents();
         if (nevents != 0) {
          if (mceventItr.next() != NULL) {
           if (dbg_prtout) 
            cout <<"GetmceventD -I- Copy event "<<mceventItr -> getevent()
                 <<", run "<<mceventItr -> getrun() <<endl;
            if ( mceventItr -> getrun() != oldrun) {
             cout <<" LMS::CopyMCEventD -I- new run, run number "
                  <<mceventItr -> getrun()<<endl;
             oldrun = mceventItr -> getrun();
             time_t time = mceventItr -> gettime();
             if (dbH != NULL) rstatus = listH -> CopyTDV(time, mode, dbH);
            }
           rstatus = listH -> CopyMCEventD(mceventItr, mode);
          }
         } else {
          do_commit = 1;
         } 
error:
        if (do_abort == 1) {
         rstatus = Abort();
         return oocError;
        }
        if (rstatus != oocSuccess) {
         if (ReadStartEnd == -1 || ReadStartEnd == -2 || do_commit == 1) {
           rstatus = Commit();
         } else {         
           rstatus = Abort();
         }
         return oocError;
         }
        }
	return rstatus;
}

ooStatus   LMS::AddTDV(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        rstatus = Start(oocUpdate); // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase();  // Get pointer to default 
        if (_databaseH != NULL) {                    // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
	  strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, oocUpdate, listH); // Check list
         if (rstatus == oocSuccess) {
          rstatus = listH -> AddTDV(dbH);
          if(rstatus != oocSuccess) {
   	   strcpy(err_mess, "Cannot add time dependent values"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"AddTMedia:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         exit(1);
        }
	return rstatus;
}
