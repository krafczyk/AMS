#ifndef TRDHSegment_h
#define TRDHSegment_h

#include "Tools.h"
#include "TRDHSegment-Streamer.h"

namespace ACsoft {

namespace AC {

/** %TRD segment data (h style)
  * A segment is one projection of the 3D H Track onto the XZ or YZ plane.
  * \todo Add documentation.
  */
class TRDHSegment {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of unsigned char (8bit) numbers */
  typedef Vector<UChar_t, 40> RawHitIndexVector;

  AC_TRDHSegment_Variables

  /** Helper method dumping an TRDHSegment object to the console
    */
  void Dump() const;

  /** Indices of associated TRDRawHits.
    */
  const RawHitIndexVector& RawHitIndex() const { return fRawHitIndex; }

private:
  REGISTER_CLASS_WITH_TABLE(TRDHSegment)
};

}

}

#endif
