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
// Oct    , 1997. ak. 
//
// last edit Oct 10, 1997, ak.
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
extern int  ntdvNames;

ooStatus   LMS::AddAllTDV()
{
	ooStatus 	     rstatus = oocError;	// Return status
        ooHandle(ooDBObj)    dbH;

        StartUpdate(); // Start the transaction
        dbH = setupdb();
        if (dbH == NULL) Fatal("AddTDV : Cannot open setup dbase");
        if (_setupdbH != dbH) _setupdbH = dbH;

  // Containers
  ooHandle(ooContObj)  conttdvH;                // Time dependent var
  // ooObj classes
  ooHandle(AMSTimeIDD)       timedvH;
  // Iterators
  ooItr(AMSTimeIDD)          timedvItr;         // TDV

  integer    id;
  char*      name;
  AMSNode*   p;
  AMSTimeID* pp;

  // get setup and container names
  char* setup = AMSJob::gethead()->getsetup();
  if(dbg_prtout) cout <<"AddTDV -I- setup "<<setup<<endl;

  char* contName = new char[strlen(setup)+8];
  strcpy(contName,"Time_Var_");
  strcat(contName,setup);
  
  // check container
  int status = Container(dbH, contName, conttdvH);
  if (status == 0)
    {
      cout << "AddTDV -I- Found container "<<contName<< endl;
      p = AMSJob::gethead()->getnodep(AMSID("TDV:",0));
      if (p == NULL) {
       if (contName) delete [] contName;
       Fatal("AddTDV : cannot find the virtual top of timeid");
      }

      if (p -> down() == NULL) {
       if (contName) delete [] contName;
       Fatal("AddTDV -E- AMSTDV == NULL");
      }

        integer nobj = 0;
        timedvItr.scan(conttdvH, Mode());
        if (timedvItr.next()) { nobj++; }
        if (nobj == 0) {
         Message("AddTDV : container is empty. Write new TDV objects ");
         goto newsetup;
        }

      Message("AddTDV : compare TDV ");
      time_t     insertd, begind, endd;
      time_t     insert, begin, end;
      integer    found = 0;
      p = p -> down();
      while (p != NULL ) {
       pp = (AMSTimeID*)p;
       if (pp -> UpdateMe()) {
        id = p -> getid();
        name = p -> getname();
        pp -> gettime(insert, begin, end);
        char pred[100];
        (void) sprintf(pred,"_id=%d && _name=~%c%s%c",id,'"',name,'"');
        cout<<"AddTDV : search for "<<pred<<endl;
        timedvItr.scan(conttdvH, Mode(), oocAll, pred);
        while (timedvItr.next() ) {
         timedvItr -> GetTime(insertd, begind, endd);
         if (begin == begind && end == endd) {
          found = 1;
          uinteger crcd = timedvItr -> getCRC();
          uinteger crc  = pp -> getCRC();
          if (crc != crcd) {
           timedvItr -> update(pp);
           cout <<"AddTDV -I- CRC is different, update done"<<endl;
          } else {
           if (insert != insertd ) cout <<"AddTDV -I- "<<
           "Insert time is different, but CRC is the same. Do nothing"<<endl;
          }
         }
         if (found == 1) break;
        }
         if (found == 0) timedvH = new(conttdvH) AMSTimeIDD(name, id, pp);
         found = 0;
       }
        p = p -> next();
      }
      rstatus = oocSuccess;
      goto end;
    } else {
      if (status == -1) Fatal("AddTDV: Cannot open/create container");
    }

newsetup:
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
     timedvH = new(conttdvH) AMSTimeIDD(name, id, pp);
    }
     p = p -> next();
   }
   rstatus = oocSuccess;

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

