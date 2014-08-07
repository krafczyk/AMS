//  $Id$
// Original program by V.Choutko, the date of creation is unknown
//
// Last Edit 
//           May      1996  ak, add OBJY part
//                              session manager is modified
//                              V106
//           July     1996  ak. V109
//                              add event to the database if trig = 1
//                              Read/Write keys
//                              UGLAST definition is added
//                              print ooRunStatus();
//           Aug      1996  ak. V1.24
//                              ReadStartEnd  =  1 start read transaction
//                             (WriteStartEnd)= -1 commit read transaction
//                                            = -2 read/commit
//           Sep      1996 ak.  V1.25, VArray of numbers
//                              call AddList at the beginning of the run
//                              The argument list of AddList is modified
//                              Count number of transactions
//                              Print dbase statistics at the end
//           Oct  02, 1996 ak.  modification in Read and Write cards 
//                              put AMSFFKey.Read/Write interpretation in 
//                              uginit, eventR, eventW
//                              use CCFFKEY.run  
//                              write geometry to dbase before event processing
//           Feb  14, 1997 ak.  AMSmceventD
//           Mar  19, 1997 ak.  see db_comm.h about new eventR/eventW
//                              call dbend from uglast
//                              eventRtype 
//           Oct    , 1997 ak.  TDV dbase version implementation
//           Nov    , 1997 ak.  TKDBc dbase version implementation
//
//  Last Edit: Nov 20, 1997. ak
//
//2012-Feb-10 new G4TOF TofSimUtil Qi Yan //all photon

#include "typedefs.h"
#include <fenv.h>

#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#include <sys/resource.h>
#ifndef __DARWIN__
#include <sys/sysinfo.h>
#endif
#ifndef _PGTRACK_
#include "trid.h"
#else
#include "HistoMan.h"
#endif
#include <limits.h>
#include "extC.h"
//#include <trigger3.h>
#include "job.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "gvolume.h"
#include "gmat.h"
#include "mccluster.h"
#include "event.h"
#include "cont.h"
#include "trrec.h"
#include "daqevt.h"
#include "status.h"
#include "geantnamespace.h"
#include "timeid.h"

#ifdef __G4AMS__
#include "TRD_SimUtil.h"
#include "TofSimUtil.h"
#endif
#ifdef __AMSVMC__
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TVirtualMC.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoMaterial.h>
#include <TGeoMedium.h>
#include <TGeoBBox.h>
#include "amsvmc_MCApplication.h"
extern amsvmc_MCApplication*  appl;
#endif






#ifdef _USEVGM_
#include "Geant4GM/volumes/Factory.h" 
#include "RootGM/volumes/Factory.h" 
#include "TGeoManager.h"
#endif

#ifdef __DB__
#include <db_comm.h>
integer trigEvents;               // number of events written to the DBase
integer notrigEvents;             // ALL events - trigEvents
integer readEvents;               // events read from the DBase
static  integer eventR;           // = AMSFFKEY.Read
static  integer eventW;           // = AMSFFKEY.Write
static  integer ReadStartEnd = 1; // Start a transaction
static  integer eventNu;          // event number to read from DB
static  integer jobtype;          // AMSJob::jobtype()

  //class AMSEventList;
  //class AMSEventTagList;

#include <dbS.h>

// declaration ooVArray(...)
#include <commonsD_ref.h>     
#include <commonsD.h>

#include <dbcatalog_ref.h>
#include <dbcatalog.h>

#include <ooVArray_uint16.h>

#include <tkdbcV_ref.h>
#include <tkdbcV.h>


#include <gmatD_ref.h>     
#include <gmatD.h>


  //#include <list_ref.h>
  //#include <list.h>
  //
  //#include <listTag_ref.h>
  //#include <listTag.h>

// -
implement (ooVArray, uint16)
implement (ooVArray, geant)   
implement (ooVArray, integer) 
implement (ooVArray, ooRef(ooDBObj)) 
implement (ooVArray, TKDBcD) 
  //implement (ooVArray, ooRef(ooAMSEventList)) 
  //implement (ooVArray, ooRef(ooAMSEventTagList)) 

LMS	               dbout;
LMS*                   lms;

#endif


