//  $Id: dbA_Delete.C,v 1.3 2001/01/22 17:32:28 choutko Exp $
// Delete methods. June, 1996. ak.
//
// Last Edit : Nov 13, 1997. ak.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>

#include <typedefs.h>
#include <dbA.h>
#include <rd45.h>

integer DeleteContainer(ooHandle(ooDBObj) & dbH, char* contName, ooMode mode)
// error     : return -1 
// not found : return 0
// success   : return 1
{
  integer                    rstatus = -1;	// Return status
  ooHandle(ooContObj)        contH;                

    
   if (contH.exist(dbH,contName,mode)) {
    ooDelete(contH);
    cout<<"DeleteContainer : "<<contName<<" is deleted "<<endl;
    rstatus = 1;
   } else {
    rstatus = 0;
   }
  return rstatus;
}

ooStatus   LMS::DeleteTDVContainer()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooHandle(AMSdbs)           dbTabH;
        ooHandle(ooDBObj)          dbH;

  ooMode mode = oocUpdate;
  char* contName = StrDup("Time_Dep_Var");
  StartUpdate(); // Start the transaction
  dbTabH = Tabdb();
  if (dbTabH != NULL) {
   integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
   for (int i=0; i<ntdvdbs; i++) {                   // go through all TDV 
    dbH = dbTabH -> getDB(dbtdv,i);                  // dbase 
    if (dbH == NULL) Fatal("DeleteTDVContainer : Cannot open tdv dbase "); 
    DeleteContainer(dbH,contName,mode);
   }
   rstatus = oocSuccess;
  } else {
   Error("DeleteTDVContainer : dbTabH is NULL");
  }

end:
    if (contName) delete [] contName;
    if (rstatus == oocSuccess) {
     Commit(); 	           // Commit the transaction
    } else {
     Abort();  // or Abort it
     Fatal("DeeletTDVContainer : Quit.");
    }
    return rstatus;
}

ooStatus   LMS::DeleteSetup(char* setup)
{
  ooStatus 	             rstatus = oocError;	// Return status
  ooHandle(AMSdbs)           dbTabH;
  ooHandle(ooDBObj)          dbH;
  ooHandle(ooContObj)        contH;                

  char*                      contName;

  ooMode mode = oocUpdate;
  StartUpdate(); 
  dbTabH = Tabdb();
  if (dbTabH != NULL) {
   integer ndbs = dbTabH -> size(dbsetup);        // number of setup dbases
   for (int i=0; i<ndbs; i++) {                   // go through all TDV 
    dbH = dbTabH -> getDB(dbsetup,i);             // dbase 
    if (dbH == NULL) Fatal("DeleteSetup : Cannot open setup dbase "); 
// delete geometry container
     contName = StrCat("Geometry_",setup);
     DeleteContainer(dbH,contName,mode);
     delete [] contName;                    

// delete materials container
     contName = StrCat("Materials_",setup);
     DeleteContainer(dbH,contName,mode);
     delete [] contName;

// delete tmedia container
     contName = StrCat("TMedia_",setup);
     DeleteContainer(dbH,contName,mode);
     delete [] contName;

// delete amsdbc container
     contName = StrCat("amsdbc_",setup);
     DeleteContainer(dbH,contName,mode);
     delete [] contName;
   }
   rstatus = oocSuccess;
  } else {
   Error("DeleteSetup : dbTabH is NULL");
  }

end:
    if (rstatus == oocSuccess) {
     Commit(); 	                    // Commit the transaction
    } else {
     Abort();                       // or Abort it
     Fatal("DeleteSetup : Quit.");
    }
    return rstatus;
}

