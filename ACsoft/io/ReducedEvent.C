#include "ReducedEvent.hh"
#include "ReducedFileHeader.hh"
#include <assert.h>

/** Default constructor
 */
ACsoft::IO::ReducedEvent::ReducedEvent(::IO::ReducedFileHeader* header)
  : FileHeader(header)
  , LR_Elec_Prot_MC(new std::vector<Float_t>)
  , LR_Heli_Elec_MC(new std::vector<Float_t>)
  , LR_Elec_Prot_Rndm(new std::vector<Float_t>) {

  assert(header);
  Clear();
}

/** Default destructor
 */
ACsoft::IO::ReducedEvent::~ReducedEvent() {

  delete LR_Elec_Prot_MC;
  delete LR_Heli_Elec_MC;
  delete LR_Elec_Prot_Rndm;
}

/** Reset all variables to default value ZERO or -1
 *
 */
void ACsoft::IO::ReducedEvent::Clear() {

  Run = -1;
  Event = -1;
  RunTag = -1;
  BadRunTag = -1;
  EventRndm = 0;
  
  EventTime = 0.0;
  TriggerRate = 0.; TriggerLifeTime = 0.; LongitudeDeg = 1000.0; LatitudeDeg = 1000.0; MagLatDeg = 1000.0; MagLongDeg = 1000.0;
  GeomagneticMaxCutOff = -1.0;
  GeomagneticMaxCutOffCone = -1.0;
  
  nAccHits = -1; nTofHits = -1; nEcalShr = -1; nTrkTrks = -1;
  
  TrkHits = -1;
  TrackerLayerPatternClassification = -2;
  
  Rig = 0.; eRinv = 0.; Chi2TrkX = -1.; Chi2TrkY = -1.;
  for( int i=0 ; i<5 ; ++i ) Rigs[i] = 0.;
  
  TrkPhi[0] = 1000.0; TrkPhi[1] = 1000.0;
  TrkTht[0] = 1000.0; TrkTht[1] = 1000.0;
  for( int i=0 ; i<6 ; ++i ) TrkXcors[i] = 1000.;
  for( int i=0 ; i<6 ; ++i ) TrkYcors[i] = 1000.;
  TrkXhits[0] = 0.; TrkXhits[1] = 0.;
  TrkYhits[0] = 0.; TrkYhits[1] = 0.;
  TrkSigLay1 = -1.0;
  
  TofC = -1; TofCe = -1; TrkC = -1; TrkCe = -1; RichC = -1; RichCe = -1;
  TofBeta = 0.; RichBeta = 0.;
  EcalEnergy = 0.; EcalBDT = -1.5; EcalSum = 0.;
  TofMeanE = 0.; TofMaxE = 0.;
  
  TrdInAcceptance     =  0;
  TrdCandidateLayers  = -1;
  TrdActiveLayers     = -1;
  TrdActiveStraws     = -1;
  TrdActivePathLen    = -1;
  TrdCandidatePathLen = -1;
  TrdUnassignedHits   =  0;
  ParticleId          = -1;
  LR_Elec_Prot        = -1;
  LR_Heli_Elec        = -1;
  LR_Heli_Prot        = -1;
  XePressure          =  0;

  TofCLay1 = -1;
  TofCLay2 = -1;
  TofCLay3 = -1;
  TofCLay4 = -1;

  nTrdRawHits = -1;

  TrdTrkDeltaAngleXZ = 1000.;
  TrdTrkDeltaAngleYZ = 1000.;
  TrdTrackCombinedFitXZChi2 = -1.;
  TrdTrackCombinedFitYZChi2 = -1.;
  TrdTrackStandAloneFitXZChi2 = -1.;
  TrdTrackStandAloneFitYZChi2 = -1.;

  LR_Elec_Prot_MC->clear();
  LR_Heli_Elec_MC->clear();
  LR_Elec_Prot_Rndm->clear();
}
