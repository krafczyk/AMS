//  $Id: point.h,v 1.15 2007/11/28 12:06:49 pzuccon Exp $
// Author V. Choutko 24-may-1996

#ifndef __AMSPOINT__
#define __AMSPOINT__
#include "typedefs.h"
#include <iostream.h>
#include <math.h>
class AMSRotMat;
using namespace std;

//! A general AMS class for defining a point in space
class AMSPoint {
protected:
  //! Internal representation in cartesian coo X
  number _x;
  //! Internal representation in cartesian coo Y
  number _y;
  //! Internal representation in cartesian coo Z
  number _z;
public:
  //! Default constructor it sets the point to (0,0,0)
  AMSPoint():_x(0),_y(0),_z(0){};
  //! Explicit constructor
  AMSPoint(number x, number y,number z):_x(x),_y(y),_z(z){};
  //! Explicit constructor
  AMSPoint(number coo[3]): _x(coo[0]),_y(coo[1]),_z(coo[2]){};
  //! Explicit constructor
  AMSPoint(geant coo[3]): _x(coo[0]),_y(coo[1]),_z(coo[2]){};
  //! it sets the position
  void setp(number x,number y,number z){_x=x;_y=y;_z=z;}
  //! it sets the position
  //! it sets the position
  void setp(number x[]){_x=x[0];_y=x[1];_z=x[2];}
  //! it gets the postion to the three args
  void getp(number &x, number &y, number &z)const{x=_x;y=_y;z=_z;}
  //! it gets the postion to the three args
  void getp(geant &x, geant &y, geant &z){x=_x;y=_y;z=_z;}
  //! it gets the postion to array arg
  void getp(number x[]){x[0]=_x;x[1]=_y;x[2]=_z;}
  //! it does the matrix product (right) with a matrix represented by the input array
  AMSPoint mm3(number [][3]);
  //! it does the matrix product (left) with a matrix represented by the input array
  AMSPoint mm3i(number [][3]);
  //! Equivalence  operator it reuire that the two point are the same 
  integer operator ==(const AMSPoint & o) const{return _x==o._x && _y==o._y &&_z==o._z;}
  //! less than operator it requires that all the threee coo are smaller than those of o
  integer operator <(AMSPoint o) const{return _x<o._x && _y<o._y &&_z<o._z;}
  //! lessequal  than operator it reuqires that all the threee coo are smaller or equal than those of o
  integer operator <=(AMSPoint o) const{return _x<=o._x && _y<=o._y &&_z<=o._z;}
  //! moreequal  than operator it reuqires that all the threee coo are larger or equal than those of o
  integer operator >=(AMSPoint o) const{return _x>=o._x && _y>=o._y &&_z>=o._z;}
  //! it returns a point in the positive octant
  AMSPoint abs(){return AMSPoint(fabs(_x),fabs(_y),fabs(_z));}
  //! it returns the vector sum of two AMSPoints
  AMSPoint  operator+( AMSPoint o)
    const{return AMSPoint(_x+o._x,_y+o._y,_z+o._z);}
  //! it returns the vector difference of two AMSPoints
  AMSPoint  operator-( AMSPoint o)const
    {return AMSPoint(_x-o._x,_y-o._y,_z-o._z);}
  //! it returns an AMSPoints whose mebers are the product member to member of the two AMSPoints
  AMSPoint  operator*( AMSPoint o)const{
      return AMSPoint(_x*o._x,_y*o._y,_z*o._z);}
  //! it returns an AMSPoint scaled by the factor o
  AMSPoint  operator*( number o)const{return AMSPoint(_x*o,_y*o,_z*o);}
  //! it returns an AMSPoints whose mebers are the division member to member of the two AMSPoints
  AMSPoint  operator/( AMSPoint o)const{
      return AMSPoint(_x/o._x,_y/o._y,_z/o._z);}
  //! it returns an AMSPoint with all the coo incremented by o
  AMSPoint  operator+( number o)const{return AMSPoint(_x+o,_y+o,_z+o);}
  //! it returns an AMSPoint scaled by the factor 1/o
  AMSPoint  operator/( number o)const{return AMSPoint(_x/o,_y/o,_z/o);}
  //! it returns an AMSPoint scaled by the factor 1/o 
  AMSPoint  operator/( integer o)const{return AMSPoint(_x/o,_y/o,_z/o);}
  //! it sets the current AMSPoint object to (o,o,o)
  AMSPoint&  operator =(integer o){_x=o;_y=o;_z=o;return *this;}
  //! it sets the current AMSPoint object to (o,o,o)
  AMSPoint&  operator =(number o){_x=o;_y=o;_z=o;return *this;}
  //! assignement operator by copy
  AMSPoint&  operator =(const AMSPoint & o){_x=o._x;_y=o._y;_z=o._z;return *this;}
  //! it return the vector scalar product of the two AMSPoints
  number prod(AMSPoint o)const{return (_x*o._x+_y*o._y+_z*o._z);}
  //! it return the vector module of the AMSPoint
  number norm()const {return sqrt(_x*_x+_y*_y+_z*_z);}
  //! it returns the distance between two AMSPoints
  number dist(AMSPoint o)const{return sqrt((_x-o._x)*(_x-o._x)+
	(_y-o._y)*(_y-o._y)+
	(_z-o._z)*(_z-o._z));}
  //! it allow the use of the array notation fot an ams point
  number &  operator[](integer i){
      if(i<=0)return _x;else if(i==1)return _y; else return _z;}
  //! it allow the use of the array notation fot an ams point
  number   operator[](integer i) const{
      if(i<=0)return _x;else if(i==1)return _y; else return _z;}

