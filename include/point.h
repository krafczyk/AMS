// Author V. Choutko 24-may-1996
 
#ifndef __AMSPOINT__
#define __AMSPOINT__
#include <typedefs.h>
#include <iostream.h>
#include <math.h>
class AMSPoint {
protected:
number _x;
number _y;
number _z;
public:
AMSPoint(){};
AMSPoint(number x, number y,number z):_x(x),_y(y),_z(z){};
AMSPoint(number coo[3]): _x(coo[0]),_y(coo[1]),_z(coo[2]){};
void setp(number x,number y,number z){_x=x;_y=y;_z=z;}
void setp(number x[]){_x=x[0];_y=x[1];_z=x[2];}
void getp(number &x, number &y, number &z)const{x=_x;y=_y;z=_z;}
void getp(geant &x, geant &y, geant &z){x=_x;y=_y;z=_z;}
void getp(number x[]){x[0]=_x;x[1]=_y;x[2]=_z;}
AMSPoint mm3(number [][3]);
integer operator <(AMSPoint o) const{return _x<o._x && _y<o._y &&_z<o._z;}
integer operator <=(AMSPoint o) const{return _x<=o._x && _y<=o._y &&_z<=o._z;}
integer operator >=(AMSPoint o) const{return _x>=o._x && _y>=o._y &&_z>=o._z;}
AMSPoint abs(){return AMSPoint(fabs(_x),fabs(_y),fabs(_z));}
AMSPoint  operator+( AMSPoint o)
          const{return AMSPoint(_x+o._x,_y+o._y,_z+o._z);}
AMSPoint  operator-( AMSPoint o)const
          {return AMSPoint(_x-o._x,_y-o._y,_z-o._z);}
AMSPoint  operator*( AMSPoint o)const{
          return AMSPoint(_x*o._x,_y*o._y,_z*o._z);}
AMSPoint  operator*( number o)const{return AMSPoint(_x*o,_y*o,_z*o);}
AMSPoint  operator/( AMSPoint o)const{
                     return AMSPoint(_x/o._x,_y/o._y,_z/o._z);}
AMSPoint  operator/( number o)const{return AMSPoint(_x/o,_y/o,_z/o);}
AMSPoint  operator/( integer o)const{return AMSPoint(_x/o,_y/o,_z/o);}
AMSPoint&  operator =(integer o){_x=o;_y=o;_z=o;return *this;}
AMSPoint&  operator =(number o){_x=o;_y=o;_z=o;return *this;}
number prod(AMSPoint o)const{return (_x*o._x+_y*o._y+_z*o._z);}
number &  operator[](integer i){
  if(i<=0)return _x;else if(i==1)return _y; else return _z;}
friend ostream &operator << (ostream &o, const  AMSPoint &b )
   {return o<<" "<<b._x<<" "<<b._y<<" "<<b._z;}
};
class AMSDir :public AMSPoint{
private:
void _copy(number x,number y,number z);
public:
AMSDir(){};
AMSDir(const AMSPoint& o){number x,y,z;o.getp( x,y,z);_copy(x,y,z);}
AMSDir(const AMSDir& o){_copy(o._x,o._y,o._z);}
AMSDir(number x, number y,number z){_copy(x,y,z);}
AMSDir(number x[]){_copy(x[0],x[1],x[2]);}
void setd(number x,number y,number z){_copy(x,y,z);}
void setd(number x[]){_copy(x[0],x[1],x[2]);}

};
#endif
