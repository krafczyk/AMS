#include "ACQtProducer-include.C"

template<typename ChargesVector, typename ChargesProbabilityVector>
static inline void AppendCharges(ChargeSubDR* pChargeSubD, ChargesVector& charges, ChargesProbabilityVector& chargesProbability) {

  Q_ASSERT(pChargeSubD);
  float probabilitySum = 0;

  charges.clear();
  chargesProbability.clear();

  int nChargeI = pChargeSubD->ChargeI.size();
  for (int chargeIndex = 0; chargeIndex < nChargeI; ++chargeIndex) {
    charges.append(pChargeSubD->ChargeI[chargeIndex]);
    chargesProbability.append(pChargeSubD->Prob[chargeIndex]);
    probabilitySum += pChargeSubD->Prob[chargeIndex];
    if (probabilitySum > 0.99)
      break;
  }

  // FIXME: This is hacky and needs to be reworked.
  if (pChargeSubD->Status & 0x10)
    charges.append(-99); // Status "BAD"
  else
    charges.append(99);

  chargesProbability.append(pChargeSubD->Q);
}

bool ACQtProducer::ProduceChargesForRICH(const AC::Particle& particle, ChargeR* pCharge) {

  Q_ASSERT(pCharge);
  ChargeSubDR* pChargeSubD = pCharge->getSubD("AMSChargeRich");

  int ringIndex = particle.RICHRingIndex();
  if (!pChargeSubD || ringIndex < 0)
    return true; // That's okay, keep going.

  if (ringIndex >= (int)fEvent->fRICH.fRings.size()) {
    WARN_OUT << "Problem building AC::RICHRing charges: AMS ROOT File is broken! "
             << "Particle associated RICHRing index " << particle.RICHRingIndex()
             << " larger than number of RICHRings in event " << fEvent->fRICH.fRings.size() << std::endl;
    return false;
  }

  AC::RICHRing& richRing = fEvent->fRICH.fRings[ringIndex];
  AppendCharges(pChargeSubD, richRing.fCharges, richRing.fChargesProbability);
  return true; 
}

bool ACQtProducer::ProduceChargesForTracker(const AC::Particle& particle, ChargeR* pCharge) {

  Q_ASSERT(pCharge);
  ChargeSubDR* pChargeSubD = pCharge->getSubD("AMSChargeTrackerInner");

  int trackerTrackIndex = particle.TrackerTrackIndex();
  if (!pChargeSubD || trackerTrackIndex < 0)
    return true; // That's okay, keep going.

  if (trackerTrackIndex >= (int)fEvent->fTracker.fTracks.size()) {
    WARN_OUT << "Problem building AC::TrackerTrack charges: AMS ROOT File is broken! "
             << "Particle associated TrackerTrack index " << particle.TrackerTrackIndex()
             << " larger than number of TrackerTracks in event " << fEvent->fTracker.fTracks.size() << std::endl;
    return false;
  }

  AC::TrackerTrack& trackerTrack = fEvent->fTracker.fTracks[trackerTrackIndex];
  AppendCharges(pChargeSubD, trackerTrack.fCharges, trackerTrack.fChargesProbability);
  return true; 
}

bool ACQtProducer::ProduceChargesForTOF(const AC::Particle& particle, ChargeR* pCharge) {

  Q_ASSERT(pCharge);
  ChargeSubDR* pChargeSubD = pCharge->getSubD("AMSChargeTOF");

  int betaIndex = particle.TOFBetaIndex();
  if (!pChargeSubD || betaIndex < 0)
    return true; // That's okay, keep going.

  if (betaIndex >= (int)fEvent->fTOF.fBetas.size()) {
    WARN_OUT << "Problem building AC::TOFBeta charges: AMS ROOT File is broken! "
             << "Particle associated TOFBeta index " << particle.TOFBetaIndex()
             << " larger than number of TOFBetas in event " << fEvent->fTOF.fBetas.size() << std::endl;
    return false;
  }

  AC::TOFBeta& tofBeta = fEvent->fTOF.fBetas[betaIndex];
  AppendCharges(pChargeSubD, tofBeta.fCharges, tofBeta.fChargesProbability);
  return true;
}

bool ACQtProducer::ProduceChargesForTRD(const AC::Particle& particle, ChargeR* pCharge) {

  Q_ASSERT(pCharge);
  ChargeSubDR* pChargeSubD = pCharge->getSubD("AMSChargeTRD");

  int vtrackIndex = particle.TRDVTrackIndex();
  if (!pChargeSubD || vtrackIndex < 0)
    return true; // That's okay, keep going.

  if (vtrackIndex >= (int)fEvent->fTRD.fVTracks.size()) {
    WARN_OUT << "Problem building AC::TRDVTrack charges: AMS ROOT File is broken! "
             << "Particle associated TRDVTrack index " << particle.TRDVTrackIndex()
             << " larger than number of TRDVTracks in event " << fEvent->fTRD.fVTracks.size() << std::endl;
    return false;
  }

  AC::TRDVTrack& trdVTrack = fEvent->fTRD.fVTracks[vtrackIndex];
  AppendCharges(pChargeSubD, trdVTrack.fCharges, trdVTrack.fChargesProbability);
  return true;
}

bool ACQtProducer::ProduceCharges() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nParticles = fEvent->fParticles.size();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    const AC::Particle& particle = fEvent->fParticles[particleIndex];

    ParticleR* pParticle = fAMSEvent->pParticle(particleIndex);
    if (!pParticle) {
      WARN_OUT << "Problem building charges: AMS ROOT File is broken! "
               << "pParticle(" << particleIndex << ") returned null." << std::endl;
      return false;
    }

    ChargeR* pCharge = pParticle->pCharge();
    if (!pCharge)
      continue;

    if (!ProduceChargesForTracker(particle, pCharge))
      return false;

    if (!ProduceChargesForTOF(particle, pCharge))
      return false;

    if (!ProduceChargesForRICH(particle, pCharge))
      return false;

    if (!ProduceChargesForTRD(particle, pCharge))
      return false;
  }

  return true;
}
