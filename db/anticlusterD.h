#ifndef __AMSAntiClusterD__
#define __AMSAntiClusterD__
// May 30, 1997. ak. AMSAntiClusterD Objectivity Class
//                   non-persistent class
//
// Last Edit : Jun 02, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <antirec.h>

class AMSAntiClusterD {
public:

 AMSPoint  _coo;
 AMSPoint  _ecoo;

 number    _edep;
 integer   _sector;

 integer   _status;

// Constructor

 AMSAntiClusterD() {};
 AMSAntiClusterD(AMSAntiCluster* p);

// methods
 void add(AMSAntiCluster* p);
 void copy(AMSAntiCluster* p);
 inline AMSPoint getcoo()  {return _coo;}
 inline AMSPoint getecoo() {return _coo;}
 inline number   getedep() {return _edep;}
 inline integer  getsector() {return _sector;}
 inline integer  getstatus() {return _status;}
};
#endif
