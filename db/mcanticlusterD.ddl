// Sep 30, 1996. ak. AMSMCAntiClusterD Objectivity Class
//
// Last Edit : Oct 10, 1996. ak.
//

#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

class AMSAntiMCClusterD: public ooObj{
public:

 AMSPoint  _xcoo;

 integer   _idsoft;

 number    _tof;
 number    _edep;

 integer   _Position;

// Associations

// Constructor

 AMSAntiMCClusterD() {};
 AMSAntiMCClusterD(AMSAntiMCCluster* p);

 void copy(AMSAntiMCCluster* p);

 inline integer getPosition()            {return _Position;}
 inline void    setPosition(integer pos) {_Position = pos;}

};
