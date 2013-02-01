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
  fTrdGainFileName = ::AC::Settings::gTrdQtGainFileName;
  fTrdPdfFileName = ::AC::Settings::gTrdQtPdfFileName;
  fTrdSlowControlFileName = ::AC::Settings::gTrdQtSlowControlFileName;
  fTrdQtPdfSettings = ACsoft::Utilities::TrdPdfLookup::Self()->Identifier();
  assert(fTrdQtPdfSettings);
}

}
