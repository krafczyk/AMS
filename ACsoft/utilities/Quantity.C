
#include "Quantity.hh"

std::ostream& operator<<( std::ostream& out, const Quantity& q ) {

  out << q.value << "+-" << q.uncertainty;
  return out;
}
