// class AMSTOFClusterD
// June 23rd, 1996. First Objectivity version
// July 10th, 1996. Add AMSBeta assosiation
// Aug  21st, 1996. Remove Event TOFCluster bidirectional link
//                  change the place of members in the class
// Mar 05th, 1997.  up to date version
//                  one directional link for beta/tof
//
// Last Edit: Mar 31, 1997 ak.
//
#include <typedefs.h>
#include <point.h>
#include <tofrec.h>


class AMSTOFClusterD: public ooObj {
protected:

 AMSPoint _Coo;      // coordinates
 AMSPoint _ErrorCoo; // Error to coordinates


 number _edep;     // reconstructed via anode (no angle correction)
 number _time;     // average cluster time
 number _etime;    // error on cluster time

 integer _status;
 integer _ntof;
 integer _plane;     // plane number (10.5 cm width)  left

 integer  _Position;

public:

static const integer BAD;
static const integer USED;
// Get/Set Methods
 integer checkstatus(integer checker) const{return _status & checker;}
 inline void setstatus(integer status)     {_status=_status | status;}
 integer getstatus()                 const {return _status;}
 integer getntof()                   const {return _ntof;}
 integer getplane()                  const {return _plane;}
 number  gettime()                   const {return _time;}
 number  getetime()                  const {return _etime;}
 number  getedep()                   const {return _edep;}
 AMSPoint getcoo()                   const {return _Coo;}
 AMSPoint getecoo()                  const {return _ErrorCoo;}

 integer  getPosition()             {return _Position;}
 void     setPosition(integer pos ) {_Position = pos;}
 void     copy(AMSTOFCluster* p);
// Constructor
 AMSTOFClusterD();
 AMSTOFClusterD(AMSTOFCluster* p);
};

