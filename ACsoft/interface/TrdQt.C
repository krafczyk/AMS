#ifdef AMS_ACQT_INTERFACE
#include "ACQtProducer.h"
#endif

#include "TrdQt.hh"

#include "TrdCandidateMatching.hh"
#include "TrdHitFactory.hh"
#include "Event.h"
#include "ReducedEvent.hh"
#include "ReducedFileHeader.hh"
#include "ParticleFactory.hh"
#include "TrdDetector.hh"
#include "TrdModule.hh"
#include "TrdStraw.hh"
#include "DetectorManager.hh"
#include "AnalysisParticle.hh"
#include "SimpleGraphLookup.hh"
#include "Utilities.hh"
#include "TrdPdfLookup.hh"
#include "TrdHit.hh"
#include "ParticleSelectionCuts.hh"
#include "TrdLikelihoodCalculator.hh"

#include <TF1.h>
#include <TRandom3.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdQt> "
#include <debugging.hh>

// FIXME move to settings, or rather, a proper MC config file
static const float MC_XenonPressure = 800.0; // in mBar

class TrdQt::TrdQtPrivate {
public:
  TrdQtPrivate(unsigned int numberOfToyMcEvents)
    : fMonteCarloEvent(false)
    , fNumberOfToyMcEvents(numberOfToyMcEvents)
    , fNumberOfRandomEvents(numberOfToyMcEvents)
    , fIsCalibrationGood(false)
    , fXePressureOk(false)
    , fReducedEvent(0) {

    fLogLikelihoodRatiosElectronToyMC.assign(numberOfToyMcEvents, std::vector<double>());
    fLogLikelihoodRatiosProtonToyMC.assign(numberOfToyMcEvents, std::vector<double>());

    fLogLikelihoodRatiosElectronRandom.assign(numberOfToyMcEvents, std::vector<double>());
    fLogLikelihoodRatiosProtonRandom.assign(numberOfToyMcEvents, std::vector<double>());

    for (int i = 0; i < Utilities::TrdPdfLookup::fNumberOfParticles; ++i)
      v_RigLog[i] = Utilities::TrdPdfLookup::GetBinningForParticle(Utilities::ParticleId(i));

  }

  bool fMonteCarloEvent;
  unsigned int fNumberOfToyMcEvents;
  unsigned int fNumberOfRandomEvents;

#ifdef AMS_ACQT_INTERFACE
  Analysis::Particle fParticle;
#endif

  Analysis::TrdCandidateMatching fCandidateMatching;
  std::vector<double> fLogLikelihoodRatiosElectron;
  std::vector<double> fLogLikelihoodRatiosProton;
  std::vector<std::vector<double> > fLogLikelihoodRatiosElectronToyMC;
  std::vector<std::vector<double> > fLogLikelihoodRatiosProtonToyMC;
  std::vector<std::vector<double> > fLogLikelihoodRatiosElectronRandom;
  std::vector<std::vector<double> > fLogLikelihoodRatiosProtonRandom;

  std::vector<double> fPvalues;
  std::vector<double> fLikelihoodProducts;
  std::vector<bool>   fLikelihoodOkForParticle;

  bool fIsCalibrationGood;
  bool fXePressureOk;

  std::vector<double> v_RigLog[Utilities::TrdPdfLookup::fNumberOfParticles];

  const IO::ReducedEvent* fReducedEvent;
};

TrdQt::TrdQt(unsigned int numberOfToyMcEventsPerEvent)
  : d(new TrdQtPrivate(numberOfToyMcEventsPerEvent)) {

}

TrdQt::~TrdQt() {

}

int TrdQt::GetRigidityBin(int particleId, float aRig) const {

  Q_ASSERT(particleId >= 0);
  Q_ASSERT(particleId < Utilities::TrdPdfLookup::fNumberOfParticles);
  Q_ASSERT(aRig >= 0);

  const std::vector<double>& v_rig = d->v_RigLog[particleId];
  if (aRig < v_rig.front()) return 0;
  if (aRig >= v_rig.back()) return v_rig.size() - 2;

  for (unsigned int i = 0; i < v_rig.size() - 1; ++i) {
    if (aRig >= v_rig[i] && aRig < v_rig[i + 1])
      return i;
  }

  Q_ASSERT(false);
  return -1;
}


