//           
// Sep 17, 1997 ak. print content of TDV
//
// Last Edit : Sep 24, 1997. 
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
   time_t  _insert;
   time_t  _begin;
   time_t  _end;
   integer _size;
  //
  tdv_time() {_insert = _begin = _end = 0;}
  tdv_time(time_t insert, time_t begin, time_t end) {
    _insert = insert;
    _begin  = begin;
    _end    = end;
  }
};

const int ntdv = 21;

static tdv_time*              tdv;
static integer                ptr_start[ntdv];
static integer                ptr_end[ntdv];

void FindTheBestTDV(char* name, time_t timeV, integer S,
                    time_t &I, time_t &B, time_t &E) 
{
  for (int i=0; i<ntdv; i++) {
    if(strcmp(name,tdvnames[i]) == 0) {
       integer  ptr = -1;
       time_t   insert = 0;
       for (int j=ptr_start[i]; j<ptr_end[i]; j++) {
       if (timeV >= tdv[j]._begin && timeV <= tdv[j]._end) {
        if (ptr_end[i] - ptr_start[i] - 1 != 0) {
         if (insert == 0) {
          insert = tdv[j]._insert;
          ptr    = j;         
         } else {
          if (insert < tdv[i]._insert) {
           insert = tdv[i]._insert;
           ptr    = j;
          }
         }
        } else {
         ptr = j;
        }
       }
      }
      if (ptr > -1) {
       I = tdv[ptr]._insert;
       B = tdv[ptr]._begin;
       E = tdv[ptr]._end;
       S = tdv[ptr]._size;
       cout <<"found TDV for "<<tdvnames[i]<<" with size "<<S<<endl;
       cout<<"i/b/e "<<asctime(localtime(&tdv[ptr]._insert))
           <<"      "<<asctime(localtime(&tdv[ptr]._begin))
           <<"      "<<asctime(localtime(&tdv[ptr]._end))<<endl;
      } else {
        cout <<"cannot find TDV for "<<name<<endl;
        cout <<"time "<<asctime(localtime(&timeV))<<endl;
      }
    break;
    }
  }
}

implement (ooVArray, ooRef(ooDBObj))

int main(int argc, char** argv)
{

 ooStatus rstatus = oocSuccess;
 ooMode   mode    = oocRead;

 char*                  dbase = "Setup";
 char*                  printMode = "S";
 char*                  cptr;
 char*                  contName = "Time_Var";

 time_t                 insert;
 time_t                 begin;
 time_t                 end;

 LMS                    lms;
 ooHandle(ooDBObj)      dbH;
 ooHandle(ooDBObj)      _catdbH;
 ooHandle(ooContObj)    contH;
 ooItr(ooContObj)       contItr;
 ooItr(AMSTimeIDD)      tdvItr;

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


 // get SetupDB pathname
 cptr = getenv("AMS_SetupDB_Path");
 if ( cptr ) 
   cout<<"ClusteringInit: SetupDB path name "<<cptr<<endl;
 else
  cout<<"ClusteringInit: SetupDB path name "<<AMSdbs::pathNameTab[dbsetup]
                                            <<endl;

 integer nsetupdbs = dbTabH -> size(dbsetup);

 integer nobj = 0;
 for (int j = 0; j < nsetupdbs; j++) {
  dbH = dbTabH -> getDB(dbsetup,j);
  if (!dbH) Fatal ("Pointer to selected dbase is NULL");
  if (contH.exist(dbH, contName, oocRead)) {
    cout <<"found container "<<contName<<endl;
    rstatus = tdvItr.scan(contH, oocRead);
    while (tdvItr.next()) nobj++;
  } else {
    cout <<"cannot find container "<<contName<<", try to find by substring"
         <<endl;
  rstatus = contItr.scan(dbH,oocNoOpen);
  if (rstatus != oocSuccess) Fatal("database scan failed");
   while (contItr.next()) {
    contH = contItr;
    if (strstr(contH.name(),contName) != NULL) {
     cout <<"found container (by sstring) "<<contName<<endl;
      rstatus = tdvItr.scan(contH, oocRead);
      while (tdvItr.next()) nobj++;
    }
   }
  }
 }
 cout <<"found "<<nobj<<" TDV objects"<<endl;
 tdv = new tdv_time[nobj];
 int jj = 0;
 for (j = 0; j < nsetupdbs; j++) {
  if (contH.exist(dbH, contName, oocRead)) {
    rstatus = tdvItr.scan(contH, oocRead);
  } else {
    rstatus = contItr.scan(dbH,oocNoOpen);
    if (rstatus != oocSuccess) Fatal("database scan failed");
    while (contItr.next()) {
     contH = contItr;
     if (strstr(contH.name(),contName) != NULL) {
      rstatus = tdvItr.scan(contH, oocRead);
      for (int i=0; i<ntdv; i++) {
       char name[80];
       char pred[100];
       strcpy(name,tdvnames[i]);
       (void) sprintf(pred,"_name=~%c%s%c",'"',name,'"');
       cout<<"TDV : search for "<<pred<<endl;
       rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
       if (rstatus != oocSuccess) Fatal("container scan failed");
       ptr_start[i] = jj;       
       while (tdvItr.next()) {
        //cout <<tdvItr -> getname()<<endl;
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
    }
   }
  }
 lms.Commit();
 for (int i=0; i< ntdv; i++) {
   cout<<tdvnames[i]<<", found "<<ptr_end[i]-ptr_start[i]<<" objects"<<endl;
   for (int j=ptr_start[i];j<ptr_end[i];j++) {
     cout<<"i/b/e "<<asctime(localtime(&tdv[j]._insert))
         <<"      "<<asctime(localtime(&tdv[j]._begin))
         <<"      "<<asctime(localtime(&tdv[j]._end))<<endl;
   }
 }
 integer size;
 for (i=0; i<ntdv; i++) {
  char name[80];
  strcpy(name,tdvnames[i]);
  FindTheBestTDV(name,900140000,size,insert,begin,end);
 }
}

