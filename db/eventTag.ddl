// Author A. Klimentov  10-Apr-1997
//
//
// Last Edit: Apr 29, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <dbevent_ref.h>
#include <dbevent.h>

class AMSeventD;
#pragma ooclassref AMSeventD <recevent_ref.h>

class AMSmcevent;
#pragma ooclassref AMSmcevent <mcevent_ref.h>

class AMSraweventD;
#pragma ooclassref AMSraweventD <raweventD_ref.h>



class AMSEventTag : public dbEvent {

private:


uinteger _runAux;        // auxillary run number
uinteger _status;        // event status, tag, label, type
uinteger _runOff;        // run number set by offline
uinteger _statusR;       // reco status

public:

// Associations
ooRef(AMSraweventD) itsRawEvent : delete (propagate);
ooRef(AMSeventD)    itsRecEvent : delete (propagate);
ooRef(AMSmcevent)   itsMCEvent  : delete (propagate);


// constructors
AMSEventTag() {};
AMSEventTag(uinteger run, uinteger eventNumber);
AMSEventTag(uinteger runUni, uinteger runAux, uinteger eventNumber, 
            time_t time, uinteger status);


// get/set methods

uinteger runAux()   const { return _runAux;}
uinteger runUni()         {return Run();}
uinteger status()   const { return _status;}
uinteger runOff()   const {return  _runOff;}

void     setrunOff(uinteger run) {_runOff = run;}

};

