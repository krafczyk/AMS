// Author A. Klimentov  10-Apr-1997
//
//
// Last Edit: Nov 14, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <dbevent_ref.h>
#include <dbevent.h>


class AMSeventD;
#pragma ooclassref AMSeventD <receventD_ref.h>



class AMSEventTag : public dbEvent {

private:

uinteger _RunType;        

number   _NorthPolePhi;
number   _StationTheta;
number   _StationPhi;

public:

// Associations
ooRef(AMSeventD)    itsRecEvent : delete (propagate);


// constructors
AMSEventTag() {};
AMSEventTag(uinteger run, uinteger eventNumber);
AMSEventTag(uinteger run, uinteger RunType, uinteger eventNumber, 
            time_t time);


// get/set methods

uinteger runType()   const { return _RunType;}
uinteger run()             {return Run();}
void GetGeographicCoo(number & pole, number & theta, number &phi){
                      pole=_NorthPolePhi;theta=_StationTheta;phi=_StationPhi;}
void SetGeographicCoo(number pole, number theta, number phi){
                      _NorthPolePhi = pole;
                      _StationTheta = theta;
                     _StationPhi    = phi;}
};

