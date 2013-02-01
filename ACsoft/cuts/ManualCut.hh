#ifndef MANUALCUT_HH
#define MANUALCUT_HH

#include <Cut.hh>

namespace Cuts {

/** A generic %cut class that is intended to be used for arbitrary user-defined cuts.
  *
  * FIXME add docu, logic will be like
  * bool conditionFulfilled = manualCut.Examine( xy>0.5 );
  * if( manualCut.Passes(particle) ) do something, or use EventSelector instead after call to Examine function of all ManualCuts
  *
 *  \sa Cuts::Cut
 */
class ManualCut : public Cut {
public:
  explicit ManualCut(std::string description="");

  virtual bool Examine( bool condition );

  virtual std::string Description() const { return fDescription; }

private:
  virtual bool TestCondition( const ACsoft::Analysis::Particle& );

private:

  bool fConditionFulfilled;  ///< store result of Examine(), until TestCondition called as usual
  bool fConditionTested;     ///< make sure fConditionFulfilled is always up-to-date

  ClassDef(Cuts::ManualCut,1)
};

}
#endif // MANUALCUT_HH
