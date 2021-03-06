//  $Id$
#include <iostream>
#include <signal.h>
#include <unistd.h> 
#include "upool.h"
#include "apool.h"
#include "status.h"
#include "commons.h"
#include "geantnamespace.h"
#include "producer.h"
#ifdef __DARWIN__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
#include <event.h>
#include <fenv.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _PGTRACK_
#include "TrRecon.h"
#endif

#ifdef __AMSVMC__
extern amsvmc_MCApplication*  appl = new amsvmc_MCApplication("AMSVMC", "AMS VirtualMC application");
#endif
#ifdef _G4AMS_
#include "ams_g4exception.h"
#endif
const int NWGEAN=15000000;
const int NWPAW=1300000;
struct PAWC_DEF{
float q[NWPAW];
};
struct GCBANK_DEF{
float q[NWGEAN];
};
//
#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
void my_unexpected () {
if (!std::uncaught_exception())
std::cerr << "my_unexpected called\n";
}
void (handler)(int);
 namespace glconst{
  integer cpul=1;
 }
int main(int argc, char * argv[] ){
std::set_unexpected (my_unexpected);
//for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--)){
//  pchar++;
//  switch (*pchar){
//   case 'v':    //version , return buildno
//   return AMSCommonsI::getbuildno();
//  } 
//}

      using namespace gams;
#if defined(__LINUX24__) || defined(__DARWIN__)
#else
     feenableexcept(FE_DIVBYZERO |  FE_INVALID | FE_OVERFLOW );
#endif
      //*signal(SIGABRT,handler);
      //   *signal(SIGFPE, handler);
     (void)*signal(SIGCONT, handler);
     (void)*signal(SIGTERM, handler);
     (void)*signal(SIGXCPU,handler);
     (void)*signal(SIGINT, handler);
     (void)*signal(SIGQUIT, handler);
     (void)*signal(SIGUSR1, handler);
     (void)*signal(SIGUSR2, handler);
     (void)*signal(SIGHUP, handler);
     (void)*signal(SIGTSTP, handler);
    (void)*signal(SIGURG, handler);
     (void)*signal(SIGTTIN, handler);
     (void)*signal(SIGTTOU, handler);
    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
try{
     
    UGINIT(argc,argv);
#ifndef __AMSVMC__
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)g4ams::G4RUN();
    else if(MISCFFKEY.G3On)GRUN();
#else
    GRUN();
#endif
#endif

#ifdef __AMSVMC__
    std::cout<<"DEBUG: in gbatch.C, Before VMCRUN()"<<std::endl;
    amsvmc::VMCRUN(appl);
#endif

} 
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< endl;
#ifdef __CORBA__
  AMSClientError ab((const char*)a.getmessage(),DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif
    UGLAST(a.getmessage());
    return 1;
}
catch (std::bad_alloc aba){


#ifdef __CORBA__
  AMSClientError ab("NoMemoryAvailable",DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif

  #ifdef __G4AMS__
   cerr<<"Preparing for OPool Released"<<endl;
   OPool.ReleaseLastResort();
   cerr<<"OPool Released a"<<endl;
  #endif

    cerr <<"gbatch-catch-F-NoMemoryAvailable "<<endl;
try{
    if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
    UGLAST("catch-F-NoMemoryAvailable ");
    exit(1);
    return 1;
}
#ifdef _G4AMS_
catch (ams_g4exception &){
#ifdef __CORBA__
  AMSClientError ab("G4ExceptionRaised",DPS::Client::CInAbort);
 if(AMSProducer::gethead()){
  cerr<<"setting errror"<< endl;
  AMSProducer::gethead()->Error()=ab;
 }
#endif
    cerr <<"gbatch-catch-F-G4ExceptionRaised "<<endl;
try{
    if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
    UGLAST("catch-F-G4ExceptionRaised");
    exit(1);
    return 1;
}
#endif
#ifdef __CORBA__
catch (AMSClientError & ab){
 cerr<<ab.getMessage()<<" 1"<<endl;
 if(AMSProducer::gethead()){
  AMSProducer::gethead()->Error()=ab;
 }
}
#endif
try{
#ifdef __AMSVMC__
  amsvmc::VMCLAST();
#endif

    cout <<"gbatch-I-NormalTerminationStart "<<endl;
    UGLAST();
}
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< " in UGLAST" <<endl;
 return 1;
}    
return 0;
}
void (handler)(int sig){
  using namespace glconst;
#ifdef _OPENMP
  int nthr=0;
#endif
  switch(sig){
  case SIGABRT:
    cerr <<" ABORT Detected "<<AMSCommonsI::AB_catch<<" "<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
#ifndef __DARWIN__
    mallopt(M_CHECK_ACTION,1);
#endif    
    if(AMSCommonsI::AB_catch>=0){
      AMSCommonsI::AB_catch=1;
      cout <<"  JUMP attempted "<<endl;
      siglongjmp(AMSCommonsI::AB_buf,0);
      
    }
    exit(1);
    break;
  case SIGFPE:
    cerr <<feclearexcept(FE_ALL_EXCEPT)<<" FPE intercepted "<<endl;
    break;
  case SIGXCPU:
#pragma omp master 
    if(cpul){
      cerr <<" Job Cpu limit exceeded"<<endl;
      cpul=0;
      GCFLAG.IEORUN=1;
      GCFLAG.IEOTRI=1;
      AMSStatus::setmode(0);

#ifdef __G4AMS__
	cerr<<"Preparing for OPool Released"<<endl;
	OPool.ReleaseLastResort();
	cerr<<"OPool Released b"<<endl;
	
#ifdef __CORBA__
	AMSClientError ab("Job Cpu limit exceeded",DPS::Client::CInAbort);
	if(AMSProducer::gethead()){
	  cerr<<"setting errror"<< endl;
	  AMSProducer::gethead()->Error()=ab;
	}
#endif
	cerr <<"gbatch-Job Cpu limit exceeded "<<endl;
try{
	if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
}
catch (std::bad_alloc abab){
}
	gams::UGLAST("SIGXCPU");
        exit(1);
#endif
    }
    break;
   case SIGINT:
    cerr <<" SIGINT intercepted sleeping 10 sec"<<endl;
  GCFLAG.IEORUN=1;
  GCFLAG.IEOTRI=1;
  AMSFFKEY.CpuLimit=10;
#ifdef G4MULTITHREADED
    G4AllocatorPool::gThreshold=0;
#endif 

    sleep(10);
    G4FFKEY.SigTerm=2;
   case SIGTERM: 
    cerr <<" SIGTERM intercepted"<<endl;
//#pragma omp master
{
  GCFLAG.IEORUN=1;
  GCFLAG.IEOTRI=1;
  AMSFFKEY.CpuLimit=10;
#ifdef G4MULTITHREADED
    G4AllocatorPool::gThreshold=0;
#endif  
#ifdef _PGTRACK_
  TrRecon::SigTERM=1;
#endif
if(G4FFKEY.SigTerm && (!AMSJob::gethead()->isProduction() || G4FFKEY.SigTerm>1)){
//#ifdef __G4AMS__
//#ifdef __CORBA__
lasthope:
//#endif
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released c"<<endl;
#ifdef __CORBA__
  AMSClientError ab("SIGTERM intercepted",DPS::Client::CInAbort);
  if(AMSProducer::gethead()){
    cerr<<"setting errror"<< endl;
    AMSProducer::gethead()->Error()=ab;
  }
#endif
  cerr <<"gbatch-SIGTERMSIMULATION "<<endl;
  //if(AMSEvent::gethead())AMSEvent::gethead()->Recovery(true);
  gams::UGLAST("SIGTERMSIM ",1);
  exit(1);
//#endif
}

 }
 break;
  case SIGQUIT:
cerr <<" SIGQUIT intercepted"<<endl;
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released d"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
    G4AllocatorPool::gThreshold=0;
#else
    cerr <<" Process suspended"<<endl;
  G4FFKEY.SigTerm=2;
    pause();
#endif
    break;
  case SIGCONT:
    cerr <<" Process resumed"<<endl;
    break;
  case SIGHUP:
#ifdef __CORBA__
    cout <<"got sighup "<<AMSEvent::get_thread_num()<<endl;
    if(AMSProducer::gethead() && AMSProducer::gethead()->Progressing()){
      cout <<" sending ... "<<endl;
      AMSProducer::gethead()->sendCurrentRunInfo(false);
    }
    else{
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released e"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
    G4AllocatorPool::gThreshold=0;

#else
          goto lasthope;
#endif
    } 
    cout << " sighup sended "<<endl;
#else
    cout << " sighup sended "<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
#ifdef G4MULTITHREADED
  cerr<<"Preparing for OPool Released"<<endl;
  OPool.ReleaseLastResort();
  cerr<<"OPool Released f"<<endl;
  G4AllocatorPool::gThreshold=0; 
#else
          goto lasthope;
#endif
#endif
    break;
  case SIGUSR1:
    cerr<< "New Run Forced"<<endl;
    if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
    break;
  case SIGUSR2:
    cerr<< "New Ntuple Forced"<<endl;
    if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
    break;
  case SIGTTOU:
#pragma omp master
    {
#ifdef _OPENMP
      nthr=omp_get_num_threads();
      if(nthr>1)nthr--;
      else MISCFFKEY.DynThreads=0;
      if(MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=nthr;
      else if(MISCFFKEY.NumThreads>1)MISCFFKEY.NumThreads--;
      cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
#endif
    }
    break;
  case SIGTTIN:
#pragma omp master
    {
#ifdef _OPENMP
    nthr=omp_get_num_threads();
    if(nthr<omp_get_num_procs())nthr++;
    else MISCFFKEY.DynThreads=1;
    if(MISCFFKEY.NumThreads<0)MISCFFKEY.NumThreads=nthr;
    else if(MISCFFKEY.NumThreads<omp_get_num_procs())MISCFFKEY.NumThreads++;
    cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
#endif
    }
    break;
 case SIGURG:
  case SIGTSTP:
#ifdef _OPENMP
#pragma omp master
    {
      MISCFFKEY.NumThreads=-1;
      cerr<<" ThreadsNumberWillBeChangedTo "<<MISCFFKEY.NumThreads<<endl;
    }
#endif
    break;
  }
}
