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
//                  'short' version (setups only)
//                  add tdv dbase
//
// Last Edit Nov 19, 1997. ak.
//
#ifndef LMSSESSION_H
#define LMSSESSION_H


#include <typedefs.h>
#include <ooSession.h>
#include <dbcatalog_ref.h>
#include <dbcatalog.h>

#include <rd45.h>



class LMS : public ooSession {

private:
       char              *_applicationName;
       char              *_prefix;
       char              *_setup;
       integer           _jobtype;
       integer           _applicationTypeR; // see db_comm.h for details
       integer           _applicationTypeW; // see db_comm.h for details

       integer              _transStart;  // number of started transactions
       integer              _transCommit; // number of commited transactions
       integer              _transAbort;  // number of aborted transactions


       ooMode            _mrowmode;

       ooHandle(ooDBObj)    _catdbH;      // MC events dbase
       ooHandle(ooDBObj)    _mcdbH;       // MC events dbase
       ooHandle(ooDBObj)    _rawdbH;      // raw events dbase
       ooHandle(ooDBObj)    _recodbH;     // reconstr. events dbase
       ooHandle(ooDBObj)    _setupdbH;    // geometry dbase
       ooHandle(ooDBObj)    _slowdbH;     // slow control and H/K dbase
       ooHandle(ooDBObj)    _tagdbH;      // handle to geometry dbase
       ooHandle(ooDBObj)    _tdvdbH;      // handle to tdv dbase
       ooHandle(AMSdbs)     _dbTabH;      // handle to database catalog

public:
// Constructor & Destructor
 LMS(); 
 LMS(const char* title); 
 ~LMS(); 

	// Action Methods
	ooStatus	AddGeometry();
        ooStatus        WriteGeometry();
	ooStatus	AddMaterial();
	ooStatus	Addamsdbc();
	ooStatus	AddTMedia();
	ooStatus	AddAllTDV();
	ooStatus	FillTDV(integer ntdv);

	ooStatus	ReadGeometry();
        ooStatus        CopyGeometry();
	ooStatus	ReadMaterial();
	ooStatus	ReadTMedia();
	ooStatus	ReadTDV(char* name, time_t I, time_t B, time_t E, 
                                uinteger* buff);
        ooStatus        ReadTKDBc();

	ooStatus	DeleteSetup(char* setup);
        ooStatus        DeleteTDVContainer();

        void            dbend();

        void            CheckConstants();
        void            CheckCommons();

// -- May 02. new methods

  void     setapplicationName(const char *name)   
                         { if(_applicationName) delete [] _applicationName;
                           _applicationName  = StrDup(name);}
  void     setprefix(const char *prefix) { 
                                           if (_prefix) delete [] _prefix;
                                           _prefix = StrDup(prefix);}
  void     setjobtype(int jobtype)       { _jobtype = jobtype;}
  void     setsetup(const char *setup)   {
                                          if (_setup) delete [] _setup;
                                          _setup  = StrDup(setup);}
  void     settypeR(const integer atype)  {_applicationTypeR = atype;}
  void     settypeW(const integer atype)  {_applicationTypeW = atype;}
  integer  applicationtypeR()             {return _applicationTypeR;}
  integer  applicationtypeW()             {return _applicationTypeW;}

  integer setup(ooMode mode);
  integer slow(ooMode mode);

  inline ooMode                 mrowMode() { return _mrowmode;}
  inline void setmrowMode(ooMode mrowmode) { _mrowmode = mrowmode;}

  inline ooHandle(ooDBObj)&    mcdb()       {return _mcdbH;}
  inline ooHandle(ooDBObj)&    rawdb()      {return _rawdbH;}
  inline ooHandle(ooDBObj)&    recodb()     {return _recodbH;}
  inline ooHandle(ooDBObj)&    setupdb()    {return _setupdbH;}
  inline ooHandle(ooDBObj)&    slowdb()     {return _slowdbH;}
  inline ooHandle(ooDBObj)&    tagdb()      {return _tagdbH;}
  inline ooHandle(ooDBObj)&    tdvdb()      {return _tdvdbH;}
  inline ooHandle(AMSdbs)&     Tabdb()      {return _dbTabH;}

        integer         nTransStart()  { return _transStart;}
        integer         nTransCommit() { return _transCommit;}
        integer         nTransAbort()  { return _transAbort;}
        void            printTransStatistics();

  ooStatus ClusteringInit(ooMode mode, ooMode mrowmode = oocNoMROW);
  integer  Container(ooHandle(ooDBObj) & dbH, const char* contName, 
                     ooHandle(ooContObj) & contH);
  void     ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH);

// Transactions
  void StartUpdate(const char *tag=NULL);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
  void Commit();
  void Abort();
  void resetTransCount();

// Get
  char*  getApplicationName() {return _applicationName;}
  char*  getsetup()           {return _setup;}
  int    jobtype()            {return _jobtype;}

// Others
 void Refresh();
 int  simulation() {if (_jobtype == 0) return 1;
                    if (_jobtype != 0) return 0;} 
};
#endif
