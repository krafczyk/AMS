//  $Id: dbA_Print.C,v 1.2 2001/01/22 17:32:28 choutko Exp $
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <db_comm.h>
#include <dbA.h>


ooStatus	LMS::PrintList(ooHandle(ooDBObj)& dbH, char* printMode,
                               char* name)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooHandle(EventList)    listH;
        ooItr(EventList)       listItr;
        

        StartRead(oocMROW);

        if (strcmp(name, "unknown") == 1) {
         rstatus = listItr.scan(dbH, Mode());
         if (rstatus == oocSuccess) {
          while (listItr.next()) {
           cout<<" "<<endl;
             listItr -> PrintListStatistics(printMode);
          }
         } else {
          Error("PrintList : scan operation failed");
          rstatus = oocError;
         }
        } else {
         rstatus = FindEventList(dbH, name, listH);
         if (rstatus == oocSuccess) {
          listH -> PrintListStatistics(printMode);
         } else {
          cout<<"PrintList-I- search for the list which has substring "
              <<name<<endl;
          cout<<endl;
          rstatus = listItr.scan(dbH, Mode());
          if (rstatus == oocSuccess) {
           while (listItr.next()) {
            if (listItr -> CheckListSstring(name)) {
             cout<<" "<<endl;
             listItr -> PrintListStatistics(printMode);
            }            
           }
          } else {
           Error("PrintList : scan operation failed");
           rstatus = oocError;
          }
         }
        }

        Commit();
        return rstatus;
}        
