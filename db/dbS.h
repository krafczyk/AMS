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
//
// Last Edit Oct 3, 1997. ak.
//
#ifndef LMSSESSION_H
#define LMSSESSION_H


#include <typedefs.h>
#include <ooSession.h>
#include <gmatD_ref.h>
#include <gtmedD_ref.h>
#include <gvolumeD_ref.h>
#include <amsdbcD_ref.h>
#include <ctcdbcD_ref.h>
#include <tofdbcD_ref.h>
#include <dbcatalog_ref.h>
#include <daqevt.h>
#include <gmatD.h>
#include <gtmedD.h>
#include <gvolumeD.h>
#include <amsdbcD.h>
#include <ctcdbcD.h>
#include <tofdbcD.h>
#include <dbcatalog.h>

#include <rd45.h>



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

       ooHandle(ooDBObj)    _catdbH;      // MC events dbase
       ooHandle(ooDBObj)    _mcdbH;       // MC events dbase
       ooHandle(ooDBObj)    _rawdbH;      // raw events dbase
       ooHandle(ooDBObj)    _recodbH;     // reconstr. events dbase
       ooHandle(ooDBObj)    _setupdbH;    // geometry dbase
       ooHandle(ooDBObj)    _slowdbH;     // slow control and H/K dbase
       ooHandle(ooDBObj)    _tagdbH;      // handle to geometry dbase


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
        void CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode);
	ooStatus	ReadMaterial();
	ooStatus	ReadTMedia();
	ooStatus	ReadAllTDV();
	ooStatus	ReadTDV(char* name, time_t I, time_t B, time_t E, 
                                uinteger* buff);

	ooStatus	DeleteSetup(char* setup);

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
  void     setsetup(const char *setup)   {
                                          if (_setup) delete [] _setup;
                                          _setup  = StrDup(setup);}
  void     settypeR(const integer atype)  {_applicationTypeR = atype;}
  void     settypeW(const integer atype)  {_applicationTypeW = atype;}
  integer  applicationtypeR()             {return _applicationTypeR;}
  integer  applicationtypeW()             {return _applicationTypeW;}

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

        integer         nTransStart()  { return _transStart;}
        integer         nTransCommit() { return _transCommit;}
        integer         nTransAbort()  { return _transAbort;}

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


  // checking methods
 void Refresh();
 
};
#endif