bool TrdQt::Process(const Analysis::Particle& particle, int particleId, bool ExcludeDeadStraws) {

 /* arguments defined in Settings.C that determine the behaviour of TrdQt:
   *
   * MinPathLength:    default 0mm and a smart algorithem is trying to maximize the number of straws on the tracker track.
   *                   If set to >0 only candidate hits matching active hits will be considered with a path length > MinPathLength.
   *
   * AddNearTrackHits: default is true and a smart algorithem is trying to combine candidate and active hits.
   *                   If set to false only straws are considered which are traversed by the tracker track.
   */

  d->fReducedEvent = particle.fReducedEvent;
  d->fNumberOfRandomEvents   = d->fNumberOfToyMcEvents;
  d->fMonteCarloEvent = particle.IsMC();

  bool AddNearTrackHits = Utilities::TrdPdfLookup::Self()->AddNearTrackHitsForCandidateMatching();

  float aRig = fabs(particle.Rigidity());
  bool PassesTrdCandidateMatchingCuts = true;
  if (!d->fReducedEvent)
    PassesTrdCandidateMatchingCuts = d->fCandidateMatching.Process(particle, AddNearTrackHits, ExcludeDeadStraws);

  CalculateLikelihoods(aRig, particle.EventTime());
  CalculateToyMcLikelihoods(aRig, particle.EventTime(), particleId);

   // This makes only sense for high rigidity protons
  for (unsigned int randomEvent = 0; randomEvent < d->fNumberOfRandomEvents; ++randomEvent) {
    d->fLogLikelihoodRatiosElectronRandom.at(randomEvent).clear();
    d->fLogLikelihoodRatiosProtonRandom.at(randomEvent).clear();
    d->fLogLikelihoodRatiosElectronRandom.at(randomEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
    d->fLogLikelihoodRatiosProtonRandom.at(randomEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
  }

  if (particleId==1 && aRig>100.0)
    CalculateRandomLikelihoods(particle.EventTime());

  CalculatePvalues(aRig, particle.EventTime());

  return PassesTrdCandidateMatchingCuts;
}

#ifdef AMS_ACQT_INTERFACE
bool TrdQt::ProcessEvent(AMSEventR* pEvent, int trackIndex, int algorithm, int pattern, int refit) {

  assert(pEvent);
  if (trackIndex < 0)
    return false;

  static ACQtProducer* gProducer = 0;
  if (!gProducer)
    gProducer = new ACQtProducer;

  gProducer->PrepareProductionForEvent(pEvent);

  bool producedRunHeader = gProducer->ProduceRunHeader(1 /* Number of events in run */);
  assert(producedRunHeader);

  // Select only a single track fit, don't build any others.
  gProducer->SetProduceSingleTrackFitOnly(algorithm, pattern, refit);

  bool successful = gProducer->ProducePart(ACQtProducer::ProduceEventHeaderData);
  successful &= gProducer->ProducePart(ACQtProducer::ProduceParticlesData);
  successful &= gProducer->ProducePart(ACQtProducer::ProduceTOFData);
  successful &= gProducer->ProducePart(ACQtProducer::ProduceTrackerData);
  successful &= gProducer->ProducePart(ACQtProducer::ProduceTRDData);
  gProducer->FinishProductionForEvent(pEvent);

  // If AC::Event production failed, just exit here.
  if (!successful)
    return false;

  // Now check if the track index the user wants is present in the AC::Event, if not bail out.
  const AC::Event& event = gProducer->ProducedEvent();
  if (trackIndex >= (int)event.Tracker().Tracks().size())
    return false;

  // Check if the user selected track fit is available, if not bail out.
  const AC::TrackerTrack& trackerTrack = event.Tracker().Tracks()[trackIndex];
  int trackerTrackFitIndex = trackerTrack.GetFit(algorithm, pattern, refit);
  if (trackerTrackFitIndex == -1 || trackerTrackFitIndex >= (int)trackerTrack.TrackFits().size())
    return false;

  // Now we have an AC::Event and an AC::TrackerTrackFit and can just use regular ProcessTrackerTrack() method and we're done.
  const AC::TrackerTrackFit& trackFit = trackerTrack.TrackFits()[trackerTrackFitIndex];

  // Make sure alignment and gain calibration constants are up-to-date.
  Detector::DetectorManager::Self()->UpdateIfNeeded(event.EventHeader().TimeStamp(),event.RunHeader()->RunType());

  static Analysis::ParticleFactory* gParticleFactory = 0;
  if (!gParticleFactory)
    gParticleFactory = new Analysis::ParticleFactory;

  // Prepare particle (fill in which particle should be used, etc..).
  d->fParticle = Analysis::Particle(&event,0,&trackerTrack,&trackFit,0,0,0,0);
  gParticleFactory->FillHighLevelObjects(d->fParticle);
  Process(d->fParticle);
  return true;
}
#endif

void TrdQt::ComputeLogLikelihoodRatiosForElectronsAndProtons(const std::vector<double>& likelihoodProductForParticle, std::vector<double>& logLikelihoodRatiosElectron, std::vector<double>& logLikelihoodRatiosProton) {

  logLikelihoodRatiosElectron.clear();
  logLikelihoodRatiosProton.clear();
  logLikelihoodRatiosElectron.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);
  logLikelihoodRatiosProton.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) {
    double likelihoodRatioElectron = likelihoodProductForParticle[idParticle]/(likelihoodProductForParticle[idParticle]+likelihoodProductForParticle[0]);
    double likelihoodRatioProton   = likelihoodProductForParticle[idParticle]/(likelihoodProductForParticle[idParticle]+likelihoodProductForParticle[1]);
    logLikelihoodRatiosElectron.at(idParticle) = -log(likelihoodRatioElectron);
    logLikelihoodRatiosProton.at(idParticle)   = -log(likelihoodRatioProton);
  }
}


