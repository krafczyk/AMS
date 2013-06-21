#ifndef MCTrackerCluster_h
#define MCTrackerCluster_h

#include "Tools.h"
#include "MCTrackerCluster-Streamer.h"

namespace ACsoft {

namespace AC {

/** Monte-Carlo %Tracker cluster data
  */
class MCTrackerCluster {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_MCTrackerCluster_Variables

  /** Helper method dumping an MCTrackerCluster object to the console
    */
  void Dump() const;

  /** Particle Identifier a la Geant3. Negative if secondary particle
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    */
  Short_t ID() const { return fID; }

  /** Cluster deposited energy [MeV].
    */
  Float_t DepositedEnergy() const { return fDepositedEnergy; }

  /** Momentum of MC particle [GeV].
    */
  Float_t MCParticleMomentum() const { return fMCParticleMomentum; }

  /** X Coordinate of middle point of the step (cm). 
    */
  Float_t X() const { return fX; }

  /** Y Coordinate of middle point of the step (cm). 
    */
  Float_t Y() const { return fY; }

  /** Z Coordinate of middle point of the step (cm). 
    */
  Float_t Z() const { return fZ; }

private:
  REGISTER_CLASS_WITH_TABLE(MCTrackerCluster)
};

}

}

#endif
