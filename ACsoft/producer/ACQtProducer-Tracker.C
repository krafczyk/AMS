#include "ACQtProducer-include.C"

// FIXME: Explain what this function does. Thorsten could fill this in.
static int CalculatePatternForFit(TrTrackR* pTrTrack, int pattern) {

  Q_ASSERT(pTrTrack);
  int adjustedPattern = pattern;

  int nHits = 0;
  if (pattern == 2) { // Lay 3,4,5,6,7,8 - Inner without Lay-2
    adjustedPattern = 0;
    int iPatt = 900;
    for (int j = 3; j<= 8; ++j, iPatt *= 10) {
      if (pTrTrack->GetHitLJ(j)) {
        ++nHits;
        adjustedPattern += iPatt;
      }
    }

    return nHits >= 3 ? adjustedPattern : -1;
  }

  if (pattern == 5) { // Lay 1,4,6,8 - Upper Half
    adjustedPattern = 0;
    if (pTrTrack->GetHitLJ(1)) {
      ++nHits;
      adjustedPattern += 9;
    }

    if (pTrTrack->GetHitLJ(4)) {
      ++nHits;
      adjustedPattern += 9000;
    }

    if (pTrTrack->GetHitLJ(6)) { 
      ++nHits;
      adjustedPattern += 900000;
    }

    if (pTrTrack->GetHitLJ(8)) {
      ++nHits;
      adjustedPattern += 90000000;
    }

    return nHits >= 3 ? adjustedPattern : -1;
  } 

  if (pattern == 6) { // Lay 2,3,5,7,9 - Lower Half
    adjustedPattern = 0;
    if (pTrTrack->GetHitLJ(2)) {
      ++nHits;
      adjustedPattern += 90;
    }

    if (pTrTrack->GetHitLJ(3)) {
      ++nHits;
      adjustedPattern += 900;
    }

    if (pTrTrack->GetHitLJ(5)) {
      ++nHits;
      adjustedPattern += 90000;
    }

    if (pTrTrack->GetHitLJ(7)) {
      ++nHits;
      adjustedPattern += 9000000;
    }

    if (pTrTrack->GetHitLJ(9)) {
      ++nHits;
      adjustedPattern += 900000000;
    }

    return nHits >= 3 ? adjustedPattern : -1;
  }

  return adjustedPattern;
}

// FIXME: Explain what this function does. Thorsten could fill this in.
static int CalculateMassAndChargeForFit(TrTrackR* pTrTrack, int refit, float beta, float& mass, float& charge) {

  Q_ASSERT(pTrTrack);
  int adjustedRefit = refit;

  if (pTrTrack->GetQ(beta) <= 1.5) {
    mass = 0.938272297; // Proton mass
    charge = 1.0;       // Charge 1
  } else {
    mass = 3.727379240; // Helium-mass
    charge = 2.0;       // Charge-2
  }

  switch (refit) {
  case 3:
    adjustedRefit = 1;
    break;
  case 4:
    adjustedRefit = 1;  // PG ext. Alignment
    break;
  case 5:
    adjustedRefit = 11; // MA ext. Alignment
    break;
  case 6: 
    adjustedRefit = 2;  // PG ext. Alignment force refit with (mass,z,beta)
    mass = 0.000510999; // electron-mass
    charge = 1.0;       // Charge-1
    break;
  case 7: 
    adjustedRefit = 2;  // PG ext. Alignment
    mass = 3.727379240; // Helium-mass
    charge = 2.0;       // Charge-2
    break;
  }

  return adjustedRefit;
}

