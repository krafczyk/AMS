#include "ReducedEvent.hh"
#include "ReducedFileHeader.hh"
#include <assert.h>

/** Default constructor
 */
ACsoft::IO::ReducedEvent::ReducedEvent(::IO::ReducedFileHeader* header)
  : FileHeader(header)
  , TofCharges(new std::vector<Float_t>)
  , TrackerCharges(new std::vector<Float_t>)
  , TrdKLikelihoodRatios(new std::vector<Float_t>)
  , TrdQtLikelihoodRatios(new std::vector<Float_t>)
  , TrdStraws(new std::vector<UShort_t>)
  , Trd2DeDx(new std::vector<UShort_t>) {

  assert(header);
  Clear();
}

/** Default destructor
 */
ACsoft::IO::ReducedEvent::~ReducedEvent() {

  delete TofCharges;
  delete TrackerCharges;
  delete TrdKLikelihoodRatios;
  delete TrdQtLikelihoodRatios;
  delete TrdStraws;
  delete Trd2DeDx;
}

/** Reset all variables to default value ZERO or -1
 *
 */
void ACsoft::IO::ReducedEvent::Clear() {

  Run                       = -1;
  Event                     = -1;
  RunTag                    = -1;
  EventRndm                 = 0;
  
  EventTime                 = 0.0;
  TriggerRate               = 0.;
  TriggerLifeTime           = 0.;
  TriggerFlags              = 0;
  LongitudeDeg              = 1000.0;
  LatitudeDeg               = 1000.0;
  MagLatDeg                 = 1000.0;
  MagLongDeg                = 1000.0;
  GeomagneticMaxCutOff      = -1.0;
  
  nAccHits                          = -1;
  
  nTrkTrks                          = -1;
  TrkHits                           = -1;
  TrackerLayerPatternClassification = -2;
  
  Rig = 0.; eRinv = 0.; Chi2TrkX = -1.; Chi2TrkY = -1.;
  for( int i=0 ; i<5 ; ++i ) Rigs[i] = 0.;
  
  TrkPhi[0] = 1000.0; TrkPhi[1] = 1000.0; TrkPhi[2] = 1000.0;
  TrkTht[0] = 1000.0; TrkTht[1] = 1000.0; TrkTht[2] = 1000.0;
  for( int i=0 ; i<6 ; ++i ) TrkXcors[i] = 1000.;
  for( int i=0 ; i<6 ; ++i ) TrkYcors[i] = 1000.;
  TrkXhits[0] = 0.; TrkXhits[1] = 0.;
  TrkYhits[0] = 0.; TrkYhits[1] = 0.;

  TrkC            = -1;
  TrkCe           = -1;
  TrkCCLikelihood = -1.0;

  nTofHits        = -1;
  TofBeta         = 0.;
  TofC            = -1;
  TofCe           = -1;

  RichC           = -1;
  RichCe          = -1;
  RichBeta        = 0.;

  nEcalShr        = -1;
  EcalEnergy      = 0.;
  EcalBDT         = -1.5;
  EcalSum         = 0.;
  EcalCog[0]      = -1000.0;
  EcalCog[1]      = -1000.0;
  EcalCog[2]      = -1000.0;
  EcalTheta       = -1000.0;
  EcalPhi         = -1000.0;

  TrdInAcceptance     = 0;
  TrdCandidateLayers  = 0;
  TrdActiveLayers     = 0;
  TrdActivePathLen    = -1;
  TrdKActiveHits      = 0;
  TrdKUnassignedHits  = 0;
  TrdCandidatePathLen = -1;
  TrdUnassignedHits   =  0;
  ParticleId          = -1;
  XePressure          =  0;
  nTrdRawHits         = 0;

  TofCharges->clear();
  TrackerCharges->clear();
  TrdKLikelihoodRatios->clear();
  TrdQtLikelihoodRatios->clear();

  TrdStraws->clear();
  Trd2DeDx->clear();

  McParticleId = -999;
  McMomentum   = 0;
  McMinimumMomentum = -1;
  McMaximumMomentum = -1;
  McNumberOfTriggeredEvents = 0;
}

