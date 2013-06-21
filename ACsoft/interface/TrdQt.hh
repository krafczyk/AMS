#ifndef TrdQt_hh
#define TrdQt_hh

// DO NOT ADD ANY OTHER INCLUDES FROM ANY OF THE ACSOFT SUBDIRECTORIES HERE!
// THE AMS ACQT INTERFACE RELIES ON THIS, TO MINIMIZE THE NUMBER OF INCLUDE
// DIRECTORIES NEEDED IN THE AMS MAKEFILES. USE CLASS FORWARDS ONLY!
#include <vector>
#include <TTimeStamp.h>
#include <TVector3.h>
#include <Settings.h>

#ifdef AMS_ACQT_INTERFACE
class AMSEventR;
class TrTrackR;
#endif

namespace ACsoft {

namespace Analysis {
  class TrdHit;
  struct TRDCandidateHit;
  struct TrackFitResult;
  class Particle;
}

/** Interface to all data related to the %TRD analysis.
 *
 * For a tracker track selected by the user, the TrdQt class determines whether the track is inside the geometrical acceptance
 * of the %TRD, performs the %TRD preselection, and determines whether enough information for reliable particle ID is provided
 * by the %TRD. Finally, likelihood ratios for particle ID, in particular for electron/proton separation, can be retrieved.
 *
 *  Definitions:
 *
 *   - candidate hit: A %TRD straw in which the path length of the chosen tracker track inside the gas volume of the straw is at least 1 mm.
 *   - active hit:    A candidate hit in which a non-zero ADC value has been measured in the given event.
 *
 *  Likelihoods for each particle are calculated from the probability density functions \f$ P(dE/dX) \f$ using the formula:
 *  \f[
 *  L_{particle} = \sqrt[n]{\prod_{i}^{n} P_{i}^{particle}(dE/dX)}
 *  \f]
 *  where \f$dE/dX\f$ is the energy deposit in a %TRD straw divided by the 3D path length and n is the total number of
 *  straws with an energy deposit associated with this particle. On average, \f$ n=19 \f$ for the 20 layer %TRD installed in AMS-02.
 *
 *  The probability density functions are parametrized as a function of rigidity for protons, Helium, ..., Carbon and as a function of the
 *  %TRD layer number for electrons. In addition, different PDFs are used for different Xenon partial pressures.
 *
 * Particle ID codes used throughout TrdQt:
 * - 0 electron
 * - 1 proton
 * - 2 Helium
 * - 3 Lithium
 * - 4 Beryllium
 * - 5 Boron
 * - 6 Carbon
 *
 * An example on how to use TrdQt on an AMSEventR:
 * \code
 * bool ProcessEvent(AMSEventR* pEvent) {
 *
 *   // Query TRD information by passing the trackIndex 0 to operate on the first tracker track, and the 1,3,3 tracker track fit.
 *   if (!trdQt.ProcessEvent(pEvent, 0, 1, 3, 3))
 *     return false; // test if an internal error occurred
 *
 *   int nActiveLayers = trdQt.GetNumberOfActiveLayers();
 *
 *   // test if chosen track is inside TRD acceptance
 *   if (trdQt.IsInsideTrdGeometricalAcceptance()) {
 *
 *     // TRD preselection
 *     if (trdQt.PassesTrdPreselectionCuts()) {
 *
 *       // enough information in TRD for reliable particle ID?
 *       if (trdQt.UsefulForTrdParticleId()) {
 *
 *         // get the e/p log-likelihood ratio
 *         float logLikelihoodRatioElectronProton = trdQt.LogLikelihoodRatioElectronProton();
 *
 *         // ... do something with it ...
 *       }
 *     }
 *   }
 *
 *   return true;
 * }
 * \endcode
 *
 */
class TrdQt {
public:

  /** Constructor.
    *
    * If a non-zero value is given for the parameter \p numberOfToyMcEvents, toy MC events will be generated
    * and LogLikelihoodRatiosElectronsToyMC() and LogLikelihoodRatiosProtonsToyMC() can be called. This can be used to
    * study if the %TRD likelihoods are distributed statistically, i.e. following a distribution where the energy
    * deposition in each layer is distributed strictly according to the probability density function (PDF) for the
    * particle at hand and the energy depositions in all layers are statistically independent of each other.
    *
    *
    * \param[in] numberOfToyMcEvents If greater than 0, activate toy MC generation with the given number of toy MC events per event.
    */
  TrdQt(unsigned int numberOfToyMcEventsPerEvent = 0);
  ~TrdQt();

