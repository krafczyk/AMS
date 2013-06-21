#ifndef ACROOT_LINKDEF_H
#define ACROOT_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


// All classes stored in the DB.root files.
#pragma link C++ class AC::Database+;
#pragma link C++ class AC::TrackerExternalAlignment+;
#pragma link C++ class AC::TRDCalibration+;
#pragma link C++ class AC::TRDModuleCalibration+;
#pragma link C++ class AC::TRDSensorData+;
#pragma link C++ class AC::TRDTubeCalibration+;

// All other TObject derived classes.
#pragma link C++ class AC::ACQtLookupFileIdentifier+;
#pragma link C++ class AC::ACQtPDFLookupFileIdentifier+;

// Non-TObject derived classes, that we still want to make available.
// TODO: Selectively add more classes, as they are needed.
#pragma link C++ defined_in "AMSGeometry.h";

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
#pragma link C++ class Utilities::TrackfindingHistogram+;

#endif

#endif // UTILITIES_LINKDEF_H
#ifndef CUTS_LINKDEF_H
#define CUTS_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class Cuts::Cut+;
#pragma link C++ class Cuts::ManualCut+;
#pragma link C++ class Cuts::ManualNMinusOneCut+;
#pragma link C++ class Cuts::NMinusOneCut+;
#pragma link C++ class Cuts::Selector+;
#pragma link C++ class Cuts::TwoSidedCut+;

/* ECALCuts */
#pragma link C++ class Cuts::CutEcalNumberOfShowers+;
#pragma link C++ class Cuts::CutTrackerTrackInEcalAcceptance+;
#pragma link C++ class Cuts::CutEcalShowerInFiducialVolume+;
#pragma link C++ class Cuts::CutEcalEnergy+;
#pragma link C++ class Cuts::CutEcalBdtEstimator+;
#pragma link C++ class Cuts::CutEcalStandaloneEstimatorV3+;
#pragma link C++ class Cuts::CutEcalTrackerTrackMatchR+;
#pragma link C++ class Cuts::CutEcalTrackerTrackMatchX+;
#pragma link C++ class Cuts::CutEcalTrackerTrackMatchY+;

/* ParticleSelectionCuts */
#pragma link C++ class Cuts::CutRemoveAntiNuclei+;
#pragma link C++ class Cuts::CutRemoveHeavyNuclei+;
#pragma link C++ class Cuts::CutProbablyChargeOneBeforeInnerTracker+;
#pragma link C++ class Cuts::CutChargeOneBeforeInnerTracker+;
#pragma link C++ class Cuts::CutReduceMisidentifiedHelium+;
#pragma link C++ class Cuts::CutEnergyOverRigidity+;
#pragma link C++ class Cuts::CutElectronCandidate+;
#pragma link C++ class Cuts::CutProtonCandidate+;

/* PreselectionCuts */
#pragma link C++ class Cuts::CutSingleParticle+;
#pragma link C++ class Cuts::CutAtLeastOneParticle+;
#pragma link C++ class Cuts::CutSingleTrackerTrack+;
#pragma link C++ class Cuts::CutTrackerNumberOfTracks+;
#pragma link C++ class Cuts::CutHasEcalShower+;
#pragma link C++ class Cuts::CutAccActivity+;
#pragma link C++ class Cuts::CutHasTrackerTrack+;
#pragma link C++ class Cuts::CutHasTrackerTrackFit+;
#pragma link C++ class Cuts::CutHasTofBeta+;
#pragma link C++ class Cuts::CutHasTRDVTrack+;
#pragma link C++ class Cuts::CutMCHasPrimaryEventGenerator+;
#pragma link C++ class Cuts::CutIsDowngoing+;
#pragma link C++ class Cuts::CutElectronIsDowngoing+;
#pragma link C++ class Cuts::CutEventConsistency+;
#pragma link C++ class Cuts::CutRigidityAboveGeomagneticCutoff+;
#pragma link C++ class Cuts::CutEcalEnergyAboveGeomagneticCutoff+;

