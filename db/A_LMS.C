// Methods for DBASE management file, objectivity first try
// Aug 23, 1996. ak. change the initial number of pages, allocated for the 
//                   containers, didn't see any improvement, so return back to
//                   the default value.
// Sep , 1996   ak.  Add Geometry and TMedia containers
//                   Count number of started and commited transactions
// Oct 01, 1996 ak.  create map per list
//                   use Geometry_SETUPName container
// Oct 17, 1996. ak. container per list
//
// last edit Oct 17, 1996, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <typedefs.h>
#include <A_LMS.h>

LMS::LMS(ooMode openMode) 
{	
  integer  i;
  ooStatus rstatus;

   cout << "LMS::Constructor: ========Constructor called========" << endl;

   _session  = ooSession::Instance("LOM");
   _openMode = openMode;
   cout << "LMS::Constructor: Database is initialized" << endl;
   //rstatus   = _session -> StartTransaction(_openMode);
   rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH != NULL) {
/*
//     if (!_mapsH.exist(_session -> DefaultDatabase(),"Maps",_openMode)) {
//      _mapsH = new("Maps", 1, 0, 0, _databaseH) ooContObj;
//      //_eventMapH = new (_mapsH) ooMap;
//      //rstatus   = _eventMapH.nameObj(_mapsH, "RawEvents");
//      //if (rstatus != oocSuccess) cerr 
//      // << "LMS::Constructor:Error - Cannot name the map RawEvents"<< endl;
//     }
//     cout << "LMS::Constructor: Maps are opened/created" << endl;
//
//   // create the "mcEventg" container, if it does not exist
//    ooHandle(ooContObj)  contmceventgH;
//    if (!contmceventgH.exist(_databaseH, "mcEventg", _openMode)) {
//    contmceventgH   = new("mcEventg",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: mcEventg is opened/created" << endl;
//
//   // create the "TrLayersN" containers, if it does not exist
//    char       nameL[6][9] = {
//        "TrLayer1","TrLayer2","TrLayer3","TrLayer4","TrLayer5","TrLayer6"};
//    if (!_trLayersH[0].exist(_databaseH, nameL[0], _openMode)) {
//     for (i = 0; i<6; i++) {
//      _trLayersH[i]    = new(nameL[i],1,0,0,_databaseH) ooContObj;}
//    }
//    cout << "LMS::Constructor: TrLayers are opened/created" << endl;
//
//   // create the "TrClusterN" container, if it does not exist
//    char       nameC[2][11] = {"TrClusterX","TrClusterY"};
//    if (!_trClusterH[0].exist(_databaseH, nameC[0], _openMode)) {
//    for (i=0; i<2; i++) {
//     _trClusterH[i]    = new(nameC[i],1,0,0,_databaseH) ooContObj;}
//    }
//    cout << "LMS::Constructor: TrClusters are opened/created" << endl;
//
//    ooHandle(ooContObj) trMCClusterH;
//    if (!trMCClusterH.exist(_databaseH,"TrMCCluster", _openMode)) {
//     trMCClusterH    = new("TrMCCluster",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: TrMCClusters are opened/created" << endl;
//
//    ooHandle(ooContObj) TOFMCClusterH;
//    if (!TOFMCClusterH.exist(_databaseH,"TOFMCCluster", _openMode)) {
//     TOFMCClusterH    = new("TOFMCCluster",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: TOFMCClusters are opened/created" << endl;
//
//   // create the "ScLayersN" containers, if the do not exist
//    char       nameSc[4][9] = {"ScLayer1","ScLayer2","ScLayer3","ScLayer4"};
//    if (!_scLayersH[0].exist(_databaseH, nameSc[0], _openMode)) {
//     for (i = 0; i<4; i++) {
//      _scLayersH[i]    = new(nameSc[i],1,0,0,_databaseH) ooContObj; }
//    }
//    cout << "LMS::Constructor: ScLayers are opened/created" << endl;
//
//
//   // create the "CTCCluster" container, if it does not exist
//    ooHandle(ooContObj) contCTCClusterH;
//    if (!contCTCClusterH.exist(_databaseH,"CTCCluster", _openMode)) {
//     contCTCClusterH    = new("CTCCluster",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: CTCClusters are opened/created" << endl;
//
//    ooHandle(ooContObj) CTCMCClusterH;
//    if (!CTCMCClusterH.exist(_databaseH,"CTCMCCluster", _openMode)) {
//     CTCMCClusterH    = new("CTCMCCluster",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: CTCMCClusters are opened/created" << endl;
//
//    // create the "TrTracks" container, if it does not exist
//    ooHandle(ooContObj)  contTrackH;
//    if (!contTrackH.exist(_databaseH, "TrTracks", oocUpdate)) {
//     contTrackH    = new("TrTracks",1,0,0,_databaseH) ooContObj; }
//    cout << "LMS::Constructor: TrTracks is opened/created" << endl;
//
//    // create the "Beta" container, if it does not exist
//    ooHandle(ooContObj)  contBetaH;
//    if (!contBetaH.exist(_databaseH, "Beta", oocUpdate)) {
//     contBetaH    = new("Beta",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: Beta is opened/created" << endl;
//
//    // create the "Charge" container, if it does not exist
//    ooHandle(ooContObj)  contChargeH;
//    if (!contChargeH.exist(_databaseH, "Charge", oocUpdate)) {
//     contChargeH    = new("Charge",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: Charge is opened/created" << endl;
//
//    // create the "Particle" container, if it does not exist
//    ooHandle(ooContObj)  contParticleH;
//    if (!contParticleH.exist(_databaseH, "Particle", oocUpdate)) {
//     contParticleH    = new("Particle",1,0,0,_databaseH) ooContObj;}
//    cout << "LMS::Constructor: Particle is opened/created" << endl;
//
*/
 } else {
  cout <<"LMS::Constructor: Error DefaultDatabase does not exist"<<endl;
  cout << "LMS::Constructor: pointer to default database is null"<<endl;
  rstatus = Abort();
  return;
 }
} else {
    cerr << "LMS::Constructor: Error - Cannot start a transaction"<<endl;
    rstatus = Abort();
    exit(1);
}
 
 rstatus = Commit();
