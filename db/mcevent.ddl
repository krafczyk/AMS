// class event header
// May  06, 1996. First try with Objectivity 
//
// May,     1996. ak. Modify according to V106
//                    Add AMSTrTrackD class
//                    Remove Scint and Level1
// June,    1996. ak. Event <-> MCCluster, Event <-> TOFCluster
// July,    1996. ak. Event <-> Beta, Event <-> Charge, Event <-> Particle
//                    add _Position;
//                    add #pragma
// Aug, 21, 1996. ak. replace bidirectional assoc. by unidirectional
//                    rearrange the position of memeber inside of the class
// Sep, 24, 1996. ak. set one to many assoc. between Event and Charge,
//                    Event and Particle
//                    Add AMSCTCClusterD definitions and ref.
// Oct,   , 1996. ak. add delete(propagate)
//                    keep number of tracks, clusters, hits per event
//                    use short instead of integer for runtype and errorcode
// Feb 27, 1997.  ak. remove fID
// Apr 01, 1997.  ak. VArray to store all TOFMCCluster(s)
//                    VArray to store all TrMCCluster(s)
//                    unidirectional link between track and event
//                    unidirectional link between beta and event
//
// Last Edit: Jun 04, 1997. ak.

#include <typedefs.h>
#include <dbevent.h>

class AMSAntiMCClusterD;
#pragma ooclassref AMSAntiMCClusterD <mcanticlusterD_ref.h>

class AMSCTCMCClusterD;
#pragma ooclassref AMSCTCMCClusterD <mcctcclusterD_ref.h>

class AMSmceventgD;
#pragma ooclassref AMSmceventgD <mceventgD_ref.h>

class AMSTOFMCClusterV;
#pragma ooclassref AMSTOFMCClusterV <mctofclusterV_ref.h>


class AMSTrMCClusterV;
#pragma ooclassref AMSTrMCClusterV <tmcclusterV_ref.h>


class AMSmcevent : public dbEvent {

 private:

  integer _runtype;

 public:

//Assosiations
  
  ooRef(AMSAntiMCClusterD) pAntiMCCluster[] : delete (propagate);
  ooRef(AMSCTCMCClusterD)  pCTCMCCluster[]  : delete (propagate);
  ooRef(AMSmceventgD)      pmcEventg        : delete (propagate);
  ooRef(AMSTOFMCClusterV)  pTOFMCCluster    : delete (propagate);
  ooRef(AMSTrMCClusterV)   pMCCluster       : delete (propagate);

//Constructors

  AMSmcevent();
  AMSmcevent(integer run, uinteger event, time_t time, integer runtype);

// Get Methods
  void 
    readEvent(uinteger& run, uinteger& event, time_t& time, integer& runtype);

};




