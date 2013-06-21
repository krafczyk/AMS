#ifndef RICH_h
#define RICH_h

#include "RICHRing.h"
#include "RICH-Streamer.h"

namespace ACsoft {

namespace AC {

/** Ring-Imaging Cherenkov %Detector event data
  */
class RICH {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of RICHRing objects */
  typedef Vector<RICHRing, 10> RingsVector;

  AC_RICH_Variables

  /** Helper method dumping an RICH object to the console
    */
  void Dump() const;

  /** Clear RICH to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Accessors for the RICHRing objects.
    */
  const RingsVector& Rings() const { return fRings; }

private:
  REGISTER_CLASS(RICH)
};

}

}

#endif
