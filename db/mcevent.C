// method source file for the object AMSEvent
// May 06, 1996. ak. First try with Objectivity 
//
// last edit Apr 29, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <mcevent.h>


AMSmcevent::AMSmcevent (integer run, uinteger eventNumber, time_t time,
                        integer runtype, number northpolephi, 
                        number stationtheta, number stationphi)
{
  setrun(run);
  setevent(eventNumber);
  settime(time);
  _NorthPolePhi  = northpolephi;
  _StationTheta  = stationtheta;
  _StationPhi    = stationphi;
  _runtype       = runtype;
}


void AMSmcevent::readEvent(uinteger& run, uinteger& eventNumber, time_t& time,
                           integer& runtype, number& northpolephi, 
                           number& stationtheta, number& stationphi)

{
  run          = Run();
  eventNumber  = Event();
  time         = Time();
  northpolephi = _NorthPolePhi;
  stationtheta = _StationTheta;
  stationphi   = _StationPhi;
  runtype      = _runtype;
}
