#include "Settings.h"

#define DEFINE_SETTING(Variable, FileName, GitSHA, Version) \
  const char* AC::Settings::Variable = FileName; \
  const char* AC::Settings::Variable##ExpectedGitSHA = GitSHA; \
  const unsigned short AC::Settings::Variable##ExpectedVersion = Version;

#define stringifyi(argument) #argument
#define stringify(argument) stringifyi(argument)

const Float_t AC::Settings::TrdTubeDefaultMinPathLength = 0.1;
const double AC::Settings::TrdMinimumPdfValue = 1.e-7;

DEFINE_SETTING(gTrdQtGeometryFileName,       "TrdQt_geometry_v01.csv",        "83521a54b0d533403d8db0a0ebc62a212406c73f",  1)
DEFINE_SETTING(gTrdQtShimmingGlobalFileName, "TrdQt_shimming_global_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f",  2)
DEFINE_SETTING(gTrdQtShimmingModuleFileName, "TrdQt_shimming_module_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f",  2)
DEFINE_SETTING(gTrdQtDeadStrawsFileName,     "TrdQt_dead_straws_v01.txt",     "b02dd5a98a5e144df64af1f0462da4c03be4f49e",  1)
DEFINE_SETTING(gTrackerChargesFileName,      "TrackerCharges_v02.txt",        "fb29b69e4a6b958d8d187d35804773c285e532ec",  2)
DEFINE_SETTING(gTrdQtAlignmentFileName,      "TrdQt_alignment_v10.root",      "e05064fee36154f8d74e21ab5d3faa93e4518eed", 10)
DEFINE_SETTING(gTrdQtGainFileName,           "TrdQt_gain_v10.root",           "e05064fee36154f8d74e21ab5d3faa93e4518eed", 10)
DEFINE_SETTING(gTrdQtPdfFileName,            "TrdQt_pdf_v20.root",            "e8043f18465e0a0e3a15c60041835e8efd974f16", 20)
DEFINE_SETTING(gTrdQtSlowControlFileName,    "TrdQt_slowcontrol_v05.root",    "86e62148d1ba946e66ddcfe0f798d4463aa92f3d",  5)

// Beamtest specific lookups
DEFINE_SETTING(gTrdQtBeamTestAlignmentFileName, "TrdQt_bt_alignment_v10.root", "57abd5520da004c88c7803080a2e933b9c508156", 10)
DEFINE_SETTING(gTrdQtBeamTestGainFileName,      "TrdQt_bt_gain_v10.root",      "57abd5520da004c88c7803080a2e933b9c508156", 10)

const char* AC::Settings::gACSoftGitSHA = stringify(ACPRODUCTIONSHA);

unsigned short& AC::CurrentACQtVersion() {

  static unsigned short* gVersion = 0;
  if (!gVersion)
    gVersion = new unsigned short(0);
  return *gVersion;
}
