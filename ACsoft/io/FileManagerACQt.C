#include "FileManager.hh"

#include "Event.h"
#include "Settings.h"
#include "TimeHistogramManager.hh"
#include "DetectorManager.hh"

#include <zlib.h>
#include <QDataStream>
#include <QFile>
#include <TStopwatch.h>

#include <cmath>
#include <set>

#define DEBUG 0
#define INFO_OUT_TAG "FileManagerACQt> "
#include <debugging.hh>

namespace ACsoft {

namespace IO {

void FileManager::DestructACQtFiles() {

  for (unsigned i = 0 ; i < fACQtFiles.size(); ++i) {
    delete fACQtFiles[i].file;
    delete fACQtFiles[i].header;
  }

  delete fEvent;
  delete fDataStream;
  delete fByteArrayIn;
  delete fByteArrayOut;
  fChunkBuffer.close();
}

void FileManager::RetrieveFirstAndLastEventTimeForACQtFiles(TTimeStamp& timeOfFirstEvent, TTimeStamp& timeOfLastEvent) {

  unsigned int size = fACQtFiles.size();
  for (unsigned int i = 0; i < size; ++i) {
    if (!fACQtFiles[i].header || !fACQtFiles[i].entries)
      continue;
    timeOfFirstEvent = fACQtFiles[i].header->FirstEventTimeStamp();
    break;
  }

  for (int i = size - 1; i >= 0; --i) {
    if (!fACQtFiles[i].header || !fACQtFiles[i].entries)
      continue;
    timeOfLastEvent = fACQtFiles[i].header->LastEventTimeStamp();
    break;
  }

  if( timeOfFirstEvent == TTimeStamp(0,0) ){
    WARN_OUT << "Time of first event from run header is zero!" << std::endl;
    GetEvent(0);
    timeOfFirstEvent = fEvent->EventHeader().TimeStamp();
    Reset();
  }

  if( timeOfLastEvent == TTimeStamp(0,0) ){
    WARN_OUT << "Time of last event from run header is zero!" << std::endl;
    GetEvent(GetEntries()-1);
    timeOfLastEvent = fEvent->EventHeader().TimeStamp();
    Reset();
  }

  delete fEvent;
  fEvent = 0;
  fRunHeader = 0;
  fCurrentEventNumber = -1;
  fChunkBufferSize = 0;
  if (fChunkBuffer.isOpen())
    fChunkBuffer.seek(0);
}

void FileManager::RetrieveFirstAndLastEventTimeForACQtFiles() {

  TTimeStamp timeOfFirstEvent;
  TTimeStamp timeOfLastEvent;
  RetrieveFirstAndLastEventTimeForACQtFiles(timeOfFirstEvent, timeOfLastEvent);

  INFO_OUT << "First event time: " << timeOfFirstEvent.AsString() << std::endl;
  INFO_OUT << "Last event time: " << timeOfLastEvent.AsString() << std::endl;

  Utilities::TimeHistogramManager::instance()->SetFirstAndLastEventTimes(timeOfFirstEvent, timeOfLastEvent);
}

bool FileManager::InitializeACQtFiles( const std::vector<std::string>& files ) {

  if (!fByteArrayIn)
    fByteArrayIn = new QByteArray(1024 * 1024 * 5, '\0');   //  5 MB buffer to hold compressed ACQt data
  else
    fByteArrayIn->fill('\0');

  if (!fByteArrayOut)
    fByteArrayOut = new QByteArray(1024 * 1024 * 10, '\0'); // 10 MB buffer to hold uncompressed ACQt data
  else
    fByteArrayOut->fill('\0');

  unsigned int nFiles = 0;
  for (unsigned int i = 0; i < files.size(); ++i) {
    std::string file = files[i];
    ExpandEnvironmentVariables(file);
    if (!DoesFileExist(file))
      continue;

    ACQtFile acQtFile;
    acQtFile.file = new QFile(file.c_str());
    INFO_OUT << "Adding file " << file << " to list of ACQt files." << std::endl;
    fACQtFiles.push_back(acQtFile);
    ++nFiles;
  }
  fNumberOfFiles = nFiles;

  if( !nFiles ) {
    WARN_OUT << "No files added to list of ACQt files!" << std::endl;
    return false;
  }

  fChunkBuffer.setBuffer(fByteArrayOut);
  fChunkBuffer.open(QIODevice::ReadOnly);

  delete fDataStream;
  fDataStream = new QDataStream(&fChunkBuffer);
  fDataStream->setVersion(QDataStream::Qt_4_6);
  fDataStream->setFloatingPointPrecision(QDataStream::SinglePrecision);

  fChunkBufferSize = 0;

  // FIXME: Add unit-tests for runs that failed to load!
  assert(!fEntries);
  for (unsigned int i = 0; i < fACQtFiles.size(); ++i) {
    if (!OpenACQtFile(fACQtFiles[i]))
      continue;
    fEntries += fACQtFiles[i].entries;
  }

  if( GetEntries() <= 0){
    WARN_OUT << "No events found!" << std::endl;
    return false;
  }

  INFO_OUT << "Created ACQt file list with " << GetEntries() << " events from " << nFiles << " files." << std::endl;

  if (fPropagateFirstAndLastRunTime)
    RetrieveFirstAndLastEventTimeForACQtFiles();
  return true;
}

bool FileManager::OpenACQtFile(ACQtFile& file) {

  std::string fileName = file.file->fileName().toLatin1().data();
  if (!file.file->open(QIODevice::ReadOnly)) {
    WARN_OUT << "ERROR opening file \"" << fileName << "\"! File can NOT be opened." << std::endl;
    return false;
  }

  QDataStream fileStream(file.file);
  fileStream.setVersion(QDataStream::Qt_4_6);
  fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
  if (fileStream.atEnd())
    return false;

  quint32 magic;
  fileStream >> magic;
  if (magic != 0xACBEEFAC) {
    WARN_OUT << "ERROR opening file \"" << fileName << "\"! Corrupt magic number." << std::endl;
    return false;
  }

  quint32 version;
  fileStream >> version;
  if (version == 5) // Fix version type in ACQt 5.2.0 files.
    version = 52;

  if (version != 52 && version != 53 && version != 54 && version != 55 && version != 56 && version != 57 && version != 58) {
    WARN_OUT << "ERROR opening file \"" << fileName << "\"! Unknown version number '" << version << "'" << std::endl;
    return false;
  }

  quint32 entries;
  fileStream >> entries;

  file.header = new AC::RunHeader;
  file.entries = entries;
  file.version = version;

  // Be sure to set the ACQt version, so version conditionals in the AC::RunHeader aren't ignored.
  ::AC::CurrentACQtVersion() = version;
  fileStream >> *file.header;
  file.headerSize = file.file->pos();
  file.file->close();
  return true;
}

// ACQt event reading

// Just like qUncompress, but operating on shared buffers, avoiding any reallocations.
static inline bool decompressByteArray(QByteArray* inputBuffer, QByteArray* outputBuffer, qint64 inputSize, unsigned long& length) {

  unsigned char* inputData = reinterpret_cast<unsigned char*>(inputBuffer->data());
  assert(inputData);

  if (inputSize <= 4) {
    if (inputSize < 4 || (inputData[0]!=0 || inputData[1]!=0 || inputData[2]!=0 || inputData[3]!=0))
      qWarning("qUncompress: Input data is corrupted");
    length = 0;
    return true;
  }

  unsigned long expectedSize = (inputData[0] << 24) | (inputData[1] << 16) | (inputData[2] << 8) | inputData[3];
  length = qMax(expectedSize, 1ul);
  switch(::uncompress(reinterpret_cast<unsigned char*>(outputBuffer->data()), &length, inputData + 4, inputSize - 4)) {
  case Z_OK:
    return true;
  case Z_MEM_ERROR:
    WARN_OUT << "decompressByteArray: Z_MEM_ERROR: Not enough memory." << std::endl;
    return false;
  case Z_BUF_ERROR:
    WARN_OUT << "decompressByteArray: Z_BUF_ERROR: Not enough memory." << std::endl;
    return false;
  case Z_DATA_ERROR:
    WARN_OUT << "decompressByteArray: Z_DATA_ERROR: Not enough memory." << std::endl;
    return false;
  }

  assert(false); // This can't be reached.
  return false;
}

bool NonInlineGetNextACQtDataChunk(FileManager* fileManager) {

  return fileManager->GetNextACQtDataChunk();
}

bool OpenACQtFileIfNeeded(const FileManager::ACQtFile& file, std::string& databaseFileName, bool forceSeekToStart = false) {

  // Build DB file name for this run. Requires that the ACQt file name has an absolute path (/hpcwork/amsana/ACQT/ISS.../DATA/PWT/...).
  assert(file.header);
  QString fileName = file.file->fileName();
  QStringList tokens = fileName.split("/", QString::SkipEmptyParts);
  if (tokens.size() > 2) {
    QString streamName = tokens[tokens.size() - 2];
    fileName.replace(streamName, "DB");
    fileName.replace(".acqt", ".root");
    databaseFileName = fileName.toStdString();
  }

  if (file.file->isOpen()) {
    // Seek to start of chunk data stream, right after the run header.
    if (forceSeekToStart) {
      bool result = file.file->seek(file.headerSize);
      assert(result);
      Q_UNUSED(result);
    }
    return true;
  }

  if (!file.file->open(QIODevice::ReadOnly)) {
    WARN_OUT << "ERROR opening file \"" << file.file->fileName().toStdString() << "\"! File can NOT be opened." << std::endl;
    return false;
  }

  // Seek to start of chunk data stream, right after the run header.
  bool result = file.file->seek(file.headerSize);
  assert(result);
  Q_UNUSED(result);
  return true;
}

int FileManager::MoveToACQtDataChunkContainingEvent(unsigned int event) {

  assert(fCurrentACQtFile < fACQtFiles.size());
  QFile* file = fACQtFiles[fCurrentACQtFile].file;

  QDataStream fileStream(file);
  fileStream.setVersion(QDataStream::Qt_4_6);
  fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

  quint32 eventsInChunk = 0;
  qint64 compressedChunkSize = 0;
  unsigned long positionInFile = fACQtFiles[fCurrentACQtFile].headerSize, lastOffset = 0;
  unsigned long examinedEvents = 0;
  while (examinedEvents < event) {
    file->seek(positionInFile);
    fileStream >> eventsInChunk >> compressedChunkSize;

    examinedEvents += eventsInChunk;
    lastOffset = compressedChunkSize + sizeof(quint32) + sizeof(qint64);
    positionInFile += lastOffset;
  }

  file->seek(positionInFile - lastOffset);
  if (!GetNextACQtDataChunk())
    return -1;

  int distance = event - (examinedEvents - eventsInChunk);
  assert(distance >= 0);
  return distance;
}

ALWAYS_INLINE bool FileManager::GetNextACQtDataChunk() {

  if (!fRunTimer) {
    fRunTimer = new TStopwatch;
    fRunTimer->Start();
  }

  if (fCurrentACQtFile == fACQtFiles.size())
    return false;
  assert(fCurrentACQtFile < fACQtFiles.size());

  QFile* file = fACQtFiles[fCurrentACQtFile].file;
  if (fChunkBuffer.isOpen() && fChunkBuffer.pos() < (long) fChunkBufferSize) {
    assert(fRunHeader);
    return true;
  }

  std::string databaseFileName;
  if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile], databaseFileName))
    return false;
  LoadDatabase(databaseFileName);

  QDataStream fileStream(file);
  if (fileStream.atEnd()) {
    fRunTimer->Stop();
    fAccumulatedRealTime += fRunTimer->RealTime();
    fAccumulatedCPUTime += fRunTimer->CpuTime();
    ++fNumberOfProcessedRuns;
    fRunTimer->Start();

    ++fCurrentACQtFile;
    fCurrentFile = fCurrentACQtFile;
    if (fCurrentACQtFile == fACQtFiles.size())
      return false;

    file->close();
    assert(fCurrentACQtFile < fACQtFiles.size());
    if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile], databaseFileName))
      return false;
    LoadDatabase(databaseFileName);

    file = fACQtFiles[fCurrentACQtFile].file;
    fileStream.setDevice(file);
  }

  fileStream.setVersion(QDataStream::Qt_4_6);
  fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

  // This information is only needed for seeking in ACQt files.
  quint32 eventsInChunk = 0;
  qint64 compressedChunkSize = 0;
  fileStream >> eventsInChunk >> compressedChunkSize;
  assert(compressedChunkSize > 0);
  assert(compressedChunkSize < 1024 * 1024 * 5);

  // If a chunk is empty, skip the file. This only happens if there's only a single empty chunk in the file.
  if (!eventsInChunk) {
    file->seek(file->size());
    assert(fileStream.atEnd());
    WARN_OUT << "Empty chunk in file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << std::endl;
    return NonInlineGetNextACQtDataChunk(this);
  }

  QByteArray* inputBuffer = fByteArrayIn;
  int readBytes = fileStream.readRawData(inputBuffer->data(), compressedChunkSize);
  if (readBytes != compressedChunkSize) {
    WARN_OUT << "Only read " << readBytes << " bytes instead of the expected " << compressedChunkSize << " bytes from file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << ". Will skip this file and continue." << std::endl;
    file->seek(file->size());
    assert(fileStream.atEnd());
    fByteArrayIn->fill('\0');
    fByteArrayOut->fill('\0');
    fChunkBufferSize = 0;
    return NonInlineGetNextACQtDataChunk(this);
  }

  if (!decompressByteArray(inputBuffer, fByteArrayOut, compressedChunkSize, fChunkBufferSize)) {
    WARN_OUT << "Decompressing chunk at position " << file->pos() << " in file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << " failed. Input size: " << float(compressedChunkSize) / (1024.0f * 1024.0f) << "MB. Will skip this file and continue." << std::endl;
    file->seek(file->size());
    fByteArrayIn->fill('\0');
    fByteArrayOut->fill('\0');
    fChunkBufferSize = 0;
    return NonInlineGetNextACQtDataChunk(this);
  }

  fChunkBuffer.seek(0);
  assert((long)fChunkBufferSize < fByteArrayOut->size());

  fRunHeader = fACQtFiles[fCurrentACQtFile].header;

  // Always synchronize the ACQt version, so the streamers know which version they're supposed to read.
  ::AC::CurrentACQtVersion() = fACQtFiles[fCurrentACQtFile].version;
  assert(fRunHeader);
  return true;
}

