#ifndef Serialization_h
#define Serialization_h

// FIXME: Eventually document this, not yet sure.
#ifndef __CINT__
#include <vector>
#include <QDataStream>
#include <QString>
#include "VectorCapacityTracker.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace ACsoft {

namespace AC {

// GenericReadArrayStream
template<typename DataType, size_t DataTypeSize>
class GenericReadArrayStream {
public:
  GenericReadArrayStream(QDataStream& stream, char* buffer, unsigned short size)
    : mBuffer(buffer) {

    stream.readRawData(mBuffer, size * DataTypeSize);
  }

private:
  char* mBuffer;
};

// GenericArrayStream
template<typename DataType, size_t DataTypeSize, unsigned short BufferSize>
class GenericArrayStream {
public:
  GenericArrayStream(QDataStream& stream)
    : mIndex(0) {

    stream.readRawData(mBuffer, BufferSize * DataTypeSize);
  }

  char* advance() {

    char* bytes = mBuffer + mIndex * DataTypeSize;
    ++mIndex;
    return bytes;
  }

private:
  unsigned short mIndex;
  char mBuffer[BufferSize * DataTypeSize];
};

// FloatArrayStream
template<unsigned short BufferSize>
class FloatArrayStream : public GenericArrayStream<float, 4, BufferSize> {
public:
  using GenericArrayStream<float, 4, BufferSize>::advance;

  FloatArrayStream(QDataStream& stream)
    : GenericArrayStream<float, 4, BufferSize>(stream) {

  }

  float read() {

    char* bytes = advance();

    union {
      float result;
      char bytes[4];
    } type;

    type.bytes[0] = bytes[3];
    type.bytes[1] = bytes[2];
    type.bytes[2] = bytes[1];
    type.bytes[3] = bytes[0];
    return type.result;
  }
};

// ShortArrayStream
template<unsigned short BufferSize>
class ShortArrayStream : public GenericArrayStream<qint16, 2, BufferSize> {
public:
  using GenericArrayStream<qint16, 2, BufferSize>::advance;

  ShortArrayStream(QDataStream& stream)
    : GenericArrayStream<qint16, 2, BufferSize>(stream) {

  }

  qint16 read() {

    char* bytes = advance();

    union {
      qint16 result;
      char bytes[2];
    } type;

    type.bytes[0] = bytes[1];
    type.bytes[1] = bytes[0];
    return type.result;
  }
};

// UCharArrayStream
template<unsigned short BufferSize>
class UCharArrayStream : public GenericArrayStream<quint8, 1, BufferSize> {
public:
  using GenericArrayStream<quint8, 1, BufferSize>::advance;

  UCharArrayStream(QDataStream& stream)
    : GenericArrayStream<quint8, 1, BufferSize>(stream) {

  }

  quint8 read() {

    char* bytes = advance();

    union {
      quint8 result;
      char bytes;
    } type;

    type.bytes = bytes[0];
    return type.result;
  }
};

// UShortArrayStream
template<unsigned short BufferSize>
class UShortArrayStream : public GenericArrayStream<quint16, 2, BufferSize> {
public:
  using GenericArrayStream<quint16, 2, BufferSize>::advance;

  UShortArrayStream(QDataStream& stream)
    : GenericArrayStream<quint16, 2, BufferSize>(stream) {

  }

  quint16 read() {

    char* bytes = advance();

    union {
      quint16 result;
      char bytes[2];
    } type;

    type.bytes[0] = bytes[1];
    type.bytes[1] = bytes[0];
    return type.result;
  }
};

// UIntArrayStream
template<unsigned short BufferSize>
class UIntArrayStream : public GenericArrayStream<quint32, 2, BufferSize> {
public:
  using GenericArrayStream<quint32, 2, BufferSize>::advance;

  UIntArrayStream(QDataStream& stream)
    : GenericArrayStream<quint32, 2, BufferSize>(stream) {

  }

  quint32 read() {

    char* bytes = advance();

    union {
      quint32 result;
      char bytes[2];
    } type;

    type.bytes[0] = bytes[1];
    type.bytes[1] = bytes[0];
    return type.result;
  }
};


// QDataStream operators
inline QDataStream& operator<<(QDataStream& stream, const std::string& object) {

  stream << QString::fromStdString(object);
  return stream;
}

inline QDataStream& operator<<(QDataStream& stream, const TTimeStamp& object) {

  stream << qint64(object.GetSec()) << qint64(object.GetNanoSec());
  return stream;
}

template<typename T, size_t inlineCapacity>
inline QDataStream& operator<<(QDataStream& stream, const Vector<T, inlineCapacity>& object) {

  quint32 size = object.size();
  stream << size;
  for( quint32 i=0 ; i<size ; ++i )
    stream << object[i];
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator<<(QDataStream& stream, const Vector<std::string, inlineCapacity>& object) {

  quint32 size = object.size();
  stream << size;
  for( quint32 i=0 ; i<size ; ++i )
    stream << QString::fromStdString(object[i]);
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator<<(QDataStream& stream, const Vector<char, inlineCapacity>& object) {

  quint32 size = object.size();
  stream << size;
  stream.writeRawData(object.data(), size);
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator<<(QDataStream& stream, const Vector<float, inlineCapacity>& object) {

  quint32 size = object.size();
  stream << size;
  stream.writeRawData(reinterpret_cast<const char*>(object.data()), size * sizeof(float));
  return stream;
}

template<typename T>
static inline T& objectPoolForType() {
  static T* gType = 0;
  if (!gType)
    gType = new T;
  return *gType;
}

inline QDataStream& operator>>(QDataStream& stream, std::string& object) {

  QString& t = objectPoolForType<QString>();
  stream >> t;
  object = t.toStdString();
  return stream;
}

inline QDataStream& operator>>(QDataStream& stream, TTimeStamp& object) {

  qint64 seconds = 0, nanoSeconds = 0;
  stream >> seconds >> nanoSeconds;
  object.SetSec(seconds);
  object.SetNanoSec(nanoSeconds);
  return stream;
}

template<typename T, size_t inlineCapacity>
inline QDataStream& operator>>(QDataStream& stream, Vector<T, inlineCapacity>& object) {

  quint32 size;
  stream >> size;
  checkInlineCapacity<T>(size, inlineCapacity);

  T& t = objectPoolForType<T>();
  object.resize(size);
  for( quint32 i=0; i<size; ++i ) {
    stream >> t;
    object[i] = t;
  }
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator>>(QDataStream& stream, Vector<std::string, inlineCapacity>& object) {

  quint32 size;
  stream >> size;
  checkInlineCapacity<std::string>(size, inlineCapacity);

  QString& t = objectPoolForType<QString>();
  object.resize(size);
  for( quint32 i=0; i<size; ++i ) {
    stream >> t;
    object[i] = t.toStdString();
  }
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator>>(QDataStream& stream, Vector<char, inlineCapacity>& object) {

  quint32 size;
  stream >> size;

  checkInlineCapacity<char>(size, inlineCapacity);
  object.resize(size);
  GenericReadArrayStream<char, 1> charStream(stream, object.data(), size);
  Q_UNUSED(charStream);
  return stream;
}

template<size_t inlineCapacity>
inline QDataStream& operator>>(QDataStream& stream, Vector<float, inlineCapacity>& object) {

  quint32 size;
  stream >> size;

  checkInlineCapacity<float>(size, inlineCapacity);
  object.resize(size);
  GenericReadArrayStream<float, 4> floatStream(stream, reinterpret_cast<char*>(object.data()), size);
  return stream;
}

}

}

#endif
#endif
#endif
