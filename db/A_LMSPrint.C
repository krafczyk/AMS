#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <A_LMS.h>

void	LMS::PrintEvent(char* listName, char* eventID)
{
        ooStatus rstatus;
    
        rstatus = _session -> StartTransaction(oocRead);
        if (rstatus != oocSuccess) {
         cerr << "LMS::PrintEvent: ERROR - cannot start a transaction"<<endl;
          return;
        }

        ooHandle(AMSEventList) listH;
        if(!listH.exist(_databaseH, listName, oocRead)) {
         cerr << "LMS::PrintEvent: ERROR - cannot open "<<listName<<endl;
          _session -> AbortTransaction();
           return;
        }

        ooHandle(AMSEventD)       eventH;
        if (listH -> FindEvent(eventID, oocRead, eventH)) {
          eventH -> Print();
        } else {
         cerr << "LMS::PrintEvent: ERROR - cannot find event with ID ";
         cerr <<eventID<<endl;
        }

        rstatus = _session -> CommitTransaction();
        if (rstatus != oocSuccess) {
         cerr << "LMS::PrintEvent: ERROR - cannot commit a transaction"<<endl;
         return;
        }
}


void	LMS::PrintList(char* listName)
{
	// Display not implemented message (to be implemented in LAB 6)

	cout << endl;
	cout << "WARNING: LMS::PrintList(" << listName << "'";
	cout << ") is not implemented." << endl;
}
