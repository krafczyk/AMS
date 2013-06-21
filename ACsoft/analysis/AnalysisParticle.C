#include "AnalysisParticle.hh"

#include "Event.h"
#include "TrdHit.hh"
#include "Settings.h"
#include "SplineTrack.hh"
#include "TrdTrack.hh"
#include "Statistics.hh"
#include "ReducedEvent.hh"
#include "BadRunManager.hh"
#include "TOFBeta.h"
#include "RTIData.hh"
#include "RTIReader.hh"
#include "SimpleGraphLookup.hh"
#include "McSpectrumScaler.hh"

#include <iostream>

#include <TMath.h>

#define DEBUG 0
#define INFO_OUT_TAG "Particle> "
#include <debugging.hh>
#include <QDebug>
#include <QStringList>

ACsoft::Analysis::Particle::Particle() :
  fRawEvent(0),
  fReducedEvent(0),
  fAmsParticle(0),
  fTrackerTrack(0),
  fTrackerTrackFit(0),
  fTrackerTrackFitID(-1),
  fEcalShower(0),
  fTofBeta(0),
  fTrdVTrack(0),
  fRichRing(0),
  fTrdHitsFromTrackerTrack(),
  fTrdHitsFromTrdTrack(),
  fSplineTrack(0),
  fTrdTrack(0),
  fMcScaler(0),
  fOverrideTimeStamp(TTimeStamp(0, 0)) {

  fTrdHitsFromTrackerTrack.reserve(40);
}

/** Constructor to manually choose the components that make up the particle.
  *
  * Set some of the objects to null if you do not want to associate any object with this particle.
  *
  * You can later call FillHighLevelObjects() in a suitable ParticleFactory to fill the high-level stuff (TrdHits, SplineTrack).
  *
  * \attention No one keeps you from choosing a certain TrackerTrack and a TrackerTrackFit that belongs to a different track.
  *
  */
ACsoft::Analysis::Particle::Particle( const ACsoft::AC::Event* rawevent, const ACsoft::AC::Particle* amsParticle, const ACsoft::AC::TrackerTrack* trackerTrack,
                              const ACsoft::AC::TrackerTrackFit* trackerTrackFit, const ACsoft::AC::ECALShower* shower, const ACsoft::AC::TOFBeta* beta,
                              const ACsoft::AC::TRDVTrack* trdvtrk, const ACsoft::AC::RICHRing* richring ) :
  fRawEvent(rawevent),
  fReducedEvent(0),
  fAmsParticle(amsParticle),
  fTrackerTrack(trackerTrack),
  fTrackerTrackFit(trackerTrackFit),
  fTrackerTrackFitID(-1),
  fEcalShower(shower),
  fTofBeta(beta),
  fTrdVTrack(trdvtrk),
  fRichRing(richring),
  fTrdHitsFromTrackerTrack(),
  fTrdHitsFromTrdTrack(),
  fSplineTrack(0),
  fTrdTrack(0),
  fMcScaler(0),
  fOverrideTimeStamp(TTimeStamp(0, 0)) {

  fTrdHitsFromTrackerTrack.reserve(40);
  if( !fTrackerTrackFit ){
    WARN_OUT << "No main tracker track fit! Please use suitable preselection cuts to prevent this!" << std::endl;
    return;
  }
}

ACsoft::Analysis::Particle::Particle( const IO::ReducedEvent* reducedEvent ) :
  fRawEvent(0),
  fReducedEvent(reducedEvent),
  fAmsParticle(0),
  fTrackerTrack(0),
  fTrackerTrackFit(0),
  fTrackerTrackFitID(-1),
  fEcalShower(0),
  fTofBeta(0),
  fTrdVTrack(0),
  fRichRing(0),
  fTrdHitsFromTrackerTrack(),
  fTrdHitsFromTrdTrack(),
  fSplineTrack(0),
  fTrdTrack(0),
  fMcScaler(0)
{
  // FIXME why not initialize fOverrideTimeStamp in this constructor?
  fTrdHitsFromTrackerTrack.reserve(40);
}

ACsoft::Analysis::Particle::~Particle() {
  Clear();
}

bool ACsoft::Analysis::Particle::IsEmpty() const {

  return( !fSplineTrack );
}

void ACsoft::Analysis::Particle::Clear() {

  fTrdHitsFromTrackerTrack.clear();
  fTrdHitsFromTrdTrack.clear();
  if( fSplineTrack ){
    delete fSplineTrack;
    fSplineTrack = 0;
  }
  if( fTrdTrack ){
    delete fTrdTrack;
    fTrdTrack = 0;
  }
}

void ACsoft::Analysis::Particle::Dump() const {

  if(fRawEvent) {
    std::cout << "[ACQt Event]\n";
    fRawEvent->Dump();
  } else {
    std::cout << "[ACROOT Event]\n";
    // FIXME: fReducedEvent->Print();
  }

  if( fTrdHitsFromTrackerTrack.size() > 0 ){
    std::cout << "  [TrdHitsFromTrackerTrack]" << std::endl;
    for( unsigned int i=0 ; i<fTrdHitsFromTrackerTrack.size() ; ++i )
      std::cout << fTrdHitsFromTrackerTrack[i] << std::endl;
  }

  if( fSplineTrack ){
    std::cout << "  [SplineTrack]" << std::endl;
    fSplineTrack->Dump();
  }

  if( fTrdTrack )
    fTrdTrack->Dump();

  std::cout << std::endl;
}

