// Sep 30, 1996. ak. AMSMCAntiClusterD Objectivity Class
// Mar  6, 1997. ak. up to date
//
// Last Edit : Apr 08, 1996. ak.
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

// Constructor

 AMSAntiMCClusterD() {};
 AMSAntiMCClusterD(AMSAntiMCCluster* p);

// methods
 void copy(AMSAntiMCCluster* p);
 inline integer  getid()   {return _idsoft;}
 inline AMSPoint getcoo()  {return _xcoo;}
 inline number   gettof()  {return _tof;}
 inline number   getedep() {return _edep;}

};
