// Author A. Klimentov  10-Apr-1997
//
// method file for AMSEventTag class
//
// Last Edit: Apr 10, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <eventTag.h>
AMSEventTag::AMSEventTag(uinteger run, uinteger eventNumber)
{
  setrun(run);
  setevent(eventNumber);
}

AMSEventTag::AMSEventTag(uinteger runUni, uinteger runAux, 
                         uinteger eventNumber, time_t time, uinteger status)
{
  setrun(runUni);
  setevent(eventNumber);
  settime(time);
  _runAux = runAux;
  _status = status;
  _runOff = 0;
}
  
