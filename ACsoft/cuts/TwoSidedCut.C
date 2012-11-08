#include "TwoSidedCut.hh"

#include <sstream>

Cuts::TwoSidedCut::TwoSidedCut( std::string description, float minimum, float maximum ) :
  Cut(description),
  fLower(minimum),
  fUpper(maximum) {

}


std::string Cuts::TwoSidedCut::Description() const {

  std::stringstream s;
  s << fDescription << " [" << fLower << "," << fUpper << "]";

  return s.str();
}

ClassImp(Cuts::TwoSidedCut)