/** Returns the mean energy of clusters contained in TOFBeta object.
  * See ACsoft::AC::TOF::CalculateMeanEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofMeanEnergy() const {

  if (fReducedEvent) {
    // FIXME This is nasty since it breaks the symmetry between ACQT and ACROOT analysis.
    WARN_OUT << "TofMeanEnergy() not available on ACROOT files." << std::endl;
    return -1.0;
  }

  assert(fRawEvent);
  return fTofBeta ? fRawEvent->TOF().CalculateMeanEnergy(*fTofBeta) : -1;
}

/** Returns the highest energy of clusters contained in TOFBeta object.
  * See ACsoft::AC::TOF::CalculateMaximumEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofMaxEnergy() const {

  if (fReducedEvent) {
    WARN_OUT << "TofMaxEnergy() not available on ACROOT files." << std::endl;
    return -1.0;
  }

  assert(fRawEvent);
  return fTofBeta ? fRawEvent->TOF().CalculateMaximumEnergy(*fTofBeta) : -1;
}

/** Returns the run number of the underlying event.
  * See ACsoft::AC::RunHeader::Run for full documentation.
  */
Int_t ACsoft::Analysis::Particle::Run() const {

  // If there is no underlying event, but an override time stamp, return the run number corresponding to the override time stamp.
  if (fOverrideTimeStamp != TTimeStamp(0, 0)) {
    assert(!fReducedEvent);
    assert(!fRawEvent);
    const ACsoft::Utilities::RTIData* rtiData = RTIData();
    assert(rtiData);
    return rtiData->GetRun();
  }

  if( fReducedEvent )
    return fReducedEvent->Run;
  assert(fRawEvent);
  return fRawEvent->RunHeader()->Run();
}

/** Returns the event number of the underlying event.
  * See ACsoft::AC::EventHeader::Event for full documentation.
  */
Int_t ACsoft::Analysis::Particle::Event() const {

  if( fReducedEvent )
    return fReducedEvent->Event;
  assert(fRawEvent);
  return fRawEvent->EventHeader().Event();
}

/** Returns the encoded run tag.
  * See ACsoft::AC::RunHeader::RunTag for full documentation.
  */
Int_t ACsoft::Analysis::Particle::RunTag() const {

  if( fReducedEvent )
    return fReducedEvent->RunTag;
  assert(fRawEvent);
  return fRawEvent->RunHeader()->RunTag();
}

/** Returns the event time (in seconds).
  * See ACsoft::AC::EventHeader::TimeStamp for full documentation.
  */
Double_t ACsoft::Analysis::Particle::EventTime() const {

  if (fOverrideTimeStamp) return fOverrideTimeStamp.AsDouble();
  if( fReducedEvent )
    return fReducedEvent->EventTime;
  assert(fRawEvent);
  return fRawEvent->EventHeader().TimeStamp().AsDouble();
}

/** Returns the event time (as TTimeStamp).
  * See ACsoft::AC::EventHeader::TimeStamp for full documentation.
  */
TTimeStamp ACsoft::Analysis::Particle::TimeStamp() const {

  if (fOverrideTimeStamp != TTimeStamp(0, 0)) return fOverrideTimeStamp;

  if( fReducedEvent ){
    time_t sec = time_t(fReducedEvent->EventTime);
    int nsec   = int(1.e9*(fReducedEvent->EventTime - sec));
    return TTimeStamp(sec,nsec);
  }
  assert(fRawEvent);
  return fRawEvent->EventHeader().TimeStamp();
}


/** Returns a random number stored in each event, which can be used for sample splitting.
  * See ACsoft::AC::EventHeader::Random for full documentation.
  */
UChar_t  ACsoft::Analysis::Particle::EventRandom() const {

  if( fReducedEvent )
    return fReducedEvent->EventRndm;
  assert(fRawEvent);
  return fRawEvent->EventHeader().Random();
}

/** Returns the trigger rate.
  * See ACsoft::AC::Trigger::TriggerRateFT for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TriggerRate() const {

  if( fReducedEvent )
    return fReducedEvent->TriggerRate;
  assert(fRawEvent);
  if ( !fRawEvent->Trigger().HasData() )
    return -1.;
  return fRawEvent->Trigger().TriggerRateFT();
}

/** Returns the trigger bits.
  * See ACsoft::AC::Trigger::PhysBFlags for full documentation.
  */
UChar_t ACsoft::Analysis::Particle::TriggerFlags() const {

  if( fReducedEvent )
    return fReducedEvent->TriggerFlags;
  assert(fRawEvent);
  return fRawEvent->Trigger().PhysBFlags();
}


/** Returns the trigger live time.
  * See ACsoft::AC::Trigger::TriggerLiveTime for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TriggerLiveTime() const {

  if( fReducedEvent )
    return fReducedEvent->TriggerLifeTime;
  assert(fRawEvent);
  if ( !fRawEvent->Trigger().HasData() )
    return -1.;
  return fRawEvent->Trigger().LiveTime();
}

/** Returns the ISS longitude.
  * See ACsoft::AC::EventHeader::ISSLongitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::LongitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->LongitudeDeg;
  assert(fRawEvent);
  Float_t longitudeDeg = fRawEvent->EventHeader().ISSLongitude();
  if ( longitudeDeg > 180.0 )
    longitudeDeg -= 360.0;
  return longitudeDeg;
}

/** Returns the ISS latitude.
  * See ACsoft::AC::EventHeader::ISSLatitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::LatitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->LatitudeDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().ISSLatitude();
}

/** Returns the magnetic latitude.
  * See ACsoft::AC::EventHeader::MagneticLatitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::MagneticLatitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->MagLatDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().MagneticLatitude();
}

/** Returns the magnetic longitude.
  * See ACsoft::AC::EventHeader::MagneticLongitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::MagneticLongitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->MagLongDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().MagneticLongitude();
}

/** Returns the geomagnetic cut off in a 40 deg field of view.
  * This only changes with the event time and is extracted from RTI data.
  */
