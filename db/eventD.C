// method source file for the object AMSEvent
// May 06, 1996. ak. First try with Objectivity 
//
// last edit Apr 01, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <trrechit_ref.h>
#include <tcluster_ref.h>
#include <tmcclusterV_ref.h>
#include <mctofclusterV_ref.h>
#include <ttrack_ref.h>
#include <tofrecD_ref.h>
#include <tbeta_ref.h>
#include <chargeD_ref.h>
#include <particleD_ref.h>
#include <eventD.h>


AMSEventD::AMSEventD (integer run, uinteger eventNumber, integer runtype, 
                      time_t time)
{
  _eventNumber = eventNumber;
  _runNumber   = run;
  _runType     = runtype;
  _time        = time;
  _triggerMask = 0;
}

void AMSEventD::Print()
{
 cout <<" Event " << _eventNumber<<" from Run "<<_runNumber<<endl;
}



void AMSEventD::getNumbers
              (integer& run, integer& runtype, uinteger& eventn, time_t& time)
{
  run     = _runNumber;
  runtype = _runType;
  eventn  = _eventNumber;
  time    = _time;
}
