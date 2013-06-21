#ifndef MagnetTemperatures_h
#define MagnetTemperatures_h

#include <TObject.h>
#include <TTree.h>
#include <TBranch.h>

#ifndef __CINT__
#include <QtGlobal>
#endif

namespace AC {

/** Wrapper class around TTree containing magnet rigidity correction factor and temperatures.
  */
// FIXME: Extend this class to be able to read in an existing MagnetTempatures tree (integrated with DBFileManager)
class MagnetTemperatures {
public:
  MagnetTemperatures() {

    fTree = new TTree("MagnetTemperatureTree", "MagnetTemperatureTree");
    fTree->Branch("UTime",        &fUnixTime,                 "UTime/i");
    fTree->Branch("CorrFactor",   &fRigidityCorrectionFactor, "CorrFactor/F");
    fTree->Branch("Temperatures", fTemperatures,              "Temperatures[12]/F");

    fSensorName[ 0] = "Port";
    fSensorName[ 1] = "Ram";
    fSensorName[ 2] = "Starboard";
    fSensorName[ 3] = "Wake";
    fSensorName[ 4] = "M-3X:31:UCF3";
    fSensorName[ 5] = "M-3X:32:UCF4";
    fSensorName[ 6] = "M-3X:35:UCF7";
    fSensorName[ 7] = "M-3X:36:UCF8";
    fSensorName[ 8] = "M-3X:39:LCF3";
    fSensorName[ 9] = "M-3X:40:LCF4";
    fSensorName[10] = "M-3X:43:LCF7";
    fSensorName[11] = "M-3X:44:LCF8";
  }

  int Entries() const { return fTree->GetEntries(); }

  const char* SensorName(unsigned int i) const {
 
    assert(i < gSensorCount);
    return fSensorName[i].c_str();
  }
 
  // Only used via ac_producer.
  void Fill(UInt_t time, Float_t corrFactor, Float_t* temperatures) {
    fUnixTime = time;
    fRigidityCorrectionFactor = corrFactor;
    for (unsigned int i = 0; i < gSensorCount; ++i)
      fTemperatures[i] = temperatures[i];
    fTree->Fill();
  }

  void Write() {
    fTree->Write();
  }

  static const unsigned int gSensorCount = 12;

private:
  TTree* fTree;
  UInt_t fUnixTime;
  Float_t fRigidityCorrectionFactor;
  Float_t fTemperatures[gSensorCount];
  std::string fSensorName[gSensorCount];
};

}

#endif
