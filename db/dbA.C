// Methods for DBASE management file, objectivity first try
// Aug 23, 1996. ak. change the initial number of pages, allocated for the 
//                   containers, didn't see any improvement, so return back to
//                   the default value.
// Sep , 1996   ak.  Add Geometry and TMedia containers
//                   Count number of started and commited transactions
// Oct , 1996   ak.  create map per list
//                   use Geometry_SETUPName container
//                   container per list
// Feb 13, 1997 ak.  add AMSmceventD , TimeV container
//                   no map, no fID in classes AMSEventD, AMSmceventD
// Mar  5, 1997 ak.  non-hashed containers.
//                   new function dbend
//                   use contH instead of trclusterH, etc
// May   , 1997 ak.  tag, mc, raw databases, dbA instead of LMS
// June  , 1997 ak.  dbase size limitation, dbcatalog, db paths
//
// last edit Jun 17, 1997, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <ooIndex.h>

#include <rd45.h>

#include <typedefs.h>
#include <db_comm.h>

#include <dbA.h>


void LMS::resetTransCount()
{
 _transStart  = 0;
 _transCommit = 0;
 _transAbort  = 0;
}

LMS::LMS()
{ resetTransCount();
}


LMS::LMS(const char *name="untitled") 
{ _applicationName = StrDup(name);
  resetTransCount();
}
  
LMS::~LMS() {
                     if (_applicationName) delete [] _applicationName;  
                     if (_prefix) delete [] _prefix; 
                     if (_setup)  delete [] _setup;
}


void LMS::dbend()
{
  if ( applicationtypeR() +  applicationtypeW() > 0) {
    integer nST = nTransStart();
    integer nCT = nTransCommit();
    integer nAT = nTransAbort();
    if (nST > nCT + nAT) {
      cout <<"LMS::dbend -W- Number of started transactions  "<<nST<<endl;
      cout <<"LMS::dbend -W- Number of commited transactions "<<nCT<<endl;
      cout <<"LMS::dbend -W- Number of aborted transactions  "<<nAT<<endl;
      cout <<"LMS::dbend -I- commit an active transaction "<<endl;
      if (in_trans()) Commit();   // Commit transaction
    }
  }
    if (dbg_prtout) ooRunStatus();
}

void LMS::StartUpdate(const char *tag)
{
    if ((TransStart(mrow(),tag) != oocSuccess) || (setUpdate() != oocSuccess))
    Fatal("could not start transaction in update mode");
    _transStart++;
}

void LMS::StartRead(ooMode mode, const char *tag)
{
    if ((TransStart(mode,tag) != oocSuccess) || (setRead() != oocSuccess))
        Fatal("could not start transaction in read mode");
    _transStart++; 
}

void LMS::Commit()
{
    if (TransCommit() != oocSuccess)
      Fatal("could not commit transaction");
    _transCommit++;
}

void LMS::Abort()
{
    if (TransAbort() != oocSuccess)
      Fatal("could not abort transaction");
    _transAbort++;
}

  integer LMS::recoevents(ooMode mode) 
{
//  if (Mode() == oocUpdate) 
    if (mode == oocUpdate)
    return (_applicationTypeW/DBWriteRecE)%2;
  else
    return (_applicationTypeR/DBWriteRecE)%2;
}

  integer LMS::rawevents(ooMode mode)  
{
//  if (Mode() == oocUpdate)
    if (mode == oocUpdate)
   return (_applicationTypeW/DBWriteRawE)%2;
  else
   return (_applicationTypeR/DBWriteRawE)%2;
}

  integer LMS::mceventg(ooMode mode)   
{
//  if (Mode() == oocUpdate)
  if (mode == oocUpdate)
    return ((_applicationTypeW/DBWriteMCEg)%2);
  else
    return ((_applicationTypeR/DBWriteMCEg)%2);
}
  integer LMS::mcevents(ooMode mode)   
{
//  if (Mode() == oocUpdate)
  if (mode == oocUpdate)
    return ((_applicationTypeW/DBWriteMC)%2);
  else
    return ((_applicationTypeR/DBWriteMC)%2);
}
  integer LMS::setup(ooMode mode)      
{
// if (Mode() == oocUpdate)
 if (mode == oocUpdate)
  return (_applicationTypeW/DBWriteGeom)%2;
 else
  return (_applicationTypeR/DBWriteGeom)%2;
}

  integer LMS::slow(ooMode mode)       
{
//  if (Mode() == oocUpdate)
 if (mode == oocUpdate)
   return (_applicationTypeW/DBWriteSlow)%2;
  else
   return (_applicationTypeR/DBWriteSlow)%2;
}

 integer  LMS::tagcontN(integer n, ooHandle(AMSEventTagList)& listH)
  {
     int rc = 0;
     if (ntagconts() >= n) {
       listH = tagcontCat[n];
       rc = 1;
     }
     return rc;
  }

// integer  LMS::rawcontN(integer n, ooHandle(ooContObj)& contH)
 integer  LMS::rawcontN(integer n, ooHandle(AMSRawEventList)& contH)
  {
     int rc = 0;
     if (nrawconts() >= n) {
       contH = rawcontCat[n];
       rc = 1;
     }
     return rc;
  }

 integer   LMS::mccontN(integer n, ooHandle(AMSMCEventList) & contH)
  {
     int rc = 0;
     if (nmcconts() >= n) {
       contH = mccontCat[n];
       rc = 1;
     }
     return rc;
  }

 integer   LMS::recocontN(integer n, ooHandle(AMSEventList) & contH)
  {
     int rc = 0;
     if (nrecoconts() >= n) {
       contH = recocontCat[n];
       rc = 1;
     }
     return rc;
  }

