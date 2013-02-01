#include "DAQ.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(DAQ)
  COL( "L3Errors\t\t",     ULong64_t,       L3Errors)
  COL( "JINJStatus\t\t\t", JINJStatusVector, JINJStatus)
  NEWTABLE
  COL( "SlaveStatus\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", SlaveStatusVector, SlaveStatus)
END_DEBUG_TABLE

void DAQ::Clear() {

  fL3Errors = 0;
  fJINJStatus.clear();
  fSlaveStatus.clear();
}

}

}
