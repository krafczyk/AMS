#ifdef AMS_ACQT_INTERFACE
#include "ACQtProducer.h"
#endif

#include <TF1.h>
#include "TrdQt.hh"

#include "TrdCandidateMatching.hh"
#include "TrdHitFactory.hh"
#include "Event.h"
#include "ParticleFactory.hh"
#include "TrdDetector.hh"
#include "AnalysisParticle.hh"
#include "SimpleGraphLookup.hh"
#include "Utilities.hh"
#include "TrdPdfLookup.hh"
#include "TrdHit.hh"
#include "ParticleSelectionCuts.hh"

#include <TRandom3.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdQt> "
#include <debugging.hh>

static const float MC_XenonPressure = 800.0; // in mBar
static const int MaximumNumberOfRigidityBins = 30;

class TrdQt::TrdQtPrivate {
public:
  TrdQtPrivate(TrdQt* trdQt, unsigned int numberOfToyMcEvents)
    : fTrdMinDeDx(0)
    , fTrdMaxDeDx(AC::AMSGeometry::TRDMaxADCCount / AC::AMSGeometry::TRDTubeMinPathLength)
    , fNumberOfSelectedHits(0)
    , fMonteCarloEvent(false)
    , fNumberOfToyMcEvents(numberOfToyMcEvents)
    , fMinimumPdfValue(1e-7) {

    // FIXME fMinimumPdfValue is a potentially important number, store it in Settings.C as well?
    fLogLikelihoodRatiosElectronToyMC.assign(numberOfToyMcEvents, std::vector<double>());
    fLogLikelihoodRatiosProtonToyMC.assign(numberOfToyMcEvents, std::vector<double>());

    for (int i = 0; i < Utilities::TrdPdfLookup::fNumberOfParticles; ++i)
      v_RigLog[i] = Utilities::TrdPdfLookup::GetBinningForParticle(i);

    for (int xenonBin = 0; xenonBin < Utilities::TrdPdfLookup::fNumberOfXenonBins; ++xenonBin) {
      for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) {
        for (int rigidityBin = 0; rigidityBin < Utilities::TrdPdfLookup::fNumberOfRigidityBins[idParticle]; ++rigidityBin) {
          for (unsigned int layer = 0; layer < AC::AMSGeometry::TRDLayers; ++layer) {
            std::stringstream string;
            string << "fPDFs_" << xenonBin << idParticle << rigidityBin << layer;
            fPDFs[xenonBin][rigidityBin][layer][idParticle] = new TF1(string.str().c_str(), trdQt, &TrdQt::FunPDF, 1.0, fTrdMaxDeDx, 4, "TrdQt", "FunPDF");
            fPDFs[xenonBin][rigidityBin][layer][idParticle]->SetParameters(double(xenonBin),double(rigidityBin),double(layer),double(idParticle));
            fPDFs[xenonBin][rigidityBin][layer][idParticle]->SetNpx(1000);
          }
        }
      }
    }
  }

  double fTrdMinDeDx;
  double fTrdMaxDeDx;
  int fNumberOfSelectedHits;

  bool fMonteCarloEvent;
  unsigned int fNumberOfToyMcEvents;

#ifdef AMS_ACQT_INTERFACE
  Analysis::Particle fParticle;
#endif

  Analysis::TrdCandidateMatching fCandidateMatching;
  std::vector<double> fLogLikelihoodRatiosElectron;
  std::vector<double> fLogLikelihoodRatiosProton;
  std::vector<std::vector<double> > fLogLikelihoodRatiosElectronToyMC;
  std::vector<std::vector<double> > fLogLikelihoodRatiosProtonToyMC;

  std::vector<double> fPvalues;

  double fMinimumPdfValue;
  std::vector<double> v_RigLog[Utilities::TrdPdfLookup::fNumberOfParticles];
  TF1* fPDFs[Utilities::TrdPdfLookup::fNumberOfXenonBins][MaximumNumberOfRigidityBins][AC::AMSGeometry::TRDLayers][Utilities::TrdPdfLookup::fNumberOfParticles];
};

