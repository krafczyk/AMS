// Author A. Klimentov  10-Apr-1997
//
//
// Last Edit: Dec 3, 1997 ak.
//
#include <iostream.h>
#include <string.h>
#include <typedefs.h>
#include <sys/time.h>
#include <sys/times.h>
#include <cern.h>


class dbEvent : public ooObj {

private:

uinteger _run;           
uinteger _eventNumber;   
time_t   _time;          


public:



// constructors
dbEvent() {};
dbEvent(uinteger run, uinteger eventNumber, time_t time) :
        _run(run), _eventNumber(eventNumber), _time(time) {}

// get/set methods

uinteger Run()      const { return _run;}
uinteger Event()    const { return _eventNumber;}
time_t   Time()     const { return _time;}

void    setrun(uinteger run)      {_run = run;}
void    setevent(uinteger event)  {_eventNumber = event;}
void    settime(time_t time)      {_time = time;}

void print() { cout<<"run, event,time... "<<_run<<", "<<_eventNumber
                   <<", "<<asctime(localtime(&_time))<<endl;}

};

