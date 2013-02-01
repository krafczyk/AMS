#ifndef IO_FileManager_hh
#define IO_FileManager_hh

#include <string>
#include <vector>
#include <QBuffer>
#include <TChain.h>

class QFile;
class TChain;
class TFile;
class TStopwatch;

namespace IO {
  class ReducedFileHeader;
}

namespace ACsoft {

namespace AC {
 class Event;
 class RunHeader;
}

// Helper macro to enforce inlining of methods in the .C files with gcc.
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif

namespace IO {
  class ReducedEvent;

/** Interface for access to ACQt files and ACROOT files.
  * ACQt files give full access to all AC datastructures to build full-fledged analysis/calibration software tools.
  * On the other hand ACROOT files provide a minimum set of data for the use within ROOT analysis macros (direct
  * processing/plotting of the ROOT TTree within the root interpreter).
  *
  * If your analysis code was built upon ACROOT files using the standard framework (FileManager, Analysis::Particle)
  * it will work as well on ACQt files, without source code modifications. All information provided via Analysis::Particle
  * works on both ACROOT/ACQt files. If you directly use any of the AC data structures, like TOFBeta/TrackerTrackFit, etc.
  * your code can only work on ACQt files, unless you add a new branch to the ROOT tree containing the necessary information.
  * 
  * Now let's inspect the FileManager interface in general. There are two operation modes:
  * a) Initialize with ReadFileList(), passing a .list file which contains the actual list of processed files.
  *
  *  Example usage (on ACQt input files):
  *
  *  \code
  *  #include "Event.h"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *    IO::FileManager fm;
  *    if (!fm.ReadFileList("mylist.list"))
  *      return 1;
  *    while (fm.GetNextEvent()) {
  *      AC::Event* event = fm.Event();
  *      // do something with event, eg.
  *      event->Dump();
  *    }
  *    ...
  *  }
  *  \endcode
  *
  **
  *  Example usage (on ACROOT input files):
  *
  *  \code
  *  #include "Event.h"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *    IO::FileManager fm;
  *    if (!fm.ReadFileList("mylist.list"))
  *      return 1;
  *    while (fm.GetNextEvent()) {
  *      Analyiss::ReducedEvent* event = fm.EventReduced();
  *      ...
  *    }
  *    ...
  *  }
  *  \endcode
  *
  *
  * b) Initialize with ReadFile(), passing a single .acqt file.
  *
  *  Example usage:
  *
  *  \code
  *  #include "Event.h"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *    IO::FileManager fm;
  *    if (!fm.ReadFile("my.acqt"))
  *      return 1;
  *    while (fm.GetNextEvent()) {
  *      AC::Event* event = fm.Event();
  *      // do something with event, eg.
  *      event->Dump();
  *    }
  *    ...
  *  }
  *  \endcode
  *
  */

class FileManager {
public:
  FileManager(bool shouldDumpSettings = true);
  virtual ~FileManager();

  /** Utility function to assemble an output file name according to standard scheme.
    *
    * Typically returns something like \c resultdir/prefix_suffix.root, but also handles the case where resultdir and/or suffix are empty.
    */
  static std::string MakeStandardRootFileName( std::string resultdir, std::string prefix, std::string suffix);

  /** Utility function to assemble an output file name according to standard scheme for pdf files.
    *
    * Typically returns something like \c resultdir/prefix_suffix.pdf, but also handles the case where resultdir and/or suffix are empty.
    */
  static std::string MakeStandardPdfFileName( std::string resultdir, std::string prefix, std::string suffix);

  /** Read a file list, and open all files specificed in that list.
    *
    * Use the tool scripts/make_ACQT_filelist.py to generate a list suitable for use
    * with the FileManager.
    *
    * This function does not initialize Event() and RunHeader() objects. The first call of GetNextEvent() or GetEvent() does this.
    *
    * \param[in] filelist list of ACROOT or ACQt files (but not both!), one file per line, with full directory.
    * \param[in] customfilelist list of ACROOT files containing custom branches (only needed when using custom variables + ACROOT files)
    * \return \c true if initialized correctly, \c false otherwise
   */
  bool ReadFileList(const std::string& filelist, std::string customfilelist = "");

  /** Read a single ACROOT/ACQt file.
    *
    * This function does not initialize Event() and RunHeader() objects. The first call of GetNextEvent() or GetEvent() does this.
    *
    * \param[in] file single ACROOT (.root) or ACQt (.acqt) file.
    * \return \c true if initialized correctly, \c false otherwise
   */
  bool ReadFile(const std::string& file);

  /** Returns a pointer to the last read Event. Returns 0 for non-ACQt files (ACROOT files).
    */
  AC::Event* Event() const { return fEvent; }

  /** Returns a pointer to the last read ReducedEvent. Returns 0 for non-ACROOT files (ACQt files).
    */
  ReducedEvent* EventReduced() const { return fReducedEvent; }

