#include <stdlib.h>
#include <math.h>
#include "../include/commons.h"
AMSDATADIR_DEF AMSDATADIR;

int main(){
double z=3.3e10;
double y=sqrt(z);
long ny=y;
ny=ny%100;
if(AMSCommonsI::remote()){
exit(1);
}
char dt[128]="touch /tmp/test26      ";
system(dt);
return 0;
}

