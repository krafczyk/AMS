
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
#include <producer.h>
const int NWPAW=1000000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=4000000;
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
void (handler)(int);
 namespace glconst{
  integer cpul=1;
 }
 main(int argc, char * argv[] ){
      using namespace gams;
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGXCPU,handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
     *signal(SIGUSR1, handler); 
     *signal(SIGUSR2, handler); 
     *signal(SIGHUP, handler); 
    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
try{
     
    UGINIT(argc,argv);
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)g4ams::G4RUN();
    else if(MISCFFKEY.G3On)GRUN();
#else
    GRUN();
#endif
} 
catch (amsglobalerror & a){
 cerr<<a.getmessage()<< endl;
    UGLAST(a.getmessage());
    return 1;
}
catch (std::bad_alloc aba){
 std::cerr <<"catch-F-NoMemoryAvailable "<<endl;
    UGLAST("catch-F-NoMemoryAvailable ");
    return 1;
}
#ifdef __CORBA__
catch (AMSClientError & ab){
 cerr<<ab.getMessage()<<" 1"<<endl;
 if(AMSProducer::gethead()){
  AMSProducer::gethead()->Error()=ab;
 }
}
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
  case SIGHUP:
      cerr <<" Artificial Loop"<<endl;
      for(;;){
      }
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

