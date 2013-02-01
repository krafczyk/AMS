#ifndef Event_h
#define Event_h

#include "ACC.h"
#include "DAQ.h"
#include "ECAL.h"
#include "EventHeader.h"
#include "MC.h"
#include "Particle.h"
#include "RunHeader.h"
#include "RICH.h"
#include "TOF.h"
#include "TRD.h"
#include "Tracker.h"
#include "Trigger.h"
#include "Event-Streamer.h"

namespace ACsoft {

namespace IO {
  class FileManager;
}

/// ACROOT event classes.

namespace AC {

/** AMS-02 reduced event data
  */
class Event {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of Particle objects */
  typedef Vector<Particle, 1> ParticlesVector;

  AC_Event_Variables

  /** Helper method dumping an event object to the console
    */
  void Dump() const;

  /** Clear event to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Check internal consistency of the event.
    */
  bool CheckConsistency() const;

  /** Run header data.
    * Note: The data is not stored in the %event, but points to the shared RunHeader of the run containing this event.
    */
  AC::RunHeader* RunHeader() const { return fRunHeader; }

  /** Accessors for the %Particle objects.
    */
  const ParticlesVector& Particles() const { return fParticles; }

  /** %Event header data
    */
  const AC::EventHeader& EventHeader() const { return fEventHeader; }

  /** %Trigger data
    */
  const AC::Trigger& Trigger() const { return fTrigger; }

  /** %DAQ data
    */
  const AC::DAQ& DAQ() const { return fDAQ; }

  /** %TOF data
    */
  const AC::TOF& TOF() const { return fTOF; }

  /** %ACC data
    */
  const AC::ACC& ACC() const { return fACC; }

  /** %TRD data
    */
  const AC::TRD& TRD() const { return fTRD; }

  /** %Tracker data
    */
  const AC::Tracker& Tracker() const { return fTracker; }

  /** %RICH data
    */
  const AC::RICH& RICH() const { return fRICH; }

  /** %ECAL data
    */
  const AC::ECAL& ECAL() const { return fECAL; }

  /** %MC data
    */
  const AC::MC& MC() const { return fMC; }

private:
  friend class IO::FileManager;
  void SetRunHeader(AC::RunHeader* runHeader) { fRunHeader = runHeader; }

private:
  AC::RunHeader* fRunHeader; //! Not stored in ROOT/ACQt file - the FileManager assigns this variable.
  REGISTER_CLASS(Event)
};

}

}

#endif
