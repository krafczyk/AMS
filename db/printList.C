// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//
// Last Edit : Oct 31, 1996. use listname as a parameter
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <rd45.h>

#include <dbevent_ref.h>
#include <list_ref.h>

#include <dbevent.h>
#include <list.h>

class LMS : public ooSession {
private:

public:
  LMS();
 ~LMS();
  void StartRead(ooMode mrowmode, const char *tag=NULL);
  void Commit();
  ooStatus Init(ooMode mode, ooMode mrowmode);
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

int main(int argc, char** argv)
{
 char*                  dbase = "MCEvents";
 char*                  list  = NULL;
 char*                  printMode = "S";

 const char *tag=NULL;
 ooMode      mode = oocRead;

 LMS                    lms;
 ooHandle(ooDBObj)      dbH;
 ooHandle(EventList)    listH;
 ooItr(EventList)       listItr;
 ooItr(dbEvent)         eventItr;

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
 dbH = lms.db("EventTag");
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

 lms.Commit();
}
