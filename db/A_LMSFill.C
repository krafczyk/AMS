// May 29, 1996. ak.  Fill methods, objectivity first try
// Oct 02, 1996. ak.  remove most of Fill subroutines
// Dec 16, 1996. ak.  check mode before starting of transaction
// Mar 22, 1997. ak.  add tofdbc checking
//                    setup moved to AMSsetupDB
//
// last edit Mar 25, 1996, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <A_LMS.h>
#include <db_comm.h>

#include <event.h>
#include <job.h>

ooStatus   LMS::FillGeometry(char* listName)
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooMode                 mode = oocRead;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        if (mode == oocRead) rstatus = Start(mode,oocMROW);
        else rstatus = Start(mode);                 // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase(); // Get pointer to default 
        if (_databaseH != NULL) {                   // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
          strcpy(err_mess, "Cannot open setup dbase "); goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, mode, listH); // Check list
         if (rstatus == oocSuccess) {
         if(!listH -> CopyGeometry(mode, dbH)) {
 	  strcpy(err_mess, "Cannot copy geometry"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"CopyGeometry:: Error "<<err_mess<<endl;
          rstatus = Abort();
          return oocError;
        }
	return rstatus;
}

ooStatus   LMS::FillMaterial(char* listName)
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooMode                 mode = oocRead;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        if (mode == oocRead) rstatus = Start(mode,oocMROW);
        else rstatus = Start(mode); // Start the transaction
        if (rstatus != oocSuccess) return rstatus;

        _databaseH = _session -> DefaultDatabase(); // Get pointer to default 
        if (_databaseH != NULL) {                   // database

         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
          strcpy(err_mess, "Cannot open setup dbase "); goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;

         rstatus = FindEventList(listName, mode, listH); // Check list
         if (rstatus == oocSuccess) {
          if(!listH -> CopyMaterial(mode, dbH)) {
  	   strcpy(err_mess, "Cannot copy material"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"CopyMaterial:: Error "<<err_mess<<endl;
          rstatus = Abort();
          return oocError;
        }
	return rstatus;
}

ooStatus   LMS::FillTMedia(char* listName)
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooMode                 mode = oocRead;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        if (mode == oocRead) rstatus = Start(mode,oocMROW);
        else rstatus = Start(mode);                // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase(); // Get pointer to default 
        if (_databaseH != NULL) {                   // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
          strcpy(err_mess, "Cannot open setup dbase "); goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;

         rstatus = FindEventList(listName, mode, listH); // Check list
         if (rstatus == oocSuccess) {
          if(!listH -> CopyTMedia(mode, dbH)) {
 	  strcpy(err_mess, "Cannot copy TMedia"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"CopyTMedia:: Error "<<err_mess<<endl;
          rstatus = Abort();
          return oocError;
        }
	return rstatus;
}

void LMS::CheckConstants()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooMode               mode = oocRead;
        ooHandle(ooContObj)  contH;                   // container
        ooItr(AMSDBcD)       amsdbcItr;               // amsdbc
        ooItr(AMScommonsD)   commonsItr;              // commons
        ooItr(CTCDBcD)       ctcdbcItr;               // ctcdbc
        ooItr(TOFDBcD)       tofdbcItr;               // tofdbc
        ooHandle(ooDBObj)    dbH;
        char*                contName;

        if (mode == oocRead) rstatus = Start(mode,oocMROW);
        else rstatus = Start(mode);                  // Start the transaction
        if (rstatus != oocSuccess) goto error;

        _databaseH = _session -> DefaultDatabase();  // Get pointer to default
        if (_databaseH != NULL) {                    // database

         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
           cerr<<"LMS::CheckConstants -E- Cannot open setup dbase "<<endl; 
           goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;

          char* setup = AMSJob::gethead() -> getsetup();
          cout <<"LMS::CheckConstants -I- setup name "<<setup<<endl;
          contName = new char[strlen(setup)+8];
          strcpy(contName,"amsdbc_");
          strcat(contName,setup);
          if (!contH.exist(dbH, contName, oocRead)) {
           rstatus = oocError;
           cout <<"LMS::CheckConstants -E- cannot open a container"<<endl;
           goto error;
          }

      amsdbcItr.scan(contH, mode);
      if (amsdbcItr.next()) {
       cout <<"LMS::CheckConstants -I- check amsdbc"<<endl;
       rstatus = amsdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) goto error;
      } 
      
      integer Read = 0;
      commonsItr.scan(contH, mode);
      if (commonsItr.next()) {
       cout <<"LMS::CheckConstants -I- check commons"<<endl;
       rstatus = commonsItr -> CmpConstants(Read);
       if (rstatus != oocSuccess) {
        cerr <<"LMS::CheckConstants -E- Quit"<<endl;
        rstatus = oocError;
        goto error;
       }
      } 

      ctcdbcItr.scan(contH, mode);
      if (ctcdbcItr.next()) {
       cout <<"LMS::CheckConstants -I- check ctcdbc"<<endl;
       rstatus = ctcdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        cerr <<"LMS::CheckConstants -E- Quit"<<endl;
        rstatus = oocError;
        goto error;
       }
      } 


      tofdbcItr.scan(contH, mode);
      if (tofdbcItr.next()) {
       cout <<"LMS::CheckConstants -I- check tofdbc"<<endl;
       rstatus = tofdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        cerr <<"LMS::CheckConstants -E- Quit"<<endl;
        rstatus = oocError;
        goto error;
       }
      } 

        }
error:
        if (contName) delete [] contName;
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
          if (rstatus != oocSuccess) exit (1);
        } else {
          rstatus = Abort();
          exit(1);
        }
}

ooStatus   LMS::FillTDV(char* listName)
{
	ooStatus 	       rstatus = oocError;	// Return status
        char                   err_mess[80];
        ooMode                 mode = oocRead;
        time_t                 timeV;
        ooHandle(ooDBObj)      dbH;
        ooHandle(AMSEventList) listH;

	strcpy(err_mess, "Error Error");
        if (mode == oocRead) rstatus = Start(mode,oocMROW);
        else rstatus = Start(mode);                  // Start the transaction
        if (rstatus != oocSuccess) return rstatus;
        _databaseH = _session -> DefaultDatabase();  // Get pointer to default
        if (_databaseH != NULL) {                    // database
         rstatus = CheckDB("AMSsetupDB", mode, dbH); // Check setup dbase
         if (rstatus != oocSuccess || dbH == NULL) {
          strcpy(err_mess, "Cannot open setup dbase in oocUpdate mode");
          goto error;}
         if (_setupdbH != dbH) _setupdbH = dbH;
         rstatus = FindEventList(listName, mode, listH); // Check list
         if (rstatus == oocSuccess) {
          if(!listH -> CopyTDV(timeV, mode, dbH)) {
  	   strcpy(err_mess, "Cannot copy TDV"); goto error; }
         }
        }
error:
        if (rstatus == oocSuccess) {
	  rstatus = Commit(); 	           // Commit the transaction
        } else {
         cout <<"CopyTDV:: Error "<<err_mess<<endl;
         Abort();
        }
	return rstatus;
}
