#include "LookupFileValidation.h"

#define DEBUG 0
#define INFO_OUT_TAG "LookupFileValidation> "
#include <debugging.hh>

void VerifySettingsInFile(const std::string& fileIdentifier, std::ifstream& file, const std::string& expectedGitSHA, unsigned short expectedVersion) {

  std::string gitSHA;
  unsigned short version = 0;
  file >> gitSHA >> version;

  if (gitSHA != expectedGitSHA) {
    WARN_OUT << "ERROR validating " << fileIdentifier << ". File mismatch! Expected Git SHA: \"" << expectedGitSHA << "\". Actual Git SHA: \"" << gitSHA << "\"" << std::endl;
    throw std::runtime_error("ERROR validating " + fileIdentifier);
  }

  if (version != expectedVersion) {
    WARN_OUT << "ERROR validating " << fileIdentifier << ". File mismatch! Expected version: \"" << expectedVersion << "\". Actual version: \"" << version << "\"" << std::endl;
    throw std::runtime_error("ERROR validating " + fileIdentifier);
  }
}
