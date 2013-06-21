#ifndef TEMPLATE_FITTER_HH
#define TEMPLATE_FITTER_HH

#include<vector>
#include<string>

class TH1F;
class TCanvas;

namespace ACsoft {

namespace Utilities {

/**
 * @brief The TemplateFitter class
 *
 * Class to fit an arbitrary number of template distributions to a data distribution.
 *
 * You have to use TH1F. Variable bin widths are fine.
 *
 * You can use a Chisquared or Likelihood fit.
 *
 */
class TemplateFitter {

public:
  TemplateFitter(int printlevel);
  ~TemplateFitter();

  /**
   *
   * Find out if fit has been performed successfully
   */
  bool HasBeenFit() const {return fHasBeenFit;}

  /**
   * @brief Re-sets the printlevel of Minuit after initialization
   */
  void SetPrintLevel(int pl){fPrintLevel=pl; }

  /**
   * @brief Set Data Histogram
   * Only TH1F histos are supported right now.
   */
  void SetDataHistogram(TH1F* hist){ fInputData=hist; }

  /**
   * @brief Add Template Histogram
   * Only TH1F histos supported right now.
   * Add as many templates as you wish. The parameter order is the same as the histogram order, i.e. the first fit parameter belongs to the first template shape.
   */
  void AddTemplateHistogram(TH1F* hist);

  const std::vector<double>& GetRelativeResult() const;
  const std::vector<double>& GetAbsoluteResult() const;
  const std::vector<double>& GetRelativeResultError() const;
  const std::vector<double>& GetAbsoluteResultError() const;

  TH1F* CloneCombinedResultHistogram(const char*) const;
  TCanvas* CreateResultDrawing(std::string canvasname, int width, int height, double chi2, bool defaultStyle = true) const;

  void Clear();

  /**
   * @brief Fit
   *
   * Calls the fit appropriate fit function. You can choose with the parameter i between chi-square (i==0) or likelihood fit (i==1)(default).
   *
   * The two doubles give the fit range to be used.
   *
   */
  double Fit(int i=1, double lowBound=0, double highBound=0);

  static void chisqr_fit_func(int &npar, double *gin, double &f, double *par, int iflag);
  static void llh_fit_func(int &npar, double *gin, double &f, double *par, int iflag);

private:

  double ChiSqrFit();
  double LlhFit();
  int TemplateColor(int i) const;
  int TemplateFillStyle(int i) const;

  bool TestHistogramCompatibility();

  int fPrintLevel;                       ///< Print Level for Minuit output
  bool fHasBeenFit;                      ///< Flag if fit has been done

  TH1F* fData;                           ///< Container for data histogram that is actually used in fit
  TH1F* fInputData;                      ///< Container for data histogram that is given to Fitter. Its range might be changed later.
  std::vector<TH1F*> fTemplates;         ///< Container for variable number of template distributions that are actually used in fit
  std::vector<TH1F*> fInputTemplates;    ///< Container for templates that are given to Fitter. Their range might be changed later.
  std::vector<double> fRelative;         ///< Fit Result in relative amounts
  std::vector<double> fAbsolute;         ///< Fit result in absolute numbers
  std::vector<double> fRelativeError;    ///< Uncertainty of fit result in relative amounts
  std::vector<double> fAbsoluteError;    ///< Uncertainty of fit result of absolute numbers

  TH1F* fFitResult;                      ///< Histogram of fit result combined as sum from all templates
  std::vector<TH1F*> fResultHistos;      ///< Histograms of fit results of individual template distributions

};

}
}

#endif
