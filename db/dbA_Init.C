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

#include <dbA.h>

ooStatus LMS::ClusteringInit(ooMode mode, ooMode mrowmode)
  {
    ooStatus rstatus = oocSuccess;
    integer  status  = 0;
    ooHandle(ooDBObj)           dbH;
    ooHandle(ooContObj)       contH;
    char                      *contName;
    char                      *message;
    char                      *cptr;
    char                      *dbpathname;

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

// Tag DB
    ooHandle(AMSEventTagList) taglistH;
    ooHandle(ooKeyField)      keyFieldH;
    ooHandle(ooKeyDesc)       keyDescH;

    ooHandle(AMSdbs)          dbTabH;

    _catdbH = db("DbList");
    ContainersC(_catdbH, dbTabH);

    if (dbTabH == NULL) 
                      Fatal("ClusteringInit: catalog of databases not found");
 
    // get TagDB pathname
    integer ntagdbs = dbTabH -> size(dbtag);
    if(ntagdbs < 1) {
     cptr = getenv("AMS_TagDB_Path");
     if ( cptr ) 
      dbpathname =StrDup(cptr);
     else 
      dbpathname =StrDup(AMSdbs::pathNameTab[dbtag]);
      cout<<"ClusteringInit: TagDB path name "<<dbpathname<<endl;
      _tagdbH = db("EventTag-0",dbpathname);
      if (_tagdbH == NULL) Fatal("ClusteringInit : tagdbH is NULL");
      dbTabH -> newDB(dbtag, _tagdbH);
      dbH = _tagdbH;
      ntagdbs = 1;
      delete [] dbpathname;
     } else {
      dbH = dbTabH -> currentDB(dbtag);
      if (!dbH) Fatal("ClusteringInit: cannot get tagDB from the catalog");
      if (mode == oocUpdate) {
       int size = (dbTabH -> dbsize(dbtag));
       if (size/1000 > maxTagDBSize) {
        Message("ClusteringInit: create new tag database ");
        cout<<"ClusteringInit : current tag database size "
            <<size/1000<<" KBytes"<<endl;
        if(dbTabH -> extend(dbtag)) {
         cout<<"new Tag database created "<<endl;
         dbH = dbTabH -> currentDB(dbtag);
         ntagdbs++;
        }
       }
      }
      _tagdbH = dbH;
     }
        
    contName  = StrCat("Events_",_prefix); 
    if (mode == oocUpdate) {
     status = TagList(dbH, contName, _setup, taglistH);
     if (status == -1) {
      message = StrCat("ClusteringInit: Cannot create container ",contName);
      Fatal(message);
     } else {
      if (status == 1 ) {
        keyDescH  = new(taglistH) ooKeyDesc(ooTypeN(AMSEventTag),oocTrue);
        keyFieldH = new(keyDescH) ooKeyField(ooTypeN(AMSEventTag),"_run");
        keyDescH  -> addField(keyFieldH);
        keyFieldH = 
                 new(keyDescH) ooKeyField(ooTypeN(AMSEventTag),"_eventNumber");
        keyDescH  -> addField(keyFieldH);
        if (keyDescH -> createIndex(taglistH) != oocSuccess) 
                                Fatal("ClusteringInit: error creating index");
      }
    }
     _tagcontH = taglistH;
    } else {
      int j = 0;
      tagcontCat.resize(ntagdbs);
      for (int i=0; i<ntagdbs; i++) {
       dbH = dbTabH -> getDB(dbtag,i);
       status = TagList(dbH, contName, _setup, taglistH);
       if (status == 0) {
        tagcontCat.set(i,taglistH);
        _tagcontH = taglistH;
        j++;
       }
      }
      if (j == 0) {
       message = StrCat("ClusteringInit: Cannot open container ",contName);
       Fatal(message);
      }
    }
    if(contName)   delete [] contName;

//MC DB
    ooHandle(AMSMCEventList)  mclistH;
    if (mcevents() || mceventg()) {
     // get db path name
     integer nsimdbs = dbTabH -> size(dbsim);
     if (nsimdbs < 1) {
      cptr = getenv("AMS_MCDB_Path");
      if ( cptr ) 
      dbpathname =StrDup(cptr);
      else 
       dbpathname =StrDup(AMSdbs::pathNameTab[dbsim]);
       cout<<"ClusteringInit: MCDB path "<<AMSdbs::pathNameTab[dbsim]<<endl;
       _mcdbH = db("MCEvents-0",dbpathname);
       if (_mcdbH == NULL) Fatal ("ClusteringInit:: _mcdbH is NULL");
       dbTabH -> newDB(dbsim, _mcdbH);
       nsimdbs = 1;
       delete [] dbpathname;
     } else {
       dbH = dbTabH -> currentDB(dbsim);
       if (!dbH) Fatal("ClusteringInit: cannot get simDB from the catalog");
       if (mode == oocUpdate) {
       int size = (dbTabH -> dbsize(dbsim));
       if (size/1000 > maxMCDBSize) {
        cout<<"ClusteringInit : current MC database size "
            <<size/1000<<" KBytes"<<endl;
        Message("ClusteringInit: create new MC database ");
        if(dbTabH -> extend(dbsim)) {
          Message("ClusteringInit:: new MC database created ");
          dbH = dbTabH -> currentDB(dbsim);
          nsimdbs++;
         }
        }
       }
       _mcdbH = dbH;
      }

     contName = StrCat("Events_",_prefix);
     if (mode == oocUpdate) {
      status = mcList(_mcdbH, contName, mclistH);
      if (status == -1) {
       message = StrCat("Cannot open/create container ",contName);
       Fatal(message);
      }
      _mccontH = mclistH;
     } else {
       int j = 0;
       mccontCat.resize(nsimdbs);
       for (int i = 0; i<nsimdbs; i++) {
        dbH = dbTabH -> getDB(dbsim,i);
        status = mcList(dbH, contName, mclistH);
        if (status == 0) {
          mccontCat.set(i,mclistH);
          _mccontH = mclistH;
          j++;
        }
       }
       if (j == 0) {
        message = StrCat("ClusteringInit: Cannot open container ",contName);
        Fatal(message);
       }
     }

     ContainersM(_mcdbH, _prefix);
     mclistH -> SetContainersNames(_prefix);
     if(contName) delete [] contName;
    }

//Raw DB
    if (rawevents()) {
     // get db path name

      integer nrawdbs = dbTabH -> size(dbraw);

      if(nrawdbs < 1) {
       cptr = getenv("AMS_RawDB_Path");
       if ( cptr ) 
        dbpathname =StrDup(cptr);
       else 
        dbpathname =StrDup(AMSdbs::pathNameTab[dbraw]);
        cout<<"ClusteringInit: RawDB path "<<AMSdbs::pathNameTab[dbraw]<<endl;
        _rawdbH = db("RawEvents-0",dbpathname);
        if (_rawdbH == NULL) Fatal("ClusteringInit : rawdbH is NULL");
        dbTabH -> newDB(dbraw, _rawdbH);
        dbH = _rawdbH;
        nrawdbs = 1;
        delete [] dbpathname;
      } else {
       dbH = dbTabH -> currentDB(dbraw);
       if (!dbH) Fatal("ClusteringInit: cannot get rawDB from the catalog");
       if (mode == oocUpdate) {
       int size = (dbTabH -> dbsize(dbraw));
       if (size/1000000 > maxRawDBSize) {
        cout<<"ClusteringInit : current raw database size "
            <<size/1000<<" KBytes"<<endl;
        Message("ClusteringInit: create new raw database ");
        if(dbTabH -> extend(dbraw)) {
          Message("ClusteringInit:: new Raw database created ");
          dbH = dbTabH -> currentDB(dbraw);
          nrawdbs++;
         }
        }
       }
       _rawdbH = dbH;
      }
        
     contName  = StrCat("Events_",_prefix); 
     if (mode == oocUpdate) {
      status = Container(dbH, contName, contH);
      if (status == -1) {
       message = StrCat("ClusteringInit: Cannot create container ",contName);
       Fatal(message);
      } 
      _rawcontH = contH;
     } else {
      int j = 0;
      rawcontCat.resize(nrawdbs);
      for (int i=0; i<nrawdbs; i++) {
       dbH = dbTabH -> getDB(dbraw,i);
       status = Container(dbH, contName, contH);
       if (status == 0) {
        rawcontCat.set(i,contH);
        _rawcontH = contH;
        j++;
       }
      }
      if (j == 0) {
       message = StrCat("ClusteringInit: Cannot open container ",contName);
       Fatal(message);
      }
     }
     if(contName)   delete [] contName;
    }

//Reco DB
    ooHandle(AMSEventList)    listH;
    if (recoevents()) {
     integer nrecdbs = dbTabH -> size(dbrec);

     if(nrecdbs < 1) {
      cptr = getenv("AMS_RecDB_Path");
      if ( cptr ) 
        dbpathname =StrDup(cptr);
      else 
       dbpathname =StrDup(AMSdbs::pathNameTab[dbrec]);
       cout<<"ClusteringInit:RecDB path "<<AMSdbs::pathNameTab[dbrec]<<endl;
       _recodbH = db("RecoEvents-0",dbpathname);
       if (_recodbH == NULL) Fatal("ClusteringInit : recodbH is NULL");
       dbTabH -> newDB(dbrec, _recodbH);
       dbH = _recodbH;
       nrecdbs = 1;
       delete [] dbpathname;
     } else {
      dbH = dbTabH -> currentDB(dbrec);
      if (!dbH) Fatal("ClusteringInit: cannot get recoDB from the catalog");
      if (mode == oocUpdate) {
       int size = (dbTabH -> dbsize(dbrec));
       if (size/1000 > maxRecDBSize) {
        cout<<"ClusteringInit : current Rec database size "
            <<size/1000<<" KBytes"<<endl;
        Message("ClusteringInit: create new rec database ");
        if(dbTabH -> extend(dbrec)) {
          Message("ClusteringInit:: new Rec database created ");
          dbH = dbTabH -> currentDB(dbrec);
          nrecdbs++;
         }
        }
      }
       _recodbH = dbH;
     }
        
     contName  = StrCat("Events_",_prefix); 
     if (mode == oocUpdate) {
      status = List(dbH, contName, listH);
      if (status == -1) {
       message = StrCat("ClusteringInit: Cannot create container ",contName);
       Fatal(message);
      } 
      _recocontH = listH;
     } else { // find all containers with name contName
      int j = 0;
      recocontCat.resize(nrecdbs);
      for (int i = 0; i<nrecdbs; i++) {
       dbH = dbTabH -> getDB(dbrec,i);
       status = List(dbH, contName, listH);
       if (status == 0) {
        recocontCat.set(i,listH);
        _recocontH = listH;
        j++;
       }
      }
      if (j == 0) {
       message = StrCat("ClusteringInit: Cannot open container ",contName);
       Fatal(message);
      }
     }
     ContainersR(_recodbH, _prefix);
     listH -> SetContainersNames(_prefix);
     if(contName)   delete [] contName;
    } 
    
    if (setup()) {
     // get db path name
     integer nsetupdbs = dbTabH -> size(dbsetup);
     cout<<"ClusteringInit :: setup database found "<<nsetupdbs<<endl;
     if (nsetupdbs < 1) {
      cptr = getenv("AMS_SetupDB_Path");
      if ( cptr ) 
       dbpathname =StrDup(cptr);
      else 
       dbpathname =StrDup(AMSdbs::pathNameTab[dbsetup]);
       cout<<"ClusteringInit: SetupDB path "<<AMSdbs::pathNameTab[dbsetup]
                                                                       <<endl;
       _setupdbH = db("Setup-0",dbpathname);
       if (_setupdbH == NULL) Fatal ("ClusteringINit:: _setupdbH is NULL");
       dbTabH -> newDB(dbsetup, _setupdbH);
       dbH = _setupdbH;
       nsetupdbs = 1;
       delete [] dbpathname;
      } else {
        // check the material container and database size (I write material
        // first if it exists I suppose others (geometry, tmed) exists also
        char* contName = StrCat("Materials_",_setup);
        ooHandle(ooContObj) contH;
        cout<<"ClusteringInit -I- check container "<<contName<<endl;
        int jj = 0;
        for (int j = 0; j < nsetupdbs; j++) {
         dbH = dbTabH -> getDB(dbsetup,j);
         if (contH.exist(dbH, contName, mode)) {
          jj = 1;
          break;
         }
        } 
        if (jj) {
         _setupdbH = dbH;
        } else {
         cout<<"ClusteringInit -I- container "<<contName<<" not found "<<endl;
         if (mode == oocUpdate) {
           dbH = dbTabH -> currentDB(dbsetup);
           int size = (dbTabH -> dbsize(dbsetup));
           if (size/1000 < maxSetupDBSize) {
           cout<<"ClusteringInit : current setup database size "
               <<size/1000<<" KBytes"<<endl;
           Message("ClusteringInit: create new setup database ");
            if(dbTabH -> extend(dbraw)) {
             Message("ClusteringInit:: new Raw database created ");
             _setupdbH = dbTabH -> currentDB(dbsetup);
            } else {
             Fatal("ClusteringInit: cannot extend setup database");
            }
           } else { // if size < MaxSetupDBSize
             _setupdbH = dbTabH -> currentDB(dbsetup);
           }
         } else { // if read mode
           Fatal("ClusteringInit: cannot create container in Read mode");
         }
        } // if container not found
       if(contName)   delete [] contName;
      }   // if database exist
    }

    if (slow()) {
     _slowdbH  = db("Slow");        // house keeping and slow control
    }

    Commit();
    Message("ClusteringInit : all done");
    return rstatus;
  }    

