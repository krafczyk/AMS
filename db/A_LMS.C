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
// last edit Mar 25, 1997, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <typedefs.h>
#include <db_comm.h>
#include <A_LMS.h>

LMS::LMS(ooMode openMode) 
{	
  ooStatus          rstatus;
  ooHandle(ooDBObj) dbH;
   cout << "LMS::Constructor: ========Constructor called========" << endl;

   _session  = ooSession::Instance("LOM");
   _openMode = openMode;
   _transStart  = 0;
   _transCommit = 0;
   _transAbort  = 0;
   cout << "LMS::Constructor: Database is initialized" << endl;
   rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH == NULL) {
     cerr <<"LMS::Constructor -E- DefaultDatabase does not exist"<<endl;
     rstatus = Abort();
     return;
    }
    if (!dbH.exist(_session -> FederatedDatabase(),"AMSsetupDB",openMode)) {
      cerr<<"LMS::Constructor -E- setup database does not exist"<<endl;
      rstatus = Abort();
      return;
    } else {
      _setupdbH = dbH;
      rstatus = setsetupdbname("AMSsetupDB");
    }
   } else {
    cerr << "LMS::Constructor: Error - Cannot start a transaction"<<endl;
    rstatus = Abort();
    exit(1);
   }
 
 rstatus = Commit();
 if (rstatus == oocSuccess) cout <<"LMS::Constructor Commit Done"<<endl;
}

LMS::~LMS() { if (fListName) delete fListName;}


ooStatus LMS::Start(ooMode mode)   // start a transaction
{
  ooStatus rstatus = oocError;

  if (_session -> getTransLevel() != 0) {
     cout << "LMS::Start -W- start a transaction, while one is active" << endl;
     cout << "LMS::Start -I- transaction nesting level "
          <<_session -> getTransLevel()<<endl;
  }  
        rstatus = _session -> StartTransaction(mode);
	if (rstatus != oocSuccess) {
	 cerr << "LMS::Start - Error - Cannot start a transaction." << endl;
	 cerr << "LMS::Start - Quit" << endl;
         exit(1);
	} else {
          //cout << "LMS::Start -I- start a transaction." << endl;
           _transStart++;
        }
        return rstatus;
}

ooStatus LMS::Start(ooMode mode, ooMode mrowmode)   // start a transaction
{
  ooStatus rstatus = oocError;

  if (_session -> getTransLevel() != 0) {
     cout << "LMS::Start -W- start a transaction, while one is active" << endl;
     cout << "LMS::Start -I- transaction nesting level "
          <<_session -> getTransLevel()<<endl;
  }  
        rstatus = _session -> StartTransaction(mode, mrowmode);
	if (rstatus != oocSuccess) {
	 cerr << "LMS::Start - Error - Cannot start a transaction." << endl;
	 cerr << "LMS::Start - Quit" << endl;
         exit(1);
	} else {
          //cout << "LMS::Start -I- start a transaction." << endl;
           _transStart++;
        }
        return rstatus;
}

ooStatus LMS::Commit()            // commit a transaction
{
  ooStatus rstatus = oocError;

  if (_session -> getTransLevel() != 0) {
        rstatus = _session -> CommitTransaction();
	if (rstatus != oocSuccess) {
	 cerr << "LMS::Commit - Error- Cannot commit a transaction." << endl;
	 cerr << "LMS::Commit - Quit" << endl;
         exit(1);
	} else {
          //   cout << "LMS::Commit -I-  commit a transaction." << endl;
           _transCommit++;
        }

        // cout <<"LMS::Commit -I- transaction started " <<_transStart<<endl;
        // cout <<"LMS::Commit -I- transaction commited "<<_transCommit<<endl;
        // cout <<"LMS::Commit -I- transaction aborted " <<_transAbort<<endl;
        if (_transStart != _transCommit || _transAbort != 0) 
            cout <<"LMS::Commit -I- transaction s/c/a " <<_transStart<<" "<<
                          _transCommit<<" "<<_transAbort<<endl;
	return rstatus;
  } else {
    cout << "LMS::Commit -W- do nothing, no active transaction." << endl;
    cout << "LMS::Commit -I- transaction nesting level "
          <<_session -> getTransLevel()<<endl;
    return oocSuccess;
  }
}