void TrdQt::ComputeLikelihoodProducts(double pXe, double rigidity, const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates,
                                      std::vector<double>& likelihoodProductForParticle, std::vector<bool>& likelihoodOkForParticle) const {

  if( candidates.empty() ) return;

  likelihoodOkForParticle.clear();
  likelihoodOkForParticle.assign(Utilities::TrdPdfLookup::fNumberOfParticles,false);

  Analysis::TrdLikelihoodCalculator calc(Analysis::RejectUnderflows);

  std::vector<ACsoft::Analysis::GenericHitData> SelectedHits;
  calc.PrepareGenericHitData(candidates,SelectedHits);

  for (int i=0; i<Utilities::TrdPdfLookup::fNumberOfParticles; i++){
    likelihoodProductForParticle[i] = calc.ComputeTrdLikelihood(Utilities::ParticleId(i),pXe,rigidity,SelectedHits);
    likelihoodOkForParticle[i] = calc.LastPdfQueriesOk();
    DEBUG_OUT << "Particle " << i << " llh: " << likelihoodProductForParticle[i] << " ok: " << likelihoodOkForParticle[i] << std::endl;
  }
}


double TrdQt::QueryXenonPressure(const TTimeStamp& time, bool& queryOk) const {

  if (d->fMonteCarloEvent)
    return MC_XenonPressure;

  if (d->fReducedEvent) {
   if (time.AsDouble() == d->fReducedEvent->EventTime) // FIXME this line has an unpredicted behaviour, why resort to the value stored in ReducedEvent at all?
     return d->fReducedEvent->XePressure;
  }

  return Utilities::TrdPdfLookup::QueryXenonPressure(time,queryOk);
}

void TrdQt::CalculatePvalues(float aRig, const TTimeStamp& time) {

  d->fPvalues.clear();
  d->fPvalues.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);

  // FIXME: Pvalues still experimental and not very usefull for the time beeing
  // FIXME: use  allQueriesOk  flag
  return;

  DEBUG_OUT << std::endl;
  int xenonBin = 0;
  if (d->fMonteCarloEvent)
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(MC_XenonPressure);
  else
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(time);

  const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates = GetCandidateHits();
  if (candidates.size()<=0) return;

  std::vector<int> rigidityBin;
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle)
    rigidityBin.push_back(GetRigidityBin(idParticle, aRig));

  static int nWarnings   =  1;
  static int maxWarnings =  0;

#ifdef AMS_ACQT_SILENCE_COMMON_WARNINGS
  maxWarnings = 0;
#endif

  static const double TrdMaxDeDx = AC::AMSGeometry::TRDMaxADCCount / ::AC::Settings::TrdTubeDefaultMinPathLength;

  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) {
    std::vector<double> pValue;
    bool allQueriesOk = true;
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      const Analysis::TRDCandidateHit&  hit   = candidates[i];
      unsigned short                    layer = ACsoft::AC::TRDStrawToLayer(hit.straw);
      double                            pV    = Utilities::TrdPdfLookup::Self()->GetTrdPvalue(xenonBin, rigidityBin.at(idParticle), layer, std::min(TrdMaxDeDx,double(hit.deDx)), (Utilities::ParticleId)idParticle);
      allQueriesOk &= Utilities::TrdPdfLookup::Self()->LastQueryOk();
      if (pV<=0.0 || pV>1.0) {
          nWarnings++;
          if (nWarnings<maxWarnings) {
              WARN_OUT << "unexpected value for pV=" << pV << " hit.deDx=" << hit.deDx << " layer=" << layer << " aRig=" << aRig << " time: " << time << std::endl;
          } else if (nWarnings==maxWarnings) {
              WARN_OUT << "Suppressing further warnings for: unexpected value for pV=" << pV << " hit.deDx=" << hit.deDx << " layer=" << layer << " aRig=" << aRig << " time: " << time << std::endl;
          }
          continue;
      }
      pValue.push_back(pV);
    }

    double ProductPvalues = 1.0;
    for (unsigned int i=0; i<pValue.size(); i++) ProductPvalues *= pValue.at(i);

    double pValueCombined = -1.0;
    if (ProductPvalues>0.0 && ProductPvalues<=1.0 && pValue.size()>0) {
        double MinusLogProductPvalues = -log(ProductPvalues);

        double sum = 0.0;
        for (unsigned int j=0; j<pValue.size()-1; j++) sum += pow(MinusLogProductPvalues,j)/TMath::Gamma(j+1);
        pValueCombined = ProductPvalues * sum;
    }
    d->fPvalues.at(idParticle) = pValueCombined;
  }
}

