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
// last edit Jun 25, 1997, ak.
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

#include <dbS.h>

ooStatus LMS::ClusteringInit(ooMode mode, ooMode mrowmode)
  {
    ooStatus rstatus = oocSuccess;
    integer  status  = 0;
    ooHandle(ooDBObj)           dbH;
    ooHandle(ooContObj)       contH;
    ooHandle(AMSdbs)          dbTabH;
    char                      *contName;
    char                      *message;
    char                      *cptr;
    char                      *dbpathname;

    if( !setup()      &&   !!slow()       )  return oocSuccess;

    cptr = getenv("OO_FD_BOOT");
    if (!cptr) Fatal("ClusteringInit: environment OO_FD_BOOT in undefined");

    // if oosession has not been initialised do it now
    if (!ooSession::initialized()) ooSession::Init();

    if (mode == oocUpdate) {
      StartUpdate();
    } else {
      setmrowMode(mrowmode);
      StartRead(mrowmode);
    }

    _catdbH = db("DbList");
    ContainersC(_catdbH, dbTabH);
    if (dbTabH == NULL) 
                      Fatal("ClusteringInit: catalog of databases not found");
 
    
    if (setup()) {
     // get db path name
     integer nsetupdbs = dbTabH -> size(dbsetup);
     cout<<"ClusteringInit :: setup database(s) found "<<nsetupdbs<<endl;

     if (nsetupdbs < 1) {

      cptr = getenv("AMS_SetupDB_Path");
      if ( cptr ) 
       dbpathname =StrDup(cptr);
      else 
       dbpathname =StrDup(AMSdbs::pathNameTab[dbsetup]);
      cout<<"ClusteringInit: SetupDB path "<<AMSdbs::pathNameTab[dbsetup]
                                                                       <<endl;
      dbH = db("Setup-0",dbpathname);
      if (dbH == NULL) Fatal ("ClusteringINit:: _setupdbH is NULL");
      dbTabH -> newDB(dbsetup, dbH);
      nsetupdbs = 1;
      delete [] dbpathname;

     } else {

      // check the material container and database size (I write material
      // first if it exists I suppose others (geometry, tmed) exist also
      contName = StrCat("Materials_",_setup);
      cout<<"ClusteringInit -I- check container "<<contName<<endl;
      int jj = 0;
      for (int j = 0; j < nsetupdbs; j++) {
       dbH = dbTabH -> getDB(dbsetup,j);
       if (contH.exist(dbH, contName, mode)) {
        jj = 1;
        break;
       }
      } 
      if (jj == 0) {
       Message("ClusteringInit : cannot find container ");
       if (mode != oocUpdate) 
        Fatal("ClusteringInit: cannot create container in Read mode");
       dbH = dbTabH -> currentDB(dbsetup);
       int size = (dbTabH -> dbsize(dbsetup));
       if (size/1000 > maxSetupDBSize*1000) {
        cout<<"ClusteringInit : current setup database size "
                                                <<size/1000<<" KBytes"<<endl;
        Message("ClusteringInit: create new setup database ");
        if(dbTabH -> extend(dbsetup)) 
         Message("ClusteringInit:: new Setup database created ");
        else 
         Fatal("ClusteringInit: cannot extend setup database");
       } // if size < MaxSetupDBSize
       dbH = dbTabH -> currentDB(dbsetup);
      } // if container not found
      if(contName)   delete [] contName;
     }   
     _setupdbH = dbH;
    }

    if (slow()) {
     _slowdbH  = db("Slow");        // house keeping and slow control
    }

    Commit();
    Message("ClusteringInit : all done");
    return rstatus;
  }    


integer LMS::Container(ooHandle(ooDBObj) & dbH, const char* name, 
                                 ooHandle(ooContObj) & contH)
// Open/Create container 
//
// dbH    - pointer to dbase
// name   - container name
// contH  - pointer to container
//
// return 
//      -1 - cannot open or create container
//       0 - container exists
//       1 - container is created
//
  {
    integer rstatus = -1;
    ooMode mode = Mode();

    if (!name) Fatal("Container name is NULL. Exit");
    if (!dbH)  Fatal("dbH is NULL. Exit");
    if (contH.exist(dbH, name, mode)) {
     rstatus = 0;
    } else {
     if( mode == oocUpdate) {
      contH    = new(name,0,0,0,dbH) ooContObj;
      rstatus = 1; // created
     } else {
      cerr<<"Container: "<<name<<endl;
      Warning("Container does not exist. Cannot be created in read mode");
      rstatus = -1;
     }
    }
  return rstatus;
  }


void LMS::ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH)
//
// create all containers for MC database
//
{	
  integer              rstatus = 1;
  ooHandle(ooContObj)  contH;
  ooItr(AMSdbs)      dbTabItr;
  ooMode openMode = Mode();
  
  dbTabH = NULL;

  if (!contH.exist(dbH, "DbCatalog", openMode) && openMode == oocUpdate) {
    contH    = new("DbCatalog",1,0,0,dbH) ooContObj;
    dbTabH = new (contH) AMSdbs();
  } else {
    dbTabItr.scan(contH, openMode);
    if (dbTabItr.next()) {
      dbTabH = dbTabItr;
    } else {
     if (openMode == oocUpdate) 
       dbTabH = new (contH) AMSdbs();
     else 
      Warning
       ("DbCatalog container is empty. Cannot create the object in Read mode");
    }
  }
  Message("DbCatalog container is opened/created");
}
