#include "Settings.h"

#define DEFINE_SETTING(Variable, FileName, GitSHA, Version) \
  const char* AC::Settings::Variable = FileName; \
  const char* AC::Settings::Variable##ExpectedGitSHA = GitSHA; \
  const unsigned short AC::Settings::Variable##ExpectedVersion = Version;

DEFINE_SETTING(gTrdQtGeometryFileName,       "TrdQt_geometry_v01.csv",        "83521a54b0d533403d8db0a0ebc62a212406c73f", 1)
DEFINE_SETTING(gTrdQtShimmingGlobalFileName, "TrdQt_shimming_global_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f", 2)
DEFINE_SETTING(gTrdQtShimmingModuleFileName, "TrdQt_shimming_module_v02.txt", "83521a54b0d533403d8db0a0ebc62a212406c73f", 2)
DEFINE_SETTING(gTrdQtAlignmentFileName,      "TrdQt_alignment_v03.root",      "72ea14758f12c1bce39e3a8fb8cec29d71c2cf96", 3)
DEFINE_SETTING(gTrdQtGainFileName,           "TrdQt_gain_v03.root",           "8ec1499266d8164d1fa14971d8be70c4a95a7941", 3)
DEFINE_SETTING(gTrdQtPdfFileName,            "TrdQt_pdf_v07.root",            "0dfafd41620c09d564b30797b0fc5de366e72d6f", 7) // FIXME: Reproduce pdfs with slow control v02, currently produced with v01, which didn't cover the whole time period
DEFINE_SETTING(gTrdQtSlowControlFileName,    "TrdQt_slowcontrol_v02.root",    "a91b95f9a23627e2bc0ad38ff8441d7a7a5fb347", 2)

const char* AC::Settings::gACQtPassTag = ACPASSTAG;
const char AC::Settings::gACQtVersion = ACROOTVERSION;
