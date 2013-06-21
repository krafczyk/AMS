#include "TwoSidedCut.hh"

#include <sstream>

Cuts::TwoSidedCut::TwoSidedCut( std::string description, float minimum, float maximum ) :
  NMinusOneCut(description),
  fLower(minimum),
  fUpper(maximum) {

}

double Cuts::TwoSidedCut::Lower() const {
  return fLower;
}

double Cuts::TwoSidedCut::Upper() const {
  return fUpper;
}

std::string Cuts::TwoSidedCut::Description() const {

  std::stringstream s;
  s << fDescription << " [" << fLower << "," << fUpper << "]";

  return s.str();
}

ClassImp(Cuts::TwoSidedCut)
