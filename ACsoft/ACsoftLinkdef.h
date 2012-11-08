#ifndef ACROOT_LINKDEF_H
#define ACROOT_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class AC::Database+;
#pragma link C++ class AC::TrackerExternalAlignment+;
#pragma link C++ class AC::TRDCalibration+;
#pragma link C++ class AC::TRDModuleCalibration+;
#pragma link C++ class AC::TRDSensorData+;
#pragma link C++ class AC::TRDTubeCalibration+;

#pragma link C++ class AC::ACQtLookupFileIdentifier+;
#pragma link C++ class AC::ACQtPDFLookupFileIdentifier+;

#endif

#endif // ACROOT_LINKDEF_H
#ifndef ANALYSIS_LINKDEF_H
#define ANALYSIS_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class Analysis::TrdPdfHistogram+;

#endif

#endif // ANALYSIS_LINKDEF_H
#ifndef UTILITIES_LINKDEF_H
#define UTILITIES_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class Utilities::Palette+;

#endif

#endif // UTILITIES_LINKDEF_H
#ifndef CUTS_LINKDEF_H
#define CUTS_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class Cuts::Cut+;
#pragma link C++ class Cuts::TwoSidedCut+;
#pragma link C++ class Cuts::Selector+;

/* ECALCuts */
#pragma link C++ class Cuts::CutTrackerTrackInEcalAcceptance+;
#pragma link C++ class Cuts::CutEcalShowerInFiducialVolume+;
#pragma link C++ class Cuts::CutEcalEnergy+;
#pragma link C++ class Cuts::CutEcalBdtEstimator+;
#pragma link C++ class Cuts::CutEcalTrackerTrackMatch+;

/* ParticleSelectionCuts */
#pragma link C++ class Cuts::CutRemoveAntiNuclei+;
#pragma link C++ class Cuts::CutRemoveHeavyNuclei+;
#pragma link C++ class Cuts::CutRemovePositrons+;
#pragma link C++ class Cuts::CutRemoveAntiProtons+;
#pragma link C++ class Cuts::CutReduceMisidentifiedHelium+;
#pragma link C++ class Cuts::CutEnergyOverRigidity+;
#pragma link C++ class Cuts::CutElectronCandidate+;
#pragma link C++ class Cuts::CutProtonCandidate+;

/* PreselectionCuts */
#pragma link C++ class Cuts::CutSingleParticle+;
#pragma link C++ class Cuts::CutSingleTrackerTrack+;
#pragma link C++ class Cuts::CutHasEcalShower+;
#pragma link C++ class Cuts::CutHasTrackerTrack+;
#pragma link C++ class Cuts::CutHasTrackerTrackFit+;
#pragma link C++ class Cuts::CutHasTofBeta+;
#pragma link C++ class Cuts::CutHasTRDVTrack+;
#pragma link C++ class Cuts::CutIsDowngoing+;
#pragma link C++ class Cuts::CutEventConsistency+;

/* TOFCuts */
#pragma link C++ class Cuts::CutTofCharge+;
#pragma link C++ class Cuts::CutTofChargeAvailable+;
#pragma link C++ class Cuts::CutTofMaxEnergy+;
#pragma link C++ class Cuts::CutTofMeanEnergy+;
#pragma link C++ class Cuts::CutTofTotalMaxEnergy+;

/* TRDCuts */
#pragma link C++ class Cuts::CutTrdNumberOfRawHits+;

/* TrackerCuts */
#pragma link C++ class Cuts::CutTrackerChargeAvailable+;
#pragma link C++ class Cuts::CutTrackerCharge+;
#pragma link C++ class Cuts::CutTrackerLayerOneOrNine+;
#pragma link C++ class Cuts::CutIsGoodInnerTrackerTrack+;
#pragma link C++ class Cuts::CutIsGoodCentralInnerTrackerTrack+;
#pragma link C++ class Cuts::CutTrackerTrackGoodnessOfFit+;
#pragma link C++ class Cuts::CutTrackerTrackHasAtLeastThreeXHits+;
#pragma link C++ class Cuts::CutTrackerTrackFitRigidity+;
#pragma link C++ class Cuts::CutTrackerTrackFitAbsoluteRigidity+;
#pragma link C++ class Cuts::CutTrackerTrackReliableRigidity+;

/* TriggerCuts */
#pragma link C++ class Cuts::CutTriggerInformationAvailable+;
#pragma link C++ class Cuts::CutTriggerLiveTime+;
#pragma link C++ class Cuts::CutTriggerRate+;

/* BadRunCuts */
#pragma link C++ class Cuts::CutBadRun+;

#endif

#endif // CUTS_LINKDEF_H
#ifndef IO_LINKDEF_H
#define IO_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class std::vector<Short_t>+;
#pragma link C++ class std::vector<Float_t>+;

#endif

#endif