Float_t ACsoft::Analysis::Particle::GeomagneticMaxCutOff() const {

  if( fReducedEvent )
    return fReducedEvent->GeomagneticMaxCutOff;
  assert(fRawEvent);
  const ACsoft::Utilities::RTIData* rtiData = RTIData();
  assert(rtiData);
  double negCutoff = fabs(rtiData->GetCutoff40N());
  double posCutoff = fabs(rtiData->GetCutoff40P());
  return std::max(negCutoff, posCutoff);
}

/** Returns the number of %ACC hits (clusters). */
Char_t ACsoft::Analysis::Particle::NumberOfAccHits() const {

  if( fReducedEvent )
    return fReducedEvent->nAccHits;
  assert(fRawEvent);
  return fRawEvent->ACC().Clusters().size();
}

/** Returns the number of %TOF hits (clusters). */
Char_t ACsoft::Analysis::Particle::NumberOfTofHits() const {

  if( fReducedEvent )
    return fReducedEvent->nTofHits;
  assert(fRawEvent);
  return fRawEvent->TOF().Clusters().size();
}

/** Returns the number of %ECAL showers. */
Char_t ACsoft::Analysis::Particle::NumberOfEcalShower() const {

  if( fReducedEvent )
    return fReducedEvent->nEcalShr;
  assert(fRawEvent);
  return fRawEvent->ECAL().Showers().size();
}

/** Returns the number of %Tracker tracks. */
Char_t ACsoft::Analysis::Particle::NumberOfTrackerTracks() const {

  if( fReducedEvent )
    return fReducedEvent->nTrkTrks;
  assert(fRawEvent);
  return fRawEvent->Tracker().Tracks().size();
}

/** Returns the number of %TRD raw hits. */
Short_t ACsoft::Analysis::Particle::NumberOfTrdRawHits() const {

  if( fReducedEvent )
    return fReducedEvent->nTrdRawHits;
  assert(fRawEvent);
  return fRawEvent->TRD().NumberOfHits();
}

/** Returns the number of particles in the event. */
Short_t ACsoft::Analysis::Particle::NumberOfParticles() const {

  if( fReducedEvent )
    return 1; // FIXME: We currently only write single particles in the ACROOT Files.
  assert(fRawEvent);
  return fRawEvent->Particles().size();
}

/** Returns the number of %RICH rings in the event. */
Short_t ACsoft::Analysis::Particle::NumberOfRichRings() const {

  if( fReducedEvent )
    return 0; // FIXME: We have no RICH rings in the ACROOT Files.
  assert(fRawEvent);
  return fRawEvent->RICH().Rings().size();
}

/** Returns a bit pattern describing the which layers have a hit and which track fit was used.
  * \todo Add more documentation, this is not sufficient, but it's also not 100% clear what happens here.
  */
Int_t ACsoft::Analysis::Particle::TrackHits() const {

  if( fReducedEvent )
    return fReducedEvent->TrkHits;
  assert(fRawEvent);
  if (!TrackerTrack())
    return 0;
  Int_t TrkHits = TrackerTrack()->MakeBitPattern().to_ulong();
  // Which track fit is used ?
  int TrkFit = TrackerTrackFitID();
  std::bitset<32> TrkBits=(TrkHits);
  TrkBits.set(10+TrkFit);//FIXME: Really bad! Who takes care that we do not overwrite the higher bits?
  TrkHits = TrkBits.to_ulong();
  return TrkHits;
}


/** Classify tracker hit pattern.
  *
  * See ACsoft::AC::TrackerTrack::TrackerLayerPatternClassification().
  */
Short_t ACsoft::Analysis::Particle::TrackerLayerPatternClassification() const {

  if( fReducedEvent )
    return fReducedEvent->TrackerLayerPatternClassification;
  assert(fRawEvent);
  if( !TrackerTrack() ) return -2;
  return TrackerTrack()->TrackerLayerPatternClassification();
}


/** Returns the rigidity of the main tracker track fit which was used.
  * See ACsoft::AC::TrackerTrackFit::Rigidity for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Rigidity() const {

  if( fReducedEvent )
    return fReducedEvent->Rig;
  assert(fRawEvent);
  return TrackerTrackFit() ? TrackerTrackFit()->Rigidity() : 0;
}

/** Returns the error on inverse rigidity of the main tracker track fit which was used.
  * See ACsoft::AC::TrackerTrackFit::InverseRigidityError for full documentation.
  */
Float_t ACsoft::Analysis::Particle::InverseRigidityError() const {

  if( fReducedEvent )
    return fReducedEvent->eRinv;
  assert(fRawEvent);
  return TrackerTrackFit() ? TrackerTrackFit()->InverseRigidityError() : 0;
}

