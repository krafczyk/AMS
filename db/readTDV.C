// Program to read TDV from file (or files) write them to the database
//
// A.Klimentov, February 14, 1998.
//
// Use original code from V.Choutko to read TDV
//
// Last Edit : Feb 15, 1998. ak
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <typedefs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <iomanip.h>
#include <strstream.h>


#include <ooSession.h>
#include <rd45.h>

#include <dbcatalog_ref.h>
#include <timeidD_ref.h>

#include <dbcatalog.h>
#include <timeidD.h>




 char       slash[]  = {"/"};
 char       dot[]    = {"."};

int UnixFileStat(const char *path, long *id, long *size,
                       long *flags, time_t *modtime)
{
   // Get info about a file: id, size, flags, modification time.
   // Id      is (statbuf.st_dev << 24) + statbuf.st_ino
   // Size    is the file size
   // Flags   is file type: bit 0 set executable, bit 1 set directory,
   //                       bit 2 set regular file
   // Modtime is modification time
   // The function returns 0 in case of success and 1 if the file could
   // not be stat'ed.
   //
   struct stat statbuf;

   if (path != 0 && stat(path, &statbuf) >= 0) {
      if (id)
         *id = (statbuf.st_dev << 24) + statbuf.st_ino;
      if (size)
         *size = statbuf.st_size;
      if (modtime)
         *modtime = statbuf.st_mtime;
      if (flags) {
         *flags = 0;
         if (statbuf.st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
            *flags |= 1;
         if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            *flags |= 2;
         if ((statbuf.st_mode & S_IFMT) != S_IFREG &&
             (statbuf.st_mode & S_IFMT) != S_IFDIR)
            *flags |= 4;
      }
      return 0;
   }
   return 1;
}

class LMS : public ooSession {
private:

public:
  LMS();
 ~LMS();

  integer LMS::Container(ooHandle(ooDBObj) & dbH, const char* name, 
                         ooHandle(ooContObj) & contH);
  void ContainersC(ooHandle(ooDBObj) & dbH, ooHandle(AMSdbs) & dbTabH);
  void Commit();
  ooStatus Init(ooMode mode, ooMode mrowmode);
  void StartRead(ooMode mrowmode, const char *tag=NULL);
  void StartUpdate(const char *tag=NULL);
  void Abort();
};

LMS::LMS(){}

LMS::~LMS() {}

void LMS::Abort()
{
    if (TransAbort() != oocSuccess) Fatal("could not abort transaction");
}



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

void LMS::StartUpdate(const char *tag)
{
    if ((TransStart(mrow(),tag) != oocSuccess) || (setUpdate() != oocSuccess))
    Fatal("could not start transaction in update mode");
}


void LMS::Commit()
{
    if (TransCommit() != oocSuccess)
      Fatal("could not commit transaction");
}

integer LMS::Container(ooHandle(ooDBObj) & dbH, const char* name, 
                       ooHandle(ooContObj) & contH)
//
// Open/Create container 
//
// dbH    - pointer to dbase
// name   - container name
// contH  - pointer to container
//
// return 
//      -1 - cannot open or create container
//       0 - container exists
//       1 - container is created
//
  {
    integer rstatus = -1;
    ooMode mode = Mode();

    if (!name) Fatal("Container name is NULL. Exit");
    if (!dbH)  Fatal("dbH is NULL. Exit");
    if (contH.exist(dbH, name, mode)) {
     rstatus = 0;
    } else {
     if( mode == oocUpdate) {
      contH    = new(name,0,0,0,dbH) ooContObj;
      rstatus = 1; // created
     } else {
      cerr<<"Container: "<<name<<endl;
      Warning("Container does not exist. Cannot be created in read mode");
      rstatus = -1;
     }
    }
  return rstatus;
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


implement (ooVArray, ooRef(ooDBObj))




int main(int argc, char* argv[])
{
//
// arg[1]  - file name 
//
  enum open_mode {binary=0x80};

  fstream    fbin;

  int        i, j, jj, l;
  char       filepath[250];             // file to read
  char       filename[250];
  char       version[20];
  char       objname[120];
  char       id[32];
  char*      status;
  char*      cptr;

  long    fid;                       //
  long    size;                      // Arguments for UnixFileStat
  long    flags;                     //
  time_t  modtime;                   //

  time_t   insert, Insert;
  time_t   begin, Begin;
  time_t   end, End;

  uinteger crc, CRC;
  uinteger *pdata;
  integer  Nbytes;

   if (argc < 2) {             
     cout <<"Error - no argument, expects readTDV file.name"<<endl;
     exit(1);
     //strcpy(filepath,"/Offline/AMSDataDir/v2.14/TrackerCmnNoise.1.993556");
   } 

   for (int nl=1; nl<argc -1; nl++) {
   strcpy(filepath,argv[nl]);
   status = strrchr(filepath,slash[0]);
   if(!status) {
    // set full path using AMSDataDir
    cptr = getenv("AMSDataDir");
    if (!cptr) {
      cerr<<"Error - cannot parse the input string "<<filepath<<endl;
      cerr<<"        full path isn't set and AMSDataDir isn't defined"<<endl;
      cerr<<"Quit"<<endl;
    } else {
     strcat(cptr,slash);
     strcat(cptr,filepath);
     strcpy(filepath,cptr);
    }
   }
   cout<<"Info - input file "<<filepath<<endl;
   // parse file name get id and AMSID name from it
   status = strrchr(filepath,slash[0]);
   if (status) {
    l  = strlen(status);
    strcpy(filename,status);
    for (i=0; i<l; i++) {filename[i] = filename[i+1];}
    filename[l] = '\0';
    l  = strlen(filepath);
    int lastslash = -1;
    int nextslash = -1;
    for (i=l; i>-1; i--) {
     if(filepath[i] == slash[0]) {
      if (lastslash < 0) 
       lastslash = i;
      else
       nextslash = i;
     }
     if(nextslash > 0) break;
    }
    if (nextslash < 0) nextslash = 0;
    jj = 0;
     for (j=nextslash+1; j<lastslash; j++) {
      version[jj] = filepath[j];
      jj++;
     }
   }
    else {
    strcpy(filename,filepath);
   }    
   status = strrchr(filename,dot[0]);
   if (!status) {
     cout <<"Error - cannot parse file name to get id"<<endl;
     exit(1);
   }
   
   l  = strlen(filename);
   int lastdot    = -1;
   int prelastdot = -1;
   for (i=l; i>0; i--) {
     if(filename[i] == dot[0]) {
       if(lastdot < 0)  
        lastdot = i;
       else 
        prelastdot = i;
     }
    if (prelastdot > 0) break;
   }
   for (i=0; i<prelastdot; i++) objname[i] = filename[i];
   objname[i++] = '\0';

   for (i=l; i>-1; i--) {
     if (filename[i] == dot[0]) {
      int ii = i-1;
      for (j=0; j<l; j++) {
       id[j] = filename[ii-j];
       if (filename[ii-j-1] == dot[0]) break;
      }
      break;
     }
   }
   id[j+1] = '\0';

   // parse done
   cout<<"Info - open file for TDV with name "<<objname<<", id "<<id<<endl;

   // open file and read TDV's now
   if(UnixFileStat(filepath, &fid, &size, &flags, &modtime) == 0) {
    Nbytes = size - 12;
    fbin.open(filepath,ios::in||binary);
    if (fbin) {
     integer ns = Nbytes/sizeof(pdata[0]);
     pdata =new uinteger[ns+3];
     if(pdata){
      fbin.read((char*)pdata,(ns+3)*sizeof(pdata[0]));
      if(fbin.good()){
       Insert = time_t(pdata[Nbytes/sizeof(pdata[0])]);
       Begin  = time_t(pdata[Nbytes/sizeof(pdata[0])+1]);
       End    = time_t(pdata[Nbytes/sizeof(pdata[0])+2]);
       CRC    = pdata[ns-1];
      }
       else {
        cerr<<"Error - Problems to Read File "<<filepath<<endl;
       }
      fbin.close();
     }
      else {
       cerr<<"Error - Failed to allocate memory "<<Nbytes<<endl;
     }
    } else {
      cerr<<"Error - CouldNot open file "<<filepath<<endl;
    }
   } else {
    cerr<<"Error - UnixFileStat cannot get information about "<<filepath<<endl;
   }

  if(!fbin.good()) exit(1);

  // database part starts here

  integer iid = atoi(id);
  cout<<"Info - write to the database "<<objname<<" with id... "<<iid
      <<", version "<<version<<endl;
  cout<<"i,b,e "<<ctime(&Insert)<<"      "<<ctime(&Begin)
                                <<"      "<<ctime(&End)<<endl;
 char*                  dbase = "TDV";
 char*                  contname  = "Time_Dep_Var_";
 char*                  contnameS = "Time_Dep_Var_S_";
 char                   contName[255];

 LMS                    lms;

 ooHandle(ooDBObj)       dbH;
 ooHandle(AMSdbs)        dbTabH;
 ooHandle(ooDBObj)      _catdbH;
 ooHandle(ooContObj)    contH;
 ooItr(ooContObj)       contItr;
 ooItr(AMSTimeIDD)      tdvItr;
 ooHandle(AMSTimeIDD)   tdvH;
 ooStatus               rstatus = oocSuccess;


 char *dptr = getenv("OO_FD_BOOT");
 if (!dptr) Fatal("environment OO_FD_BOOT in undefined");

 // if oosession has not been initialised do it now
 lms.Init(oocRead,oocMROW);

 // start Transaction in READ mode
 lms.StartRead(oocMROW);


 _catdbH = lms.db("DbList");
 lms.ContainersC(_catdbH, dbTabH);

 if (dbTabH == NULL) Fatal("ClusteringInit: catalog of databases not found");


 // get TDVDB pathname
 dptr = getenv("AMS_TDVDB_Path");
 if ( dptr ) 
   cout<<"ClusteringInit: TDVDB path name "<<dptr<<endl;
 else
  cout<<"ClusteringInit: TDVDB path name "<<AMSdbs::pathNameTab[dbtdv]
                                            <<endl;

 integer ntdvdbs = dbTabH -> size(dbtdv);
 integer found = 0;

 if (iid == 1) strcpy(contName,contname);
 if (iid == 2) strcpy(contName,contnameS);
 strcat(contName,version); 

  char pred[200];
  (void) sprintf(pred,"_id=%d && _name=~%c%s%c",iid,'"',objname,'"');
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
     tdvItr -> GetTime(insert, begin, end);
     if (begin == Begin && end == End) {
      Message("Found....... ");
      cout<<"id... "<<tdvItr -> getid()<<endl;
      cout<<"i/b/e "<<asctime(localtime(&insert))
          <<"      "<<asctime(localtime(&begin))
          <<"      "<<asctime(localtime(&end))<<endl;
     found = 1;
     crc = tdvItr -> getCRC();
      if (crc != CRC) {
       Warning("found the object with the same i/b/e,  but different CRC");  
       Warning("do nothing");
      } else {
       Message("CRC is the same do nothing");
      }
     }
    if (found == 1) break;
    }
    if (found == 1) break;
  }
  lms.Commit(); // commmit read transaction

  if(!found && pdata) {
   lms.StartUpdate();
   dbH = dbTabH -> currentDB(dbtdv);
   int status = lms.Container(dbH, contName, contH);
   if (status != -1) {
    tdvH = new(contH) AMSTimeIDD(objname, iid, Begin, Insert, End);
    tdvH -> CopyIn(Nbytes, pdata);
    lms.Commit();
   } else {
    lms.Abort();
    exit(1);
   }
  }
 if(pdata)  delete [] pdata;
 }
}
