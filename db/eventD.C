// method source file for the object AMSEvent
// May 06, 1996. ak. First try with Objectivity 
//
// last edit Oct 04, 1996. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <trrechit_ref.h>
#include <tcluster_ref.h>
#include <tmccluster_ref.h>
#include <mctofclusterD_ref.h>
#include <ttrack_ref.h>
#include <tofrecD_ref.h>
#include <tbeta_ref.h>
#include <chargeD_ref.h>
#include <particleD_ref.h>
#include <eventD.h>


AMSEventD::AMSEventD (uinteger EventNumber, integer run, integer runtype, 
                      time_t time, char* ID)
{
  fEventNumber = EventNumber;
  _run         = run;
  _runtype     = runtype;
  _Timestamp   = time;
  fTriggerMask = fErrorCode = 0;
  //if (ID) strcpy(fID, ID);
  fID = ID;
}

void AMSEventD::Print()
{
 cout <<" Event " << fEventNumber<<" from Run "<<_run<<endl;
 cout <<" ID "<<fID<<endl;
}



void AMSEventD::getNumbers(integer* ibuff, uinteger* ubuff, time_t& time)
{
  UCOPY(&_run,ibuff,sizeof(integer)*3/4);
  UCOPY(&fEventNumber,ubuff,sizeof(uinteger)*2/4);
  time = _Timestamp;
}
