#include "AnalysisParticle.hh"

#include "Event.h"

#include "TrdHit.hh"
#include "SplineTrack.hh"
#include "ReducedEvent.hh"
#include "BadRunManager.hh"
#include "TOFBeta.h"

#include <iostream>

#include <TMath.h>

#define DEBUG 0
#define INFO_OUT_TAG "Particle> "
#include <debugging.hh>
#include <QDebug>

ACsoft::Analysis::Particle::Particle() :
  fRawEvent(0),
  fReducedEvent(0),
  fMainAmsParticle(0),
  fMainTrackerTrack(0),
  fMainTrackerTrackFit(0),
  fTrackerTrackFitID(-1),
  fMainEcalShower(0),
  fTofBeta(0),
  fMainTrdVTrack(0),
  fMainRichRing(0),
  fTrdHits(),
  fSplineTrack(0)
{
  fTrdHits.reserve(40);
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
ACsoft::Analysis::Particle::Particle( const AC::Event* rawevent, const AC::Particle* amsParticle, const AC::TrackerTrack* trackerTrack,
                              const AC::TrackerTrackFit* trackerTrackFit, const AC::ECALShower* shower, const AC::TOFBeta* beta,
                             const AC::TRDVTrack* trdvtrk, const AC::RICHRing* richring ) :
  fRawEvent(rawevent),
  fReducedEvent(0),
  fMainAmsParticle(amsParticle),
  fMainTrackerTrack(trackerTrack),
  fMainTrackerTrackFit(trackerTrackFit),
  fTrackerTrackFitID(-1),
  fMainEcalShower(shower),
  fTofBeta(beta),
  fMainTrdVTrack(trdvtrk),
  fMainRichRing(richring),
  fTrdHits(),
  fSplineTrack(0)
{
  fTrdHits.reserve(40);
  if( !fMainTrackerTrackFit ){
    WARN_OUT << "No main tracker track fit! Please use suitable preselection cuts to prevent this!" << std::endl;
    return;
  }
}

ACsoft::Analysis::Particle::Particle( const IO::ReducedEvent* reducedEvent ) :
  fRawEvent(0),
  fReducedEvent(reducedEvent),
  fMainAmsParticle(0),
  fMainTrackerTrack(0),
  fMainTrackerTrackFit(0),
  fTrackerTrackFitID(-1),
  fMainEcalShower(0),
  fTofBeta(0),
  fMainTrdVTrack(0),
  fMainRichRing(0),
  fTrdHits(),
  fSplineTrack(0)
{
  fTrdHits.reserve(40);
}

ACsoft::Analysis::Particle::~Particle() {
  if( fSplineTrack )
    delete fSplineTrack;
}

bool ACsoft::Analysis::Particle::IsEmpty() const {

  return( fTrdHits.empty() && !fSplineTrack);
}

void ACsoft::Analysis::Particle::Dump() const {

  if(fRawEvent) {
    std::cout << "[ACQt Event]\n";
    fRawEvent->Dump();
  } else {
    std::cout << "[ACROOT Event]\n";
    // FIXME: fReducedEvent->Print();
  }

  if( fTrdHits.size() > 0 ){
    std::cout << "  [TrdHits]" << std::endl;
    for( unsigned int i=0 ; i<fTrdHits.size() ; ++i )
      std::cout << fTrdHits[i] << std::endl;
  }

  if( fSplineTrack ){
    std::cout << "  [SplineTrack]" << std::endl;
    fSplineTrack->Dump();
  }

  std::cout << std::endl;
}


/** Returns the mean energy of clusters contained in TOFBeta object.
  * See AC::TOF::CalculateMeanEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofMeanEnergy() const {

  if( fReducedEvent )
    return fReducedEvent->TofMeanE;
  assert(fRawEvent);
  return fTofBeta ? fRawEvent->TOF().CalculateMeanEnergy(*fTofBeta) : -1;
}

/** Returns the highest energy of clusters contained in TOFBeta object.
  * See AC::TOF::CalculateMaximumEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofMaxEnergy() const {

  if( fReducedEvent )
    return fReducedEvent->TofMaxE;
  assert(fRawEvent);
  return fTofBeta ? fRawEvent->TOF().CalculateMaximumEnergy(*fTofBeta) : -1;
}

/** Returns the run number of the underlying event.
  * See AC::RunHeader::Run for full documentation.
  */
Int_t ACsoft::Analysis::Particle::Run() const {

  if( fReducedEvent )
    return fReducedEvent->Run;
  assert(fRawEvent);
  return fRawEvent->RunHeader()->Run();
}

/** Returns the event number of the underlying event.
  * See AC::EventHeader::Event for full documentation.
  */
Int_t ACsoft::Analysis::Particle::Event() const {

  if( fReducedEvent )
    return fReducedEvent->Event;
  assert(fRawEvent);
  return fRawEvent->EventHeader().Event();
}

/** Returns the encoded run tag.
  * See AC::RunHeader::RunTag for full documentation.
  */
Int_t ACsoft::Analysis::Particle::RunTag() const {

  if( fReducedEvent )
    return fReducedEvent->RunTag;
  assert(fRawEvent);
  return fRawEvent->RunHeader()->RunTag();
}

/** Returns the encoded bad run tag.
  * An integer is returned where each bit represents if this run is present in any of the bad run lists.
  * See ACsoft::Analysis::BadRunManager for full documentation on all vailable bad run categories.
  */
Int_t ACsoft::Analysis::Particle::BadRunTag() const {

  if( fReducedEvent )
    return fReducedEvent->BadRunTag;
  assert(fRawEvent);
  ACsoft::Analysis::BadRunManager* brm = ACsoft::Analysis::BadRunManager::Self();
  Int_t BadRunTag = 0;
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::General, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::General);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::DAQ, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::DAQ);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::TRD, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::TRD);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::TOF, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::TOF);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::ACC, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::ACC);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::Tracker, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::Tracker);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::Rich, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::Rich);
  if( brm->IsBad(ACsoft::Analysis::BadRunManager::Ecal, *this) ) BadRunTag += 1<<int(ACsoft::Analysis::BadRunManager::Ecal);
  return BadRunTag;
}

