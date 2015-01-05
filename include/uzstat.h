//  $Id$
// Author V. Choutko 24-may-1996
#ifndef __AMSUZSTAT__
#define __AMSUZSTAT__

#include "typedefs.h"
#include <limits.h>
#include <math.h>
#include "node.h"
#include "snode.h"
#include <float.h>
//#define __LVL3ONLY__

 
/*! \class AMSStat 
   \brief The "uztime L3" like package
 The "uztime L3" like package
 to check the CPU time consuming by different code parts.
 To use it:

 AMSgObj:BookTimer.book("string");   // books the timer

 AMSgObj:BookTimer.start("string");   // starts the timer

 AMSgObj:BookTimer.stop("string");   // stops the timer & update the statistics

 AMSgObj::BookTimer.print();        // prints the timers statistics, i.e.
*/                                    // min,max,average,sum


//! High granularity time
extern "C" number HighResTime();


class AMSStatNode: public AMSNode{

  friend class AMSStat;

private:
  /// flag 0 stopped  1 started
  integer _startstop;
  number _time;  // PZ why is not set to zero in the cnstr
  number _last;  /// last count
  number _entry;
  number _sum;
  number _sum2;   ///  thread diff
  number _nsum2;   ///  thread diff
  number _max;
  number _min;
  integer _freq;
public:
  AMSStatNode()
    :AMSNode(0),_startstop(0),_last(0),
     _entry(0),_sum(0),_sum2(0),_nsum2(0),
     _max(-FLT_MAX),_min(FLT_MAX),_freq(1){};
  
  AMSStatNode(char * name, int freq)
    :AMSNode(name),_startstop(0),_last(0),
     _entry(0),_sum(0),_sum2(0),_nsum2(0),
     _max(-FLT_MAX),_min(FLT_MAX),_freq(freq){};
  
  AMSStatNode(char * name, int freq,int thr=0)
    :AMSNode(AMSID(name,thr)),_startstop(0),_last(0),
     _entry(0),_sum(0),_sum2(0),_nsum2(0),
     _max(-FLT_MAX),_min(FLT_MAX),_freq(freq){};
  
  void _init(){};
  ostream & print(ostream & stream ) const;
};


class AMSStat: public AMSNodeMap{
private:
  AMSStatNode Timer;
public: 
  AMSStat();
  ~AMSStat();
  /// Create a new timer
  //
#ifdef __MIC__ 
 void book(char * name, int freq=1,int thr=256);
#else
void book(char * name, int freq=1,int thr=128);
#endif
  void accu();
  ///Starts the timer
  void start(char * name);
  /// Returns the elapsed time during the count 
  number check(char *name);
  /// Returns the last elapsed after stop
  number Get(char *name);
  // Stop the timer
  number stop(char * name, integer force=0);
  void print(char *name);
  void print();
};



class AMSStatErrNode: public AMSNode{

  friend class AMSStatErr;

private:
  char _severity;
  uinteger _entry;
public:
  AMSStatErrNode():AMSNode(0),_severity('I'),_entry(0){};
  AMSStatErrNode(char * name, uinteger severity ):AMSNode(name),_entry(0){
    switch (severity){
    case 0:
      _severity='I';
      break;
    case 1:
      _severity='W';
      break;
    case 2:
      _severity='E';
      break;
    case 3:
      _severity='S';
      break;
    case 4:
      _severity='F';
      break;
    default:
      _severity='I';
    }
  }

  void _init(){};
  ostream & print(ostream & stream ) const;
};



class AMSStatErr: public AMSNodeMap{
private:
  AMSStatErrNode Timer;
public:
  AMSStatErr();
  ~AMSStatErr();
  void book(char * name, char severity);
  void print(char * name, char *message=0, uinteger maxprint=10);
  void print();
};
#endif
