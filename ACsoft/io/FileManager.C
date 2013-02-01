#include "FileManager.hh"

#include <zlib.h>
#include <fstream>

#include <TChain.h>
#include <TList.h>
#include <TSystem.h>
#include <TStopwatch.h>
#include <QDataStream>
#include <QFile>
#include <QProcessEnvironment>

#include "Event.h"
#include "ReducedEvent.hh"
#include "ReducedFileHeader.hh"
#include "Settings.h"
#include "TimeHistogramManager.hh"
#include "TrdPdfLookup.hh"
#include "VectorCapacityTracker.h"

#define DEBUG 0
#define INFO_OUT_TAG "FileManager> "
#include <debugging.hh>

namespace ACsoft {

namespace IO {

FileManager::FileManager(bool shouldDumpSettings)
  : fShouldDumpSettings(shouldDumpSettings)
  , fCurrentEventNumber(-1)
  , fEntries(0)
  , fRunTimer(0)
  , fEventTimer(0)
  , fAccumulatedEventRealTime(0)
  , fEventBunchSize(1e4)
  , fNumberOfProcessedBunches(0)
  , fAccumulatedRealTime(0)
  , fAccumulatedCPUTime(0)
  , fNumberOfProcessedRuns(0)
  , fCurrentFile(0)
  , fNumberOfFiles(0)
  , fRunHeader(0)
  , fEvent(0)
  , fCurrentACQtFile(0)
  , fChunkBufferSize(0)
  , fReducedEvent(0)
  , fReducedFileHeader(0)
  , fChain(0)
  , fCurrentRootFile(0)
  , fRun(-1)
  , fCustomChain(0) {
}

FileManager::~FileManager() {

  // Print warnings if the vector capacities were exceeded. Tunings these parameters will yield a better performance.
  if (AC::isVectorBenchmarkEnabled())
    AC::VectorCapacityTracker::Self()->reportCapacityExceededWarnings();

  DestructACQtFiles();
  DestructACROOTFiles();
  if (fRunTimer) {
    delete fRunTimer;
    fRunTimer = 0;
  }
  if (fEventTimer) {
    delete fEventTimer;
    fEventTimer = 0;
  }
}

static inline bool IsACQtFile(const std::string& name) {

  return TString(name).EndsWith(".acqt");
}

static inline bool IsROOTFile(const std::string& name) {

  return TString(name).EndsWith(".root");
}

bool FileManager::ReadFileList(const std::string& fileList, std::string customlist) {

  std::ifstream list(fileList.c_str());
  if (!list.good()) {
    WARN_OUT << "ERROR opening file \"" << fileList << "\"!" << std::endl;
    return false;
  }

  Q_ASSERT(fCurrentEventNumber == -1);
  Q_ASSERT(!fEvent);

  std::vector<std::string> acqtFiles;
  std::vector<std::string> rootFiles;
  std::string line;
  while (list.good()) {
    std::getline(list, line);
    if (list.eof())
      break;

    if (IsACQtFile(line))
      acqtFiles.push_back(line);
    else if (IsROOTFile(line))
      rootFiles.push_back(line);
  }

  if (acqtFiles.empty() && rootFiles.empty()) {
    WARN_OUT << "File list does not contain any valid *.acqt / *.root files!" << std::endl;
    return false;
  }

  if (!acqtFiles.empty() && !rootFiles.empty()) {
    WARN_OUT << "File list contains a mix of *.acqt / *.root files, this is not supported! You have to load one kind of files per FileManager." << std::endl;
    return false;
  }

  std::vector<std::string> rootFilesCustom;
  if (!customlist.empty()) {
    if (!acqtFiles.empty()) {
      WARN_OUT << "It makes no sense to specify a list of ROOT files with custom branches, when processing ACQt files. Aborting.";
      return false;
    }

    if (rootFiles.empty()) {
      WARN_OUT << "It makes no sense to specify a list of ROOT files with custom branches, and no ACROOT files. Aborting.";
      return false;
    }

    std::ifstream list(customlist.c_str());
    if (!list.good()) {
      WARN_OUT << "ERROR opening file \"" << fileList << "\"!" << std::endl;
      return false;
    }

    while (list.good()) {
      std::getline(list, line);
      if (list.eof())
        break;

      if (IsROOTFile(line))
        rootFilesCustom.push_back(line);
    }
  }

  if (acqtFiles.empty())
    return InitializeACROOTFiles(rootFiles, rootFilesCustom);
  return InitializeACQtFiles(acqtFiles);
}

bool FileManager::ReadFile(const std::string& file) {

  Q_ASSERT(fCurrentEventNumber == -1);
  Q_ASSERT(!fEvent);

  std::vector<std::string> files;
  files.push_back(file);

  if (IsACQtFile(file))
    return InitializeACQtFiles(files);
  if (IsROOTFile(file)) {
    std::vector<std::string> customfiles;
    return InitializeACROOTFiles(files, customfiles);
  }

  return false;
}

void FileManager::DumpEventLoopProgress(unsigned int dumpAllNEvents, char separator) {

  if (fCurrentEventNumber == 1 || (!(fCurrentEventNumber % dumpAllNEvents)) ) {
    std::cout << "Processed Events: " << std::setw(8) << fCurrentEventNumber
              << " (" << std::setw(3) << TMath::Nint(float(fCurrentEventNumber) / float(fEntries) * 100.0f) << "%)"
              << " Processed Files: " << std::setw(8) << fCurrentFile
              << " (" << std::setw(3) << TMath::Nint(float(fCurrentFile) / float(fNumberOfFiles) * 100.0f) << "%)";
    
    if (fNumberOfProcessedRuns > 0) {
      std::printf(" Mean real time per run: %5.2f [s]", fAccumulatedRealTime / fNumberOfProcessedRuns);
      std::printf(" Mean CPU time per run: %5.2f [s]", fAccumulatedCPUTime / fNumberOfProcessedRuns);
    }
    if (fNumberOfProcessedBunches > 0) {
      const int nMaxEvents = GetEntries();
      const int nEventsLeft = nMaxEvents - fCurrentEventNumber;
      const int totalSecondsLeft = (fAccumulatedEventRealTime * nEventsLeft / fNumberOfProcessedBunches) / fEventBunchSize;
      const int hh = floor(totalSecondsLeft / 60 / 60);
      const int mm = floor(totalSecondsLeft / 60 % 60);
      const int ss = floor(totalSecondsLeft % 60);
      std::printf(" ETE: %3d:%02d:%02d", hh, mm, ss);
    }
    std::cout << separator << std::flush;
  }
}

void FileManager::ExpandEnvironmentVariables(std::string& string) {

  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
  AC::TableStream::findAndReplace(string, "${ACROOTSOFTWARE}", environment.value("ACROOTSOFTWARE").toStdString());
}

bool FileManager::DoesFileExist(const std::string& file) {

  TString basename(gSystem->BaseName(file.c_str()));
  if (gSystem->FindFile(gSystem->DirName(file.c_str()), basename))
    return true;

  WARN_OUT << "File \"" << gSystem->BaseName(file.c_str()) << "\" not found in directory "
           << gSystem->DirName(file.c_str()) << " !" << std::endl;

  return false;
}

void FileManager::DumpSettings() {

  bool  addNearTrackHits = Utilities::TrdPdfLookup::Self()->AddNearTrackHitsForCandidateMatching();

  std::cout << std::endl;
  std::cout << "==============================================================================================="  << std::endl;
  std::cout << "||                                                                                           ||" << std::endl;

  std::string alignmentFile = ::AC::Settings::gTrdQtAlignmentFileName;
  std::string gainFile = ::AC::Settings::gTrdQtGainFileName;
  std::string pdfFile = ::AC::Settings::gTrdQtPdfFileName;
  std::string slowControlFile = ::AC::Settings::gTrdQtSlowControlFileName;
  if (fReducedEvent) {
    std::cout << "|| Lookup file settings read from ACROOT file:" << std::endl;
    // Use settings from ACROOT file, if possible.
    assert(fReducedEvent->FileHeader);
    alignmentFile = fReducedEvent->FileHeader->TrdAlignmentFileName(); 
    gainFile = fReducedEvent->FileHeader->TrdGainFileName(); 
    pdfFile = fReducedEvent->FileHeader->TrdPdfFileName(); 
    slowControlFile = fReducedEvent->FileHeader->TrdSlowControlFileName(); 
    addNearTrackHits = fReducedEvent->FileHeader->TrdQtPdfSettings()->fAddNearTrackHits;
  } else
    std::cout << "|| Lookup file settings for ACQt file: (equivalent to the hard-coded values in Settings.C)" << std::endl;
  
  std::cout << "||" << std::endl;
  std::cout << "|| Alignment-File:          " << alignmentFile << std::endl;
  std::cout << "|| Gain-File:               " << gainFile << std::endl;
  std::cout << "|| PDF-File:                " << pdfFile << std::endl;
  std::cout << "|| SlowControl-File:        " << slowControlFile << std::endl;
  std::cout << "|| TRD Add near track hits: " << addNearTrackHits << std::endl;
  std::cout << "||                                                                                           ||" << std::endl;
  std::cout << "==============================================================================================="  << std::endl;
  std::cout << std::endl;
}

bool FileManager::GetNextEvent() {

  if (fCurrentEventNumber == -1 && fShouldDumpSettings)
    DumpSettings();

  if (fReducedEvent)
    return GetNextACROOTEvent();
  return GetNextACQtEvent();
}

bool FileManager::GetEvent(int index) {

  if (index < 0 || index >= GetEntries())
    return false;

  if (fCurrentEventNumber == -1 && fShouldDumpSettings)
    DumpSettings();

  if (fReducedEvent)
    return GetACROOTEvent(index);
  return GetACQtEvent(index);
}

void FileManager::Reset() {

  fCurrentEventNumber = -1;
  ResetACQtFiles();
  ResetACROOTFiles();
}

bool FileManager::CheckFileIntegrities() const {

  // FIXME: Also verify ACROOT files.
  bool result = true;
  for (unsigned i = 0 ; i < fACQtFiles.size(); ++i) {
    if (!CheckACQtFileIntegrity(i))
      result = false;
  }

  return result;
}

std::string FileManager::MakeStandardRootFileName( std::string resultdir, std::string prefix, std::string suffix) {

  if( prefix == "" )
    WARN_OUT << "Empty prefix!" << std::endl;

  std::string outputFileName = "";
  if( resultdir != "" )
    outputFileName += ( resultdir + std::string("/") );
  outputFileName += prefix;
  if( suffix != "" )
  outputFileName += ( std::string("_") + suffix );
  outputFileName += std::string(".root");

  return outputFileName;
}

std::string FileManager::MakeStandardPdfFileName( std::string resultdir, std::string prefix, std::string suffix) {

  if( prefix == "" )
    WARN_OUT << "Empty prefix!" << std::endl;

  std::string outputFileName = "";
  if( resultdir != "" )
    outputFileName += ( resultdir + std::string("/") );
  outputFileName += prefix;
  if( suffix != "" )
  outputFileName += ( std::string("_") + suffix );
  outputFileName += std::string(".pdf");

  return outputFileName;
}

}

}