integer  LMS::TagList(ooHandle(ooDBObj) &dbH, char* listName, char* setup,
                                ooHandle(AMSEventTagList)& taglistH)

  // listName  - name of container to store events
  // setup     - name of setup
  // listH     - pointer to the created container
  // return    -1  - error
  //            0  - container exists
  //            1  - container is created
{
  integer         rstatus = -1;

  if (!listName) Fatal ("AddTagList: listName is NULL");

  ooHandle(ooDBObj) tagdbH = dbH;
  if(!tagdbH) Fatal("AddTagList: _tagdbH is NULL");

  ooMode mode = Mode();
  if (taglistH.exist(tagdbH, listName, mode)) {
   rstatus = 0;
  } else {
    if (mode == oocUpdate) {
     taglistH = new(listName,1,0,0,tagdbH) AMSEventTagList(listName, setup);
     rstatus = 1;
    } else {
      cerr<<"TagList: "<<listName<<endl;
      Warning("Container does not exist. Cannot be created in read mode");
      rstatus = -1;
    }
  }
  return rstatus;
}

void LMS::ContainersR(ooHandle(ooDBObj) & dbH, char* listname)
//
// create all containers for Reconstructed database
//
{	
  ooHandle(ooContObj)  contH;
  char*                contName;
  int                  i;

  ooMode openMode = Mode();

   // create the "TrLayersN" containers, if it does not exist
    char       nameL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
     for (i = 0; i<6; i++) {
      contName = StrCat(nameL[i],listname);
      if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate) {
       contH    = new(contName,0,0,0,dbH) ooContObj;
      }
      delete [] contName;
     }
      Message(" TrLayers are created");

   // create the "TrClusterN" container, if it does not exist
    char       nameC[2][12] = {"TrClusterX_","TrClusterY_"};

    for (i=0; i<2; i++) {
     contName = StrCat(nameC[i],listname);
     if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate) 
      contH    = new(contName,0,0,0,dbH) ooContObj;
      delete [] contName;
    }
    Message("TrClusters are opened/created");

   // create the "ScLayersN" containers, if the do not exist
    char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
    for (i = 0; i<4; i++) {
     contName = StrCat(nameSc[i],listname);
     if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate) 
       contH    =  new(contName,0,0,0,dbH) ooContObj; 
      delete [] contName;
    }
    Message("ScLayers are opened/created");


    // create the "AntiCluster" container, if it does not exist
    contName = StrCat("AntiCluster_",listname);
    if (!contH.exist(dbH, contName, openMode)&& openMode == oocUpdate)
      contH    = new(contName,0,0,0,dbH) ooContObj;
      delete [] contName;
      Message("AntiCluster is opened/created");

   // create the "CTCCluster" container, if it does not exist
    contName = StrCat("CTCCluster_",listname);
    if (!contH.exist(dbH,contName, openMode) && openMode == oocUpdate)
     contH    = new(contName,0,0,0,dbH) ooContObj;
     delete [] contName;
    Message("CTCClusters are opened/created");

    // create the "TrTracks" container, if it does not exist
    contName = StrCat("TrTracks_",listname);
    if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
     contH    = new(contName,0,0,0,dbH) ooContObj; 
     delete [] contName;
    Message("TrTracks is opened/created");

    // create the "Beta" container, if it does not exist
    contName = StrCat("Beta_",listname);
    if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
     contH    = new(contName,0,0,0,dbH) ooContObj;
     delete [] contName;
     Message("Beta is opened/created");

    // create the "Charge" container, if it does not exist
    contName = StrCat("Charge_",listname);
    if (!contH.exist(dbH, contName, openMode)&& openMode == oocUpdate)
      contH    = new(contName,0,0,0,dbH) ooContObj;
      delete [] contName;
      Message("Charge is opened/created");

    // create the "Particle" container, if it does not exist
    contName = StrCat("Particle_",listname);
    if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
     contH    = new(contName,0,0,0,dbH) ooContObj;
     delete [] contName;
     Message("Particle is opened/created");

}

