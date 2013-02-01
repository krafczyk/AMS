#include "ECAL.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(ECAL)
  DUMP(Showers)
END_DEBUG_OUTPUT

void ECAL::Clear() {

  fShowers.clear();
}

}

}
