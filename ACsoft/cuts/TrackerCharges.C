#include "Settings.h"
#include "TrackerCharges.hh"
#include "LookupFileValidation.h"

#include <assert.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

#define DEBUG 0
#define INFO_OUT_TAG "TrackerCharges> "
#include <debugging.hh>

namespace Cuts {

float TrackerCharges::gTrackerChargeMean[gTrackerCharges]  = {0, 0, 0, 0, 0, 0, 0, 0};
float TrackerCharges::gTrackerChargeSigma[gTrackerCharges] = {0, 0, 0, 0, 0, 0, 0, 0};
bool TrackerCharges::gTrackerChargeIsInitialized = false;

float TrackerCharges::GetMean(unsigned int index) {

  assert(index < 8);
  if (!gTrackerChargeIsInitialized) 
    InitTrackerChargesFile();
  return gTrackerChargeMean[index];
}

float TrackerCharges::GetSigma(unsigned int index){

  assert(index < 8);
  if (!gTrackerChargeIsInitialized) 
    InitTrackerChargesFile();
  return gTrackerChargeSigma[index];
}

void TrackerCharges::InitTrackerChargesFile(){

  char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    WARN_OUT << "Tracker Charge information are NOT available." << std::endl;
    return;
  }

  std::string filePath(acrootsoftware);
  filePath += "/acroot/data/";

  std::string fname = filePath + AC::Settings::gTrackerChargesFileName;
  INFO_OUT << "InitTrackerChargesFile from file " << fname << std::endl;

  std::ifstream file(fname.c_str());
  if (!file.good())
    throw std::runtime_error("ERROR opening Tracker charges definition file.");

  VALIDATE_LOOKUP_FILE("Tracker Charges File", gTrackerChargesFileName);

  int peakIndex;
  while (file.good()) {
    file >> peakIndex;
    assert(peakIndex > 0);
    assert(peakIndex < 9);
    file >> gTrackerChargeMean[peakIndex - 1] >> gTrackerChargeSigma[peakIndex - 1];

    if (file.eof())
      break;
  
    DEBUG_OUT << peakIndex << ". Peak " << std::setw(10) << "Mean: " << gTrackerChargeMean[peakIndex-1] << std::setw(10) << "Sigma: " << gTrackerChargeSigma[peakIndex-1] << std::endl;
  }

  file.close();
  gTrackerChargeIsInitialized = true;
}

}
