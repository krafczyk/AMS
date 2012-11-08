#ifndef MC_h
#define MC_h

#include "MCEventGenerator.h"
#include "MCTrack.h"
#include "MCTRDHit.h"

namespace AC {

/** Monte-Carlo event data
  */
class MC {
  WTF_MAKE_FAST_ALLOCATED;
public:
  // FIXME: Obtain reasonable inline capacities from MC files (ask Antje).

  /** A vector of MCEventGenerator objects */
  typedef Vector<MCEventGenerator, 1> EventGeneratorsVector;

  /** A vector of MCTrack objects */
  typedef Vector<MCTrack, 1> TracksVector;

  /** A vector of MCTRDHit objects */
  typedef Vector<MCTRDHit, 1> TRDHitsVector;

  MC() { }

  /** Helper method dumping an MC object to the console
    */
  void Dump() const;

  /** Clear MC to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

  /** Accessors for the MCEventGenerator objects.
    */
  const EventGeneratorsVector& EventGenerators() const { return fEventGenerators; }

  /** Accessors for the MCTrack objects.
    */
  const TracksVector& Tracks() const { return fTracks; }

  /** Accessors for the MCTRDHit objects.
    */
  const TRDHitsVector& TRDHits() const { return fTRDHits; }

private:
  EventGeneratorsVector fEventGenerators;
  TracksVector fTracks;
  TRDHitsVector fTRDHits;

  REGISTER_CLASS(MC)
};

}

#endif
