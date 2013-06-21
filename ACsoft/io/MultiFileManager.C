#include "MultiFileManager.hh"
#include "FileManager.hh"
#include "StringTools.hh"
#include "Event.h"
#include "TimeHistogramManager.hh"

#include <TTimeStamp.h>

#define DEBUG 0
#define INFO_OUT_TAG "MultiFileManager> "
#include <debugging.hh>

namespace ACsoft {

namespace IO {

MultiFileManager::MultiFileManager(bool shouldDumpSettings)
  : fShouldDumpSettings(shouldDumpSettings)
  , fCurrentFileManager(-1)
  , fEntries(0) 
  , fCurrentEventNumber(0) {

}

MultiFileManager::~MultiFileManager() {

  for (unsigned int i = 0; i < fFileManagers.size(); ++i)
    delete fFileManagers[i]; // vector of pointers 

  fFileManagers.clear();
}

bool MultiFileManager::ReadFileList(const std::string& multiFileList) {

  // comma separated filelists for separate FileManager instances
  assert(fCurrentFileManager == -1);
  assert(!fEntries);

  TTimeStamp minimumTime(0, 0);
  TTimeStamp maximumTime(0, 0);
  std::vector<std::string> fileLists = split(multiFileList, ',');
  for (unsigned int i = 0; i < fileLists.size(); ++i) {
    if (!i)
      fFileManagers.push_back(new IO::FileManager);
    else
      fFileManagers.push_back(new IO::FileManager(DontDumpSettings)); // dump settings only on first
    fFileManagers[i]->DisablePropagateFirstAndLastRunTime();
    fFileManagers[i]->ReadFileList(fileLists[i]);
    fEntries += fFileManagers[i]->GetEntries();
    fStreamTags.push_back(fFileManagers[i]->GetCurrentFileName().substr(fFileManagers[i]->GetCurrentFileName().size() - 8, 3));

    TTimeStamp timeOfFirstEvent, timeOfLastEvent;
    fFileManagers[i]->RetrieveFirstAndLastEventTimeForACQtFiles(timeOfFirstEvent, timeOfLastEvent);
    if (minimumTime == TTimeStamp(0, 0) || timeOfFirstEvent < minimumTime)
      minimumTime = timeOfFirstEvent;
    if (maximumTime == TTimeStamp(0, 0) || timeOfLastEvent > maximumTime)
      maximumTime = timeOfLastEvent;

    INFO_OUT << "Found " << fFileManagers[i]->GetEntries() << " events in '" << fStreamTags[i] << "' stream." << std::endl;
    INFO_OUT << "First event time: " << timeOfFirstEvent.AsString() << " in '" << fStreamTags[i] << "' stream." << std::endl;
    INFO_OUT << "Last event time: " << timeOfLastEvent.AsString() << " in '" << fStreamTags[i] << "' stream." << std::endl;
  }

  INFO_OUT << "Combined first event time: " << minimumTime.AsString() << std::endl;
  INFO_OUT << "Combined last event time: " << maximumTime.AsString() << std::endl;
  Utilities::TimeHistogramManager::instance()->SetFirstAndLastEventTimes(minimumTime, maximumTime);

  return true;
}

void MultiFileManager::DumpEventLoopProgress(unsigned int dumpAllNEvents, char separator) {

  if (fCurrentFileManager >= 0) 
    fFileManagers[fCurrentFileManager]->DumpEventLoopProgress(dumpAllNEvents,separator);
}

void MultiFileManager::Reset() {

  fEntries = 0;
  fCurrentEventNumber = 0;
  for (unsigned int i = 0; i < fFileManagers.size(); ++i)
    fFileManagers[i]->Reset();
}

bool MultiFileManager::GetNextEvent() { 

  if (fCurrentFileManager == -1) {
    for (unsigned int i = 0; i < fFileManagers.size(); ++i) { 
      if ( !fFileManagers[i]->GetNextEvent() ) { // no events for this FileManager - remove from List:
        delete fFileManagers[i];
        fFileManagers.erase(fFileManagers.begin() + i);
      } else
        ++fCurrentEventNumber;
    }
  } else {
    int OldEvent = fFileManagers[fCurrentFileManager]->Event()->EventHeader().Event();
    int OldRun   = fFileManagers[fCurrentFileManager]->Event()->RunHeader()->Run();

    if ( !fFileManagers[fCurrentFileManager]->GetNextEvent() ) { // no events for this FileManager - remove from List
      std::cout << fStreamTags[fCurrentFileManager] << " ";
      fFileManagers[fCurrentFileManager]->DumpEventLoopProgress(1, '\n'); 
      delete fFileManagers[fCurrentFileManager];
      fFileManagers.erase(fFileManagers.begin() + fCurrentFileManager);
    } else {
      int nSkip = 0;
      while(    fFileManagers[fCurrentFileManager]->Event()->RunHeader()->Run()    == OldRun
             && fFileManagers[fCurrentFileManager]->Event()->EventHeader().Event() <= OldEvent ) {

        WARN_OUT << "Skipping event " << fFileManagers[fCurrentFileManager]->Event()->EventHeader().Event()
                 << " of run " << fFileManagers[fCurrentFileManager]->Event()->RunHeader()->Run() << std::endl;

        ++nSkip;
        if ( !fFileManagers[fCurrentFileManager]->GetNextEvent() ) { // no events for this FileManager - remove from List
          std::cout << fStreamTags[fCurrentFileManager] << " ";
          fFileManagers[fCurrentFileManager]->DumpEventLoopProgress(1, '\n'); 
          delete fFileManagers[fCurrentFileManager];
          fFileManagers.erase(fFileManagers.begin() + fCurrentFileManager);
          break;
        }
      }

      if( nSkip )  WARN_OUT << "Negative event number jump in file " << fFileManagers[fCurrentFileManager]->GetCurrentFileName()  
                            << " Skipped " << nSkip << " events." << std::endl;

      ++fCurrentEventNumber;
    }
  }

  if (!fFileManagers.size()) { // all FileManagers finished
    fCurrentFileManager = -1;
    return false; 
  }

  fCurrentFileManager = 0;
  int minRun   = fFileManagers[0]->Event()->RunHeader()->Run();
  int minEvent = fFileManagers[0]->Event()->EventHeader().Event();
  for (unsigned int i = 1; i < fFileManagers.size(); ++i) {
    if (fFileManagers[i]->Event()->RunHeader()->Run() > minRun)   
      continue;

    if (fFileManagers[i]->Event()->RunHeader()->Run() < minRun) {
      minRun   = fFileManagers[i]->Event()->RunHeader()->Run();
      minEvent = fFileManagers[i]->Event()->EventHeader().Event();
      fCurrentFileManager = i;
      continue;
    }

    assert(fFileManagers[i]->Event()->RunHeader()->Run() == minRun);
    if (fFileManagers[i]->Event()->EventHeader().Event() < minEvent) {
      minEvent = fFileManagers[i]->Event()->EventHeader().Event();
      fCurrentFileManager = i;
    }
  }

  return true;
}

AC::Event* MultiFileManager::Event() const {

  assert(fCurrentFileManager >= 0);
  assert(fCurrentFileManager < (int)fFileManagers.size());
  return fFileManagers[fCurrentFileManager]->Event(); 
}

AC::RunHeader* MultiFileManager::RunHeader() const {

  assert(fCurrentFileManager >= 0);
  assert(fCurrentFileManager < (int)fFileManagers.size());
  return fFileManagers[fCurrentFileManager]->RunHeader(); 
}

std::string MultiFileManager::GetCurrentFileName() const {

  assert(fCurrentFileManager >= 0);
  assert(fCurrentFileManager < (int)fFileManagers.size());
  return fFileManagers[fCurrentFileManager]->GetCurrentFileName();
}

}

}
