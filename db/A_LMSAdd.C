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
//
// last edit Nov 21, 1996, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <sys/times.h>
#include <unistd.h>

#include <typedefs.h>
#include <A_LMS.h>

#include <event.h>
#include <job.h>

static  Float_t  StartRealTime;
static  Float_t  EndRealTime;
static  long int gTicks;
static  integer dbg_prtout;
static  integer NN_events;        // number of events written to the database
static  integer NW_commit = 10;   // commit after each NW_commit events
static  ooItr(AMSEventD)       mceventgItr;


ooStatus   LMS::AddEvent(char* listName, 
                         integer run, uinteger eventNumber, char* eventID, 
                         integer WriteStartEnd, integer eventW)

{
	ooStatus 	     rstatus = oocError;	// Return status
        integer              i;
        ooMode               mode = oocUpdate;
        char                 err_mess[80];
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;
        ooHandle(ooMap)        mapH;

        struct    tms cpt;

        gTicks = sysconf(_SC_CLK_TCK);

//
	strcpy(err_mess, "error");
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
    
        rstatus = listH -> FindEvent(eventID, mode, eventH, mapH);
        if (rstatus == oocSuccess) {
          cout <<"LMS::AddEvent -W- event with ID "<<eventID
               <<" will be overwritten "<<endl;
          rstatus = mapH -> remove(eventID);
          rstatus = ooDelete(eventH);
          listH -> decNEvents();
        }

        rstatus = listH -> AddEvent(run, eventNumber, eventID, eventW, mapH);
        if (rstatus != oocSuccess) {
	 strcpy(err_mess, "Cannot add the event to the list");
         goto error; }
         }
        } else {
	 strcpy(err_mess, "_databaseH == NULL");
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

ooStatus	LMS::AddList(char* listName, char* setup, 
                             integer flag, ooHandle(AMSEventList)& listH)
{
	ooStatus	rstatus = oocError;	// Return status
        
        if (!listName) return rstatus;

        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;

       _databaseH = _session -> DefaultDatabase();

       if (listH.exist(_databaseH, listName, _openMode)) {
         rstatus = Commit();
          return oocError;
       }
        // Create the list, if it doesn't exist
        listH = new(listName,1,0,0,_databaseH) AMSEventList(listName, setup);

        ooHandle(ooContObj)  mapsH;       // container of maps
        ooHandle(ooMap)      eventMapH;   // maps of events

        if (flag != -1 ) {
          char* setup = AMSJob:: gethead() -> getsetup();
          if (setup) listH -> setsetup(setup); 
          integer type = AMSJob:: gethead() -> getjobtype();
          listH -> setlisttype(type); 
          char* mapname =  new char[strlen(listName)+6];
          strcpy(mapname,"Maps_");
          strcat(mapname,listName);
          if (mapsH.exist(_databaseH, mapname, oocUpdate)) {
            eventMapH = new (mapsH) ooMap;
            rstatus = eventMapH.nameObj(mapsH,mapname);
            if (rstatus != oocSuccess) {
              cerr<<"LMS::AddList-E- error creating map "<<mapname<<endl;
            }
          }
        if (mapname) delete [] mapname;
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

ooStatus LMS::GetEvent(char* listName, char* eventID, integer run, 
                       integer eventN, ooMode mode, integer ReadStartEnd, 
                       integer eventT)
//
// read event from the database, with all banks available
//
{
	ooStatus	       rstatus;             // Return status
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;
        char                   err_mess[80];

        if (ReadStartEnd == 1 || ReadStartEnd == -2) {
         rstatus = Start(mode);
         if (rstatus != oocSuccess) return rstatus;
         cout <<"GetEvent:: -I- StartTransaction"<<endl;
        }

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus != oocSuccess) {
	 strcpy(err_mess, "Cannot open a list ");
          goto error;}

        if (eventN < 1) {
         rstatus = listH -> FindEvent(eventID, mode, eventH);
         if (rstatus != oocSuccess) {
	 strcpy(err_mess, "Cannot find an event by ID ");
          goto error;}
        } else {
         rstatus = listH -> FindEventByN(eventN, mode, eventH);
        if (rstatus != oocSuccess) {
	 strcpy(err_mess, "Cannot find an event by number ");
          goto error;}
        }

        if(eventT > 1) 
             rstatus = listH -> CopyEventHeader(eventID, eventH, mode);
        if (rstatus == oocSuccess) {
         if(eventT == 10)rstatus = 
                          listH -> CopyMCeventg(eventID, eventH, mode);
         if(eventT == 100)rstatus = 
                          listH -> CopyMCEvent(eventID, eventH, mode);
         if(eventT == 1000) rstatus = 
                          listH -> CopyEvent(eventID, eventH, mode);
        } else {
	 strcpy(err_mess, "Cannot copy event header. Quit. ");
        }          
error:
        if (rstatus == oocSuccess) {
          if (ReadStartEnd == -1 || ReadStartEnd == -2) {
            rstatus = Commit();
             cout <<"GetEvent:: -I- CommitTransaction"<<endl;
          }
        } else {
          cout <<"GetEvent -E- "<<err_mess<<endl;
          cout <<"GetEvent:: -E- Transaction is aborted"<<endl;
          rstatus = Abort();
          return oocError;
        }

	return rstatus;
}


ooStatus   LMS::AddGeometry(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
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
        if(lmode == oocUpdate) {
          cout<<"LMS::AddGeometry -I- do nothing, list already opened "<<endl;
        }
    
        if(!listH -> AddGeometry()) {
 	 strcpy(err_mess, "Cannot add geometry"); goto error; }
        }
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
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
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
        if(lmode == oocUpdate) {
          cout<<"LMS::AddMaterial -I- do nothing, list already opened "<<endl;
        }
    
        if(!listH -> AddMaterial()) {
 	 strcpy(err_mess, "Cannot add materials"); goto error; }
        }
       }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"AddMaterial:: Error "<<err_mess<<endl;
         rstatus = Abort();  // or Abort it
         return oocError;
        }

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus   LMS::AddTMedia(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocUpdate;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
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
        if(lmode == oocUpdate) {
          cout<<"LMS::AddTMedia -I- do nothing, list already opened "<<endl;
        }
    
        rstatus = listH -> AddTMedia();
        if(rstatus != oocSuccess) {
 	 strcpy(err_mess, "Cannot add tmedia"); goto error; }
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

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus LMS::GetNEvents
            (char* listName, char* eventID, integer run, integer eventN, 
             integer N, ooMode mode, integer eventR)
//+
//
// listName  - name of List
// eventID - ID of event to start from
// eventN  - number of event to start from
// N       - number of events to read from dbase
// eventR  - type of event to read, 100 - MC banks, 1000 - all banks
//-
{
	ooStatus	       rstatus;             // Return status
        ooHandle(AMSEventList) listH;
        ooHandle(AMSEventD)    eventH;
        ooItr(AMSEventD)       eventItr;
        integer                position;            // Position of event
        integer                ii=0;

        if (eventR < 10) {
         cerr<<"LMS::GetNEvent -W- do nothing, eventR "<<eventR<<endl;
          return oocError;
        }

        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;
        cout <<"LMS::GetNEvents -I- StartTransaction"<<endl;

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         if (eventN < 0) {
          if (eventID) {
           rstatus = listH -> FindEvent(eventID, mode, eventH);
           if (rstatus != oocSuccess) {
            cout <<"LMS::GetNEvents: error in FindEvent "<<endl;
            goto error;
           }
           position = eventH -> getPosition();
          } else {
           goto error;
          }
         } else {
          position = eventN - 1;
         }

         if (position < 0) position = 0;
         rstatus = oocError;
         integer nevents = listH -> getNEvents();
         if (nevents != 0) {
          char pred[32];
          (void) sprintf(pred,"_Position>%d",position);
          cout<<"LMS::GetNEvents -I- read "<<N
              <<" events, starting from "<<position+1<<endl;
          eventItr.scan(listH, mode, oocAll, pred);
          if (dbg_prtout == 1) 
           cout<<"LMS::GetNEvent-I- scan event of "<<pred<<endl;
          while (eventItr.next()) 
          {
           if ( (eventR/100)%2 == 1)
               rstatus = listH -> CopyMCEvent(eventID, eventItr, mode);
           if ( (eventR/1000)%2 == 1)
               rstatus = listH -> CopyEvent(eventID, eventItr, mode);
           ii++;
           cout <<"I++ "<<ii<<endl;
           if (rstatus == oocSuccess) {
            cout <<"call guout_ "<<ii<<endl;
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
            (char* listName, char* eventID, integer run, integer eventN, 
             ooMode mode, integer ReadStartEnd)
//+
//
// listName     - name of List
// eventID      - ID of event to start from
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
        integer                frun, N;
        integer                nevents;
        integer                do_abort = 0;
        char                   pred[32];
        integer                do_commit = 0;

        if (ReadStartEnd == 1 || ReadStartEnd == -2) {
         rstatus = Start(mode);
         if (rstatus != oocSuccess) return rstatus;
         cout <<"LMS::Getmceventg: -I- StartTransaction"<<endl;
        }

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         if (eventN < 0) {
          if (eventID) {
           rstatus = listH -> FindEvent(eventID, mode, eventH);
           if (rstatus != oocSuccess) {
            cout <<"LMS::Getmceventg: error in FindEvent "<<endl;
            do_abort = 1;
            goto error;
           }
           N = eventH -> EventNumber();
           frun = eventH -> RunNumber();
          } else {
           do_abort = 1;
           goto error;
          }
         } else {
          N  = eventN;
          frun = run;
          if (N < 1 ) {
           N = 1;
          }
          if (run < 0 ) {
           frun = 0;
          }
         }
         if (ReadStartEnd == 1 || ReadStartEnd == -2) { //init Itr if Start
          if (run > 0)
            (void) sprintf(pred,"fEventNumber>=%d && _run=%d",N,frun);
          if (run < 1)
            (void) sprintf(pred,"_run>%d",frun);
          mceventgItr.scan(listH, mode, oocAll, pred);
          cout<<"LMS::Getmceventg-I- scan event of "<<pred<<endl;
         }
         rstatus = oocError;
         nevents = listH -> getNEvents();
         if (nevents != 0) {
          if (mceventgItr.next() != NULL) {
           rstatus = listH -> CopyEventHeader(eventID, mceventgItr, mode);
           rstatus = listH -> CopyMCeventg(eventID, mceventgItr, mode);
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
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
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
        if(lmode == oocUpdate) {
          cout<<"LMS::Addamsdbc -I- do nothing, list already opened "<<endl;
        }
    
        if(!listH -> Addamsdbc()) {
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
         //return oocError;
        }

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

