// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
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
 integer                i, N = 0, Ncl = 0, Ne = 0;
 char*                  listName = NULL;
 char*                  runName  = NULL;
 char*                  mapName  = NULL;
 char*                  contName = NULL;
 char                   predE[32], predC[32];
 ooVString              trclusterCont[2];  

 integer                runNumber;
 ooStatus               rstatus;
 ooHandle(ooDBObj)      _databaseH;
 ooHandle(ooContObj)    contClusterH[2];         // tracker cluster 
 ooHandle(AMSEventList) listH;        
 ooHandle(ooMap)        mapH;
 ooHandle(AMSEventD)    eventH;
 ooHandle(AMSEventD)    *eventHT;
 ooItr(AMSEventD)       eventItr;
 ooItr(AMSTrClusterD)   trClusterItr;            // cluster iterator


 listName = dbout.PromptForValue("Enter List Name: ");
 runName  = dbout.PromptForValue("Enter Run Number: ");
 
 istrstream stat(runName);
 stat >> runNumber;

 char* mapname =  new char[strlen(listName)+6];
 strcpy(mapname,"Maps_");
 strcat(mapname,listName);

 char* name;

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


   for (int k=0; k<2; k++) {
    rstatus = dbout.Start(_openMode);
    if (rstatus == oocSuccess) {
     rstatus = dbout.FindEventList(listName, _openMode, listH);
     if (rstatus == oocSuccess) {
      for (i=0; i<2; i++) {
       rstatus = listH -> CheckContainer
                    (trclusterCont[i].head(), oocUpdate, contClusterH[i], 1);
      }
      if (rstatus == oocSuccess) {
      listH -> FindMap(mapname, _openMode, mapH);
      if (mapH != NULL) {
       if (k == 0) {
       (void) sprintf(predE,"_run=%d",runNumber);
        integer nevents = listH -> getNEvents();
        eventHT = new ooHandle(AMSEventD) [nevents];
        eventItr.scan(listH, _openMode, oocAll, predE);
        while (eventItr.next()) {
         N++;
         rstatus = mapH -> lookup(eventItr -> GetID(), eventH, _openMode);
         if (rstatus == oocSuccess) {
          eventH -> pCluster(trClusterItr, _openMode);
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
          eventHT[N-1] = eventH;
        } else {
          cout <<"cannot find event with ID "<<eventItr -> GetID()<<endl;
        }
       } 
       cout <<N<<" objects will be removed from the map "<<mapname<<endl;
       for (integer i=0; i<N; i++) {
        rstatus = mapH -> remove(eventHT[i] -> GetID());
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
      } else {
       cout<<"cannot find or open map "<<mapname<<endl;
      }
     } else {
       cout<< "cannot find or open trCluster containers "<<endl;
     }
     } else {
       cout<<"cannot find or open list "<<listName<<endl;
     }
 } else {
   cout <<"cannot start a transaction "<<endl;
 }

end:
 if (rstatus == oocSuccess) {
   rstatus = dbout.Commit();
   cout <<"LMS::Constructor Commit Done"<<endl;
  } else {
   rstatus = dbout.Abort();
  }
 
   }
}
