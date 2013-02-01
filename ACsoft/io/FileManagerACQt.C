#include "FileManager.hh"

#include "Event.h"
#include "Settings.h"
#include "TimeHistogramManager.hh"

#include <zlib.h>
#include <QDataStream>
#include <QFile>
#include <TStopwatch.h>

#include <cmath>

#define DEBUG 0
#define INFO_OUT_TAG "FileManagerACQt> "
#include <debugging.hh>

namespace ACsoft {

namespace IO {

// Scratch buffer used to read chunks from ACQt files.
static inline QByteArray*& sharedInputByteArray() {

  static QByteArray* gByteArray = 0;
  if (!gByteArray)
    gByteArray = new QByteArray(1024 * 1024 * 5, '\0');

  return gByteArray;
}

// Scratch buffer used to hold the decompressed chunk data from ACQt files.
static inline QByteArray*& sharedOutputByteArray() {

  static QByteArray* gByteArray = 0;
  if (!gByteArray)
    gByteArray = new QByteArray(1024 * 1024 * 10, '\0');

  return gByteArray;
}

static inline QDataStream& sharedDataStream(QBuffer* buffer = 0) {

  static QDataStream* gDataStream = 0;
  if (buffer && gDataStream) {
    delete gDataStream;
    gDataStream = 0;
  }

  if (!gDataStream) {
    Q_ASSERT(buffer);
    if (!buffer)
      WARN_OUT << "FATAL ERROR in sharedDataStream()!" << std::endl;
    gDataStream = new QDataStream(buffer);
    gDataStream->setVersion(QDataStream::Qt_4_6);
    gDataStream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  }

  return *gDataStream;
}

void FileManager::DestructACQtFiles() {

  for (unsigned i = 0 ; i < fACQtFiles.size(); ++i) {
    delete fACQtFiles[i].file;
    delete fACQtFiles[i].header;
  }

  delete fEvent;

  fChunkBuffer.close();
}

void FileManager::RetrieveFirstAndLastEventTimeForACQtFiles() {

  Q_ASSERT(fEntries > 0);

  TTimeStamp timeOfFirstEvent;
  TTimeStamp timeOfLastEvent;
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
    // FIXME: This is uber-slow for ACQt files. It needs to read the _all_ entries until it reaches the end, that makes it horrible slow at the moment.
    GetEvent(GetEntries()-1);
    timeOfLastEvent = fEvent->EventHeader().TimeStamp();
    Reset();
  }

  INFO_OUT << "First event time: " << timeOfFirstEvent.AsString() << std::endl;
  INFO_OUT << "Last event time: " << timeOfLastEvent.AsString() << std::endl;

  delete fEvent;
  fEvent = 0;
  fRunHeader = 0;
  fCurrentEventNumber = -1;
  fChunkBufferSize = 0;
  if (fChunkBuffer.isOpen())
    fChunkBuffer.seek(0);

