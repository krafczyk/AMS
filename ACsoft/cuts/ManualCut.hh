#ifndef MANUALCUT_HH
#define MANUALCUT_HH

#include <Cut.hh>

namespace Cuts {

/** A specific Cut class that's intended to be used without Selector.
 *  \sa Cut
 */
class ManualCut : public Cut {
public:
  ManualCut(std::string description);

  void LetPass( const Analysis::Particle& );
  void LetFail( const Analysis::Particle& );

  virtual std::string Description() const { return fDescription; }

private:
  // not implemented since the user has to tell this cut manually if an event passes
  virtual bool TestCondition( const Analysis::Particle& ) { return false; }
};

}
#endif // MANUALCUT_HH