ooStatus LMS::Abort()            // abort a transaction
{
  ooStatus rstatus = oocError;

        rstatus = _session -> AbortTransaction();
	if (rstatus != oocSuccess) {
	   cerr << "LMS::Abort - Error- Cannot abort a transaction." << endl;
	} else {
	   cout << "LMS::Abort -I-  abort a transaction." << endl;
           _transAbort++;
        }
	return rstatus;
}

integer LMS::getTransLevel()   // get a transaction nesting level
{
    return _session -> getTransLevel();
}

ooStatus LMS::LMSInit(ooMode openMode, ooMode mrowmode, char* prefix) 
{	
  integer              i;
  ooStatus             rstatus;
  ooHandle(ooContObj)  contH;
  char*                contName;

   _openMode = openMode;
   if (openMode == oocRead) rstatus = Start(_openMode, mrowmode);
   if (openMode != oocRead) rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH != NULL) {

   // create the "mcEventg" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"mceventg_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode) && openMode == oocUpdate)
      //contH   = new(contName,1,0,0,_databaseH) ooContObj;
     contH   = new(contName,0,0,0,_databaseH) ooContObj;
     delete [] contName;
     cout << "LMS::Constructor: mcEventg is opened/created" << endl;

   // create the "TrLayersN" containers, if it does not exist
    char       nameL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
     for (i = 0; i<6; i++) {
      contName = new char[strlen(prefix)+10];
      strcpy(contName,nameL[i]);
      strcat(contName,prefix);
      if (!contH.exist(_databaseH, contName, _openMode) 
          && openMode == oocUpdate) 
      contH    = new(contName,0,0,0,_databaseH) ooContObj;
      delete [] contName;
     }
     cout << "LMS::Constructor: TrLayers are opened/created" << endl;

   // create the "TrClusterN" container, if it does not exist
    char       nameC[2][12] = {"TrClusterX_","TrClusterY_"};

    for (i=0; i<2; i++) {
     contName = new char[strlen(prefix)+12];
     strcpy(contName,nameC[i]);
     strcat(contName,prefix);
     if (!contH.exist(_databaseH, contName, _openMode) 
                                             && openMode == oocUpdate) 
      contH    = new(contName,0,0,0,_databaseH) ooContObj;
      delete [] contName;
    }
    cout << "LMS::Constructor: TrClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+13];
    strcpy(contName,"TrMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode) && openMode == oocUpdate) 
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
     delete [] contName;
     cout << "LMS::Constructor: TrMCClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+14];
    strcpy(contName,"TOFMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode) && openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
     delete [] contName;
     cout << "LMS::Constructor: TOFMCClusters are opened/created" << endl;

   // create the "ScLayersN" containers, if the do not exist
    char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
    for (i = 0; i<4; i++) {
     contName = new char[strlen(prefix)+10];
     strcpy(contName,nameSc[i]);
     strcat(contName,prefix);
     if (!contH.exist(_databaseH, contName, _openMode) 
         && openMode == oocUpdate) contH    = 
                                    new(contName,0,0,0,_databaseH) ooContObj; 
      delete [] contName;
    }
    cout << "LMS::Constructor: ScLayers are opened/created" << endl;


   // create the "CTCCluster" container, if it does not exist
    contName = new char[strlen(prefix)+12];
    strcpy(contName,"CTCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode) && openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: CTCClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+14];
    strcpy(contName,"CTCMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode)&& openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: CTCMCClusters are opened/created" << endl;

    // create the "TrTracks" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"TrTracks_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode) && openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj; 
     contH    = new(contName,0,0,0,_databaseH) ooContObj; 
    delete [] contName;
    cout << "LMS::Constructor: TrTracks is opened/created" << endl;

    // create the "Beta" container, if it does not exist
    contName = new char[strlen(prefix)+6];
    strcpy(contName,"Beta_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode) && openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: Beta is opened/created" << endl;

    // create the "Charge" container, if it does not exist
    contName = new char[strlen(prefix)+8];
    strcpy(contName,"Charge_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode)&& openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
      contH    = new(contName,0,0,0,_databaseH) ooContObj;
      delete [] contName;
      cout << "LMS::Constructor: Charge is opened/created" << endl;

    // create the "Particle" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"Particle_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode)&& openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
     delete [] contName;
     cout << "LMS::Constructor: Particle is opened/created" << endl;

    contName = new char[strlen(prefix)+15];
    strcpy(contName,"AntiMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode) && openMode == oocUpdate)
      //contH    = new(contName,1,0,0,_databaseH) ooContObj;
     contH    = new(contName,0,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: AntiMCClusters are opened/created" << endl;

 } else {
  cout <<"LMS::LMSInit: Error DefaultDatabase does not exist"<<endl;
  cout << "LMS::LMSInit: pointer to default database is null"<<endl;
  rstatus = Abort();
  exit(1);
 }
} else {
    cerr << "LMS::Constructor: Error - Cannot start a transaction"<<endl;
    rstatus = Abort();
    exit(1);
}
 
 rstatus = Commit();
 if (rstatus == oocSuccess) cout <<"LMS::LMSInit Commit Done"<<endl;
 return rstatus;
}

ooStatus LMS::LMSInitS(ooMode openMode, ooMode mrowmode, char* prefix) 
{	
  integer              i;
  ooStatus             rstatus;
  ooHandle(ooContObj)  contH;
  char*                contName;

   _openMode = openMode;
   if (openMode == oocRead) rstatus = Start(_openMode, mrowmode);
   if (openMode != oocRead) rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH == NULL) {
     cout <<"LMS::LMSInit: Error DefaultDatabase does not exist"<<endl;
     cout << "LMS::LMSInit: pointer to default database is null"<<endl;
     rstatus = Abort();
     exit(1);
    }
   } else {
    cerr << "LMS::Constructor: Error - Cannot start a transaction"<<endl;
    rstatus = Abort();
    exit(1);
   }
 
 rstatus = Commit();
 if (rstatus == oocSuccess) cout <<"LMS::LMSInit Commit Done"<<endl;
 return rstatus;
}

void LMS::dbend(integer eventR, integer eventW)
{
  if (eventR + eventW > 0) {
    integer nST = getNTransStart();
    integer nCT = getNTransCommit();
    integer nAT = getNTransAbort();
    if (nST > nCT + nAT) {
      cout <<"LMS::dbend -W- Number of started transactions  "<<nST<<endl;
      cout <<"LMS::dbend -W- Number of commited transactions "<<nCT<<endl;
      cout <<"LMS::dbend -W- Number of aborted transactions  "<<nAT<<endl;
      cout <<"LMS::dbend -I- commit an active transaction "<<endl;
      cout <<"LMS::dbend -I- transaction nesting level "
           <<getTransLevel()<<endl;
      Commit();   // Commit transaction
    }
  }
    if (dbg_prtout) ooRunStatus();
}
ooStatus LMS::setsetupdbname(char* name) 
{
  ooStatus rstatus = oocError;
  if (name) {
   if (_setupdbname) delete [] _setupdbname;
    _setupdbname = new char[strlen(name)+1];
    strcpy(_setupdbname,name);
    rstatus = oocSuccess;
  }
  return rstatus;
}

ooStatus LMS::CheckDB(char* name, ooMode mode, ooHandle(ooDBObj)& dbH)
{
     ooStatus rstatus = oocSuccess;
     ooHandle(ooFDObj) fddatabaseH;

      if (name) {
       if (dbg_prtout == 1)cout << "LMS:: CheckDB : dbase "<<name
                               << ", mode "<<mode<<endl;
       fddatabaseH = _session -> FederatedDatabase();
       if (fddatabaseH != NULL ) {
        if (!dbH.exist(fddatabaseH,name,mode)) {
         cerr <<"LMS::CheckDBr : Warning - Cannot find or open dbase "<<name
              <<" in mode "<<mode<<endl;
         rstatus = oocError;
        }
       } else {
         cerr <<"LMS::CheckDB -E- fddbase pointer is NULL"<<endl;
         rstatus = oocError;
       }
      } else {
        cerr <<"LMS::CheckDB -E-  dbase name is NULL"<<endl;
         rstatus = oocError;
      }
       return rstatus;
}