  Utilities::TimeHistogramManager::instance()->SetFirstAndLastEventTimes( timeOfFirstEvent, timeOfLastEvent );
  int nbins;
  double xmin, xmax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(nbins,xmin,xmax);
}

bool FileManager::InitializeACQtFiles( const std::vector<std::string>& files ) {

  unsigned int nFiles = 0;

  for (unsigned int i = 0; i < files.size(); ++i) {
    std::string file = files[i];
    ExpandEnvironmentVariables(file);
    if (!DoesFileExist(file))
      continue;

    INFO_OUT << "Adding file " << file << " to list of ACQt files." << std::endl;
    ACQtFile acQtFile;
    acQtFile.file = new QFile(file.c_str());
    fACQtFiles.push_back(acQtFile);
    ++nFiles;
  }
  fNumberOfFiles = nFiles;

  if( !nFiles ) {
    WARN_OUT << "No files added to list of ACQt files!" << std::endl;
    return false;
  }

  fChunkBuffer.setBuffer(sharedOutputByteArray());
  fChunkBuffer.open(QIODevice::ReadOnly);
  (void) sharedDataStream(&fChunkBuffer);

  fChunkBufferSize = 0;

  // FIXME: Add unit-tests for runs that failed to load!
  Q_ASSERT(!fEntries);
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

  if (version != 52 && version != 53 && version != 54) {
    WARN_OUT << "ERROR opening file \"" << fileName << "\"! Unknown version number '" << version << "'" << std::endl;
    return false;
  }

  quint32 entries;
  fileStream >> entries;

  file.header = new AC::RunHeader;
  file.entries = entries;
  file.version = version;
  fileStream >> *file.header;
  file.headerSize = file.file->pos();
  file.file->close();
  return true;
}

// ACQt event reading

// Just like qUncompress, but operating on shared buffers, avoiding any reallocations.
static inline bool decompressByteArray(QByteArray* inputBuffer, QByteArray* outputBuffer, qint64 inputSize, unsigned long& length) {

  unsigned char* inputData = reinterpret_cast<unsigned char*>(inputBuffer->data());
  Q_ASSERT(inputData);

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

  Q_ASSERT(false); // This can't be reached.
  return false;
}

bool NonInlineGetNextACQtDataChunk(FileManager* fileManager) {

  return fileManager->GetNextACQtDataChunk();
}

bool OpenACQtFileIfNeeded(const FileManager::ACQtFile& file, bool forceSeekToStart = false) {

  if (file.file->isOpen()) {
    // Seek to start of chunk data stream, right after the run header.
    if (forceSeekToStart) {
      bool result = file.file->seek(file.headerSize);
      Q_ASSERT(result);
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
  Q_ASSERT(result);
  Q_UNUSED(result);
  return true;
}

ALWAYS_INLINE bool FileManager::GetNextACQtDataChunk() {

  if (!fRunTimer) {
    fRunTimer = new TStopwatch;
    fRunTimer->Start();    
  }

  if (fCurrentACQtFile == fACQtFiles.size())
    return false;
  Q_ASSERT(fCurrentACQtFile < fACQtFiles.size());

  QFile* file = fACQtFiles[fCurrentACQtFile].file;
  if (fChunkBuffer.isOpen() && fChunkBuffer.pos() < (long) fChunkBufferSize) {
    Q_ASSERT(fRunHeader);
    return true;
  }

  if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile]))
    return false;

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
    Q_ASSERT(fCurrentACQtFile < fACQtFiles.size());
    if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile]))
      return false;

    file = fACQtFiles[fCurrentACQtFile].file;
    fileStream.setDevice(file);
  }

  fileStream.setVersion(QDataStream::Qt_4_6);
  fileStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

  // FIXME: Make use of eventsInChunk to speed-up ACQt random-access mode.
  quint32 eventsInChunk = 0;
  qint64 compressedChunkSize = 0;
  fileStream >> eventsInChunk >> compressedChunkSize;

  // FIXME: Automatically chose the right size.
  Q_ASSERT(compressedChunkSize < 1024 * 1024 * 5);
  if (compressedChunkSize >= 1024 * 1024 * 5) {
    WARN_OUT << "Compressed chunk size larger than 5MB in file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << ". Will skip this file and continue." << std::endl;
    file->seek(file->size());
    Q_ASSERT(fileStream.atEnd());
    return NonInlineGetNextACQtDataChunk(this);
  }

  QByteArray* inputBuffer = sharedInputByteArray();
  int readBytes = fileStream.readRawData(inputBuffer->data(), compressedChunkSize);
  if (readBytes != compressedChunkSize) {
    WARN_OUT << "Only read " << readBytes << " bytes instead of the expected " << compressedChunkSize << " bytes from file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << ". Will skip this file and continue." << std::endl;
    file->seek(file->size());
    Q_ASSERT(fileStream.atEnd());
    return NonInlineGetNextACQtDataChunk(this);
  }

  if (!decompressByteArray(inputBuffer, sharedOutputByteArray(), compressedChunkSize, fChunkBufferSize)) {
    WARN_OUT << "Decompressing chunk at position " << file->pos() << " in file " << fACQtFiles[fCurrentACQtFile].file->fileName().toStdString() << " failed. Input size: " << float(compressedChunkSize) / (1024.0f * 1024.0f) << "MB. Will skip this file and continue." << std::endl;
    file->seek(file->size());
    return NonInlineGetNextACQtDataChunk(this);
  }

  fChunkBuffer.seek(0);
  Q_ASSERT((long)fChunkBufferSize < sharedOutputByteArray()->size());

  fRunHeader = fACQtFiles[fCurrentACQtFile].header;

  // Always synchronize the ACQt version, so the streamers know which version they're supposed to read.
  ::AC::CurrentACQtVersion() = fACQtFiles[fCurrentACQtFile].version; 
  Q_ASSERT(fRunHeader);
  return true;
}

