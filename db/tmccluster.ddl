// class AMSTrMCCluster
// June 04, 1996. First try with Objectivity 
// Aug  21, 1996. Remove MCCluster <-> Event bidirectional link
//                change the place of members in the class
// Last Edit : Oct 10, 1996. ak
//
#include <typedefs.h>
//#include <trid.h>
#include <point.h>
#include <mccluster.h>

//class AMSEventD;

class AMSTrMCClusterD: public ooObj {

protected:

 AMSPoint  _xca;
 AMSPoint  _xcb;
 AMSPoint  _xgl;
 number    _sum;

 geant     _ss[2][5];

 integer   _idsoft;
 integer   _itra;
 integer   _left[2];
 integer   _center[2];
 integer   _right[2];

 integer   _Position;

public:

// Assosiations

// Constructor for noise &crosstalk
  AMSTrMCClusterD(){};
  AMSTrMCClusterD(AMSTrMCCluster* p);

// Get/Set Methods
 void copy(AMSTrMCCluster* p);
 void   setsum(number sum)        {_sum = sum;}
 number getsum()                  {return _sum;}
 integer getPosition()            {return _Position;}
 void    setPosition(integer pos) {_Position = pos;}
};
