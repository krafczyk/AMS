// method source file for the object EventList
//
// last edit May 09, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

#include <db_comm.h>
#include <rd45.h>

#include <list_ref.h>
#include <list.h>

#include <dbevent_ref.h>
#include <dbevent.h>

EventList::EventList()
{
 resetCounters();  
}

EventList::EventList(char* listname)
{
  setlistname(listname);
  resetCounters();  
}

EventList::EventList(char* listname, char* setup)
{
  setlistname(listname);
  setsetupname(setup);
  resetCounters();  
}


void EventList::setlistname (char* listname)
{
  if (listname) {
    if (_listName) {
     cout<<"setlistname -W- listname of list "<<_listName
         <<" cannot be set to  "<<listname<<endl;
    } else {
    _listName = listname;
    }
  }
}

void EventList::setsetupname (char* setup)
{
  if (setup) {
    if (_setupName) {
     cout<<"setsetupname -W- setupname "<<_setupName
         <<" of list "<<_listName<<endl;
     cout<<"cannot be set to setupname "<<setup<<endl;
    } else {
     _setupName = setup;
    }
  }
}

ooStatus EventList::CheckContainer(char* name, ooMode mode, 
                                      ooHandle(ooContObj)& containerH)
{
     ooStatus rstatus = oocSuccess;
     ooHandle(ooDBObj)   databaseH;
     ooThis().containedIn(databaseH);

     if (name) {
      if (dbg_prtout == 1)
        cout<< "EventList:: CheckContainer : container "<<name
            << ", mode "<<mode<<endl;
      if (!containerH.exist(databaseH,name,mode)) {
       cout <<"EventList::CheckContainer -W- Cannot find or open"
            <<"  container "<<name<<" in mode "<<mode<<endl;
       rstatus = oocError;
      }
     } else {
       Warning("CheckContainer : container name is NULL");
       rstatus = oocError;
     }
      return rstatus;
}

void EventList::printListHeader()
{
  cout<<"List :  "<<ListName()<<endl;
  cout<<"with setup... "<<SetupName() <<" has "<<Events()<<" events"<<endl;
}

ooBoolean EventList::CheckListSstring(char* sstring)
{
  if (sstring) {
    if (strstr(_listName,sstring) != NULL) {
     return oocTrue;
    }
  }
  return oocFalse;
}

ooStatus EventList::PrintListStatistics(char* printMode)
{
     ooStatus               rstatus = oocSuccess;
     ooItr(dbEvent)         eventItr;
     ooHandle(EventList)    listH = ooThis();

     printListHeader();

     if (strcmp(printMode, "F") == 0 || strcmp(printMode, "f")== 0 ) {
       if (!listH.isUpdated()) {
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
         Error("PrintListStatistics : scan failed ");
       }
       } else {
         Message
    ("PrintListStatistics : list is opened in Update mode by another process");
       }
     }
   return rstatus;
}
