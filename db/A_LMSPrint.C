#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <db_comm.h>
#include <A_LMS.h>


ooStatus	LMS::PrintList(ooHandle(ooDBObj)& dbH, char* printMode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooItr(AMSEventList)    listItr;
        

        StartRead(oocMROW);

        rstatus = listItr.scan(dbH, oocNoOpen);
        if (rstatus == oocSuccess) {
         while (listItr.next()) {
          cout<<" "<<endl;
             listItr -> PrintListStatistics(printMode);
         }
        } else {
         Error("LMS::PrintList-E- scan operation failed");
        }

        Commit();

	return rstatus;
}

ooStatus	LMS::PrintList
                 (ooHandle(ooDBObj)& dbH, char* listName, char* printMode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooHandle(AMSEventList) listH;
        ooItr(AMSEventList)    listItr;
        

        StartRead(oocMROW);

        rstatus = FindEventList(dbH, listName, listH);
        if (rstatus == oocSuccess) {
         listH -> PrintListStatistics(printMode);
        } else {
         cout<<"LMS::PrintList-I- search for the list which has substring "
             <<listName<<endl;
         cout<<endl;
         rstatus = listItr.scan(dbH, Mode());
         if (rstatus == oocSuccess) {
          while (listItr.next()) {
           if (listItr -> CheckListSstring(listName)) {
            cout<<" "<<endl;
            listItr -> PrintListStatistics(printMode);
           }            
          }
        } else {
          Error("LMS::PrintList : scan operation failed");
        }
       }

        if (rstatus != oocSuccess) {
          Abort();
          return oocError;
        }

        Commit();

	return oocSuccess;
}
