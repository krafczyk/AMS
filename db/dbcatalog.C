//  $Id: dbcatalog.C,v 1.3 2001/01/22 17:32:29 choutko Exp $
// method source file for the object db_catalog
// June 08, 1997, ak.
//
// last edit Oct 13, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>

#include <db_comm.h>
#include <rd45.h>

#include <dbcatalog_ref.h>
#include <dbcatalog.h>

char    *AMSdbs::pathNameTab[] = {
   "../../AMS/db/dbs/any/",
   "../../AMS/db/dbs/tag/",
   "../../AMS/db/dbs/raw/",
   "../../AMS/db/dbs/sim/",
   "../../AMS/db/dbs/rec/",
   "../../AMS/db/dbs/setup/",
   "../../AMS/db/dbs/tdv/"
};
int             AMSdbs::filesPerDir = 100;


dbcatalog::dbcatalog() {}
dbcatalog::dbcatalog( const ooHandle(ooDBObj) &dbH)
{
  if (dbH != NULL) dbCat.extend(dbH);
}

ooStatus dbcatalog::extend()
{
  ooStatus rstatus = oocSuccess;
  char    *dbname;
  char    *tagname;
  ooHandle(ooDBObj) newdb;  
  

    tagname = ooThis().getObjName(oovTopFD);
    dbname = new char[strlen(tagname)+8];
    int size = dbCat.size();
    sprintf(dbname,"%s-%d",tagname,size);
    newdb = new ooDBObj(dbname, 0,0, dbCat[size-1].hostName(), 
                                     dbCat[size-1].pathName());
    if (!newdb.isValid()) {
      Error("dbcatalog::extend: Cannot extend catalog (!newdb.isValid())");
      rstatus = oocError;
    } else {
      dbCat.extend(newdb);
    }
   if(dbname) delete [] dbname;

   return rstatus;
}
 ooStatus dbcatalog::getLastDB(ooHandle(ooDBObj) &dbH) 
{
  ooStatus  rstatus = oocError;
  int size = dbCat.size();
  if (size < 1) {
   Warning("dbcatalog::getLastDB: catalog is empty");
  } else {
   dbH = dbCat[size-1];
   rstatus = oocSuccess;
  }
  return rstatus;
}

 ooStatus dbcatalog::getDB(integer n, ooHandle(ooDBObj) &dbH) 
{
  ooStatus  rstatus = oocError;
  int size = dbCat.size();
  if (size < 1 ) {
   Warning("dbcatalog::getLastDB: catalog is empty");
  } else {
    if ( n > size) {
      Warning ("dbcatalog :: getDB:: invalid parameter");
    } else {
     dbH = dbCat[n];
     rstatus = oocSuccess;
    }
  }
  return rstatus;
}

AMSdbs::AMSdbs() 
{
}

