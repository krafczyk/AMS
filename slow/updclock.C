#include <iostream.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <fstream.h>
main(int argc, char* argv[]){
        switch (argc) {
        case 1:
         cerr <<"updclock-F-Please give  time diff in sec"<<endl;
         return 1;
        default:
         break;
        }
time_t Time;
        sscanf(argv[1],"%d",&Time);
ofstream fbin;
fbin.open("/Offline/vdev/slow/clock");
if(fbin){
  fbin << Time<<endl;;
  
}
else{
  cerr <<" updclock-F-unable to open file "<<"/Offline/vdev/slow/clock"<<endl;
}

return 0;
}
