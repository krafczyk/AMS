//  $Id: mcevent.C,v 1.4 2001/01/22 17:32:30 choutko Exp $
// method source file for the object AMSEvent
// May 06, 1996. ak. First try with Objectivity 
//
// last edit Apr 29, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mcevent.h>


AMSmcevent::AMSmcevent (integer run, integer eventNumber, time_t time,
                        integer runtype){
  cout << "AMSmcevent: Run= " << run << " Event= " << eventNumber << endl;
  setrun(run);
  setevent(eventNumber);
  settime(time);
  _runtype       = runtype;
}


void AMSmcevent::readEvent(integer& run, integer& eventNumber, time_t& time,
                           integer& runtype)
{
  run          = Run();
  eventNumber  = Event();
  time         = Time();
  runtype      = _runtype;
}
