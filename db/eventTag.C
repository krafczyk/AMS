// Author A. Klimentov  10-Apr-1997
//
// method file for AMSEventTag class
//
// Last Edit: Nov 14, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <eventTag.h>

AMSEventTag::AMSEventTag(uinteger run, uinteger eventNumber)
{
  setrun(run);
  setevent(eventNumber);
  _runType = -1;
}

AMSEventTag::AMSEventTag(uinteger run, uinteger runType, 
                         uinteger eventNumber, time_t time)
{
  setrun(run);
  setevent(eventNumber);
  settime(time);
  _RunType = runType;
}
  
