// Last Edit : Mar 26, 1997. ak.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <typedefs.h>
#include <A_LMS.h>

ooStatus      LMS::DeleteEvent(char* listName, integer run, uinteger event)
{
	ooStatus	rstatus = oocError;	// Return status
        ooHandle(AMSEventD)    eventH;
        ooHandle(AMSmceventD)  mceventH;

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
        
        if (listH -> EventType() > 10000) {
          rstatus = listH -> FindEvent(run, event, oocUpdate, mceventH);
          if (rstatus == oocSuccess) {
            ooDelete(mceventH);
          }
        } else {
          rstatus = listH -> FindEvent(run, event, oocUpdate, eventH);
          if (rstatus == oocSuccess) {
            ooDelete(eventH);
          }
        }
        if (rstatus != oocSuccess) {
          cerr <<"LMS::DeleteEvent: Error - cannot find event "<<event
               <<", run "<<run<<endl;
        }
        // commit the transaction
        rstatus = _session -> CommitTransaction();
        if (rstatus != oocSuccess) {
          cerr <<"LMS::DeleteEvent: Error - cannot commit a transaction"
               <<endl;
           return rstatus;
        }
        return rstatus;
}

ooStatus      LMS::DeleteSetup(char* setup) {

	ooStatus 	     rstatus = oocError;	// Return status
        char*                contName;
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)    dbH;
        ooHandle(ooContObj)  contH;

        rstatus = Start(oocUpdate); // Start the transaction
        if (rstatus != oocSuccess) return rstatus;

        rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
        if (rstatus != oocSuccess || dbH == NULL) {
          cerr<<"DeleteSetup -E- Cannot open setup dbase in oocUpdate mode"
              <<endl;
          return oocError;
        }

// delete geometry container
        contName = new char[strlen(setup)+10];
        strcpy(contName,"Geometry_");
        strcat(contName,setup);
        if (contH.exist(dbH,contName,mode)) ooDelete(contH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;                    

// delete materials container
        contName = new char[strlen(setup)+11];
        strcpy(contName,"Materials_");
        strcat(contName,setup);
        if (contH.exist(dbH,contName,mode)) ooDelete(contH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete tmedia container
        contName = new char[strlen(setup)+8];
        strcpy(contName,"TMedia_");
        strcat(contName,setup);
        if (contH.exist(dbH,contName,mode)) ooDelete(contH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete amsdbc container
        contName = new char[strlen(setup)+8];
        strcpy(contName,"amsdbc_");
        strcat(contName,setup);
        if (contH.exist(dbH,contName,mode)) ooDelete(contH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;

// delete TDV container
        contName = new char[strlen(setup)+10];
        strcpy(contName,"Time_Var_");
        strcat(contName,setup);
        if (contH.exist(dbH,contName,mode)) ooDelete(contH);
        cout<<"Container "<<contName<<" is deleted "<<endl;
        delete [] contName;
        
        rstatus = oocSuccess;

error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         rstatus = Abort();                // or Abort it
        }

        return rstatus;
}
