//  $Id: deleteList.C,v 1.6 2001/01/22 17:32:29 choutko Exp $
// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//
// Last Edit : Oct 31, 1996. use listname as a parameter
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <rd45.h>

#include <dbcatalog_ref.h>
#include <dbevent_ref.h>
#include <list_ref.h>

#include <dbcatalog.h>
#include <dbevent.h>
#include <list.h>

class LMS : public ooSession {
private:

public:
  LMS();
 ~LMS();
  void StartUpdate(const char *tag=NULL);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
  void Abort();
  void Commit();
  ooStatus Init(ooMode mode, ooMode mrowmode);
};

LMS::LMS(){}

LMS::~LMS() {}

ooStatus LMS::Init(ooMode mode, ooMode mrowmode)
{
    ooStatus rstatus = oocSuccess;

    // if oosession has not been initialised do it now
    if (!ooSession::initialized()) ooSession::Init();
    return rstatus;
}

void LMS::Abort()
{
    if (TransAbort() != oocSuccess)
      Fatal("could not abort transaction");
}

void LMS::StartUpdate(const char *tag)
{
    if ((TransStart(mrow(),tag) != oocSuccess) || (setUpdate() != oocSuccess))
    Fatal("could not start transaction in update mode");
}

void LMS::StartRead(ooMode mode, const char *tag)
{
    if ((TransStart(mode,tag) != oocSuccess) || (setRead() != oocSuccess))
        Fatal("could not start transaction in read mode");
}

void LMS::Commit()
{
    if (TransCommit() != oocSuccess)
      Fatal("could not commit transaction");
}

implement (ooVArray, ooRef(ooDBObj))

int main(int argc, char** argv)
{
 char*                  dbase = "MCEvents";
 char*                  list  = NULL;
 char*                  printMode = "S";

 const char *tag=NULL;
 ooMode      mode = oocUpdate;

 LMS                    lms;
 ooHandle(ooFDObj)      fdbH;
 ooHandle(ooDBObj)      dbH;
 ooHandle(ooDBObj)      catdbH;
 ooHandle(ooDBObj)      tagdbH;
 ooHandle(ooContObj)    contH;
 ooHandle(EventList)    listH;
 ooHandle(AMSdbs)       dbTabH;
 ooItr(AMSdbs)          dbTabItr;
 
 char*                  contName = NULL;
 char*                  tagName  = NULL;
 char*                  listName = NULL;
 char*                  yesno = NULL;

 int                    i, j;
 int                    ndbs;

 ooItr(EventList)       listItr;
 ooItr(dbEvent)         eventItr;

 ooStatus rstatus;



 cout<<" "<<endl;
 listName = PromptForValue("Enter List Name: ");
 if(!listName) Fatal ("listName is NULL");

 cout<<"DeleteList : delete will be done for list "<<listName<<endl;

 lms.Init(oocRead, oocMROW);
 lms.StartRead(oocMROW);
 fdbH = lms.fd();
 if (fdbH == NULL)  Fatal ("Pointer to federated dbase is NULL");

 //db's catalog
 catdbH = lms.db("DbList");
 if (!contH.exist(catdbH, "DbCatalog", oocRead))
            Fatal("DbCatalog does not exist. Check DbList database");
 dbTabItr.scan(contH, oocRead);
   if (dbTabItr.next()) 
     dbTabH = dbTabItr;
   else 
     Fatal("DbCatalog is empty. Check DbList database");

  integer ntagdbs = dbTabH -> size(dbtag);
  if (ntagdbs < 1) Fatal("There is no tag databases");

  tagName = StrCat("Events_",listName);
  for (i = 0; i<ntagdbs; i++) {
   tagdbH = dbTabH -> getDB(dbtag,i);
   if (!tagdbH) Fatal ("Pointer to selected dbase is NULL");
   if (contH.exist(tagdbH, tagName, oocRead)) goto deleteList;
  }
  cout<<"DeleteList : cannot find list "<<listName<<endl;
   yesno = PromptForValue("Do you want to search by substring ");
  if (yesno) { 
    if (strstr("No",yesno) || strstr("NO",yesno) || strstr("no", yesno)) {
     lms.Abort();
     Fatal("Quit.");
    }
  } else {
     lms.Abort();
     Fatal("Quit.");
  }

  if (yesno) delete [] yesno;
  cout<<"search for "<<listName<<" substring"<<endl;
  for (j = 0; j<ntagdbs; j++) {
   tagdbH = dbTabH -> getDB(dbtag,j);
    rstatus = listItr.scan(tagdbH, oocNoOpen);
   if (rstatus != oocSuccess) Fatal("Scan failed");
    while (listItr.next()) {
     cout<<" "<<endl;
     if (listItr -> CheckListSstring(listName)) listItr -> printListHeader();
    }
  }
   goto end;
//
deleteList:

   lms.Commit();
   lms.StartUpdate();
   
   if (contH.exist(tagdbH, tagName, oocUpdate)) ooDelete(contH);
   delete [] tagName;

mcdbase:
  ndbs = dbTabH -> size(dbsim);
  if (ndbs < 1) {
    Message("There are no MC databases");
    goto rawdbase;
  }
  for (i = 0; i<ndbs; i++) {
   dbH = dbTabH -> getDB(dbsim,i);
      contName =  StrCat("Events_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      if (contName) delete [] contName;

      contName =  StrCat("mceventg_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      if (contName) delete [] contName;

      contName =  StrCat("AntiMCCluster_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      if (contName) delete [] contName;

      contName =  StrCat("CTCMCCluster_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      delete [] contName;

      contName =  StrCat("TOFMCCluster_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      delete [] contName;

      contName =  StrCat("TrMCCluster_",listName);
      if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
      delete [] contName;

     contName =  StrDup(listName);
     if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
     delete [] contName;
  }

rawdbase:
  ndbs = dbTabH -> size(dbraw);
  if (ndbs < 1) {
    Message("There are no Raw databases");
    goto recodbase;
  }
  contName =  StrCat("Events_",listName);
  for (i = 0; i<ndbs; i++) {
   dbH = dbTabH -> getDB(dbraw,i);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
   }
   delete [] contName;
  
recodbase:
  ndbs = dbTabH -> size(dbrec);
  if (ndbs < 1) {
    Message("There are no Rec databases");
    goto end;
  }

  for (j = 0; j<ndbs; j++) {
   dbH = dbTabH -> getDB(dbrec,j);

   contName =  StrCat("Events_",listName);
   if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
   if (contName) delete [] contName;

    char       nameTrL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
    for (i=0; i<6; i++) {
     contName = StrCat(nameTrL[i],listName);
     if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
     delete [] contName;
    }
    contName =  StrCat("TrClusterX_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    contName =  StrCat("TrClusterY_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
    for (i=0; i<4; i++) {
     contName =  StrCat(nameSc[i],listName);
     if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
     delete [] contName;
    }

    contName = StrCat("CTCCluster_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    contName =  StrCat("TrTracks_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    contName =  StrCat("Beta_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    contName =  StrCat("Charge_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

    contName =  StrCat("Particle_",listName);
    if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
    delete [] contName;

     contName =  StrDup(listName);
     if (contH.exist(dbH, contName, oocUpdate)) ooDelete(contH);
     delete [] contName;

  }
     
  
end:
   lms.Commit();

}
