// MC CTC class schema file, Sep. 30, 1996. ak.
//
// Last Edit : Oct 04, 1996. ak.
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
 number   _edep;    // energy deposition
 number   _beta;    // particle velocity

 integer  _idsoft;   // barnumber +(1-if agel;2-if wls)*100
 integer  _Position;

// Associations

// Constructor

 AMSCTCMCClusterD() {};
 AMSCTCMCClusterD(AMSCTCMCCluster* p);

 void  copy(AMSCTCMCCluster* p);

 inline integer getPosition()            {return _Position;}
 inline void    setPosition(integer pos) {_Position = pos;}

};
