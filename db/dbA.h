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
// May    , 1997 ak raw, rec, tag, etc databases, dbA hep application
// June   , 1997 ak db catalogs
// Oct   1, 1997 ak FillTDV, ReadTDV functions
//
// Last Edit Oct 2, 1997. ak.
//
#ifndef LMSSESSION_H
#define LMSSESSION_H


#include <typedefs.h>
#include <ooSession.h>
#include <recevent_ref.h>
#include <tcluster_ref.h>
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
#include <dbcatalog_ref.h>
#include <daqevt.h>
#include <recevent.h>
#include <tcluster.h>
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
#include <dbcatalog.h>

#include <rd45.h>

#include <list.h>
#include <listTag.h>
#include <mclist.h>
#include <rclist.h>

#include <list_ref.h>
#include <listTag_ref.h>
#include <mclist_ref.h>
#include <rclist_ref.h>

#include <eventTag_ref.h>
#include <eventTag.h>

declare(ooVArray, ooRef(AMSEventList))
declare(ooVArray, ooRef(AMSEventTagList))
declare(ooVArray, ooRef(AMSMCEventList))

class LMS : public ooSession {

private:
       char              *_applicationName;
       char              *_prefix;
       char              *_setup;
       integer           _applicationTypeR; // see db_comm.h for details
       integer           _applicationTypeW; // see db_comm.h for details

       integer              _transStart;  // number of started transactions
       integer              _transCommit; // number of commited transactions
       integer              _transAbort;  // number of aborted transactions


       ooMode            _mrowmode;

// Handles for objects of type ooDBObj
       ooHandle(ooDBObj)    _catdbH;       // MC events dbase
       ooHandle(ooDBObj)    _mcdbH;       // MC events dbase
       ooHandle(ooDBObj)    _rawdbH;      // raw events dbase
       ooHandle(ooDBObj)    _recodbH;     // reconstr. events dbase
       ooHandle(ooDBObj)    _setupdbH;    // geometry dbase
       ooHandle(ooDBObj)    _slowdbH;     // slow control and H/K dbase
       ooHandle(ooDBObj)    _tagdbH;      // handle to geometry dbase

       ooHandle(AMSMCEventList)   _mccontH;      // to mc events container
       ooHandle(ooContObj)        _rawcontH;     // to raw events container
       ooHandle(AMSEventList)     _recocontH;    // to reco events container
       ooHandle(AMSEventTagList)  _tagcontH;     // to tag container

       ooVArray(ooRef(AMSEventTagList))  tagcontCat;    //
       ooVArray(ooRef(AMSEventList))     recocontCat;   //
       ooVArray(ooRef(AMSMCEventList))   mccontCat;     //
       ooVArray(ooRef(ooContObj))        rawcontCat;    //

public:
// Constructor & Destructor
 LMS(); 
 LMS(const char* title); 
 ~LMS(); 

	// Action Methods
        ooStatus       AddRawEvent(ooHandle(ooContObj)& contH, 
                                   DAQEvent *pdaq, integer WriteStartEnd);

        ooStatus       AddRawEvent(ooHandle(ooContObj)& contH, 
                                    uinteger run, uinteger runAux,
                                    uinteger eventNumber, time_t time, 
                                    integer ldata, uint16* data, 
                                    integer WriteStartEnd);

        ooStatus        AddEvent(uinteger run, uinteger eventNumber,
                                 time_t time, integer runtype,
                                 number pole, number stationT, 
                                 number stationP, integer WriteStartEnd);

        ooStatus        AddMCEvent(ooHandle(AMSEventTag)&  tageventH,
                                 uinteger run, uinteger eventNumber,
                                 time_t time, integer runtype);

	ooStatus	AddGeometry();
        ooStatus        WriteGeometry();
	ooStatus	AddMaterial();
	ooStatus	Addamsdbc();
	ooStatus	AddTMedia();
	ooStatus	AddTDV();
	ooStatus	FillTDV();


        ooStatus        ReadEvents(uinteger& run, uinteger& eventNumber,
                                   integer nevents, time_t& time,
                                   integer StartCommit);
        ooStatus        ReadMCEvents(uinteger& run, uinteger& eventNumber, 
                                     integer nevents, time_t& time, 
                                     integer StartCommit);
        ooStatus        ReadRawEvent(uinteger& run, uinteger & runAux,
                                     uinteger & eventNumber, time_t & time, 
                                     integer & ldata, uint16* data,
                                     integer StartCommit);
        ooStatus        ReadRawEvents(uinteger& run, uinteger & runAux,
                                     uinteger & eventNumber, time_t & time,
                                     integer & ldata, uint16* data,
                                     integer nevents, integer StartCommit);
	ooStatus	ReadGeometry();
        ooStatus        CopyGeometry();
        void CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode);
	ooStatus	ReadMaterial();
	ooStatus	ReadTMedia();
	ooStatus	ReadAllTDV();
	ooStatus	ReadTDV(char* name, time_t I, time_t B, time_t E, 
                                uinteger* buff);

        ooStatus        DeleteEvent
                          (uinteger run, uinteger eventNumber, integer flag);
	ooStatus	DeleteSetup(char* setup);

        ooStatus        FindEventList (ooHandle(ooDBObj)& dbH, char* listName, 
                                       ooHandle(EventList)& listH);
        void            dbend();

        void            CheckConstants();
        void            CheckCommons();
        
