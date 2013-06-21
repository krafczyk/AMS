#ifndef ManualNMinusOneCut_HH
#define ManualNMinusOneCut_HH

#include <NMinusOneCut.hh>

namespace Cuts {

/** A generic %cut class that is intended to be used for arbitrary user-defined cuts.
  *
  * A ManualNMinusOneCut can be used to quickly define an arbitrary cut expression, without implementing a new %cut class but keeping
  * access to the usual cut statistics and n-1 logic.
  *
  */
class ManualNMinusOneCut : public NMinusOneCut {
public:
  explicit ManualNMinusOneCut(std::string description="");

  bool Examine(const ACsoft::Analysis::Particle& particle, float value, bool condition);

  virtual std::string Description() const { return fDescription; }

private:
  virtual bool TestCondition( const ACsoft::Analysis::Particle& );

private:
  virtual bool IsManualNMinusOneCut() const { return true; }

  bool fConditionFulfilled;  ///< store result of Examine(), until TestCondition called as usual
  bool fConditionTested;     ///< make sure fConditionFulfilled is always up-to-date

  ClassDef(Cuts::ManualNMinusOneCut,2)
};

}
#endif // ManualNMinusOneCut_HH
