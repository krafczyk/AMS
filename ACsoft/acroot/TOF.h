#ifndef TOF_h
#define TOF_h

#include "TOFBeta.h"
#include "TOFCluster.h"

namespace AC {

/** Time-Of-Flight event data
  */
class TOF {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of TOFBeta objects */
  typedef Vector<TOFBeta, 1> BetasVector;

  /** A vector of TOFCluster objects */
  typedef Vector<TOFCluster, 34> ClustersVector;

  TOF() { }

  /** Helper method dumping an TOF object to the console
    */
  void Dump() const;

  /** Clear TOF to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

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
  BetasVector fBetas;
  ClustersVector fClusters;

  REGISTER_CLASS(TOF)
};

}

#endif
