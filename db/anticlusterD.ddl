// May 30, 1997. ak. AMSAntiClusterD Objectivity Class
//
// Last Edit : May 30, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <antirec.h>

class AMSAntiClusterD: public ooObj{
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
 void copy(AMSAntiCluster* p);
 inline AMSPoint getcoo()  {return _coo;}
 inline AMSPoint getecoo() {return _coo;}
 inline number   getedep() {return _edep;}
 inline integer  getsector() {return _sector;}
 inline integer  getstatus() {return _status;}

};
