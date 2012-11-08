#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceParticles() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nParticles = fAMSEvent->nParticle();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    ParticleR* pParticle = fAMSEvent->pParticle(particleIndex);
    if (!pParticle) {
      WARN_OUT << "Problem building AC::Particle[" << particleIndex << "]: "
               << "AMS ROOT File is broken! pParticle() returned null." << std::endl;
      return false;
    }

    AC::Particle particle;
    particle.fID = pParticle->Particle;
    particle.fIDProbability = pParticle->Prob[0];
    particle.fMomentum = pParticle->Momentum;
    particle.fMomentumError = pParticle->ErrMomentum;
    particle.fMass = pParticle->Mass;
    particle.fMassError = pParticle->ErrMass;
    particle.fBeta = pParticle->Beta;
    particle.fBetaError = pParticle->ErrBeta;
    particle.fTOFBetaIndex = pParticle->iBeta();
    particle.fTRDVTrackIndex = pParticle->iTrdTrack();
    particle.fTRDHTrackIndex = pParticle->iTrdHTrack();
    particle.fTrackerTrackIndex = pParticle->iTrTrack();
    particle.fRICHRingIndex = pParticle->iRichRing();
    particle.fECALShowerIndex = pParticle->iEcalShower();

    if (ChargeR* amsCharge = pParticle->pCharge()) {
      particle.fCharges.append(amsCharge->ChargeI[0]);
      particle.fCharges.append(amsCharge->ChargeI[1]);
      particle.fChargesProbability.append(amsCharge->Prob[0]);
      particle.fChargesProbability.append(amsCharge->Prob[1]);
    }

    fMaxTrackerTracks = max(fMaxTrackerTracks, particle.TrackerTrackIndex());
    fMaxRICHRings = max(fMaxRICHRings, particle.RICHRingIndex());
    fMaxECALShowers = max(fMaxECALShowers, particle.ECALShowerIndex());

    fEvent->fParticles.append(particle);
  }

  return true;
}
