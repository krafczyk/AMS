// Last Edit 
//           May 24, 1996 ak. extracted from geant.C
//           Sep 30, 1996 ak.

#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>

        ooSession *          _session;
        ooMode               _openMode;

ooStatus Start(ooMode mode)   // start a transaction
{
  ooStatus rstatus = oocError;
        char*                fListName;    // name of event's list

        rstatus = _session -> StartTransaction(mode);
	if (rstatus != oocSuccess) {
	   cerr << "LMS::Start -Error - Cannot start a transaction." << endl;
	}
        
        return rstatus;
}

ooStatus Commit()            // commit a transaction
{
  ooStatus rstatus = oocError;

        rstatus = _session -> CommitTransaction();
	if (rstatus != oocSuccess) {
	   cerr << "LMS::Commit - Error- Cannot commit a transaction." << endl;
	}

	return rstatus;
}

int main()
{
   ooStatus rstatus;
   integer  i;
        ooHandle(ooDBObj)    _databaseH;
        ooHandle(ooContObj)  _trClusterH[2];
        ooHandle(ooContObj)  _trLayersH[6];
        ooHandle(ooContObj)  _scLayersH[4];
        ooHandle(ooContObj)  _mapsH;       // container of maps
        ooHandle(ooMap)      _eventMapH;   // maps of events
        ooHandle(ooContObj)  _betaH;
        ooHandle(ooContObj)  contCTCClusterH;
        ooHandle(ooContObj)  contTrackH;

   cout << "LMS:: main ========  Create containers =======" << endl;

   _session  = ooSession::Instance("LOM");
   _openMode = oocUpdate;
   rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH != NULL) {
      //if (!_mapsH.exist(_session -> DefaultDatabase(),"Maps",_openMode)) {
      //_mapsH = new("Maps", 1, 0, 0, _databaseH) ooContObj;
      //_eventMapH = new (_mapsH) ooMap;
      //rstatus   = _eventMapH.nameObj(_mapsH, "RawEvents");
      //if (rstatus != oocSuccess) {
      //cerr << "LMS:::Error - Cannot name the map RawEvents"<< endl;
      //}
    }
    //cout << "LMS:: Maps are opened/created" << endl;

 } else {
  cout <<"LMS:: Error DefaultDatabase does not exist"<<endl;
  cout << "LMS:: pointer to default database is null"<<endl;
  _session -> AbortTransaction();
   goto error;
 } 

error:
  if (rstatus == oocSuccess) {
   rstatus = Commit();
  if (rstatus == oocSuccess) cout <<"LMS::Constructor Commit Done"<<endl;
  } else {
    cerr << "LMS:: Error - Cannot start a transaction"<<endl;
    _session -> AbortTransaction();
     goto error;
  }
}
