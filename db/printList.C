//  $Id: printList.C,v 1.7 2001/01/22 17:32:30 choutko Exp $
// Last Edit 
//           
// Oct 22, 1996 ak. print content of database
//
// Last Edit : Jun 16, 1996. use listname as a parameter
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <rd45.h>

#include <dbcatalog_ref.h>
#include <dbevent_ref.h>
#include <list_ref.h>

#include <dbcatalog.h>
#include <dbevent.h>
#include <list.h>


class LMS : public ooSession {
private:

public:
  LMS();
 ~LMS();

  void ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH);
  void Commit();
  ooStatus Init(ooMode mode, ooMode mrowmode);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
};

LMS::LMS(){}

LMS::~LMS() {}

ooStatus LMS::Init(ooMode mode, ooMode mrowmode)
{
    ooStatus rstatus = oocSuccess;

    // if oosession has not been initialised do it now
    if (!ooSession::initialized()) ooSession::Init();
    return rstatus;
}

void LMS::StartRead(ooMode mode, const char *tag)
{
    if ((TransStart(mode,tag) != oocSuccess) || (setRead() != oocSuccess))
        Fatal("could not start transaction in read mode");
}

void LMS::Commit()
{
    if (TransCommit() != oocSuccess)
      Fatal("could not commit transaction");
}

void LMS::ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH)
//
// create all containers for MC database
//
{	
  integer              rstatus = 1;
  ooHandle(ooContObj)  contH;
  ooItr(AMSdbs)        dbTabItr;
  ooMode               openMode =   oocRead;
  
  dbTabH = NULL;

  if (contH.exist(dbH, "DbCatalog", openMode)) {
    dbTabItr.scan(contH, openMode);
    if (dbTabItr.next()) {
      dbTabH = dbTabItr;
    } else {
      Fatal
       ("DbCatalog container is empty. Cannot create the object in Read mode");
    }
  } else {
   Fatal("DbCatalog container does not exist");
  }
}

implement (ooVArray, ooRef(ooDBObj))

int main(int argc, char** argv)
{
 char*                  dbase = "MCEvents";
 char*                  list  = NULL;
 char*                  printMode = "S";
 char*                  cptr;

 const char *tag=NULL;
 ooMode      mode = oocRead;

 LMS                    lms;
 ooHandle(ooDBObj)      dbH;
 ooHandle(ooDBObj)      _catdbH;
 ooHandle(EventList)    listH;
 ooItr(EventList)       listItr;
 ooItr(dbEvent)         eventItr;

 cptr = getenv("OO_FD_BOOT");
 if (!cptr) Fatal("environment OO_FD_BOOT in undefined");

 cout<<" "<<endl;
 if(argc > 2)  list  = argv[2];
 if(argc > 1)  printMode = argv[1];

 if (argc > 2) {
   cout<<"database EventTag, list "<<list<<", mode "<<printMode<<endl;
 } else {
   cout<<"database EventTag, all lists, mode "<<printMode<<endl;
 }

 
 // if oosession has not been initialised do it now
 lms.Init(oocRead,oocMROW);
 lms.StartRead(oocMROW);

 ooHandle(AMSdbs)        dbTabH;

 _catdbH = lms.db("DbList");
 lms.ContainersC(_catdbH, dbTabH);

 if (dbTabH == NULL) Fatal("ClusteringInit: catalog of databases not found");


 // get TagDB pathname
 cptr = getenv("AMS_TagDB_Path");
 if ( cptr ) 
   cout<<"ClusteringInit: TagDB path name "<<cptr<<endl;
 else
  cout<<"ClusteringInit: TagDB path name "<<AMSdbs::pathNameTab[dbtag]<<endl;

 integer ntagdbs = dbTabH -> size(dbtag);

 for (int j = 0; j < ntagdbs; j++) {
  dbH = dbTabH -> getDB(dbtag,j);
  if (!dbH) Fatal ("Pointer to selected dbase is NULL");
  ooStatus rstatus = listItr.scan(dbH, oocNoOpen);
  if (rstatus == oocSuccess) {
   while (listItr.next()) {
    cout<<" "<<endl;
    listItr -> printListHeader();
    listH = listItr;
    if (strcmp(printMode, "F") == 0 || strcmp(printMode, "f")== 0 ) {
     rstatus = eventItr.scan(listH,oocRead);
     if (rstatus == oocSuccess) {
      integer run     = 0;
      integer nevents = 0;
      while(eventItr.next()) {
       integer r = eventItr -> Run();
       if (r == run) {
        nevents++;
       } else {
        if (run != 0) cout<<"run "<<run<<", events "<<nevents<<endl;
        run = r;
        nevents = 1;
       }
      }
        cout<<"run "<<run<<", events "<<nevents<<endl;
     } else {
       Error("PrintList : scan failed ");
     }
    }
   }       
  } else {
   Error("LMS::PrintList : scan operation failed");
  }

 }
 lms.Commit();
}
