// Author A.Klimentov  Apr 01, 1997.
// 
// VArray to store AMSMCTOFClusterD objects.
//
// Last Edit : Apr  1, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <mctofclusterD.h>

declare (ooVArray, AMSTOFMCClusterD);

class AMSTOFMCClusterV: public ooObj{

public:

 ooVArray(AMSTOFMCClusterD) _tofmccluster;

// Constructor

 AMSTOFMCClusterV() {};
 AMSTOFMCClusterV(integer size);

// methods
ooStatus AMSTOFMCClusterV::add(integer n, AMSTOFMCClusterD tofmccluster);
integer  getnelem() {return _tofmccluster.size();}
AMSTOFMCClusterD    get(integer n);

};
