// Find methods for DbApplication class
//
// Author : A.Klimentov
// April 14, 1997.
//
// Last Edit : Apr. 14, 1997.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>

#include <ooIndex.h>

#include <dbA.h>


ooStatus LMS::FindTagEvent(uinteger runUni, uinteger eventNumber,
                                     ooHandle(AMSEventTag)& eventH)
{
 ooStatus rstatus = oocError;
 ooLookupKey          lookupKey(ooTypeN(AMSEventTag),2);
 ooItr(AMSEventTag)   eventItr;
 ooHandle(AMSEventTagList) listH;

 ooHandle(ooDBObj)  dbH = tagdb();
 if (dbH == NULL) 
             Fatal("FindTagEvent -F- pointer to tag event database is NULL");

  char                      *contName;
  integer                   ret;
  contName  = StrCat("Events_",_prefix); 
  ret = TagList(contName, _prefix, listH);
  if (listH != NULL) {
//ooEqualLookupField runLookupField(ooTypeN(AMSEventTag), "_runUni", &runUni);
   ooEqualLookupField runLookupField(ooTypeN(AMSEventTag), "_run", &runUni);
   lookupKey.addField(runLookupField); 
    ooEqualLookupField        
        eventLookupField(ooTypeN(AMSEventTag), "_eventNumber", &eventNumber);
   lookupKey.addField(eventLookupField); 

   ooMode               mode = Mode();

   ooStatus status = eventItr.scan(listH,lookupKey,mode);
   if (status == oocSuccess) {
    while (eventItr.next()) {
     eventH = eventItr;
     if (dbg_prtout) 
              cout << "DbApplication::FindEvent: found the event with run # "
                   << runUni<<" and event # "<<eventNumber<<endl;
     rstatus = oocSuccess;
     return rstatus;
    }
   } else {
    Fatal("FindTagEvent : scan init failed");
    rstatus = oocError;
   }
  } else {
    Error ("FindTagEvent: pointer to tag container is NULL");
   rstatus = oocError;
  }
 return rstatus;
}

integer LMS::FindRun(uinteger runUni)
{
 ooStatus rstatus = oocError;
 ooItr(AMSEventTag)   eventItr;
 integer              ret = 0;

 ooHandle(ooDBObj)  dbH = tagdb();
 if (dbH == NULL) 
             Fatal("FindTagEvent -F- pointer to tag event database is NULL");

 ooMode               mode = Mode();

 char                   pred[40];
 sprintf(pred,"_run=%d ",runUni);
 ooStatus status = eventItr.scan(dbH, mode, oocAll, pred);
 if (status == oocSuccess) {
  while (eventItr.next()) {
    ret = 1;
    return ret;
  }
 } else {
   Fatal("FindTagEvent : scan init failed");
 }
 return ret;
}

void LMS::Refresh()
//
// activate main lists after commit
//
{
 ooHandle(ooContObj) contH;

 if(tagCont() != NULL ) {
  contH.exist(tagdb(), tagCont() -> ListName(), Mode());
 } else {
  Warning("Refresh:: pointer to tag list is NULL");
 }

 if (mcevents()) {
   if (mcCont() != NULL ) {
      contH.exist(mcdb(), mcCont() -> ListName(), Mode());
   } else {
      Warning("Refresh:: pointer to MC list is NULL");
   }     
 }

 if(recoevents()) {
   if(recoCont() != NULL ) {
     contH.exist(recodb(), recoCont() -> ListName(), Mode());
   } else {
     Warning("Refresh:: pointer to Reco list is NULL");
   }
 }
}
