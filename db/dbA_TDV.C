//  $Id: dbA_TDV.C,v 1.5 2001/01/22 17:32:28 choutko Exp $
//
// May  05, 1996. ak. Add methods, objectivity first try
// July 29, 1996. ak. Modify AddEvent, split Add and Fill functions
// Aug    , 1996. ak. All time is lost in commit, so ReadStartEnd flag is added
//                    to CopyMCEvent, CopyEvent 
//                    Add function GetNEvents;
//                    Add WriteStartEnd to AddEvent
// Sep    , 1996. ak. Add Geometry, Materials and TMedia.
//                    remove fRunNumber class member from list.ddl
//                    set map per list, not map per dbase as before
// Nov    , 1996. ak. exit, if Addamsdbc fails
// Dec  16, 1996. ak. oocMROW if mode == oocREAD
// Feb    , 1997. ak. mceventItr, AddTDV
//                    no map anymore, use indexes
// Mar  18, 1997. ak. Getmceventg and GetNEvents are modified
//                    setup moved to AMSsetupDB
// May  05, 1997. ak. separate file for setup
// Oct    , 1997. ak. tdv database. Time_Var container name is modified
//                    reset UpdateMe in AddAllTDV
// Nov    , 1997. ak. FillTDV is modified, use one iteration cycle to get 
//                    all tdv's.
//                    ReadTDV search by name,id,i,b,e
// Dec    , 1997. ak. Modify the logic in AddTDV
//
// last edit Dec 28, 1997, ak.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>

#include <sys/times.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>

#include <typedefs.h>
#include <db_comm.h>
#include <dbA.h>


#include <timeidD_ref.h>
#include <timeidD.h>

#include <job.h>
#include <timeid.h>

extern char *tdvNameTab[maxtdv];
extern int  tdvIdTab[maxtdv];
extern int  ntdvNames;

ooStatus   LMS::AddAllTDV()
//
//  Check update flag for all TDV objects in memory and write them 
//  to the database if flag == 1 and 
//  there is no object with the same crc and the the same begin/end time
//
{
	ooStatus 	           rstatus = oocError;	// Return status
        ooHandle(AMSdbs)           dbTabH;              // ptr to dbcatalog
        ooHandle(ooDBObj)          dbH;                 // tdv dbase
        ooHandle(ooContObj)        contH;                
        ooHandle(AMSTimeIDD)       tdvH;                // tdv object
        ooItr(AMSTimeIDD)          tdvItr;              // object iterator

        integer    id;
        char*      name;
        AMSNode*   p;
        AMSTimeID* pp;

        char*      contName;

  if (simulation()) 
   contName = StrCat("Time_Dep_Var_S",_version);
  else
   contName = StrCat("Time_Dep_Var",_version);

  StartUpdate(); // Start the transaction

  dbTabH = Tabdb();
  if (dbTabH == NULL) Fatal("AddAllTDV : dbTabH is NULL");
  integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
  for (int i=0; i<ntdvdbs; i++) {                   // go through all TDV 
    dbH = dbTabH -> getDB(dbtdv,i);                 // dbase and find whatever
    if (dbH == NULL) Fatal("AddAllTDV : Cannot open tdv dbase "); // possible
    int status = Container(dbH, contName, contH);
    if (status != -1) {
     cout << "AddTDV -I- Found container "<<contName<< endl;
     p = AMSJob::gethead()->getnodep(AMSID("TDV:",0));
     if (p == NULL) Fatal("AddTDV : cannot find the virtual top of timeid");
     if (p -> down() == NULL) Fatal("AddTDV : AMSTDV == NULL");
     tdvItr.scan(contH, Mode());
     integer nobj = 0;                    // number of TDV Objs
     if (tdvItr.next()) { nobj++; }
     if (nobj != 0) {                     // container isn't empty compare TDV 
      time_t     insertd, begind, endd;
      time_t     insert, begin, end;
      p = p -> down();
      while (p != NULL ) {
       pp = (AMSTimeID*)p;
       if (pp -> UpdateMe()) {             // write TDV ONLY with UpdateMe == 1
       id = p -> getid();
       name = p -> getname();
       uinteger crc  = pp -> getCRC();
       pp -> gettime(insert, begin, end);
       char pred[100];
       (void) sprintf(pred,"_id=%d && _name=~%c%s%c",id,'"',name,'"');
       cout<<"AddTDV : search for "<<pred<<endl;
       integer object = 0;
       tdvItr.scan(contH, Mode(), oocAll, pred);
       while (tdvItr.next() ) {
         object++;
         uinteger crcd = tdvItr -> getCRC();
         tdvItr -> GetTime(insertd, begind, endd);
          if (crc != crcd) {
          if (begind == begin && endd == end) {
           cout<<"AddTDV -I- object "<<object<<endl;
           Message("AddTDV : CRC is different, but begin/end are the same");
           Message("AddTDV : delete the old object and add the new one");
           tdvH = tdvItr;
           ooDelete(tdvH);
          } else {
           cout<<"AddTDV -I- object "<<object<<endl;
           Message("AddTDV : CRC, begin/end are different, add object later");
          }
         } else {
          if (begind == begin && endd == end) {
           cout<<"AddTDV -I- object "<<object<<endl;
           Message
           ("AddTDV: Begin/End time and CRC are the same. Do nothing");
           pp -> UpdateMe() = 0;     // reset UpdateMe
           break;
          } else {
           cout<<"AddTDV -I- object "<<object<<endl;
           Message
           ("AddTDV: CRC is the same, but Begin/End are different, add object");
          }
         }
       } // iterate over all obj with pred for the container and reset 
         // UpdateMe if CRC and CRC of database object are the same
      }  // if UpdateMe for the Obj is 1
       p = p -> next();
      }  // do it for all objects in memory
     }   // if container not empty
    }    // if container exist
   rstatus = oocSuccess;
  }      // for all TDV databases.
  Commit();

  // assume there are some object that haven't been found. They will be stored
  // in the last TDV database
  StartUpdate();                                      // Start the transaction
  dbH = dbTabH -> currentDB(dbtdv);
  int status = Container(dbH, contName, contH);
  if (status != -1) {
   // get pointer to the top
   p = AMSJob::gethead()->getnodep(AMSID("TDV:",0));
   if (p == NULL)   Fatal("AddTDV cannot find the virtual top of TDV");
   p = p -> down();
   while (p != NULL) {
    pp = (AMSTimeID*)p;
    if( pp -> UpdateMe()) {
     cout<<p -> getid() <<" "<<p -> getname() <<endl;
     id = p -> getid();
     name = p -> getname();
     tdvH = new(contH) AMSTimeIDD(name, id, pp);
     pp -> UpdateMe() = 0;
    }
     p = p -> next();
   }
   rstatus = oocSuccess;
  } 
end:
    if (contName) delete [] contName;
    if (rstatus == oocSuccess) {
     Commit(); 	           // Commit the transaction
    } else {
     Abort();  // or Abort it
     Fatal("AddTDV : Quit.");
    }
    return rstatus;
}

