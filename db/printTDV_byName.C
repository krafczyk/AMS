//           
// Sep 17, 1997 ak. print content of TDV
//
// Last Edit : Feb 24, 1998. 
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



const int ntdv = 21;

implement (ooVArray, ooRef(ooDBObj))

int main(int argc, char** argv)
{

 ooStatus rstatus = oocSuccess;
 ooMode   mode    = oocRead;

 char*                  dbase = "TDV";
 char*                  printMode = "S";
 char*                  cptr;
 char*                  contName = "Time_Dep_Var";
 char*                  objname  = "MagneticFieldMap";

 time_t                 insert;
 time_t                 begin;
 time_t                 end;

 LMS                    lms;
 ooHandle(ooDBObj)      dbH;
 ooHandle(ooDBObj)      _catdbH;
 ooHandle(ooContObj)    contH;
 ooItr(ooContObj)       contItr;
 ooItr(AMSTimeIDD)      tdvItr;
 
 int                    i, j;

 cptr = getenv("OO_FD_BOOT");
 if (!cptr) Fatal("environment OO_FD_BOOT in undefined");

 cout<<" "<<endl;
 if(argc > 1)  objname   = argv[1];
 if(argc > 2)  contName  = argv[2];

 if (argc > 2) {
   cout<<"TDV container "<<contName<<endl;
 } else {
   cout<<"all TDV containers "<<endl;
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

  char name[80];
  char pred[100];
  strcpy(name,objname);
  (void) sprintf(pred,"_name=~%c%s%c",'"',objname,'"');
  cout<<"TDV : search for "<<pred<<endl;
  for (j = 0; j < ntdvdbs; j++) {
   dbH = dbTabH -> getDB(dbtdv,j);
   if (!contH.exist(dbH, contName, oocRead)) {
    rstatus = contItr.scan(dbH,oocNoOpen);
    if (rstatus != oocSuccess) Fatal("database scan failed");
    while (contItr.next()) {
     contH = contItr;
     if (strstr(contH.name(),contName) != NULL) break;
    }
   }
   rstatus = tdvItr.scan(contH, oocRead, oocAll, pred);
   if (rstatus != oocSuccess) Fatal("container scan failed");
    while (tdvItr.next()) {
     int size = tdvItr -> getsize();
     uinteger* buff = new uinteger[size];
     tdvItr -> copy(buff);
     cout<<"buff[size-1]... "<<buff[size-1]<<endl;
     delete [] buff;
     tdvItr -> GetTime(insert, begin, end);
     cout<<"id... "<<tdvItr -> getid()<<endl;
     cout<<"crc   "<<tdvItr -> getCRC()<<endl;
     
     cout<<"i/b/e "<<asctime(localtime(&insert))
         <<"      "<<asctime(localtime(&begin))
         <<"      "<<asctime(localtime(&end))<<endl;
    }
  }
 lms.Commit();
}

