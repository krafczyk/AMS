#include <iostream.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>
main(){
time_t Time;
tm begin;
begin.tm_sec=10;
begin.tm_min=20;
begin.tm_hour=10;
begin.tm_mday=2;
begin.tm_mon=1;
begin.tm_year=96;
//begin.tm_wday=0;
//begin.tm_yday=0;
Time=mktime(&begin);
cout <<ctime(&Time)<<endl;
return 0;
}
