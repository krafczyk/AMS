// Author A. Klimentov  10-Apr-1997
//
//
// Last Edit: May 30, 1997 ak.
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

// uinteger _runAux;        // auxillary run number
// uinteger _runOff;        // run number set by offline
// uinteger _statusR;       // reco status
uinteger _RunType;

number   _NorthPolePhi;
number   _StationTheta;
number   _StationPhi;

public:

// Associations
ooRef(AMSraweventD) itsRawEvent : delete (propagate);
ooRef(AMSeventD)    itsRecEvent : delete (propagate);
ooRef(AMSmcevent)   itsMCEvent  : delete (propagate);


// constructors
AMSEventTag() {};
AMSEventTag(uinteger run, uinteger eventNumber);
AMSEventTag(uinteger run, uinteger RunType, uinteger eventNumber, 
            time_t time);


// get/set methods

uinteger runType()   const { return _RunType;}
uinteger run()         {return Run();}
// uinteger runOff()   const {return  _runOff;}
void GetGeographicCoo(number & pole, number & theta, number &phi){
                      pole=_NorthPolePhi;theta=_StationTheta;phi=_StationPhi;}

// void     setrunOff(uinteger run) {_runOff = run;}
void SetGeographicCoo(number pole, number theta, number phi){
                      _NorthPolePhi = pole;
                      _StationTheta = theta;
                     _StationPhi    = phi;}
};