/** Returns the chisquare of the track fit x hits.
  * See ACsoft::AC::TrackerTrackFit::ChiSquareNormalizedX for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Chi2TrackerX() const {

  if (fReducedEvent) return fReducedEvent->Chi2TrkX;
  assert(fRawEvent);
  return TrackerTrackFit() ? TrackerTrackFit()->ChiSquareNormalizedX() : -1;
}

/** Returns the chisquare of the track fit y hits.
  * See ACsoft::AC::TrackerTrackFit::ChiSquareNormalizedY for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Chi2TrackerY() const {

  if (fReducedEvent) return fReducedEvent->Chi2TrkY;
  assert(fRawEvent);
  return TrackerTrackFit() ? TrackerTrackFit()->ChiSquareNormalizedY() : -1;
}

/** Returns a vector of rigidities corresponding to the various track fits produced by write_event_tree.
  * \sa IO::ReducedEventWriter
  * \todo Add more documentation, this is not sufficient.
  */
std::vector<Float_t> ACsoft::Analysis::Particle::Rigidities() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->Rigs) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->Rigs, fReducedEvent->Rigs + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> rigs(5, 0.);
  if (!TrackerTrack())
    return rigs;

  // Synchronize any change here with ACQtProducer-Tracker.C / AnalysisParticle / ParticleFactory.
  int forceRefitPG = ::AC::CurrentACQtVersion() >= 57 ? 3 : 2;
  int forceRefitMA = forceRefitPG + 10;

  const ACsoft::AC::TrackerTrack* TRKtrk = TrackerTrack();
  if (TRKtrk->GetFit(1,0,forceRefitPG)>=0) rigs[0] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,forceRefitPG)).Rigidity();
  if (TRKtrk->GetFit(1,0,forceRefitMA)>=0) rigs[1] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,forceRefitMA)).Rigidity();
  if (TRKtrk->GetFit(1,3,forceRefitPG)>=0) rigs[2] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,forceRefitPG)).Rigidity();
  if (TRKtrk->GetFit(1,3,forceRefitMA)>=0) rigs[3] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,forceRefitMA)).Rigidity();
  if (TRKtrk->GetFit(0,0,0)>=0) rigs[4] = TRKtrk->TrackFits().at(TRKtrk->GetFit(0,0,0)).Rigidity();

  return rigs;
}

/** Returns the tracker track phi angle at the upper TOF position (first entry), at tracker layer 1 (second entry) and at tracker layer 9 (third entry). */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerPhi() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkPhi) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkPhi, fReducedEvent->TrkPhi + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkPhi(3, 1000.);
  if (!TrackerTrackFit())
    return trkPhi;
  trkPhi[0] = TrackerTrackFit()->PhiTOFUpper() * TMath::DegToRad();
  trkPhi[1] = TrackerTrackFit()->PhiLayer1() * TMath::DegToRad();
  trkPhi[2] = TrackerTrackFit()->PhiLayer9() * TMath::DegToRad();
  return trkPhi;
}

/** Returns the tracker track theta angle at the upper TOF position (first entry), at tracker layer 1 (second entry) and at tracker layer 9 (third entry). */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerTheta() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkTht) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkTht, fReducedEvent->TrkTht + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkTht(3, 1000.);
  if (!TrackerTrackFit())
    return trkTht;
  trkTht[0] = TrackerTrackFit()->ThetaTOFUpper() * TMath::DegToRad();
  trkTht[1] = TrackerTrackFit()->ThetaLayer1() * TMath::DegToRad();
  trkTht[2] = TrackerTrackFit()->ThetaLayer9() * TMath::DegToRad();
  return trkTht;
}

/** Returns the tracker track x positions at upper TOF, TRD lower, TRD center, TRD upper, layer 1, layer 9.
  */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerXcoordinates() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkXcors) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkXcors, fReducedEvent->TrkXcors + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkXcors(6, 1000.);
  if (!TrackerTrackFit())
    return trkXcors;
  trkXcors[0] = TrackerTrackFit()->XTOFUpper();
  trkXcors[1] = TrackerTrackFit()->XTRDLower();
  trkXcors[2] = TrackerTrackFit()->XTRDCenter();
  trkXcors[3] = TrackerTrackFit()->XTRDUpper();
  trkXcors[4] = TrackerTrackFit()->XLayer1();
  trkXcors[5] = TrackerTrackFit()->XLayer9();// historical order, could be improved
  return trkXcors;
}

/** Returns the tracker track y positions at upper TOF, TRD lower, TRD center, TRD upper, layer 1, layer 9.
  */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerYcoordinates() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkYcors) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkYcors, fReducedEvent->TrkYcors + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkYcors(6, 1000.);
  if (!TrackerTrackFit())
    return trkYcors;
  trkYcors[0] = TrackerTrackFit()->YTOFUpper();
  trkYcors[1] = TrackerTrackFit()->YTRDLower();
  trkYcors[2] = TrackerTrackFit()->YTRDCenter();
  trkYcors[3] = TrackerTrackFit()->YTRDUpper();
  trkYcors[4] = TrackerTrackFit()->YLayer1();
  trkYcors[5] = TrackerTrackFit()->YLayer9();// historical order, could be improved
  return trkYcors;
}