bool ACQtProducer::ProduceTrackerTrackFit(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, int algorithm, int pattern, int refit, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  float mass = 0, charge = 0;
  int adjustedRefit = CalculateMassAndChargeForFit(pTrTrack, refit, beta, mass, charge);
  int adjustedPattern = CalculatePatternForFit(pTrTrack, pattern);

  int result = pTrTrack->iTrTrackPar(algorithm, adjustedPattern, adjustedRefit, mass, charge);
  if (result < 0) {
    WARN_OUT << "Problem building AC::TrackerFit[algo=" << algorithm << " pattern=" << pattern << " refit=" << refit << "]: "
             << "Track fit failed (status=" << result << ")" << std::endl;

    // This is not a fatal error and happens for lots of events, not all fits converge. Keep going by returning true here.
    return true;
  }

  AC::TrackerTrackFit trackerTrackFit;
  trackerTrackFit.fParameters = ((algorithm & 3) << 6) + ((pattern & 7) << 3) + ((refit & 7));

  trackerTrackFit.fRigidity = pTrTrack->GetRigidity(result);
  trackerTrackFit.fInverseRigidityError = pTrTrack->GetErrRinv(result);
  trackerTrackFit.fChiSquareNormalizedX = pTrTrack->GetNormChisqX(result);
  trackerTrackFit.fChiSquareNormalizedY = pTrTrack->GetNormChisqY(result);

  AMSPoint point;
  AMSDir direction;

  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer1, point, direction, result);
  trackerTrackFit.fThetaLayer1 = direction.gettheta();
  trackerTrackFit.fPhiLayer1 = direction.getphi();
  trackerTrackFit.fXLayer1 = point.x();
  trackerTrackFit.fYLayer1 = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDUpper, point, direction, result);
  trackerTrackFit.fXTRDUpper = point.x();
  trackerTrackFit.fYTRDUpper = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDCenter, point, direction, result);
  trackerTrackFit.fXTRDCenter = point.x();
  trackerTrackFit.fYTRDCenter = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTRDLower, point, direction, result);
  trackerTrackFit.fXTRDLower = point.x();
  trackerTrackFit.fYTRDLower = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTOFUpper, point, direction, result);
  trackerTrackFit.fThetaTOFUpper = direction.gettheta();
  trackerTrackFit.fPhiTOFUpper = direction.getphi();
  trackerTrackFit.fXTOFUpper = point.x();
  trackerTrackFit.fYTOFUpper = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZRICH, point, direction, result);
  trackerTrackFit.fThetaRICH  = direction.gettheta();
  trackerTrackFit.fPhiRICH = direction.getphi();
  trackerTrackFit.fXRICH = point.x();
  trackerTrackFit.fYRICH = point.y();

  pTrTrack->Interpolate(AC::AMSGeometry::ZTrackerLayer9,  point, direction, result);
  trackerTrackFit.fThetaLayer9 = direction.gettheta();
  trackerTrackFit.fPhiLayer9 = direction.getphi();
  trackerTrackFit.fXLayer9 = point.x();
  trackerTrackFit.fYLayer9 = point.y();

  trackerTrack.fTrackFits.append(trackerTrackFit);
  return true;
}

