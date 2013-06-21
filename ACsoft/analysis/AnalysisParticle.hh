#ifndef ANALYSIS_PARTICLE_HH
#define ANALYSIS_PARTICLE_HH

// This file is named AnalysisParticle.hh to avoid clashes with Particle.h from acroot/.
// When building ACsoft within AMS both Particle.C files would produce Particle.o
// objects - and linking would fail.

#include <vector>
#include <Rtypes.h>
#include <TTimeStamp.h>
#include <TVector3.h>

#ifndef __CINT__
#include "TrdHit.hh"
#endif

namespace ACsoft {

namespace AC {
class Event;
class Particle;
class TrackerTrack;
class TrackerTrackFit;
class ECALShower;
class TOFBeta;
class TRDVTrack;
class RICHRing;
}

namespace IO {
  class ReducedEvent;
}

namespace Utilities {
  class RTIData;
  class McSpectrumScaler;
}

class TrdQt;

namespace Analysis {

class ParticleFactory;
class SplineTrack;
class TrdTrack;

/** High-level analysis object.
  *
  * Filled by ParticleFactory.
  */
class Particle {

public:
  /** enum identifier to specify which ToF charge you want to calculate in the TofChargeFor() function. */
  enum TofChargeType {TofMean, TofMax, TofMin, TofLay1, TofLay2, TofLay3, TofLay4};
  /** enum identifier to specify which Tracker charge you want to calculate in the TrackerChargeFor() function. */
  enum TrackerChargeType {TrkMean, TrkMax, TrkMin, TrkLay1, TrkLay2, TrkLay3, TrkLay4, TrkLay5, TrkLay6, TrkLay7, TrkLay8, TrkLay9};

  Particle();
  Particle( const AC::Event* rawevent, const AC::Particle* amsParticle, const AC::TrackerTrack* trackerTrack,
            const AC::TrackerTrackFit* trackerTrackFit, const AC::ECALShower* shower, const AC::TOFBeta* beta,
            const AC::TRDVTrack* trdvtrk, const AC::RICHRing* richring );
  Particle( const IO::ReducedEvent* reducedEvent );

  virtual ~Particle();

  // These AC:: classes are only available when reading ACQt files, not on ACROOT files.
  const AC::Event* RawEvent() const { return fRawEvent; }
  const AC::Particle* AmsParticle() const { return fAmsParticle; }
  const AC::TrackerTrack* TrackerTrack() const { return fTrackerTrack; }
  const AC::TrackerTrackFit* TrackerTrackFit() const { return fTrackerTrackFit; }
  int TrackerTrackFitID() const { return fTrackerTrackFitID; }
  const AC::ECALShower* EcalShower() const { return fEcalShower; }
  const AC::TOFBeta* TofBeta() const { return fTofBeta; }
  const AC::TRDVTrack* TrdVTrack() const { return fTrdVTrack; }
  const AC::RICHRing* RichRing() const { return fRichRing; }

  // The following methods work on both ACQt & ACROOT files.
  const std::vector<TrdHit>& TrdHitsFromTrackerTrack() const { return fTrdHitsFromTrackerTrack; }
  unsigned int NumberOfTrdHitsFromTrackerTrack() const { return fTrdHitsFromTrackerTrack.size(); }

  const std::vector<TrdHit>& TrdHitsFromTrdTrack() const { return fTrdHitsFromTrdTrack; }
  unsigned int NumberOfTrdHitsFromTrdTrack() const { return fTrdHitsFromTrdTrack.size(); }

  const SplineTrack* GetSplineTrack() const { return fSplineTrack; }
  SplineTrack* GetSplineTrack() { return fSplineTrack; }

  void Dump() const;

  bool IsEmpty() const;

  void Clear();

  Float_t TofMeanEnergy() const;
  Float_t TofMaxEnergy() const;

  Int_t       Run() const;
  Int_t       Event() const;
  Int_t       RunTag() const;
  Double_t    EventTime() const;
  TTimeStamp  TimeStamp() const;
  UChar_t     EventRandom() const;

  Float_t TriggerRate() const;
  Float_t TriggerLiveTime() const;
  UChar_t TriggerFlags() const;
  Float_t LongitudeDegree() const;
  Float_t LatitudeDegree() const;
  Float_t MagneticLatitudeDegree() const;
  Float_t MagneticLongitudeDegree() const;
  Float_t GeomagneticMaxCutOff() const;

