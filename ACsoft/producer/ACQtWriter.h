#ifndef ACQtWriter_h
#define ACQtWriter_h

#include <QString>
#include <TTimeStamp.h>
#include "RunHeader.h"

class QByteArray;
class QDataStream;
class QFile;

namespace AC {
  class Event;
  class RunHeader;
}

/** Produces ACQt files from a stream of Events.
  */
class ACQtWriter {
public:
  ACQtWriter()
    : fFile(0)
    , fFileStream(0)
    , fChunk(0)
    , fChunkStream(0)
    , fEvents(0)
    , fEventsInChunk(0)
    , fEventsInAllChunks(0)
    , fEventNumberPosition(0)
    , fRunHeaderPosition(0) {

  }
 
  /** Initializes the producer.
    */
  void Initialize(const QString& rootFileName);

  /** Write RunHeader to file
    */
  void WriteRunHeader(const AC::RunHeader&);

  /** Write Event to file
    */
  void WriteEvent(const AC::Event&);

  /** Finish writing file
    */
  void Finish(const TTimeStamp& firstEventTime, const TTimeStamp& lastEventTime);

private:
  QFile* fFile;
  QDataStream* fFileStream;
  QByteArray* fChunk;
  QDataStream* fChunkStream;
  unsigned int fEvents;
  unsigned int fEventsInChunk;
  unsigned int fEventsInAllChunks;
  unsigned long long fEventNumberPosition;
  unsigned long long fRunHeaderPosition;
  AC::RunHeader fRunHeader;
};

#endif