// Used for TGeometry
#include "ams2tgeo.h"


extern "C" void npq_();
extern "C" void timest_(float & t);
extern "C" void gstran_(int & itrt,float & ch, float &mas);
#include "producer.h"

void gams::UGINIT(int argc,  char * argv[]){
  float zero=0;
  timest_(zero);


#ifdef __AMSVMC__
  TFile* file1 = new TFile("ams02vmcgeometry.root","RECREATE");
  TGeoManager*  rootgeom = new TGeoManager("ams02vmcgeometry", "ams02vmcgeometry");
  amsvmc::VMCINIT(appl);
  //AMSJob::gethead()->data(); already done in geant3 library
#else
  cout.sync_with_stdio();   
  GINIT();
  new AMSJob();
  AMSJob::gethead()->data();
#endif
  GCTLIT.ITCKOV=1;
  GCPHYS.IRAYL=1;

#ifndef __AMSVMC__
 GFFGO();
#endif

#ifndef __DARWIN__
  if(MISCFFKEY.RaiseFPE==0){
      fedisableexcept(FE_ALL_EXCEPT );
  }
  else{
      cout <<"UGINIT-W-FPEExceptionsAreSet "<<fegetexcept()<<endl;
  }
#endif  
  AMSJob::gethead()->udata();
 
#ifdef __CORBA__
  AMSJob::gethead()->add( new AMSProducer(argc,argv,PRODFFKEY.Debug));
  AMSProducer::gethead()->sendid();
  AMSJob::gethead()->setjobtype(AMSJob::Production);  
#endif


  // Geant initialization
#ifndef __AMSVMC__
  GZINIT();
  GPART();
  GPIONS(4);
#endif
  int itrt=4;
  gstran_(itrt,CCFFKEY.StrCharge,CCFFKEY.StrMass);

#ifdef __DB__
   initDB();
   lms = &dbout;
   readSetup();
   if ((AMSFFKEY.Read%2) == 1) lms -> CheckConstants();
#else
#ifdef __G4AMS__
   trdSimUtil.UpdateGas();
   if(MISCFFKEY.G4On&&G4FFKEY.TFNewGeant4>0){//Qi Yan
       TofSimUtil::CreateTofG4();
   }
#endif
   AMSgmat::amsmat();
   AMSgtmed::amstmed();
   AMSgvolume::amsgeom();


#ifdef __AMSVMC__
      TGeoVolume *top = rootgeom->GetVolume("AMSG");
      //   TGeoBBox *box = new TGeoBBox("s_box", 10,10,10);
      //      TGeoVolume *top = new TGeoVolume("Dumy",box,gGeoManager->GetMedium(15));
   rootgeom->SetTopVolume(top); // mandatory !
   rootgeom->CloseGeometry();
   cout<<"Root Geometry Closed"<<endl;
   rootgeom->Write();
   cout<<"Root Geometry Saved"<<endl;
   file1->Write();
   cout<<"Root Geometry file Saved"<<endl;
   file1->Close();
   cout<<"Root Geometry file Closed"<<endl;
#endif

// GRFILE(1,"geomstr.dat","Q");
// GROUT("VOLU",1," ");
// GREND(1);
 
#endif
 //GRFILE(1,"geomstr.dat","N");
 //GROUT("VOLU",1," ");
 //GREND(1);

  AMSJob::map();

  AMSJob::gethead()->init();
#ifdef __CORBA__
    AMSTimeID * phead=AMSJob::gethead()->gettimestructure();
    AMSTimeID * down=(AMSTimeID *)phead->down();
    int nb=down->GetNbytes();
   AMSProducer::gethead()->getRunEventInfo();
#endif
  if(!AMSJob::gethead()->isProduction() && !AMSJob::gethead()->isMonitoring())AMSJob::gethead()->uhinit();
//  AMSJob::gethead()->urinit();
#ifndef __BATCH__
#ifdef __G4AMS__
if(MISCFFKEY.G3On)
#endif
GDINIT();
#endif


// Save geometry in TGeo format with ams2tgeo automatic converter
if (IOPA.WriteTGeometry==1) {
  // ROOT Geometry
  char tgeofilename[40];
  UHTOC(IOPA.TGeometryFileName,40/sizeof(integer),tgeofilename,40-1);
  cout << "gams::UGINIT-Create Root TGeoManager, output file is " << tgeofilename << endl;
#ifdef _PGTRACK_
  ams2tgeo::Build("TGeoAMS02","The AMS-02 Geometry",tgeofilename);
#endif
}


#ifdef __DB__
  if ((AMSFFKEY.Read%2) == 1) dbout.CheckCommons();
  writeSetup();
  //  int n = AMSJob::gethead()->FillTDVTable();
  //  ooStatus rstatus = dbout.FillTDV(n);
  //  if (rstatus != oocSuccess) cerr<<"uginit_ -W- FillTDV"<<endl;
  AMSJob::gethead() -> seteventRtype(eventR);
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G4On)g4ams::G4INIT();
#endif
#ifndef __AMSVMC__
GPHYSI();
#endif
AMSJob::map(1);