/** Returns the tracker hit x positions in layer 1 and 9.
  * \todo This only returns two results maximum, and always the last reconstructed hits found that have a hit in layer 1 or 9. Is that desired?
  */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerXhits() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkXhits) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkXhits, fReducedEvent->TrkXhits + size);
  }
  assert(fRawEvent);
  // tracker hits in Layer 1 and 9
  std::vector<Float_t> hits(2, 0.);
  if (!TrackerTrack())
    return hits;
  const ACsoft::AC::TrackerTrack::ReconstructedHitsVector& rhits = TrackerTrack()->ReconstructedHits();
  for( unsigned int i=0 ; i < rhits.size() ; ++i ){
    const ACsoft::AC::TrackerReconstructedHit& hit = rhits[i];
    if( hit.Layer() == 1)
      hits[0] = hit.X();
    if( hit.Layer() == 9)
      hits[1] = hit.X();
  }
  return hits;
}

/** Returns the tracker hit y positions in layer 1 and 9.
  * \todo This only returns two results maximum, and always the last reconstructed hits found that have a hit in layer 1 or 9. Is that desired?
  */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerYhits() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkYhits) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkYhits, fReducedEvent->TrkYhits + size);
  }
  assert(fRawEvent);
  // tracker hits in Layer 1 and 9
  std::vector<Float_t> hits(2, 0.);
  if (!TrackerTrack())
    return hits;
  const ACsoft::AC::TrackerTrack::ReconstructedHitsVector& rhits = TrackerTrack()->ReconstructedHits();
  for( unsigned int i=0 ; i < rhits.size() ; ++i ){
    const ACsoft::AC::TrackerReconstructedHit& hit = rhits[i];
    if( hit.Layer() == 1)
      hits[0] = hit.Y();
    if( hit.Layer() == 9)
      hits[1] = hit.Y();
  }
  return hits;
}

/** Returns a vector of size 9 of the Tracker charge measurements
 *
 *  if a layer has no charge measured the value is set to -2000.
 *  if there is no MainTrakerTrack available the value is set to -1000.
 *
 *  the vector is sortet by layer ( layer1 = entry0 ... )
 *
 *  To access the charges use TrackerChargeFor() function.
 */
void ACsoft::Analysis::Particle::FillTrackerCharges(std::vector<Float_t>& charges) const {

  if( fReducedEvent ) {
    charges = *fReducedEvent->TrackerCharges;
    return;
  }

  assert(fRawEvent);

  if (!TrackerTrack()) {
    static std::vector<Float_t>* gNoTrackerCharges = 0;
    if (!gNoTrackerCharges)
      gNoTrackerCharges = new std::vector<Float_t>(9, -1000);
    charges = *gNoTrackerCharges;
    return;
  }

  charges.clear();
  charges.assign(9,-2000);

  const ACsoft::AC::TrackerTrack::ReconstructedHitsVector& rhits = TrackerTrack()->ReconstructedHits();
  for( unsigned int i = 0 ; i < rhits.size() ; ++i ) {
    const ACsoft::AC::TrackerReconstructedHit& hit = rhits[i];
    charges.at(hit.Layer() - 1) = fabs(hit.QLayerJ());
  }
}

/** Returns the desired tracker charge measurement according to the used enum.
 *
 *  supported are:
 *
 *  Individual layer charges
 *  - TrkLay1
 *  - TrkLay2
 *  - TrkLay3
 *  - TrkLay4
 *  - TrkLay5
 *  - TrkLay6
 *  - TrkLay7
 *  - TrkLay8
 *
 *  Mean Charges
 *  - TrkMean
 *
 *  Maximal and minimal measured charge
 *  - TrkMax
 *  - TrkMin
 *
 *
 */
Float_t ACsoft::Analysis::Particle::TrackerChargeFor(ACsoft::Analysis::Particle::TrackerChargeType type) const {

  std::vector<Float_t> trackerCharges;
  FillTrackerCharges(trackerCharges);

  switch (type) {
  case TrkLay1: return trackerCharges.at(0);
  case TrkLay2: return trackerCharges.at(1);
  case TrkLay3: return trackerCharges.at(2);
  case TrkLay4: return trackerCharges.at(3);
  case TrkLay5: return trackerCharges.at(4);
  case TrkLay6: return trackerCharges.at(5);
  case TrkLay7: return trackerCharges.at(6);
  case TrkLay8: return trackerCharges.at(7);
  case TrkLay9: return trackerCharges.at(8);
  case TrkMean: return ACsoft::Utilities::CalculateMoments(trackerCharges, ACsoft::Utilities::OnlyPositiveEntries).mean;
  case TrkMax: return ACsoft::Utilities::FindMaximum<std::vector<Float_t>, Float_t>(trackerCharges, ACsoft::Utilities::OnlyPositiveEntries);
  case TrkMin: return ACsoft::Utilities::FindMinimum<std::vector<Float_t>, Float_t>(trackerCharges, ACsoft::Utilities::OnlyPositiveEntries);
  }

  assert(false); // Can't happen.
  return 0;
}

/** Returns the tracker likelihood for charge confusion */
Float_t ACsoft::Analysis::Particle::TrackerChargeConfusionLikelihood() const {

  if (fReducedEvent)
    return fReducedEvent->TrkCCLikelihood;
  assert(fRawEvent);
  if (!TrackerTrack()) return -1000.;
  return TrackerTrack()->CCLikelihood();
}

