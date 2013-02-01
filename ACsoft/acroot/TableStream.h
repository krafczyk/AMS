#ifndef TableStream_h
#define TableStream_h

#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ostream>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// FIXME: Eventually document this, not yet sure.
namespace ACsoft {

namespace AC {

namespace TableStream {

// Per-row indentation
class indentation {
public:
  static void increment() { ++currentIndentation(); }
  static void decrement() { --currentIndentation(); }
  
  static std::string string() {

    std::stringstream stringStream;
    unsigned int indentation = currentIndentation();
    for (unsigned int i = 0; i < indentation; ++i)
      stringStream << " ";
    return stringStream.str();
  }

private:
  static unsigned int& currentIndentation() {

    static unsigned int gIndentation = 0;
    return gIndentation;
  }
};

// Title rows
class title_row {
public:
  title_row() { }

  title_row(const title_row& other) {

    fStream.str("");
    fStream << other.fStream.rdbuf();
    fColumnSizes = other.fColumnSizes;
  }

  std::stringstream& stream() { return fStream; }
  operator std::string() const { return fStream.str(); }

  unsigned int columns() const { return fColumnSizes.size(); }
  unsigned int sizeForColumn(unsigned int i) const { return fColumnSizes[i]; }
  void addColumn(unsigned int size) { fColumnSizes.push_back(size); }
 
private:
  friend class row;
  std::stringstream fStream;
  std::vector<unsigned int> fColumnSizes;
};

// Data rows
class row {
public:
  row()
    : fColumn(0) {

  }

  row(const title_row& other)
    : fColumn(0)
    , fColumnSizes(other.fColumnSizes) {

  }

  row(const row& other)
    : fColumn(other.fColumn)
    , fColumnSizes(other.fColumnSizes) {

    fStream.str("");
    fStream << other.fStream.rdbuf();
  }

  unsigned int advanceColumn() {
    
    assert(fColumnSizes.size());
    assert(fColumn <= fColumnSizes.size());
    unsigned int columnSize = fColumnSizes[fColumn];
    ++fColumn;
    return columnSize;
  }

  std::stringstream& stream() { return fStream; }
  operator std::string() const { return fStream.str(); }

private:
  std::stringstream fStream;
  unsigned int fColumn;
  std::vector<unsigned int> fColumnSizes;
};

// Columns
class column {
public:
  column(const char* name) {

    fStream << name;
  }

  column(const column& other) {

    fStream.str("");
    fStream << other.fStream.rdbuf();
  }

  operator std::string() const { return fStream.str(); }

private:
  std::stringstream fStream;
};

static inline void findAndReplace( std::string& source, const std::string& find, const std::string& replace ) {

  size_t j;
  for (; (j = source.find(find)) != std::string::npos;) {
    source.replace(j, find.length(), replace);
  }
}

inline title_row& operator<<( title_row& row, const column& column ) {

  std::string columnString = column;
  findAndReplace(columnString, "\t", "    ");
  row.addColumn(columnString.size());
  row.stream() << columnString << " | ";
  return row;
}

// Cells
class cell {
public:
  cell() { }

  cell(const cell& other) {

    fStream.str("");
    fStream << other.fStream.rdbuf();
  }

  std::stringstream& stream() { return fStream; }
  operator std::string() const { return fStream.str(); }

private:
  std::stringstream fStream;
};

template<typename T>
inline row& operator<<( row& row, const T& t) {

  unsigned int size = row.advanceColumn();
  row.stream() << std::setw(size) << t << " | ";
  return row;
}

template<>
inline row& operator<<( row& row, const TTimeStamp& t) {

  unsigned int size = row.advanceColumn();
  std::stringstream stream;
  stream << t.GetSec() << " [s] + " << t.GetNanoSec() << " [ns]";
  row.stream() << std::setw(size) << stream.str() << " | ";
  return row;
}

template<>
inline row& operator<<( row& row, const UChar_t& t) {

  unsigned int size = row.advanceColumn();
  row.stream() << std::setw(size) << (UInt_t) t << " | ";
  return row;
}

inline std::ostream& operator<<( std::ostream& stream, const UChar_t& t) {

  stream <<  (UInt_t) t;
  return stream;
}

template<typename T, size_t inlineCapacity>
inline row& operator<<( row& row, const Vector<T, inlineCapacity>& t) {

  std::stringstream cellStringStream;
  cellStringStream << "{";

  for (unsigned int i = 0; i < t.size(); ++i) {
    if (i > 0)
      cellStringStream << ", ";
    cellStringStream << t[i];
  }
  cellStringStream << "}";

  unsigned int size = row.advanceColumn();
  row.stream() << std::setw(size) << cellStringStream.str() << " | ";
  return row;
}

template<>
inline row& operator<<( row& row, const cell& cell ) {

  unsigned int size = row.advanceColumn();
  std::string cellString = cell;
  row.stream() << std::setw(size) << cellString << " | ";
  return row;
}

template<typename T>
inline cell operator<<( cell cell, const T& t ) {

  cell.stream() << t;
  return cell;
}

// Eventually implement this if cell formatting is needed for vectors.
template<typename T, size_t inlineCapacity>
inline cell operator<<( cell cell, const Vector<T, inlineCapacity>& ) { return cell; }

// OStream operators

inline std::ostream& operator<<( std::ostream& stream, const row& row ) {

  stream << std::left << (std::string) row;
  return stream;
}

inline std::ostream& operator<<( std::ostream& stream, const title_row& row ) {

  stream << std::left << (std::string) row << std::endl << indentation::string();

  unsigned int columns = row.columns();
  for (unsigned int column = 0; column < columns; ++column) {
    unsigned int columnSize = row.sizeForColumn(column);
    for (unsigned int i = 0; i <= columnSize; ++i)
      stream << "-";
    stream << "|";
    if (column != columns - 1)
      stream << "-";
  }

  return stream;
}

inline std::ostream& operator<<( std::ostream& stream, const Char_t& character) {

  stream << (Int_t) character;
  return stream;
}

}

}

using AC::TableStream::cell;
using AC::TableStream::column;
using AC::TableStream::indentation;
using AC::TableStream::row;
using AC::TableStream::title_row;

}

#endif

#endif