#ifdef _USEVGM_
if (IOPA.WriteTGeometry>1) {
    // ROOT Geometry
    char tgeofilename[40];
      UHTOC(IOPA.TGeometryFileName,40/sizeof(integer),tgeofilename,40-1);
        cout << "gams::UGINIT-Create Root TGeoManager, output file is " << tgeofilename << endl;
	TFile * f=TFile::Open(tgeofilename,"RECREATE");

	// Import Geant4 geometry to VGM
	//
	Geant4GM::Factory g4Factory;
	g4Factory.Import(AMSJob::gethead()->getgeom()->pg4v());
	// where physiWorld is of G4VPhysicalVolume* type

	// Export VGM geometry to Root
	//
	RootGM::Factory rtFactory;
	g4Factory.Export(&rtFactory);
	gGeoManager->CloseGeometry();
	gGeoManager->Write();
	f->Write();
	f->Close();

}
#endif
}


int gams::mem_not_enough(int Threshold)
{
#ifdef __DARWIN__
  // FIXME: Implement memory allocation protection for Mac OS X.
  return 0;
#else
   using std::ios_base;
   using std::ifstream;
   using std::string;
   using std::cout;
   using std::endl;

   unsigned long page_size = sysconf(_SC_PAGE_SIZE) / 1024;

   ifstream statm_stream("/proc/self/statm",ios_base::in);
   unsigned long vmsize=0;
   if(statm_stream){
    statm_stream >> vmsize;
   }
   statm_stream.close();
   unsigned long vm_usage = vmsize*page_size;

   struct rlimit ulimit_v;
   getrlimit(RLIMIT_AS, &ulimit_v);
   unsigned long ulimit_size = ulimit_v.rlim_cur / 1024;

   struct sysinfo info;
   sysinfo(&info);
   unsigned long free_mem = (info.freeram + info.freeswap)*info.mem_unit/1024;

   if ( long(ulimit_size - vm_usage) < Threshold ) {
     cerr << "gams::mem_not_enough-E-Not enough memory: ULIMIT=" << ulimit_size << ", CUR_USAGE=" << vm_usage << ". Trying to terminate gracefully..." << endl;
     return 1;
   }
   if ( free_mem < 102400 ) {
     cerr << "gams::mem_not_enough-E-Not enough memory: FREE PHY+SWAP=" << ulimit_size << ", CUR_USAGE=" << vm_usage << ". Trying to terminate gracefully..." << endl;
     return 1;
   }
   return 0;
#endif
}