std::string FileManager::GetCurrentACQtFileName() const {

  assert(fCurrentACQtFile < fACQtFiles.size());
  assert(fACQtFiles[fCurrentACQtFile].file);
  return fACQtFiles[fCurrentACQtFile].file->fileName().toStdString();
}

bool FileManager::GetNextACQtEvent() {

  if (!GetNextACQtDataChunk())
    return false;

  assert(fRunHeader);
  assert(fDataStream);
  assert(!fDataStream->atEnd());
  ++fCurrentEventNumber;

  if (!fEventTimer) {
    fEventTimer = new TStopwatch;
    fEventTimer->Start();
  }
  const unsigned int bunch = floor((double)fCurrentEventNumber / fEventBunchSize);
  if (fNumberOfProcessedBunches < bunch) {
    fEventTimer->Stop();
    fAccumulatedEventRealTime += fEventTimer->RealTime();
    fEventTimer->Start();
    ++fNumberOfProcessedBunches;
  }

  if (!fEvent)
    fEvent = new AC::Event;

  *fDataStream >> *fEvent;
  fEvent->SetRunHeader(fRunHeader);

  const TTimeStamp& eventTime = fEvent->EventHeader().TimeStamp();
  Detector::DetectorManager::Self()->UpdateIfNeeded(eventTime,fRunHeader->RunType());

  return true;
}

