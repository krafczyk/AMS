#ifndef TWOSIDEDCUT_HH
#define TWOSIDEDCUT_HH

#include "NMinusOneCut.hh"

namespace Cuts {

/** %Cut that is of the from lower cut \<= event value \<= upper cut.
  *
  */
class TwoSidedCut : public NMinusOneCut {
public:
  TwoSidedCut( std::string description, float minimum, float maximum);

  double Lower() const;
  double Upper() const;

  virtual std::string Description() const;

protected:
  bool ValueIsInRange(const ACsoft::Analysis::Particle& particle, float value) {

    fLastCutValue = value;
    fLastRigidityOrEnergy = NMinusOneValue(particle, fNMinusOneMode);
    return value >= fLower && value <= fUpper;
  }

protected:
  /// cut range for value: fLower<=value<=fUpper
  float fLower;
  /// cut range for value: fLower<=value<=fUpper
  float fUpper;

  ClassDef(Cuts::TwoSidedCut,3)
};

}

#endif // TWOSIDEDCUT_HH