/* TOFCuts */
#pragma link C++ class Cuts::CutTofInverseBeta+;
#pragma link C++ class Cuts::CutTofBetaProtons+;
#pragma link C++ class Cuts::CutTofBetaHelium+;
#pragma link C++ class Cuts::CutTofCharge+;
#pragma link C++ class Cuts::CutLowerTofCharge+;
#pragma link C++ class Cuts::CutUpperTofCharge+;
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
#pragma link C++ class Cuts::CutHasAdditionalNonInnerTrackerLayers+;
#pragma link C++ class Cuts::CutRelativeSagittaError+;
#pragma link C++ class Cuts::CutIsFullSpanTrackerTrack+;
#pragma link C++ class Cuts::CutIsFullSpanWithXandY+;
#pragma link C++ class Cuts::CutTrackerChargeLayer1+;
#pragma link C++ class Cuts::CutTrackerChargeLayer9+;
#pragma link C++ class Cuts::CutIsGoodInnerTrackerTrack+;
#pragma link C++ class Cuts::CutIsGoodCentralInnerTrackerTrack+;
#pragma link C++ class Cuts::CutTrackerTrackYsideGoodnessOfFit+;
#pragma link C++ class Cuts::CutTrackerTrackXsideGoodnessOfFit+;
#pragma link C++ class Cuts::CutTrackerRelativeSagittaError+;
#pragma link C++ class Cuts::CutTrackerTrackHasAtLeastThreeXHits+;
#pragma link C++ class Cuts::CutTrackerTrackFitRigidity+;
#pragma link C++ class Cuts::CutTrackerTrackFitAbsoluteRigidity+;
#pragma link C++ class Cuts::CutTrackerTrackReliableRigidity+;
#pragma link C++ class Cuts::CutTrackerThetaLayer1+;
#pragma link C++ class Cuts::CutTrackerHasLayer2Hit+;
#pragma link C++ class Cuts::CutTrackInCentralTofAcceptance+;
#pragma link C++ class Cuts::CutPositiveRigidity+;
#pragma link C++ class Cuts::CutNegativeRigidity+;
#pragma link C++ class Cuts::CutTrackInCentralLayer1Acceptance+;

/* RICH cuts */
#pragma link C++ class Cuts::CutRichRingAvailable+;
#pragma link C++ class Cuts::CutRichRingHits+;
#pragma link C++ class Cuts::CutRichRingBeta+;
#pragma link C++ class Cuts::CutRichRingBetaError+;
#pragma link C++ class Cuts::CutRichRingBetaConsistency+;
#pragma link C++ class Cuts::CutRichRingProbability+;
#pragma link C++ class Cuts::CutRichRingWidth+;
#pragma link C++ class Cuts::CutRichRingAGL+;
#pragma link C++ class Cuts::CutRichRingNaF+;
#pragma link C++ class Cuts::CutRichRingExpectedPhotoElectronsAGL+;
#pragma link C++ class Cuts::CutRichRingExpectedPhotoElectronsNaF+;
#pragma link C++ class Cuts::CutRichRingFractionTotalPhotoElectrons+;
#pragma link C++ class Cuts::CutRichRingDistanceToTileBorder+;

/* TriggerCuts */
#pragma link C++ class Cuts::CutTriggerInformationAvailable+;
#pragma link C++ class Cuts::CutTriggerLiveTime+;
#pragma link C++ class Cuts::CutTriggerRate+;
#pragma link C++ class Cuts::CutPhysicsTriggerChargedParticles+;
#pragma link C++ class Cuts::CutHasAnyPhysicsTrigger+;

/* BadRunCuts */
#pragma link C++ class Cuts::CutBadRun+;

/* GroupACuts */
#pragma link C++ class Cuts::CutEventErrors+;
#pragma link C++ class Cuts::CutScienceRunTag+;
#pragma link C++ class Cuts::CutGoodHW+;
#pragma link C++ class Cuts::CutIsNotInSolarArrayShadow+;
#pragma link C++ class Cuts::CutInEcalAcceptance+;
#pragma link C++ class Cuts::CutInTrdAcceptance+;
#pragma link C++ class Cuts::CutTrackerEcalMatching+;
#pragma link C++ class Cuts::CutTofTrackerMatching+;

/* RTICuts */
#pragma link C++ class Cuts::CutRTIdataAvailable+;
#pragma link C++ class Cuts::CutSecondWithinRun+;
#pragma link C++ class Cuts::CutMostEventsTriggered+;
#pragma link C++ class Cuts::CutBadReconstructionPeriod+;
#pragma link C++ class Cuts::CutBadFacingAngle+;
#pragma link C++ class Cuts::CutBadLiveTime+;
#pragma link C++ class Cuts::CutNoMissedEvents+;

#endif

#endif // CUTS_LINKDEF_H
#ifndef IO_LINKDEF_H
#define IO_LINKDEF_H

#ifdef __CINT__

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedefs;


#pragma link C++ class IO::ReducedFileHeader+;

#endif

#endif // IO_LINKDEF_H