bool FileManager::GetNextACQtEvent() {

  if (!GetNextACQtDataChunk())
    return false;

  QDataStream& chunkStream = sharedDataStream();
  Q_ASSERT(fRunHeader);
  Q_ASSERT(!chunkStream.atEnd());
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

  chunkStream >> *fEvent;
  fEvent->SetRunHeader(fRunHeader);

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

  Q_ASSERT(acqtFile >= 0);
  Q_ASSERT(acqtFile <= (int) fACQtFiles.size()); /// \todo shouldn't this be '<' instead of '<=' ?

  int indexInFile = index - (entries - fACQtFiles[acqtFile].entries);
  Q_ASSERT(indexInFile >= 0);
  Q_ASSERT(indexInFile < (int)fACQtFiles[acqtFile].entries);

  if (fCurrentACQtFile < fACQtFiles.size()) {
    Q_ASSERT(fACQtFiles[fCurrentACQtFile].file);
    if (!OpenACQtFileIfNeeded(fACQtFiles[fCurrentACQtFile], true))
      return false;
    fChunkBuffer.seek(0);
    fChunkBufferSize = 0;
  }

  // FIXME: Optimize for the case where we the event lies in the current chunk stream.
  // There's no need to reopen the file and find the right chunk. For that we need to record
  // the number of events in a chunk, before the decoded chunk data, so that we can skip
  // those events if possible, without having to read all chunks before the desired event.
  fCurrentACQtFile = acqtFile;
  fCurrentFile = fCurrentACQtFile;
  Q_ASSERT(fACQtFiles[acqtFile].file);

  if (!OpenACQtFileIfNeeded(fACQtFiles[acqtFile], true))
    return false;

  fCurrentEventNumber = index - indexInFile - 1;
  bool result = false;
  for (int i = 0; i <= indexInFile; ++i) {
    result = GetNextEvent();
    if (!result)
      break;
  }

  if (!result) {
    WARN_OUT << "Coudln't seek to event " << indexInFile << " in file " << fACQtFiles[acqtFile].file->fileName().toStdString() << ". The file is broken! Aborting." << std::endl;
    return false;
  }

  Q_ASSERT(fEvent);
  Q_ASSERT(fRunHeader);
  Q_ASSERT(fCurrentEventNumber == index);
  return true;
}

void FileManager::ResetACQtFiles() {

  for (unsigned i = 0 ; i < fACQtFiles.size(); ++i) {
    QFile* file = fACQtFiles[i].file;
    if (!file || !file->isOpen())
      continue;
    file->seek(fACQtFiles[i].headerSize);
  }

  fRunHeader = 0;
  fCurrentACQtFile = 0;

  delete fEvent;
  fEvent = 0;
}

static inline void dumpSize(qint64 size) {

  printf("%2.2f", size / 1024.0f);
  std::cout << " KB (";
  printf("%2.2f", size / (1024.0f * 1024.0f));
  std::cout << " MB)";
}

bool FileManager::CheckACQtFileIntegrity(unsigned int fileNumber) const {

  if (fACQtFiles.empty() || fileNumber >= fACQtFiles.size())
    return false;

  QFile* file = fACQtFiles[fileNumber].file;
  std::cout << "Check file " << fileNumber + 1 << "/" << fACQtFiles.size() << ": " << file->fileName().toStdString() << " with size ";
  dumpSize(file->size());
  std::cout << " for integrity..." << std::endl;

  if (!OpenACQtFileIfNeeded(fACQtFiles[fileNumber], true)) {
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

    QByteArray* inputBuffer = sharedInputByteArray();
    int readBytes = fileStream.readRawData(inputBuffer->data(), compressedChunkSize);
    bool isChunkComplete = readBytes == compressedChunkSize;

    unsigned long uncompressedChunkSize = 0;
    bool isDecompressable = false;
    if (isChunkComplete)
      isDecompressable = decompressByteArray(inputBuffer, sharedOutputByteArray(), compressedChunkSize, uncompressedChunkSize);

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

}

}
