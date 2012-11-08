#ifndef Tools_h
#define Tools_h

#include <platform/Vector.h>
#include <math.h>
#include <TObject.h>
#include <TMath.h>
#include <TTimeStamp.h>
#include "TableStream.h"
#include "Serialization.h"

namespace AC {

/** Helper structure.
  * Note: This struct has no associated streaming operators, as it's not
  * stored in files unlike PatternChargeAndError.
  */
struct ChargeAndError {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** Default constructor */
  ChargeAndError(Float_t _charge = 0, Float_t _error = -1)
    : charge(_charge)
    , error(_error) {

  }

  /** Charge */
  Float_t charge;

  /** Error */
  Float_t error;
};

/** Helper function to create a TTimeStamp from a unix time & time fraction in microsends */
static inline TTimeStamp ConstructTimeStamp(time_t unixTime, float timeFraction) {

  return TTimeStamp(unixTime, TMath::Nint(timeFraction * 1000.0));
}

}

// FIXME: Eventually document this, not yet sure.
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace AC {

class FieldDescriptionBase {
public:
  virtual ~FieldDescriptionBase() { }
  virtual bool isSplitTableMarker() const { return false; }
  virtual bool isSeperatedTableMarker() const { return false; }
  virtual const char* columnText() const { return ""; }
  virtual void printDataToTableRow(const void*, row&) { }
  virtual void printDataToStream(const void*, std::ostream&, unsigned int) { }
};

template<typename ClassType>
inline void writeSeperatedTablesToStream(std::ostream& stream, const ClassType* type, unsigned int index, const std::vector<FieldDescriptionBase*>& descriptors) {

  for (unsigned int i = 0; i < descriptors.size(); ++i) {
    Q_ASSERT(descriptors[i]->isSeperatedTableMarker());
    descriptors[i]->printDataToStream(type, stream, index);
  }
}

template<typename T, size_t inlineCapacity>
static inline void writeTableRangeToStream(std::ostream& stream, const Vector<T, inlineCapacity>& list, unsigned columnStart, unsigned columns) {

  title_row titleRow;
  titleRow << column("  #");
  T::printDataToTitleRow(titleRow, columnStart, columns);
  stream << indentation::string() << titleRow << std::endl;

  for (unsigned int i = 0; i < list.size(); ++i) {
    row row(titleRow);
    row << (cell() << i);
    list[i].printDataToTableRow(row, columnStart, columns);
    stream << indentation::string() << row << std::endl;
  }
}

template<typename T>
static inline void writeTableRangeToStream(std::ostream& stream, const T& object, unsigned columnStart, unsigned columns) {

  title_row titleRow;
  T::printDataToTitleRow(titleRow, columnStart, columns);
  stream << indentation::string() << titleRow << std::endl;

  row row(titleRow);
  object.printDataToTableRow(row, columnStart, columns);
  stream << indentation::string() << row << std::endl;
}

template<typename T, typename VectorType>
static inline void writeGenericVectorToStream(std::ostream& stream, const VectorType& list) {

  if (!list.size()) {
    stream << std::endl;
    return;
  }

  indentation::increment();
  std::vector<int> splitTable = T::SplitTableAfterColumns();

  unsigned int start = 0;
  for (unsigned int i = 0; i < splitTable.size(); ++i) {
    writeTableRangeToStream(stream, list, start, splitTable[i] - start);
    start = splitTable[i] + 1;
    stream << std::endl;
  }
  writeTableRangeToStream(stream, list, start, T::NumberOfDebugColumns() - start);
  stream << std::endl;

  std::vector<FieldDescriptionBase*> seperatedTableDescriptors;
  const std::vector<FieldDescriptionBase*>& descriptors = T::FieldDescriptions();
  for (unsigned int i = 0; i < descriptors.size(); ++i) { 
    if (!descriptors[i]->isSeperatedTableMarker())
      continue;
    seperatedTableDescriptors.push_back(descriptors[i]);
  }

  if (!seperatedTableDescriptors.empty()) {
    for (unsigned int i = 0; i < list.size(); ++i)
      writeSeperatedTablesToStream<T>(stream, &list[i], i, seperatedTableDescriptors); 
  }

  indentation::decrement();
}

template<typename T>
inline void writeObjectToStream( std::ostream& stream, const T& object ) {

  indentation::increment();
  std::vector<int> splitTable = T::SplitTableAfterColumns();
  
  unsigned int start = 0;
  for (unsigned int i = 0; i < splitTable.size(); ++i) {
    writeTableRangeToStream(stream, object, start, splitTable[i] - start);
    start = splitTable[i] + 1;
    stream << std::endl;
  }
  writeTableRangeToStream(stream, object, start, T::NumberOfDebugColumns() - start);
  stream << std::endl;

  indentation::decrement();
}
 
class FieldDescriptionSplitTable : public FieldDescriptionBase {
public:
  virtual ~FieldDescriptionSplitTable() { }
  virtual bool isSplitTableMarker() const { return true; }
};

template<typename T>
struct TypeTraits { typedef T ReturnType; };

template<>
struct TypeTraits<TTimeStamp> { typedef const TTimeStamp& ReturnType; };

template<typename T, size_t inlineCapacity>
struct TypeTraits<Vector<T, inlineCapacity> > { typedef const Vector<T, inlineCapacity>& ReturnType; };

template<typename ClassType, typename DataType>
class FieldDescription : public FieldDescriptionBase {
public:
  typedef typename TypeTraits<DataType>::ReturnType (ClassType::*Getter)() const;
  typedef typename TypeTraits<DataType>::ReturnType (ClassType::*ErrorGetter)() const;

  FieldDescription() : fColumnText(0), fGetter(0), fErrorGetter(0) { }
  FieldDescription(const char* text, Getter getter, ErrorGetter errorGetter) : fColumnText(text), fGetter(getter), fErrorGetter(errorGetter) { }

  virtual const char* columnText() const { return fColumnText; }

  virtual void printDataToTableRow(const void* maskedObject, row& stream) {

    const ClassType* object = reinterpret_cast<const ClassType*>(maskedObject);
    if (fErrorGetter)
      stream << (cell() << (object->*fGetter)() << " +/- " << (object->*fErrorGetter)());
    else
      stream << (object->*fGetter)();
  }

protected:
  const char* fColumnText;
  Getter fGetter;
  ErrorGetter fErrorGetter;
};
 
template<typename ClassType, typename DataType>
class FieldDescriptionSeperatedTable : public FieldDescription<ClassType, DataType> {
public:
  virtual ~FieldDescriptionSeperatedTable() { }
  virtual bool isSeperatedTableMarker() const { return true; }

  typedef typename TypeTraits<DataType>::ReturnType (ClassType::*Getter)() const;

  FieldDescriptionSeperatedTable(const char* text, Getter getter) : FieldDescription<ClassType, DataType>(text, getter, 0) { }

private:
  using FieldDescription<ClassType, DataType>::fGetter;

  virtual void printDataToStream(const void* maskedObject, std::ostream& stream, unsigned int index) {

    const ClassType* object = reinterpret_cast<const ClassType*>(maskedObject);
    indentation::increment();
    stream << indentation::string(); 
    stream << "Additional information for object " << index << ":" << std::endl;
    stream << (object->*fGetter)();
    indentation::decrement();
  }
};

// Helper macros for classes like ECALShower that want to dump a table of its data fields in the Dump() method.
#define BEGIN_DEBUG_TABLE(ClassName) \
  typedef ClassName LocalClassName; \
  \
  unsigned int ClassName::NumberOfDebugColumns() { \
    std::vector<FieldDescriptionBase*> descriptors = FieldDescriptions(); \
    for (unsigned int i = 0; i < descriptors.size(); ++i) { \
      if (descriptors[i]->isSeperatedTableMarker()) \
        return i; \
    } \
    return descriptors.size(); \
  } \
  \
  std::vector<int> ClassName::SplitTableAfterColumns() { \
    std::vector<int> result; \
    std::vector<FieldDescriptionBase*>& descriptors = FieldDescriptions(); \
    for (unsigned int i = 0; i < descriptors.size(); ++i) { \
      if (descriptors[i]->isSplitTableMarker()) \
        result.push_back(i); \
    } \
    return result; \
  } \
  \
  void ClassName::printDataToTableRow(row& stream, unsigned columnStart, unsigned columns) const { \
    std::vector<FieldDescriptionBase*>& descriptors = FieldDescriptions(); \
    for (unsigned int i = columnStart; i < columnStart + columns; ++i) \
      descriptors[i]->printDataToTableRow(this, stream); \
  } \
  \
  void ClassName::printDataToTitleRow(title_row& stream, unsigned columnStart, unsigned columns) { \
    std::vector<FieldDescriptionBase*>& descriptors = FieldDescriptions(); \
    for (unsigned int i = columnStart; i < columnStart + columns; ++i) \
      stream << descriptors[i]->columnText(); \
  } \
  \
  void ClassName::Dump() const { std::cout << *this; } \
  \
  std::vector<FieldDescriptionBase*>& ClassName::FieldDescriptions() { \
    static std::vector<FieldDescriptionBase*>* gFieldDescriptions = 0; \
    if (!gFieldDescriptions) { \
      gFieldDescriptions = new std::vector<FieldDescriptionBase*>; 

// A column in the table representing a value
#define COL(ColumnText, Type, Getter) \
  gFieldDescriptions->push_back(new FieldDescription<LocalClassName, Type>(ColumnText, &LocalClassName::Getter, 0));

// A column in the table representing a value and its error
#define COLE(ColumnText, Type, Getter) \
  gFieldDescriptions->push_back(new FieldDescription<LocalClassName, Type>(ColumnText, &LocalClassName::Getter, &LocalClassName::Getter##Error));

#define ADD_SEPERATED_DUMP(ColumnText, Type, Getter) \
  gFieldDescriptions->push_back(new FieldDescriptionSeperatedTable<LocalClassName, Type>(ColumnText, &LocalClassName::Getter));

#define NEWTABLE \
  gFieldDescriptions->push_back(new FieldDescriptionSplitTable);

#define END_DEBUG_TABLE } return *gFieldDescriptions; }

#define BEGIN_DEBUG_OUTPUT_ROOT(ClassName) \
  BEGIN_DEBUG_OUTPUT(ClassName)

#define BEGIN_DEBUG_OUTPUT(ClassName) \
  void ClassName::Dump() const { std::cout << *this; } \
  void ClassName::DumpToStream(std::ostream& stream) const {
 
#define DUMP_POD(Name, Getter) \
  indentation::increment(); \
  stream << indentation::string() << "[" << Getter() << " " Name "]" << std::endl; \
  indentation::decrement();

#define DUMP(Getter) stream << Getter();
#define END_DEBUG_OUTPUT }
#define END_DEBUG_OUTPUT_ROOT }

// These operators must be placed after the class definition.
#define ADD_TABLE_DUMP_OPERATORS(ClassName) \
  template<size_t inlineCapacity> \
  inline std::ostream& operator<<( std::ostream& stream, const Vector<ClassName, inlineCapacity>& object ) { \
    indentation::increment(); \
    stream << indentation::string() << "[" << object.size() << " " #ClassName << "]" << std::endl; \
    writeGenericVectorToStream<ClassName, Vector<ClassName, inlineCapacity> >(stream, object); \
    indentation::decrement(); \
    return stream; \
  } \
  \
  inline std::ostream& operator<<(std::ostream& stream, const ClassName& object) { \
    indentation::increment(); \
    stream << indentation::string() << "[" #ClassName "]" << std::endl; \
    writeObjectToStream<ClassName>(stream, object); \
    indentation::decrement(); \
    return stream; \
  }

#define ADD_DUMP_OPERATORS(ClassName) \
  inline std::ostream& operator<<(std::ostream& stream, const ClassName& object) { \
    indentation::increment(); \
    stream << indentation::string() << "[" #ClassName "]" << std::endl; \
    object.DumpToStream(stream); \
    indentation::decrement(); \
    return stream; \
  }

#define ADD_ACQT_OPERATOR_FRIENDS(ClassName) \
  friend QDataStream& operator<<(QDataStream&, const ClassName&); \
  friend QDataStream& operator>>(QDataStream&, ClassName&);

// These operators must be placed right in the class definition.
#define ADD_ACQT_OPERATORS(ClassName) \
  QDataStream& operator<<(QDataStream&, const ClassName&); \
  QDataStream& operator>>(QDataStream&, ClassName&);

#ifdef __CINT__
#define REGISTER_CLASS_WITH_TABLE(ClassName) \
  REGISTER_CLASS(ClassName)

#define REGISTER_CLASS(ClassName) \
  public:
#else
#define REGISTER_CLASS_WITH_TABLE(ClassName) \
  REGISTER_CLASS_SHARED(ClassName) \
  void printDataToTableRow(row& stream, unsigned columnStart, unsigned columns) const; \
  static void printDataToTitleRow(title_row& stream, unsigned columnStart, unsigned columns); \
  static unsigned int NumberOfDebugColumns(); \
  static std::vector<int> SplitTableAfterColumns(); \
  static std::vector<FieldDescriptionBase*>& FieldDescriptions(); \
  }; \
  ADD_ACQT_OPERATORS(ClassName) \
  ADD_TABLE_DUMP_OPERATORS(ClassName) \
  struct ClassName##Ignore {

#define REGISTER_CLASS_SHARED(ClassName) \
  public: \
  void DumpToStream(std::ostream&) const; \
  \
  ADD_ACQT_OPERATOR_FRIENDS(ClassName) \

#define REGISTER_CLASS(ClassName) \
  REGISTER_CLASS_SHARED(ClassName) \
  }; \
  ADD_ACQT_OPERATORS(ClassName) \
  ADD_DUMP_OPERATORS(ClassName) \
  struct ClassName##Ignore {

#endif

}

#endif
#endif
