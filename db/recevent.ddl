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
// May 29, 1997.  ak. add AntiClusters
// Jun 02, 1997.  ak. VArray of AntiClusters
//
// Last Edit: Jun 02, 1997. ak.
//

#include <typedefs.h>
#include <dbevent.h>
#include <particle.h>

class ParticleS {

 private :

  AMSPoint _Coo;

  number   _Mass;
  number   _ErrMass;
  number   _Momentum;
  number   _ErrMomentum;
  number   _Charge;
  number   _Theta;
  number   _Phi;

  integer  _GPart;        // Geant particle ID

 public :

  ParticleS() {};
  ParticleS(integer gpart, number mass, number errmass, number momentum, 
           number errmomentum, number charge, number theta, number phi, 
           AMSPoint coo): 
  _GPart(gpart),  _Mass(mass), _ErrMass(errmass), _Momentum(momentum), 
  _ErrMomentum(errmomentum), _Charge(charge), _Theta(theta), _Phi(phi), 
  _Coo(coo) {};
 void  setAll(integer gpart, number mass, number errmass, number momentum, 
              number errmomentum, number charge, number theta, number phi, 
              AMSPoint coo)
 {
  _GPart   = gpart;
  _Mass    = mass;
  _ErrMass = errmass;
  _Momentum    = momentum; 
  _ErrMomentum = errmomentum;
  _Charge  = charge;
  _Theta   = theta;
  _Phi     = phi; 
  _Coo     = coo;
 }
};

class AMSAntiClusterV;
#pragma ooclassref AMSAntiClusterV <anticlusterV_ref.h>

class AMSTOFClusterD;
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>

class AMSTrClusterD;
#pragma ooclassref AMSTrClusterD <tcluster_ref.h>

class AMSTrRecHitD;
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>

class AMSTrTrackD;
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>

class AMSCTCClusterD;
#pragma ooclassref AMSCTCClusterD <ctcrecD_ref.h>

class AMSBetaD;
#pragma ooclassref AMSBetaD <tbeta_ref.h>

class AMSChargeD;
#pragma ooclassref AMSChargeD <chargeD_ref.h>

class AMSParticleD;
#pragma ooclassref AMSParticleD <particleD_ref.h>


declare (ooVArray, ParticleS);

class AMSeventD : public dbEvent {

 private:
   
  ooVArray(ParticleS)  particleS;

 public:

//Assosiations
  ooRef(AMSAntiClusterV)   pAntiCluster   : delete (propagate);
  ooRef(AMSTOFClusterD)    pTOFCluster[]  : delete (propagate);
  ooRef(AMSTrClusterD)     pCluster[]     : delete (propagate);  
  ooRef(AMSTrRecHitD)      pTrRecHitS[]   : delete (propagate);
  ooRef(AMSCTCClusterD)    pCTCCluster[]  : delete (propagate);
  ooRef(AMSTrTrackD)       pTrack[]       : delete (propagate);
  ooRef(AMSBetaD)          pBeta[]        : delete (propagate);
  ooHandle(AMSChargeD)     pChargeE[]     <-> pEventCh : delete (propagate);
  ooHandle(AMSParticleD)   pParticleE[]   <-> pEventP  : delete (propagate);


//Constructors

  AMSeventD();
  AMSeventD(integer run, uinteger event, time_t time);

//
  void addParticle(int npart, ParticleS* particle);
  void readEvent(uinteger& run, uinteger& eventNumber, time_t& time);

};
