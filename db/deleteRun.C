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
 integer                i, N = 0, Ncl = 0;
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
        eventItr.scan(listH, _openMode, oocAll, predE);
        while (eventItr.next()) {
        N++;
        rstatus = mapH -> lookup(eventItr -> GetID(), eventH, _openMode);
        if (rstatus == oocSuccess) {
         rstatus = mapH -> remove(eventItr -> GetID());
        } else {
          cout <<"cannot find event with ID "<<eventItr -> GetID()<<endl;
        }
        }
       cout <<N<<" objects are removed from the map "<<mapname<<endl;
       (void) sprintf(predE,"_run=%d",runNumber);
       (void) sprintf(predC,"_Position=%d",-777);
        for (i=0; i<N; i++) {
        eventItr.scan(listH, _openMode, oocAll, predE);
        eventH = eventItr;
        //cout <<"delete event with ID "<<eventH -> GetID()<<endl;
        eventH -> pCluster(trClusterItr, _openMode);
        while (trClusterItr.next()) {
         Ncl++;
         trClusterItr -> setPosition(-777); 
        }
         rstatus = ooDelete(eventH);
         listH -> decNEvents();
        }
       cout <<"deleted "<<i<<" events of run "<<runNumber<<" list "<<listName
            <<endl;
       }
      
       if (k == 1) {
        integer j;
        cout<<"delete "<<Ncl<<" clusters now "<<endl;
        for (i=0; i<2; i++) {
         trClusterItr.scan(contClusterH[i],predC);
         j = 0;
         while (trClusterItr.next()) {
           //cout <<j<<" clusters are deleted in container "
           //<<trclusterCont[i]<<endl;
          ooHandle(AMSTrClusterD) clusterH = trClusterItr;
           if (clusterH != NULL) {
             //cout <<clusterH -> getPosition()<<endl;
            //rstatus = ooDelete(clusterH);
            delete (AMSTrClusterD*) clusterH;
           j++;
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

 if (rstatus == oocSuccess) {
   rstatus = dbout.Commit();
   cout <<"LMS::Constructor Commit Done"<<endl;
  } else {
   rstatus = dbout.Abort();
  }
 
   }
}