ooStatus   LMS::FillTDV(integer ntdv)
//
// Fill AMSJob::tdv table from database
// program reads (i,b,e) times from database for all TDV and writes them
// to AMSJob::tdv
//
{
	ooStatus 	       rstatus = oocError;	// Return status
        ooHandle(AMSdbs)       dbTabH;                  // handle to dbcatalog
        ooHandle(ooDBObj)      dbH;                     // tdv dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator
        int                    i, j, k;

  char* contName;
  if (simulation()) 
   contName = StrCat("Time_Dep_Var_S",_version);
  else
   contName = StrCat("Time_Dep_Var",_version);
  cout <<"FillTDV -I-  container "<<contName<<endl;
  
  StartRead(oocMROW);
  dbTabH = Tabdb();
  if (dbTabH == NULL) Fatal("FillTDV : dbTabH is NULL");
  integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
  integer nobj   = 0;                               // number of tdv objects
  integer nnobj  = 1;                               // number of hundreds
  integer nobj_0 = 100;                             // initital size of array
  time_t  insert, begin, end;
  tdv_time*   tdvt;
  tdv_time*   tdv;
  tdvt = new tdv_time[nobj_0];

  for (i=0; i<ntdvdbs; i++) {
    dbH = dbTabH -> getDB(dbtdv,i);                 // current container
    if (dbH == NULL) Fatal("FillTDV : Cannot open tdv dbase ");
    if (contH.exist(dbH, contName, oocRead)) {
     rstatus = tdvItr.scan(contH, oocRead);
     while (tdvItr.next()) {
      char* name = tdvItr -> getname();
      tdvt[nobj].setname(name);
      tdvItr -> GetTime(insert, begin, end);
      tdvt[nobj].settime(insert, begin, end);
      tdvt[nobj]._size   = tdvItr -> getsize();
      tdvt[nobj]._id     = tdvItr -> getid();
      nobj++;
      if (nobj == nobj_0*nnobj) {
       tdv = new tdv_time[nobj_0*nnobj];
       for (k=0; k<nobj_0*nnobj; k++) {tdv[k] = tdvt[k];}
       delete [] tdvt;
       nnobj++;
       tdvt = new tdv_time[nobj_0*nnobj];
       for (k=0; k<nobj_0*(nnobj-1); k++) {tdvt[k] = tdv[k];}
       delete [] tdv;
      }
     }
    }
   }
    cout <<"FillTDV -I- found "<<nobj<<" TDV objects"<<endl;
    if (nobj < 1) Fatal("FillTDV : TDV containers are empty");

    tdv                 = new tdv_time[nobj];
    integer  *ptr_start = new integer[ntdv];
    integer  *ptr_end   = new integer[ntdv];
    integer   jj        = 0;
    for (i=0; i<ntdv; i++) {
     ptr_start[i] = -1;
     ptr_end[i]   = -1;
     int jj_start = jj;
     for (k=0; k<nobj; k++) {
      char* name = tdvt[k].getname();
      if (strcmp(tdvNameTab[i],name) == 0) {
       if (tdvIdTab[i] == tdvt[k].getid()) {
        tdv[jj] = tdvt[k];
        jj++;
        ptr_end[i] =jj;
       }
      }
     }
     if (ptr_end[i] > 0) ptr_start[i] = jj_start;
    }
   delete [] tdvt; 

   AMSJob::FillJobTDV(nobj, tdv);
   AMSJob::SetTDVPtrs(ptr_start, ptr_end);

   delete [] tdv;
   if (ptr_start) delete [] ptr_start;
   if (ptr_end)   delete [] ptr_end;

   rstatus = oocSuccess;

  Commit();

  if (contName) delete [] contName;

  return rstatus;

}

