// Last Edit : Oct 25, 1996. ak.

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

ooStatus      LMS::DeleteSetup(char* setup) {

	ooStatus 	     rstatus = oocError;	// Return status
        char*                contName;
        ooMode               mode = oocUpdate;
        ooHandle(ooContObj)  containerH;

//
// Start the transaction
        rstatus = Start(oocUpdate);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {

// delete geometry container
        contName = new char[strlen(setup)+10];
        strcpy(contName,"Geometry_");
        strcat(contName,setup);
        if (containerH.exist(_databaseH,contName,mode)) ooDelete(containerH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete materials container
        contName = new char[strlen(setup)+11];
        strcpy(contName,"Materials_");
        strcat(contName,setup);
        if (containerH.exist(_databaseH,contName,mode)) ooDelete(containerH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete tmedia container
        contName = new char[strlen(setup)+8];
        strcpy(contName,"TMedia_");
        strcat(contName,setup);
        if (containerH.exist(_databaseH,contName,mode)) ooDelete(containerH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete amsdbc container
        contName = new char[strlen(setup)+8];
        strcpy(contName,"amsdbc_");
        strcat(contName,setup);
        if (containerH.exist(_databaseH,contName,mode)) ooDelete(containerH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;
        
        rstatus = oocSuccess;
      }






error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         rstatus = Abort();  // or Abort it
        }

        return rstatus;
}