ooStatus   LMS::ReadAllTDV()
{
	ooStatus 	       rstatus = oocError;	// Return status
        time_t                 timeV;
        ooHandle(ooDBObj)      dbH;

        StartRead(oocMROW);
        dbH = setupdb();
        if (dbH == NULL) Fatal("Cannot open setup dbase ");
        if (_setupdbH != dbH) _setupdbH = dbH;

// Containers
  ooHandle(ooContObj)  conttdvH;                // Time dependent var
// ooObj classes
  ooHandle(AMSTimeIDD)       timedvH;
// Iterators
  ooItr(AMSTimeIDD)          timedvItr;         // TDV


  // get setup and container names
  char* setup = AMSJob::gethead() -> getsetup();
  cout <<"ReadTDV -I-  setup "<<setup<<endl;

  char* contName = new char[strlen(setup)+8];
  strcpy(contName,"Time_Var_");
  strcat(contName,setup);
  
  // check container
  cout <<"ReadTDV -I- container : "<<contName<<endl;
  int status = Container(dbH, contName, conttdvH);
  if (status != 0) Fatal("ReadTDV : cannot find or open container ");
  if(contName) delete [] contName;

     integer nobj = 0;
     timedvItr.scan(conttdvH, Mode());
     if (timedvItr.next()) {nobj++;}
     if (nobj == 0) Fatal("ReadTDV container is empty");
      
  integer    id;
  char*      name;
  AMSNode*   p;
  AMSTimeID* pp;
  time_t     insertd, begind, endd;


  // get pointer to the top
  p = AMSJob::gethead()->getnodep(AMSID("TDV:",0));
  if (p == NULL) Fatal("CopyTDV : cannot find the virtual top of TDV");

   p = p -> down();
   while (p != NULL) {
     pp = (AMSTimeID*)p;
     id = p -> getid();
     name = p -> getname();
     char pred[100];
     (void) sprintf(pred,"_id=%d && _name=~%c%s%c",id,'"',name,'"');
     cout<<"ReadTDV -I- search for "<<pred<<endl;
     timedvItr.scan(conttdvH, Mode(), oocAll, pred);
     if (timedvItr.next()) {
      timedvItr -> GetTime(insertd, begind, endd);
      if (timeV >= begind && timeV <= endd) {
       cout <<"ReadTDV -I- found. copy from database "<<endl;
       integer size = timedvItr -> getsize();
       if (size > 0) {
        uinteger * tmp = new uinteger[size];
        timedvItr -> copy(tmp);
        pp -> CopyIn((void*)tmp);
        delete [] tmp;
       } 
      }
     }
     p = p -> next();
   }
 
  rstatus = oocSuccess;

end:
        if (rstatus == oocSuccess) {
	  Commit(); 	           // Commit the transaction
        } else {
         Abort();
         Fatal("ReadTDV: Quit.");
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
        ooHandle(ooDBObj)      dbH;                     // setup dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator

        integer                nobj = 0;                // number of TDVObj
  // get setup and container names
  // char* setup = AMSJob::gethead() -> getsetup();
  cout <<"ReadTDV -I-  setup "<<_setup<<endl;

  char* contName = new char[strlen(_setup)+8];

  strcpy(contName,"Time_Var_");
  strcat(contName,_setup);
  
  cout <<"FillTDV -I-  container "<<contName<<endl;
  
  StartRead(oocMROW);
  dbH = setupdb();
  if (dbH == NULL) Fatal("FillTDV : Cannot open setup dbase ");
  if (_setupdbH != dbH) _setupdbH = dbH;

  if (contH.exist(dbH, contName, oocRead)) {
   rstatus = tdvItr.scan(contH, oocRead);
   while (tdvItr.next()) nobj++;
   cout <<"FillTDV -I- found "<<nobj<<" TDV objects"<<endl;
   tdv_time *tdv = new tdv_time[nobj];
   integer  *ptr_start = new integer[ntdv];
   integer  *ptr_end   = new integer[ntdv];

   int jj = 0;
   for (int i=0; i<ntdv; i++) {
     //char name[80];
    char pred[256];
    //strcpy(name,tdvNameTab[i]);
    char* name = StrDup(tdvNameTab[i]);
    (void) sprintf(pred,"_name=~%c%s%c",'"',name,'"');
    if (name) delete [] name;
    cout<<"TDV : search for "<<pred<<endl;
    rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
    if (rstatus != oocSuccess) Fatal("FillTDV : container scan failed");
    ptr_start[i] = jj;       
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
   AMSJob::FillJobTDV(nobj, tdv);
   AMSJob::SetTDVPtrs(ptr_start, ptr_end);
   rstatus = oocSuccess;
  }
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
        ooHandle(ooDBObj)      dbH;                     // setup dbase
        ooHandle(ooContObj)    contH;                   // TDV container
        ooItr(AMSTimeIDD)      tdvItr;                  // TDVObj iterator

        integer                nobj = 0;                // number of TDVObj
  // get setup and container names
  // char* setup = AMSJob::gethead() -> getsetup();
  cout <<"ReadTDV -I-  setup "<<_setup<<endl;

  char* contName = new char[strlen(_setup)+8];
  strcpy(contName,"Time_Var_");
  strcat(contName,_setup);
  
  cout <<"ReadTDV -I-  container "<<contName<<endl;
  
  StartRead(oocMROW);
  dbH = setupdb();
  if (dbH == NULL) Fatal("ReadTDV : Cannot open setup dbase ");
  if (_setupdbH != dbH) _setupdbH = dbH;

  if (contH.exist(dbH, contName, oocRead)) {
    char name[80];
    char pred[120];
    strcpy(name,tdvname);
    (void) sprintf(pred,"_Insert=%d && _Begin=%d && _End=%d &&_name=~%c%s%c",
                   I,B,E,'"',name,'"');
    cout<<"ReadTDV -I- search for "<<pred<<endl;
    rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
    if (rstatus != oocSuccess) Fatal("ReadTDV : container scan failed");
    time_t  insert, begin, end;
    while (tdvItr.next()) {
     tdvItr -> GetTime(insert, begin, end);
     if (insert == I && begin == B && end == E) {
      cout<<"ReadTDV -I- found TDV in database for "<<tdvname<<endl;
      tdvItr -> copy(buff);
      rstatus = oocSuccess;
      break;
     }
    }
  }
  if (rstatus != oocSuccess) 
     cout<<"ReadTDV -W- cannot find TDV in database for "<<tdvname<<endl;

  Commit();

  if (contName) delete [] contName;

  return rstatus;

}
