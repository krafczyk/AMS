// MC CTC class schema file, Sep. 30, 1996. ak.
//
// Last Edit : Apr 08, 1997. ak.
//


#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

// MC bank for CTC 

class AMSCTCMCClusterD: public ooObj{
public:

 AMSPoint _xcoo;    // Coordinates of input point
 AMSDir   _xdir;    // dir cos
 number   _step;    // step
 number   _charge;  // charge
 number   _edep;    // energy deposition
 number   _beta;    // particle velocity
 integer  _idsoft;   // barnumber +(1-if agel;2-if wls)*100
 number   _time;      // time


// Constructor

 AMSCTCMCClusterD() {};
 AMSCTCMCClusterD(AMSCTCMCCluster* p);

// methods
 void  copy(AMSCTCMCCluster* p);
 inline AMSPoint getxcoo() { return _xcoo;}
 inline AMSPoint getxdir() { return _xdir;}
 inline number   getstep() { return _step;}
 inline number   getcharge() { return _charge;}
 inline number   getedep() { return _edep;}
 inline number   getbeta() { return _beta;}
 inline integer  getid()   { return _idsoft;}
 inline number   gettime() { return _time;}
 inline integer  getlayno() const{ return CTCDBc::getgeom()<2?(_idsoft/1000)%10:
 _idsoft/1000000;}

};
