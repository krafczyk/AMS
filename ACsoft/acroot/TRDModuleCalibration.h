#ifndef TRDModuleCalibration_h
#define TRDModuleCalibration_h

#include <TObject.h>

#ifndef __CINT__
#include <QtGlobal>
#endif

namespace AC {

/** Transition Radiation Detector module calibration data
  */
class TRDModuleCalibration : public TObject {
public:
  TRDModuleCalibration()
    : fType(0) {

    for (unsigned int i = 0; i < 328; ++i) {
      for (unsigned int j = 0; j < 3; ++j) {
        fGains[i][j] = 0;
        fSlopes[i][j] = 0;
      }
    }
  }

  /** Calibration type.
    * 0:SS-GainCor/Slope
    */
  UChar_t Type() const { return fType; }

  /** Returns gain for module and section.
    * \param module 0 .. 327
     *\param section 0 .. 2 (0: negative, 1: center, 2: positive section)
     * Note: At the moment only section=1 data is available, the rest is 0.
    */
  Float_t GainForModule(unsigned int module, unsigned int section) const {

    Q_ASSERT(module < 328);
    Q_ASSERT(section < 3);
    return fGains[module][section];
  }

  /** Returns slope for module and section.
    * \param module 0 .. 327
     *\param section 0 .. 2 (0: negative, 1: center, 2: positive section)
     * Note: At the moment only section=1 data is available, the rest is 0.
    */
  Float_t SlopeForModule(unsigned int module, unsigned int section) const {

    Q_ASSERT(module < 328);
    Q_ASSERT(section < 3);
    return fSlopes[module][section];
  }

private:
  UChar_t fType;

  Float_t fGains[328][3]; // value-pairs for each module-section [N,C,P]
  Float_t fSlopes[328][3]; // value-pairs for each module-section [N,C,P]

  ClassDef(TRDModuleCalibration, 1);
};

}

#endif
