#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceParticles() {

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


    //links to SubD objects 
    particle.fTOFBetaIndex = pParticle->iBetaH();
    particle.fTRDVTrackIndex = pParticle->iTrdTrack();
    particle.fTRDHTrackIndex = pParticle->iTrdHTrack();
    particle.fTrackerTrackIndex = pParticle->iTrTrack();
    particle.fRICHRingIndex = pParticle->iRichRing();
    particle.fECALShowerIndex = pParticle->iEcalShower();

    // recover missing links to TrTrack, TrdTrack, EcalShower from BetaH:
    if (pParticle->iBetaH() >= 0){
      BetaHR* pBetaH = fAMSEvent->pBetaH(pParticle->iBetaH());
      if (pBetaH->iTrTrack() != pParticle->iTrTrack() && pBetaH->iTrTrack() >= 0) {
        particle.fTrackerTrackIndex = pBetaH->iTrTrack();
        particle.fStatus |= AC::Particle::FlagTrackerTrackIndexFromBetaH;
      }
      if (pBetaH->iTrdTrack() != pParticle->iTrdTrack() && pBetaH->iTrdTrack() >= 0) {
        particle.fTRDVTrackIndex = pBetaH->iTrdTrack();
        particle.fStatus |= AC::Particle::FlagTRDVTrackIndexFromBetaH;
      }
      if (pBetaH->iEcalShower() != pParticle->iEcalShower() && pBetaH->iEcalShower() >= 0) {
        particle.fECALShowerIndex = pBetaH->iEcalShower();
        particle.fStatus |= AC::Particle::FlagECALShowerIndexFromBetaH;
      }
    }

#ifndef AMS_ACQT_INTERFACE
    if( IsISSDataRun() ) {
      AMSPoint amspoint;
      if (fAMSEvent->isInShadow(amspoint, particleIndex))
        particle.fStatus |= AC::Particle::FlagIsInSolarArrayShadow;

      particle.fGalacticLongitudeFromRigidityBacktracing = 0.0;
      particle.fGalacticLatitudeFromRigidityBacktracing  = 0.0;
      particle.fGalacticLongitudeFromEnergyBacktracing = 0.0;
      particle.fGalacticLatitudeFromEnergyBacktracing  = 0.0;
      particle.fGalacticLongitudeFromOrbit       = 0.0;
      particle.fGalacticLatitudeFromOrbit        = 0.0;

      // Backtracing
      if (particle.fTrackerTrackIndex >= 0) { // - Particle with TrTrack
        timer_btrc->Start(kFALSE);

        // Eventually perform backtracing, see DoCustomBacktracing() for the exact conditions.
        double latitudeFromRigidity = 0, longitudeFromRigidity = 0;
        double latitudeFromEnergy = 0, longitudeFromEnergy = 0;
        UChar_t statusOfAllBacktracings = 0, statusOfRigidityBacktracing = 0, statusOfEnergyBacktracing = 0;
        if (DoCustomBacktracing(pParticle, statusOfAllBacktracings,
                                statusOfRigidityBacktracing, latitudeFromRigidity, longitudeFromRigidity,
                                statusOfEnergyBacktracing, latitudeFromEnergy, longitudeFromEnergy)) {
          ++nBTRC;
          particle.fStatus |= statusOfAllBacktracings << 8;

          if (statusOfRigidityBacktracing == 1) { // get Galactic Coordinates from BackTracking
            particle.fGalacticLongitudeFromRigidityBacktracing = longitudeFromRigidity;
            particle.fGalacticLatitudeFromRigidityBacktracing  = latitudeFromRigidity;
            particle.fStatus |= AC::Particle::FlagGalacticCoordinatesFromRigidityBacktracing;
          }

          if (statusOfEnergyBacktracing == 1) { // get Galactic Coordinates from BackTracking
            particle.fGalacticLongitudeFromEnergyBacktracing = longitudeFromEnergy;
            particle.fGalacticLatitudeFromEnergyBacktracing  = latitudeFromEnergy;
            particle.fStatus |= AC::Particle::FlagGalacticCoordinatesFromEnergyBacktracing;
          }
        }

        timer_btrc->Stop();
      }

      timer_galc->Start(kFALSE);
      int result;
      double glong, glatt;
      ++nGALC;
      if( fAMSEvent->GetGalCoo(result, glong, glatt, pParticle->Theta, pParticle->Phi) >= 0 ) {
        // Got Galactic Coordinates from ISS Orbit Parameters
        particle.fGalacticLongitudeFromOrbit = glong;
        particle.fGalacticLatitudeFromOrbit  = glatt;
        particle.fStatus |= AC::Particle::FlagGalacticCoordinatesFromOrbit;
      }
      timer_galc->Stop();

    }
#endif

    fEvent->fParticles.append(particle);
  }

  return true;
}