void TrdQt::CalculateLikelihoods(float aRig, const TTimeStamp& time) {

  double pXe = d->fMonteCarloEvent ? MC_XenonPressure : Utilities::TrdPdfLookup::QueryXenonPressure(time,d->fXePressureOk);

  d->fLogLikelihoodRatiosElectron.clear();
  d->fLogLikelihoodRatiosProton.clear();
  d->fLogLikelihoodRatiosElectron.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);
  d->fLogLikelihoodRatiosProton.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);
  d->fLikelihoodProducts.clear();
  d->fLikelihoodProducts.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);

  d->fLikelihoodOkForParticle.clear();
  d->fLikelihoodOkForParticle.assign(Utilities::TrdPdfLookup::fNumberOfParticles,false);

  const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates = GetCandidateHits();
  if (candidates.size()<=0) return;

  ComputeLikelihoodProducts(pXe, aRig, candidates, d->fLikelihoodProducts,d->fLikelihoodOkForParticle);
  ComputeLogLikelihoodRatiosForElectronsAndProtons(d->fLikelihoodProducts, d->fLogLikelihoodRatiosElectron, d->fLogLikelihoodRatiosProton);

  // check if calibration constants for all modules with hits used in the likelihood calculation are good
  d->fIsCalibrationGood = true;
  for( unsigned int i=0 ; i<candidates.size() ; ++i ){
    const Analysis::TRDCandidateHit& hit = candidates[i];
    const Detector::TrdModule* module = Detector::DetectorManager::Self()->GetAlignedTrd()->GetTrdStraw(hit.straw)->GetMother();
    d->fIsCalibrationGood &= ( module->IsCurrentGainValueOk() && module->IsAlignmentOk() );
  }
}

bool TrdQt::IsCalibrationGood() const {

  return d->fIsCalibrationGood;
}

bool TrdQt::IsSlowControlDataGood() const {

  return d->fXePressureOk;
}

double TrdQt::CalculateElectronLikelihoodInSelectedEnergyBin(const TTimeStamp& time, double BinLowEdge, double BinHighEdge) const {

  const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates = GetCandidateHits();
  if (candidates.size()<=0) return -1.0;

  bool queryOk = false;
  double pXe = d->fMonteCarloEvent ? MC_XenonPressure : Utilities::TrdPdfLookup::QueryXenonPressure(time,queryOk);

  // assume that we have an electron in energy bin between BinLowEdge and BinHighEdge
  // and that the measured rigidity follows a E^-3 distribution
  static TF1* gEnergyFunction = 0;
  if (!gEnergyFunction)
    gEnergyFunction = new TF1("Efunc","[0]*x**(-3)");

  gEnergyFunction->SetMinimum(BinLowEdge);
  gEnergyFunction->SetMaximum(BinHighEdge);
  gEnergyFunction->SetParameter(0, 2 *(pow(BinLowEdge, -2) - pow(BinHighEdge, -2))); // normalize
  double assumedEnergy = gEnergyFunction->GetRandom();

  std::vector<double> likelihoodProductForParticleSpecified;
  likelihoodProductForParticleSpecified.assign(Utilities::TrdPdfLookup::fNumberOfParticles,0.0);
  std::vector<bool> likelihoodsOk; // dummy, this function has to be removed anyway...
  ComputeLikelihoodProducts(pXe, assumedEnergy, candidates, likelihoodProductForParticleSpecified,likelihoodsOk);

  std::vector<double> calculatedRatio_Electron;
  std::vector<double> calculatedRatio_Proton;
  ComputeLogLikelihoodRatiosForElectronsAndProtons(likelihoodProductForParticleSpecified, calculatedRatio_Electron, calculatedRatio_Proton);
  return calculatedRatio_Proton.at(0);
}

