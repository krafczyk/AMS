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
//
// Last Edit: Mar 12, 1997. ak.

#include <typedefs.h>

class AMSTrRecHitD;
#pragma ooclassref AMSTrRecHitD <trrechit_ref.h>

class AMSTrClusterD;
#pragma ooclassref AMSTrClusterD <tcluster_ref.h>

class AMSTrTrackD;
#pragma ooclassref AMSTrTrackD <ttrack_ref.h>

class AMSTrMCClusterD;
#pragma ooclassref AMSTrMCClusterD <tmccluster_ref.h>

class AMSTOFClusterD;
#pragma ooclassref AMSTOFClusterD <tofrecD_ref.h>

class AMSTOFMCClusterD;
#pragma ooclassref AMSTOFMCClusterD <mctofclusterD_ref.h>

class AMSCTCClusterD;
#pragma ooclassref AMSCTCClusterD <ctcrecD_ref.h>

class AMSAntiMCClusterD;
#pragma ooclassref AMSAntiMCClusterD <mcanticlusterD_ref.h>

class AMSCTCMCClusterD;
#pragma ooclassref AMSCTCMCClusterD <mcctcclusterD_ref.h>

class AMSBetaD;
#pragma ooclassref AMSBetaD <tbeta_ref.h>

class AMSChargeD;
#pragma ooclassref AMSChargeD <chargeD_ref.h>

class AMSParticleD;
#pragma ooclassref AMSParticleD <particleD_ref.h>

class AMSmceventgD;
#pragma ooclassref AMSmceventgD <mceventgD_ref.h>

class AMSEventD : public ooObj {

 private:

//  ooVString    fID;
   
  time_t     _time;             // time of the trigger
  integer    _runNumber;        // run number
  short      _runType;          // type of Run
  uinteger   _eventNumber;      // event number
  uinteger   _triggerMask;      // trigger mask

  short      _nTrHits;          // number of TrRecHits
  short      _nTrClusters;      // number of TrClusters
  short      _nTOFClusters;     // number of TOFClusters
  short      _nCTCClusters;     // number of CTCClusters
  short      _nTracks;          // number of Tracks
  short      _nBetas;           // number of Betas
  short      _nCharges;         // number of Charges
  short      _nParticles;       // number of Particles


  integer    _Position;         // position in the container

// containers

 public:

//Assosiations
  
  ooRef(AMSTrClusterD)     pCluster[]       : copy (delete);
  //ooRef(AMSTrClusterD)     pCluster[]     : delete (propagate);  
  ooRef(AMSTrRecHitD)      pTrRecHitS[]     : delete (propagate);
  ooRef(AMSTOFClusterD)    pTOFCluster[]    : delete (propagate);
  ooRef(AMSCTCClusterD)    pCTCCluster[]    : delete (propagate);
  ooRef(AMSTrMCClusterD)   pMCCluster[]     : delete (propagate);
  ooRef(AMSTOFMCClusterD)  pTOFMCCluster[]  : delete (propagate);
  ooRef(AMSCTCMCClusterD)  pCTCMCCluster[]  : delete (propagate);
  ooRef(AMSAntiMCClusterD) pAntiMCCluster[] : delete (propagate);
  ooRef(AMSmceventgD)      pmcEventg[]      : delete (propagate);
  ooRef(AMSTrTrackD)       pTrack[]       <-> pEventT  : delete (propagate);
  ooRef(AMSBetaD)          pBeta[]        <-> pEventB  : delete (propagate);
  ooHandle(AMSChargeD)     pChargeE[]     <-> pEventCh : delete (propagate);
  ooHandle(AMSParticleD)   pParticleE[]   <-> pEventP  : delete (propagate);

//Constructors

  AMSEventD();
  AMSEventD(
    integer runnumber, uinteger eventnumber, integer runtype, time_t time);

// Get Methods

  inline   integer&  getrun()           {return _runNumber;}
  inline   short&    RunType()          {return _runType;}
  inline   uinteger& getevent()         {return _eventNumber;}
  inline   time_t&   TriggerTime()      {return _time;}
  inline   uinteger& Trigmask()         {return _triggerMask;}
//inline   short&    ErrorCode()        {return _errorCode;}
  integer  getPosition()                { return _Position;}
  void     getNumbers
             (integer& run, integer& runtype, uinteger& eventn, time_t& time);

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


// Set Methods
  void setPosition(integer position)    {_Position = position;}

//
  void Print();

};




