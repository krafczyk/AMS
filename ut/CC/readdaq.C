//  $Id: readdaq.C,v 1.6 2008/01/23 15:34:33 choutko Exp $
#include "daqevt.h"
#include "commons.h"
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  int id=0;
  char *pnull=0;
  char *fnam=0;
  char fnamd[]="/scr/choutko/AMS/datacards/daq.sea.dat";
  switch (argc) {
  case 1:
    cerr <<"readdaq-F-Please give a file name as first parameter "<<endl;
    id=1;
    argc=2;
  default:
   break;
  }
  int i;
  DAQCFFKEY.BTypeInDAQ[0]=0;
  DAQCFFKEY.BTypeInDAQ[1]=1023;
  for(i=1;i<argc;i++){
   delete [] fnam;
   if(id==0){
     fnam = new char[strlen(argv[i])+1];
     cout <<"readdaq-I-Open file "<<argv[i]<<endl;
     DAQEvent::setfiles(argv[i],pnull);
   }
   else {
      
     cout <<"readdaq-I-Open file "<<fnamd<<endl;
     DAQEvent::setfiles(fnamd,pnull);
   }
   SELECTFFKEY.Run=-1;
   SELECTFFKEY.Event=0;
   DAQEvent::init();
  }
return 0;
}
