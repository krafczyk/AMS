#ifndef RICH_h
#define RICH_h

#include "RICHRing.h"

namespace AC {

/** Ring-Imaging Cherenkov %Detector event data
  */
class RICH {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of RICHRing objects */
  typedef Vector<RICHRing, 4> RingsVector;

  RICH() { }

  /** Helper method dumping an RICH object to the console
    */
  void Dump() const;

  /** Clear RICH to the initial empty state, as it was just constructed.
    */
  void Clear(Option_t * /*option*/ ="");

  /** Accessors for the RICHRing objects.
    */
  const RingsVector& Rings() const { return fRings; }

private:
  RingsVector fRings;

  REGISTER_CLASS(RICH)
};

}

#endif
