#include <daqevt.h>
#include <commons.h>
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  char *pnull=0;
  char *fnam=0;
  switch (argc) {
  case 1:
    cerr <<"readdaq-F-Please give a file name as first parameter "<<endl;
    return 1;
  default:
   break;
  }
  int i;
  for(i=1;i<argc;i++){
   delete [] fnam;
   fnam = new char[strlen(argv[i])+1];
   cout <<"readdaq-I-Open file "<<argv[i]<<endl;

   DAQEvent::setfiles(argv[i],pnull);
   SELECTFFKEY.Run=-1;
   DAQEvent::init(1,1);
  }
return 0;
}
