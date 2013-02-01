#ifndef ACCRawSide_h
#define ACCRawSide_h

#include "Tools.h"
#include "ACCRawSide-Streamer.h"

namespace ACsoft {

namespace AC {

/** Anti Coincidence Counter raw side event data
  */
class ACCRawSide {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of float numbers */
  typedef Vector<Float_t, 8> ACCHitVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 16> ACCTimeVector;

  AC_ACCRawSide_Variables

  /** Helper method dumping an ACCCluster object to the console
    */
  void Dump() const;
 
  // FIXME: Remove this accessor, add Bar/Side accessors.
  /** BBS (Bar/Side).
   * \todo Add documentation. SWID? BBS? Please rename this.
   */
  UChar_t SWID() const { return fHWAddress; }

  /** Returns ACC Raw side status information.
    * See http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup for details.
    * \li \c Bit \c 0: alive;
    * \li \c Bit \c 1: dead;
    * \todo Verify documentation.
    */
  UChar_t Status() const { return fStatus; }

  /** Board temperature [deg].
    */ 
  Float_t Temperature() const { return fTemperature; }

  /** Anode pulse-charge hit (ped-subtracted) [ADC counts].
    */
  Float_t ADC() const { return fADC; }

  /** Accessor for the FTDC (time-to-digital-converter) hits.
    */
  const ACCHitVector& Hits() const { return fFTDCHits; }

  /** Accessor for the TDC times.
    */
  const ACCTimeVector& Times() const { return fTDCTimes; }

private:
  REGISTER_CLASS_WITH_TABLE(ACCRawSide)
};

}

}

#endif