/** Returns the %TOF charge.
  * See ACsoft::AC::TOFBeta::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofCharge() const {

  if( fReducedEvent )
    return fReducedEvent->TofC;
  assert(fRawEvent);
  if (!TofBeta())
    return -1000.;
  if (!TofBeta()->ChargesNew().size())
    return -2000.;
  return TofBeta()->GetChargeAndErrorNew(fRawEvent, ACsoft::AC::TOFBeta::GoodLayers).charge;
}

/** Returns the %TOF charge errror.
  * See ACsoft::AC::TOFBeta::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofChargeError() const {

  if( fReducedEvent )
    return fReducedEvent->TofCe;
  if (!TofBeta())
    return -1000.;
  assert(fRawEvent);
  if (!TofBeta()->ChargesNew().size())
    return -2000.;
  return TofBeta()->GetChargeAndErrorNew(fRawEvent, ACsoft::AC::TOFBeta::GoodLayers).error;
}

/** Returns the upper %TOF charge. If measurements from both upper %TOF layers 1 and 2 are available, the mean is used.
  */
Float_t ACsoft::Analysis::Particle::UpperTofCharge() const {

  double tofChargeLayer1 = TofChargeFor(ACsoft::Analysis::Particle::TofLay1);
  double tofChargeLayer2 = TofChargeFor(ACsoft::Analysis::Particle::TofLay2);
  double tofChargeUpper = std::max(tofChargeLayer1, tofChargeLayer2);
  if (tofChargeLayer1 > 0 && tofChargeLayer2 > 0)
    tofChargeUpper = (tofChargeLayer1 + tofChargeLayer2) * 0.5;
  return tofChargeUpper;
}

/** Returns the lower %TOF charge. If measurements from both lower %TOF layers 3 and 4 are available, the mean is used.
  */
Float_t ACsoft::Analysis::Particle::LowerTofCharge() const {

  double tofChargeLayer3 = TofChargeFor(ACsoft::Analysis::Particle::TofLay3);
  double tofChargeLayer4 = TofChargeFor(ACsoft::Analysis::Particle::TofLay4);
  double tofChargeLower = std::max(tofChargeLayer3, tofChargeLayer4);
  if (tofChargeLayer3 > 0 && tofChargeLayer4 > 0)
    tofChargeLower = (tofChargeLayer3 + tofChargeLayer4) * 0.5;
  return tofChargeLower;
}

/** Returns a vector of size 4 with the tof charge measurements.
 *
 *  if a layer has no charge measured the value is set to -2000.
 *  if there is no TofBeta object available the value is set to -1000.
 *
 *  the vector is sorted by layer ( layer1 = entry0 ... )
 *
 *  to access the charges and combined values, use the TofChargesFor() function.
 */

void ACsoft::Analysis::Particle::FillTofCharges(std::vector<Float_t>& charges) const {

  if( fReducedEvent) {
    charges = *fReducedEvent->TofCharges;
    return;
  }

  assert(fRawEvent);

  if( !TofBeta() ) {
    static std::vector<Float_t>* gNoTofCharges = 0;
    if (!gNoTofCharges)
      gNoTofCharges = new std::vector<Float_t>(4, -1000);
    charges = *gNoTofCharges;
    return;
  }

  charges.clear();
  charges.assign(4,-2000);

  Float_t Q;
  for (unsigned int i = 0; i < 4; ++i) {
    if (TofBeta()->ChargeInLayer(fRawEvent, i + 1, Q))
      charges.at(i) = Q;
  }
}

/** Returns the desired Tof charge according to the used enum.
 *
 *  supported are:
 *
 *  Individual layer charges
 *  - TofLay1
 *  - TofLay2
 *  - TofLay3
 *  - TofLay4
 *
 *  Mean Charges
 *  - TofMean
 *
 *  Maximal and minimal measured charge
 *  - TofMax
 *  - TofMin
 *
 */
Float_t ACsoft::Analysis::Particle::TofChargeFor(ACsoft::Analysis::Particle::TofChargeType type) const {

  std::vector<Float_t> tofCharges;
  FillTofCharges(tofCharges);

  switch (type) {
  case TofLay1: return tofCharges.at(0);
  case TofLay2: return tofCharges.at(1);
  case TofLay3: return tofCharges.at(2);
  case TofLay4: return tofCharges.at(3);
  case TofMean: return ACsoft::Utilities::CalculateMoments(tofCharges, ACsoft::Utilities::OnlyPositiveEntries).mean;
  case TofMax: return ACsoft::Utilities::FindMaximum<std::vector<Float_t>, Float_t>(tofCharges, ACsoft::Utilities::OnlyPositiveEntries);
  case TofMin: return ACsoft::Utilities::FindMinimum<std::vector<Float_t>, Float_t>(tofCharges, ACsoft::Utilities::OnlyPositiveEntries);
  }

  assert(false); // Can't happen.
  return -2000;
}

/** Returns the truncated mean of the inner %tracker charge.
  * See ACsoft::AC::TrackerTrack::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TrackerCharge() const {

  if( fReducedEvent )
    return fReducedEvent->TrkC;
  assert(fRawEvent);
  if (!TrackerTrack())
    return -1.;
  if (!TrackerTrack()->ChargesNew().size())
    return -2.;
  return TrackerTrack()->GetChargeAndErrorNew(3).charge;
}

/** Returns the truncated mean of the inner %tracker charge errror.
  * See ACsoft::AC::TrackerTrack::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TrackerChargeError() const {

  if( fReducedEvent )
    return fReducedEvent->TrkCe;
  assert(fRawEvent);
  if (!TrackerTrack())
    return -1.;
  if (!TrackerTrack()->ChargesNew().size())
    return -2.;
  return TrackerTrack()->GetChargeAndErrorNew(3).error;
}

/** Returns the %RICH charge.
  */
