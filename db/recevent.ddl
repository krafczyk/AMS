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
// Last Edit: Apr 08, 1997. ak.

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
  number   _SumAnti;

  integer  _GPart;        // Geant particle ID

 public :

  ParticleS() {};
  ParticleS(integer gpart, number mass, number errmass, number momentum, 
           number errmomentum, number charge, number theta, number phi, 
           AMSPoint coo, number sumanti): 
  _GPart(gpart),  _Mass(mass), _ErrMass(errmass), _Momentum(momentum), 
  _ErrMomentum(errmomentum), _Charge(charge), _Theta(theta), _Phi(phi), 
  _Coo(coo), _SumAnti(sumanti) {};
 void  setAll(integer gpart, number mass, number errmass, number momentum, 
              number errmomentum, number charge, number theta, number phi, 
              AMSPoint coo, number sumanti)
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
  _SumAnti = sumanti;
 }
};

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

  short      _nBetas;           // number of Betas
  short      _nCharges;         // number of Charges
  short      _nCTCClusters;     // number of CTCClusters
  short      _nParticles;       // number of Particles
  short      _nTOFClusters;     // number of TOFClusters
  short      _nTracks;          // number of Tracks
  short      _nTrClusters;      // number of TrClusters
  short      _nTrHits;          // number of TrRecHits


 public:

//Assosiations
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

// Methods

  void     clearCounts();

  inline void incTrHits() {_nTrHits++;}
  inline void decTrHits() {_nTrHits--;}
  inline short& TrHits()  {return _nTrHits;}

  inline void incTrClusters() {_nTrClusters++;}
  inline void decTrClusters() {_nTrClusters--;}
  inline short& TrClusters()  {return _nTrClusters;}

  inline void incTOFClusters() {_nTOFClusters++;}
  inline void decTOFClusters() {_nTOFClusters--;}
  inline short& TOFClusters()  {return _nTOFClusters;}

  inline void incCTCClusters() {_nCTCClusters++;}
  inline void decCTCClusters() {_nCTCClusters--;}
  inline short& CTCClusters()  {return _nCTCClusters;}

  inline void incTracks() {_nTracks++;}
  inline void decTracks() {_nTracks--;}
  inline short& Tracks()  {return _nTracks;}

  inline void incBetas()        {_nBetas++;}
  inline void decBetas()        {_nBetas--;}
  inline short& Betas()         {return _nBetas;}

  inline void incCharges()      {_nCharges++;}
  inline void decCharges()      {_nCharges--;}
  inline short& Charges()       {return _nCharges;}

  inline void incParticles()    {_nParticles++;}
  inline void decParticles()    {_nParticles--;}
  inline short& Particles()     {return _nParticles;}

//
  void addParticle(int npart, ParticleS* particle);
//
  void readEvent(uinteger& run, uinteger& eventNumber, time_t& time);

};
