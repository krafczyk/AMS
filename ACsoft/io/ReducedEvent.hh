#ifndef ReducedEvent_hh
#define ReducedEvent_hh

#include <Rtypes.h>
#include <vector>

namespace IO {

class ReducedFileHeader;

}

/// Reduced size event structure for quick physics analysis
namespace ACsoft {

namespace IO {

/** Base class of reduced events
 *  Container class for reduced-event variables.
 */
class ReducedEvent {

  // FIXME add docu on how to add a new variable: here, Clear function, ReducedEventWriter::SetBranches, ReducedEventWriter::Process, FileManagerACROOT::SetACROOTBranchAddresses

public:
  ReducedEvent(::IO::ReducedFileHeader*);
  ~ReducedEvent();

  void Clear();

  ::IO::ReducedFileHeader* FileHeader;          ///! Pointer to run-header, not saved in ROOT file.

  Int_t    Run;                               ///< Run number
  Int_t    Event;                             ///< Event number
  Int_t    RunTag;                            ///< Run Tag (Science, Debug)
  Double_t EventTime;                         ///< Time of event in [s] since EPOCH
  UChar_t  EventRndm;                         ///< Random Number (same as in ACQT event)

  Float_t TriggerRate;                        ///< Trigger rate [Hz]
  Float_t TriggerLifeTime;                    ///< Trigger life time
  UChar_t TriggerFlags;                       ///< Trigger flags
  Float_t LongitudeDeg;                       ///< Geographic Longitude of ISS in degrees
  Float_t LatitudeDeg;                        ///< Geographic Latitude of ISS in degrees
  Float_t MagLatDeg;                          ///< Magnetic Longitude of ISS in degrees
  Float_t MagLongDeg;                         ///< Magnetic Latitude of ISS in degrees
  Float_t GeomagneticMaxCutOff;               ///< Rigidity of geomagnetic cutoff for this event in GV

  Char_t  nAccHits;                           ///< Number of ACC clusters
  Char_t  nTofHits;                           ///< Number of ToF clusters
  Char_t  nEcalShr;                           ///< number of ECAL showers
  Char_t  nTrkTrks;                           ///< Number of Tracker Tracks

  Int_t   TrkHits;                            ///< bit pattern describing the which layers have a hit and which track fit was used
  Short_t TrackerLayerPatternClassification;  ///< tracker layer pattern class
  Float_t Rig;                                ///< Rigidity of "preferred" Tracker Fit [GV]  TRKtrk.TrackFits().at(TRKtrk.GetFit(0,0,0)).Rigidity()
  Float_t eRinv;                              ///< Error on inverse of Rigidity
  Float_t Chi2TrkX;                           ///< Chisquare of Tracker Fit in X plane
  Float_t Chi2TrkY;                           ///< Chisquare of Tracker Fit in Y plane

  /** Rigidities [GV] of various TrackFits
   * See AC::TrackerTrack::GetFit() and cACQtProducer-Tracker.C for an explainatation of the GetFit() parameters.
   *
   * Rigs[0] = TRKtrk.TrackFits().at(TRKtrk.GetFit(1,0,2)).Rigidity();
   * Rigs[1] = TRKtrk.TrackFits().at(TRKtrk.GetFit(1,0,12)).Rigidity();
   * Rigs[2] = TRKtrk.TrackFits().at(TRKtrk.GetFit(1,3,2)).Rigidity();
   * Rigs[3] = TRKtrk.TrackFits().at(TRKtrk.GetFit(1,3,12)).Rigidity();
   * Rigs[4] = TRKtrk.TrackFits().at(TRKtrk.GetFit(0,0,0)).Rigidity();
   *
   * GetFit(a,b,c):
   *  see AC::TrackerTrack docu
   */
  Float_t Rigs[5];

  Float_t TrkPhi[3];                          ///< Azimuthal Angle [rad] of TrkTrack in [0] UToF, [1] Layer 1, [2] Layer 9
  Float_t TrkTht[3];                          ///< Zenith Angle [rad] of TrkTrack in [0] UToF, [1] Layer 1, [2] Layer 9

  /** X coordinates [cm] of TrkTrack at z of...
   *
   * TrkXcors[0]: TOFUpper
   * TrkXcors[1]: TRDLower
   * TrkXcors[2]: TRDCenter
   * TrkXcors[3]: TRDUpper
   * TrkXcors[4]: Layer1
   * TrkXcors[5]: Layer9
   *
   */
  Float_t TrkXcors[6];