Float_t ACsoft::Analysis::Particle::RichCharge() const {

  if( fReducedEvent )
    return fReducedEvent->RichC;
  assert(fRawEvent);
  return RichRing() ? RichRing()->ChargeEstimate() : -1.0;
}

/** Returns the %RICH charge errror.
  * \todo This is not implemented yet.
  */
Float_t ACsoft::Analysis::Particle::RichChargeError() const {

  if( fReducedEvent )
    return fReducedEvent->RichCe;
  assert(fRawEvent);
  return -1.; //FIXME: not yet implemented
}

/** Returns the %TOFBeta beta value of the main particle. */
Float_t ACsoft::Analysis::Particle::BetaTof() const {

  if( fReducedEvent )
    return fReducedEvent->TofBeta;
  assert(fRawEvent);
  return TofBeta() ? TofBeta()->Beta() : 0.;
}

/** Returns the beta value measured by the %RICH. */
Float_t ACsoft::Analysis::Particle::RichBeta() const {

  if( fReducedEvent )
    return fReducedEvent->RichBeta;
  assert(fRawEvent);
  return RichRing() ? RichRing()->Beta() : 0;
}

/** Returns the reconstructed %ECAL energy in the main shower.
  * See ACsoft::AC::ECALShower::ReconstructedEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalEnergy() const {

  if( fReducedEvent )
    return fReducedEvent->EcalEnergy;
  assert(fRawEvent);
  return EcalShower() ? EcalShower()->ReconstructedEnergy() : 0;
}

/** Returns the reconstructed %ECAL energy in the main shower, under the hypothesis that the shower is an electron.
  * See ACsoft::AC::ECALShower::ReconstructedEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalEnergyElectron() const {

  if( fReducedEvent ) {
    WARN_OUT << "EcalEnergyElectron() not available on ACROOT files." << std::endl;
    return 0;
  }
  assert(fRawEvent);
  return EcalShower() ? EcalShower()->ReconstructedEnergyElectron() : 0;
}


/** Returns the %ECAL BDT value of the main shower (with version 5 of the estimator).
  * See ACsoft::AC::ECALShower::Estimators for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalBDTv5() const {

  if( fReducedEvent ) {
    WARN_OUT << "EcalBDTv5() is not available on ACROOT files." << std::endl;
    return fReducedEvent->EcalBDT;
  }
  assert(fRawEvent);
  if(!EcalShower() || !EcalShower()->Estimators().size())
    return -1.5;
  return EcalShower()->Estimators().at(0);
}

/** Returns the %ECAL BDT value of the main shower (with version 4 of the estimator).
  * See ACsoft::AC::ECALShower::Estimators for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalBDTv4() const {

  if( fReducedEvent )
    return fReducedEvent->EcalBDT;
  assert(fRawEvent);
  if(!EcalShower() || EcalShower()->Estimators().size() < 5)
    return -1.5;
  return EcalShower()->Estimators().at(4);
}

/** Returns the %ECAL standalone estimator (version 3, from Annecy) of the main shower.
  * See ACsoft::AC::ECALShower::Estimators for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalStandaloneEstimatorV3() const {

  if( fReducedEvent ) {
    WARN_OUT << "EcalStandaloneEstimatorV3() not available on ACROOT files." << std::endl;
    return -99999;
  }
  assert(fRawEvent);
  if(!EcalShower() || EcalShower()->Estimators().size() < 2)
    return -99999; // Use a value surely outside the range of this estimator. We should unify this somewhen.
  return EcalShower()->Estimators().at(2);
}

/** Returns the reconstructed %ECAL energy in all showers.
  * See ACsoft::AC::ECALShower::ReconstructedEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalSum() const {

  if( fReducedEvent )
    return fReducedEvent->EcalSum;
  assert(fRawEvent);
  Float_t ecalSum = 0.;
  for (unsigned int i = 0; i < fRawEvent->ECAL().Showers().size(); ++i)
    ecalSum += fRawEvent->ECAL().Showers().at(i).ReconstructedEnergy();
  return ecalSum;
}

/** Returns the center of gravity coordinates of the %ECAL shower.
  */
TVector3 ACsoft::Analysis::Particle::EcalCentreOfGravity() const {

  if (fReducedEvent)
    return TVector3(fReducedEvent->EcalCog[0],fReducedEvent->EcalCog[1],fReducedEvent->EcalCog[2]);

  assert(fRawEvent);
  if(!EcalShower()) return TVector3(-1000.,-1000.,-1000.);

  return EcalShower()->CentreOfGravity();
}

/** Returns the Zenith angle theta [rad] of the %ECAL shower
  * See ACsoft::AC::ECALShower::Theta for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalTheta() const {

  if (fReducedEvent) return fReducedEvent->EcalTheta;

  assert(fRawEvent);
  if(!EcalShower()) return -1000.0;

  return EcalShower()->Theta();
}

/** Returns the  Azimutal angle phi [rad] of the %ECAL shower
  * See ACsoft::AC::ECALShower::Phi for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalPhi() const {

  if (fReducedEvent) return fReducedEvent->EcalPhi;

  assert(fRawEvent);
  if(!EcalShower()) return -1000.0;

  return EcalShower()->Phi();
}

/** Returns the MC Particle ID of primary generator, a negative ID if no primary generator available or -999 if not a MC event
 */