bool FileManager::GetACQtEvent(int index) {

  // Find out to which ACQtFile this index belongs.
  int entries = 0;
  int acqtFile = 0;
  for(; acqtFile < (int)fACQtFiles.size(); ++acqtFile) {
    entries += fACQtFiles[acqtFile].entries;
    if (index < entries)
      break;
  }

  assert(acqtFile >= 0);
  assert(acqtFile < (int) fACQtFiles.size());

  int indexInFile = index - (entries - fACQtFiles[acqtFile].entries);
  assert(indexInFile >= 0);
  assert(indexInFile < (int)fACQtFiles[acqtFile].entries);

  std::string databaseFileName;
  if (fCurrentACQtFile < fACQtFiles.size()) {
    assert(fACQtFiles[fCurrentACQtFile].file);
    if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile], databaseFileName, true))
      return false;
    LoadDatabase(databaseFileName);
    fChunkBuffer.seek(0);
    fChunkBufferSize = 0;
  }

  // FIXME: Optimize for the case where we the event lies in the current chunk stream.
  // There's no need to reopen the file and find the right chunk. For that we need to record
  // the number of events in a chunk, before the decoded chunk data, so that we can skip
  // those events if possible, without having to read all chunks before the desired event.
  fCurrentACQtFile = acqtFile;
  fCurrentFile = fCurrentACQtFile;
  assert(fACQtFiles[acqtFile].file);

  if (!OpenACQtFileIfNeeded(fACQtFiles[acqtFile], databaseFileName, true))
    return false;

  LoadDatabase(databaseFileName);
  fCurrentEventNumber = index - indexInFile - 1;
  bool result = false;

  int distanceToTargetEvent = MoveToACQtDataChunkContainingEvent(indexInFile);
  for (int i = 0; i <= distanceToTargetEvent; ++i) {
    result = GetNextEvent();
    if (!result)
      break;
  }

  if (!result) {
    WARN_OUT << "Coudln't seek to event " << indexInFile << " in file " << fACQtFiles[acqtFile].file->fileName().toStdString() << ". The file is broken! Aborting." << std::endl;
    return false;
  }

  assert(fEvent);
  assert(fRunHeader);
  fCurrentEventNumber = index;

  const TTimeStamp& eventTime = fEvent->EventHeader().TimeStamp();
  Detector::DetectorManager::Self()->UpdateIfNeeded(eventTime,fRunHeader->RunType());

  return true;
}

