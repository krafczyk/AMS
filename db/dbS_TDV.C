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
// Nov    , 1997. ak. 
//
// last edit Nov 19, 1997, ak.
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
#include <dbS.h>


#include <timeidD_ref.h>
#include <timeidD.h>

#include <job.h>
#include <timeid.h>

extern char *tdvNameTab[maxtdv];
extern int  tdvIdTab[maxtdv];
extern int  ntdvNames;

ooStatus   LMS::AddAllTDV()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooHandle(AMSdbs)           dbTabH;
        ooHandle(ooDBObj)          dbH;
        ooHandle(ooContObj)        contH;                
        ooHandle(AMSTimeIDD)       tdvH;
        ooItr(AMSTimeIDD)          tdvItr;         

        integer    id;
        char*      name;
        AMSNode*   p;
        AMSTimeID* pp;

        char*      contName;

  if (simulation()) 
   contName = StrDup("Time_Dep_Var_S");
  else
   contName = StrDup("Time_Dep_Var");

  StartRead(oocMROW); // Start the transaction

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
     if (p -> down() == NULL) Fatal("AddTDV -E- AMSTDV == NULL");
     tdvItr.scan(contH, Mode());
     integer nobj = 0;                    // number of TDV Objs
     if (tdvItr.next()) { nobj++; }
     if (nobj != 0) {                     // container isn't empty compare TDV 
      //Message("AddTDV : compare TDV "); // compare TDV in memory and dbase 
      time_t     insertd, begind, endd;
      time_t     insert, begin, end;
      p = p -> down();
      while (p != NULL ) {
       pp = (AMSTimeID*)p;
       if (pp -> UpdateMe()) {             // write TDV ONLY with UpdateMe == 1
       id = p -> getid();
       name = p -> getname();
       //pp -> gettime(insert, begin, end);
       uinteger crc  = pp -> getCRC();
       char pred[100];
       (void) sprintf(pred,"_id=%d && _name=~%c%s%c",id,'"',name,'"');
       cout<<"AddTDV : search for "<<pred<<endl;
       tdvItr.scan(contH, Mode(), oocAll, pred);
       while (tdvItr.next() ) {
        //tdvItr -> GetTime(insertd, begind, endd);
        //if (begin == begind && end == endd) {
         uinteger crcd = tdvItr -> getCRC();
         if (crc != crcd) {
          //tdvItr -> update(pp);
          Message("AddTDV : CRC is different, do update later");
         } else {
          if (insert != insertd ) Message
          ("AddTDV Insert time is different, but CRC is the same. Do nothing");
          pp -> UpdateMe() = 0;     // reset UpdateMe
          break;
         }
         //}
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
  StartUpdate(); // Start the transaction
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
        ooHandle(AMSdbs)       dbTabH;
        ooHandle(ooDBObj)      dbH;                     // tdv dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator
        int                    i, j;

  char* contName;
  if (simulation()) 
   contName = StrDup("Time_Dep_Var_S");
  else
   contName = StrDup("Time_Dep_Var");
  cout <<"FillTDV -I-  container "<<contName<<endl;
  
  StartRead(oocMROW);
  dbTabH = Tabdb();
  if (dbTabH == NULL) Fatal("FillTDV : dbTabH is NULL");
  integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
  short   nobj = 0;                                 // number of TDV Objs
  for (i=0; i<ntdvdbs; i++) {
    dbH = dbTabH -> getDB(dbtdv,i);                 // current container
    if (dbH == NULL) Fatal("FillTDV : Cannot open tdv dbase ");
    if (contH.exist(dbH, contName, oocRead)) {
     rstatus = tdvItr.scan(contH, oocRead);
     while (tdvItr.next()) {nobj++;}
    }
  }
  cout <<"FillTDV -I- found "<<nobj<<" TDV objects"<<endl;
  if (nobj < 1) Fatal("FillTDV : TDV containers is empty");
  tdv_time *tdv       = new tdv_time[nobj];
  integer  *ptr_start = new integer[ntdv];
  integer  *ptr_end   = new integer[ntdv];

  int jj       = 0;
  int jj_start = 0;
  for (i=0; i<ntdv; i++) {
   ptr_start[i] = -1;       
   ptr_end[i]   = -1;       
   jj_start     = jj;
   char pred[256];
   (void) 
   sprintf(pred,"_id=%d && _name=~%c%s%c",tdvIdTab[i],'"',tdvNameTab[i],'"');
   //(void) sprintf(pred,"_name=~%c%s%c",'"',tdvNameTab[i],'"');
   cout<<"FillTDV : search for "<<pred<<endl;
   for ( j=0; j<ntdvdbs; j++) {
     dbH = dbTabH -> getDB(dbtdv,j);
     if (dbH == NULL) Fatal("FillTDV : Cannot open tdv dbase ");
     if (contH.exist(dbH, contName, oocRead)) {
      rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
      if (rstatus != oocSuccess) Fatal("FillTDV : container scan failed");
      time_t  insert, begin, end;
      while (tdvItr.next()) {
       tdvItr -> GetTime(insert, begin, end);
       tdv[jj]._insert = insert;
       tdv[jj]._begin  = begin;
       tdv[jj]._end    = end;
       tdv[jj]._size   = tdvItr -> getsize();
       jj++;
       ptr_end[i] = jj;       
      }
     }
   }
   if (ptr_end[i] > 0) ptr_start[i] = jj_start;
  }

   AMSJob::FillJobTDV(nobj, tdv);
   AMSJob::SetTDVPtrs(ptr_start, ptr_end);

   rstatus = oocSuccess;

  Commit();

  if (contName) delete [] contName;

  return rstatus;

}

ooStatus   LMS::ReadTDV
             (char* tdvname, time_t I, time_t B, time_t E, uinteger* buff)
//
// find TDV (with name && I,B,E) in database and copy to buff
// 
//
{
	ooStatus 	       rstatus = oocError;	// Return status
        ooHandle(AMSdbs)       dbTabH;                  // catalog of dbases
        ooHandle(ooDBObj)      dbH;                     // tdv dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator

        integer                nobj = 0;                // number of TDVObj

  char* contName;
  if (simulation()) 
   contName = StrDup("Time_Dep_Var_S");
  else
   contName = StrDup("Time_Dep_Var");
  cout <<"ReadTDV -I-  container "<<contName<<endl;
  
  StartRead(oocMROW);
  dbTabH = Tabdb();
  if (dbTabH == NULL) Fatal("ReadTDV : dbTabH is NULL");
  integer ntdvdbs = dbTabH -> size(dbtdv);          // number of TDV dbases
  char pred[120];
  (void) sprintf(pred,"_Insert=%d && _Begin=%d && _End=%d &&_name=~%c%s%c",
                 I,B,E,'"',tdvname,'"');
  cout<<"ReadTDV -I- search for "<<pred<<endl;
  for (int i=0; i<ntdvdbs; i++) {
   dbH = dbTabH -> getDB(dbtdv,i);
   if (dbH == NULL) Fatal("ReadTDV : Cannot open tdv dbase ");
   if (contH.exist(dbH, contName, oocRead)) {
    rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
    if (rstatus != oocSuccess) Fatal("ReadTDV : container scan failed");
    time_t  insert, begin, end;
    while (tdvItr.next()) {
      cout<<"ReadTDV -I- found TDV in database for "<<tdvname<<endl;
      tdvItr -> PrintTime();
      tdvItr -> copy(buff);
      rstatus = oocSuccess;
      break;
    }
   }
   if (rstatus == oocSuccess) break;
  }
  if (rstatus != oocSuccess) 
     cout<<"ReadTDV -W- cannot find TDV in database for "<<tdvname<<endl;

  Commit();

  if (contName) delete [] contName;

  return rstatus;
}
