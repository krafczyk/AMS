#ifndef __AMSSTATUS__
#define __AMSSTATUS__ 
#include <node.h>
#include <typedefs.h>
#include <string.h>
#include <cern.h>
#include <amsdbc.h>
#include <time.h>
const integer STATUSSIZE=48500;
const integer MAXDAQRATE=1500;
class AMSStatus : public AMSNode {
protected:
  time_t _Begin;
  time_t _End;
  integer _Hint;
  integer _Errors;
  integer _Accepted;
  integer _Rejected;
  uinteger _Run;
  integer  _Nelem;
  uinteger _Status[3][STATUSSIZE+MAXDAQRATE];  //eventno, status, offset
   void _init();
  AMSStatus (const AMSStatus&);   // do not want cc
  AMSStatus &operator=(const AMSStatus&); // do not          
  virtual ostream & print(ostream &)const;
  static integer _Mode;   // 0 initial state; 1 read ; 2 write; 3 update
  integer _statusok(uinteger status);
public:
  ~AMSStatus(){
   if(_Errors)cout<<"AMSStatus Errors : "<<_Errors<<endl;
   if(_Accepted)cout<<"AMSStatus Accepted : "<<_Accepted<<endl;
   if(_Rejected)cout<<"AMSStatus Rejected : "<<_Rejected<<endl;
  }
  AMSStatus (): AMSNode(0),_Nelem(0){};
  AMSStatus (const char name[]): AMSNode(0),_Accepted(0),_Rejected(0),_Errors(0),_Hint(0),_Nelem(0),_Begin(0),_End(0),_Run(0){setname(name);setid(0);}
  AMSStatus * next(){return (AMSStatus *)AMSNode::next();}
  AMSStatus * prev(){return (AMSStatus *)AMSNode::prev();}
  AMSStatus * up(){return   (AMSStatus *)AMSNode::up();}
  AMSStatus * down(){return (AMSStatus *)AMSNode::down();}
  integer getsize(){return sizeof(_Run)+sizeof(_Nelem)+sizeof(_Status);}
  void * getptr(){return &_Run;}
  void updates(uinteger run, uinteger evt, uinteger status, time_t time=0);
  void adds(uinteger run, uinteger evt, uinteger status, time_t time=0);
  integer statusok(uinteger evt,uinteger run);
  integer   getnextok();
  static void UpdateStatusTableDB();
  uinteger getstatus(uinteger evt, uinteger run);
  void geteventpos(uinteger run, uinteger evt, uinteger curevent);
  static void create();
  static void init();
  static void setmode(integer mode){_Mode=mode;}
  static integer isDBWriteR(){return _Mode==2;}
  static integer isDBUpdateR(){return _Mode==3;}
  integer isFull(uinteger run, uinteger evt, time_t time);
  void reset(){_Nelem=0;}
  time_t getbegin(){return _Begin;}
  time_t getend(){return _End;}
};

#endif
