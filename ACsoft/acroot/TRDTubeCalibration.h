#ifndef TRDTubeCalibration_h
#define TRDTubeCalibration_h

#include <TObject.h>

#ifndef __CINT__
#include <QtGlobal>
#endif

namespace AC {

/** Transition Radiation Detector tube calibration data
  */
class TRDTubeCalibration : public TObject {
public:
  TRDTubeCalibration()
    : fType(0) {

    for (unsigned int i = 0; i < 5248; ++i) {
      fPedestals[i] = 0;
      fSigmas[i] = 0;
    }
  }

  /** Calibration type.
    * 0:ped/sig 1:MM-GainCor/0 2:AK-GainCor/Slope
    */
  UChar_t Type() const { return fType; }

  /** Returns pedestal for tube.
    */
  Float_t PedestalForTube(unsigned int tube) const {

    Q_ASSERT(tube < 5248);
    return fPedestals[tube];
  }

  /** Returns sigma for tube.
    */
  Float_t SigmaForTube(unsigned int tube) const {

    Q_ASSERT(tube < 5248);
    return fSigmas[tube];
  }

private:
  UChar_t fType;
  Float_t fPedestals[5248];
  Float_t fSigmas[5248];

  ClassDef(TRDTubeCalibration, 1);
};

}

#endif