/*
 ooHandle(AMSEventList) listH;
 cout <<"LMS::Constructor -I- Open/Create EventKeeping list"<<endl;
 rstatus = AddList("EventKeeping","Setup", -1, listH);
 //if (rstatus != oocSuccess) 
 //cout<<"LMS::Constructor-E- failed to add EventKeeping List"<<endl;
*/
 if (rstatus == oocSuccess) cout <<"LMS::Constructor Commit Done"<<endl;
}

LMS::~LMS() { if (fListName) delete fListName;}


ooStatus LMS::Start(ooMode mode)   // start a transaction
{
  ooStatus rstatus = oocError;

        rstatus = _session -> StartTransaction(mode);
	if (rstatus != oocSuccess) {
	   cerr << "LMS::Start -Error - Cannot start a transaction." << endl;
	} else {
          //cout << "LMS::Start -I- start a transaction." << endl;
           _transStart++;
        }
        return rstatus;
}

ooStatus LMS::Commit()            // commit a transaction
{
  ooStatus rstatus = oocError;

        rstatus = _session -> CommitTransaction();
	if (rstatus != oocSuccess) {
	   cerr << "LMS::Commit - Error- Cannot commit a transaction." << endl;
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

ooStatus LMS::LMSInit(ooMode openMode, char* prefix) 
{	
  integer              i;
  ooStatus             rstatus;
  ooHandle(ooContObj)  contH;
  char*                contName;

   _openMode = openMode;
   rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH != NULL) {

    contName = new char[strlen(prefix)+5];
    strcpy(contName,"Maps_");
    strcat(contName,prefix);
    if (!_mapsH.exist(_session -> DefaultDatabase(),contName,_openMode)) {
      _mapsH = new(contName, 1, 0, 0, _databaseH) ooContObj;
     }
    delete [] contName;

   // create the "mcEventg" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"mceventg_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode)) {
    contH   = new(contName,1,0,0,_databaseH) ooContObj;}
    delete [] contName;
    cout << "LMS::Constructor: mcEventg is opened/created" << endl;

   // create the "TrLayersN" containers, if it does not exist
    char       nameL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
     for (i = 0; i<6; i++) {
      contName = new char[strlen(prefix)+10];
      strcpy(contName,nameL[i]);
      strcat(contName,prefix);
      if (!_trLayersH[i].exist(_databaseH, contName, _openMode)) {
      _trLayersH[i]    = new(contName,1,0,0,_databaseH) ooContObj;}
      delete [] contName;
     }
     cout << "LMS::Constructor: TrLayers are opened/created" << endl;

   // create the "TrClusterN" container, if it does not exist
    char       nameC[2][12] = {"TrClusterX_","TrClusterY_"};

    for (i=0; i<2; i++) {
     contName = new char[strlen(prefix)+12];
     strcpy(contName,nameC[i]);
     strcat(contName,prefix);
     if (!_trClusterH[0].exist(_databaseH, contName, _openMode)) {
      _trClusterH[i]    = new(contName,1,0,0,_databaseH) ooContObj;}
     delete [] contName;
    }
    cout << "LMS::Constructor: TrClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+13];
    strcpy(contName,"TrMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: TrMCClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+14];
    strcpy(contName,"TOFMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, _openMode)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: TOFMCClusters are opened/created" << endl;

   // create the "ScLayersN" containers, if the do not exist
    char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
    for (i = 0; i<4; i++) {
     contName = new char[strlen(prefix)+10];
     strcpy(contName,nameSc[i]);
     strcat(contName,prefix);
     if (!_scLayersH[0].exist(_databaseH, contName, _openMode)) 
      _scLayersH[i]    = new(contName,1,0,0,_databaseH) ooContObj; 
     delete [] contName;
    }
    cout << "LMS::Constructor: ScLayers are opened/created" << endl;


   // create the "CTCCluster" container, if it does not exist
    contName = new char[strlen(prefix)+12];
    strcpy(contName,"CTCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: CTCClusters are opened/created" << endl;

    contName = new char[strlen(prefix)+14];
    strcpy(contName,"CTCMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: CTCMCClusters are opened/created" << endl;

    // create the "TrTracks" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"TrTracks_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, oocUpdate)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj; 
    delete [] contName;
    cout << "LMS::Constructor: TrTracks is opened/created" << endl;

    // create the "Beta" container, if it does not exist
    contName = new char[strlen(prefix)+6];
    strcpy(contName,"Beta_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, oocUpdate)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
    delete [] contName;
    cout << "LMS::Constructor: Beta is opened/created" << endl;

    // create the "Charge" container, if it does not exist
    contName = new char[strlen(prefix)+8];
    strcpy(contName,"Charge_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, oocUpdate)) {
     contH    = new(contName,1,0,0,_databaseH) ooContObj;}
    delete [] contName;
    cout << "LMS::Constructor: Charge is opened/created" << endl;

    // create the "Particle" container, if it does not exist
    contName = new char[strlen(prefix)+10];
    strcpy(contName,"Particle_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH, contName, oocUpdate)) {
     contH    = new(contName,1,0,0,_databaseH) ooContObj;}
    delete [] contName;
    cout << "LMS::Constructor: Particle is opened/created" << endl;

    contName = new char[strlen(prefix)+15];
    strcpy(contName,"AntiMCCluster_");
    strcat(contName,prefix);
    if (!contH.exist(_databaseH,contName, _openMode)) 
     contH    = new(contName,1,0,0,_databaseH) ooContObj;
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