void FileManager::ResetACQtFiles() {

  for (unsigned i = 0 ; i < fACQtFiles.size(); ++i) {
    QFile* file = fACQtFiles[i].file;
    if (!file || !file->isOpen())
      continue;
    file->seek(fACQtFiles[i].headerSize);
  }

  fChunkBufferSize = 0;
  if (fChunkBuffer.isOpen())
    fChunkBuffer.seek(0);

  fRunHeader = 0;
  fCurrentACQtFile = 0;

  fByteArrayIn->fill('\0');
  fByteArrayOut->fill('\0');

  delete fEvent;
  fEvent = 0;
}

static inline void dumpSize(qint64 size) {

  printf("%2.2f", size / 1024.0f);
  std::cout << " KB (";
  printf("%2.2f", size / (1024.0f * 1024.0f));
  std::cout << " MB)";
}

bool FileManager::CheckACQtFileIntegrity(unsigned int fileNumber) const{

  if (fACQtFiles.empty() || fileNumber >= fACQtFiles.size())
    return false;

  QFile* file = fACQtFiles[fileNumber].file;
  std::cout << "Check file " << fileNumber + 1 << "/" << fACQtFiles.size() << ": " << file->fileName().toStdString() << " with size ";
  dumpSize(file->size());
  std::cout << " for integrity..." << std::endl;

  std::string databaseFileName;
  if (!OpenACQtFileIfNeeded(fACQtFiles[fileNumber], databaseFileName, true)) {
    WARN_OUT << " -> FATAL ERROR IN FILE " << file->fileName().toStdString() << "! Cannot read from file." << std::endl << std::endl;
    return false;
  }

  QDataStream fileStream(file);
  fileStream.setVersion(QDataStream::Qt_4_6);
  fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

  std::cout << "Chunk table:" << std::endl;
  unsigned int chunk = 0;
  unsigned int maxEvents = 0;
  while (!fileStream.atEnd()) {
    quint32 eventsInChunk = 0;
    qint64 compressedChunkSize = 0;
    fileStream >> eventsInChunk >> compressedChunkSize;
    maxEvents += eventsInChunk;

    QByteArray* inputBuffer = fByteArrayIn;
    int readBytes = fileStream.readRawData(inputBuffer->data(), compressedChunkSize);
    bool isChunkComplete = readBytes == compressedChunkSize;

    unsigned long uncompressedChunkSize = 0;
    bool isDecompressable = false;
    if (isChunkComplete)
      isDecompressable = decompressByteArray(inputBuffer, fByteArrayOut, compressedChunkSize, uncompressedChunkSize);

    std::cout << "Chunk " << std::setw(3) << chunk << " size: ";
    dumpSize(compressedChunkSize);
    std::cout << " events: " << std::setw(5) << eventsInChunk;
    std::cout << " is complete? " << isChunkComplete << " is decompressable? " << isDecompressable;
    std::cout << " uncompressed size: ";
    dumpSize(uncompressedChunkSize);
    std::cout << std::endl;

    if (!isChunkComplete || !isDecompressable) {
      WARN_OUT << " -> FATAL ERROR IN FILE " << file->fileName().toStdString() << "! CHUNK IS NOT DECOMPRESSABLE, OR COMPLETE!" << std::endl << std::endl;
      return false;
    }
    ++chunk;
  }

  // TODO: Extend this for other streams. For now we only make sure that the 2ecal stream is complete.
  if (file->fileName().contains("2ecal")) {
    unsigned int totalAMSEvents = fACQtFiles[fileNumber].header->TotalAMSEvents();
    if (totalAMSEvents != maxEvents) {
      WARN_OUT << " -> FATAL ERROR IN FILE " << file->fileName().toStdString() << "! AMS FILE HAS "
               << totalAMSEvents << " EVENTS, WE ONLY HAVE " << maxEvents << " EVENTS!" << std::endl << std::endl;
      return false;
    }
  }

  file->close();

  INFO_OUT << "File is valid." << std::endl << std::endl;
  return true;
}

