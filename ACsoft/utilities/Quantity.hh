#ifndef QUANTITY_HH
#define QUANTITY_HH

#include <ostream>

namespace ACsoft {

namespace Utilities {

/** A simple structure for a physical value with an uncertainty associated to it.
  *
  */
struct Quantity {

public:

  Quantity( double Value = 0.0, double Uncertainty = 0.0 ) :
    value(Value),
    uncertainty(Uncertainty)
  { }

  /// value
  double value;
  /// associated uncertainty
  double uncertainty;

};

std::ostream& operator<<( std::ostream& out, const Quantity& q );

}

}

#endif // QUANTITY_HH
