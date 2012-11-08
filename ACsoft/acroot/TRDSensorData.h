#ifndef TRDSensorData_h
#define TRDSensorData_h

#include <TObject.h>

#ifndef __CINT__
#include <QtGlobal>
#endif

namespace AC {

/** Transition Radiation Detector sensor data
  */
class TRDSensorData : public TObject {
public:
  TRDSensorData() {

    for (unsigned int i = 0; i < 228; ++i)
      fTemperatures[i] = 0;
    for (unsigned int i = 0; i < 2; ++i)
      fPressures[i] = 0;
    for (unsigned int i = 0; i < 10; ++i)
      fDifferentialPressures[i] = 0;
  }

  /** Dallas temperature sensor data
    * Note: The range -15 degC .. +45 degC is mapped to a signed char in ac_producer.
    * \param sensor 0..227
    */
  Char_t TemperatureForSensor(unsigned int sensor) const {

    Q_ASSERT(sensor < 228);
    return fTemperatures[sensor];
  }

  /** Pressure data per box.
    * \param box 0: AMS TRDGAS-M BoxC sup (P3); 1: AMS TRDGAS-M BoxC ret (P4)
    */
  UShort_t PressureForBox(unsigned int box) const {

    Q_ASSERT(box < 2);
    return fPressures[box];
  }

  /** Differential pressure from the manifold DP10sensors in gas groups.
    * Note: 2*(MFdPin+out)[0..9]  not yet in AMI (14-AUG-2012)
    * \param group Manifold Gas-Group 1-5:W3; 6-10:W7
    */
  UShort_t DifferentialPressureForGasGroup(unsigned int group) const {

    Q_ASSERT(group > 0);
    Q_ASSERT(group <= 10);
    return fDifferentialPressures[group - 1];
  }

private:
  Char_t fTemperatures[228];
  UShort_t fPressures[2];
  UShort_t fDifferentialPressures[10];

  ClassDef(TRDSensorData, 1);
};

}

#endif
