#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <db_comm.h>
#include <A_LMS.h>


ooStatus	LMS::PrintList(ooMode mode, char* printMode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooItr(AMSEventList)    listItr;
        

        if (mode == oocRead) rstatus = Start(mode, oocMROW);
        if (mode != oocRead) rstatus = Start(oocRead);
        if (rstatus != oocSuccess) return rstatus;

        _databaseH = _session -> DefaultDatabase();

        rstatus = listItr.scan(_databaseH, oocNoOpen);
        if (rstatus == oocSuccess) {
         while (listItr.next()) {
          cout<<" "<<endl;
          if ( ((listItr -> EventType())/DBWriteMCEv)%2 != 1) {
             listItr -> PrintListStatistics(printMode);
          } else {
             listItr -> PrintListStatistics_mc(printMode);
          }
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

ooStatus	LMS::PrintList(char* listName, ooMode mode, char* printMode)
{
	ooStatus	       rstatus = oocError;	// Return status
        ooHandle(AMSEventList) listH;
        ooItr(AMSEventList)    listItr;
        

        if (mode == oocRead) rstatus = Start(mode, oocMROW);
        if (mode != oocRead) rstatus = Start(oocRead);
        if (rstatus != oocSuccess) return rstatus;

        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {
         listH -> PrintListStatistics(printMode);
        } else {
         cout<<"LMS::PrintList-I- search for the list which has substring "
             <<listName<<endl;
         cout<<endl;
         _databaseH = _session -> DefaultDatabase();
         rstatus = listItr.scan(_databaseH, mode);
         if (rstatus == oocSuccess) {
          while (listItr.next()) {
           if (listItr -> CheckListSstring(listName)) {
            cout<<" "<<endl;
            listItr -> PrintListStatistics(printMode);
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
