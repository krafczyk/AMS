// Author V. Choutko 24-may-1996
 
#ifndef __AMSID__
#define __AMSID__
#include <typedefs.h>
#include <strstream.h>
#include <stddef.h>
#include <stdlib.h>
class AMSID
{
 protected:
// realization
  integer _id;
  char * _name;
  void _copyname( char  const [] );
  virtual ostream & print(ostream &) const;
// Error handling
  static char *_error;
  static char _pull[];
 public:
   AMSID (): _id(0),_name(0){}
   AMSID(integer id, const char name[]=0);
   AMSID(const char name[],integer id=0);
   AMSID (const AMSID&);
   virtual ~AMSID();

   AMSID &operator=(const AMSID&);
   AMSID operator+(const AMSID&);
   AMSID operator-(const AMSID&);
   AMSID operator*(const AMSID&);
   friend ostream &operator << (ostream &o, const AMSID &b );
// compare
   int operator < (const AMSID&) const;
   int operator == (const AMSID&) const;
// get
   inline integer getid() const{return _id;}
   inline char *  getname() const{return _name;}
// set
   inline integer & id(){return _id;}  //getsetid
   inline void setid(integer id){_id=id;}
   inline void setname(const char name []){delete[] _name;_copyname(name);}
// Error handling decl
  static void ResetError();
  static char * IsError();
  static void PrintError();

};
#endif