int FileManager::GetNumberOfACQtRuns() {

  std::set<int> runs;
  for (unsigned int i = 0; i < fACQtFiles.size(); ++i)
    runs.insert(fACQtFiles[i].header->Run());
  return runs.size();
}

int FileManager::GetCurrentACQtRunIndex() {

  int oldrun = fACQtFiles[0].header->Run();
  int count = 0;
  for (unsigned int i = 0; i<=fCurrentACQtFile;i++) {
    if (fACQtFiles[i].header->Run() != oldrun) {
      oldrun = fACQtFiles[i].header->Run();
      count++;
    }
  }
  return count;
}

void FileManager::SkipCurrentACQtRun() {

  assert(fCurrentACQtFile < fACQtFiles.size());

  Long64_t entries = 0;
  int run = fACQtFiles[fCurrentACQtFile].header->Run();
  for (unsigned int i = 0; i <= fCurrentACQtFile; ++i)
    entries += fACQtFiles[i].entries;

  // check if next files belong to current run
  int offset = 1;
  while ((fCurrentACQtFile + offset != fACQtFiles.size()) && (fACQtFiles[fCurrentACQtFile + offset].header->Run() == run)) {
      entries += fACQtFiles[fCurrentACQtFile + offset].entries;
      ++offset;
  }
  assert(GetEvent(entries - 1));
  assert(GetCurrentEventIndex() == (entries - 1));
}

