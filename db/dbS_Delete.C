// Last Edit : Mar 26, 1997. ak.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>

#include <typedefs.h>
#include <dbS.h>


ooStatus      LMS::DeleteSetup(char* setup) {

	ooStatus 	     rstatus = oocError;	// Return status
        char*                contName;
        ooMode               mode = oocUpdate;
        ooHandle(ooDBObj)    dbH;
        ooHandle(ooContObj)  contH;

        StartUpdate(); // Start the transaction

        dbH = setupdb(); // Check setup dbase
        if (dbH == NULL) 
          Fatal("DeleteSetup: Cannot open setup dbase in oocUpdate mode");

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
	  Commit(); 	           // Commit the transaction
        } else {
        Abort();                // or Abort it
        }

        return rstatus;
}

