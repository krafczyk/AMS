// Author V. Choutko 24-may-1996
//
// July 22, 1996. ak. AMSMCTOFClusterD Objectivity Class
// Aug  21, 1996. ak. remove TOFMCCluster <-> Event bidirectional link
//                    rearrange the position of members inside the class
// Mar   6, 1997. ak. up to date.
// 
// Last Edit : Oct 10, 1996. ak.
//

#include <typedefs.h>
#include <point.h>
#include <mccluster.h>

// MC bank for TOF & anti  - skeleton only

class AMSTOFMCClusterD: public ooObj{
public:

 AMSPoint  _xcoo;

 integer   _idsoft;

 number    _tof;
 number    _edep;

 integer   _Position;

// Associations

// Constructor

 AMSTOFMCClusterD() {};
 AMSTOFMCClusterD(AMSTOFMCCluster* p);

 void copy(AMSTOFMCCluster* p);

 inline integer getPosition()            {return _Position;}
 inline void    setPosition(integer pos) {_Position = pos;}

};