  /** Process a given particle.
    *
    * This methods returns true if the extrapolated tracker track is within the geometrical acceptance of the TRD \i and if the event
    * is useful for particle idenfication (passes candidate matching and preselection cuts). Otherwise it returns false.
    * If you want to examine exactly why it returned false, you can ask IsInsideTrdGeometricalAcceptance(), etc. to find the cause.
    *
    * \param[in] particleId If toy MC was enabled in the constructor, generate a list of Toy MC likelihoods drawn from the PDFs for
    * the particle specified. These can later be accessed by LogLikelihoodRatiosElectronsToyMC() and LogLikelihoodRatiosProtonsToyMC().
    */
  bool Process(const Analysis::Particle& particle, int particleId=-1, bool ExcludeDeadStraws = true);

#ifdef AMS_ACQT_INTERFACE
  /** Process a given AMS event and track fit. AMS Software users will use this method.
    * Unlike the other ProcessEvent() method operating on an Analysis::Event, this method returns true if the conversion
    * from an AMSEventR* to an AC::Event worked fine. That does NOT mean that preselection cuts & geometrical acceptance cuts are fulfilled.
    * In consequence, if this method returns false, you will not be able to query any information from this class. See the example code
    * provided in the description of the TrdQt class on how to proceed after the call to this function.
    */
  bool ProcessEvent(AMSEventR*, int trackIndex, int algorithm, int pattern, int refit);
#endif

  /** Returns true if the tracker track passes through layer 0 and 19 of the TRD. */
  bool IsInsideTrdGeometricalAcceptance() const;

  /** Returns true if we find more than 15 candidate layers in the TRD with a summed candidate path length of at least 7 cm. */
  bool PassesTrdPreselectionCuts() const;

  /** Returns true if we find more than 12 active hits on the tracker track. */
  bool UsefulForTrdParticleId() const;

  /** Returns \f$ -log(\frac{L_e}{L_e+L_p}) \f$, where \f$L_e\f$ is the electron likelihood and \f$L_p\f$ is the proton likelihood.*/
  double LogLikelihoodRatioElectronProton() const;

  /** Returns \f$ -log(\frac{L_{He}}{L_{He}+L_e}) \f$, where \f$L_{He}\f$ is the helium likelihood and \f$L_e\f$ is the electron likelihood.*/
  double LogLikelihoodRatioHeliumElectron() const;

  /** Returns \f$ -log(\frac{L_{He}}{L_{He}+L_p}) \f$, where \f$L_{He}\f$ is the helium likelihood and \f$L_p\f$ is the proton likelihood.*/
  double LogLikelihoodRatioHeliumProton() const;

  /** Returns the number of candidate straws, those with a pathlength of at least 0.1 cm.*/
  unsigned short GetNumberOfCandidateStraws() const;

  /** Returns the number of active straws, those candidate straws with a non-zero ADC value.*/
  unsigned short GetNumberOfActiveStraws() const;

  /** Returns the number of candidate layers, those with straws that have a pathlength of at least 0.1 cm. */
  unsigned short GetNumberOfCandidateLayers() const;

  /** Returns the number of active layers, those candidate layers with straws that have a non-zero ADC count. */
  unsigned short GetNumberOfActiveLayers() const;

  /** Returns the number of unassigned hits. Unlike GetUnassignedHits() this function is available on reduced ACROOT files. */
  unsigned short GetNumberOfUnassignedHits() const;

  /** Returns the total expected path length in the TRD in cm. */
  float GetCandidatePathLength() const;

  /** Returns the total path length in the TRD in cm for those candidate straws with a non-zero ADC value. */
  float GetActivePathLength() const;

  /** Returns the vector with TrdHits that are assigned to a candidate hit, these are only those hits with a path length > 0 */
  const std::vector<ACsoft::Analysis::TrdHit>& GetAssignedHits() const;

  /** Returns the vector with Trd Candidate Hits */
  const std::vector<ACsoft::Analysis::TRDCandidateHit>& GetCandidateHits() const;

  /** Returns the vector with TrdHits that are not assigned to a candidate hit */
  const std::vector<ACsoft::Analysis::TrdHit>& GetUnassignedHits() const;

  /** Returns the Xenon partial pressure for a given time. */
  double QueryXenonPressure(const TTimeStamp&, bool &queryOk) const;

  /** Returns the log-likelihood ratios for particle id N against electrons. */
  const std::vector<double>& LogLikelihoodRatiosElectrons() const;

  /** Returns the log-likelihood ratios for particle id N against protons. */
  const std::vector<double>& LogLikelihoodRatiosProtons() const;

  /** Returns the log-likelihood ratios for protons against electrons.
    * Unlike the general LogLikelihoodRatiosProtonToyMC() accessor this is available on ACROOT files as well.
    **/
  void GetLogLikelihoodRatiosElectronsProtonsToyMC(std::vector<float>&) const;

