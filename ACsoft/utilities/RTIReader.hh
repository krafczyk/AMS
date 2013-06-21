#ifndef RTI_RTIReader_hh
#define RTI_RTIReader_hh

#include <vector>

namespace ACsoft {

namespace Utilities {

class RTIData;

/** RTIReader to read the RTI informations from the RTI files. They can be found at: HPC-cluster in $ACROOTLOOKUPS/RTI, lxplus, $AMSDataDir/altec/RTI/
  * The RTI informations give inforamtion about the quality of the datataking in each unix second aswell as some related variables.
  *
  * example usage:
  *
  *  \code
  *  #include "RTIReader.hh"
  *
  *  int main() {
  *
  *  IO::RTIReader* rti = IO::RTIReader::Instance();
  *
  *    while (fm.GetNextEvent()) {
  *      const RTI::RTIData* RTIData = rti->GetRTIData(particle);
  *      histo->Fill(RTIData->GetLiveTime());
  *    }
  *    ...
  *  }
  *  \endcode
  * */

class RTIReader {
public:
  ~RTIReader();

  /** accessor for the RTI reader singleton */
  static RTIReader* Instance();

  /** Accessor function for the RTIData object via a unix time.
    * use this if you want to loop only over the rti files.
    * (usefull since most informations for the measuring time determination is inside there.) */
  const RTIData* GetRTIData(int time);

private:
  RTIReader();
  bool ReadRTIFile(int time);

  int fStartTime;
  int fEndTime;
  std::vector<RTIData*> fData;
};

}

}

#endif  // RTI_RTIReader_hh
