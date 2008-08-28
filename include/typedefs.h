//  $Id: typedefs.h,v 1.12 2008/08/28 20:33:39 choutko Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __TYPEDEFS__
#define __TYPEDEFS__
typedef float geant;
typedef double number;
typedef int integer;
typedef float float_r;
typedef double float_d;
typedef unsigned char uchar;
typedef unsigned short int int16u;
typedef unsigned short int uint16;
typedef  short int int16;
typedef long int_L;
typedef unsigned int uinteger;
typedef unsigned long int ulong;
typedef unsigned long long uint64;
#include <assert.h>
#include <iostream.h>

// implemntation of a 64 bit unsigned integer
class uintl{
protected:
  uinteger _a[2];  // low 
public:
  uintl(const uintl &a){_a[0]=a._a[0];_a[1]=a._a[1];}
  uintl(integer a, integer b){_a[0]=a;_a[1]=b;}
  uintl(uinteger a=0, uinteger b=0){_a[0]=a;_a[1]=b;}
  uintl(uinteger a[2]){_a[0]=a[0];_a[1]=a[1];}

  uinteger v(uinteger l)const {return l<sizeof(_a)/sizeof(_a[0])?_a[l]:0;}

  uinteger & operator () (uinteger l) {return _a[l];}
  bool operator < (const uintl&a) const{ if(v(1)==a.v(1))return v(0)<a.v(0); else return v(1)<a.v(1);  }
  bool operator == (const uintl&a) const{ return v(1)==a.v(1) && v(0)==a.v(0);  }
  bool operator == (uinteger a) const{ return v(1)==0 && v(0)==a;  }

  friend ostream &operator << (ostream &o, const uintl &b ){return (o<<" [0] "<<b.v(0)<<" [1] "<<b.v(1));}
};


// implemntation of a 128 bit unsigned integer
class uint128{
protected:
  uint64 _a[2];  // low 
public:
  uint128(const uint128 &a){_a[0]=a._a[0];_a[1]=a._a[1];}
  uint128(integer a, integer b){_a[0]=a;_a[1]=b;}
  uint128(uint64 a=0, uint64 b=0){_a[0]=a;_a[1]=b;}
  uint128(uint64 a[2]){_a[0]=a[0];_a[1]=a[1];}

  uint64 v(uint64 l)const {return l<sizeof(_a)/sizeof(_a[0])?_a[l]:0;}

  uint64 & operator () (uint64 l) {return _a[l];}
  bool operator < (const uint128&a) const{ if(v(1)==a.v(1))return v(0)<a.v(0); else return v(1)<a.v(1);  }
  bool operator == (const uint128&a) const{ return v(1)==a.v(1) && v(0)==a.v(0);  }
  bool operator == (uint64 a) const{ return v(1)==0 && v(0)==a;  }

  friend ostream &operator << (ostream &o, const uint128 &b ){return (o<<" [0] "<<b.v(0)<<" [1] "<<b.v(1));}
};


#endif
