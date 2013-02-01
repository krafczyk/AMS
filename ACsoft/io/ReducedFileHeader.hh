#ifndef ReducedFileHeader_hh
#define ReducedFileHeader_hh

#include <string>
#include <vector>
#include <TObject.h>

namespace AC {
 class ACQtPDFLookupFileIdentifier;
}

namespace IO {

class ReducedEventWriter;

/** Run header data for ACROOT files.
  */
class ReducedFileHeader : public TObject {
public:
  ReducedFileHeader();

  /** Git SHA stored in the associated ACQt file(s) used to generate the tree.
    */
  std::string GitSHA() const { return fGitSHA; }

  /** TRD alignment file name used to generate the tree. 
    */
  std::string TrdAlignmentFileName() const { return fTrdAlignmentFileName; }

  /** TRD gain file name used to generate the tree. 
    */
  std::string TrdGainFileName() const { return fTrdGainFileName; }

  /** TRD PDF file name used to generate the tree. 
    */
  std::string TrdPdfFileName() const { return fTrdPdfFileName; }

  /** TRD slow control file name used to generate the tree. 
    */
  std::string TrdSlowControlFileName() const { return fTrdSlowControlFileName; }

  /** TrdQt settings.
    */
  AC::ACQtPDFLookupFileIdentifier* TrdQtPdfSettings() const { return fTrdQtPdfSettings; }

  /** Used by IO::ReducedEventWriter only, to fill in data.
    */
  void Initialize(const std::string& acqtGitSHA);

private:
  std::string fGitSHA;                                ///> Git SHA of the ACQt file.
  std::string fTrdAlignmentFileName;                  ///> Alignment file used to generate the tree.
  std::string fTrdGainFileName;                       ///> Gain file used to generate the tree.
  std::string fTrdPdfFileName;                        ///> PDF file used to generate the tree.
  std::string fTrdSlowControlFileName;                ///> Slow control file used to generate the tree.
  AC::ACQtPDFLookupFileIdentifier* fTrdQtPdfSettings; ///> TrdQt settings

  ClassDef(ReducedFileHeader,2)
};

}

#endif