  /** Y coordinates [cm] of TrkTrack at z of...
   *
   * TrkYcors[0]: TOFUpper
   * TrkYcors[1]: TRDLower
   * TrkYcors[2]: TRDCenter
   * TrkYcors[3]: TRDUpper
   * TrkYcors[4]: Layer1
   * TrkYcors[5]: Layer9
   *
   */
  Float_t TrkYcors[6];

  Float_t TrkXhits[2];          ///< X position [cm] of Hit (not Track!) in outer Trk Layers [0]=Layer 1, [1]=Layer 9
  Float_t TrkYhits[2];          ///< Y position [cm] of Hit (not Track!) in outer Trk Layers [0]=Layer 1, [1]=Layer 9
  Float_t TrkC;                 ///< Reconstructed Charge Z in Tracker
  Float_t TrkCe;                ///< Uncertainty in Tracker charge
  Float_t TrkCCLikelihood;      ///< Charge confusion likelihood

  Float_t TofBeta;              ///< beta=v/c from ToF measurement
  Float_t TofC;                 ///< Reconstructed Charge Z in ToF
  Float_t TofCe;                ///< Uncertainty on ToF C charge

  Float_t RichC;                ///< Reconstructed Charge Z in RICH
  Float_t RichCe;               ///< Uncertainty in RICH charge
  Float_t RichBeta;             ///< beta=v/c from RICH measurement

  Float_t EcalEnergy;           ///< Reconstructed deposited energy in ECAL of shower matching particle [GeV]
  Float_t EcalBDT;              ///< Ecal BDT estimator
  Float_t EcalSum;              ///< Sum of reconstructed energies of all ECAL showers [GeV]
  Float_t EcalCog[3];           ///< Coordinates of the center of gravity of the shower
  Float_t EcalTheta;            ///< Zenith angle theta [rad] of the shower
  Float_t EcalPhi;              ///< Azimuth angle phi [rad] of the shower

  Short_t ParticleId;           ///< from TrdQt based on Tracker Charge and Rigidity sign: 0==Electron, 1==Proton, 2==Helium, ...
  Bool_t  TrdInAcceptance;      ///< 0 not in TRD acceptance, 1 in TRD acceptance, i.e. the tracker track passes through layer 0 and 19
  UChar_t TrdCandidateLayers;   ///< Number of candidate Layers in TRD on TrkTrack
  UChar_t TrdActiveLayers;      ///< Number of active Layers in TRD on TrkTrack
  UChar_t TrdUnassignedHits;    ///< Number of active straws near TrkTrack, but not assigned to the TrkTrack
  UChar_t TrdKActiveHits;       ///< TRDK: number of used hits for e/p/he likelihoods.
  UChar_t TrdKUnassignedHits;   ///< TRDK: number of off-track hits for e/p/he likelihoods.
  Float_t TrdActivePathLen;     ///< Sum of TrkTrack path lengths through active straws [cm]
  Float_t TrdCandidatePathLen;  ///< Expected path length throug TRD straws based on TrkTrack [cm]
  Short_t XePressure;           ///< Xe pressure at time of event [mbar]
  // FIXME should be superfluous once a proper slow control lookup is available

  UShort_t nTrdRawHits;         ///< Number of TRD raw Hits

  // FIXME relationship to TofC and TofCe variables??
  std::vector<Float_t>* TofCharges;       ///< vector of size 4 with the tof charge measurements
  // FIXME relationship to TrkC and TrkCe ??
  std::vector<Float_t>* TrackerCharges;   ///< vector of size 9 of the Tracker charge measurements
  std::vector<Float_t>* TrdKLikelihoodRatios;  ///< log Likelihood Ratios from TrkK package (MIT) , e/(e+p), He/(He+p), e/(e+He)
  std::vector<Float_t>* TrdQtLikelihoodRatios; ///< log Likelihood Ratios from TrkQt package (RWTH), e/(e+p), He/(He+p), e/(e+He)

  std::vector<UShort_t>* TrdStraws; ///< straw numbers for all TrdQt candidate hits
  std::vector<UShort_t>* Trd2DeDx;  ///<  2*dEdx for all TrdQt candidate hits

  Short_t McParticleId;                   ///< MC particle ID, see Analysis::Particle::McParticleId()
  Float_t McMomentum;                     ///< MC momentum
  Float_t McMinimumMomentum;              ///< MC minimum generated momentum
  Float_t McMaximumMomentum;              ///< MC maximum generated momentum
  ULong64_t McNumberOfTriggeredEvents;    ///< MC number of triggered events
};

}

}

#endif
