// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//

#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <A_LMS.h>



ooMode               _openMode = oocUpdate;

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;

LMS                  dbout(oocUpdate);

int main()
{
 integer                i, N = 0, Ncl = 0;
 char*                  listName = NULL;
 ooStatus               rstatus;
 ooHandle(ooDBObj)      _databaseH;
 ooHandle(AMSEventList) listH;        


 listName = dbout.PromptForValue("Enter List Name: ");
 
    rstatus = dbout.Start(_openMode);
    if (rstatus == oocSuccess) {
     rstatus = dbout.FindEventList(listName, _openMode, listH);
     if (rstatus == oocSuccess) {
       rstatus = listH -> DeleteAllContainers();
       rstatus = listH -> DeleteMap();
       if (rstatus == oocSuccess) ooDelete(listH);
     }
 } else {
   cout <<"cannot start a transaction "<<endl;
 }

 if (rstatus == oocSuccess) {
   rstatus = dbout.Commit();
   cout <<"LMS::Constructor Commit Done"<<endl;
  } else {
   rstatus = dbout.Abort();
  }
 
}
