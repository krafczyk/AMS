#include <iostream.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <math.h>
main(){
time_t Time;
time(&Time);
timestruc_t TimeStruct;
//curtime(&TimeStruct);
//Time=TimeStruct.tv_sec;
char  TimeC[26];
//TimeC=ctime(&Time);
char * str=new char[200];
strcat(str,"jopa333 ");
strcat(str,ctime(&Time));
strcat(str," jopa444");
cout <<str<<endl;
cout <<Time<<endl;
double t=-1.345;
cout <<abs(t)<<endl;
return 0;
}