integer LMS::List(ooHandle(ooDBObj) &dbH, char* listName, 
                 ooHandle(AMSEventList)& listH)

  // dbH       - pointer to the database
  // listName  - name of container to store events
  // listH     - pointer to the created container
  // return    -1  - error
  //            0  - container exists
  //            1  - container is created
{
  integer rstatus = -1;
  if (!listName) Fatal ("List: listName is NULL");

  if(!dbH) Fatal("List: recodbH is NULL");

  ooMode mode = Mode();
  if (listH.exist(dbH, listName, mode)) {
   rstatus = 0;
  } else {
    if (mode == oocUpdate) {
     listH = new(listName,1,0,0,dbH) AMSEventList(listName, _prefix);
     if (getsetup()) listH -> setsetupname(getsetup());
     rstatus = 1;
    } else {
      cerr<<"List: "<<listName<<endl;
      Warning("Container does not exist. Cannot be created in read mode");
      rstatus = -1;
    }
  }
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

integer  LMS::mcList(ooHandle(ooDBObj) &dbH, char* listName, 
                      ooHandle(AMSMCEventList)& listH)

  // dbH       - pointer to database
  // listName  - name of container to store events
  // listH     - pointer to the created container
  // return    -1  - error
  //            0  - container exists
  //            1  - container is created
{
  integer         rstatus = -1;

  if (!listName) Fatal ("mcList: listName is NULL");

  if(!dbH) Fatal("mcList: mcdbH is NULL");

  ooMode mode = Mode();
  if (listH.exist(dbH, listName, mode)) {
   rstatus = 0;
  } else {
    if (mode == oocUpdate) {
     listH = new(listName,1,0,0,dbH) 
                               AMSMCEventList(listName,_setup,_prefix);
     rstatus = 1;
    } else {
      cerr<<"List: "<<listName<<endl;
      Warning("Container does not exist. Cannot be created in read mode");
      rstatus = -1;
    }
  }
  return rstatus;
}

void LMS::ContainersM(ooHandle(ooDBObj) & dbH, char* listname)
//
// create all containers for MC database
//
{	
  integer              rstatus = 1;
  ooHandle(ooContObj)  contH;
  char*                contName;

  ooMode openMode = Mode();


  contName = StrCat("TrMCCluster_",listname);
  
  if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
   contH    = new(contName,0,0,0,dbH) ooContObj;
   
  if (contName) delete [] contName;
  Message("TrMCClusters are opened/created");

  contName = StrCat("TOFMCCluster_",listname);
  if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
   contH    = new(contName,0,0,0,dbH) ooContObj;
   if (contName) delete [] contName;
   Message("TOFMCClusters are opened/created");

  contName = StrCat("CTCMCCluster_",listname);
  if (!contH.exist(dbH,contName, openMode)&& openMode == oocUpdate)
    contH    = new(contName,0,0,0,dbH) ooContObj;
  if (contName) delete [] contName;
  Message("CTCMCClusters are opened/created");

    contName = StrCat("AntiMCCluster_",listname);
    if (!contH.exist(dbH,contName, openMode) && openMode == oocUpdate)
     contH    = new(contName,0,0,0,dbH) ooContObj;
    if (contName) delete [] contName;
    Message("AntiMCClusters are opened/created");

   // create the "mcEventg" container, if it does not exist
    contName = StrCat("mceventg_",listname);
    if (!contH.exist(dbH, contName, openMode) && openMode == oocUpdate)
     contH   = new(contName,0,0,0,dbH) ooContObj;
     if (contName) delete [] contName;
     Message("mcEventg is opened/created");
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
