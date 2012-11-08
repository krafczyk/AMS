#ifndef TRDPERFORMANCEPLOTS_HH
#define TRDPERFORMANCEPLOTS_HH

#include <vector>
#include <string>

class TList;
class TH1I;
class TH2I;
class TGraphErrors;
class TProfile;

namespace Utilities {
  class ConfigHandler;
}


namespace Analysis {

class Particle;

/** Container class for plots checking basic TRD functionality.
  *
  * To add a new plot, add a pointer to a histogram in the class
  * declaration, create the histogram and add its pointer to the fHistogramList
  * in CreateHistograms(), add code
  * for filling the histogram in FillHistosFrom(), and add code for
  * creating a canvas that shows the plot in DrawHistos().
  */
class TrdPerformancePlots
{

public:

  TrdPerformancePlots( Utilities::ConfigHandler* cfghandler,
                       std::string resultDirectory = "",
                       std::string resultFileSuffix = "" );
  virtual ~TrdPerformancePlots();

  void FillHistosRaw( const Analysis::Particle& particle );
  void FillHistosPreselected( const Analysis::Particle& particle );

  void FillIntegralHistos();
  void SetTrdPerformanceStyle();
  void DrawHistos();
  void WriteHistos();
  void WriteHistosToNewFile();

private:

  void CreateHistograms();
  void DeleteHistos();

protected:

  std::string fResultDirectory;  ///< directory where result files should be stored
  std::string fOutputFilePrefix; ///< prefix for name of output file
  std::string fResultFileSuffix; ///< suffix for name of output file

  TList* fHistogramList;         ///< List of all "histograms", can store any TObject

  TH1I* fNumberOfTrdHitsHisto;                     ///< Number of Hits in TRD before Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdHitsHistoafterPreselection;    ///< Number of Hits in the TRD after Preselection. Event-by-event. Is Filled.

  TH1I* fNumberOfTrdHitsOnHTrack;  ///< Number of Hits in the TRD on (d<0.3cm) H Track after Preselection. Event-by-event. Bug.
  TH1I* fNumberOfTrdHitsOnVTrack;  ///< Number of Hits in the TRD on (d<0.3cm) V Track after Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdHitsOnTTrack;  ///< Number of Hits in the TRD on (d<0.3cm) T Track after Preselection. Event-by-event. Is Filled.

  TH1I* fNumberOfTrdHitsAroundHTrack;  ///< Number of Hits in the TRD around (d<2.0cm) H Track after Preselection. Event-by-event. Bug.
  TH1I* fNumberOfTrdHitsAroundVTrack;  ///< Number of Hits in the TRD around (d<2.0cm) V Track after Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdHitsAroundTTrack;  ///< Number of Hits in the TRD around (d<2.0cm) T Track after Preselection. Event-by-event. Is Filled.

  TH1I* fNumberOfTrdHTracksHisto;                  ///< Number of TRDHtracks before Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdVTracksHisto;                  ///< Number of TRDVtracks before Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdHTracksHistoafterPreselection; ///< Number of TRDHtracks after Preselection. Event-by-event. Is Filled.
  TH1I* fNumberOfTrdVTracksHistoafterPreselection; ///< Number of TRDVtracks after Preselection. Event-by-event. Is Filled.

  TH2I* fEventsvsLatLon;                     ///< Number of Events as a function of ISS position. Event-by-event. Is Filled.
  TH2I* fEventsvsLatLonafterPreselection;    ///< Number of Events as a function of ISS position after Preselection. Event-by-event. Is Filled.

  TH2I* fHPreCalibrationPhsHisto;                ///< Pulseheightspectra of each Straw on HTrack before calibration, after Preselection. Event-by-event. Straw according to Hcalib. Bug.
  TGraphErrors* fHPreCalibrationMpvPerStraw;     ///< Mpv vs. Straw (HTrack) before calibration, after Preselection. Integral. Straw according to Hcalib. Bug.
  TH1I* fHPreCalibrationMpvHisto;                ///< Mpv Histogram (HTrack, Projection of fHPreCalibrationMpvPerStraw). Integral. Bug.
  TH1I* fHPreCalibrationWidthHisto;              ///< Width of Landau Histogram (HTrack, Projection of Errors of fHPreCalibrationMpvPerStraw). Integral. Bug.

  TH2I* fVPreCalibrationPhsHisto;                ///< Pulseheightspectra of each Straw on VTrack before calibration, after Preselection. Event-by-event. Straw according to Hcalib. Is Filled.
  TGraphErrors* fVPreCalibrationMpvPerStraw;     ///< Mpv vs. Straw (VTrack) before calibration, after Preselection. Integral. Straw according to Hcalib. Is Filled.
  TH1I* fVPreCalibrationMpvHisto;                ///< Mpv Histogram (VTrack, Projection of fVPreCalibrationMpvPerStraw). Integral. Is Filled.
  TH1I* fVPreCalibrationWidthHisto;              ///< Width of Landau Histogram (VTrack, Projection of Errors of fVPreCalibrationMpvPerStraw). Integral. Is Filled.

  TH2I* fTPreCalibrationPhsHisto;                ///< Pulseheightspectra of each Straw on TRKTrack before calibration, after Preselection. Event-by-event. Straw according to Hcalib. Is Filled.
  TGraphErrors* fTPreCalibrationMpvPerStraw;     ///< Mpv vs. Straw (TRKTrack) before calibration, after Preselection. Integral. Straw according to Hcalib. Is Filled.
  TH1I* fTPreCalibrationMpvHisto;                ///< Mpv Histogram (TRKTrack, Projection of fTPreCalibrationMpvPerStraw). Integral. Is Filled.
  TH1I* fTPreCalibrationWidthHisto;              ///< Width of Landau Histogram (TRKTrack, Projection of Errors of fTPreCalibrationMpvPerStraw). Integral. Is Filled.

  TH2I* fHOccupancyOnTrack;    ///< Occupancy of Hits on HTrack after Preselection.  Event-by-event. Bug.
  TH2I* fHOccupancyOffTrack;   ///< Occupancy of Hits off HTrack after Preselection.  Event-by-event. Bug.

  TH2I* fVOccupancyOnTrack;    ///< Occupancy of Hits on VTrack after Preselection.  Event-by-event. Is Filled.
  TH2I* fVOccupancyOffTrack;   ///< Occupancy of Hits off VTrack after Preselection.  Event-by-event. Is Filled.

  TH2I* fTOccupancyOnTrack;    ///< Occupancy of Hits on TRKTrack after Preselection.  Event-by-event. Is Filled.
  TH2I* fTOccupancyOffTrack;   ///< Occupancy of Hits off TRKTrack after Preselection.  Event-by-event. Is Filled.

  TH2I* fHTrackingQuality;  ///< Comparisson of HTrack with TRKtrk at the UToF after Preselection. Event-by-event. Bug.
  TH2I* fVTrackingQuality;  ///< Comparisson of VTrack with TRKtrk at the UToF after Preselection. Event-by-event. Is Filled.
  TH1I* fHTrackingQualityAngle;  ///< Angle between HTrack and TRKtrk at the UToF after Preselection. Event-by-event. Bug.
  TH1I* fVTrackingQualityAngle;  ///< Angle between VTrack and TRKtrk at the UToF after Preselection. Event-by-event. Is Filled.

  TProfile* fHCalibFactors;  ///< Average HCalib calibration factor for each straw. Event-by-event when HCalib present. Straw according to Hcalib.

  TH1I* fTest;
  TH1I* fTest2;

};

}

#endif // TRDPERFORMANCEPLOTS_HH
