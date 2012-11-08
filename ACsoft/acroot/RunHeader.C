#include "RunHeader.h"
#include "Tools.h"

#include <QDateTime>

namespace AC {

BEGIN_DEBUG_TABLE(RunHeader)
  COL( "RunTag  ",           UShort_t,   RunTag)
  COL( "Run\t\t",            Int_t,      Run)
  COL( "AMSEvents",          Int_t,      TotalAMSEvents)
  COL( "FirstETime\t\t\t\t", TTimeStamp, FirstEventTimeStamp)
  COL( "LastETime\t\t\t\t",  TTimeStamp, LastEventTimeStamp)
  NEWTABLE
  COL( "TRDHV",              UShort_t,      TRDHighVoltage)
  COL( "AMSPatch",           Int_t,         Version)
  COL( "AMSPass",            Char_t,        AMSPassNumber)
  COL( "ACRootVS",           UChar_t,       ACRootVersion)
  COL( "ProducerTime\t\t\t", TTimeStamp,    ProducerJobTimeStamp)
  COL( "Streams\t\t",        StreamsVector, SelectedStreams) 
  NEWTABLE
  COL( "Filename\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", std::string,   StrippedAMSRootFileName)
  COL( "GitSHA  \t\t\t\t\t\t\t\t",                 std::string,   GitSHA)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const RunHeader& object) {

  stream << object.fRunTag << object.fRun << object.fTotalAMSEvents
         << object.fFirstEventTimeStamp << object.fLastEventTimeStamp
         << object.fTRDHighVoltage << object.fAMSPatchVersion;
  stream.writeRawData(&object.fAMSPassNumber, 1);

  stream << object.fACRootVersion << object.fProducerJobTimeStamp
         << object.fAMSRootFileName << object.fGitSHA << object.fSelectedStreams;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, RunHeader& object) {

  stream >> object.fRunTag >> object.fRun >> object.fTotalAMSEvents
         >> object.fFirstEventTimeStamp >> object.fLastEventTimeStamp
         >> object.fTRDHighVoltage >> object.fAMSPatchVersion;
  stream.readRawData(&object.fAMSPassNumber, 1);

  stream >> object.fACRootVersion >> object.fProducerJobTimeStamp
         >> object.fAMSRootFileName >> object.fGitSHA >> object.fSelectedStreams;

  return stream;
}

std::string RunHeader::StrippedAMSRootFileName() const {

  std::string::size_type location = fAMSRootFileName.find("castor/");
  if (location == std::string::npos)
    return fAMSRootFileName;

  std::string strippedFileName = fAMSRootFileName;
  strippedFileName.erase(0, location + 6);
  return strippedFileName;
}

/** Retrieves the Git SHA hash (aka. version number) for the current checkout **/
std::string calculateSoftwareVersionHash()
{
  const char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    std::cerr << "Could not find ACROOTSOFTWARE. Please make sure it's set!" << std::endl;
    return std::string();
  }

  FILE* file = popen("cd $ACROOTSOFTWARE && git rev-parse HEAD | tr -d '\n'", "r");
  char line[41];
  if (fscanf(file, "%s", line) != 1)
    std::cerr << "Could not read SHA1 hash!" << std::endl;
  pclose(file);
  return line;
}

}