/** Returns the event time (in seconds).
  * See AC::EventHeader::TimeStamp for full documentation.
  */
Double_t ACsoft::Analysis::Particle::EventTime() const {

  if( fReducedEvent )
    return fReducedEvent->EventTime;
  assert(fRawEvent);
  return fRawEvent->EventHeader().TimeStamp().AsDouble();
}

/** Returns the event time (as TTimeStamp).
  * See AC::EventHeader::TimeStamp for full documentation.
  */
TTimeStamp ACsoft::Analysis::Particle::TimeStamp() const {

  if( fReducedEvent ){
    time_t sec = time_t(fReducedEvent->EventTime);
    int nsec   = int(1.e9*(fReducedEvent->EventTime - sec));
    return TTimeStamp(sec,nsec);
  }
  assert(fRawEvent);
  return fRawEvent->EventHeader().TimeStamp();
}


/** Returns a random number stored in each event, which can be used for sample splitting.
  * See AC::EventHeader::Random for full documentation.
  */
UChar_t  ACsoft::Analysis::Particle::EventRandom() const {

  if( fReducedEvent )
    return fReducedEvent->EventRndm;
  assert(fRawEvent);
  return fRawEvent->EventHeader().Random();
}

/** Returns the trigger rate.
  * See AC::Trigger::TriggerRateFT for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TriggerRate() const {

  if( fReducedEvent )
    return fReducedEvent->TriggerRate;
  assert(fRawEvent);
  if ( !fRawEvent->Trigger().HasData() )
    return -1.;
  return fRawEvent->Trigger().TriggerRateFT();
}

/** Returns the trigger live time.
  * See AC::Trigger::TriggerLiveTime for full documentation.
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
  * See AC::EventHeader::ISSLongitude for full documentation.
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
  * See AC::EventHeader::ISSLatitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::LatitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->LatitudeDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().ISSLatitude();
}

/** Returns the magnetic latitude.
  * See AC::EventHeader::MagneticLatitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::MagneticLatitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->MagLatDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().MagneticLatitude();
}

/** Returns the magnetic longitude.
  * See AC::EventHeader::MagneticLongitude for full documentation.
  */
