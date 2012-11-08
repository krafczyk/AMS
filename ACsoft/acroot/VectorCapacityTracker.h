#ifndef CapacityCheck_h
#define CapacityCheck_h

// FIXME: Eventually document this, not yet sure.
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __CINT__
#include <typeinfo>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

#define PRETTY_PRINT_TYPE(x) do { typedef void(*Type)x; prettyPrintTypeString += PrettyPrintType<Type>(Type(), #x).name; } while(0)

template<typename T>
struct PrettyPrintType {

  template<typename U>
  PrettyPrintType(void(*)(U), const std::string&) {

    int status;
    name = abi::__cxa_demangle(typeid(U).name(), 0, 0, &status);
  }

  std::string name;
};

static inline std::string currentBacktrace() {

  std::string backtraceString;
  static const int framesToShow = 1;
  static const int framesToSkip = 2;
  void* samples[framesToShow + framesToSkip];
  int frames = backtrace(samples, framesToShow + framesToSkip);

  void** stack = samples + framesToSkip;
  int size = frames - framesToSkip;
  for (int i = 0; i < size; ++i) {
    const char* mangledName = 0;
    char* cxaDemangled = 0;

    Dl_info info;
    if (dladdr(stack[i], &info) && info.dli_sname)
        mangledName = info.dli_sname;
    if (mangledName)
        cxaDemangled = abi::__cxa_demangle(mangledName, 0, 0, 0);

    if (mangledName || cxaDemangled)
      backtraceString += cxaDemangled ? cxaDemangled : mangledName;

    free(cxaDemangled);
  }

  return backtraceString;
}
#else
#define PRETTY_PRINT_TYPE(x) 
#endif

namespace AC {

class VectorCapacityTracker {
public:
  static VectorCapacityTracker* Self() {
    static VectorCapacityTracker* gInstance = 0;
    if (!gInstance)
      gInstance = new VectorCapacityTracker;
    return gInstance;
  }

  void reportCapacityExceededWarnings();

  void addCapacityWarning(const std::string& typeString, size_t* maximumCapacity, size_t inlineCapacity) {

    capacityTypesMap()->insert(std::make_pair(typeString, std::make_pair(maximumCapacity, inlineCapacity)));
  }

  template<typename T>
  size_t* maximumCapacityForType() {

    static size_t* gCapacity = 0;
    if (!gCapacity) {
      gCapacity = new size_t;
      *gCapacity = 0;
    }
    return gCapacity;
  }

private:
  VectorCapacityTracker() { }

  std::map<std::string, std::pair<size_t*, size_t> >* capacityTypesMap() {

    static std::map<std::string, std::pair<size_t*, size_t> >* gTypes = 0;
    if (!gTypes)
     gTypes = new std::map<std::string, std::pair<size_t*, size_t> >;
    return gTypes;
  }
};

// Returns true if the ACSOFT_VECTOR_BENCHMARK environment variable is present and not set to zero.
static inline bool isVectorBenchmarkEnabled() {

  static const char* value = getenv("ACSOFT_VECTOR_BENCHMARK");
  if(!value)
    return false;

  return std::string(value) != std::string("0");
}

template<typename T>
static inline void checkInlineCapacity(size_t size, size_t inlineCapacity) {

  if (size <= inlineCapacity || !isVectorBenchmarkEnabled())
    return;

  size_t* maximumCapacity = VectorCapacityTracker::Self()->maximumCapacityForType<T>();
  if (size <= *maximumCapacity)
    return;

  std::string prettyPrintTypeString = "Vector<";
  PRETTY_PRINT_TYPE((T));
  prettyPrintTypeString += ">";

  std::stringstream stream;
  stream << currentBacktrace() << std::endl;
  stream << "\t" << std::setw(25) << prettyPrintTypeString;

  *maximumCapacity = size;
  VectorCapacityTracker::Self()->addCapacityWarning(stream.str(), maximumCapacity, inlineCapacity);
}

}

#endif
#endif
