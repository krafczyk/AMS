// Author V. Choutko 24-may-1996
#ifndef __AMSDBC__
#define __AMSDBC__
#include <typedefs.h> 
#include <astring.h>

class amsglobalerror{
private:
AString _a;
public:
const char * getmessage(){return (const char*)_a;};
amsglobalerror(char* a):_a(a){};
amsglobalerror(const char* a=0):_a(a){};
};

class AMSDBc {
//
// Geometrical and other constants etc 
//
public:

#ifdef __DB__
   friend class AMSDBcD;
#endif

// constants
   static const number raddeg;
   static const number pi;
   static const number twopi;
// AMS parameters
     static  geant ams_size[3];
     static number ams_nrm[3][3];
     static const integer ams_rotmno;
     static const char ams_name[129];
     static  geant ams_coo[3];
//bits
     static const integer AMBIG;
     static const integer BAD;
     static const integer USED;
     static const integer DELETED;
     static const integer RELEASED;
     static const integer REFITTED;
     static const integer WIDE;
     static const integer BADHIS;
     static const integer ONESIDE;
     static const integer BADTIME;
     static const integer NEAR;
     static const integer WEAK;
     static const integer AwayTOF;
     static const integer FalseX;
     static const integer FalseTOFX;
     static const integer RECOVERED;
     static const integer LocalDB;
     static const integer GlobalDB;
     static const integer CHARGEUSED;
     static const integer GOOD;
     static const integer BADINTERPOL;
     static const integer NOTRACK;
     static const integer AOVERFLOW;

// machine dependent values

    static integer BigEndian;
  static integer debug;
};




#endif
