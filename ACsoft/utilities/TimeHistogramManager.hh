#ifndef TIMEHISTOGRAMMANAGER_HH
#define TIMEHISTOGRAMMANAGER_HH

#include <iostream>
#include <TTimeStamp.h>
#include <TClass.h>

class TH1;
class TH2;
class TH3;

namespace ACsoft {

namespace Utilities {


/** Utitlity class for enforcing consistent time binning of histograms.
  *
  * Since analysis and calibration will usually be run as parallel jobs,
  * it is necessary to enforce a global time binning for histograms that
  * contain time-dependent data. Then it is possible to simply merge histograms
  * obtained in different jobs together and add events in overlapping bins.
  *
  * The FileManager will scan the data set used in the analysis and set the
  * time of the first and last event in this class. Then, the user can generate
  * histograms with the proper time binning by a call to the MakeNewTimeHistogram1D(),
  * MakeNewTimeHistogram2D(), and MakeNewTimeHistogram3D()
  * functions.
  *
  * Time histograms can be merged by the \c ac_merge tool.
  *
  * The class is implemented as a singleton. Use the instance() function to obtain
  * a pointer.
  *
  * \attention Make sure that you only create time histograms after the call to FileManager::ReadFileList() !
  */
class TimeHistogramManager
{
public:

  /// Obtain pointer to singleton class instance.
  static TimeHistogramManager* instance() {
    static TimeHistogramManager* fInstance = 0;
    if( !fInstance )
      fInstance = new TimeHistogramManager;
    return fInstance;
  }

  // static (utility) functions
  template<class T>
  static T* MakeNewTimeHistogram1D( std::string name, std::string title);

  template<class T>
  static T* MakeNewTimeHistogram2D( std::string name, std::string title, std::string yaxisLabel, int nbinsY, double ymin, double ymax );

  template<class T>
  static T* MakeNewTimeHistogram2D( std::string name, std::string title, std::string yaxisLabel, int nbinsY, double* yBinning );

  template<class T>
  static T* MakeNewTimeHistogram3D( std::string name, std::string title, std::string yaxisLabel, std::string zaxisLabel, int nbinsY, double ymin, double ymax, int nbinsZ, double zmin, double zmax);

  template<class T>
  static T* MakeNewTimeHistogram3D( std::string name, std::string title, std::string yaxisLabel, std::string zaxisLabel, int nbinsY, double* yBinning, int nbinsZ, double* zBinning );

  static TH1*  Merge1DTimeHistograms( std::vector<TH1*>& histos );
  static TH2*  Merge2DTimeHistograms( std::vector<TH2*>& histos );
  static TH3*  Merge3DTimeHistograms( std::vector<TH3*>& histos );

  // non-static functions

  void SetFirstAndLastEventTimes( TTimeStamp first, TTimeStamp last );

  /// Timestamp of first event.
  const TTimeStamp& GetTimeOfFirstEvent() const;
  /// Timestamp of last event.
  const TTimeStamp& GetTimeOfLastEvent() const;

  bool GetTimeBinning( int& nbins, double& xmin, double& xmax ) const;

  /// Global bin width for time binning (seconds)
  int GetBinWidthSeconds() const { return fBinWidthSeconds; }

  void Reset();

private:

  TimeHistogramManager();
  ~TimeHistogramManager();

private:

  TTimeStamp fTimeOfFirstEvent;
  TTimeStamp fTimeOfLastEvent;
  bool fIsValid;

