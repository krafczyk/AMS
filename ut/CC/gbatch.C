// Author V. Choutko 24-may-1996
 
#include <iostream.h>
#include <gvolume.h>
#include <commons.h>
#include <signal.h>
GCBANK_DEF GCBANK;
PAWC_DEF PAWC;

PROTOCCALLSFSUB0(UGINIT,uginit)
#define UGINIT() CCALLSFSUB0(UGINIT,uginit)
PROTOCCALLSFSUB0(UGLAST,uglast)
#define UGLAST() CCALLSFSUB0(UGLAST,uglast)
PROTOCCALLSFSUB0(GBATCH,gbatch)
#define GBATCH() CCALLSFSUB0(GBATCH,gbatch)
//
void (handler)(int);
main(){
     *signal(SIGFPE, handler);
  //  cout << "Debug ?";
  AMSgvolume::debug=0;
    GZEBRA(NWGEAN);
    HLIMIT(-NWPAW);
    UGINIT();
    GRUN();
    UGLAST();
    
return 0;
}
void (handler)(int sig){
  if(sig==SIGFPE)cerr <<" FPE intercepted"<<endl;
}

