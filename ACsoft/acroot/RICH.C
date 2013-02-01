#include "RICH.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_OUTPUT(RICH)
  DUMP(Rings)
END_DEBUG_OUTPUT

void RICH::Clear() {

  fRings.clear();
}

}

}
