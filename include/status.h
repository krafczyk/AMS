#ifndef __AMSSTATUS__
#define __AMSSTATUS__ 
#include <node.h>
#include <typedefs.h>
#include <string.h>
#include <cern.h>
#include <amsdbc.h>
#include <time.h>
const integer STATUSSIZE=50000;
const integer MAXDAQRATE=1500;
class AMSStatus : public AMSNode {
protected:
  time_t _Begin;
  time_t _End;
  uinteger _Run;
  integer  _Nelem;
  uinteger _Status[2][STATUSSIZE+MAXDAQRATE];
  void _init();
  AMSStatus (const AMSStatus&);   // do not want cc
  AMSStatus &operator=(const AMSStatus&); // do not          
  virtual ostream & print(ostream &)const;
  static integer _Mode;
public:
  AMSStatus (): AMSNode(0),_Nelem(0){};
  AMSStatus (const char name[]): AMSNode(0),_Nelem(0),_Begin(0),_End(0),_Run(0){setname(name);setid(0);}
  AMSStatus * next(){return (AMSStatus *)AMSNode::next();}
  AMSStatus * prev(){return (AMSStatus *)AMSNode::prev();}
  AMSStatus * up(){return   (AMSStatus *)AMSNode::up();}
  AMSStatus * down(){return (AMSStatus *)AMSNode::down();}
  integer getsize(){return sizeof(_Nelem)+sizeof(_Status);}
  void * getptr(){return &_Nelem;}
  void update(uinteger run, uinteger evt, uinteger status, time_t time=0);
  uinteger getstatus(uinteger evt);
  static void create();
  static void init();
  void setmode(integer mode){_Mode=mode;}
  static integer isDBUpdate(){return _Mode==2;}
  integer isFull(uinteger run, uinteger evt, time_t time);
  time_t getbegin(){return _Begin;}
  time_t getend(){return _End;}
};

#endif
