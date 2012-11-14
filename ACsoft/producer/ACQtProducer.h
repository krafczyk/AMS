#ifndef ACQtProducer_h
#define ACQtProducer_h

#include <vector>

class AMSEventR;
class ChargeR;
class TrTrackR;
struct TRDExtraHit;

namespace AC {
  class Event;
  class Particle;
  class RunHeader;
  class TrackerTrack;
}

/** Produces ACQt events from a a single AMSEvent.
  *
  * If you intent to produce a full AC::Event object for a given AMSEventR, use ProduceFullEvent():
  *
  * ACQtProducer producer;
  * 
  * bool successfull = producer.ProduceFullEvent(theAMSEvent, true, 10); // For the first event of a run.
  * if (successfull) {
  *   const AC::Event& producedEvent = producer.ProducedEvent();
  *   ...
  * }
  * 
  * For the next N events:
  * bool successfull = producer.ProduceFullEvent(theAMSEvent); // For the following N events of a run.
  * if (successfull) {
  *   const AC::Event& producedEvent = producer.ProducedEvent();
  *   ...
  * }
  *
  * If you intent to produce all parts individually (to be able to benchmark times spent in each step)
  * or only a subset of an AC::Event call ProducePart() as in this example:
  *
  * ACQtProducer producer;
  * producer.PrepareProductionForEvent(theAMSEvent);
  * if (producer.ProduceRunHeader(numberOfEventsInRun)) {
  *   bool successfull = producer.ProducePart(ProduceACCData);
  *   ...
  *   successfull = producer.ProducePart(ProduceTRDData);
  *   ...
  *   if (successsfull) {
  *     const AC::Event& producedEvent = producer.ProducedEvent();
  *     ...
  *   }
  * }
  */
class ACQtProducer {
public:
  ACQtProducer();
  ~ACQtProducer();

  /** Produces a full AC::Event object with all subcomponents filled from an AMSEventR.
    * If you intend to produce more than one event from a run, you should set produceRunHeader=true
    * on the first event of a run, and supply the correct number of entries in the run which are then
    * filled in the run header.
    */
  bool ProduceFullEvent(AMSEventR*, bool produceRunHeader = false, int entriesInRun = 1);

  /** Describes a list of all fields that can be filled into an AC::Event from an AMSEventR.
    * The ordering is relevant! Don't change this enumeration without discussion.
    * ac_producer produces a full AC::Event in that order, we should keep it consistent.
    */
  enum ProductionSelection {
    ProduceNothing = 0,
    ProduceEventHeaderData,
    ProduceParticlesData,
    ProduceTriggerData,
    ProduceTOFData,
    ProduceACCData,
    ProduceTrackerData,
    ProduceTRDData,
    ProduceRICHData,
    ProduceECALData,
    ProduceChargesData,
    ProduceMCData,
    ProduceLast = ProduceMCData + 1
  };

  /** Produce a new run header. This should be done once for a series of events grouped in a run. */
  bool ProduceRunHeader(int entries);

  /** Setup production of a new event. Call ProducePart() for any parts you want to fill
    * in the AC::Event, then use FinishProductionForEvent() to finish the event production.
    *
    * After that you cal call PrepareProductionForEvent() again with a different event and so on.
    * ac_producer uses this instead of ProduceFullEvent() to benchmark the time spent in each stage.
    */
  void PrepareProductionForEvent(AMSEventR*);

  /** Produce a single part of an AC::Event. \sa ProductionSelection flags.
    * Can be called multple times for a single event, to compose the needed parts.
    */
  bool ProducePart(ProductionSelection);

  /** Call this after you're done processing parts of the AC::Event. */
  void FinishProductionForEvent(AMSEventR*);

  /** Access the last produced run header. */
  const AC::RunHeader* ProducedRunHeader() const { return fRunHeader; }

  /** Access the last produced event. */
  const AC::Event& ProducedEvent() const { return *fEvent; }

  /** Use with care! ac_producer has a complex algorithm to determine if it should use tracker external alignment
    * The default is true, only use this if you're sure you want it.
    *
    * When the value is set to true, ProduceTrackerData will include additional track fits wrt. to external alignment.
    */
  void SetUseTrackerExternalAlignment(bool value) { fUseTrackerExternalAlignment = value; }
 
  /** Use with care! ac_producer uses this method to switch between operation modes on standard/passN files. */
  void SetIsProcessingEventsFromStandardRun(bool value) { fIsProcessingEventsFromStandardRun = value; }

  /** Use with care! Only produce one single track fit, instead of multiple ones. */
  void SetProduceSingleTrackFitOnly(int algorithm, int pattern, int refit) {

    fAlgorithm = algorithm;
    fPattern = pattern;
    fRefit = refit;
  }

private:
  bool ProduceACC();
  bool ProduceChargesForRICH(const AC::Particle&, ChargeR*);
  bool ProduceChargesForTOF(const AC::Particle&, ChargeR*);
  bool ProduceChargesForTRD(const AC::Particle&, ChargeR*);
  bool ProduceChargesForTracker(const AC::Particle&, ChargeR*);
  bool ProduceCharges();
  bool ProduceEventHeader();
  bool ProduceECAL();
  bool ProduceParticles();
  bool ProduceRICH();
  bool ProduceTrackerTrackFit(AC::TrackerTrack&, TrTrackR*, int algorithm, int pattern, int refit, float beta);
  bool ProduceTrackerTrackFits(AC::TrackerTrack&, TrTrackR*, float beta);
  bool ProduceTrackerTrackCharges(AC::TrackerTrack&, TrTrackR*, float beta);
  bool ProduceTrackerTrackReconstructedHits(AC::TrackerTrack&, TrTrackR*, float beta);
  bool ProduceTracker();
  bool ProduceTrigger();
  bool ProduceTOFBetas();
  bool ProduceTOFClusters();
  bool ProduceTOF();
  bool ProduceTRDHSegments();
  bool ProduceTRDHTracks();
  bool ProduceTRDVTracks();
  bool ProduceTRD();
  bool ProduceMC();

  AC::RunHeader* fRunHeader;
  AC::Event* fEvent;
  AMSEventR* fAMSEvent;
  bool fUseTrackerExternalAlignment;
  bool fIsProcessingEventsFromStandardRun;
  int fAlgorithm;
  int fPattern;
  int fRefit;
  short fMaxTrackerTracks;
  short fMaxRICHRings;
  short fMaxECALShowers;

  // ProduceTRD() specific data.
  std::vector<int> fTRDRawHitIndices;
  std::vector<TRDExtraHit> fTRDExtraHits;
};

struct TRDExtraHit {
  int hitIndex;
  float amplitude;
  float residual;
};

#endif