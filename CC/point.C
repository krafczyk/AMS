// Author V. Choutko 24-may-1996
 
#include <typedefs.h>
#include <point.h>
#include <math.h>
AMSPoint  AMSPoint::mm3(number m1[][3]){
  const int s=3;
   number x,y,z;
    x=m1[0][0]*_x+m1[1][0]*_y+m1[2][0]*_z;
    y=m1[0][1]*_x+m1[1][1]*_y+m1[2][1]*_z;
    z=m1[0][2]*_x+m1[1][2]*_y+m1[2][2]*_z;
   return AMSPoint(x,y,z);
  }
void AMSDir::_copy(number u, number v, number w){
  if(u==0 && v==0 && w==0){
   _x=0;
   _y=0;
   _z=0;
  }
  else{
   number s=sqrt(u*u+v*v+w*w);
   if(s !=0){
    _x=u/s; 
    _y=v/s; 
    _z=w/s; 
   }
  }
}
