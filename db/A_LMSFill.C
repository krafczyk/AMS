// May 29, 1996. ak.  Fill methods, objectivity first try
// Oct 02, 1996. ak.  remove most of Fill subroutines
//
// last edit Nov 26, 1996, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <typedefs.h>
#include <A_LMS.h>

#include <event.h>
#include <job.h>

ooStatus   LMS::FillGeometry(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocRead;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
// Check for the list
        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {

        ooMode  lmode = listH.openMode();
        if(lmode == oocNoOpen) {
         if(!listH.open(_databaseH, listName, mode)) {
	  strcpy(err_mess, "Cannot open the list in mode "); goto error;}
        } 
    
        if(!listH -> CopyGeometry(mode)) {
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

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus   LMS::FillMaterial(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocRead;
        ooHandle(AMSEventList) listH;

//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
// Check for the list
        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {

        ooMode  lmode = listH.openMode();
        if(lmode == oocNoOpen) {
         if(!listH.open(_databaseH, listName, mode)) {
	  strcpy(err_mess, "Cannot open the list in mode "); goto error;}
        } 
    
        if(!listH -> CopyMaterial(mode)) {
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

	// Return the status (oocSuccess or oocError)
	return rstatus;
}

ooStatus   LMS::FillTMedia(char* listName)
{
	ooStatus 	     rstatus = oocError;	// Return status
        char                 err_mess[80];
        ooMode               mode = oocRead;
        ooHandle(AMSEventList) listH;


//
	strcpy(err_mess, "Error Error");
// Start the transaction
        rstatus = Start(mode);
        if (rstatus != oocSuccess) return rstatus;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
// Check for the list
        rstatus = FindEventList(listName, mode, listH);
        if (rstatus == oocSuccess) {

        ooMode  lmode = listH.openMode();
        if(lmode == oocNoOpen) {
         if(!listH.open(_databaseH, listName, mode)) {
	  strcpy(err_mess, "Cannot open the list in mode "); goto error;}
        } 
    
        if(!listH -> CopyTMedia(mode)) {
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

	// Return the status (oocSuccess or oocError)
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

//
// Start the transaction
        rstatus = Start(mode);
        if (rstatus != oocSuccess) return;
// Get pointer to default database
        _databaseH = _session -> DefaultDatabase();
        if (_databaseH != NULL) {
         char* setup = AMSJob::gethead() -> getsetup();
         cout <<"LMS::CheckConstants -I- setup name "<<setup<<endl;
         char* contName = new char[strlen(setup)+8];
         strcpy(contName,"amsdbc_");
         strcat(contName,setup);
         if (!contH.exist(_databaseH, contName, oocRead)) {
          rstatus = oocError;
          cout <<"AMSEventList::CheckConstants -E- cannot open a container"
               <<endl;
          goto error;
         }

      amsdbcItr.scan(contH, mode);
      if (amsdbcItr.next()) {
       cout <<"AMSEventList::CheckConstants -I- check amsdbc"<<endl;
       rstatus = amsdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) goto error;
      } 
      
      integer Read = 0;
      commonsItr.scan(contH, mode);
      if (commonsItr.next()) {
       cout <<"AMSEventList::CheckConstants -I- check commons"<<endl;
       rstatus = commonsItr -> CmpConstants(Read);
       if (rstatus != oocSuccess) {
        cout <<"AMSEventList::CheckConstants -E- Quit"<<endl;
        rstatus = oocError;
        goto error;
       }
      } 

      ctcdbcItr.scan(contH, mode);
      if (ctcdbcItr.next()) {
       cout <<"AMSEventList::CheckConstants -I- check ctcdbc"<<endl;
       rstatus = ctcdbcItr -> CmpConstants();
      } 

        }
error:
	  rstatus = Commit(); 	           // Commit the transaction
          if (rstatus != oocSuccess) exit (1);
}
