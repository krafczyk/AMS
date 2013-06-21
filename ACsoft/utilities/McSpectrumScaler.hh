#ifndef McSpectrumScaler_hh
#define McSpectrumScaler_hh

#include <vector>
#include <string>

#include <QMap>
#include <QList>
#include <QString>

#include <ConfigHandler.hh>
#include <McParticleId.hh>

class TH1;
class TH1D;
class TObject;
class TF1;
class TGraph;
class TCanvas;

namespace ACsoft {

namespace Analysis {
  class Particle;
}

namespace Utilities {

  class ConfigHandler;
  class ObjectManager;
  class McGeneratedEventsCalculation;

  /** A simple class for calculation of weights for a desired mc spectra.
   *
   * The McSpectrumScaler uses the same inputlist as for your FileManager to calculate weights for a desired spectrum for different mc particle species.
   * It runs on this inputlist and histogramize the raw triggered and generated mc spectra to calculate scaling factors.
   *
   * Example Usage:
   * \code
   * #include <Binning.hh>
   * #include <McSpectrumScaler.hh>
   *
   * ...
   *
   * Utilities::McSpectrumScaler scaler;
   *
   * const int nSclaerBins = 100;
   * const double minSimulationMomentum = 0.5;
   * const double maxSimulationMomentum = 10000;
   *
   * TF1 protonSpectrum("proton spectrum", Utilities::McSpectrumScaler::PowerSpectrum, minSimulationMomentum, maxSimulationMomentum, 4);
   * protonSpectrum.SetParameters(10000.0, -2.7, 0.5, 0.93827203);
   * scaler.Add(Utilities::McParticleId::Proton, Utilities::Binning::GenerateLogBinning(nSclaerBins, minSimulationMomentum, maxSimulationMomentum), &protonSpectrum);
   *
   * TF1 antiprotonSpectrum("antiproton spectrum", Utilities::McSpectrumScaler::PowerSpectrum, minSimulationMomentum, maxSimulationMomentum, 4);
   * antiprotonSpectrum.SetParameters(10.0, -2.7, 0.5, 0.93827203);
   * scaler.Add(Utilities::McParticleId::Antiproton, Utilities::Binning::GenerateLogBinning(nSclaerBins, minSimulationMomentum, maxSimulationMomentum), &antiprotonSpectrum);
   *
   * scaler.FillHistograms(inputlist);
   *
   * scaler.CalculateWeights();
   * \endcode
   *
   * After this calculation you can get the weights for a specified particle. And use it when filling your histograms.
   *
   * \code
   * const double weight = scaler.GetWeight(&particle);
   * \endcode
   *
   * And then use these weights when filling your histograms. In case you want to use the errors of your histogram you meight want to call histogram->Sumw2() before filling to calculate the errors accordingly.
   *
   * \code
   * histogram->Fill(value, weight);
   * \endcode
   *
   * For a quick check of the scaling you can call McSpectrumScaler::ComparissonCanvas(McSpectrumScaler*);
   *
   */

  class McSpectrumScaler {
  public:
    McSpectrumScaler();

    /** Constructor with config handler. 
     *  To save and read the histograms to file you can use this constructor. with --McSpectrumScaler/save und enable the saving of those histograms.
     *
     * @param cfghandler Config handler
     * @param resultdir Result directory where the output file is stored
     * @param suffix Suffix for the output file. Used in parallel computing, to uniquely identify result files
     */
    McSpectrumScaler(ConfigHandler* cfghandler, const std::string& resultdir, const std::string& suffix);
     
    ~McSpectrumScaler();

    /** Add a specified particle for which you want to be able to calculate weights.
     *
     * \param mcSpecies MC particle species.
     * \param binning Binning for the desired raw histogram for scaling.
     */
    void Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning);

    /** Add a specified particle for which you want to be able to calculate weights and also set your desired spectrum.
     *
     * \param mcSpecies MC particle species.
     * \param binning Binning for the desired raw histogram for scaling.
     * \param spectrum The desired spectrum for which you want to get the wieghts.
     */
    void Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning, TGraph* spectrum);