  /** Returns a pointer to the last read ReducedFileHeader. Returns 0 for non-ACROOT files (ACQt files).
    */
  ::IO::ReducedFileHeader* FileHeaderReduced() const { return fReducedFileHeader; }

  /** Returns a pointer to the current RunHeader. This will be 0 for non-ACQt files.
    * Note: If you navigate from one run to the other, the run header will differ!
    */
  AC::RunHeader* RunHeader() const { return fRunHeader; }

  /** Moves to next Event.
    *
    * If none of GetNextEvent(), or GetEvent(int) were called before, this moves the manager to the first event.
    * After calling this function it's safe to access both Event, and the RunHeader() inside the Event.
    *
    * \see GetEvent()
    * \return true if next event was available, otherwise false
    */
  bool GetNextEvent();

  /** Moves to next event at index.
    *
    * \see GetNextEvent()
    * \param[in] index index of event to get
    * \return true if event at index was available, otherwise false
    */
  bool GetEvent(int index);

  /** Resets FileManager to the state, right after calling ReadFileList/ReadFile.
    * After this call the FileManager no longer points to any event.
    */
  void Reset();

  /** Get sum of entries in all runs.
    *
    * \return number of entries in all runs
    */
  int GetEntries() const { return fEntries; }

  /** Get current event index.
    *
    * Note: This returns -1, if no event was ever loaded. (Before the first call of GetNextEvent/GetEvent).
    * \return current event index
    */
  int GetCurrentEventIndex() const { return fCurrentEventNumber; }

  /** Register a custom branch within the ROOT tree that's read as well during GetNextEvent() & co.
    **/
  template<class T>
  void ReadCustomBranch(const char* branchName, T* pointerToType) {

    if (fChain)
      fChain->SetBranchAddress(branchName, pointerToType);
  }

  /** Dumps an ASCII event processing progress bar.
    * Use this from the main event loop, if desired.
    */
  void DumpEventLoopProgress(unsigned int dumpNEvents = 20000, char separator = '\r');

  /** Checks whether all loaded files are valid.
    * Note: This is used for debugging and/or by the acroot_dump utility.
    */
  bool CheckFileIntegrities() const;

  static void ExpandEnvironmentVariables(std::string&);

private:
  friend bool NonInlineGetNextACQtDataChunk(FileManager*); 

  bool DoesFileExist(const std::string&);
  void DumpSettings();

  // ACROOT specific code, implemented in FileManagerACROOT.C
  void DestructACROOTFiles();
  bool GetNextACQtEvent();
  bool GetACQtEvent(int index);
  bool InitializeACROOTFiles(const std::vector<std::string>&, const std::vector<std::string>&);
  void ResetACROOTFiles();
  void RetrieveFirstAndLastEventTimeForACROOTFiles();
  void SetACROOTBranchAddresses();

  // ACQt specific code, implemented in FileManagerACQt.C.
  bool CheckACQtFileIntegrity(unsigned int fileNumber) const;
  void DestructACQtFiles();
  bool GetNextACROOTEvent();
  bool GetACROOTEvent(int index);
  bool InitializeACQtFiles(const std::vector<std::string>&);
  void ResetACQtFiles();
  void RetrieveFirstAndLastEventTimeForACQtFiles();


  /** ACQtFile structure, used only internally.
   */
  struct ACQtFile {
    ACQtFile()
      : file(0)
      , header(0)
      , version(0)
      , headerSize(0)
      , entries(0) {

    }

    QFile* file;
    AC::RunHeader* header;
    unsigned short version;
    unsigned int headerSize;
    unsigned int entries;
  };

  bool OpenACQtFile(ACQtFile&);
  bool GetNextACQtDataChunk();
  friend bool OpenACQtFileIfNeeded(const ACQtFile&, bool);

private:
  bool fShouldDumpSettings;
  int fCurrentEventNumber;
  int fEntries;

  TStopwatch* fRunTimer;
  TStopwatch* fEventTimer;
  float fAccumulatedEventRealTime;
  unsigned int fEventBunchSize;
  unsigned int fNumberOfProcessedBunches;
  float fAccumulatedRealTime;
  float fAccumulatedCPUTime;
  unsigned int fNumberOfProcessedRuns;
  unsigned int fCurrentFile;
  unsigned int fNumberOfFiles;

  // ACQt specific variables.
  AC::RunHeader* fRunHeader;
  AC::Event* fEvent;
  std::vector<ACQtFile> fACQtFiles;
  unsigned int fCurrentACQtFile;
  unsigned long fChunkBufferSize;
  QBuffer fChunkBuffer;

  // ACROOT specific variables.
  ReducedEvent* fReducedEvent;
  ::IO::ReducedFileHeader* fReducedFileHeader;
  TChain* fChain;
  TFile* fCurrentRootFile;
  int fRun;
  TChain* fCustomChain;
};

}

}

#endif