void TrdQt::CalculateToyMcLikelihoods(float aRig, const TTimeStamp& time, int toyMCParticleID) {

  for (unsigned int mcEvent = 0; mcEvent < d->fNumberOfToyMcEvents; ++mcEvent) {
    d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent).clear();
    d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent).clear();
    d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
    d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
  }
  if (d->fNumberOfToyMcEvents<=0) return;

  if (toyMCParticleID<0 || toyMCParticleID>=Utilities::TrdPdfLookup::fNumberOfParticles) return;

  const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates = GetCandidateHits();
  if (candidates.size()<=0) return;

  bool queryOk = false; // used here as dummy, since we will have tested this for the real data already
  double pXe = d->fMonteCarloEvent ? MC_XenonPressure : Utilities::TrdPdfLookup::QueryXenonPressure(time,queryOk);

  for (unsigned int mcEvent = 0; mcEvent < d->fNumberOfToyMcEvents; ++mcEvent) {

    // generate the Toy MC event
    std::vector<ACsoft::Analysis::TRDCandidateHit> candidatesMC;
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      Analysis::TRDCandidateHit hitMC  = candidates[i];
      unsigned short            layer  = ACsoft::AC::TRDStrawToLayer(hitMC.straw);
      hitMC.deDx = Utilities::TrdPdfLookup::Self()->GetToymcDedxValue(pXe,aRig,layer,Utilities::ParticleId(toyMCParticleID));
      candidatesMC.push_back(hitMC);
    }

    // calculate the likelihood for the Toy MC event
    std::vector<double> likelihoodProductForParticleToyMC;
    likelihoodProductForParticleToyMC.assign(Utilities::TrdPdfLookup::fNumberOfParticles,0.0);
    std::vector<bool> likelihoodsOk; // dummy, the same pdfs as for the calculation of the real likelihoods will be used, so we already know if they are ok
    ComputeLikelihoodProducts(pXe, aRig, candidatesMC, likelihoodProductForParticleToyMC,likelihoodsOk);
    ComputeLogLikelihoodRatiosForElectronsAndProtons(likelihoodProductForParticleToyMC, d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent), d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent));
  }
}


void TrdQt::CalculateRandomLikelihoods(const TTimeStamp& time) {

  for (unsigned int randomEvent = 0; randomEvent < d->fNumberOfRandomEvents; ++randomEvent) {
    d->fLogLikelihoodRatiosElectronRandom.at(randomEvent).clear();
    d->fLogLikelihoodRatiosProtonRandom.at(randomEvent).clear();
    d->fLogLikelihoodRatiosElectronRandom.at(randomEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
    d->fLogLikelihoodRatiosProtonRandom.at(randomEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
   }
  if (d->fNumberOfRandomEvents<=0) return;

  const std::vector<ACsoft::Analysis::TRDCandidateHit>& candidates = GetCandidateHits();
  if (candidates.size()<=0) return;

  bool queryOk = false; // used here as dummy, since we will have tested this for the real data already
  double pXe = d->fMonteCarloEvent ? MC_XenonPressure : Utilities::TrdPdfLookup::QueryXenonPressure(time,queryOk);

  // assume that we have a particle with charge confusion
  // and that the measured rigidity follows in this case a uniform distribution
  static TRandom3 *sRandom = new TRandom3();

  for (unsigned int randomEvent = 0; randomEvent < d->fNumberOfRandomEvents; ++randomEvent) {

    float aRigRandom = sRandom->Uniform(1.0,1000.0);

    // calculate the likelihood for the Toy MC event
    std::vector<double> likelihoodProductForParticleRandom;
    std::vector<bool> likelihoodsOk; // dummy, the same pdfs as for the calculation of the real likelihoods will be used, so we already know if they are ok

    likelihoodProductForParticleRandom.assign(Utilities::TrdPdfLookup::fNumberOfParticles,0.0);
    ComputeLikelihoodProducts(pXe, aRigRandom, candidates, likelihoodProductForParticleRandom,likelihoodsOk);
    ComputeLogLikelihoodRatiosForElectronsAndProtons(likelihoodProductForParticleRandom, d->fLogLikelihoodRatiosElectronRandom.at(randomEvent), d->fLogLikelihoodRatiosProtonRandom.at(randomEvent));
  }

}


const std::vector<ACsoft::Analysis::TrdHit>& TrdQt::GetUnassignedHits() const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetUnassignedHits() not available on ACROOT files." << std::endl;
    static const std::vector<ACsoft::Analysis::TrdHit>* gEmptyVector = 0;
    if (!gEmptyVector)
      gEmptyVector = new std::vector<ACsoft::Analysis::TrdHit>;
    return *gEmptyVector;
  }

  return d->fCandidateMatching.UnassignedHits();
}

