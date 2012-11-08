#include "Event.h"

namespace AC {

BEGIN_DEBUG_OUTPUT_ROOT(Event)
  DUMP(EventHeader)
  DUMP(Particles)
  DUMP(Trigger)
  DUMP(TOF)
  DUMP(ACC)
  DUMP(TRD)
  DUMP(Tracker)
  DUMP(RICH)
  DUMP(ECAL)
  DUMP(MC)
END_DEBUG_OUTPUT_ROOT

Event::Event():
  fRunHeader(0)
{

}

void Event::Clear(Option_t *) {

  fRunHeader = 0;
  fParticles.clear();
  fEventHeader.Clear();
  fTrigger.Clear();
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

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const Event& object) {

   stream << object.fEventHeader << object.fParticles << object.fTrigger << object.fTOF << object.fACC
          << object.fTRD << object.fTracker << object.fRICH << object.fECAL << object.fMC;
   return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, Event& object) {

  object.fRunHeader = 0;
  stream >> object.fEventHeader >> object.fParticles >> object.fTrigger >> object.fTOF >> object.fACC
         >> object.fTRD >> object.fTracker >> object.fRICH >> object.fECAL >> object.fMC;
  return stream;
}

}