bool ACQtProducer::ProduceTrackerTrackFits(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  // Respect override track fits. If set, only produce a single track fit.
  if (fAlgorithm != -1 && fPattern != -1 && fRefit != -1) {
    ProduceTrackerTrackFit(trackerTrack, pTrTrack, fAlgorithm, fPattern, fRefit, beta);
    return true;
  }

  try {
    // FIXME: Is this still needed?
    pTrTrack->iTrTrackPar(2, 3, 2); // dummy call 

    if (fUseTrackerExternalAlignment) {
      // Tracker External Alignment available - for analysis (pass2 or later)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 3, 3, beta);     //   Choutko algorithm (P3=1)  Inn Hits (P4=3) Refit with new alignment (P5=3)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 2, 3, beta);     //   Choutko algorithm (P3=1)  Inn Hits without Lay-2 (P4=2) Refit with new alignment (P5=3)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 3, 3, beta);     //   Alcaraz algorithm (P3=2)  Inn Hits (P4=3) Refit with new alignment (P5=3)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 2, 3, beta);     //   Alcaraz algorithm (P3=2)  Inn Hits without Lay-2 (P4=2) Refit with new alignment (P5=3)

      if (pTrTrack->HasExtLayers()) {
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 0, 5, beta);   //   Choutko algorithm (P3=1)  All Hits (P4=0) with MA ext alignment (P5=5): 
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 0, 5, beta);   //   Alcaraz algorithm (P3=2)  All Hits (P4=0) with MA ext alignment (P5=5): 
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 0, 4, beta);   //   Choutko algorithm (P3=1)  All Hits (P4=0) with PG ext alignment (P5=4): 
        ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 0, 4, beta);   //   Alcaraz algorithm (P3=2)  All Hits (P4=0) with PG ext alignment (P5=4): 

        if (pTrTrack->HasExtLayers() == 3) {
          // has hit in Layer-1 and Layer-9 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 5, 5, beta); //   Choutko algorithm (P3=1)  Upper+L1 Hits (P4=5) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 6, 5, beta); //   Choutko algorithm (P3=1)  Lower+L9 Hits (P4=6) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 5, 5, beta); //   Alcaraz algorithm (P3=2)  Upper+L1 Hits (P4=5) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 6, 5, beta); //   Alcaraz algorithm (P3=2)  Lower+L9 Hits (P4=6) with MA ext alignment (P5=5): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 5, 4, beta); //   Choutko algorithm (P3=1)  Upper+L1 Hits (P4=5) with PG ext alignment (P5=4): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 6, 4, beta); //   Choutko algorithm (P3=1)  Lower+L9 Hits (P4=6) with PG ext alignment (P5=4): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 5, 4, beta); //   Alcaraz algorithm (P3=2)  Upper+L1 Hits (P4=5) with PG ext alignment (P5=4): 
          ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 6, 4, beta); //   Alcaraz algorithm (P3=2)  Lower+L9 Hits (P4=6) with PG ext alignment (P5=4): 
        }
      }

      // Fits for electron Mass
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 0, 6, beta);     //   Choutko algorithm (P3=1)  All Hits (P4=0) with PG ext alignment and e Mass (P5=6): 
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 0, 6, beta);     //   Alcaraz algorithm (P3=2)  All Hits (P4=0) with PG ext alignment and e Mass (P5=6): 
    } else {
      // external layer alignment not available - for calibration (std)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 1, 3, 3, beta);     //   Choutko algorithm (P3=1)  Inn Hits (P4=3) Refit with new alignment (P5=3)
      ProduceTrackerTrackFit(trackerTrack, pTrTrack, 2, 3, 3, beta);     //   Alcaraz algorithm (P3=2)  Inn Hits (P4=3) Refit with new alignment (P5=3)
    }
  } catch (...) {
    return false;
  }

  return true;
}

static inline bool LookupTOFBetaForTrackerTrackIndex(const AC::Event* event, int trackIndex, float& beta) {

  beta = 999; // FIXME: Preserve historical behaviour.

  // Lookup particle which is associated with the given trackIndex.
  int nParticles = (int)event->fParticles.size();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    const AC::Particle& particle = event->fParticles[particleIndex];
    if (particle.TrackerTrackIndex() != trackIndex)
      continue;

    short betaIndex = particle.TOFBetaIndex();
    if (betaIndex < 0 || betaIndex >= (short)event->fTOF.Betas().size())
      return false;

    beta = event->fTOF.Betas()[betaIndex].Beta();
    return true;
  }

  return false;
}

bool ACQtProducer::ProduceTrackerTrackCharges(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
  Q_ASSERT(pTrTrack);

  int pattern = 3;
  float charge = pTrTrack->GetInnerQ(beta);
  float error = pTrTrack->GetInnerQ_RMS(beta) / sqrt(max(1.0,(float)pTrTrack->GetInnerQ_NPoints(beta) - 1.0));
  trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern,charge, error));

  if (pTrTrack->GetHitLJ(1) > 0) { // has hit in L1
    pattern = 1;
    charge = pTrTrack->GetLayerJQ(1, beta);
    error = (float)pTrTrack->GetInnerQ_NPoints(beta);
    if (charge > 0.0)
      trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern, charge, error));
  }

  if (pTrTrack->GetHitLJ(9) > 0) { // has hit in L9
    pattern = 9;
    charge = pTrTrack->GetLayerJQ(9, beta);
    error = (float)pTrTrack->GetInnerQ_NPoints(beta);
    if (charge > 0.0)
      trackerTrack.fChargesNew.append(AC::PatternChargeAndError(pattern, charge, error));
  }

  return true;
}

