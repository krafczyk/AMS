// Author A.Klimentov  Apr 01, 1997.
// 
// VArray to store AMSTrMCClusterD objects.
//
// Last Edit : Apr  1, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <tmccluster.h>

declare (ooVArray, AMSTrMCClusterD);

class AMSTrMCClusterV: public ooObj{

public:

 ooVArray(AMSTrMCClusterD) _trmccluster;

// Constructor

 AMSTrMCClusterV() {};
 AMSTrMCClusterV(integer size);

// methods
ooStatus AMSTrMCClusterV::add(integer n, AMSTrMCClusterD trmccluster);
integer  getnelem() {return _trmccluster.size();}
AMSTrMCClusterD    get(integer n);

};
