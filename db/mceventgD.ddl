// class AMSMCEVENTG, Oct 02, 1996. ak.
//
// last edit : Oct 02, 1996
//
#include <typedefs.h>
#include <point.h>
#include <mceventg.h>
#include <cern.h>

class AMSmceventgD: public ooObj {

private:

AMSPoint _coo;
AMSDir _dir;

number _mom;
number _mass;
number _charge;

integer _ipart;
integer _seed[2];
integer _Position;

public:

// constructors
AMSmceventgD();
AMSmceventgD(AMSmceventg* p);

// get/set methods
void copy(AMSmceventg* p);
integer getPosition()    {return _Position;}
void    setPosition(integer pos) {_Position = pos;}
};
