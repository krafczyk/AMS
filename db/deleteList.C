// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//
// Last Edit : Mar 11, 1997. ak.
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <db_comm.h>
#include <A_LMS.h>



ooMode               _openMode = oocUpdate;

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;
implement (ooVArray, AMSTOFMCClusterD) //;
implement (ooVArray, AMSTrMCClusterD) //;

LMS                  dbout(oocUpdate);

int main()
{
 char*                  listName = NULL;
 char*                  yesno = NULL;
 ooStatus               rstatus;
 ooHandle(ooDBObj)      _databaseH;
 ooHandle(AMSEventList) listH;        
 ooItr(AMSEventList)    listItr;


 listName = dbout.PromptForValue("Enter List Name: ");
 ooSession * _session  = ooSession::Instance("LOM");
 
    rstatus = dbout.Start(_openMode);
    if (rstatus != oocSuccess) goto end;
    _databaseH = _session -> DefaultDatabase();
    if (_databaseH == NULL ) {
     cout <<" Error - pointer to default database is NULL"<<endl;
     rstatus = oocError;
     goto end;
    }
     rstatus = dbout.FindEventList(listName, _openMode, listH);
     if (rstatus == oocSuccess) {
       goto deletelist;
     } else {
      cout <<" search for the list which has substring "<<listName<<endl;
      rstatus = listItr.scan(_databaseH, _openMode);
      if (rstatus == oocSuccess) {
       while (listItr.next()) {
        if (listItr -> CheckListSstring(listName)) {
         listItr -> PrintListStatistics(" ");
         yesno = dbout.PromptForValue("Do you want to delete this list ? ");
         if (yesno) { 
          if (strstr("No",yesno) || 
              strstr("NO",yesno) || strstr("no", yesno)) {
          } else {
           listH = listItr;
           if (listName) delete [] listName;
           listName = new char[strlen(listItr ->ListName())];
           strcpy(listName,listItr -> ListName());
           goto deletelist;
          }
         }           
        }
       }
      }
     }
    rstatus = oocError;
    goto end;

deletelist:
     listH -> SetContainersNames();
     rstatus = listH -> DeleteAllContainers();
     ooDelete(listH);

end:
 if (rstatus == oocSuccess) {
   rstatus = dbout.Commit();
   cout <<"Commit Done"<<endl;
  } else {
   rstatus = dbout.Abort();
   cout <<"Transaction aborted"<<endl;
  }
 
}