const std::vector<ACsoft::Analysis::TrdHit>& TrdQt::GetAssignedHits() const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetAssignedHits() not available on ACROOT files." << std::endl;
    static const std::vector<ACsoft::Analysis::TrdHit>* gEmptyVector = 0;
    if (!gEmptyVector) gEmptyVector = new std::vector<ACsoft::Analysis::TrdHit>;
    return *gEmptyVector;
  }

  return d->fCandidateMatching.AssignedHits();
}

const std::vector<ACsoft::Analysis::TRDCandidateHit>& TrdQt::GetCandidateHits() const {

  if (d->fReducedEvent) {
    static std::vector<ACsoft::Analysis::TRDCandidateHit>* gCandidates = 0;
    if (!gCandidates)
      gCandidates = new std::vector<ACsoft::Analysis::TRDCandidateHit>;
    else
      gCandidates->clear();
    for (unsigned int i=0; i<d->fReducedEvent->TrdStraws->size(); i++) {
      float PathLen = 0.0;      // Warning: PathLen no were used in TrdQt, only in Candidate Matching !
      gCandidates->push_back(Analysis::TRDCandidateHit(d->fReducedEvent->TrdStraws->at(i),PathLen,0.5*d->fReducedEvent->Trd2DeDx->at(i)));
    }
    return *gCandidates;
  }

  return d->fCandidateMatching.CandidateHits();
}


unsigned short TrdQt::GetNumberOfUnassignedHits() const {

  if (d->fReducedEvent)
    return d->fReducedEvent->TrdUnassignedHits;
  return GetUnassignedHits().size();
}

unsigned short TrdQt::GetNumberOfActiveLayers() const {

  if (d->fReducedEvent)
    return d->fReducedEvent->TrdActiveLayers;
  return d->fCandidateMatching.GetNumberOfActiveLayers();
}

unsigned short TrdQt::GetNumberOfActiveStraws() const {

  if (d->fReducedEvent) return d->fReducedEvent->TrdStraws->size();
  return d->fCandidateMatching.GetNumberOfActiveStraws();
}

float TrdQt::GetActivePathLength() const {

  if (d->fReducedEvent)
    return d->fReducedEvent->TrdActivePathLen;
  return d->fCandidateMatching.GetActivePathLength();
}

unsigned short TrdQt::GetNumberOfCandidateLayers() const {

  if (d->fReducedEvent)
    return d->fReducedEvent->TrdCandidateLayers;
  return d->fCandidateMatching.Preselection().GetNumberOfCandidateLayers();
}

unsigned short TrdQt::GetNumberOfCandidateStraws() const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetNumberOfCandidateStraws() not available on ACROOT files.\n";
    return 1000;
  }

  return d->fCandidateMatching.Preselection().GetNumberOfCandidateStraws();
}

bool TrdQt::IsInsideTrdGeometricalAcceptance() const {

  if (d->fReducedEvent)
    return d->fReducedEvent->TrdInAcceptance;
  return d->fCandidateMatching.Preselection().IsInsideTrdGeometricalAcceptance();
}

bool TrdQt::PassesTrdPreselectionCuts() const {

  if (d->fReducedEvent) {
    WARN_OUT << "PassesTrdPreselectionCuts() not available on ACROOT files.\n";
    return true;
  }

  return d->fCandidateMatching.PassesTrdPreselectionCuts();
}

bool TrdQt::UsefulForTrdParticleId() const {

  if (d->fReducedEvent) {
    WARN_OUT << "UsefulForTrdParticleId() not available on ACROOT files.\n";
    return true;
  }

  return d->fCandidateMatching.UsefulForTrdParticleId();
}

double TrdQt::LogLikelihoodRatioElectronProton() const {

  if (d->fLogLikelihoodRatiosProton.empty()) return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosProton.at(0);
}

double TrdQt::LogLikelihoodRatioHeliumElectron() const {

  if (d->fLogLikelihoodRatiosProton.empty()) return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosElectron.at(2);
}

double TrdQt::LogLikelihoodRatioHeliumProton() const {

  if (d->fLogLikelihoodRatiosProton.empty()) return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosProton.at(2);
}

float TrdQt::GetCandidatePathLength() const {

  if (d->fReducedEvent) return d->fReducedEvent->TrdCandidatePathLen;
  return d->fCandidateMatching.Preselection().GetCandidatePathLength();
}

static inline const std::vector<double>& sharedEmptyDoubleVector() {

  static const std::vector<double>* gEmptyVector = 0;
  if (!gEmptyVector)
    gEmptyVector = new std::vector<double>;
  return *gEmptyVector;
}