ooStatus   LMS::ReadTDV(char* tdvname, integer id, 
                        time_t I, time_t B, time_t E, uinteger* buff)
//
// find TDV (with name && id && I,B,E) in database and copy to buff
// 
//
{
	ooStatus 	       rstatus = oocError;	// Return status
        ooHandle(AMSdbs)       dbTabH;                  // catalog of dbases
        ooHandle(ooDBObj)      dbH;                     // tdv dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator
        char*                  contName;
        integer                found = 0;

const char *tag;

  if (simulation()) 
   contName = StrCat("Time_Dep_Var_S",_version);
  else
   contName = StrCat("Time_Dep_Var",_version);
  cout <<"ReadTDV -I-  container "<<contName<<endl;

// if ((TransStart(oocMROW,tag) != oocSuccess))
// || (setRead() != oocSuccess))
// if ( in_trans ( ) ) { cout << "Event.C: transaction is Active" << endl; }
// if ( !in_trans ( ) ) 
//  if ( !ntranscount() )
cout << "Transaction Started= " <<  nTransStart() << endl; 
cout << "Transaction Committed= " <<  nTransCommit() << endl;
cout << "Transaction Aborted= " <<  nTransAbort() << endl;
integer ndiff =  nTransStart() - nTransCommit();


    if ( ndiff == 0 )
{
 StartRead(oocMROW);
}
// StartRead(oocRead);
  dbTabH = Tabdb();
  if (dbTabH == NULL) Fatal("ReadTDV : dbTabH is NULL");
  integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
  char pred[120];
  (void) sprintf
       (pred,"_Insert=%d && _Begin=%d && _End=%d && _id=%d &&_name=~%c%s%c",
              I,B,E,id,'"',tdvname,'"');
#ifdef __AMSDEBUG__
  cout<<"ReadTDV -I- search for "<<pred<<endl;
#endif
  for (int i=0; i<ntdvdbs; i++) {
   dbH = dbTabH -> getDB(dbtdv,i);
   if (dbH == NULL) Fatal("ReadTDV : Cannot open tdv dbase ");
   if (contH.exist(dbH, contName, oocRead)) {
    rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
    if (rstatus != oocSuccess) Fatal("ReadTDV : container scan failed");
    while (tdvItr.next()) {
#ifdef __AMSDEBUG__
      cout<<"ReadTDV -I- found TDV in database for "<<tdvname<<endl;
      tdvItr -> PrintTime();
#endif
      tdvItr -> copy(buff);
      found = 1;
      break;
    }
   }
   if (found == 1) break;
  }
  if(found != 1) Warning("ReadTDV : cannot find TDV in database");

//   if ( !ntranscount() ) Commit();
   if ( ndiff > 1 ) Commit();

  if (contName) delete [] contName;

  return rstatus;
}
