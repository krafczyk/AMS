// Last Edit : June 05, 1996. ak.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <typedefs.h>
#include <A_LMS.h>

ooStatus      LMS::DeleteEvent(char* listName, char* eventID)
{
	ooStatus	rstatus = oocError;	// Return status

        // Start the transaction
        rstatus = _session -> StartTransaction(oocUpdate);
        if (rstatus != oocSuccess) {
          cerr <<" LMS::DeleteEvent: Error - cannot start a transaction"<<endl;
           return rstatus;
        }

        // check list of events
        ooHandle(AMSEventList) listH;
        rstatus = FindEventList(listName, oocUpdate, listH);
        if(rstatus != oocSuccess) {
          cerr <<" LMS::DeleteEvent: Error - cannot open list with name "
               <<listName<<" (does not exist)"<<endl;
           _session -> AbortTransaction();
            return rstatus;
        }

        // print statistical information
        ooHandle(ooDBObj)        databaseH;
        ooItr(ooObj)             objectI;
        integer                  nevents;

        databaseH = listH.containedIn();

        nevents = 0;
        listH.contains(objectI);
        while (objectI.next()) { nevents++; }
        cout <<"LMS::DeleteEvent - I - there are "<<nevents<<" events";
        cout <<" in AMSEventList "<<listName<<endl;

        // delete event
        ooHandle(AMSEventD) eventH;
        //rstatus = listH -> DeleteEvent(eventID);
        if (rstatus != oocSuccess) {
          cerr <<"LMS::DeleteEvent: Error - cannot delete event with ID ";
          cerr <<eventID<<endl;
        }

        nevents = 0;
        listH.contains(objectI);
        while (objectI.next()) { nevents++; }
        cout <<"LMS::DeleteEvent - I - there are "<<nevents<<" events";
        cout <<" in AMSEventList (after delete)"<<listName<<endl;

        // commit the transaction
        rstatus = _session -> CommitTransaction();
        if (rstatus != oocSuccess) {
          cerr <<"LMS::DeleteEvent: Error - cannot commit a transaction";
          cerr <<endl;
           return rstatus;
        }
        return rstatus;
}
