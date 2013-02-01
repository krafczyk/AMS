#include "Event.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT_ROOT(Event)
  DUMP(EventHeader)
  DUMP(Particles)
  DUMP(Trigger)
  DUMP(DAQ)
  DUMP(TOF)
  DUMP(ACC)
  DUMP(TRD)
  DUMP(Tracker)
  DUMP(RICH)
  DUMP(ECAL)
  DUMP(MC)
END_DEBUG_OUTPUT_ROOT

void Event::Clear() {

  fRunHeader = 0;
  fParticles.clear();
  fEventHeader.Clear();
  fTrigger.Clear();
  fDAQ.Clear();
  fTOF.Clear();
  fACC.Clear();
  fTRD.Clear();
  fTracker.Clear();
  fRICH.Clear();
  fECAL.Clear();
  fMC.Clear();
}

bool Event::CheckConsistency() const {

  // does track number associated with first particle make sense?
  if( fParticles.size() > 0 ){
    Short_t trackIndex = fParticles[0].TrackerTrackIndex();
    if (trackIndex<0 || trackIndex>=Short_t(fTracker.Tracks().size()))
      return false;
  }

  return true;
}

}

}
