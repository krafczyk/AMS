#include "ACCRawSide.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(ACCRawSide)
  COL( "SWID",         UChar_t,       SWID)
  COL( "Status",       UChar_t,       Status)
  COL( "Temp [deg]\t", Float_t,       Temperature)
  COL( "ADC\t",        Float_t,       ADC)
  NEWTABLE
  COL( "Hits\t\t\t\t",                  ACCHitVector,  Hits)
  COL( "Times\t\t\t\t\t\t\t\t\t\t\t\t", ACCTimeVector, Times)
END_DEBUG_TABLE

}

}
