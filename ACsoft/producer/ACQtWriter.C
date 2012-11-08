#include "ACQtWriter.h"
#include "RunHeader.h"
#include "Event.h"

#include <QByteArray>
#include <QDataStream>
#include <QFile>

static const quint32 gACQtChunkSizeInMegaByte = 1024 * 1024 * 5;
static const quint32 gACQtIdentifier = 0xACBEEFAC;
static const quint32 gACQtVersion = 5;
static const int gACQtCompressionLevel = -1; // Use zlib default compression level (6 at the time of writing).

static inline void writeChunkToFile( QDataStream& fileStream, QByteArray& chunk, qint64 position, quint32 events ) {

  Q_ASSERT(position < gACQtChunkSizeInMegaByte);
  if (position >= gACQtChunkSizeInMegaByte)
    std::cout << "FATAL ERROR! Chunk size is too small. The next chunk will be corrupt, resulting in a Z_BUF_ERROR when reading." << std::endl;
  chunk.resize(position);

  QByteArray compressedData = qCompress(chunk, gACQtCompressionLevel);
  fileStream << events << (qint64) compressedData.size();
  fileStream.device()->write(compressedData);
}

void ACQtWriter::Initialize( const QString& rootFileName ) {

  // ACQt File Format
  // -------------------------------------
  // uint32: magic
  // uint32: version
  // uint32: number-of-events
  // [uncompressed-run-header-data]
  // uint32: number-of-events-in-chunk
  // int64: size-of-next-chunk
  // [zlib-compressed-chunk-of-xxx-events]
  // uint32: number-of-events-in-chunk
  // int64: size-of-next-chunk
  // ...

  Q_ASSERT(rootFileName.endsWith(".root"));
  QString acqtFile = rootFileName;
  acqtFile.replace(".root", ".acqt");
  fFile = new QFile(acqtFile);
  fFile->open(QIODevice::WriteOnly);

  // Write ACQt header.
  fFileStream = new QDataStream(fFile);
  fFileStream->setVersion(QDataStream::Qt_4_6);
  fFileStream->setFloatingPointPrecision(QDataStream::SinglePrecision);
  *fFileStream << gACQtIdentifier;
  *fFileStream << gACQtVersion;

  fEventNumberPosition = fFile->pos();
  *fFileStream << (quint32) 0;

  fChunk = new QByteArray(gACQtChunkSizeInMegaByte, '0');
  fChunkStream = new QDataStream(fChunk, QIODevice::WriteOnly);
  fChunkStream->setVersion(QDataStream::Qt_4_6);
  fChunkStream->setFloatingPointPrecision(QDataStream::SinglePrecision);
}

void ACQtWriter::WriteRunHeader( const AC::RunHeader& runHeader ) {

  Q_ASSERT(fFile);
  fRunHeader = runHeader;
  fRunHeaderPosition = fFile->pos();
  *fFileStream << fRunHeader;
}

void ACQtWriter::WriteEvent( const AC::Event& event ) {

  Q_ASSERT(fFile);
  QIODevice* chunkDevice = fChunkStream->device();
  *fChunkStream << event;
  ++fEvents;
  ++fEventsInChunk;

  Q_ASSERT(!fChunkStream->atEnd());
  qint64 position = chunkDevice->pos();
  if (position <= 0.99 * gACQtChunkSizeInMegaByte)
    return;

  writeChunkToFile(*fFileStream, *fChunk, position, fEventsInChunk);
  fEventsInAllChunks += fEventsInChunk;
  fEventsInChunk = 0;

  fChunk->resize(gACQtChunkSizeInMegaByte);
  fChunk->fill('0');
  chunkDevice->seek(0);
}

void ACQtWriter::Finish(const TTimeStamp& firstEventTime, const TTimeStamp& lastEventTime) {

  Q_ASSERT(fFile);
  writeChunkToFile(*fFileStream, *fChunk, fChunkStream->device()->pos(), fEventsInChunk);
  fEventsInAllChunks += fEventsInChunk;

  // Seek back to the beginning to write the actual number of events written.
  fFile->flush();
  fFile->seek(fEventNumberPosition);
  *fFileStream << (quint32) fEvents;

  // Update the first & last event time in the run header
  fRunHeader.fFirstEventTimeStamp = firstEventTime;
  fRunHeader.fLastEventTimeStamp = lastEventTime;
  fFile->flush();
  fFile->seek(fRunHeaderPosition);
  *fFileStream << fRunHeader;

  fFile->close();

  Q_ASSERT(fEventsInAllChunks == fEvents);

  fEvents = 0;
  fEventsInChunk = 0;
  fEventsInAllChunks = 0;
  fEventNumberPosition = 0;
  fRunHeaderPosition = 0;
  fRunHeader = AC::RunHeader();
  delete fChunkStream;
  fChunkStream = 0;
  delete fChunk;
  fChunk = 0;

  delete fFileStream;
  fFileStream = 0;
  delete fFile;
  fFile = 0;
}