TrdQt::TrdQt(unsigned int numberOfToyMcEventsPerEvent)
  : d(new TrdQtPrivate(this, numberOfToyMcEventsPerEvent)) {

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


bool TrdQt::Process(const Analysis::Particle& particle, int particleId) {

 /* arguments defined in Settings.C that determine the behaviour of TrdQt:
  *
  * MinPathLength: default 0mm and a smart algorithem is trying to maximize the number of straws on the tracker track.
  *                If set to >0 only candidate hits matching active hits will be considered with a path length > MinPathLength.
  *
  * ActiveStraws:  default is true and a smart algorithem is trying to combine candidate and active hits.
  *                If set to false only straws are considered which are traversed by the tracker track.
  */

  d->fNumberOfSelectedHits = 0;

  d->fMonteCarloEvent      = false; // FIXME this can be improved
  if (particle.RawEvent()->MC().EventGenerators().size() != 0) d->fMonteCarloEvent = true;

  d->fLogLikelihoodRatiosElectron.clear();
  d->fLogLikelihoodRatiosProton.clear();
  d->fLogLikelihoodRatiosElectron.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);
  d->fLogLikelihoodRatiosProton.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);

  for (unsigned int mcEvent = 0; mcEvent < d->fNumberOfToyMcEvents; ++mcEvent) {
    d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent).clear();
    d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent).clear();
    d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
    d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent).assign(Utilities::TrdPdfLookup::fNumberOfParticles, -1);
  }
  d->fPvalues.clear();
  d->fPvalues.assign(Utilities::TrdPdfLookup::fNumberOfParticles,-1);

  float minPathLength  = Utilities::TrdPdfLookup::Self()->GetMinimumPathLengthForCandidateMatching();
  bool useActiveStraws = Utilities::TrdPdfLookup::Self()->UseActiveStrawsForCandidateMatching();
  bool PassesTrdCandidateMatchingCuts = d->fCandidateMatching.Process(particle, minPathLength, useActiveStraws);

  assert(particle.MainTrackerTrackFit());
  float aRig = fabs(particle.MainTrackerTrackFit()->Rigidity());

  CalculateLikelihoods(aRig, particle.RawEvent()->EventHeader().TimeStamp());
  if (d->fNumberOfToyMcEvents > 0)
    CalculateToyMcLikelihoods(aRig, particle.RawEvent()->EventHeader().TimeStamp(), particleId);

  CalculatePvalues(aRig, particle.RawEvent()->EventHeader().TimeStamp());

  return PassesTrdCandidateMatchingCuts;
}

#ifdef AMS_ACQT_INTERFACE
bool TrdQt::ProcessEvent(AMSEventR* pEvent, int trackIndex, int algorithm, int pattern, int refit) {

  if (trackIndex < 0)
    return false;

  // FIXME: Ideally we don't want to produce all track fits, but only the one we need. Needs more fine-grained control.
  // (I've postponed this, to not complicate this example any more)
  ACQtProducer producer; // this will be a member variable later on.
  producer.PrepareProductionForEvent(pEvent);
  if (!producer.ProduceRunHeader(1 /* Number of events in run */))
    return false;

  // Select only a single track fit, don't build any others.
  producer.SetProduceSingleTrackFitOnly(algorithm, pattern, refit);

  bool successful = producer.ProducePart(ACQtProducer::ProduceEventHeaderData);
  successful &= producer.ProducePart(ACQtProducer::ProduceParticlesData);
  successful &= producer.ProducePart(ACQtProducer::ProduceTOFData);
  successful &= producer.ProducePart(ACQtProducer::ProduceTrackerData);
  successful &= producer.ProducePart(ACQtProducer::ProduceTRDData);
  producer.FinishProductionForEvent(pEvent);

  // If AC::Event production failed, just exit here.
  if (!successful)
    return false;

  // Now check if the track index the user wants is present in the AC::Event, if not bail out.
  const AC::Event& event = producer.ProducedEvent();
  if (trackIndex >= (int)event.Tracker().Tracks().size())
    return false;

  // Check if the user selected track fit is available, if not bail out.
  const AC::TrackerTrack& trackerTrack = event.Tracker().Tracks()[trackIndex];
  int trackerTrackFitIndex = trackerTrack.GetFit(algorithm, pattern, refit);
  if (trackerTrackFitIndex == -1 || trackerTrackFitIndex >= (int)trackerTrack.TrackFits().size())
    return false;

  // Now we have an AC::Event and an AC::TrackerTrackFit and can just use regular ProcessTrackerTrack() method and we're done.
  const AC::TrackerTrackFit& trackFit = trackerTrack.TrackFits()[trackerTrackFitIndex];

  static Analysis::ParticleFactory* gParticleFactory = 0;
  if (!gParticleFactory) {
    const int stepsForTrdHits = Analysis::SecondCoord | Analysis::ShimGlobalAlign | Analysis::AlignmentCorrection | Analysis::Pathlength | Analysis::GainCorrection;
    gParticleFactory = new Analysis::ParticleFactory(stepsForTrdHits);
  }

  // Prepare particle (fill in which particle should be used, etc..).
  d->fParticle = Analysis::Particle(&event,0,&trackerTrack,&trackFit,0,0,0);
  gParticleFactory->FillHighLevelObjects(d->fParticle);
  Process(d->fParticle);
  return true;
}
#endif