ooStatus AMSdbs::newDB(const integer whichdb, const ooHandle(ooDBObj) &dbH) 
{ 
  if (whichdb == dbtag) {
   tag = new dbcatalog(dbH);
   if(tag != NULL) {
    tag.nameObj(oovTopFD,tagdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

  if (whichdb == dbraw) {
   raw = new dbcatalog(dbH);
   if(raw != NULL) {
    raw.nameObj(oovTopFD,rawdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

  if (whichdb == dbsim) {
   mc = new dbcatalog(dbH);
   if(mc != NULL) {
    mc.nameObj(oovTopFD,simdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

  if (whichdb == dbsetup) {
   setup = new dbcatalog(dbH);
   if(setup != NULL) {
    setup.nameObj(oovTopFD,setupdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

  if (whichdb == dbrec) {
   reco = new dbcatalog(dbH);
   if(reco != NULL) {
    reco.nameObj(oovTopFD,recdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

  if (whichdb == dbtdv) {
   tdv = new dbcatalog(dbH);
   if(tdv != NULL) {
    tdv.nameObj(oovTopFD,tdvdb_name);
    return oocSuccess;
   } else {
    return oocError;
   }
  }

} 

long  AMSdbs::dbsize(integer whichdb) 
{
 struct stat stat_buf;
 ooHandle(ooDBObj) dbH;

 if (whichdb == dbtag) 
   if (tag) tag -> getLastDB(dbH);

 if (whichdb == dbraw) 
   if (raw) raw -> getLastDB(dbH);

 if (whichdb == dbsim) 
   if (mc)  mc -> getLastDB(dbH);

 if (whichdb == dbsetup) 
  if (setup) setup -> getLastDB(dbH);

 if (whichdb == dbtdv) 
  if (tdv) tdv -> getLastDB(dbH);

 if (whichdb == dbrec) 
  if (reco) reco -> getLastDB(dbH);

  if (dbH != NULL) {
    stat(dbH.fileName(), &stat_buf);
    return stat_buf.st_size;
  } 

   return -1;
}

ooHandle(ooDBObj)  AMSdbs::currentDB(integer whichdb) 
{
 ooHandle(ooDBObj) dbH;

 if (whichdb == dbtag) 
   if (tag) tag -> getLastDB(dbH);

 if (whichdb == dbraw) 
  if (raw) raw -> getLastDB(dbH);

 if (whichdb == dbsim) 
  if (mc) mc -> getLastDB(dbH);

 if (whichdb == dbsetup) 
  if (setup) setup -> getLastDB(dbH);

 if (whichdb == dbtdv) 
  if (tdv) tdv -> getLastDB(dbH);

 if (whichdb == dbrec) 
  if (reco) reco -> getLastDB(dbH);

  return dbH;
}

ooStatus AMSdbs::extend(integer whichdb) 
{
 ooStatus rstatus = oocError;
 ooHandle(ooDBObj) dbH;

 if (whichdb == dbtag) 
   if(tag) 
     if(tag -> extend()) rstatus = oocSuccess;

 if (whichdb == dbraw) 
   if(raw) 
     if(raw -> extend()) rstatus = oocSuccess;

 if (whichdb == dbsim) 
   if(mc) 
     if(mc -> extend()) rstatus = oocSuccess;

 if (whichdb == dbsetup) 
   if(setup) 
     if(setup -> extend()) rstatus = oocSuccess;

 if (whichdb == dbtdv) 
   if(tdv) 
     if(tdv -> extend()) rstatus = oocSuccess;

 if (whichdb == dbrec) 
   if(reco) 
     if(reco -> extend()) rstatus = oocSuccess;


   return rstatus;
}

ooHandle(ooDBObj)  AMSdbs::getDB(integer whichdb, integer n) 
{
 ooHandle(ooDBObj) dbH;

 if (whichdb == dbtag) 
   if (tag) tag -> getDB(n, dbH);

 if (whichdb == dbraw) 
   if (raw) raw -> getDB(n, dbH);

 if (whichdb == dbsim) 
   if (mc) mc -> getDB(n, dbH);

 if (whichdb == dbsetup) 
   if (setup) setup -> getDB(n, dbH);

 if (whichdb == dbtdv) 
   if (tdv) tdv -> getDB(n, dbH);

 if (whichdb == dbrec) 
   if (reco) reco -> getDB(n, dbH);

 return dbH;

}

integer  AMSdbs::size(integer whichdb) 
{
 int n = -1;
 
 if (whichdb == dbtag) 
   if (tag) n = tag -> size();

 if (whichdb == dbraw) 
   if (raw) n = raw -> size();

 if (whichdb == dbsim) 
   if (mc) n = mc -> size();

 if (whichdb == dbsetup) 
   if (setup) n = setup -> size();

 if (whichdb == dbtdv) 
   if (tdv) n = tdv -> size();

 if (whichdb == dbrec) 
   if (reco) n = reco -> size();

 return n;

}
