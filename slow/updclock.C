//  $Id: updclock.C,v 1.4 2001/01/22 17:32:54 choutko Exp $
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
time_t Time;
        switch (argc) {
        case 1:
         cerr <<"updclock-F-Please give  time diff in sec"<<endl;
//         return 1;
         Time=100;
           break;
        default:
         sscanf(argv[1],"%d",&Time);
         break;
        }
ofstream fbin;
fbin.open("clock");
if(fbin){
  fbin << Time;
  
}
else{
  cerr <<" updclock-F-unable to open file "<<"/Offline/vdev/slow/clock"<<endl;
}

return 0;
}
