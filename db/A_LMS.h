// Database Management Class
//
// May  16, 1996 ak add inline functions to get handles
// July 10, 1996 ak add AMSBeta
// Sep      1996 ak add gvolumeD, gmatD, gtmedD
//                  remove fRunNumber class memeber from list.ddl
// Oct  02, 1996 ak merge GetEvent&GetMCEvent, GetNevents&GetNMCEvents
//                  add eventR, eventW flags
//
// Last Edit Oct 16, 1996. ak.
//
#ifndef LMSSESSION_H
#define LMSSESSION_H


#include <typedefs.h>
#include <ooSession.h>
#include <eventD_ref.h>
#include <tcluster_ref.h>
#include <tmccluster_ref.h>
#include <mctofclusterD_ref.h>
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
#include <commonsD_ref.h>
#include <list.h>
#include <eventD.h>
#include <tcluster.h>
#include <tmccluster.h>
#include <mctofclusterD.h>
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
        ooHandle(ooContObj)  _trClusterH[2];
        ooHandle(ooContObj)  _trLayersH[6];
        ooHandle(ooContObj)  _scLayersH[4];
        ooHandle(ooContObj)  _mapsH;       // container of maps
        ooHandle(ooMap)      _eventMapH;   // maps of events
        ooHandle(ooContObj)  _betaH;

	void	       SetCurrentList(char* listName);

        // Menu Methods

        void            DisplayTopMenu();
        void            DisplayAddMenu();
        void            DisplayDeleteMenu();
        void            DisplayFillMenu();
public:
        void            AddListPrompts();
        void            AddEventPrompts(integer N);
        void            AddGeometryPrompts();
        void            AddMaterialPrompts();
        void            AddTMediaPrompts();
        void            FillEventPrompts(integer N);
        void            FillGeometryPrompts();
        void            FillMaterialPrompts();
        void            FillTMediaPrompts();

        void            DeleteEventPrompts();
        char*           PromptForValue(char* promptString);
        void            SetCurrentListPrompts();

        integer         getNTransStart()  { return _transStart;}
        integer         getNTransCommit() { return _transCommit;}
        integer         getNTransAbort()  { return _transAbort;}

	// Action Methods
	ooStatus	AddList(char* listName, char* setup, 
                                integer flag, ooHandle(AMSEventList)& listH);
	ooStatus	AddEvent(char* listName, integer run, uinteger event,
                                 char* id, integer WriteStartEnd, 
                                 integer eventW);
	ooStatus	AddGeometry(char* listName);
	ooStatus	AddMaterial(char* listName);
	ooStatus	Addamsdbc(char* listName);
	ooStatus	AddTMedia(char* listName);
//	ooStatus	FillEvent(char* listName, char* id, ooMode mode);
	ooStatus	FillGeometry(char* listName);
	ooStatus	FillMaterial(char* listName);
	ooStatus	FillTMedia(char* listName);

	ooStatus	DeleteEvent(char* listName, char* id);

        ooStatus        Start(ooMode mode); // start a transaction
        ooStatus        Commit();           // commit a transaction
        ooStatus        Abort();            // abort a transaction
        ooStatus        FindEventList (char* listName, ooMode mode, 
                                       ooHandle(AMSEventList)& listH);
        ooStatus        LMSInit(ooMode mode, char* prefix);

        void            CheckConstants();

	// Constructor & Destructor
	LMS(ooMode openMode); 
       ~LMS(); 

	// Session Methods
	void		Interactive();

        //Get Methods
        inline ooHandle(ooDBObj)    databaseH() {return _databaseH;}
        inline ooHandle(ooContObj) trClusterH(integer n) { 
                                       ooHandle(ooContObj) contObjH;
                                       contObjH = NULL;
                                       if (n<2) contObjH = _trClusterH[n];
                                       return contObjH;
        }
        inline ooHandle(ooContObj) trLayersH(integer n) { 
                                       ooHandle(ooContObj) contObjH;
                                       contObjH = NULL;
                                       if (n<6) contObjH = _trLayersH[n];
                                       return contObjH;
        }
        inline ooHandle(ooContObj)  mapsH()      {return _mapsH;}
        inline ooHandle(ooMap)      eventMapH()  {return _eventMapH;}

        ooStatus GetEvent (char* listName, char* eventID, integer run,
                           integer eventN, ooMode mode, integer ReadStartEnd, 
                           integer eventR);
        ooStatus GetNEvents (char* listName, char* eventID, integer run, 
                             integer eventN, integer N, ooMode mode, 
                             integer eventR);
        ooStatus Getmceventg (char* listName, char* eventID, integer run,
                               integer eventN, ooMode mode, 
                               integer ReadStartEnd);
};
#endif