  Char_t  NumberOfAccHits() const;
  Char_t  NumberOfTofHits() const;
  Char_t  NumberOfEcalShower() const;
  Char_t  NumberOfTrackerTracks() const;
  Short_t NumberOfTrdRawHits() const;
  Short_t NumberOfParticles() const;
  Short_t NumberOfRichRings() const;

  Int_t TrackHits() const;
  Short_t TrackerLayerPatternClassification() const;
  Float_t Rigidity() const;
  Float_t InverseRigidityError() const;
  Float_t Chi2TrackerX() const;
  Float_t Chi2TrackerY() const;

  std::vector<Float_t> Rigidities() const;
  std::vector<Float_t> TrackerPhi() const;
  std::vector<Float_t> TrackerTheta() const;
  std::vector<Float_t> TrackerXcoordinates() const;
  std::vector<Float_t> TrackerYcoordinates() const;
  std::vector<Float_t> TrackerXhits() const;
  std::vector<Float_t> TrackerYhits() const;
  Float_t TrackerCharge() const;
  Float_t TrackerChargeFor(TrackerChargeType) const;
  Float_t TrackerChargeError() const;
  Float_t TrackerChargeConfusionLikelihood() const;
  void FillTrackerCharges(std::vector<Float_t>&) const;

  Float_t BetaTof() const;
  Float_t TofCharge() const;
  Float_t TofChargeError() const;
  Float_t UpperTofCharge() const;
  Float_t LowerTofCharge() const;
  void FillTofCharges(std::vector<Float_t>&) const;
  Float_t TofChargeFor(TofChargeType) const;

  Float_t RichCharge() const;
  Float_t RichChargeError() const;
  Float_t RichBeta() const;

  Float_t  EcalEnergy() const;
  Float_t  EcalEnergyElectron() const;
  Float_t  EcalBDT() const { return EcalBDTv4(); }
  Float_t  EcalBDTv4() const;
  Float_t  EcalBDTv5() const;
  Float_t  EcalStandaloneEstimatorV3() const;
  Float_t  EcalSum() const;
  TVector3 EcalCentreOfGravity() const;
  Float_t  EcalTheta() const;
  Float_t  EcalPhi() const;

  Short_t McParticleId() const;
  Float_t McMomentum() const;
  Float_t McMinimumMomentum() const;
  Float_t McMaximumMomentum() const;
  ULong64_t McNumberOfTriggeredEvents() const;

  const Utilities::RTIData* RTIData() const;

  bool HasTrackerTrack() const;
  bool HasTrackerTrackFit() const;
  bool HasEcalShower() const;
  bool HasEcalShowerEstimator() const;
  bool HasTofBeta() const;
  bool HasTofCharge() const;
  bool HasTrackerCharge() const;
  bool HasRichRing() const;
  bool HasPrimaryEventGenerator() const;

  bool HasTrdTrack() const { return (fTrdTrack!=0); }
  const TrdTrack* GetTrdTrack() const { return fTrdTrack; }

  bool IsMC() const;
  bool IsBeamTest() const;
  bool IsISS() const;

  Double_t McWeight() const;

  /** Returns true if the underlying event is an ACQt event, with full access to all variables. */
  bool HasRawEventData() const { return fRawEvent; }

protected:

  friend class ACsoft::TrdQt;
  friend class ParticleFactory;

  /// pointer to underlying raw ACQt event
  const AC::Event* fRawEvent;
  const IO::ReducedEvent* fReducedEvent;

  const AC::Particle* fAmsParticle;
  const AC::TrackerTrack* fTrackerTrack;
  const AC::TrackerTrackFit* fTrackerTrackFit;
  /// a number that can be used to identify which track fit was chosen as the fit (used e.g. by ReducedEventWriter)
  int fTrackerTrackFitID;
  const AC::ECALShower* fEcalShower;
  const AC::TOFBeta* fTofBeta;
  const AC::TRDVTrack* fTrdVTrack;
  const AC::RICHRing* fRichRing;


  std::vector<TrdHit> fTrdHitsFromTrackerTrack; ///< vector of TrdHit objects, calculated from tracker SplineTrack selected for this particle
  std::vector<TrdHit> fTrdHitsFromTrdTrack; ///< vector of TrdHit objects, calculated from %TRD track created for this particle
  SplineTrack* fSplineTrack;    ///< spline track corresponding to \c fTrackerTrackFit
  TrdTrack* fTrdTrack; ///< TRD track, if any, closest to tracker spline track
  Utilities::McSpectrumScaler* fMcScaler;

  // Only used to be able to construct a Particle that can be used to apply bad run & RTI cuts, for just a given unix second.
  TTimeStamp fOverrideTimeStamp;
};
}

}

#endif
