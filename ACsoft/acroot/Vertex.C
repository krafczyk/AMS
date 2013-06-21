#include "Vertex.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(Vertex)
  COL( "X / cm\t", Float_t, X)
  COL( "Y / cm\t", Float_t, Y)
  COL( "Z / cm\t", Float_t, Z)
  COL( "Theta / rad", Float_t, Theta)
  COL( "Phi / rad", Float_t, Phi)
  COL( "Momentum / GeV", Float_t, Momentum)
  COL( "Galactic Longitude / deg", Float_t, GalacticLongitude)
  COL( "Galactic Latitude / deg", Float_t, GalacticLatitude)
  COL( "First Track Index", UChar_t, FirstTrackIndex)
  COL( "Second Track Index", UChar_t, SecondTrackIndex)
END_DEBUG_TABLE

}

}
