// May 30, 1997. ak. AMSAntiClusterD Objectivity Class
//
// Last Edit : May 30, 1997. ak.
//

#include <typedefs.h>
#include <anticlusterD.h>

declare (ooVArray, AMSAntiClusterD);

class AMSAntiClusterV: public ooObj{
public:

 ooVArray(AMSAntiClusterD) _anticluster;

// Constructor

 AMSAntiClusterV() {};
 AMSAntiClusterV(integer size);

// methods
 ooStatus           add(integer n, AMSAntiClusterD anticluster);
 integer            getnelem() {return _anticluster.size();}
 AMSAntiClusterD    get(integer n);

};
