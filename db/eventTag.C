// Author A. Klimentov  10-Apr-1997
//
// method file for AMSEventTag class
//
// Last Edit: Jun 25, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <eventTag.h>

AMSEventTag::AMSEventTag(uinteger run, uinteger eventNumber)
{
  setrun(run);
  setevent(eventNumber);
  _runAux = 0;
  _runOff = 0;
}

AMSEventTag::AMSEventTag(uinteger runUni, uinteger runAux, 
                         uinteger eventNumber, time_t time)
{
  setrun(runUni);
  setevent(eventNumber);
  settime(time);
  _runAux = runAux;
  _runOff = 0;
}
  