void gams::UGLAST(const char *message){
#ifdef __CORBA__
try{
#endif
if(message)AMSJob::gethead()->setMessage(message);
#ifdef __G4AMS__
   trdSimUtil.EndOfRun();
if(MISCFFKEY.G4On)g4ams::G4LAST();
if(MISCFFKEY.G3On)
#endif
GLAST();


       delete AMSJob::gethead();
       
#ifdef __CORBA__
}
catch (AMSClientError & ab){
  cerr<<ab.getMessage()<< endl;
    return ;
}
#endif

}
//------------------------------------------------------------------------------------
extern "C" void readDB(){

#ifdef __DB_All__

ooHandle(AMSEventList) listH;
ooStatus               rstatus;

  integer nevents = GCFLAG.NEVENT;
  char* jobname   = AMSJob::gethead()->getname();
  char* setup     = AMSJob::gethead() -> getsetup();
  uinteger run    = SELECTFFKEY.Run;         // run number to read
  integer  eventF = SELECTFFKEY.Event;       // event number to start from
  uinteger eventn;
  time_t   time;

   if (eventF > 0 ) {
     eventn = eventF;
   } else {
    eventn       = 0;
   }
   if (run < 0)     run = 0;

    ReadStartEnd = 0; 
    integer nST = dbout.nTransStart();
    integer nCT = dbout.nTransCommit();
    integer nAT = dbout.nTransAbort();
    number  pole, stationT, stationP;
    integer rtype;
    if (readEvents == 0) AMSJob::gethead() -> seteventRtype(eventR);
    if ((readEvents == 0) || (nST == (nCT + nAT))) ReadStartEnd = 1;
    if (dbout.recoevents()) {
     for (;;) {
      rstatus = dbout.ReadEvents(run, eventn, nevents, time, ReadStartEnd);
      if (rstatus != oocSuccess) break;
      ReadStartEnd = 0;
     }
    }
    if (dbout.mcevents() || dbout.mceventg())
      rstatus =dbout.ReadMCEvents(run, eventn, nevents, time, ReadStartEnd);
   if (rstatus == oocSuccess) {
    readEvents++;
    if ((eventR < DBWriteMC) || dbout.mceventg()) {
       eventNu++;
       return; }
   }
   gams::UGLAST();
   exit(0);
#endif
}


extern "C" void writeSetup(){

#ifdef __DB__
     if ( (AMSFFKEY.Write%2) == 1) {
      integer eventW = AMSFFKEY.Write;
      char* jobname = AMSJob::gethead()->getname();
      char* setup = AMSJob::gethead() -> getsetup();
      cout <<"uglast -I- geometry and setup will be written to database"<<endl;
      cout <<"           for the job "<<jobname<<endl;
      cout <<"           with setup  "<<setup<<endl;
      dbout.AddMaterial();
      dbout.AddTMedia();
      dbout.AddGeometry();
      dbout.Addamsdbc();
      dbout.AddAllTDV();
     }
#endif
}

extern "C" void readSetup(){

#ifdef __DB__

  if ( (AMSFFKEY.Read%2) == 0) {
   AMSgmat::amsmat();
   AMSgtmed::amstmed();
   AMSgvolume::amsgeom();
  }  
  if ((AMSFFKEY.Read%2) == 1)   {
   dbout.ReadMaterial();
   GPMATE(0);
   dbout.ReadTMedia();
   GPTMED(0);
   //AMSgmat::amsmat();
   //AMSgvolume::amsgeom();
   dbout.ReadGeometry();
   ooStatus rstatus = dbout.ReadTKDBc();
   if (rstatus != oocSuccess) {
     cerr<<"readSetup -E- Error in ReadTKDBc"<<endl;
     exit(0);
   }
  }
#endif
}

extern "C" void initDB() 
{
#ifdef __DB__
   ooMode   mode;
   ooMode   mrowmode;
   eventR = AMSFFKEY.Read;
   eventW = AMSFFKEY.Write;
   if (eventW > 0) mode = oocUpdate;
   if (eventW < 1) {
     mode = oocRead;
     mrowmode = oocMROW;
   }
   char* jobname   = AMSJob::gethead()-> getname();
   char* setup     = AMSJob::gethead() -> getsetup();
   int   jobtype   = AMSJob::gethead() -> AMSJob::jobtype();
   const char* version   = AMSCommonsI::getversion();

   cout <<"_uginit -I- LMS init for job     "<<jobname<<endl;
   cout <<"                         setup   "<<setup<<endl;
   cout <<"                         jobtype "<<jobtype<<endl;
   cout <<"                         version "<<version<<endl;
   dbout.setapplicationName(jobname);
   dbout.setprefix(jobname);
   dbout.setsetup(setup);
   dbout.setversion(version);
   dbout.setjobtype(jobtype);
   dbout.settypeR(eventR);
   dbout.settypeW(eventW);
   dbout.ClusteringInit(mode,mrowmode);
#endif
}
