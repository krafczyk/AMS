// Author A. Klimentov  9-Apr-1997
//
// use event format from A.Lebedev (version Jan '97) to check overhead
//
// Last Edit: Apr 9, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>

#include <dbevent_ref.h>
#include <dbevent.h>

declare (ooVArray, integer);

class AMSraweventD : public dbEvent {

private:

uinteger _runAux;        // auxillary run number
uinteger _status;        // status/tag/label

ooVArray(integer)  Trig;        // Trigger data block
ooVArray(integer)  Tracker;     // Tracker data block
ooVArray(integer)  TOF;         // Scintillator data block
ooVArray(integer)  CTC;         // Aerogel data block
ooVArray(integer)  Anti;        // AntiCoinc. data block
ooVArray(integer)  Slow;        // Slow data block
ooVArray(integer)  TBD;         // tbd

public:

// constructors
AMSraweventD() {};
AMSraweventD(uinteger runUni, uinteger runAux, uinteger eventNumber, 
             uinteger status, time_t time, integer ltrig, integer trig[], 
             integer ltracker, integer tracker[], integer lscint, 
             integer scint[], integer lanti, integer anti[],
             integer lctc, integer ctc[],  
             integer lslow, integer slow[]); 

// get/set methods

uinteger getrunAux()   const { return _runAux;}
uinteger getstatus()   const { return _status;}

void readEvent (uinteger& run, uinteger & runAux, 
                uinteger & eventNumber, uinteger & status, 
                time_t & time,
                integer & ltrig, integer* trig, 
                integer & lscint, integer* scint,
                integer & ltracker, integer* tracker,
                integer & lanti, integer* anti,
                integer & lctc, integer* ctc,  
                integer & lslow, integer* slow);

};
