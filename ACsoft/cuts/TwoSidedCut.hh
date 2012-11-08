#ifndef TWOSIDEDCUT_HH
#define TWOSIDEDCUT_HH

#include "Cut.hh"

namespace Cuts {

/** %Cut that is of the from lower cut \<= event value \<= upper cut.
  *
  */
class TwoSidedCut : public Cut {
public:
  TwoSidedCut( std::string description, float minimum, float maximum);

  virtual std::string Description() const;

protected:
  virtual bool ValueIsInRange(float value) const { return (value >= fLower) && (value <= fUpper); }

protected:
  /// cut range for value: fLower<=value<=fUpper
  float fLower;
  /// cut range for value: fLower<=value<=fUpper
  float fUpper;

  ClassDef(Cuts::TwoSidedCut,1)
};

}

#endif // TWOSIDEDCUT_HH
