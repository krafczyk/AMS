#ifndef TRDHTrack_h
#define TRDHTrack_h

#include <math.h>
#include "Tools.h"
#include "TRDRawHit.h"
#include "TRDHTrack-Streamer.h"

namespace ACsoft {

namespace AC {

/** %TRD track data (h style)
  * Stores a 3D Track through the %TRD by Segments (X and Y Projections).
  * Original implementation in AMS Software by M. Millinger.
  *
  * \todo Recover x(z),y(z),theta, phi.
  */
class TRDHTrack {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of float numbers */
  typedef Vector<Float_t, 7> ChargesNewVector;

  /** A vector of unsigned char (8bit) numbers */
  typedef Vector<UChar_t, 2> HSegmentIndexVector;

  AC_TRDHTrack_Variables

  /** Helper method dumping an TRDHTrack object to the console
    */
  void Dump() const;
  
  /** TRDHTrack status information.
    * \todo Add documentation.
   */
  UShort_t Status() const { return fStatus; }

  /** Measured charges in layers.
    */
  const ChargesNewVector& ChargesNew() const { return fChargesNew; }

  /** Indices of associated TRDHSegments.
    */
  const HSegmentIndexVector& HSegmentIndex() const { return fHSegmentIndex; }

private:
  REGISTER_CLASS_WITH_TABLE(TRDHTrack)
};

}

}

#endif