Float_t ACsoft::Analysis::Particle::MagneticLongitudeDegree() const {

  if( fReducedEvent )
    return fReducedEvent->MagLongDeg;
  assert(fRawEvent);
  return fRawEvent->EventHeader().MagneticLongitude();
}

/** Returns the geomagnetic cut off (stoermer cut off for individual particle).
  */
Float_t ACsoft::Analysis::Particle::GeomagneticMaxCutOff() const {

  if( fReducedEvent )
    return fReducedEvent->GeomagneticMaxCutOff;
  assert(fRawEvent);
  if (!MainAmsParticle())
     return 0.;
  return MainAmsParticle()->StoermerMaxCutoff();
}

/** Returns the geomagnetic cut off (stoermer cut off for AMS field-of-view).
  */
Float_t ACsoft::Analysis::Particle::GeomagneticMaxCutOffCone() const {

  if( fReducedEvent )
    return fReducedEvent->GeomagneticMaxCutOffCone;
  assert(fRawEvent);
  return fRawEvent->EventHeader().MaxCutOffCone();
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

/** Returns a bit pattern describing the which layers have a hit and which track fit was used.
  * \todo Add more documentation, this is not sufficient, but it's also not 100% clear what happens here.
  */
Int_t ACsoft::Analysis::Particle::TrackHits() const {

  if( fReducedEvent )
    return fReducedEvent->TrkHits;
  assert(fRawEvent);
  if (!MainTrackerTrack())
    return 0;
  Int_t TrkHits = MainTrackerTrack()->MakeBitPattern().to_ulong();
  // Which track fit is used ?
  int TrkFit = TrackerTrackFitID();
  std::bitset<32> TrkBits=(TrkHits);
  TrkBits.set(10+TrkFit);
  TrkHits = TrkBits.to_ulong();
  return TrkHits;
}


/** Classify tracker hit pattern.
  *
  * See AC::TrackerTrack::TrackerLayerPatternClassification().
  */
Short_t ACsoft::Analysis::Particle::TrackerLayerPatternClassification() const {

  if( fReducedEvent )
    return fReducedEvent->TrackerLayerPatternClassification;
  assert(fRawEvent);
  if( !MainTrackerTrack() ) return -2;
  return MainTrackerTrack()->TrackerLayerPatternClassification();
}


/** Returns the rigidity of the main tracker track fit which was used.
  * See AC::TrackerTrackFit::Rigidity for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Rigidity() const {

  if( fReducedEvent )
    return fReducedEvent->Rig;
  assert(fRawEvent);
  return MainTrackerTrackFit() ? MainTrackerTrackFit()->Rigidity() : 0;
}

/** Returns the error on inverse rigidity of the main tracker track fit which was used.
  * See AC::TrackerTrackFit::InverseRigidityError for full documentation.
  */
Float_t ACsoft::Analysis::Particle::InverseRigidityError() const {

  if( fReducedEvent )
    return fReducedEvent->eRinv;
  assert(fRawEvent);
  return MainTrackerTrackFit() ? MainTrackerTrackFit()->InverseRigidityError() : 0;
}

/** Returns the chisquare of the track fit x hits.
  * See AC::TrackerTrackFit::ChiSquareNormalizedX for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Chi2TrackerX() const {

  if( fReducedEvent )
    return fReducedEvent->Chi2TrkX;
  assert(fRawEvent);
  return MainTrackerTrackFit() ? MainTrackerTrackFit()->ChiSquareNormalizedX() : -1;
}

/** Returns the chisquare of the track fit y hits.
  * See AC::TrackerTrackFit::ChiSquareNormalizedY for full documentation.
  */
Float_t ACsoft::Analysis::Particle::Chi2TrackerY() const {

  if( fReducedEvent )
    return fReducedEvent->Chi2TrkY;
  assert(fRawEvent);
  return MainTrackerTrackFit() ? MainTrackerTrackFit()->ChiSquareNormalizedY() : -1;
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
  if (!MainTrackerTrack())
    return rigs;
  const AC::TrackerTrack* TRKtrk = MainTrackerTrack();
  if (TRKtrk->GetFit(1,0,5)>=0) rigs[0] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,5)).Rigidity();
  else if(TRKtrk->GetFit(2,0,5)>=0) rigs[0] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,0,5)).Rigidity();
  if (TRKtrk->GetFit(1,0,4)>=0) rigs[1] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,0,4)).Rigidity();
  else if(TRKtrk->GetFit(2,0,4)>=0) rigs[1] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,0,4)).Rigidity();
  if (TRKtrk->GetFit(1,3,3)>=0) rigs[2] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,3,3)).Rigidity();
  else if(TRKtrk->GetFit(2,3,3)>=0) rigs[2] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,3,3)).Rigidity();
  if (TRKtrk->GetFit(1,5,5)>=0) rigs[3] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,5,5)).Rigidity();
  else if(TRKtrk->GetFit(2,5,5)>=0) rigs[3] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,5,5)).Rigidity();
  if (TRKtrk->GetFit(1,6,5)>=0) rigs[4] = TRKtrk->TrackFits().at(TRKtrk->GetFit(1,6,5)).Rigidity();
  else if(TRKtrk->GetFit(2,6,5)>=0) rigs[4] = TRKtrk->TrackFits().at(TRKtrk->GetFit(2,6,5)).Rigidity();

  return rigs;
}