static void ComputeLogLikelihoodRatiosForElectronsAndProtons(double* likelihoodProductForParticle, std::vector<double>& logLikelihoodRatiosElectron, std::vector<double>& logLikelihoodRatiosProton) {

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


void TrdQt::ComputeLikelihoodProducts(int xenonBin, const std::vector<int>& rigidityBin, const std::vector<Analysis::TRDCandidateHit>& candidates, double* likelihoodProductForParticle) {

  int Model = 1;   // 0==All Hits, 1==reject underflows, 2==reject underflows and overflows
  std::vector<Analysis::TRDCandidateHit> SelectedHits;
  for (unsigned int i=0; i<candidates.size(); ++i) {
    const Analysis::TRDCandidateHit& hit = candidates.at(i);
    if (Model==1 && hit.deDx<d->fTrdMinDeDx+0.5) continue;
    if (Model==2 && (hit.deDx<d->fTrdMinDeDx+0.5 || hit.deDx>d->fTrdMaxDeDx-0.5)) continue;
    SelectedHits.push_back(hit);
  }

  d->fNumberOfSelectedHits = SelectedHits.size();
  if (d->fNumberOfSelectedHits<=0) return;

  double weight = 1.0/double(d->fNumberOfSelectedHits);
  for (int i=0; i<Utilities::TrdPdfLookup::fNumberOfParticles; i++) likelihoodProductForParticle[i]=1;

  for (unsigned int i = 0; i < SelectedHits.size(); ++i) {
    const Analysis::TRDCandidateHit& hit = SelectedHits[i];
    unsigned short layer = AC::TRDStrawToLayer(hit.straw);
    for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) {
      double   value = 0;
      if (hit.deDx<d->fTrdMinDeDx+0.5) {
        value = GetTrdPdfValue(xenonBin, rigidityBin.at(idParticle), layer, d->fTrdMinDeDx, idParticle);
      } else if (hit.deDx>d->fTrdMaxDeDx-0.5) {
        value = GetTrdPdfValue(xenonBin, rigidityBin.at(idParticle), layer, d->fTrdMaxDeDx, idParticle);
      } else {
        value = d->fPDFs[xenonBin][rigidityBin.at(idParticle)][layer][idParticle]->Eval(hit.deDx);
      }
      value = std::max(d->fMinimumPdfValue,value);
      likelihoodProductForParticle[idParticle]*= pow(value, weight);
    }
  }
}


double TrdQt::QueryXenonPressure(const TTimeStamp& time) const {

  return Utilities::TrdPdfLookup::QueryXenonPressure(time);
}


void TrdQt::CalculatePvalues(float aRig, const TTimeStamp& time) {

  //std::cout << "Start: TrdQt::CalculatePvalues"  << std::endl;
  int xenonBin = 0;
  if (d->fMonteCarloEvent)
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(MC_XenonPressure);
  else
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(time);

  const std::vector<Analysis::TRDCandidateHit>& candidates = d->fCandidateMatching.CandidateHits();

  std::vector<int> rigidityBin;
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle)
    rigidityBin.push_back(GetRigidityBin(idParticle, aRig));

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  static int nWarnings=0;
#endif
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) {
    std::vector<double> pValue;
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      const Analysis::TRDCandidateHit&  hit   = candidates[i];
      unsigned short                    layer = AC::TRDStrawToLayer(hit.straw);
      double                            pV    = Utilities::TrdPdfLookup::Self()->GetTrdPvalue(xenonBin, rigidityBin.at(idParticle), layer, std::min(d->fTrdMaxDeDx,double(hit.deDx)), (Utilities::ParticleId)idParticle);
      if (pV<=0.0 || pV>1.0) {
#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
          nWarnings++;
          if (nWarnings<10) {
              WARN_OUT << "unexpected value for pV=" << pV << " hit.deDx=" << hit.deDx << " layer=" << layer << " aRig=" << aRig << " time: " << time << std::endl;
          } else if (nWarnings==10) {
              WARN_OUT << "Suppressing further warnings for: unexpected value for pV=" << pV << " hit.deDx=" << hit.deDx << " layer=" << layer << " aRig=" << aRig << " time: " << time << std::endl;
          }
#endif
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
        //printf("idParticle=%4d d->fPvaluesElectron=%12.4E \n",idParticle,pValueCombined);
    }
    d->fPvalues.at(idParticle) = pValueCombined;
  }
  //std::cout << "End: TrdQt::CalculatePvalues"  << std::endl;
}


