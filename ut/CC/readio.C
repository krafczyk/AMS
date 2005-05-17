//  $Id: readio.C,v 1.6 2005/05/17 09:54:05 pzuccon Exp $
#include "io.h"
#include "trid.h"
#include "commons.h"
AMSDATADIR_DEF AMSDATADIR;
int main(int argc, char* argv[]){ 
  char *fnam=0;
  switch (argc) {
  case 1:
    cerr <<"readio-F-Please give a file name as first parameter "<<endl;
    return 1;
  default:
   break;
  }
  int i;
  for(i=1;i<argc;i++){
   delete [] fnam;
   fnam = new char[strlen(argv[i])+1];
   cout <<"readio-I-Open file "<<argv[i]<<endl;
   AMSIO::setfile(argv[i]);
   SELECTFFKEY.Run=-1;
   AMSIO::init(1,1);
  }
return 0;
}
