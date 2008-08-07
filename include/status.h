//  $Id: status.h,v 1.16 2008/08/07 18:38:52 choutko Exp $
#ifndef __AMSSTATUS__
#define __AMSSTATUS__ 
#include "node.h"
#include "typedefs.h"
#include <string.h>
#include "cern.h"
#include "amsdbc.h"
#include <time.h>
const integer STATUSSIZE=250000;
const integer MAXDAQRATE=5000;
class AMSStatus : public AMSNode {
protected:
  class statusI{
   uinteger _data[2];
   public:
   statusI(){_data[0]=0;_data[1]=0;}
   statusI(uinteger s1, uinteger s2){_data[0]=s1;_data[1]=s2;}
   uinteger *getp(){return _data;}
   uinteger  operator[](uinteger i){return i==0?_data[0]:_data[1];}
  };
  time_t _Begin;
  time_t _End;
  integer _Hint;
  integer _Errors;
  integer _Accepted;
  integer _Rejected;
  uinteger _Run;
  uint64 _Offset;   
  integer  _Nelem;
  uinteger _Status[4][STATUSSIZE+MAXDAQRATE];  //eventno, status, offset
 
   void _init();
  AMSStatus (const AMSStatus&);   // do not want cc
  AMSStatus &operator=(const AMSStatus&); // do not          
  virtual ostream & print(ostream &)const;
  static integer _Mode;   // 0 initial state; 1 read ; 2 write; 3 update
  integer _statusok(statusI status);
public:
  ~AMSStatus(){
   if(_Errors)cout<<"AMSStatus Errors : "<<_Errors<<endl;
   if(_Accepted)cout<<"AMSStatus Accepted : "<<_Accepted<<endl;
   if(_Rejected)cout<<"AMSStatus Rejected : "<<_Rejected<<endl;
  }
  AMSStatus (): AMSNode(0),_Nelem(0){};
  AMSStatus (const char name[], integer version): AMSNode(0),_Accepted(0),_Rejected(0),_Errors(0),_Hint(0),_Nelem(0),_Begin(0),_End(0),_Run(0),_Offset(0){setname(name);setid(version);}
  integer getnelem()const{return _Nelem;}
  AMSStatus * next(){return (AMSStatus *)AMSNode::next();}
  AMSStatus * prev(){return (AMSStatus *)AMSNode::prev();}
  AMSStatus * up(){return   (AMSStatus *)AMSNode::up();}
  AMSStatus * down(){return (AMSStatus *)AMSNode::down();}
  integer getsize(){return _id==0?sizeof(_Run)+sizeof(_Nelem)+3*sizeof(_Status)/4:sizeof(_Run)+sizeof(_Nelem)+sizeof(_Status)+sizeof(_Offset);}
  void * getptr(){return &_Run;}
  void updates(uinteger run, uinteger evt, uinteger* status, time_t time=0);
  void adds(uinteger run, uinteger evt, uinteger * status, time_t time=0);
  integer statusok(uinteger evt,uinteger run);
  integer   getnextok();
  static void UpdateStatusTableDB();
  statusI getstatus(uinteger evt, uinteger run);
  bool geteventpos(uinteger run, uinteger evt, uinteger curevent);
  static AMSStatus* create(int version);
  static void init();
  static void setmode(integer mode){_Mode=mode;}
  static integer isDBWriteR(){return _Mode==2;}
  static integer isDBUpdateR(){return _Mode==3;}
  integer isFull(uinteger run, uinteger evt, time_t time, bool force=false);
  void reset(){_Nelem=0;}
  time_t getbegin()const{return _Begin;}
  time_t getend()const{return _End;}
  uinteger getrun()const {return _Run;}
  void getFL(uinteger &first, uinteger & last){first=_Status[0][0];last=_Status[0][_Nelem-1];}
};

#endif