void TrdQt::CalculateLikelihoods(float aRig, const TTimeStamp& time) {

  int xenonBin = 0;
  if (d->fMonteCarloEvent)
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(MC_XenonPressure);
  else
    xenonBin = Utilities::TrdPdfLookup::GetXenonBin(time);

  const std::vector<Analysis::TRDCandidateHit>& candidates = d->fCandidateMatching.CandidateHits();
  if (candidates.size() <= 0)
    return;

  std::vector<int> rigidityBin;
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle)
    rigidityBin.push_back(GetRigidityBin(idParticle, aRig));

  double likelihoodProductForParticle[Utilities::TrdPdfLookup::fNumberOfParticles];
  ComputeLikelihoodProducts(xenonBin, rigidityBin, candidates, likelihoodProductForParticle);
  ComputeLogLikelihoodRatiosForElectronsAndProtons(likelihoodProductForParticle, d->fLogLikelihoodRatiosElectron, d->fLogLikelihoodRatiosProton);
}


Double_t TrdQt::FunPDF(Double_t *x, Double_t *par) {
    Int_t    xenonBin    = TMath::Nint(par[0]);
    Int_t    rigidityBin = TMath::Nint(par[1]);
    Int_t    layer       = TMath::Nint(par[2]);
    Int_t    particleId  = TMath::Nint(par[3]);
    double      dEdX          = x[0];

    double pdf=0.0;
    if (dEdX>d->fTrdMinDeDx+0.5 && dEdX<d->fTrdMaxDeDx-0.5) pdf = GetTrdPdfValue(xenonBin, rigidityBin, layer, dEdX, particleId);

    //printf("xenonBin=%4d rigidityBin=%4d layer=%4d idParticle=%4d dEdX=%12.4E value=%12.4E\n",xenonBin,rigidityBin,layer,particleId,dEdX,pdf);
    return     std::max(d->fMinimumPdfValue,pdf);
}


void TrdQt::CalculateToyMcLikelihoods(float aRig, const TTimeStamp& time, int particleId) {

  if (particleId<0 || particleId>=Utilities::TrdPdfLookup::fNumberOfParticles) return;

  const std::vector<Analysis::TRDCandidateHit>& candidates = d->fCandidateMatching.CandidateHits();
  if (candidates.size()<=0) return;

  int xenonBin = 0;
  if (d->fMonteCarloEvent) {
      xenonBin = Utilities::TrdPdfLookup::GetXenonBin(MC_XenonPressure);
  } else {
      xenonBin = Utilities::TrdPdfLookup::GetXenonBin(time);
  }

  std::vector<int> rigidityBin;
  // FIXME: now we have particleId and idParticle !?
  for (int idParticle = 0; idParticle < Utilities::TrdPdfLookup::fNumberOfParticles; ++idParticle) rigidityBin.push_back(GetRigidityBin(idParticle, aRig));

  static TRandom3 *fRndm = new TRandom3();

  for (unsigned int mcEvent = 0; mcEvent < d->fNumberOfToyMcEvents; ++mcEvent) {

    // generate the Toy MC event
    std::vector<Analysis::TRDCandidateHit> candidatesMC;
    for (unsigned int i = 0; i < candidates.size(); ++i) {
      Analysis::TRDCandidateHit hitMC           = candidates[i];
      unsigned short            layer           = AC::TRDStrawToLayer(hitMC.straw);
      double                    ZeroEntries     = GetTrdPdfValue(xenonBin, rigidityBin.at(particleId), layer, d->fTrdMinDeDx, particleId);
      double                    OverflowEntries = GetTrdPdfValue(xenonBin, rigidityBin.at(particleId), layer, d->fTrdMaxDeDx, particleId);
      double                    xRndm           = fRndm->Uniform();
      if (xRndm <= ZeroEntries) {
          hitMC.deDx = d->fTrdMinDeDx;
      } else if (xRndm>=1.0-OverflowEntries) {
          hitMC.deDx = d->fTrdMaxDeDx;
      } else {
          hitMC.deDx = d->fPDFs[xenonBin][rigidityBin.at(particleId)][layer][particleId]->GetRandom();
      }
      candidatesMC.push_back(hitMC);
    }

    // calculate the likelihood for the Toy MC event
    double likelihoodProductForParticleToyMC[Utilities::TrdPdfLookup::fNumberOfParticles];
    ComputeLikelihoodProducts(xenonBin, rigidityBin, candidatesMC, likelihoodProductForParticleToyMC);
    ComputeLogLikelihoodRatiosForElectronsAndProtons(likelihoodProductForParticleToyMC, d->fLogLikelihoodRatiosElectronToyMC.at(mcEvent), d->fLogLikelihoodRatiosProtonToyMC.at(mcEvent));
  }

}


