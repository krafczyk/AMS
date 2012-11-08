#ifndef SETTINGS_H
#define SETTINGS_H

#include <TNamed.h>

namespace AC {

#define DECLARE_SETTING(Variable) \
  static const char* Variable; \
  static const char* Variable##ExpectedGitSHA; \
  static const unsigned short Variable##ExpectedVersion; \

/** Global names of current version of all lookup files. */
struct Settings {

  // Text files
  DECLARE_SETTING(gTrdQtGeometryFileName);
  DECLARE_SETTING(gTrdQtShimmingGlobalFileName)
  DECLARE_SETTING(gTrdQtShimmingModuleFileName)

  // ROOT files
  DECLARE_SETTING(gTrdQtAlignmentFileName)
  DECLARE_SETTING(gTrdQtGainFileName)
  DECLARE_SETTING(gTrdQtSlowControlFileName)
  DECLARE_SETTING(gTrdQtPdfFileName)

  static const char* gACQtPassTag;
  static const char gACQtVersion;
};

/** Helper class that's written to a ROOT file to identify its origin */
class ACQtLookupFileIdentifier : public TNamed {
public:
  ACQtLookupFileIdentifier()
    : TNamed("ACQtVersion", "")
    , fVersion(0) {

  }

  std::string fGitSHA;     ///> Git SHA used to produce this file
  unsigned short fVersion; ///> Version of the lookup file

  ClassDef(AC::ACQtLookupFileIdentifier, 2)
};

/** Helper class that's written to a ROOT file to identify its origin */
class ACQtPDFLookupFileIdentifier : public TNamed {
public:
  ACQtPDFLookupFileIdentifier()
    : TNamed("ACQtPDFVersion", "")
    , fVersion(0)
    , fMinimumPathLengthForCandidateMatching(0)
    , fUseActiveStrawsForCandidateMatching(false) {

  }

  std::string fGitSHA;     ///> Git SHA used to produce this file
  unsigned short fVersion; ///> Version of the lookup file
  float fMinimumPathLengthForCandidateMatching;
  bool fUseActiveStrawsForCandidateMatching;

  ClassDef(AC::ACQtPDFLookupFileIdentifier, 1)
};

}

#endif // SETTINGS_H
