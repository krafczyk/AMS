// Last Edit 
//           May 24, 1996 ak. extracted from geant.C
//           Sep 30, 1996 ak.
//           Mar 25, 1997 ak.
//
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
   ooHandle(ooDBObj)    _databaseH;
   ooHandle(ooDBObj)    _setupdbH;

   _session  = ooSession::Instance("LOM");
   _openMode = oocUpdate;
   rstatus = Start(_openMode);
   if (rstatus == oocSuccess) {
    _databaseH = _session -> DefaultDatabase();
    _setupdbH   = new ooDBObj("AMSsetupDB",0,0);
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
