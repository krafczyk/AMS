#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <A_LMS.h>


ooStatus	LMS::PrintList(ooMode mode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooItr(AMSEventList)    listItr;
        

        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;

        _databaseH = _session -> DefaultDatabase();

        rstatus = listItr.scan(_databaseH, mode);
        if (rstatus == oocSuccess) {
          while (listItr.next()) {
            listItr -> PrintListStatistics();
          }
        } else {
          cout<<"LMS::PrintList-E- scan operation failed"<<endl;
        }

        if (rstatus != oocSuccess) {
          rstatus = Abort();
          return oocError;
        }

        rstatus = Commit();

	return rstatus;
}

ooStatus	LMS::PrintList(char* listName, ooMode mode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooHandle(AMSEventList) listH;
        ooItr(AMSEventList)    listItr;
        

        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;


        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         listH -> PrintListStatistics();
        } else {
         cout<<"LMS::PrintList-I- search for the list which has substring "
             <<listName<<endl;
         cout<<endl;
         _databaseH = _session -> DefaultDatabase();
         rstatus = listItr.scan(_databaseH, mode);
         if (rstatus == oocSuccess) {
          while (listItr.next()) {
           if (listItr -> CheckListSstring(listName)) {
            listItr -> PrintListStatistics();
           }            
          }
        } else {
          cout<<"LMS::PrintList-E- scan operation failed"<<endl;
        }
       }

        if (rstatus != oocSuccess) {
          rstatus = Abort();
          return oocError;
        }

        rstatus = Commit();

	return rstatus;
}
