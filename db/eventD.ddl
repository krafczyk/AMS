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
// Oct, 16, 1996. ak. add delete(propagate)
//
// Last Edit: Oct 16, 1996. ak.

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

  //char       fID[256];          // unique ID
  ooVString    fID;

  time_t     _Timestamp;        // time of the trigger

  integer    _run;              // run number
  integer    _runtype;          // type of Run
  integer    fErrorCode;        // error code, if any
  uinteger   fEventNumber;      // event number
  uinteger   fTriggerMask;      // trigger mask

  integer    _Position;         // position in the container

// containers

 public:

//Assosiations
  
  ooRef(AMSTrRecHitD)      pTrRecHitS[]   : delete (propagate);
  ooRef(AMSTrClusterD)     pCluster[]     : copy (delete);
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
    uinteger EventNumber, integer run, integer runtype, time_t time, char* ID);

// Get Methods

  inline   integer&  RunNumber()        {return _run;}
  inline   integer&  RunType()          {return _runtype;}
  inline   uinteger& EventNumber()      {return fEventNumber;}
  inline   time_t&   TriggerTime()      {return _Timestamp;}
  inline   uinteger& Trigmask()         {return fTriggerMask;}
  inline   integer&  ErrorCode()        {return fErrorCode;}
  const    char*     GetID()            {return fID;}
  integer  getPosition() { return _Position;}
  void     getNumbers(integer* ibuff, uinteger* ubuff, time_t& time);

// Set Methods
 void setPosition(integer position) {_Position = position;}

//
  void Print();

};




