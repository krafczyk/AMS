// class AMSTOFClusterD
// June 23rd, 1996. First Objectivity version
// July 10th, 1996. Add AMSBeta assosiation
// Aug  21st, 1996. Remove Event TOFCluster bidirectional link
//                  change the place of members in the class
//
// Last Edit: Oct 10, 1996 ak.
//
#include <typedefs.h>
#include <point.h>
#include <tofrec.h>

class AMSBetaD;

class AMSTOFClusterD: public ooObj {
protected:

 AMSPoint _Coo;      // coordinates
 AMSPoint _ErrorCoo; // Error to coordinates

 number  _edep;      // depositions in MeV
 number  _time; 
 number  _etime; 

 integer _status;
 integer _ntof;
 integer _plane;     // plane number (10.5 cm width)  left

 integer  _Position;

public:
// Associations
 ooRef(AMSBetaD)  pBetaTOF  <-> pTOFBeta[];

static const integer BAD;
static const integer USED;
// Get/Set Methods
 integer checkstatus(integer checker) const{return _status & checker;}
 inline void setstatus(integer status)     {_status=_status | status;}
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

