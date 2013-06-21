#ifndef SETTINGS_H
#define SETTINGS_H

#include <TNamed.h>

namespace AC {

#define DECLARE_SETTING(Variable) \
  static const char* Variable; \
  static const char* Variable##ExpectedGitSHA; \
  static const unsigned short Variable##ExpectedVersion;

/** Global settings and names of current version of all lookup files. */
struct Settings {

  /** Minimal path length in TRD tubes [cm] */
  static const Float_t TrdTubeDefaultMinPathLength;

  static const double TrdMinimumPdfValue;

  // Text files
  DECLARE_SETTING(gTrdQtGeometryFileName)
  DECLARE_SETTING(gTrdQtShimmingGlobalFileName)
  DECLARE_SETTING(gTrdQtShimmingModuleFileName)
  DECLARE_SETTING(gTrdQtDeadStrawsFileName)
  DECLARE_SETTING(gTrackerChargesFileName)

  // ROOT files
  DECLARE_SETTING(gTrdQtAlignmentFileName)
  DECLARE_SETTING(gTrdQtBeamTestAlignmentFileName)
  DECLARE_SETTING(gTrdQtGainFileName)
  DECLARE_SETTING(gTrdQtBeamTestGainFileName)
  DECLARE_SETTING(gTrdQtSlowControlFileName)
  DECLARE_SETTING(gTrdQtPdfFileName)

  static const char* gACSoftGitSHA;
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

  ClassDef(AC::ACQtLookupFileIdentifier,2)
};

/** Helper class that's written to a ROOT file to identify its origin */
class ACQtPDFLookupFileIdentifier : public TNamed {
public:
  ACQtPDFLookupFileIdentifier()
    : TNamed("ACQtPDFVersion", "")
    , fVersion(0)
    , fAddNearTrackHits(false) {

  }

  std::string fGitSHA;     ///> Git SHA used to produce this file
  unsigned short fVersion; ///> Version of the lookup file
  bool fAddNearTrackHits;

  ClassDef(AC::ACQtPDFLookupFileIdentifier,2)
};

unsigned short& CurrentACQtVersion();

}

#endif // SETTINGS_H