//Print Methods

        ooStatus PrintList(ooHandle(ooDBObj)& dbH, char* printMode, 
                           char *name = "unknown");

// -- May 02. new methods

  void     setapplicationName(const char *name)   
                         { if(_applicationName) delete [] _applicationName;
                           _applicationName  = StrDup(name);}
  void     setprefix(const char *prefix) { 
                                           if (_prefix) delete [] _prefix;
                                           _prefix = StrDup(prefix);}
  void     setsetup(const char *setup)   {
                                          if (_setup) delete [] _setup;
                                          _setup  = StrDup(setup);}
  void     settypeR(const integer atype)  {_applicationTypeR = atype;}
  void     settypeW(const integer atype)  {_applicationTypeW = atype;}
  integer  applicationtypeR()             {return _applicationTypeR;}
  integer  applicationtypeW()             {return _applicationTypeW;}

  integer recoevents();
  integer rawevents();
  integer mcevents();
  integer mceventg();
  integer setup();
  integer slow();

  inline ooMode                 mrowMode() { return _mrowmode;}
  inline void setmrowMode(ooMode mrowmode) { _mrowmode = mrowmode;}

  inline ooHandle(ooDBObj)&    mcdb()       {return _mcdbH;}
  inline ooHandle(ooDBObj)&    rawdb()      {return _rawdbH;}
  inline ooHandle(ooDBObj)&    recodb()     {return _recodbH;}
  inline ooHandle(ooDBObj)&    setupdb()    {return _setupdbH;}
  inline ooHandle(ooDBObj)&    slowdb()     {return _slowdbH;}
  inline ooHandle(ooDBObj)&    tagdb()      {return _tagdbH;}

  inline ooHandle(AMSMCEventList)&   mcCont()       {return _mccontH;}
  inline ooHandle(ooContObj)&        rawCont()      {return _rawcontH;}
  inline ooHandle(AMSEventList)&     recoCont()     {return _recocontH;}
  inline ooHandle(AMSEventTagList)&  tagCont()      {return _tagcontH;}

        integer         nTransStart()  { return _transStart;}
        integer         nTransCommit() { return _transCommit;}
        integer         nTransAbort()  { return _transAbort;}

  ooStatus ClusteringInit(ooMode mode, ooMode mrowmode = oocNoMROW);
  integer  Container(ooHandle(ooDBObj) & dbH, const char* contName, 
                     ooHandle(ooContObj) & contH);
  void     ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH);
  void     ContainersR(ooHandle(ooDBObj) & dbH, char* listname);
  void     ContainersM(ooHandle(ooDBObj) & dbH, char* listname);
  integer  TagList(ooHandle(ooDBObj) & dbH, char* listName, char* setup,
                                    ooHandle(AMSEventTagList) & listH);

  integer  List(ooHandle(ooDBObj) & dbH, char* listName, 
                ooHandle(AMSEventList)& listH);
  integer  mcList(ooHandle(ooDBObj) & dbH, char* listName, 
                                     ooHandle(AMSMCEventList) & listH);

// Transactions
  void StartUpdate(const char *tag=NULL);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
  void Commit();
  void Abort();
  void resetTransCount();

// Get
  char*  getApplicationName() {return _applicationName;}
  char*  getsetup()           {return _setup;}

// Find
  ooStatus FindTagEvent(uinteger runUni, uinteger eventNumber,
                        ooHandle(AMSEventTag)& eventH);
  integer  FindRun(uinteger runUni);

  // checking methods
  inline integer isTagKeyValid(uinteger run, uinteger eventNumber)
  { integer rstatus = 1;
    if (run < 1 || eventNumber < 1) {
     cerr<<"Invalid Run/Event. Run # "<<run<<", event # "<<eventNumber<<endl;
     rstatus = 0;
    }
    return rstatus;
  }
 void Refresh();
 
 integer  tagcontN(integer n, ooHandle(AMSEventTagList)& listH);
 inline integer   ntagconts()         {return tagcontCat.size();}

 integer  rawcontN(integer n, ooHandle(ooContObj)& contH);
 inline integer   nrawconts()         {return rawcontCat.size();}

 integer   mccontN(integer n, ooHandle(AMSMCEventList) & contH);
 inline integer   nmcconts()         {return mccontCat.size();}

 integer   recocontN(integer n, ooHandle(AMSEventList) & contH);
 inline integer   nrecoconts()         {return recocontCat.size();}
};
#endif