static inline const std::vector<std::vector<double> >& sharedEmptyDoubleVectorVector() {

  static const std::vector<std::vector<double> >* gEmptyVector = 0;
  if (!gEmptyVector)
    gEmptyVector = new std::vector<std::vector<double> >;
  return *gEmptyVector;
}

static inline const ACsoft::Analysis::TrackFitResult& sharedEmptyTrackFitResult() {

  static const ACsoft::Analysis::TrackFitResult* gEmptyObject = 0;
  if (!gEmptyObject)
    gEmptyObject = new ACsoft::Analysis::TrackFitResult(0, 0);
  return *gEmptyObject;
}

const std::vector<double>& TrdQt::LogLikelihoodRatiosElectrons() const {

  return d->fLogLikelihoodRatiosElectron;
}

const std::vector<double>& TrdQt::LogLikelihoodRatiosProtons() const {
  return d->fLogLikelihoodRatiosProton;
}

const std::vector<std::vector<double> >& TrdQt::LogLikelihoodRatiosElectronsToyMC() const {
  return d->fLogLikelihoodRatiosElectronToyMC;
}

const std::vector<std::vector<double> >& TrdQt::LogLikelihoodRatiosProtonsToyMC() const {
  return d->fLogLikelihoodRatiosProtonToyMC;
}

const std::vector<double>& TrdQt::GetPvalues() const {
  return d->fPvalues;
}

void TrdQt::GetLikelihoodProducts(std::vector<float>& LikelihoodProducts) const {
  LikelihoodProducts.clear();
  for (unsigned int i = 0; i < d->fLikelihoodProducts.size(); ++i)
    LikelihoodProducts.push_back(d->fLikelihoodProducts.at(i)); // FIXME there must be a better way to copy a vector
}


bool TrdQt::IsLikelihoodOkForElectron() const {
  return d->fLikelihoodOkForParticle.at(Utilities::Electron);
}

bool TrdQt::IsLikelihoodOkForProton() const {
  return d->fLikelihoodOkForParticle.at(Utilities::Proton);
}

void TrdQt::AreLikelihoodsOk(std::vector<bool>& LikelihoodsOk) const {

  LikelihoodsOk.clear();
  LikelihoodsOk.assign(d->fLikelihoodOkForParticle.begin(),d->fLikelihoodOkForParticle.end());
}

const ACsoft::Analysis::TrackFitResult& TrdQt::GetTrdTrackCombinedFit(unsigned int direction) const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetTrdTrackCombinedFit() not available on ACROOT files.\n";
    return sharedEmptyTrackFitResult();
  }

  return d->fCandidateMatching.GetTrdTrackCombinedFit(direction);
}

const ACsoft::Analysis::TrackFitResult& TrdQt::GetTrdTrackStandAloneFit(unsigned int direction) const {

 if (d->fReducedEvent) {
    WARN_OUT << "GetTrdTrackStandAloneFit() not available on ACROOT files.\n";
    return sharedEmptyTrackFitResult();
  }

  return d->fCandidateMatching.GetTrdTrackStandAloneFit(direction);
}

const ACsoft::Analysis::TrackFitResult& TrdQt::GetTrkTrackFit(unsigned int direction) const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetTrkTrackFit() not available on ACROOT files.\n";
    return sharedEmptyTrackFitResult();
  }

  return d->fCandidateMatching.GetTrkTrackFit(direction);
}

float TrdQt::GetTrdTrkAngle(unsigned int direction) const {

  if (d->fReducedEvent) {
    WARN_OUT << "GetTrdTrkAngle not available on ACROOT files.\n";
    return -1000.0;
  }
  return d->fCandidateMatching.GetTrdTrkAngle(direction);
}

void TrdQt::GetLogLikelihoodRatiosElectronsProtonsToyMC(std::vector<float>& result) const {
  result.clear();
  result.reserve(d->fLogLikelihoodRatiosProtonToyMC.size());

  for (unsigned int i = 0; i < d->fLogLikelihoodRatiosProtonToyMC.size(); ++i)
    result.push_back(d->fLogLikelihoodRatiosProtonToyMC.at(i).at(0));
}

void TrdQt::GetLogLikelihoodRatiosHeliumElectronToyMC(std::vector<float>& result) const {
  result.clear();
  result.reserve(d->fLogLikelihoodRatiosElectronToyMC.size());

  for (unsigned int i = 0; i < d->fLogLikelihoodRatiosElectronToyMC.size(); ++i)
    result.push_back(d->fLogLikelihoodRatiosElectronToyMC.at(i).at(2));
}