  //! print out to a stream the values of the AMSPoint
  friend ostream &operator << (ostream &o, const  AMSPoint &b )
  {return o<<" "<<b._x<<" "<<b._y<<" "<<b._z;}

  //! read from a stream the values of the AMSPoint
  friend istream &operator >> (istream &o,   AMSPoint &b )
  {return o>>b._x>>b._y>>b._z;}
  //! allow the rigth matrix product with an AMSRotMat
  friend AMSPoint operator*(const AMSRotMat  &mat, const AMSPoint &Point);

  //! it returns an AMSPoint which is the vector cross product
  AMSPoint crossp(const AMSPoint & o);
#ifdef __WRITEROOT__
    friend class AMSPointRoot;
#endif
};


//! A general AMS class to define a direction in space. It is an AMSPoint with unit verctor module.
class AMSDir :public AMSPoint{
  private:
  
    void _copy(number x,number y,number z);
  public:
  //! Standard constructor  it sets the dirction to (0,0,0)
  AMSDir():AMSPoint(){};
  //! explicit condstructor from an AMSPoint
  AMSDir(const AMSPoint& o){number x,y,z;o.getp( x,y,z);_copy(x,y,z);}
  //! explcit constructor in polar notation
  AMSDir(number theta, number phi);
  //! it returns the zenith angle
  number gettheta() const{return acos(_z);}
  //! it returns the azimutal angle
  number getphi() const{return atan2(_y,_x);}
  //! copy constructor
  AMSDir(const AMSDir& o){_copy(o._x,o._y,o._z);}
  //! explicit constructor from coo
  AMSDir(number x, number y,number z){_copy(x,y,z);}
  //! explicit constructor from an array
  AMSDir(number x[]){_copy(x[0],x[1],x[2]);}
  //! it sets the direction from coo
  void setd(number x,number y,number z){_copy(x,y,z);}
  //! it sets the direction from an array
  void setd(number x[]){_copy(x[0],x[1],x[2]);}
  //! it does the vector cross product of two AMSDir
  AMSDir cross(const AMSDir & o);
};

//!A Genral AMS class to implements a 3D rotation matrix
class AMSRotMat{
  
protected:
  //! internal representation of the rotation matrix
  number _nrm[3][3];
public:

  //!null constructor it create a identy   rotation matrix {{1,0,0},{0,1,0},{0,0,1}} 
  AMSRotMat(){Reset();}
  //! copy constructor
  AMSRotMat(const AMSRotMat& orig);  
  //! explicit constructor from an array pointer
  AMSRotMat(number** nin);
  //! explicit constructor from an array
  AMSRotMat(number nin[][3]);
  ~AMSRotMat(){}
  
  //! set the rotation matrix from an array pointer
  void SetMat(number** nin);
  //! set the rotation matrix from an array 
  void SetMat(number nin[][3]); 
  //! does an parity transformation on X
  void XParity();
  //! does an parity transformation on Y
  void YParity();
  //! does an parity transformation on Z
  void ZParity();
  //! it return the matrix element with the requested row,column
  number GetEl(int row,int col){return _nrm[row][col];}
  //!it reset the matrix a no rotation one {{1,0,0},{0,1,0},{0,0,1}} 
  void Reset();
  //! it stream out the content of an AMSRotMat
  friend ostream &operator << (ostream &o, const  AMSRotMat &b )
  {return o<<
      b._nrm[0][0]<<" "<<b._nrm[0][1]<<" "<<b._nrm[0][2]<<" "<<endl<<
      b._nrm[1][0]<<" "<<b._nrm[1][1]<<" "<<b._nrm[1][2]<<" "<<endl<<
      b._nrm[2][0]<<" "<<b._nrm[2][1]<<" "<<b._nrm[2][2]<<" "<<endl;}

  //! it reads from a stream  the content of an AMSRotMat
  friend istream &operator >> (istream &o,  AMSRotMat &b )
  {return o>>
      b._nrm[0][0]>>b._nrm[0][1]>>b._nrm[0][2]>>
      b._nrm[1][0]>>b._nrm[1][1]>>b._nrm[1][2]>>
      b._nrm[2][0]>>b._nrm[2][1]>>b._nrm[2][2];}
  
  //! it does the right protduct of AMSRotMat on an AMSPoint
  friend AMSPoint operator*(const AMSRotMat& mat, const AMSPoint& Point);
  //! it does the matrix product (right) with another AMSRotMat 
  AMSRotMat & operator*(const AMSRotMat& matin);
  //! copy operator
  AMSRotMat& operator=(const AMSRotMat& orig);
  //! it requires that the two matrix are identical
  bool operator==(const AMSRotMat& orig);
};


#endif



