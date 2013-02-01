#include "MC.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(MC)
  DUMP(EventGenerators)
  DUMP(Tracks)
  DUMP(TRDHits)
END_DEBUG_OUTPUT

void MC::Clear() {

  fEventGenerators.clear();
  fTracks.clear();
  fTRDHits.clear();
}

}

}