  /** Returns the log-likelihood ratios for electrons against helium.
    * Unlike the general LogLikelihoodRatiosElectronsToyMC() accessor this is available on ACROOT files as well.
    **/
  void GetLogLikelihoodRatiosHeliumElectronToyMC(std::vector<float>&) const;

  /** Returns the log-likelihood ratios for particle id N against electrons for toy MC.
    *
    * First index: i-th toyMC event
    * Second index: particle-ID
    */
  const std::vector<std::vector<double> >& LogLikelihoodRatiosElectronsToyMC() const;

  /** Returns the log-likelihood ratios for particle id N against protons for toy MC.
    *
    * First index: i-th toyMC event
    * Second index: particle-ID
    */
  const std::vector<std::vector<double> >& LogLikelihoodRatiosProtonsToyMC() const;

  /** Returns the p-value for various particle hypotheses. (Still experimental.) */
  const std::vector<double>& GetPvalues() const;

  /** Returns the Likelihood Products for various particle hypotheses. */
  void GetLikelihoodProducts(std::vector<float>& LikelihoodProducts) const;


  /** Check if likelihood for electron hypothesis was calculated without errors in the lookup of the pdf value. */
  bool IsLikelihoodOkForElectron() const;
  /** Check if likelihood for proton hypothesis was calculated without errors in the lookup of the pdf value. */
  bool IsLikelihoodOkForProton() const;
  /** Check if likelihoods for the available particle hypotheses were calculated without errors in the lookup of the pdf value. */
  void AreLikelihoodsOk(std::vector<bool>& LikelihoodsOk) const;

  /** Check if the calibration constants for all modules with hits used in the likelihood calculation are ok. */
  bool IsCalibrationGood() const;

  /** Check if all relevant slow control data was available for the likelihood calculation. */
  bool IsSlowControlDataGood() const;

  /** Returns the result from the TRD internal track fit combined withe the seed tracker track in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const Analysis::TrackFitResult& GetTrdTrackCombinedFit(unsigned int direction) const;

  /** Returns the result from the TRD internal track fit in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const Analysis::TrackFitResult& GetTrdTrackStandAloneFit(unsigned int direction) const;

  /** Returns the result from the fit of the spline tracker track with a straight line XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  const Analysis::TrackFitResult& GetTrkTrackFit(unsigned int direction) const;

  /** Returns the angle between the TRD stand alone track and the tracker track at the upper ToF in the XZ-Plane for direction=0 and in the YZ-Plane for direction=1*/
  float GetTrdTrkAngle(unsigned int direction) const;

  /** Returns \f$ -log(\frac{L_e}{L_e+L_p}) \f$, where \f$L_e\f$ is the electron likelihood and \f$L_p\f$ is the proton likelihood
    * and the rigidity is following a uniform random distribution.*/
  void GetLogLikelihoodRatioElectronProtonRandom(std::vector<float>&) const;

  void GetTrdKLikelihoodRatios(const Analysis::Particle&, std::vector<Float_t>& likelihoodRatios) const;
  Short_t TrdKActiveHits(const Analysis::Particle&) const;
  Short_t TrdKUnassignedHits(const Analysis::Particle&) const;
  Float_t TrdKLrElectronProton(const Analysis::Particle&) const;
  Float_t TrdKLrHeliumProton(const Analysis::Particle&) const;
  Float_t TrdKLrHeliumElectron(const Analysis::Particle&) const;

  void ComputeLikelihoodProducts(double pXe, double rigidity, const std::vector<ACsoft::Analysis::TRDCandidateHit>&,
                                 std::vector<double>& likelihoodProductForParticle, std::vector<bool> &likelihoodOkForParticle) const;

  static void ComputeLogLikelihoodRatiosForElectronsAndProtons(const std::vector<double>& likelihoodProductForParticle, std::vector<double>& logLikelihoodRatiosElectron, std::vector<double>& logLikelihoodRatiosProton);

  double CalculateElectronLikelihoodInSelectedEnergyBin(const TTimeStamp&, double BinLowEdge, double BinHighEdge) const;

private:
  friend class TrdQtPrivate;
  int GetRigidityBin(int particleId, float aRig) const;

  void CalculateLikelihoods(float aRig, const TTimeStamp&);
  void CalculatePvalues(float aRig, const TTimeStamp&);
  void CalculateToyMcLikelihoods(float aRig, const TTimeStamp&, int particleId);
  void CalculateRandomLikelihoods(const TTimeStamp& time);

  class TrdQtPrivate;
  TrdQtPrivate* d;
};

}

using ACsoft::TrdQt;

#endif // TrdQt_hh
