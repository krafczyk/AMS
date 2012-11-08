#ifndef ACC_h
#define ACC_h

#include "ACCCluster.h"

namespace AC {

/** Anti Coincidence Counter event data
  */
class ACC {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of ACCCluster objects */
  typedef Vector<ACCCluster, 8> ClustersVector;

  ACC() { }

  /** Helper method dumping an ACC object to the console
    */
  void Dump() const;

  /** Clear ACC to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

  /** Accessors for the ACCCluster objects.
    */
  const ClustersVector& Clusters() const { return fClusters; }

private:
  ClustersVector fClusters; // pAntiCluster

  REGISTER_CLASS(ACC)
};

}

#endif
