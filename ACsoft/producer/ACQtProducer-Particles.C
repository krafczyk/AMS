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
    AMSPoint amspoint;


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

    fMaxTrackerTracks = max(fMaxTrackerTracks, particle.TrackerTrackIndex());
    fMaxRICHRings     = max(fMaxRICHRings,     particle.RICHRingIndex());
    fMaxECALShowers   = max(fMaxECALShowers,   particle.ECALShowerIndex());


    bool isISSdata = fAMSEvent->nMCEventg()==0 && fAMSEvent->Run()>1300000000; // not MC   not BeamTest
 
    if( isISSdata ) {


      if (fAMSEvent->isInShadow(amspoint, particleIndex))
        particle.fStatus |= AC::Particle::FlagIsInSolarArrayShadow;
      
      // GeoMagnetic CutOff
      CutOffNegPos[0]=0.0;
      CutOffNegPos[1]=0.0;
      GetCustomGeoCutoff(pParticle, fAMSEvent); // modified implementation in ACQtProducer-BackTracking.C
      particle.fStoermerCutoffNegative = CutOffNegPos[0];
      particle.fStoermerCutoffPositive = CutOffNegPos[1];
      particle.fGalacticLongitudeFromBacktracing = 0.0;
      particle.fGalacticLatitudeFromBacktracing  = 0.0;
      particle.fGalacticLongitudeFromOrbit       = 0.0;
      particle.fGalacticLatitudeFromOrbit        = 0.0;
      
      // Backtracing
      
      //call backtracing for all particles above 0.9*StoermerMinCutoff
      //inside DoBacktracing do all 4 only below 1.5*StoermerMaxCutoff 
      
      if (particle.fTrackerTrackIndex >= 0) { // - Particle with TrTrack
        float rigidity = fabs(fAMSEvent->pTrTrack(particle.fTrackerTrackIndex)->GetRigidity(0));
        float energy = 0.0; if( particle.fECALShowerIndex >= 0 ) energy = fAMSEvent->pEcalShower(particle.fECALShowerIndex)->EnergyE;
        // above 0.9*StoermerMinCutoff AND  ( below 1.5*StoermerMaxCutoff  OR e.m. (E/R > 0.5) )
        if( rigidity > 0.9 * min(fabs(particle.fStoermerCutoffNegative), particle.fStoermerCutoffPositive) 
	    && ( rigidity < 1.5 * max(fabs(particle.fStoermerCutoffNegative), particle.fStoermerCutoffPositive) || energy / rigidity > 0.5 ) ) {

          timer_btrc->Start(kFALSE);
          if (DoCustomBacktracing(pParticle) >= 0) {
            ++nBTRC;
            particle.fStatus |= BT_status<<8;
            if(pParticle->BT_status == 1) { // get Galactic Coordinates from BackTracking
              particle.fGalacticLongitudeFromBacktracing = pParticle->BT_glong;
              particle.fGalacticLatitudeFromBacktracing  = pParticle->BT_glat;
              particle.fStatus |= AC::Particle::FlagGalacticCoordinatesFromBacktracing;
            }
          }
          timer_btrc->Stop();

        }
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

    fEvent->fParticles.append(particle);
  }

  return true;
}
