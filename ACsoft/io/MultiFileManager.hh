#ifndef IO_MultiFileManager_hh
#define IO_MultiFileManager_hh

#include <string>
#include <vector>
#include <QBuffer>

namespace ACsoft {

namespace AC {
  class Event;
  class RunHeader;
}

namespace IO {
  class FileManager;

/** Interface for access to multiple ACQt files at the same time.
  * Useful for running over PWT/SAA/OTH streams in AMS event order.
  *
  *  Example usage:
  *
  *  \code
  *  #include "Event.h"
  *  #include "MultiFileManager.hh"
  *
  *  int main() {
  *    IO::MultiFileManager fm;
  *    if (!fm.ReadFileList("somePWT.list,someOTH.list,someSAA.list"))
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
  *
  */

class MultiFileManager {
public:
  MultiFileManager(bool shouldDumpSettings = true);
  ~MultiFileManager();

  /** Read a file list, and open all files specificed in that list.
    *
    * Use the tool scripts/make_ACQT_filelist.py to generate a list suitable for use
    * with the MultiFileManager.
    *
    * This function does not initialize Event() and RunHeader() objects. The first call of GetNextEvent() or GetEvent() does this.
    *
    * \param[in] filelist list of ACQt file lists, seperated by comma.
    * \return \c true if initialized correctly, \c false otherwise
   */
  bool ReadFileList(const std::string& filelist);

  /** Returns a pointer to the last read Event. Returns 0 for non-ACQt files (ACROOT files).
    */
  AC::Event* Event() const;

  /** Returns a pointer to the current RunHeader. This will be 0 for non-ACQt files.
    * Note: If you navigate from one run to the other, the run header will differ!
    */
  AC::RunHeader* RunHeader() const;

  /** Moves to next Event.
    *
    * If none of GetNextEvent(), or GetEvent(int) were called before, this moves the manager to the first event.
    * After calling this function it's safe to access both Event, and the RunHeader() inside the Event.
    *
    * \see GetEvent()
    * \return true if next event was available, otherwise false
    */
  bool GetNextEvent();

  /** Resets MultiFileManager to the state, right after calling ReadFileList.
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

  /** Get current file name.
    * Returns an empty string for non-ACQt files (ACROOT files).
    */
  std::string GetCurrentFileName() const;

  /** Dumps an ASCII event processing progress bar.
    * Use this from the main event loop, if desired.
    */
  void DumpEventLoopProgress(unsigned int dumpNEvents = 20000, char separator = '\r');

private:
  bool fShouldDumpSettings;
  int fCurrentFileManager;
  int fEntries;
  int fCurrentEventNumber;
  std::vector<IO::FileManager*> fFileManagers;
  std::vector<std::string> fStreamTags;
};

}

}

#endif
