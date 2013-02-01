#ifndef DETECTORMANAGER_HH
#define DETECTORMANAGER_HH

#include <TTimeStamp.h>

namespace ACsoft {

namespace Detector {

/** Different alignment parameters available. */
enum AlignmentMethod {
  OnlyShimming = 0,
  EffectiveModuleShifts = 1
};

class Trd;


/** Access to TRD geometries (singleton).
  *
  * This class offers access to the pre- and post-alignment %TRD geometry information.
  * Alignment information is refreshed after an interval that can be specified by a call
  * to SetRefreshInterval(). In order to trigger the refreshing of alignment parameters,
  * UpdateIfNeeded() has to be called. This is done, e.g. in the TrdHitFactory, so users will rarely
  * have to do this themselves.
  *
  * The alignment algorithm to be applied has to be chosen by a call to SetAlignmentMethod().
  *
  * \sa Detector::Trd
  */
class DetectorManager
{
public:

  static DetectorManager* Self() {

    static DetectorManager* gInstance = 0;
    if( !gInstance )
      gInstance = new DetectorManager;
    return gInstance;
  }

  void SetAlignmentMethod( AlignmentMethod m ) { fAlignmentMethod = m; }
  void SetRefreshInterval( double seconds ) { fRefreshInterval = seconds; }

  void UpdateIfNeeded( const TTimeStamp& time );

  Trd* GetAlignedTrd();
  Trd* GetUnalignedTrd();

private:

  DetectorManager();
  void UpdateFromEffectiveModuleShifts( const TTimeStamp& time );

private:

  Trd* fTrdNoAlignment; ///< %TRD geometry sructure without alignment
  Trd* fTrdAligned;     ///< %TRD geometry structure, aligned according to chosen set of parameters

  AlignmentMethod fAlignmentMethod; ///< which set of alignment parameters to use

  double fRefreshInterval; ///< interval (seconds) between updates of detector alignment

  TTimeStamp fLastUpdateTime; ///< time of last update of alignment

};
}

}

#endif // DETECTORMANAGER_HH