double TrdQt::GetTrdPdfValue(int xenonBin, int rigidityBin, unsigned short layer, float deDx, int idParticle) const {

  return Utilities::TrdPdfLookup::Self()->GetTrdPdfValue(xenonBin, rigidityBin, layer, deDx, (Utilities::ParticleId)idParticle);
}


short TrdQt::ParticleID(float trackerCharge, float rigidity) {

  // FIXME: This should not be in TrdQt. It is purely tracker-related code. -> Selector
  int Q = rigidity < 0. ? -1 : 1;

  int IdPart = 1;
  if (Q < 0) IdPart = 0;

  int Z = 0;
  for (unsigned int i = 0; i < Cuts::gTrackerCharges; ++i) {
    if (fabs(trackerCharge - Cuts::gTrackerChargeMean[i]) < 3 * Cuts::gTrackerChargeSigma[i]) {
      Z = i+1;
      break;
    }
  }

  if (Z == 0) { return -1; }

  if (Z==1) {
    if (Q==+1) IdPart = 1;       // Proton
    else if (Q==-1) IdPart = 0;  // Electron
  }
  else IdPart = Z;

  return IdPart;
}


const std::vector<Analysis::TrdHit>& TrdQt::GetUnassignedHits() const {

    return d->fCandidateMatching.UnassignedHits();
}

const std::vector<Analysis::TrdHit>& TrdQt::GetAssignedHits() const {

    return d->fCandidateMatching.AssignedHits();
}

unsigned short TrdQt::GetNumberOfActiveLayers() const {

    return d->fCandidateMatching.GetNumberOfActiveLayers();
}

unsigned short TrdQt::GetNumberOfActiveStraws() const {

    return d->fCandidateMatching.GetNumberOfActiveStraws();
}

float TrdQt::GetActivePathLength() const {

    return d->fCandidateMatching.GetActivePathLength();
}

unsigned short TrdQt::GetNumberOfCandidateLayers() const {

  return d->fCandidateMatching.Preselection().GetNumberOfCandidateLayers();
}

unsigned short TrdQt::GetNumberOfCandidateStraws() const {

  return d->fCandidateMatching.Preselection().GetNumberOfCandidateStraws();
}

bool TrdQt::IsInsideTrdGeometricalAcceptance() const {

  return d->fCandidateMatching.Preselection().IsInsideTrdGeometricalAcceptance();
}

bool TrdQt::PassesTrdPreselectionCuts() const {

  return d->fCandidateMatching.PassesTrdPreselectionCuts();
}

bool TrdQt::UsefulForTrdParticleId() const {

  return d->fCandidateMatching.UsefulForTrdParticleId();
}

double TrdQt::LogLikelihoodRatioElectronProton() const {

  if (d->fLogLikelihoodRatiosProton.empty())
    return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosProton.at(0);
}

double TrdQt::LogLikelihoodRatioHeliumElectron() const {

  if (d->fLogLikelihoodRatiosProton.empty())
    return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosElectron.at(2);
}

double TrdQt::LogLikelihoodRatioHeliumProton() const {

  if (d->fLogLikelihoodRatiosProton.empty())
    return std::numeric_limits<float>::max() - 1;
  return d->fLogLikelihoodRatiosProton.at(2);
}

float TrdQt::GetCandidatePathLength() const {

  return d->fCandidateMatching.Preselection().GetCandidatePathLength();
}

const std::vector<double>& TrdQt::LogLikelihoodRatiosElectrons() const {

  return d->fLogLikelihoodRatiosElectron;
}

const std::vector<double>& TrdQt::LogLikelihoodRatiosProtons() const {

  return d->fLogLikelihoodRatiosProton;
}

const std::vector<std::vector<double> > & TrdQt::LogLikelihoodRatiosElectronsToyMC() const {

  return d->fLogLikelihoodRatiosElectronToyMC;
}

const std::vector<std::vector<double> >& TrdQt::LogLikelihoodRatiosProtonsToyMC() const {

  return d->fLogLikelihoodRatiosProtonToyMC;
}

const std::vector<double>& TrdQt::GetPvalueVector() const {

  return d->fPvalues;
}
