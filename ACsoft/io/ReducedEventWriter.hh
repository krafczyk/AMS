#ifndef ReducedEventWriter_HH
#define ReducedEventWriter_HH

#include <string>

#include <assert.h>
#include <TTree.h>
#ifndef __CINT__
#include "TrdQt.hh"
#endif

class TFile;

namespace IO {
  class ReducedFileHeader;
}

namespace ACsoft {

namespace Utilities {
  class ConfigHandler;
}

namespace IO {
  
  class ReducedEvent;

/** Store selected event variables in a ROOT Tree.
  *
  * \todo add help example
  * \todo add howto for adding a variable
  *
  */
class ReducedEventWriter
{
public:

  ReducedEventWriter( Utilities::ConfigHandler* cfghandler,
               std::string resultDirectory = "",
               std::string resultFileSuffix = "", bool onlyWriteExtraBranches = false );
  
  ~ReducedEventWriter();

  void Clear();

  /** Register a custom branch within the ROOT tree that
    * should be read in as additional branch in personal analysis code.
    *
    * eg:
    * \startcode
    * float myFloat = 0;
    * eventWriter->WriteCustomBranch("SomeNumber", &myFloat, "SomeNumber/F");
    **/
  template<class T>
  void WriteCustomBranch(const char* branchName, T* pointerToType, const char* dataTypeDescriptor) {

    assert(fTree);
    fTree->Branch(branchName, pointerToType, dataTypeDescriptor);
  }
  
  void Initialize(::IO::ReducedFileHeader*, unsigned int NumberOfToyMcEvents);

  void Process( const Analysis::Particle& particle );

  void Finish();

private:
  std::string fResultDirectory;
  std::string fOutputFilePrefix;
  std::string fResultFileSuffix;

  TFile* fFile;
  TTree* fTree;

  ReducedEvent* fReducedEvent;

  void SetBranches(ReducedEvent* event);

#ifndef __CINT__
  TrdQt* fTrdQt;
#endif

  bool fOnlyWriteExtraBranches;
};

}

}

#endif // ReducedEventWriter_HH