void FileManager::SkipBackACQtRun() {

  Long64_t entries = 0;
  int run = fACQtFiles[fCurrentACQtFile].header->Run();

  // Check if previous file(s) in the list, belongs to the current run as well, if so, skip it.
  int fileindex = fCurrentACQtFile - 1;
  for (int i = fileindex; i >= 0; i++) {
    if (fACQtFiles[i].header->Run() != run)
      break;
    --fileindex;
  }

  // Count all entries in the chain until the beginning of the current run.
  for (int j = 0; j <= fileindex; ++j)
    entries += fACQtFiles[j].entries;

  // If there are no entries, we're already at the beginning of the file list: early exit and reset state.
  if (!entries) {
    Reset();
    return;
  }

  // If we found a new position, check that it's different from the current one. If so go ahead and we're done.
  if (entries != GetCurrentEventIndex()) {
    assert(GetEvent(entries - 1));
    assert(GetCurrentEventIndex() == (entries - 1));
    return;
  }

  // If SkipBackACQtRun() was called at the beginning of the current run, we reach this code.
  // Skip back another run, otherwhise this function would be a no-op.
  entries -= fACQtFiles[fCurrentACQtFile - 1].entries;
  int oldrun = fACQtFiles[fCurrentACQtFile - 1].header->Run();

  if (fCurrentACQtFile - 1 > 0) {
    int offset = 2;
    while ((entries > 0) && (oldrun == fACQtFiles[fCurrentACQtFile - offset].header->Run())) {
      entries -= fACQtFiles[fCurrentACQtFile - offset].entries;
      ++offset;
    }
  }

  if (!entries) {
    Reset();
    return;
  }

  assert(GetEvent(entries - 1));
  assert(GetCurrentEventIndex() == (entries - 1));
}

}

}