bool ACQtProducer::ProduceTrackerTrackReconstructedHits(AC::TrackerTrack& trackerTrack, TrTrackR* pTrTrack, float beta) {

  int nReconstructedHits = pTrTrack->NTrRecHit();
  for (int reconstructedHitIndex=0; reconstructedHitIndex < nReconstructedHits; ++reconstructedHitIndex) {
    TrRecHitR* pTrRecHit = pTrTrack->pTrRecHit(reconstructedHitIndex);
    if (!pTrRecHit) {
      WARN_OUT << "Problem building AC::TrackerReconstructedHit[" << reconstructedHitIndex << "]: "
               << "AMS ROOT File is broken! pTrTrack->pTrRecHit() returned null." << std::endl;
      return false;
    }

    AC::TrackerReconstructedHit trackerReconstructedHit;
    trackerReconstructedHit.fLayer = pTrRecHit->GetLayerJ();

    int multiplicity = pTrRecHit->GetResolvedMultiplicity();
    if (multiplicity == -1) {
      trackerReconstructedHit.fLayer *= -1;
      trackerReconstructedHit.fX = pTrRecHit->GetCoord().x();
      trackerReconstructedHit.fY = pTrRecHit->GetCoord().y();

      WARN_OUT << "Problem building AC::TrackerReconstructedHit[" << reconstructedHitIndex << "]: "
               << "XY multiplicity unresolved in layer " << trackerReconstructedHit.fLayer << ": "
               << "x=" << trackerReconstructedHit.fX << " y=" << trackerReconstructedHit.fY << std::endl;
    } else {
      trackerReconstructedHit.fX = pTrRecHit->GetGlobalCoordinateA(multiplicity).x();
      trackerReconstructedHit.fY = pTrRecHit->GetGlobalCoordinateA(multiplicity).y();
    }

    trackerReconstructedHit.fSignalX = pTrRecHit->GetSignalCombination(0);
    trackerReconstructedHit.fSignalY = pTrRecHit->GetSignalCombination(1);

    trackerTrack.fReconstructedHits.append(trackerReconstructedHit);
  }

  return true;
}

bool ACQtProducer::ProduceTracker() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  // FIXME: Document what's happening here, and why it's insufficient to just use nTrTrack() here.
  int nTracks = max(fMaxTrackerTracks + 1, min(5, fAMSEvent->nTrTrack()));
  for (int trackIndex = 0; trackIndex < nTracks; ++trackIndex) {
    TrTrackR* pTrTrack = fAMSEvent->pTrTrack(trackIndex);
    if (!pTrTrack) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "AMS ROOT File is broken! pTrTrack() return null." << std::endl;
      return false;
    }

    AC::TrackerTrack trackerTrack;
    trackerTrack.fStatus = pTrTrack->getstatus(); 
    trackerTrack.fNumberOfHitsX = pTrTrack->GetNhitsX();
    trackerTrack.fNumberOfHitsY = pTrTrack->GetNhitsY();

    // get Charge from new estimator getQ - needs beta - look for matching Particle
    float beta = 999;
    LookupTOFBetaForTrackerTrackIndex(fEvent, trackIndex, beta);

    if (!ProduceTrackerTrackCharges(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "Building tracker charges failed." << std::endl;
      return false;
    }

    // get TrdSCalib e/p/He -logLikelihoodRatios for this TrTrack extrapolated into TRD:
    if (!fIsProcessingEventsFromStandardRun) { // TRD-Charge-Probabilities not available for std runs 
      int fitcode = 0;
      if (TrdSCalibR::gethead()->ProcessTrdEvtWithTrTrack(fAMSEvent, pTrTrack, fitcode) == 0) {
        trackerTrack.fSCalibProbability.append(TrdSCalibR::gethead()->TrdLPs[0]);
        trackerTrack.fSCalibProbability.append(TrdSCalibR::gethead()->TrdLPs[1]);
        trackerTrack.fSCalibProbability.append(TrdSCalibR::gethead()->TrdLPs[2]);
      }
    }

    if (!ProduceTrackerTrackFits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

    if (!ProduceTrackerTrackReconstructedHits(trackerTrack, pTrTrack, beta)) {
      WARN_OUT << "Problem building AC::TrackerTrack[" << trackIndex << "]: "
               << "AMS ROOT File is broken! Tracker fitting throwed." << std::endl;
      return false;
    }

    fEvent->fTracker.fTracks.append(trackerTrack);
  }

  fEvent->fTracker.fClusters = fAMSEvent->nTrCluster();
  return true;
}
