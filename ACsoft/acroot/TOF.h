#ifndef TOF_h
#define TOF_h

#include "TOFBeta.h"
#include "TOFCluster.h"
#include "TOF-Streamer.h"

namespace ACsoft {

namespace AC {

/** Time-Of-Flight event data
  */
class TOF {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of TOFBeta objects */
  typedef Vector<TOFBeta, 9> BetasVector;

  /** A vector of TOFCluster objects */
  typedef Vector<TOFCluster, 34> ClustersVector;

  AC_TOF_Variables

  /** Helper method dumping an TOF object to the console
    */
  void Dump() const;

  /** Clear TOF to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Calculate mean energy deposited in TOF
    */
  Float_t CalculateMeanEnergy(const TOFBeta&) const;

  /** Calculate maximum energy deposited in TOF
    */
  Float_t CalculateMaximumEnergy(const TOFBeta&) const;

  /** Accessors for the TOFBeta objects.
    */
  const BetasVector& Betas() const { return fBetas; }

  /** Accessors for the TOFCluster objects.
    */
  const ClustersVector& Clusters() const { return fClusters; }

private:
  REGISTER_CLASS(TOF)
};

}

}

#endif
