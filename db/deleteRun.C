// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
// Mar 11, 1997 ak.
//
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <A_LMS.h>



ooMode               _openMode = oocUpdate;

implement (ooVArray, geant)  //;
implement (ooVArray, number) //;
implement (ooVArray, integer) //;

LMS                  dbout(oocUpdate);

int main()
{
 integer                i, N = 0, Ncl = 0;
 integer                k, NN;

 char*                  listName = NULL;
 char*                  yesno    = NULL;
 char*                  runName  = NULL;
 char*                  contName = NULL;
 char                   predE[32], predC[32];
 ooVString              trclusterCont[2];  

 integer                runNumber;
 ooStatus               rstatus;
 ooHandle(ooDBObj)      _databaseH;
 ooHandle(ooContObj)    contClusterH[2];         // tracker cluster 
 ooHandle(AMSEventList) listH;        
 ooItr(AMSEventList)    listItr;
 ooHandle(AMSEventD)    eventH;
 ooHandle(AMSEventD)    *eventHT;
 ooItr(AMSEventD)       eventItr;
 ooHandle(AMSmceventD)     mceventH;
 ooHandle(AMSmceventD)    *mceventHT;
 ooItr(AMSmceventD)        mceventItr;
 ooItr(AMSTrClusterD)   trClusterItr;            // cluster iterator

 integer                eventType;

 listName = dbout.PromptForValue("Enter List Name: ");
 runName  = dbout.PromptForValue("Enter Run Number: ");
 
 istrstream stat(runName);
 stat >> runNumber;

 char* name;

  ooSession * _session  = ooSession::Instance("LOM");

  rstatus = dbout.Start(_openMode);
  if (rstatus == oocSuccess) {
  _databaseH = _session -> DefaultDatabase();
  if (_databaseH == NULL ) {
     cout <<" Error - pointer to default database is NULL"<<endl;
     goto end;
  }
   rstatus = dbout.FindEventList(listName, _openMode, listH);
   if (rstatus != oocSuccess) {
     cout <<" search for the list which has substring "<<listName<<endl;
     
     rstatus = listItr.scan(_databaseH, _openMode);
     if (rstatus == oocSuccess) {
      while (listItr.next()) {
       if (listItr -> CheckListSstring(listName)) {
        listItr -> PrintListStatistics(" ");
        yesno = dbout.PromptForValue("Do you want to delete this list ? ");
        if (yesno) { 
         if (strstr("No",yesno) || strstr("NO",yesno) || strstr("no", yesno)) {
         } else {
          listH = listItr;
          if (listName) delete [] listName;
          listName = new char[strlen(listItr ->ListName())];
          strcpy(listName,listItr -> ListName());
          goto deleterun;
         }
        }           
       }
     }
      goto end;
     } else {
       goto end;
     }
   } else {
     goto deleterun;
   }
  } else {
    cout <<"Error -  cannot start a transaction "<<endl;
    goto end;
  }
deleterun:

//TrCluster
   name =  new char[strlen(listName)+12];
   strcpy(name,"TrClusterX_");
   strcat(name,listName);
   trclusterCont[0] = name;
   delete [] name;

   name =  new char[strlen(listName)+12];
   strcpy(name,"TrClusterY_");
   strcat(name,listName);
   trclusterCont[1] = name;
   delete [] name;

   eventType = listH -> EventType();
   if (eventType < 100000) {
     NN = 2;
   } else {
     NN =1;
   }

   for (k=0; k<NN; k++) {
      if (eventType < 100000) {
       for (i=0; i<2; i++) {
        rstatus = listH -> CheckContainer
                    (trclusterCont[i].head(), oocUpdate, contClusterH[i], 1);
        if (rstatus != oocSuccess) {
         cout<< "cannot find or open trCluster containers "<<endl;
        }
       }
      }
       if (k == 0) {
       (void) sprintf(predE,"_runNumber=%d",runNumber);
        integer nevents = listH -> getNEvents();
        if (eventType < 100000) {
         eventHT = new ooHandle(AMSEventD) [nevents];
         eventItr.scan(listH, _openMode, oocAll, predE);
         while (eventItr.next()) {
         N++;
          eventItr -> pCluster(trClusterItr, _openMode);
          while (trClusterItr.next()) {
           Ncl++;
           trClusterItr -> setPosition(-777); 
          }
          if ( N > nevents) {
           cout<<"Fatal Error. Found more events than expected "<<endl;
           cout <<"N ... "<<N<<", nevents... "<<nevents<<endl;
           rstatus = oocError;
           goto end;
          }
          eventHT[N-1] = eventItr;
         }
       for (integer i=0; i<N; i++) {
        rstatus = ooDelete(eventHT[i]);
        if (rstatus != oocSuccess) {
           cout<<"Fatal Error. cannot delete event "<<endl;
           rstatus = oocError;
           goto end;
        }                   
        listH -> decNEvents();
       }      
        cout <<"deleted "<<N<<" events of run "<<runNumber<<" list "<<listName
            <<endl;
        delete [] eventHT;
        } else {
         mceventHT = new ooHandle(AMSmceventD) [nevents];
         mceventItr.scan(listH, _openMode, oocAll, predE);
         while (mceventItr.next()) {
         N++;
          if ( N > nevents) {
           cout<<"Fatal Error. Found more events than expected "<<endl;
           cout <<"N ... "<<N<<", nevents... "<<nevents<<endl;
           rstatus = oocError;
           goto end;
          }
          mceventHT[N-1] = mceventItr;
         }
       for (integer i=0; i<N; i++) {
        rstatus = ooDelete(mceventHT[i]);
        if (rstatus != oocSuccess) {
           cout<<"Fatal Error. cannot delete event "<<endl;
           rstatus = oocError;
           goto end;
        }                   
        listH -> decNEvents();
       }      
       cout <<"deleted "<<N<<" events of run "<<runNumber<<" list "<<listName
            <<endl;
       delete [] mceventHT;
        }
        goto end;
       }

       (void) sprintf(predC,"_Position=%d",-777);
       if (k == 1 && Ncl != 0) {
        integer j;
        cout<<"delete "<<Ncl<<" clusters now "<<endl;
        
        for (i=0; i<2; i++) {
         trClusterItr.scan(contClusterH[i],predC);
         while (trClusterItr.next()) {
          ooHandle(AMSTrClusterD) clusterH = trClusterItr;
           if (clusterH != NULL) {
            delete (AMSTrClusterD*) clusterH;
          }
         }
        }
       }      
   }
end:
 if (rstatus == oocSuccess) {
   rstatus = dbout.Commit();
   cout <<"Commit Done"<<endl;
  } else {
   rstatus = dbout.Abort();
   cout <<"Session aborted"<<endl;
  }
}
