//  $Id: link.h,v 1.11 2002/09/17 12:12:19 choutko Exp $
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
class TObject;
class AMSlink {
protected:
 uinteger _status;
 integer _pos;
 TObject *_ptr; 
  virtual void _erase();
  virtual void _print(ostream & stream);
  virtual void _write();
  virtual void _copy();
  virtual void _copyEl()=0;
  virtual void _printEl(ostream &stream)=0;
  virtual void _writeEl()=0;
public:
 uinteger checkstatus(integer checker) const{return _status & checker;}
 uinteger getstatus() const{return _status;}
 void setstatus(uinteger status){_status=_status | status;}
 void clearstatus(uinteger status){_status=_status & ~status;}    
 TObject *GetClonePointer(){ return _ptr;}
 void SetClonePointer(TObject *ptr){ _ptr=ptr;}
 virtual void resethash(integer id, AMSlink *head){};
  virtual AMSID crgid(integer i=0){return AMSID();}
  virtual integer operator < ( AMSlink & o) const;
  AMSlink * _next;
  AMSlink(uinteger status,AMSlink * n=0): 
  _next(n),_pos(0), _ptr(0),_status(status){};
  AMSlink(integer status=0): 
  _next(0),_pos(0), _ptr(0),_status(status){};
  virtual ~AMSlink(){};
  integer getpos()const{return _pos;}
  void setpos(integer pos){_pos=pos;}
//  void * operator new(size_t t, void *p) {return p;}
//  void  operator delete(size_t t, void *p) {p=0;}
  void * operator new(size_t t) {return UPool.insert(t);}
  void operator delete(void *p)
   {UPool.udelete(p);p=0;}
  void erase(){_erase();delete this;} 
  inline AMSlink*  next(){return _next;}
  AMSlink*  next(integer & last);
  virtual integer   testlast();
  friend class AMSContainer;


};
#endif
