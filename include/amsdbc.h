//  $Id: amsdbc.h,v 1.29 2001/08/03 17:28:09 choutko Exp $
// Author V. Choutko 24-may-1996
#ifndef __AMSDBC__
#define __AMSDBC__
#include <typedefs.h> 
#include <astring.h>

class amsglobalerror{
private:
AString _a;
uinteger _level;
public:
const char * getmessage(){return (const char*)_a;};
uinteger getlevel(){return _level;}
amsglobalerror(char* a,uinteger level=1):_a(a),_level(level){};
amsglobalerror(const char* a=0,uinteger level=1):_a(a),_level(level){};
};

class AMSDBc {
//
// Geometrical and other constants etc 
//
public:

#ifdef __DB__
   friend class AMSDBcD;
#endif
static char * amsdatabase;
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
     static const integer TRDTRACK;
     static const integer LEAK;
     static const integer CATLEAK;
     static const integer JUNK;

// machine dependent values

    static integer BigEndian;
  static integer debug;
  static void init();
};




#endif
