// Author A. Klimentov  9-Apr-1997
//
// use event format from A.Lebedev (version Jan '97) to check overhead
// June 22, 1997. still have no event format use one Data block instead of 
//                block per subdetector.
//
// Last Edit: June 22, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>

#include <daqevt.h>

#include <dbevent_ref.h>
#include <dbevent.h>

#include <ooVArray_uint16.h>

class AMSraweventD : public dbEvent {

private:

uinteger _runAux;               // auxillary run number

ooVArray(uint16)  Data;         // DataBlock

public:

// constructors
AMSraweventD() {};
AMSraweventD(uinteger runUni, uinteger runAux, uinteger eventNumber, 
             time_t time, integer ldata, uint16 data[]);

AMSraweventD
       (uinteger runUni, uinteger eventNumber, time_t time, DAQEvent *pdaq);

// get/set methods

void    dump(integer sdetid); 
integer eventlength() {return Data.size();}
uinteger getrunAux()   const { return _runAux;}
void    readEvent (uinteger & run, uinteger & runAux, 
                   uinteger & eventNumber, time_t & time, 
                   integer & ldata, uint16* data);
integer sdetlength(integer sdetid);
integer sdet(integer sdetid);

};
