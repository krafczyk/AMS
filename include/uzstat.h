// Author V. Choutko 24-may-1996
 
// The "uztime L3" like package
// to check the CPU time consuming by different code parts.
// To use it:
// AMSgObj:BookTimer.book("string");   // books the timer
// AMSgObj:BookTimer.start("string");   // starts the timer
// AMSgObj:BookTimer.stop("string");   // stops the timer & update the statistics
// AMSgObj::BookTimer.print();        // prints the timers statistics, i.e.
//                                    // min,max,average,sum

#ifndef __AMSUZSTAT__
#define __AMSUZSTAT__
#include <cern.h>
#include <typedefs.h>
#include <limits.h>
#include <math.h>
#include <node.h>
#include <snode.h>


// High granularity time

extern "C" number HighResTime();


class AMSStatNode: public AMSNode{

friend class AMSStat;

private:
integer _startstop;
number _time;
number _entry;
number _sum;
number _max;
number _min;
public:
AMSStatNode():AMSNode(0),_startstop(0),_entry(0),_sum(0),_max(-FLT_MAX),_min(FLT_MAX){};
AMSStatNode(char * name):AMSNode(name),_startstop(0),_entry(0),_sum(0),_max(-FLT_MAX),_min(FLT_MAX){};
void _init(){};
ostream & print(ostream & stream ) const;
};


class AMSStat: public AMSNodeMap{
private:
AMSStatNode Timer;
public: 
  AMSStat();
  ~AMSStat();
  void book(char * name);
  void start(char * name);
  number check(char *name);
  number stop(char * name);
  void print();
};



#endif


