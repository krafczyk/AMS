
// Author V. Choutko 24-may-1996
 
#include <iostream.h>
#include <signal.h>
#include <unistd.h> 
#include <new.h>
#include <upool.h>
#include <apool.h>
#include <status.h>
#include <commons.h>
#include <geantnamespace.h>
GCBANK_DEF GCBANK;
PAWC_DEF PAWC;
void (handler)(int);
 namespace glconst{
  integer cpul=1;
 }
 main(){
     using namespace gams;
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGXCPU,handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
     *signal(SIGUSR1, handler); 
     *signal(SIGUSR2, handler); 
    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
    UGINIT();
#ifdef __G4AMS__
try{
    if(MISCFFKEY.G4On)g4ams::G4RUN();
    else if(MISCFFKEY.G3On)GRUN();
} 
catch (std::bad_alloc a){
 cerr <<"catch-F-NoMemoryAvailable "<<endl;
}
#else
    GRUN();
#endif
    UGLAST();
    
return 0;
}
void (handler)(int sig){
using namespace glconst;
  switch(sig){
  case SIGFPE:
   cerr <<" FPE intercepted"<<endl;
   break;
  case SIGXCPU:
    if(cpul){
       cerr <<" Cpu limit exceeded"<<endl;
       cpul=0;
       GCFLAG.IEORUN=1;
       GCFLAG.IEOTRI=1;
       AMSStatus::setmode(0);
    }
    break;
  case SIGTERM: case SIGINT: 
    cerr <<" SIGTERM intercepted"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
    AMSStatus::setmode(0);
    break;
  case SIGQUIT:
    cerr <<" Process suspended"<<endl;
    pause();
    break;
  case SIGCONT:
      cerr <<" Process resumed"<<endl;
      break;
  case SIGUSR1:
      cerr<< "New Run Forced"<<endl;
      if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
      break;
  case SIGUSR2:
      cerr<< "New Ntuple Forced"<<endl;
      if(GCFLAG.ITEST>0)GCFLAG.ITEST=-GCFLAG.ITEST;
      break;
  }
}

