#ifndef BINNING_HH
#define BINNING_HH

#include <vector>

class TH1D;
class TAxis;

namespace ACsoft {

namespace Utilities {
  class Binning {
  public:
    static std::vector<double> PamelaBinning();
    static int nBinsPamela();
    static std::vector<double> EcalBinning();
    static int nBinsEcal();
    static std::vector<double> AlphaBinning();
    static int nBinsAlpha();
    static std::vector<double> AmsPositronFractionPaper2013Binning();
    static int nBinsAmsPositronFractionPaper2013Binning();

    /** Returns a vector with the lower edges for a logarithmic equidistant binning */
    static std::vector<double> GenerateLogBinning(int nBinLog, double Tmin, double Tmax);

    static void CollectEquidistantBinning(std::vector<double>& vec, int nbins, double fromValue, double toValue);

    /** Calculate binning according to ECAL energy resolution.
     *
     * \param minEnergy Minimal energy start value for binning.
     * \param maxEnergy Maximum energy value which should be included in the highest energy bin.
     * \param factor Fraction of sigmaE to each bin side.
     */
    static std::vector<double> EnergyResolutionBinning(double minEnergy, double maxEnergy, const double factor = 1.5);

    /** Calculate binning according to tracker sagitta resolution.
     *
     * \param minRigidity Minimal rigidity start value for binning.
     * \param maxRigidity Maximum rigidity value which should be included in the highest rigidity bin.
     * \param mdr MDR of the desired tracker configuration
     * \param factor Fraction of sigmaS to each bin side.
     */
    static std::vector<double> SagittaResolutionBinning(const double minRigidity, const double maxRigidity, const double mdr, const double factor = 1.5);

    /** Calculate binning acording to fullspan tracker sagitta resolution.
     *
     * \param minRigidity Minimal rigidity start value for binning.
     * \param maxRigidity Maximum rigidity value which should be included int the highest rigidity bin.
     * \param factor Fraction of sigmaS to each bin side.
     */
    static std::vector<double> SagittaResolutionBinningFullspan(const double minRigidity, const double maxRigidity, const double factor = 1.5);

    /** Convert the sagitta axis to rigidity and return a new histogram with this axis.
     *  if the axis contains values of different signs the axis has to be symmetric up to now
     *
     * \param sagittaHistogram Histogram with axis along sagitta.
     */
    static TH1D* ConvertToRigidity(const TH1D* sagittaHistogram);

    /** Get the the bins of a TAxis Object
     *
     * \param axis TAxis object from were you want to get the bins.
     */
    static std::vector<double> GetAxis(const TAxis* axis);

    /** Get only positive bins from a TAxis object
     *
     * \param axis TAxis object from were you want to get the bins.
     */
    static std::vector<double> GetPositiveBinning(const TAxis* axis);

    /** Mirror the sign of the bins
     */
    static std::vector<double> MirrorSign(const std::vector<double>&);

    /** Inverse the bins. Use this if you want to convert a binning in sagitta to rigidity
     */
    static std::vector<double> Inverse(const std::vector<double>&);

    /** Get a binning covering symetric negative and positive range.
     *  If you have a positive binning from 0.1 to 100 for example you can use this to get a full binning from -100 to 100 acording to your input bins.
     */
    static std::vector<double> CompleteWithNegative(const std::vector<double>& positiveBinning);
  };
};

}

#endif // BINNING_HH
