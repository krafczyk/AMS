// class AMSMCEVENTG, Oct 02, 1996. ak.
//
// last edit : Mar 31, 1997
//
#include <typedefs.h>
#include <point.h>
#include <mceventg.h>
#include <cern.h>

class AMSmceventgD: public ooObj {

private:

AMSPoint _coo;
AMSDir   _dir;

number _mom;
number _mass;
number _charge;

integer _ipart;
integer _seed[2];
integer _nskip;

public:

// constructors
AMSmceventgD();
AMSmceventgD(AMSmceventg* p);
AMSmceventgD(AMSPoint coo, AMSDir dir, number mom, number mass, number charge, 
             integer ipart, integer seed0, integer seed1, integer nskip);
// get/set methods
void copy(AMSmceventg* p);
void    getAll(AMSPoint& coo, AMSDir& dir, number& mom, number& mass, 
               number& charge, integer& ipart, integer& seed0, integer& seed1,
               integer& nskip);
};
