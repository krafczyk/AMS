// Author V. Choutko 24-may-1996
// 
// Oct 04, 1996. add _ContPos
// 
// last edit : Oct 04, 1996. ak.
// 

#ifndef __AMSlink__
#define __AMSlink__
#include <typedefs.h>
#include <upool.h>
#include <apool.h>
#include <id.h>
class AMSlink {
protected:
 integer _status;
 integer _pos;
 integer _ContPos;
  virtual void _erase();
  virtual void _print(ostream & stream);
  virtual void _write();
  virtual void _copy();
  virtual void _copyEl()=0;
  virtual void _printEl(ostream &stream)=0;
  virtual void _writeEl()=0;
public:
 integer checkstatus(integer checker) const{return _status & checker;}
 void setstatus(integer status){_status=_status | status;}
 void clearstatus(integer status){_status=_status & ~status;}    
 
 virtual void resethash(integer id, AMSlink *head){};
  virtual AMSID crgid(integer i=0){return AMSID();}
  virtual integer operator < ( AMSlink & o) const;
  AMSlink * _next;
  AMSlink(integer status,AMSlink * n=0): 
  _next(n),_pos(0), _ContPos(0),_status(status){};
  AMSlink(integer status=0): 
  _next(0),_pos(0), _ContPos(0),_status(status){};
  virtual ~AMSlink(){};
  integer getpos()const{return _pos;}
  void setpos(integer pos){_pos=pos;}
//+
  integer getContPos() {return _ContPos;}                           
  void    setContPos(integer pos) {_ContPos = pos;}
//-
  void * operator new(size_t t, void *p) {return p;}
  void * operator new(size_t t) {return UPool.insert(t);}
  void operator delete(void *p)
   {if(p){((AMSlink*)p)->~AMSlink();UPool.udelete(p);}}
  inline AMSlink*  next(){return _next;}
  AMSlink*  next(integer & last);
  virtual integer   testlast();
  friend class AMSContainer;


};
#endif
