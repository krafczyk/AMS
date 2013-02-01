#ifndef ACC_h
#define ACC_h

#include "ACCCluster.h"
#include "ACCRawSide.h"
#include "ACC-Streamer.h"

namespace ACsoft {

namespace AC {

/** Anti Coincidence Counter event data
  */
class ACC {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of ACCCluster objects */
  typedef Vector<ACCCluster, 8> ClustersVector;

  /** A vector of ACCRawSide objects */
  typedef Vector<ACCRawSide, 16> RawSideVector;

  AC_ACC_Variables

  /** Helper method dumping an ACC object to the console
    */
  void Dump() const;

  /** Clear ACC to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Accessors for the ACCCluster objects.
    */
  const ClustersVector& Clusters() const { return fClusters; }

  /** Accessors for the ACCRawSide objects.
    */
  const RawSideVector& RawSides() const { return fRawSides; }

private:
  REGISTER_CLASS(ACC)
};

}

}

#endif
