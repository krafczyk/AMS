// Database Management Class
//
// May  16, 1996 ak add inline functions to get handles
// July 10, 1996 ak add AMSBeta
// Sep      1996 ak add gvolumeD, gmatD, gtmedD
//                  remove fRunNumber class memeber from list.ddl
// Oct  02, 1996 ak merge GetEvent&GetMCEvent, GetNevents&GetNMCEvents
//                  add eventR, eventW flags
// Nov  27, 1996 ak MROW mode 
// Dec  12, 1996 ak MROW in LMSInit, etc
// Feb  11, 1997 ak AMSmceventD, AddTDV
// Mar  22, 1997 ak new function dbend()
//                  add class member setupdbH
//
// Last Edit Apr 10, 1997. ak.
//
#ifndef LMSSESSION_H
#define LMSSESSION_H


#include <typedefs.h>
#include <ooSession.h>
#include <eventD_ref.h>
#include <mceventD_ref.h>
#include <tcluster_ref.h>
#include <tmcclusterV_ref.h>
#include <mctofclusterV_ref.h>
#include <trrechit_ref.h>
#include <ttrack_ref.h>
#include <tofrecD_ref.h>
#include <ctcrecD_ref.h>
#include <tbeta_ref.h>
#include <chargeD_ref.h>
#include <particleD_ref.h>
#include <gmatD_ref.h>
#include <gtmedD_ref.h>
#include <gvolumeD_ref.h>
#include <amsdbcD_ref.h>
#include <ctcdbcD_ref.h>
#include <tofdbcD_ref.h>
#include <commonsD_ref.h>
#include <list.h>
#include <eventD.h>
#include <mceventD.h>
#include <tcluster.h>
#include <tmccluster.h>
#include <tmcclusterV.h>
#include <mctofclusterD.h>
#include <mctofclusterV.h>
#include <trrechit.h>
#include <ttrack.h>
#include <tofrecD.h>
#include <ctcrecD.h>
#include <tbeta.h>
#include <chargeD.h>
#include <particleD.h>
#include <gmatD.h>
#include <gtmedD.h>
#include <gvolumeD.h>
#include <amsdbcD.h>
#include <ctcdbcD.h>
#include <tofdbcD.h>
#include <commonsD.h>

class LMS {

private:
        ooSession *          _session;
        ooMode               _openMode;
        char*                fListName;    // name of event's list
        integer              _transStart;  // number of started transactions
        integer              _transCommit; // number of commited transactions
        integer              _transAbort;  // number of aborted transactions
        ooHandle(ooDBObj)    _databaseH;
        char*                _setupdbname; // name of setup database
        ooHandle(ooDBObj)    _setupdbH;    // handle to geometry dbase

	void	       SetCurrentList(char* listName);

public:
        char*           PromptForValue(char* promptString);

        integer         getNTransStart()  { return _transStart;}
        integer         getNTransCommit() { return _transCommit;}
        integer         getNTransAbort()  { return _transAbort;}

	// Action Methods
	ooStatus	AddList(char* listName, integer listType,
                                integer eventType, char* setup,
                                ooHandle(AMSEventList)& listH);
	ooStatus	AddEvent(char* listName, integer run, uinteger event,
                                 integer WriteStartEnd, integer eventW);
	ooStatus	AddGeometry(char* listName);
	ooStatus	AddMaterial(char* listName);
	ooStatus	Addamsdbc(char* listName);
	ooStatus	AddTMedia(char* listName);
	ooStatus	AddTDV(char* listName);
	ooStatus	FillGeometry(char* listName);
	ooStatus	FillMaterial(char* listName);
	ooStatus	FillTMedia(char* listName);
	ooStatus	FillTDV(char* listName);

	ooStatus	DeleteEvent
                              (char* listName, integer run, uinteger event);
	ooStatus	DeleteSetup(char* setup);

        ooStatus        Start(ooMode mode); // start a transaction
        ooStatus        Start(ooMode mode, ooMode mrowmode); // + mrowmode
        ooStatus        Commit();           // commit a transaction
        ooStatus        Abort();            // abort a transaction
        integer         getTransLevel();    // checking for an active

        ooStatus        FindEventList (char* listName, ooMode mode, 
                                       ooHandle(AMSEventList)& listH);
        ooStatus        LMSInit(ooMode mode, ooMode mrowmode, char* prefix);
        ooStatus        LMSInitS(ooMode mode, ooMode mrowmode, char* prefix);

        void            dbend(integer eventR, integer eventW);

        void            CheckConstants();

	// Constructor & Destructor
	LMS(ooMode openMode); 
       ~LMS(); 


        //Get Methods
        inline ooHandle(ooDBObj)&    databaseH()   {return _databaseH;}
        inline ooHandle(ooDBObj)&    setupdbH()    {return _setupdbH;}
        char*                        setupdbname() {return _setupdbname;}
        ooStatus                     setsetupdbname(char* name);
        ooStatus GetNEvents (char* listName, integer run, integer event, 
                             integer N, ooMode mode, ooMode mrowmode, 
                             integer eventR);
        ooStatus Getmceventg (char* listName, integer run, integer event, 
                              ooMode mode, ooMode mrowmode,
                              integer ReadStartEnd);
        ooStatus GetmceventD (char* listName, integer run, integer event, 
                              ooMode mode, ooMode mrowmode, 
                              integer ReadStartEnd);

        ooStatus PrintList(ooMode mode, char* printMode);
        ooStatus PrintList(char* listName, ooMode mode, char* printMode);
        ooStatus CheckDB(char* name, ooMode mode, ooHandle(ooDBObj)& dbH);
};
#endif

