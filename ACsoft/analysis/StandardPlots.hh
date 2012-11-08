#ifndef STANDARDPLOTS_HH
#define STANDARDPLOTS_HH

#include <vector>
#include <string>

class TList;
class TH1;

namespace Utilities {
  class ConfigHandler;
}

namespace Analysis {

class Particle;

/** Container class for commonly-used standard plots.
  *
  * To add a new plot, add a pointer to a histogram in the class
  * declaration, create the histogram and add its pointer to the fHistogramList
  * in CreateHistograms(), add code
  * for filling the histogram in FillHistosFrom(), and add code for
  * creating a canvas that shows the plot in DrawHistos().
  */
class StandardPlots
{

public:

  StandardPlots( Utilities::ConfigHandler* cfghandler,
                 std::string resultDirectory = "",
                 std::string resultFileSuffix = "" );
  virtual ~StandardPlots();

  virtual void FillHistosFrom( const Particle& );

  virtual void DrawHistos();

  virtual void WriteHistos();
  virtual void WriteHistosToNewFile();

protected:

  virtual void CreateHistograms();

protected:

  std::string fResultDirectory;  ///< directory where result files should be stored
  std::string fOutputFilePrefix; ///< prefix for name of output file
  std::string fResultFileSuffix; ///< suffix for name of output file

  TList* fHistogramList; //can store any TObject

};

}

#endif // STANDARDPLOTS_HH
