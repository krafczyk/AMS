#include "RunHeader.h"
#include "Tools.h"

namespace ACsoft {

namespace AC {

BEGIN_DEBUG_TABLE(RunHeader)
  COL( "RunTag  ",                 UShort_t,          RunTag)
  COL( "RunType ",                 std::string,       RunTypeString)
  COL( "Run\t\t",                  Int_t,             Run)
  COL( "AMSEvents",                Int_t,             TotalAMSEvents)
  COL( "MCTriggeredEvents",        ULong64_t,         MCNumberOfTriggeredEvents)
  COL( "MCMinMomentum",            Float_t,           MCMinimumMomentum)
  COL( "MCMaxMomentum",            Float_t,           MCMaximumMomentum)
  COL( "FirstEventTime\t\t\t\t\t", TTimeStamp,        FirstEventTimeStamp)
  COL( "LastEventTime\t\t\t\t\t",  TTimeStamp,        LastEventTimeStamp)
  NEWTABLE
  COL( "TRDHV",              UShort_t,      TRDHighVoltage)
  COL( "AMSPatch",           Int_t,         Version)
  COL( "AMSPass",            Char_t,        AMSPassNumber)
  COL( "ACRootVS",           UChar_t,       ACRootVersion)
  COL( "ProducerTime\t\t\t", TTimeStamp,    ProducerJobTimeStamp)
  COL( "Streams\t\t",        StreamsVector, SelectedStreams) 
  NEWTABLE
  COL( "Filename\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", std::string,   StrippedAMSRootFileName)
  COL( "GitSHA  \t\t\t\t\t\t\t\t",                     std::string,   GitSHA)
END_DEBUG_TABLE

std::string RunHeader::StrippedAMSRootFileName() const {

  std::string::size_type location = fAMSRootFileName.find("castor/");
  if (location == std::string::npos)
    return fAMSRootFileName;

  std::string strippedFileName = fAMSRootFileName;
  strippedFileName.erase(0, location + 6);
  return strippedFileName;
}

}

}