Short_t ACsoft::Analysis::Particle::McParticleId() const {

  if (fOverrideTimeStamp) return -999; // Workaround to let IsMc() work on dummy particles.
  if (fReducedEvent) return fReducedEvent->McParticleId;

  assert(fRawEvent);
  if (fRawEvent->MC().EventGenerators().size() < 1) return -999; // FIXME changing this number might be dangerous...

  const ACsoft::AC::MCEventGenerator* eventg = fRawEvent->MC().PrimaryEventGenerator();
  if (!eventg) return fRawEvent->MC().EventGenerators().at(0).ParticleID();
  return eventg->ParticleID();
}

/** Returns the absolute value of MC Momentum or 0 if not a MC event
 */
Float_t ACsoft::Analysis::Particle::McMomentum() const {
  if (fReducedEvent) return fReducedEvent->McMomentum;

  assert(fRawEvent);
  if (fRawEvent->MC().EventGenerators().size() < 1) return 0;

  const ACsoft::AC::MCEventGenerator* eventg = fRawEvent->MC().PrimaryEventGenerator();
  if (!eventg) return fabs(fRawEvent->MC().EventGenerators().at(0).Momentum());
  return fabs(eventg->Momentum());
}

/** Returns the minimum generated MC momentum
 */
Float_t ACsoft::Analysis::Particle::McMinimumMomentum() const {
  if (fReducedEvent) return fReducedEvent->McMinimumMomentum;

   assert(fRawEvent);
   return fRawEvent->RunHeader()->MCMinimumMomentum();
}

/** Returns the maximum generated MC momentum
 */
Float_t ACsoft::Analysis::Particle::McMaximumMomentum() const {
  if (fReducedEvent) return fReducedEvent->McMaximumMomentum;

  assert(fRawEvent);
  return fRawEvent->RunHeader()->MCMaximumMomentum();
}

/** Returns the number of triggered MC events
 */
ULong64_t ACsoft::Analysis::Particle::McNumberOfTriggeredEvents() const {
  if (fReducedEvent) return fReducedEvent->McNumberOfTriggeredEvents;

  assert(fRawEvent);
  return fRawEvent->RunHeader()->MCNumberOfTriggeredEvents();
}

/** Returns the RTIData object for the time of the particle.
 */
const ACsoft::Utilities::RTIData* ACsoft::Analysis::Particle::RTIData() const {

  return ACsoft::Utilities::RTIReader::Instance()->GetRTIData(TimeStamp().GetSec());
}

/** Returns true if event has primary event generator available
 */
bool ACsoft::Analysis::Particle::HasPrimaryEventGenerator() const {
  return (McParticleId() > 0);
}

/** Returns true if the selected tracker track is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasTrackerTrack() const {

  if( fReducedEvent )
    return TrackHits() != 0;
  assert(fRawEvent);
  return TrackerTrack();
}

/** Returns true if the selected tracker track fit is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasTrackerTrackFit() const {

  if( fReducedEvent )
    return Chi2TrackerX() != -1 && Chi2TrackerY() != -1;
  assert(fRawEvent);
  return TrackerTrackFit();
}

/** Returns true if the selected %ECAL shower is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasEcalShower() const {

  if( fReducedEvent )
    return EcalEnergy() != 0.;
  assert(fRawEvent);
  return EcalShower();
}

/** Returns true if the selected %ECAL shower has a particle-ID estimator in the underlying event. */
bool ACsoft::Analysis::Particle::HasEcalShowerEstimator() const {

  if( fReducedEvent )
    return EcalBDT() != -1.5;
  assert(fRawEvent);
  return EcalShower()->Estimators().size();
}

/** Returns true if a %TOF beta is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasTofBeta() const {

  if( fReducedEvent )
    return TofCharge() != -1. || true /* FIXME: Work-around production bug, Normally Tof-Charge should never be < 0, but due the bug it's always -1 */;
  assert(fRawEvent);
  return TofBeta();
}

/** Returns true if the %TOF charges data are available in the underlying event. */
bool ACsoft::Analysis::Particle::HasTofCharge() const {

  if( fReducedEvent )
    return TofCharge() != -2.;
  assert(fRawEvent);
  return TofBeta()->ChargesNew().size();
}

/** Returns true if the selected tracker track is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasTrackerCharge() const {

  if( fReducedEvent )
    return TrackerCharge() != -2.;
  assert(fRawEvent);
  return TrackerTrack()->ChargesNew().size();
}

/** Returns true if the selected %RICH ring is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasRichRing() const {

  if( fReducedEvent )
    return RichCharge() != -1.0;
  assert(fRawEvent);
  return RichRing();
}

/** Returns true if the particle is part of a MC event
 */
bool ACsoft::Analysis::Particle::IsMC() const {
  return (McParticleId() != -999);
}

bool ACsoft::Analysis::Particle::IsBeamTest() const {
  return (McParticleId() == -999 && ACsoft::Utilities::IsBeamTestTime(EventTime()));
}

bool ACsoft::Analysis::Particle::IsISS() const {
  return (McParticleId() == -999 && !ACsoft::Utilities::IsBeamTestTime(EventTime()));
}

Double_t ACsoft::Analysis::Particle::McWeight() const {

  if (fMcScaler)
    return fMcScaler->GetWeight(this);
  else
    return 1;
}


