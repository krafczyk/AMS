//  $Id: eventTag.C,v 1.5 2001/01/22 17:32:29 choutko Exp $
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
  _RunType = -1;
//  _runAux = 0;
//  _runOff = 0;
}

AMSEventTag::AMSEventTag(uinteger run, uinteger runType, 
                         uinteger eventNumber, time_t time)
{
  setrun(run);
  setevent(eventNumber);
  settime(time);
  _RunType = runType;
//  _runOff = 0;
}
  