  /// global bin width for time binning (seconds)
  const int fBinWidthSeconds;

};
}


/** Create a new 1D time histogram with correct global time binning.
  *
  * The time axis is always the x-axis.
  *
  * \param name Name for new histogram.
  * \param title Title for new histogram.
  * \param yaxisLabel Label for y-axis of new histogram.
  * \param nbinsY Number of bins on y-axis of new histogram.
  * \param ymin Y-axis start of new histogram.
  * \param ymax Y-axis end of new histogram.
  *
  * This function works for all 1-dimensional histograms (TH1I, TH1F, TH1D,...), for example:
  * \code
  * TH1I* histo = Utilities::TimeHistogramManager::MakeNewTimeHistogram1D<TH1I>( "histo", "time histogram" );
  * \endcode
  *
  * \note ROOT's SetTimeFormat function is pesky! If you do not take extra precautions, the time axis will
  * be converted to local time, including daylight saving time! You can set the \c TZ environment variable
  * \code
  * export TZ=UTC
  * \endcode
  * but this will affect your entire system!
  */
template<class T>
T* Utilities::TimeHistogramManager::MakeNewTimeHistogram1D( std::string name, std::string title) {

  // FIXME Avoid code duplication. Unify this function with the next one!

  int ntimebins;
  double timemin, timemax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(ntimebins,timemin,timemax);

  T* histo = new T( name.c_str(), title.c_str(), ntimebins, timemin, timemax);
  histo->GetXaxis()->SetTitle("time"); // changing this will cause the ac_merge tool to fail!
  histo->GetXaxis()->SetTimeDisplay(1);
  histo->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  return histo;
}


/** Create a new 2D time histogram with correct global time binning.
  *
  * The time axis is always the x-axis.
  *
  * Use this function for an equidistant y-binning.
  *
  * \param name Name for new histogram.
  * \param title Title for new histogram.
  * \param yaxisLabel Label for y-axis of new histogram.
  * \param nbinsY Number of bins on y-axis of new histogram.
  * \param ymin y-axis start of new histogram.
  * \param ymax y-axis end of new histogram.
  *
  * This function works for all 2-dimensional histograms (TH2I, TH2F, TH2D,...), for example:
  * \code
  * TH2I* histo = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "histo", "residuals", "#Delta R / cm", 100, -2.0, 2.0 );
  * \endcode
  *
  * \note ROOT's SetTimeFormat function is pesky! If you do not take extra precautions, the time axis will
  * be converted to local time, including daylight saving time! You can set the \c TZ environment variable
  * \code
  * export TZ=UTC
  * \endcode
  * but this will affect your entire system!
  */
template<class T>
T* Utilities::TimeHistogramManager::MakeNewTimeHistogram2D( std::string name, std::string title, std::string yaxisLabel, int nbinsY, double ymin, double ymax ) {

  // FIXME Avoid code duplication. Unify this function with the next one!

  int ntimebins;
  double timemin, timemax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(ntimebins,timemin,timemax);

  T* histo = new T( name.c_str(), title.c_str(), ntimebins, timemin, timemax, nbinsY, ymin, ymax );
  histo->GetXaxis()->SetTitle("time"); // changing this will cause the ac_merge tool to fail!
  histo->GetYaxis()->SetTitle(yaxisLabel.c_str());
  histo->GetXaxis()->SetTimeDisplay(1);
  histo->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  return histo;
}


/** Create a new 2D time histogram with correct global time binning.
  *
  * The time axis is always the x-axis.
  *
  * Use this function for an arbitrary y-binning.
  *
  * \param name Name for new histogram.
  * \param title Title for new histogram.
  * \param yaxisLabel Label for y-axis of new histogram.
  * \param nbinsY Number of bins on y-axis of new histogram.
  * \param yBinning Array with bin boundaries on y-axis.
  */
template<class T>
T* Utilities::TimeHistogramManager::MakeNewTimeHistogram2D( std::string name, std::string title, std::string yaxisLabel, int nbinsY, double* yBinning ) {

  int ntimebins;
  double timemin, timemax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(ntimebins,timemin,timemax);

  double tBinWidth = (timemax - timemin) / double(ntimebins);
  double* tBinning = new double[ntimebins + 1];
  for (int i = 0; i <= ntimebins; ++i)
    tBinning[i] = timemin + tBinWidth * i;

  T* histo = new T( name.c_str(), title.c_str(), ntimebins, tBinning, nbinsY, yBinning );
  histo->GetXaxis()->SetTitle("time"); // changing this will cause the merge_histograms tool to fail!
  histo->GetYaxis()->SetTitle(yaxisLabel.c_str());
  histo->GetXaxis()->SetTimeDisplay(1);
  histo->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  return histo;
}

/** Create a new 3D time histogram with correct global time binning.
  *
  * The time axis is always the x-axis.
  *
  * Use this function for an equidistant y- and z-binning.
  *
  * \param name Name for new histogram.
  * \param title Title for new histogram.
  * \param yaxisLabel Label for y-axis of new histogram.
  * \param nbinsY Number of bins on y-axis of new histogram.
  * \param ymin y-axis start of new histogram.
  * \param ymax y-axis end of new histogram.
  * \param nbinsZ Number of bins on z-axis of new histogram.
  * \param zmin z-axis start of new histogram.
  * \param zmax z-axis end of new histogram.
  */
template<class T>
T* Utilities::TimeHistogramManager::MakeNewTimeHistogram3D( std::string name, std::string title, std::string yaxisLabel, std::string zaxisLabel, int nbinsY, double ymin, double ymax, int nbinsZ, double zmin, double zmax ) {

  int ntimebins;
  double timemin, timemax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(ntimebins,timemin,timemax);

  T* histo = new T( name.c_str(), title.c_str(), ntimebins, timemin, timemax, nbinsY, ymin, ymax, nbinsZ, zmin, zmax );
  histo->GetXaxis()->SetTitle("time"); // changing this will cause the merge_histograms tool to fail!
  histo->GetYaxis()->SetTitle(yaxisLabel.c_str());
  histo->GetZaxis()->SetTitle(zaxisLabel.c_str());
  histo->GetXaxis()->SetTimeDisplay(1);
  histo->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  return histo;
}

/** Create a new 3D time histogram with correct global time binning.
  *
  * The time axis is always the x-axis.
  *
  * Use this function for an arbitrary y- and z-binning.
  *
  * \param name Name for new histogram.
  * \param title Title for new histogram.
  * \param yaxisLabel Label for y-axis of new histogram.
  * \param nbinsY Number of bins on y-axis of new histogram.
  * \param yBinning Array with bin boundaries on y-axis.
  * \param nbinsZ Number of bins on z-axis of new histogram.
  * \param zBinning Array with bin boundaries on z-axis.
  *
  */
template<class T>
T* Utilities::TimeHistogramManager::MakeNewTimeHistogram3D( std::string name, std::string title, std::string yaxisLabel, std::string zaxisLabel, int nbinsY, double* yBinning, int nbinsZ, double* zBinning ) {

  int ntimebins;
  double timemin, timemax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(ntimebins,timemin,timemax);

  double tBinWidth = (timemax - timemin) / double(ntimebins);
  double* tBinning = new double[ntimebins + 1];
  for (int i = 0; i <= ntimebins; ++i)
    tBinning[i] = timemin + tBinWidth * i;

  T* histo = new T( name.c_str(), title.c_str(), ntimebins, tBinning, nbinsY, yBinning, nbinsZ, zBinning );
  histo->GetXaxis()->SetTitle("time"); // changing this will cause the merge_histograms tool to fail!
  histo->GetYaxis()->SetTitle(yaxisLabel.c_str());
  histo->GetZaxis()->SetTitle(zaxisLabel.c_str());
  histo->GetXaxis()->SetTimeDisplay(1);
  histo->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  return histo;
}

}

#endif // TIMEHISTOGRAMMANAGER_HH
