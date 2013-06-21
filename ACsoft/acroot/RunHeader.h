#ifndef RunHeader_h
#define RunHeader_h

#include <stdlib.h>
#include "Tools.h"
#include "RunHeader-Streamer.h"

namespace ACsoft {

class ACQtWriter;

namespace AC {

enum RunTypeIdentifier {
  ISSRun = 0,
  MCRun = 1,
  BTRun = 2
};

#ifndef __CINT__
// QDataStream operators
inline QDataStream& operator<<(QDataStream& stream, const RunTypeIdentifier& object) {

  stream << (UShort_t) object;
  return stream;
}

inline QDataStream& operator>>(QDataStream& stream, RunTypeIdentifier& object) {

  UShort_t runType;
  stream >> runType;
  object = (RunTypeIdentifier) runType;
  return stream;
}
#endif

/** Run header data
  */
class RunHeader {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of strings */
  typedef Vector<std::string, 2> StreamsVector;

  AC_RunHeader_Variables

  /** Helper method dumping an RunHeader object to the console
    */
  void Dump() const;

  /** Run tag.
    * \todo Add documentation.
    */
  UShort_t RunTag() const { return fRunTag; }

  /** Run type.
    * Used to differentiate between ISS/MC/BT runs.
    */
  RunTypeIdentifier RunType() const { return static_cast<RunTypeIdentifier>(fRunType); }

  /** Run number.
    */
  Int_t Run() const { return fRun; }

  /** Total events in AMS run. May be larger than the actual number of
    * events in the ACRoot/ACQt file due the data reduction.
    */
  Int_t TotalAMSEvents() const { return fTotalAMSEvents; }

  /** Time of first event in run.
    */
  const TTimeStamp& FirstEventTimeStamp() const { return fFirstEventTimeStamp; }

  /** Time of last event in run.
    */
  const TTimeStamp& LastEventTimeStamp() const { return fLastEventTimeStamp; }

  /** TRD High Voltage [V].
    */
  UShort_t TRDHighVoltage() const { return fTRDHighVoltage; }

  /** AMS Patch version - returns 584 for B584.
    * Straight copy from "Version()" in http://ams.cern.ch/cgi-bin/viewcvs.cgi/AMS/include/root.h?view=markup.
    */
  Int_t Version() const { return abs(fAMSPatchVersion / 16) > 465 ? (fAMSPatchVersion > 0 ? fAMSPatchVersion / 16 : 1023 + fAMSPatchVersion / 16) : fAMSPatchVersion / 4; }

  /** AMS Pass number
    * \todo Add documentation.
    */
  Char_t AMSPassNumber() const { return fAMSPassNumber; }

  /** ACRoot version
    * \todo Add documentation.
    */
  UChar_t ACRootVersion() const { return fACRootVersion; }

  /** Producer time stamp.
    */
  const TTimeStamp& ProducerJobTimeStamp() const { return fProducerJobTimeStamp; }

  /** AMS Root file name.
    */
  std::string AMSRootFileName() const { return fAMSRootFileName; }

  /** Stripped AMS Root file name.
    * Removes the CASTOR file name prefix if present.
    */
  std::string StrippedAMSRootFileName() const;

  /** Git SHA commit identifier.
    * Every ACRoot/ACQt file is associated with the SHA1 of the software that was used to create it.
    */
  std::string GitSHA() const { return fGitSHA; }

  /** ac_producer production stream categories.
    * A run falls in one of these exclusive categories:
    * - HR (high rate from SAA & poles)
    * - NP (no particle)
    * - MP (multiple particles)
    * - E1 (single Particle with TrTrack with JL1 hit with ECAL Shower)
    * - ST (Single Particle with TrTrack)
    * - SP (Single Particle)
    * A vector of strings describes in which production category this event belongs, eg. "ST,E1".
    */
  const StreamsVector& SelectedStreams() const { return fSelectedStreams; }

  /** Returns the number of triggered events, as specified in the MC datacard, for MC runs.
    * Otherwhise this returns 0.
    */
  ULong64_t MCNumberOfTriggeredEvents() const { return fMCNumberOfTriggeredEvents; }

  /** Returns the lower boundary of the generated MC momentum spectrum, as specified in the MC datacard, for MC runs.
    * Otherwhise this returns 0.
    */
  Float_t MCMinimumMomentum() const { return fMCMinimumMomentum; }

  /** Returns the upper boundary of the generated MC momentum spectrum, as specified in the MC datacard, for MC runs.
    * Otherwhise this returns 0.
    */
  Float_t MCMaximumMomentum() const { return fMCMaximumMomentum; }

private:
  /** Accessor to runtype as string.
    */
  std::string RunTypeString() const {
    return RunTypeAsString(RunType());
  }

public:
  /** Run type as string.
    */
  static std::string RunTypeAsString( RunTypeIdentifier id ) {
    switch (id) {
    case ISSRun: return "ISS";
    case MCRun: return "MC";
    case BTRun: return "BT";
    }
    return "";
  }

  friend class ACsoft::ACQtWriter;
  REGISTER_CLASS_WITH_TABLE(RunHeader)
};

}

}

#endif
