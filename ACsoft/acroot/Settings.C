#include "Settings.h"

#define DEFINE_SETTING(Variable, FileName, GitSHA, Version) \
  const char* AC::Settings::Variable = FileName; \
  const char* AC::Settings::Variable##ExpectedGitSHA = GitSHA; \
  const unsigned short AC::Settings::Variable##ExpectedVersion = Version;

#define stringifyi(argument) #argument
#define stringify(argument) stringifyi(argument)

DEFINE_SETTING(gTrdQtGeometryFileName,       "TrdQt_geometry_v01.csv",        "83521a54b0d533403d8db0a0ebc62a212406c73f",  1)
DEFINE_SETTING(gTrdQtShimmingGlobalFileName, "TrdQt_shimming_global_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f",  2)
DEFINE_SETTING(gTrdQtShimmingModuleFileName, "TrdQt_shimming_module_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f",  2)
DEFINE_SETTING(gTrdQtAlignmentFileName,      "TrdQt_alignment_v08.root",      "c65e81dc3b3558f1aef468e168466276d7f470dd",  8)
DEFINE_SETTING(gTrdQtGainFileName,           "TrdQt_gain_v08.root",           "c65e81dc3b3558f1aef468e168466276d7f470dd",  8)
DEFINE_SETTING(gTrdQtPdfFileName,            "TrdQt_pdf_v17.root",            "c65e81dc3b3558f1aef468e168466276d7f470dd", 17)
DEFINE_SETTING(gTrdQtSlowControlFileName,    "TrdQt_slowcontrol_v04.root",    "cd385bf986c8c4ffc4e7069c9dc434c67d4a7b9c",  4)

const char* AC::Settings::gACSoftGitSHA = stringify(ACPRODUCTIONSHA);

unsigned short& AC::CurrentACQtVersion() {

  static unsigned short* gVersion = 0;
  if (!gVersion)
    gVersion = new unsigned short(0);
  return *gVersion;
}
