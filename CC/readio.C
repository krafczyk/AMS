#include <io.h>
#include <commons.h>
int main(int argc, char* argv[]){ 
  char *fnam=0;
  switch (argc) {
  case 1:
    cerr <<"readio-F-Please give a file name as first parameter "<<endl;
    return 1;
  case 2:
   fnam = new char[strlen(argv[1])+1];
   AMSIO::setfile(argv[1]);
   break;
  default:
   cerr <<"readio-F-Too many parameters"<<endl;
   return 1;
  }
  SELECTFFKEY.Run=-1;
  AMSIO::init(1,1);
return 0;
}
