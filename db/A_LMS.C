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
//
// last edit May 08, 1997, ak.
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

#include <A_LMS.h>

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


ooStatus LMS::ClusteringInit(ooMode mode, ooMode mrowmode)
  {
    ooStatus rstatus = oocSuccess;
    integer  status  = 0;
    ooHandle(ooDBObj)           dbH;
    ooHandle(ooContObj)       contH;
    char                      *contName;
    char                      *message;

    // if oosession has not been initialised do it now
    if (!ooSession::initialized()) ooSession::Init();

    if (mode == oocUpdate) StartUpdate();
    if (mode != oocUpdate) {
      setmrowMode(mrowmode);
      StartRead(mrowmode);
    }

// Tag DB
    ooHandle(AMSEventTagList) taglistH;
    ooHandle(ooKeyField)      keyFieldH;
    ooHandle(ooKeyDesc)       keyDescH;

     _tagdbH = db("EventTag");
     if (_tagdbH == NULL) Fatal("ClusteringInit : tagdbH is NULL");

    contName  = StrCat("Events_",_prefix); 
    status = TagList(contName, _setup, taglistH);
    if (status == -1) {
     message =StrCat("ClusteringInit: Cannot open/create container ",contName);
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
     if(contName) delete [] contName;

//MC DB
    ooHandle(AMSMCEventList)  mclistH;
    if (mcevents() || mceventg()) {
     _mcdbH   = db("MCEvents");   // Raw Events containers
     if (_mcdbH == NULL) Fatal ("_mcdbH is NULL");
     contName = StrCat("Events_",_prefix);
     status = mcList(contName, mclistH);
     if (status == -1) {
      message = StrCat("Cannot open/create container ",contName);
      Fatal(message);
     }
     _mccontH = mclistH;
     ContainersM(_mcdbH, contName);
     mclistH -> SetContainersNames(contName);
     if(contName) delete [] contName;
    }

//Raw DB
    if (rawevents()) {
     _rawdbH   = db("RawEvents");   // Raw Events containers
     contName = StrCat("Events_",_prefix);
     status = Container(_rawdbH, contName, contH);
     if (status == -1) {
      message = StrCat("Cannot open/create container ",contName);
      Fatal(message);
     }
     _rawcontH = contH;
     if (contName) delete [] contName;
    }

//Reco DB
    ooHandle(AMSEventList)    listH;
    if (recoevents()) {
     _recodbH  = db("RecoEvents"); // Reconstructed Events containers
     if (_recodbH == NULL) Fatal ("_recodbH is NULL");
     contName = StrCat("Events_",_prefix);
     status = List(contName, listH);
     if (status == -1) {
      message = StrCat("Cannot open/create container ",contName);
      Fatal(message);
     }
     _recocontH = listH;
     if(contName) delete [] contName;
     char* listname = StrDup(listH ->ListName());
     ContainersR(_recodbH, listname);
     listH -> SetContainersNames();
     if (listname) delete [] listname;
    } 
    
    if (setup()) {
     _setupdbH = db("Setup");      // setup (geometry, constants, materials...)
    }

    if (slow()) {
     _slowdbH  = db("Slow");        // house keeping and slow control
    }

    Commit();

    return rstatus;
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

integer  LMS::TagList(char* listName, char* setup,
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

  ooHandle(ooDBObj) tagdbH = tagdb();
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

integer LMS::List(char* listName, ooHandle(AMSEventList)& listH)

  // listName  - name of container to store events
  // listH     - pointer to the created container
  // return    -1  - error
  //            0  - container exists
  //            1  - container is created
{
  integer rstatus = -1;
  if (!listName) Fatal ("AddTagList: listName is NULL");

  ooHandle(ooDBObj) dbH = recodb();
  if(!dbH) Fatal("List: recodbH is NULL");

  ooMode mode = Mode();
  if (listH.exist(dbH, listName, mode)) {
   rstatus = 0;
  } else {
    if (mode == oocUpdate) {
     listH = new(listName,1,0,0,dbH) AMSEventList(listName);
     char* setup = getsetup();
     if (setup) {
       listH -> setsetupname(setup);
       delete [] setup;
     }
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

  integer LMS::recoevents() 
{
  if (Mode() == oocUpdate) 
    return (_applicationTypeW/DBWriteRecE)%2;
  else
    return (_applicationTypeR/DBWriteRecE)%2;
}

  integer LMS::rawevents()  
{
  if (Mode() == oocUpdate)
   return (_applicationTypeW/DBWriteRawE)%2;
  else
   return (_applicationTypeR/DBWriteRawE)%2;
}

  integer LMS::mceventg()   
{
  if (Mode() == oocUpdate)
    return ((_applicationTypeW/DBWriteMCEg)%2);
  else
    return ((_applicationTypeR/DBWriteMCEg)%2);
}
  integer LMS::mcevents()   
{
  if (Mode() == oocUpdate)
    return ((_applicationTypeW/DBWriteMC)%2);
  else
    return ((_applicationTypeR/DBWriteMC)%2);
}
  integer LMS::setup()      
{
 if (Mode() == oocUpdate)
  return (_applicationTypeW/DBWriteGeom)%2;
 else
  return (_applicationTypeR/DBWriteGeom)%2;
}

  integer LMS::slow()       
{
  if (Mode() == oocUpdate)
   return (_applicationTypeW/DBWriteSlow)%2;
  else
   return (_applicationTypeR/DBWriteSlow)%2;
}

integer  LMS::mcList(char* listName, ooHandle(AMSMCEventList)& listH)

  // listName  - name of container to store events
  // listH     - pointer to the created container
  // return    -1  - error
  //            0  - container exists
  //            1  - container is created
{
  integer         rstatus = -1;

  if (!listName) Fatal ("mcList: listName is NULL");

  ooHandle(ooDBObj) dbH = mcdb();
  if(!dbH) Fatal("List: mcdbH is NULL");

  ooMode mode = Mode();
  if (listH.exist(dbH, listName, mode)) {
   rstatus = 0;
  } else {
    if (mode == oocUpdate) {
     listH = new(listName,1,0,0,dbH) AMSMCEventList(listName,getsetup());
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
