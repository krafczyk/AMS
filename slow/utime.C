//  $Id: utime.C,v 1.2 2001/01/22 17:32:54 choutko Exp $
#include <iostream.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
main(int argc, char* argv[]){
        switch (argc) {
        case 1:
         cerr <<"utime-F-Please give input unix time "<<endl;
         return 1;
        default:
         break;
        }
time_t Time;
        sscanf(argv[1],"%d",&Time);
cout <<ctime(&Time)<<endl;
return 0;
}