/** Returns the tracker track phi angle at the upper TOF position (first entry) and at tracker layer 1 (second entry). */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerPhi() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkPhi) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkPhi, fReducedEvent->TrkPhi + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkPhi(2, 1000.);
  if (!MainTrackerTrackFit())
    return trkPhi;
  trkPhi[0] = MainTrackerTrackFit()->PhiTOFUpper() * TMath::DegToRad();
  trkPhi[1] = MainTrackerTrackFit()->PhiLayer1() * TMath::DegToRad();
  return trkPhi;
}

/** Returns the tracker track theta angle at the upper TOF position (first entry) and at tracker layer 1 (second entry). */
std::vector<Float_t> ACsoft::Analysis::Particle::TrackerTheta() const {

  if( fReducedEvent ) {
    const unsigned int size = sizeof(fReducedEvent->TrkTht) / sizeof(Float_t);
    return std::vector<Float_t>(fReducedEvent->TrkTht, fReducedEvent->TrkTht + size);
  }
  assert(fRawEvent);
  std::vector<Float_t> trkTht(2, 1000.);
  if (!MainTrackerTrackFit())
    return trkTht;
  trkTht[0] = MainTrackerTrackFit()->ThetaTOFUpper() * TMath::DegToRad();
  trkTht[1] = MainTrackerTrackFit()->ThetaLayer1() * TMath::DegToRad();
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
  if (!MainTrackerTrackFit())
    return trkXcors;
  trkXcors[0] = MainTrackerTrackFit()->XTOFUpper();
  trkXcors[1] = MainTrackerTrackFit()->XTRDLower();
  trkXcors[2] = MainTrackerTrackFit()->XTRDCenter();
  trkXcors[3] = MainTrackerTrackFit()->XTRDUpper();
  trkXcors[4] = MainTrackerTrackFit()->XLayer1();
  trkXcors[5] = MainTrackerTrackFit()->XLayer9();// historical order, could be improved
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
  if (!MainTrackerTrackFit())
    return trkYcors;
  trkYcors[0] = MainTrackerTrackFit()->YTOFUpper();
  trkYcors[1] = MainTrackerTrackFit()->YTRDLower();
  trkYcors[2] = MainTrackerTrackFit()->YTRDCenter();
  trkYcors[3] = MainTrackerTrackFit()->YTRDUpper();
  trkYcors[4] = MainTrackerTrackFit()->YLayer1();
  trkYcors[5] = MainTrackerTrackFit()->YLayer9();// historical order, could be improved
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
  if (!MainTrackerTrack())
    return hits;
  const AC::TrackerTrack::ReconstructedHitsVector& rhits = MainTrackerTrack()->ReconstructedHits();
  for( unsigned int i=0 ; i < rhits.size() ; ++i ){
    const AC::TrackerReconstructedHit& hit = rhits[i];
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
  if (!MainTrackerTrack())
    return hits;
  const AC::TrackerTrack::ReconstructedHitsVector& rhits = MainTrackerTrack()->ReconstructedHits();
  for( unsigned int i=0 ; i < rhits.size() ; ++i ){
    const AC::TrackerReconstructedHit& hit = rhits[i];
    if( hit.Layer() == 1)
      hits[0] = hit.Y();
    if( hit.Layer() == 9)
      hits[1] = hit.Y();
  }
  return hits;
}

/** Returns the x and y signal combination of the first reconstructed hit in layer 1. */
Float_t ACsoft::Analysis::Particle::TrackerSignalLayer1() const {

  if( fReducedEvent )
    return fReducedEvent->TrkSigLay1;
  assert(fRawEvent);
  // tracker hits in Layer 1 and 9
  if (!MainTrackerTrack())
    return -1.;
  const AC::TrackerTrack::ReconstructedHitsVector& rhits = MainTrackerTrack()->ReconstructedHits();
  for( unsigned int i=0 ; i < rhits.size() ; ++i ){
    const AC::TrackerReconstructedHit& hit = rhits[i];
    if( hit.Layer() == 1)
      return hit.SignalX() + hit.SignalY();
  }
  return -1.;
}

/** Returns the %TOF charge.
  * See AC::TOFBeta::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofCharge() const {

  if( fReducedEvent )
    return fReducedEvent->TofC;
  assert(fRawEvent);
  if (!TofBeta())
    return -1.;
  if (!TofBeta()->ChargesNew().size())
    return -2.;
  return TofBeta()->GetChargeAndErrorNew(fRawEvent, AC::TOFBeta::GoodLayers).charge;
}

/** Returns the %TOF charge errror.
  * See AC::TOFBeta::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TofChargeError() const {

  if( fReducedEvent )
    return fReducedEvent->TofCe;
  if (!TofBeta())
    return -1.;
  assert(fRawEvent);
  if (!TofBeta()->ChargesNew().size())
    return -2.;
  return TofBeta()->GetChargeAndErrorNew(fRawEvent, AC::TOFBeta::GoodLayers).error;
}

/** Returns the %TOF charge for layer 1.
 * See AC::TOFBeta::ChargesNew() for full documentation.
 */
Float_t ACsoft::Analysis::Particle::TofChargeLayer1() const {

  if( fReducedEvent )
    return fReducedEvent->TofCLay1;
  assert(fRawEvent);
  if (!TofBeta())
    return -1.;
  Float_t charge = 0;
  if (!TofBeta()->ChargeInLayer(fRawEvent,1,charge))
    return -2.;
  return charge;
}

/** Returns the %TOF charge for layer 2.
 * See AC::TOFBeta::ChargesNew() for full documentation.
 */
Float_t ACsoft::Analysis::Particle::TofChargeLayer2() const {

  if( fReducedEvent )
    return fReducedEvent->TofCLay2;
  assert(fRawEvent);
  if (!TofBeta())
    return -1.;
  Float_t charge = 0;
  if (!TofBeta()->ChargeInLayer(fRawEvent,2,charge))
    return -2.;
  return charge;
}

/** Returns the %TOF charge for layer 3.
 * See AC::TOFBeta::ChargesNew() for full documentation.
 */
Float_t ACsoft::Analysis::Particle::TofChargeLayer3() const {

  if( fReducedEvent )
    return fReducedEvent->TofCLay3;
  assert(fRawEvent);
  if (!TofBeta())
    return -1.;
  Float_t charge = 0;
  if (!TofBeta()->ChargeInLayer(fRawEvent,3,charge))
    return -2.;
  return charge;
}

/** Returns the %TOF charge for layer 4.
 * See AC::TOFBeta::ChargesNew() for full documentation.
 */
Float_t ACsoft::Analysis::Particle::TofChargeLayer4() const {

  if( fReducedEvent )
    return fReducedEvent->TofCLay4;
  assert(fRawEvent);
  if (!TofBeta())
    return -1.;
  if (!TofBeta()->ChargesNew().size())
    return -2.;
  Float_t charge = 0;
  if (!TofBeta()->ChargeInLayer(fRawEvent,4,charge))
    return -2.;
  return charge;
}

/** Returns the %Tracker charge.
  * See AC::TrackerTrack::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TrackerCharge() const {

  if( fReducedEvent )
    return fReducedEvent->TrkC;
  assert(fRawEvent);
  if (!MainTrackerTrack())
    return -1.;
  if (!MainTrackerTrack()->ChargesNew().size())
    return -2.;
  return MainTrackerTrack()->GetChargeAndErrorNew(3).charge;
}

/** Returns the %Tracker charge errror.
  * See AC::TrackerTrack::GetChargeAndErrorNew for full documentation.
  */
Float_t ACsoft::Analysis::Particle::TrackerChargeError() const {

  if( fReducedEvent )
    return fReducedEvent->TrkCe;
  assert(fRawEvent);
  if (!MainTrackerTrack())
    return -1.;
  if (!MainTrackerTrack()->ChargesNew().size())
    return -2.;
  return MainTrackerTrack()->GetChargeAndErrorNew(3).error;
}

/** Returns the %RICH charge.
  */
Float_t ACsoft::Analysis::Particle::RichCharge() const {

  if( fReducedEvent )
    return fReducedEvent->RichC;
  assert(fRawEvent);
  return MainRichRing() ? MainRichRing()->ChargeEstimate() : -1.0;
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
  return MainRichRing() ? MainRichRing()->Beta() : 0;
}

/** Returns the reconstructed %ECAL energy in the main shower.
  * See AC::ECALShower::ReconstructedEnergy for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalEnergy() const {

  if( fReducedEvent )
    return fReducedEvent->EcalEnergy;
  assert(fRawEvent);
  return MainEcalShower() ? MainEcalShower()->ReconstructedEnergy() : 0;
}

/** Returns the %ECAL BDT value of the main shower.
  * See AC::ECALShower::Estimators for full documentation.
  */
Float_t ACsoft::Analysis::Particle::EcalBDT() const {

  if( fReducedEvent )
    return fReducedEvent->EcalBDT;
  assert(fRawEvent);
  if(!MainEcalShower() || !MainEcalShower()->Estimators().size())
    return -1.5;
  return MainEcalShower()->Estimators().at(0);
}

/** Returns the reconstructed %ECAL enrgy in all showers.
  * See AC::ECALShower::ReconstructedEnergy for full documentation.
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

/** Returns true if the selected tracker track is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasMainTrackerTrack() const {

  if( fReducedEvent )
    return TrackHits() != 0;
  assert(fRawEvent);
  return MainTrackerTrack();
}

/** Returns true if the selected tracker track fit is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasMainTrackerTrackFit() const {

  if( fReducedEvent )
    return Chi2TrackerX() != -1 && Chi2TrackerY() != -1;
  assert(fRawEvent);
  return MainTrackerTrackFit();
}

/** Returns true if the selected %ECAL shower is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasMainEcalShower() const {

  if( fReducedEvent )
    return EcalEnergy() != 1.;
  assert(fRawEvent);
  return MainEcalShower();
}

/** Returns true if the selected %ECAL shower has a particle-ID estimator in the underlying event. */
bool ACsoft::Analysis::Particle::HasMainEcalShowerEstimator() const {

  if( fReducedEvent )
    return EcalBDT() != -1.5;
  assert(fRawEvent);
  return MainEcalShower()->Estimators().size();
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
bool ACsoft::Analysis::Particle::HasMainTrackerCharge() const {

  if( fReducedEvent )
    return TrackerCharge() != -2.;
  assert(fRawEvent);
  return MainTrackerTrack()->ChargesNew().size();
}

/** Returns true if the selected %RICH ring is available in the underlying event. */
bool ACsoft::Analysis::Particle::HasMainRichRing() const {

  if( fReducedEvent )
    return RichCharge() != -1.0;
  assert(fRawEvent);
  return MainRichRing();
}
