#ifndef ANALYSIS_PARTICLE_HH
#define ANALYSIS_PARTICLE_HH

// This file is named AnalysisParticle.hh to avoid clashes with Particle.h from acroot/.
// When building ACsoft within AMS both Particle.C files would produce Particle.o
// objects - and linking would fail.

#include <vector>
#include <Rtypes.h>
#include <TTimeStamp.h>

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

class TrdQt;

namespace Analysis {

class ParticleFactory;
class SplineTrack;
class TrdHit;

/** High-level analysis object.
  *
  * Filled by ParticleFactory.
  */
class Particle {

public:

  Particle();
  Particle( const AC::Event* rawevent, const AC::Particle* amsParticle, const AC::TrackerTrack* trackerTrack,
            const AC::TrackerTrackFit* trackerTrackFit, const AC::ECALShower* shower, const AC::TOFBeta* beta,
           const AC::TRDVTrack* trdvtrk, const AC::RICHRing* richring );
  Particle( const IO::ReducedEvent* reducedEvent );

  virtual ~Particle();

  // These AC:: classes are only available when reading ACQt files, not on ACROOT files.
  const AC::Event* RawEvent() const { return fRawEvent; }
  const AC::Particle* MainAmsParticle() const { return fMainAmsParticle; }
  const AC::TrackerTrack* MainTrackerTrack() const { return fMainTrackerTrack; }
  const AC::TrackerTrackFit* MainTrackerTrackFit() const { return fMainTrackerTrackFit; }
  int TrackerTrackFitID() const { return fTrackerTrackFitID; }
  const AC::ECALShower* MainEcalShower() const { return fMainEcalShower; }
  const AC::TOFBeta* TofBeta() const { return fTofBeta; }
  const AC::TRDVTrack* MainTrdVTrack() const { return fMainTrdVTrack; }
  const AC::RICHRing* MainRichRing() const { return fMainRichRing; }

  // The following methods work on both ACQt & ACROOT files.
  const std::vector<TrdHit>& TrdHits() const { return fTrdHits; }
  std::vector<TrdHit>& TrdHits() { return fTrdHits; }
  const SplineTrack* GetSplineTrack() const { return fSplineTrack; }
  SplineTrack* GetSplineTrack() { return fSplineTrack; }

  void Dump() const;

  bool IsEmpty() const;

  Float_t TofMeanEnergy() const;
  Float_t TofMaxEnergy() const;

  Int_t Run() const;
  Int_t Event() const;
  Int_t RunTag() const;
  Int_t BadRunTag() const;
  Double_t EventTime() const;
  TTimeStamp TimeStamp() const;
  UChar_t  EventRandom() const;

  Float_t TriggerRate() const;
  Float_t TriggerLiveTime() const;
  Float_t LongitudeDegree() const;
  Float_t LatitudeDegree() const;
  Float_t MagneticLatitudeDegree() const;
  Float_t MagneticLongitudeDegree() const;
  Float_t GeomagneticMaxCutOff() const;
  Float_t GeomagneticMaxCutOffCone() const;

  Char_t NumberOfAccHits() const;
  Char_t NumberOfTofHits() const;
  Char_t NumberOfEcalShower() const;
  Char_t NumberOfTrackerTracks() const;
  Short_t NumberOfTrdRawHits() const;
  Short_t NumberOfParticles() const;

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
  Float_t TrackerSignalLayer1() const;

  Float_t TofCharge() const;
  Float_t TofChargeError() const;
  Float_t TofChargeLayer1() const;
  Float_t TofChargeLayer2() const;
  Float_t TofChargeLayer3() const;
  Float_t TofChargeLayer4() const;
  Float_t TrackerCharge() const;
  Float_t TrackerChargeError() const;
  Float_t RichCharge() const;
  Float_t RichChargeError() const;
  Float_t BetaTof() const;
  Float_t RichBeta() const;
  Float_t EcalEnergy() const;
  Float_t EcalBDT() const;
  Float_t EcalSum() const;

  bool HasMainTrackerTrack() const;
  bool HasMainTrackerTrackFit() const;
  bool HasMainEcalShower() const;
  bool HasMainEcalShowerEstimator() const;
  bool HasTofBeta() const;
  bool HasTofCharge() const;
  bool HasMainTrackerCharge() const;
  bool HasMainRichRing() const;

  /** Returns true if the underlying event is an ACQt event, with full access to all variables. */
  bool HasRawEventData() const { return fRawEvent; }

protected:

  friend class ACsoft::TrdQt;
  friend class ParticleFactory;

  /// pointer to underlying raw ACQt event
  const AC::Event* fRawEvent;
  const IO::ReducedEvent* fReducedEvent;

  const AC::Particle* fMainAmsParticle;
  const AC::TrackerTrack* fMainTrackerTrack;
  const AC::TrackerTrackFit* fMainTrackerTrackFit;
  /// a number that can be used to identify which track fit was chosen as the main fit (used e.g. by EventWriter)
  int fTrackerTrackFitID;
  const AC::ECALShower* fMainEcalShower;
  const AC::TOFBeta* fTofBeta;
  const AC::TRDVTrack* fMainTrdVTrack;
  const AC::RICHRing* fMainRichRing;


  std::vector<TrdHit> fTrdHits; ///< vector of TrdHit objects, calculated from objects selected for this particle
  SplineTrack* fSplineTrack;    ///< spline track corresponding to \c fMainTrackerTrackFit

};
}

}

#endif
