
// Author V. Choutko 24-may-1996
 
#include <iostream.h>
#include <gvolume.h>
#include <signal.h>
#include <unistd.h> 
#include <new.h>
#include <upool.h>
#include <apool.h>



const int NWPAW=350000;
struct PAWC_DEF{
float q[NWPAW];
};
const int NWGEAN=1200000;
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




PROTOCCALLSFSUB0(UGINIT,uginit)
#define UGINIT() CCALLSFSUB0(UGINIT,uginit)
PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)
//
void (handler)(int);
main(){
     //set_new_handler(UPool.StHandler);
    *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
     *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
    AMSgvolume::debug=0;
    cout.sync_with_stdio();
    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
    UGINIT();
    GRUN();
    UGLAST();
    
return 0;
}
void (handler)(int sig){
  if(sig==SIGFPE)cerr <<" FPE intercepted"<<endl;
  else if (sig==SIGTERM || sig==SIGINT){
    cerr <<" SIGTERM intercepted"<<endl;
    GCFLAG.IEORUN=1;
    GCFLAG.IEOTRI=1;
  }
  else if(sig==SIGQUIT){
      cerr <<" Process suspended"<<endl;
     pause();
  }
  else if(sig==SIGCONT){
      cerr <<" Process resumed"<<endl;
  }
}

