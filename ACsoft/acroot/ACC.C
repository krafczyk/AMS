#include "ACC.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(ACC)
  DUMP(Clusters)
  DUMP(RawSides)
END_DEBUG_OUTPUT

void ACC::Clear() {

  fClusters.clear();
  fRawSides.clear();
}

}

}
