#ifndef __AMSMCTOFClusterD__
#define __AMSMCTOFClusterD__
// Author V. Choutko 24-may-1996
//
// July 22, 1996. ak. AMSMCTOFClusterD Objectivity Class
// Aug  21, 1996. ak. remove TOFMCCluster <-> Event bidirectional link
//                    rearrange the position of members inside the class
// Mar   6, 1997. ak. up to date.
// Apr   1, 1997. ak. no persistence, put all AMSTOFMCClusterD objects in 
//                    VArray
// Last Edit : Apr 08, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

class AMSTOFMCClusterD {

public:

 AMSPoint  _xcoo;

 number    _tof;
 number    _edep;

 integer   _idsoft;

// Constructor

 AMSTOFMCClusterD() {};
 AMSTOFMCClusterD(AMSTOFMCCluster* p);

// methods
 void add(AMSTOFMCCluster* p);
 void copy(AMSTOFMCCluster* p);
 inline AMSPoint getxcoo() {return _xcoo;}
 inline number   gettof()  {return _tof;}
 inline number   getedep() {return _edep;}
 inline integer  getid()   {return _idsoft;}
};
#endif
