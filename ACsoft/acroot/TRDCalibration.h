#ifndef TRDCalibration_h
#define TRDCalibration_h

#include "TRDTubeCalibration.h"
#include "TRDModuleCalibration.h"

namespace AC {

/** Transition Radiation Detector calibration data
  */
class TRDCalibration : public TObject {
public:
  TRDCalibration() { }

  /** Time of entry.
    */
  const TTimeStamp& TimeStamp() const { return fTimeStamp; }

  /** Vector of different tube calibrations.
    */
  const std::vector<AC::TRDTubeCalibration>& TubeCalibrations() const { return fTubeCalibrations; }

  /** Vector of different module calibrations.
    */
  const std::vector<AC::TRDModuleCalibration>& ModuleCalibrations() const { return fModuleCalibrations; }

private:
  TTimeStamp fTimeStamp;                                 // Unix-time of entry
  std::vector<TRDTubeCalibration> fTubeCalibrations;     // indiv.tube calibrations ped;sig;gain[MM,AK,SS] 
  std::vector<TRDModuleCalibration> fModuleCalibrations; // module calibrations new gain-cal ?

  ClassDef(TRDCalibration, 2);
};

}

#endif
