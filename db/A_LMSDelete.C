// Last Edit : Mar 26, 1997. ak.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>

#include <typedefs.h>
#include <A_LMS.h>


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

ooStatus LMS::DeleteEvent(uinteger run, uinteger eventNumber, integer flag)
//
// run           - unique run number
// eventNumber   - event number
// flag          - event type to delete (see db_comm.h)
//
  {
    ooStatus rstatus = oocError;

    if (Mode() != oocUpdate) {
     Error("DeleteEvent: Cannot delete event in non-oocUpdate mode");
     return rstatus;
    }

    if (!isTagKeyValid(run,eventNumber)) return rstatus;

    //if (tagCont() == NULL) 
    if (_tagcontH == NULL) 
                     Fatal("DeleteEvent : pointer to tag container is NULL");

    // Start the transaction
    StartUpdate();     

    ooHandle(AMSEventTag)  tageventH;

    rstatus = FindTagEvent(run, eventNumber, tageventH);
    if (rstatus == oocSuccess) {
      if(BitSet(flag,TagEvent)) {
       rstatus = ooDelete(tageventH);
       goto end; 
      }
      if(BitSet(flag,RawEvent)) {
       ooHandle(AMSraweventD) eventH;
       ooItr(AMSraweventD)    raweventItr;             
       tageventH -> itsRawEvent(raweventItr);
       if (raweventItr != NULL) {
        rstatus = ooDelete(raweventItr);
       } else {
        cout <<"DeleteEvent -W- cannot find raw event ";tageventH -> print();
       }
      }

      if(BitSet(flag,RecoEvent)) {
       ooHandle(AMSeventD) eventH;
       ooItr(AMSeventD)    eventItr;             
       tageventH -> itsRecEvent(eventItr);
       if (eventItr != NULL) {
        rstatus = ooDelete(eventItr);
       } else {
        cout <<"DeleteEvent -W- cannot find reco event ";tageventH -> print();
       }
      }
    } else {
      cout<<"DeleteEvent -W- cannot find tag event. run, event "
          <<", "<<run<<eventNumber<<endl;
    }

end:
    Commit();

    return oocSuccess;

  }

