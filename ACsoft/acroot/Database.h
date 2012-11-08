#ifndef Database_h
#define Database_h

#include "TrackerExternalAlignment.h"
#include "TRDCalibration.h"
#include "TRDSensorData.h"

namespace AC {

/** Global database which gives access to calibration / sensor / alignment data associated with the run.
  */
class Database : public TObject {
public:
  Database() { }

  /** Calibration data.
    */
  const AC::TRDCalibration& TRDCalibration() const { return fTRDCalibration; }

  /** Vector of different external tracker alignments.
   */
  const std::vector<AC::TrackerExternalAlignment>& TrackerExternalAlignments() const { return fTrackerExternalAlignments; }

  /** Vector of different TRD sensor data.
   */
  const std::vector<AC::TRDSensorData>& TRDSensorData() const { return fTRDSensorData; }

private:
  AC::TRDCalibration fTRDCalibration;                               // TRD-Hit-Calibration from DataBase
  std::vector<TrackerExternalAlignment> fTrackerExternalAlignments; // TRK External Plane Alignment 
  std::vector<AC::TRDSensorData> fTRDSensorData;                    // TRD-HK UG-DTS P3 P4 from AMI

  ClassDef(Database, 1)
};

}

#endif
