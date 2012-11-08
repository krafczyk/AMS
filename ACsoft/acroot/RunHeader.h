#ifndef RunHeader_h
#define RunHeader_h

#include <stdlib.h>
#include "Tools.h"

class ACQtWriter;

namespace AC {

/** Run header data
  */
class RunHeader {
  WTF_MAKE_FAST_ALLOCATED;
public:
  RunHeader()
    : fRunTag(0)
    , fRun(0)
    , fTotalAMSEvents(0)
    , fTRDHighVoltage(0)
    , fAMSPatchVersion(0)
    , fAMSPassNumber(0)
    , fACRootVersion(0) {

  }
 
  /** A vector of strings */
  typedef Vector<std::string, 2> StreamsVector;

  /** Helper method dumping an RunHeader object to the console
    */
  void Dump() const;

  /** Run tag.
    * \todo Add documentation.
    */
  UShort_t RunTag() const { return fRunTag; }

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

private:
  friend class ::ACQtWriter;

  UShort_t fRunTag;                 // Header->RunType
  Int_t fRun;                       // AMSEvent->Run
  Int_t fTotalAMSEvents;            // ams->GetEntries()
  TTimeStamp fFirstEventTimeStamp;  // AMSsetup->fHeader.FEventTime;
  TTimeStamp fLastEventTimeStamp;   // AMSsetup->fHeader.LEventTime;
  UShort_t fTRDHighVoltage;         // from AMI?
  UShort_t fAMSPatchVersion;        // Header->Versiong (example '584' for B584)
  Char_t fAMSPassNumber;            // >=0: ISS-Data 0: std  +N: passN <0: MC
  UChar_t fACRootVersion;           // 10*Major+Subversion
  TTimeStamp fProducerJobTimeStamp; // AMSEvent->UTime    (UNIX-Time GMT/GPS/1553?)
  std::string fAMSRootFileName;     // AMS-ROOT Filename
  std::string fGitSHA;              // GIT Revision identifier
  StreamsVector fSelectedStreams;   // ac_producer production stream categories

  REGISTER_CLASS_WITH_TABLE(RunHeader)
};

std::string calculateSoftwareVersionHash();

}

#endif