    /** Overloaded function for TF1.*/
    void Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning, TF1* spectrum);

    /** Overloaded function if you want to set an equidistant binning.
     *
     * (nBinsX, xMin, xMax according to ROOT TH1 class)
     *
     * \param mcSpecies MC particle species.
     * \param nBinsX Number of bins
     * \param xMin Low edge of first bin
     * \param xMax Up edge of last bin
     */
    void Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax);

    /** Overloaded function for TGraph.*/
    void Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax, TGraph* spectrum);

    /** Overloaded function for TF1.*/
    void Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax, TF1* spectrum);

    /** Overloaded function if you want to use your own histograms for scaling.
     *
     * Using this function the the histograms will belong to you. You are responsible for the right use.
     * If you want to use the McSpectrumScaler::FillHistogram() function you might want to pass an empty histogram.
     *
     * \param mcSpecies MC particle species.
     * \param histogram Histogram which is used for calculation of the weights.
     */
    void Add(McParticleId::McSpecies mcSpecies, TH1D* histogram);

    /** Returns the number of added particles.*/
    int GetNumberOfAddedParticles() const;

    /** Returns the corresponding triggered histogram for a desired mcSpecies.
     *
     * Keep in mind that these histograms belong to the Sclaer. If the scaler gets destructed also these histograms will. If you want to use them afterwards clone or copy them!
     * Keep also in mind that manipulating these histograms can result in different weights.
     *
     * \param mcSpecies MC particle species.
     * \param binningMode Specify what valus this histogram should contain: either momentum or kinetic energy.
     */
    TH1D* GetTriggeredHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode = MomentumBinning);

    /** Overloaded const function.*/
    const TH1D* GetTriggeredHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode = MomentumBinning) const;

    TH1D* CorrectedTriggeredHistogram(McParticleId::McSpecies mcSpecies) const;

    /** Returns the corresponding generated histogram for a desired mcSpecies.
     *
     * Keep in mind that these histograms belong to the Sclaer. If the scaler gets destructed also these histograms will. If you want to use them afterwards clone or copy them!
     * Keep also in mind that manipulating these histograms can result in different weights.
     *
     * \param mcSpecies MC particle species.
     * \param binningMode Specify what valus this histogram should contain: either momentum or kinetic energy.
     */
    TH1D* GetGeneratedHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode = MomentumBinning);

    /** Overloaded const function.*/
    const TH1D* GetGeneratedHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode = MomentumBinning) const;

    TH1D* CorrectedGeneratedHistogram(McParticleId::McSpecies mcSpecies) const;

    /** Start filling the histograms which are needed for the calculation of the weights.
     *
     * \param input The same inputlist or inputfile later on used for your analysis.
     */
    void FillHistograms(const std::string& input);

    /** Calculate the weights for the desired spectrum.
     *
     * After McSpectrumScaler::FillHistograms() you have to call this function to calculate the weights.
     * You can call it multiple times if you want to recalculate weights with d different spectrum.
     * Make sure you added the corresponding mcSpecies before calling this function.
     *
     * \param mcSpecies MC particle species.
     * \param spectrum The desired spectrum for which you want to get the wieghts.
     */
    void CalculateWeights(McParticleId::McSpecies mcSpecies, const TGraph* spectrum, bool useCorrection = false);

    /** Overloaded function for TF1.*/
    void CalculateWeights(McParticleId::McSpecies mcSpecies, const TF1* spectrum, bool useCorrection = false);

    /** Calculate the weights for the desired spectrum of histogram from mcSpeciesToUse.
     *
     * This function replaces the content of the mc histogram from mcSpecies by the content of histogram from mcSpeciesToUse.
     *
     * \param mcSpecies MC particle species.
     * /param mcSpeciesToUse of which you want to use the primary distribution for this mcSpecies.
     * \param spectrum The desired spectrum for which you want to get the wieghts.
     */
    void CalculateWeights(McParticleId::McSpecies mcSpecies, McParticleId::McSpecies mcSpeciesToUse, const TGraph* spectrum, bool useCorrection = false);

    /** Overloaded function for TF1.*/
    void CalculateWeights(McParticleId::McSpecies mcSpecies, McParticleId::McSpecies mcSpeciesToUse, const TF1* spectrum, bool useCorrection = false);

    /** Calculate the weights for all spectra which were added before For example with the Add() function.*/
    void CalculateWeights(bool useCorrection = false);

    /** Get the corresponding weight to your particle.
     *
     * \param particle The particle for which you want the weight.
     */
    double GetWeight(const Analysis::Particle* particle) const;

    /** Get the corresponding weight to your particle and mimic a different mcSpecies.
     *
     * \param particle The particle for which you want the weight.
     * \param mcSpecies MC particle species you want to use instead of the species obtained from the mc eventgenerator.
     */
    double GetWeight(const Analysis::Particle* particle, McParticleId::McSpecies mcSpecies) const;

    const TObject* GetSpectrum(McParticleId::McSpecies) const;

    /** Values of a powerspectrum with solar modulation.
     *
     * flux = N0 * pow(T, alpha) * T*(T+2.*M)/(T0*(T0+2.*M)) with T0 = T0 = T + phi.
     *
     * N0 = par[0] absolute scaling
     * alpha = par[1] spectral index
     * phi = par[2] solar modulation parameter in GV
     * M = par[3] mass of the particle in GeV
     * T = sqrt(momentum[0] * momentum[0] + M * M) - M kinetic energy of the particle
     *
     * \param momentum The momentum.
     * \param par The parameters of the spectrum.
     */
    static double PowerSpectrum(double* momentum, double* par);

    /** Creates an comparisson canvas of the raw histograms, the passed spectra and the resulting scaling
     *
     * A new canvas is created and put in your hands. Act responsible.
     *
     * \param scaler The scaler object from whom you want to produce the canvas.
     * \param name Opitonal name for the canvas.
     */
    static TCanvas* ComparissonCanvas(McSpectrumScaler* scaler, std::string name = "");

    /** Function to store the histograms directly from the code
     *
     * thisway you dont have to call --McSpectrumScaler/save
     */
    void SaveHistograms();

  private:
    QMap<McParticleId::McSpecies, TH1D*> m_triggeredHistograms;
    QMap<McParticleId::McSpecies, TH1D*> m_generatedHistograms;
    QMap<McParticleId::McSpecies, TH1D*> m_triggeredHistogramsKineticEnergy;
    QMap<McParticleId::McSpecies, TH1D*> m_generatedHistogramsKineticEnergy;
    QMap<McParticleId::McSpecies, QList<double> > m_scalingFactors;
    QMap<McParticleId::McSpecies, const TObject*> m_spectra;
    QMap<McParticleId::McSpecies, bool> m_isExternalHistogram;
    QMap<McParticleId::McSpecies, double> m_masses;
    ObjectManager* m_objectManager;
    bool m_saveHistograms;

    QString CheckAddedParticAndGetName(McParticleId::McSpecies mcSpecies) const;
    void AddSpectrum(McParticleId::McSpecies mcSpecies, const TObject* spectrum);
    void CalculateWeightsInternal(McParticleId::McSpecies mcSpecies, const TObject* spectrum, bool useCorrection);
    double SpectrumValue(double valueToEval, const TObject* spectrum) const;
    double BinnedValue(const Analysis::Particle* particle, Utilities::McBinningMode binningMode) const;

    static void SetMaximumMinimum(const std::vector<TH1*>& histograms);
    static double MinimumNonZeroBinContent(const TH1*);
    static TH1D* ReplaceHistogramContent(const TH1D* origin, TH1D* destination);

    const QMap<McParticleId::McSpecies, TH1D*>& GetTriggeredHistograms() const {return m_triggeredHistograms;}
    const QMap<McParticleId::McSpecies, TH1D*>& GetGeneratedHistograms() const {return m_generatedHistograms;}
    const QMap<McParticleId::McSpecies, QList<double> >& GetScalings() const {return m_scalingFactors;}
    const QMap<McParticleId::McSpecies, const TObject*>& GetSpectra() const {return m_spectra;}
    const QMap<McParticleId::McSpecies, bool>& GetIsExternalHistogram() const {return m_isExternalHistogram;}

    TH1D* ApplyScaling(TH1D* histogram, const QList<double>& scalingFactors) const;
    TH1D* CreateCorrectedHistogram(const TH1D* histogram, const QList<double>& scalingFactors) const;

  };
}

}

#endif
