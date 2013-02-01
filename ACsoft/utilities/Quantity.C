
#include "Quantity.hh"

std::ostream& operator<<( std::ostream& out, const ACsoft::Utilities::Quantity& q ) {

  out << q.value << "+-" << q.uncertainty;
  return out;
}
