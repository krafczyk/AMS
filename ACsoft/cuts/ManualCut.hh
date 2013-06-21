#ifndef MANUALCUT_HH
#define MANUALCUT_HH

#include <Cut.hh>

namespace Cuts {

/** A generic %cut class that is intended to be used for arbitrary user-defined cuts.
  *
  * A ManualCut can be used to quickly define an arbitrary cut expression, without implementing a new %cut class but keeping
  * access to the usual cut statistics.
  *
  * When creating a ManualCut, you have to give a description of what the cut does. The %cut can then be registered in an EventSelector
  * as usual:
  * \code
  *  Cuts::Selector chargeselection("Z=1 selection");
  *  chargeselection.RegisterCut(new Cuts::CutTrackerCharge(0.,1.5));
  *  chargeselection.RegisterCut(new Cuts::CutTofCharge(0.,1.8));
  *  Cuts::ManualCut* lowTrdLogLCut = new Cuts::ManualCut("Low TRD -log(L) value");
  *  chargeselection.RegisterCut(lowTrdLogLCut);
  * \endcode
  *
  * In the event loop, you have to call the Examine function of the ManualCut \b before the call to the Passes() function of the EventSelector.
  * \code
  *  lowTrdLogLCut->Examine(minusLogL_Elec_Prot>0.15);
  *
  *  (...)
  *  if(!chargeselection.Passes(particle))
  *    continue;
  * \endcode
  *
  * If you do not use an EventSelector, you can also simply call the Passes() function of the ManualCut.
  *
  *  \sa Cuts::Cut
  */
class ManualCut : public Cut {
public:
  explicit ManualCut(std::string description="");
  virtual bool IsNMinusOneCut() const { return false; }

  bool Examine( bool condition );

  virtual std::string Description() const { return fDescription; }

private:
  virtual bool TestCondition( const ACsoft::Analysis::Particle& );

private:

  bool fConditionFulfilled;  ///< store result of Examine(), until TestCondition called as usual
  bool fConditionTested;     ///< make sure fConditionFulfilled is always up-to-date

  ClassDef(Cuts::ManualCut,2)
};

}
#endif // MANUALCUT_HH