void TrdQt::GetLogLikelihoodRatioElectronProtonRandom(std::vector<float>& result) const {
  result.clear();
  result.reserve(d->fNumberOfRandomEvents);

  for (unsigned int i = 0; i < d->fNumberOfRandomEvents; ++i)
    result.push_back(d->fLogLikelihoodRatiosProtonRandom.at(i).at(0));
}

/** Returns the TrdK number of off-track hits for e/p/he likelihoods.
  */
Short_t TrdQt::TrdKUnassignedHits(const Analysis::Particle& particle) const {

  if (d->fReducedEvent) {
    assert(particle.fReducedEvent == d->fReducedEvent);
    return d->fReducedEvent->TrdKUnassignedHits;
  }

  assert(particle.RawEvent());
  return particle.TrackerTrack() ? particle.TrackerTrack()->TrdKChargeNumberOfOffTrackHitsForLikelihoods() : -1;
}

/** Returns the TrdK number of used hits for e/p/he likelihoods.
  */
Short_t TrdQt::TrdKActiveHits(const Analysis::Particle& particle) const {

  if (d->fReducedEvent) {
    assert(particle.fReducedEvent == d->fReducedEvent);
    return d->fReducedEvent->TrdKActiveHits;
  }

  assert(particle.RawEvent());
  return particle.TrackerTrack() ? particle.TrackerTrack()->TrdKChargeNumberOfHitsForLikelihoods() : -1;
}

/** Returns the e/p/He Likelihoods from the TrdK package (A.Kounine, MIT)
  */
void TrdQt::GetTrdKLikelihoodRatios(const Analysis::Particle& particle, std::vector<Float_t>& likelihoods) const {

  if (d->fReducedEvent) {
    likelihoods = *d->fReducedEvent->TrdKLikelihoodRatios;
    return;
  }

  assert(particle.RawEvent());

  likelihoods.clear();
  likelihoods.assign(3,-1.0);
  if (!particle.TrackerTrack())
    return;

  const AC::TrackerTrack::TrdKChargeLikelihoodVector& v_Likelihoods = particle.TrackerTrack()->TrdKChargeLikelihood();
  if (!v_Likelihoods.size())
    return;

  if (v_Likelihoods.at(0)>0 && v_Likelihoods.at(1)>0) likelihoods.at(0) = -log(v_Likelihoods.at(0)/(v_Likelihoods.at(0)+v_Likelihoods.at(1)));
  if (v_Likelihoods.at(1)>0 && v_Likelihoods.at(2)>0) likelihoods.at(1) = -log(v_Likelihoods.at(2)/(v_Likelihoods.at(2)+v_Likelihoods.at(1)));
  if (v_Likelihoods.at(0)>0 && v_Likelihoods.at(2)>0) likelihoods.at(2) = -log(v_Likelihoods.at(0)/(v_Likelihoods.at(0)+v_Likelihoods.at(2)));
}

/** Returns the TrdK log likelihood ratio Electron/(Electron+Proton)
  */
Float_t TrdQt::TrdKLrElectronProton(const Analysis::Particle& particle) const {

  if (d->fReducedEvent) {
    if (d->fReducedEvent->TrdKLikelihoodRatios->size() > 0)
      return d->fReducedEvent->TrdKLikelihoodRatios->at(0);
  } else {
    std::vector<Float_t> likelihoodRatios;
    GetTrdKLikelihoodRatios(particle, likelihoodRatios);
    return likelihoodRatios.size() ? likelihoodRatios.at(0) : -1;
  }
  return -1;
}

/** Returns the TrdK log likelihood ratio Helium/(Helium+Proton)
  */
Float_t TrdQt::TrdKLrHeliumProton(const Analysis::Particle& particle) const {

  if (d->fReducedEvent) {
    if (d->fReducedEvent->TrdKLikelihoodRatios->size() > 0)
      return d->fReducedEvent->TrdKLikelihoodRatios->at(1);
  } else {
    std::vector<Float_t> likelihoodRatios;
    GetTrdKLikelihoodRatios(particle, likelihoodRatios);
    return likelihoodRatios.size() ? likelihoodRatios.at(1) : -1;
  }
  return -1;
}

/** Returns the TrdK log likelihood ratio Electron/(Helium+Electron)
  */
Float_t TrdQt::TrdKLrHeliumElectron(const Analysis::Particle& particle) const {

  if (d->fReducedEvent) {
    if (d->fReducedEvent->TrdKLikelihoodRatios->size() > 0)
      return d->fReducedEvent->TrdKLikelihoodRatios->at(2);
  } else {
    std::vector<Float_t> likelihoodRatios;
    GetTrdKLikelihoodRatios(particle, likelihoodRatios);
    return likelihoodRatios.size() ? likelihoodRatios.at(2) : -1;
  }
  return -1;
}


