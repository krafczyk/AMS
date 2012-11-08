#ifndef Analysis_FileManager_hh
#define Analysis_FileManager_hh

#include <string>
#include <vector>
#include <QBuffer>

class QFile;
class TStopwatch;

namespace AC {
 class Event;
 class RunHeader;
}

// Helper macro to enforce inlining of methods in the .C files with gcc.
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif

/// Tools for high-level analysis.
namespace Analysis {

/** Interface for access to ACQt event files.
  * There are two operation modes.
  * a) Initialize with ReadFileList(), passing a .list file which contains the actual list of processed files.
  *
  *  Example usage:
  *
  *  \code
  *  #include "Event.h"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *    Analysis::FileManager fm;
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
  * b) Initialize with ReadFile(), passing a single .acqt file.
  *
  *  Example usage:
  *
  *  \code
  *  #include "Event.h"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *    Analysis::FileManager fm;
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
  FileManager();
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
    * Use the tool scripts/make_ACROOT_filelist.py to generate a list suitable for use
    * with the FileManager.
    *
    * This function does not initialize Event() and RunHeader() objects. The first call of GetNextEvent() or GetEvent() does this.
    *
    * \param[in] filelist list of ACROOT or ACQt files (but not both!), one file per line, with full directory.
    * \return \c true if initialized correctly, \c false otherwise
   */
  bool ReadFileList(const std::string& filelist);

  /** Read a single ACROOT/ACQt file.
    *
    * This function does not initialize Event() and RunHeader() objects. The first call of GetNextEvent() or GetEvent() does this.
    *
    * \param[in] file single ACROOT (.root) or ACQt (.acqt) file.
    * \return \c true if initialized correctly, \c false otherwise
   */
  bool ReadFile(const std::string& file);

  /** Returns a pointer to Event.
    */
  AC::Event* Event() const { return fEvent; }

  /** Returns a pointer to the current RunHeader.
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

  /** Dumps an ASCII event processing progress bar.
    * Use this from the main event loop, if desired.
    */
  void DumpEventLoopProgress(unsigned int dumpNEvents = 20000, char separator = '\r');

  /** Checks whether all loaded files are valid.
    * Note: This is used for debugging and/or by the acroot_dump utility.
    */
  bool CheckFileIntegrities() const;

private:
  friend bool NonInlineGetNextACQtDataChunk(FileManager*); 

  bool CheckFileIntegrity(unsigned int fileNumber) const;
  bool InitializeACQtFiles( const std::vector<std::string>& files );

  void UpdateRunHeaderIfNeeded();
  void RetrieveFirstAndLastEventTime();

  /** ACQtFile structure, used only internally.
   */
  struct ACQtFile {
    ACQtFile()
      : file(0)
      , header(0)
      , headerSize(0)
      , entries(0) {

    }

    QFile* file;
    AC::RunHeader* header;
    unsigned int headerSize;
    unsigned int entries;
  };

  bool OpenACQtFile(ACQtFile&);
  bool GetNextACQtDataChunk();
  friend bool OpenACQtFileIfNeeded(const ACQtFile&, bool);

private:
  int fCurrentEventNumber;
  int fLastTreeNumber;
  AC::RunHeader* fRunHeader;
  AC::Event* fEvent;
  int fEntries;

  TStopwatch* fRunTimer;
  float fAccumulatedRealTime;
  float fAccumulatedCPUTime;
  unsigned int fNumberOfProcessedRuns;

  // ACQt specific variables.
  std::vector<ACQtFile> fACQtFiles;
  unsigned int fCurrentACQtFile;
  unsigned long fChunkBufferSize;
  QBuffer fChunkBuffer;
};

}

#endif
