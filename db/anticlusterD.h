//  $Id: anticlusterD.h,v 1.4 2001/01/22 17:32:27 choutko Exp $
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
 number    _signal;

// Constructor

 AMSAntiClusterD() { _edep=0.0; _sector=0; _status=0; _signal=0.0;};
 AMSAntiClusterD(AMSAntiCluster* p);
 ~AMSAntiClusterD(){};

// methods
// void add(AMSAntiCluster* p,AMSAntiRawCluster* pR);
 void add(AMSAntiCluster* p);
 void copy(AMSAntiCluster* p);
 inline AMSPoint getcoo()  {return _coo;}
 inline AMSPoint getecoo() {return _coo;}
 inline number   getedep() {return _edep;}
 inline integer  getsector() {return _sector;}
 inline integer  getstatus() {return _status;}
 inline number   getsignal() {return _signal;}
};
#endif
