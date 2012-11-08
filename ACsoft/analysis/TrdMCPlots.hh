#ifndef TRDMCPLOTS_HH
#define TRDMCPLOTS_HH

#include <vector>
#include <string>

class TList;
class TH1;
class TH1I;
class TH1F;
class THStack;

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
class TrdMCPlots
{

public:

  TrdMCPlots(std::string name, Utilities::ConfigHandler* cfghandler,
             std::string resultDirectory = "",
             std::string resultFileSuffix = "" );
  virtual ~TrdMCPlots();

  void FillHistosFrom( const Analysis::Particle& );

  void NormaliseHistograms();                 ///< Normalise histograms for residual calculation and ease of comparison
  void CalculateResiduals();                  ///< Calculate residuals between MC and experimental data
  
  void DrawHistos();

  void WriteHistos();
  void WriteHistosToNewFile();
  
  unsigned short GetNumberMCevents()    {return fMCevents;}
  unsigned short GetNumberDataevents()  {return fDataevents;}

private:

  void CreateHistograms();

protected:

  std::string fName;
  std::string fResultDirectory;  ///< directory where result files should be stored
  std::string fOutputFilePrefix; ///< prefix for name of output file
  std::string fResultFileSuffix; ///< suffix for name of output file

  TList* fHistogramList; //can store any TObject
  
  unsigned short fMCevents;
  unsigned short fDataevents;

  TH1F* fHNumberTRDhitsData;                  ///< Number of hits in the TRD for each event in the data file.
  TH1F* fHNumberTRDhitsMC;                    ///< Number of hits in the TRD for each event in the MC file.
  THStack* fHNumberTRDhitsStack;              ///< THStack of TRDhits histograms;
  TH1F* fHNumberTRDhitsResiduals;             ///< Residuals (n_MC - n_data)/n_MC
  TH1F* fHNumberTRDhitsOnTrackData;           ///< Number of hits in the TRD on the track for each event in the data file.
  TH1F* fHNumberTRDhitsOnTrackMC;             ///< Number of hits in the TRD on the track for each event in the MC file.
  TH1F* fHNumberTRDhitsOnTrackResiduals;      ///< Residuals (n_MC - n_data)/n_MC
  TH1F* fHDepositedEnergyData;                ///< Deposited energy in the TRD for each event in the data file.
  TH1F* fHDepositedEnergyMC;                  ///< Deposited energy in the TRD for each event in the MC file.
  THStack* fHDepositedEnergyStack;            ///< THStack if Deposited Energy histograms
  TH1F* fHDepositedEnergyResiduals;           ///< Residuals (dE_MC - dE_data)/dEdx_MC
  TH1F* fHDepositedEnergyOnTrackData;         ///< Deposited energy in the TRD on track for each event in the data file.
  TH1F* fHDepositedEnergyOnTrackMC;           ///< Deposited energy in the TRD on track for each event in the MC file.
  TH1F* fHDepositedEnergyOnTrackResiduals;    ///< Residuals (dE_MC - dE_data)/dE_MC
};

}

#endif // TRDMCPLOTS_HH
