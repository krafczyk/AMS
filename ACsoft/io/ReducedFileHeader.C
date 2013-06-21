#include "ReducedFileHeader.hh"

#include <assert.h>
#include "Settings.h"
#include "TrdPdfLookup.hh"

namespace IO {

ReducedFileHeader::ReducedFileHeader()
  : fTrdQtPdfSettings(0) {
  
}

void ReducedFileHeader::Initialize(const std::string& acqtGitSHA) {

  fGitSHA = acqtGitSHA;
  fTrdAlignmentFileName = ::AC::Settings::gTrdQtAlignmentFileName;
  fTrdBeamTestAlignmentFileName = ::AC::Settings::gTrdQtBeamTestAlignmentFileName;
  fTrdGainFileName = ::AC::Settings::gTrdQtGainFileName;
  fTrdBeamTestGainFileName = ::AC::Settings::gTrdQtBeamTestGainFileName;
  fTrdPdfFileName = ::AC::Settings::gTrdQtPdfFileName;
  fTrdSlowControlFileName = ::AC::Settings::gTrdQtSlowControlFileName;
  fTrdQtPdfSettings = ACsoft::Utilities::TrdPdfLookup::Self()->Identifier();
  assert(fTrdQtPdfSettings);
}

}
