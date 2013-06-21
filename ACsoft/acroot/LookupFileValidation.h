#ifndef LOOKUPFILEVALIDATION_H
#define LOOKUPFILEVALIDATION_H

#include <fstream>
#include <sstream>
#include <stdexcept>

void VerifySettingsInFile(const std::string& fileIdentifier, std::ifstream& file, const std::string& expectedGitSHA, unsigned short expectedVersion);

#define VALIDATE_LOOKUP_FILE(FileIdentifier, VariableName) \
  VerifySettingsInFile(FileIdentifier, file, ::AC::Settings::VariableName##ExpectedGitSHA, ::AC::Settings::VariableName##ExpectedVersion)
  
#endif
