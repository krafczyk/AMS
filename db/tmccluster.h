//  $Id: tmccluster.h,v 1.4 2001/01/22 17:32:30 choutko Exp $
#ifndef __AMSTrMCClusterD__
#define __AMSTrMCClusterD__
// class AMSTrMCCluster
// June 04, 1996. First try with Objectivity 
// Aug  21, 1996. Remove MCCluster <-> Event bidirectional link
//                change the place of members in the class
// Mar  06, 1997. up to date
// Apr  01, 1997. non persisten class
//
// Last Edit : Apr 9, 1997. ak
//

#include <typedefs.h>
#include <point.h>
#include <mccluster.h>


class AMSTrMCClusterD {

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

public:

// Constructor for noise &crosstalk
  AMSTrMCClusterD(){};
  AMSTrMCClusterD(AMSTrMCCluster* p);

// Get/Set Methods
 void   add(AMSTrMCCluster* p);
 void   copy(AMSTrMCCluster* p);

 inline integer  getid()   {return _idsoft;}
 inline AMSPoint getxca()  {return _xca;}
 inline AMSPoint getxcb()  {return _xcb;}
 inline AMSPoint getxgl()  {return _xgl;}
 inline number   getsum()  {return _sum;}
 inline integer  getitra() {return _itra;}

};

#endif
