#ifndef DETECTORMANAGER_HH
#define DETECTORMANAGER_HH

#include <TTimeStamp.h>
#include <RunHeader.h>

namespace ACsoft {

namespace Detector {

/** Different alignment parameters available. */
enum AlignmentMethod {
  None = 0,
  OnlyShimming = 1,
  EffectiveModuleShifts = 2
};

class Trd;


/** Access to TRD geometries (singleton).
  *
  * This class offers access to the pre- and post-alignment %TRD geometry information.
  * Alignment information is refreshed after an interval that can be specified by a call
  * to SetRefreshInterval(). In order to trigger the refreshing of alignment parameters,
  * UpdateIfNeeded() has to be called. This is done globally in the FileManager. This also
  * means that the DetectorManager serves as a global clock and you can access the current
  * timestamp by the CurrentTime() function.
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

  const TTimeStamp& CurrentTime() const { return fCurrentTime; }

  /** Choose alignment method to be used for the aligned %TRD structure. To be called before main event loop. */
  void SetAlignmentMethod( AlignmentMethod m ) { fAlignmentMethod = m; }
  void SetUpdateGain( bool val ) { fUpdateGain = val; }
  void SetRefreshInterval( double seconds ) { fRefreshInterval = seconds; }

  void UpdateIfNeeded( const TTimeStamp& time, AC::RunTypeIdentifier runtype );

  Trd* GetAlignedTrd();   ///< Get %TRD geometry structure, aligned according to chosen set of parameters (see DetectorManager::SetAlignmentMethod())
  Trd* GetShimmedTrd();   ///< Get %TRD geometry structure with only global shimming corrections applied
  Trd* GetUnalignedTrd(); ///< Get %TRD geometry structure without any alignment

  bool IsCompleteTrdCalibrationAvailable() const;
  bool IsCompleteTrdCalibrationAvailable( double time ) const;
  bool IsCompleteTrdCalibrationAvailable( const TTimeStamp& time ) const;

  bool WasCompleteTrdCalibrationAvailableForLastUpdate() const { return fAlignmentOkForLastUpdate && fGainOkForLastUpdate; }

private:

  DetectorManager();
  void Update( const TTimeStamp& time, AC::RunTypeIdentifier runtype );
  void UpdateFromEffectiveModuleShifts( const TTimeStamp& time );
  void UpdateGainValues( const TTimeStamp& time, AC::RunTypeIdentifier runtype );

private:

  Trd* fTrdNoAlignment;  ///< %TRD geometry structure without any alignment
  Trd* fTrdOnlyShimming; ///< %TRD geometry structure with only global shimming corrections applied
  Trd* fTrdAligned;      ///< %TRD geometry structure, aligned according to chosen set of parameters

  AlignmentMethod fAlignmentMethod; ///< which set of alignment parameters to use

  bool fUpdateGain; ///< for certain jobs (e.g. gain calibration!) we do not want to update gain calibration constants

  double fRefreshInterval; ///< interval (seconds) between updates of detector alignment

  TTimeStamp fCurrentTime;    ///< timestamp of previous call to UpdateIfNeeded()
  TTimeStamp fLastUpdateTime; ///< time of last update of alignment
  AC::RunTypeIdentifier fCurrentRunType; ///< run type (ISS/BT/MC) for which the current detector information is valid
  AC::RunTypeIdentifier fLastRunType; ///< run type for which detector info was last updated
  bool fEnableUpdateWarnings; ///< internal switch for suppressing false-positive warnings

  bool fAlignmentOkForLastUpdate; ///< was alignment available for all modules in last update?
  bool fGainOkForLastUpdate; ///< was gain calibration available for all modules in last update?

};
}

}

#endif // DETECTORMANAGER_HH
