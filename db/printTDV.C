//           
// Sep 17, 1997 ak. print content of TDV
//
// Last Edit : Oct 14, 1997. 
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <rd45.h>

#include <dbcatalog_ref.h>
#include <timeidD_ref.h>

#include <dbcatalog.h>
#include <timeidD.h>


class LMS : public ooSession {
private:

public:
  LMS();
 ~LMS();

  void ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH);
  void Commit();
  ooStatus Init(ooMode mode, ooMode mrowmode);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
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

void LMS::ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH)
//
// create all containers for MC database
//
{	
  integer              rstatus = 1;
  ooHandle(ooContObj)  contH;
  ooItr(AMSdbs)        dbTabItr;
  ooMode               openMode =   oocRead;
  
  dbTabH = NULL;

  if (contH.exist(dbH, "DbCatalog", openMode)) {
    dbTabItr.scan(contH, openMode);
    if (dbTabItr.next()) {
      dbTabH = dbTabItr;
    } else {
      Fatal
       ("DbCatalog container is empty. Cannot create the object in Read mode");
    }
  } else {
   Fatal("DbCatalog container does not exist");
  }
}

static char    *tdvnames[] = {
   "MagneticFieldMap",
   "TrackerPedestals.l",
   "TrackerPedestals.r",
   "TrackerGains.l",
   "TrackerGains.r",
   "TrackerSigmas.l",
   "TrackerSigmas.r",
   "TrackerStatus.l",
   "TrackerStatus.r",
   "TrackerCmnNoise",
   "TrackerIndNoise",
   "Tofbarcal1",
   "Tofvpar",
   "Tofmcscans",
   "Antisccal",
   "ChargeLkhd1",
   "ChargeLkhd2",
   "ChargeLkhd3",
   "ChargeLkhd4",
   "ChargeLkhd5",
   "ChargeLkhd6"
};


class tdv_time {

  public :
   char*   _name;
   integer _id;
   time_t  _insert;
   time_t  _begin;
   time_t  _end;
   integer _size;
   ooRef(AMSTimeIDD) _ref;

// methods
  tdv_time() {_insert = _begin = _end = 0;}
  gettime(time_t &insert, time_t &begin, time_t &end) {
    insert = _insert;
    begin  = _begin;
    end    = _end;
  }
  settime(time_t insert, time_t begin, time_t end) {
    _insert = insert;
    _begin  = begin;
    _end    = end;
  }
  setH(ooHandle(AMSTimeIDD)  &H) {_ref = H;}
  setname(char* name) {if (name) {
                        _name = new char[strlen(name)+1];
                        strcpy (_name,name);
   }
  }
  char* getname() {return _name;}
//
};

const int ntdv = 21;

static tdv_time*              tdv;
static integer                ptr_start[ntdv];
static integer                ptr_end[ntdv];

implement (ooVArray, ooRef(ooDBObj))

int main(int argc, char** argv)
{

 ooStatus rstatus = oocSuccess;
 ooMode   mode    = oocRead;
 tdv_time* tdvt;

 char*                  dbase = "TDV";
 char*                  printMode = "S";
 char*                  cptr;
 char*                  contName = "Time_Dep_Var";

 time_t                 insert;
 time_t                 begin;
 time_t                 end;

 LMS                    lms;
 ooHandle(ooDBObj)      dbH;
 ooHandle(ooDBObj)      _catdbH;
 ooHandle(ooContObj)    contH;
 ooItr(ooContObj)       contItr;
 ooItr(AMSTimeIDD)      tdvItr;
 
 int                    i, j, k;

 cptr = getenv("OO_FD_BOOT");
 if (!cptr) Fatal("environment OO_FD_BOOT in undefined");

 cout<<" "<<endl;
 if(argc > 2)  contName  = argv[2];
 if(argc > 1)  printMode = argv[1];

 if (argc > 2) {
   cout<<"TDV container "<<contName<<", mode "<<printMode<<endl;
 } else {
   cout<<"all TDV containers, mode "<<printMode<<endl;
 }

 
 // if oosession has not been initialised do it now
 lms.Init(oocRead,oocMROW);
 lms.StartRead(oocMROW);

 ooHandle(AMSdbs)        dbTabH;

 _catdbH = lms.db("DbList");
 lms.ContainersC(_catdbH, dbTabH);

 if (dbTabH == NULL) Fatal("ClusteringInit: catalog of databases not found");


 // get TDVDB pathname
 cptr = getenv("AMS_TDVDB_Path");
 if ( cptr ) 
   cout<<"ClusteringInit: TDVDB path name "<<cptr<<endl;
 else
  cout<<"ClusteringInit: TDVDB path name "<<AMSdbs::pathNameTab[dbtdv]
                                            <<endl;

 integer ntdvdbs = dbTabH -> size(dbtdv);
 integer nobj = 0;
 integer nnobj  = 1; 
 integer nobj_0 = 100;
 tdvt = new tdv_time[nobj_0];

 for (j = 0; j < ntdvdbs; j++) {
  dbH = dbTabH -> getDB(dbtdv,j);
  if (!dbH) Fatal ("Pointer to selected dbase is NULL");
  if (contH.exist(dbH, contName, oocRead)) {
   cout <<"found container "<<contName<<endl;
  } else {
   cout <<"cannot find container "<<contName<<", try to find by substring"
         <<endl;
   rstatus = contItr.scan(dbH,oocNoOpen);
   if (rstatus != oocSuccess) Fatal("database scan failed");
   while (contItr.next()) {
    contH = contItr;
    if (strstr(contH.name(),contName) != NULL) {
     cout <<"found container (by sstring) "<<contName<<endl;
    }
   }
  }
      rstatus = tdvItr.scan(contH, oocRead);
      while (tdvItr.next()) {
       char* name = tdvItr -> getname();
       tdvt[nobj].setname(name);
       tdvItr -> GetTime(insert, begin, end);
       tdvt[nobj].settime(insert, begin, end);
       tdvt[nobj]._size   = tdvItr -> getsize();
       tdvt[nobj]._id     = tdvItr -> getid();
       ooHandle(AMSTimeIDD) H = tdvItr;
       tdvt[nobj].setH(H);
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
 cout <<"found "<<nobj<<" TDV objects in "<<ntdvdbs<<" files"<<endl;

 tdv = new tdv_time[nobj];
 int jj = 0;
 for (i=0; i<ntdv; i++) {
  ptr_start[i] = -1;
  ptr_end[i]   = -1;
  int jj_start = jj;
  for (k=0; k<nobj; k++) {
    char* name = tdvt[k].getname();
    if (strcmp(tdvnames[i],name) == 0) {
     tdv[jj] = tdvt[k];
     jj++;
     ptr_end[i] =jj;
    }
  }
  if (ptr_end[i] > 0) ptr_start[i] = jj_start;
 }
 delete [] tdvt; 

 for (i=0; i< ntdv; i++) {
   cout<<tdvnames[i]<<endl;
   for (int j=ptr_start[i]; j<ptr_end[i]; j++) {
     cout<<"id... "<<tdv[j]._id<<endl;
     ooRef(AMSTimeIDD) ref = tdv[j]._ref;
     if (ref) ref -> PrintTime();
   }
 }  
 lms.Commit();
}
