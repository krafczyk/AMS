#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceRICH() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nRings = fAMSEvent->nRichRing();
  for (int ringIndex = 0; ringIndex < nRings; ringIndex++) {
    RichRingR* pRichRing = fAMSEvent->pRichRing(ringIndex);
    if (!pRichRing) {
      WARN_OUT << "Problem building AC::RICHRing[" << ringIndex << "]: "
               << "AMS ROOT File is broken! pRichRing() returned null." << std::endl;
      return false;
    }

    // Dummy call to enable PMT corrections
    pRichRing->getCharge2Estimate();
    // get pmt correction status: -1 = correction not done; 0 = correction done; 1 = correction failed
    int pmtcorrstatus = pRichRing->PmtCorrectionsFailed();
    if(pmtcorrstatus == 1)
    {
	WARN_OUT << "AMS ROOT File is broken! PMT Corrections failed. RICH is not calibrated correctly." << std::endl;	
	return false; // correction failed; stop processing the whole event
    }
    else if (pmtcorrstatus == -1)
      return true; // correction not done; continue processing event

    AC::RICHRing richRing;

    richRing.fStatus = pRichRing->Status;

    if (pRichRing->getHits() > 0xffff)
      WARN_OUT << "Problem building AC::RICHRing[" << ringIndex << "]: "
               << "Used: " << pRichRing->getHits() << " way too large." << std::endl;

    richRing.fNumberOfHits = min(0xffff, pRichRing->getHits());
    richRing.fNumberOfUsedHits = min(0xffff, pRichRing->getUsedHits());
    richRing.fBeta = pRichRing->getBeta();
    richRing.fBetaError = pRichRing->getBetaError();
    richRing.fBetaConsistency = pRichRing->getBetaConsistency();
    richRing.fX = pRichRing->getTrackEmissionPoint()[0];
    richRing.fY = pRichRing->getTrackEmissionPoint()[1];
    richRing.fZ = pRichRing->getTrackEmissionPoint()[2];
    richRing.fTheta = M_PI - pRichRing->getTrackTheta();    // Tracker Convention
    richRing.fPhi = pRichRing->getTrackPhi();  
    richRing.fPhi += richRing.fPhi > 0.0 ? -M_PI : M_PI; // Tracker Convention
    richRing.fChargeEstimate = sqrt(pRichRing->getCharge2Estimate());
    richRing.fNPhotoElectrons = pRichRing->getPhotoElectrons();
    richRing.fNExpectedPhotoElectrons = pRichRing->getExpectedPhotoelectrons();
    richRing.fNCollectedPhotoElectrons = RichHitR::getCollectedPhotoElectrons();
    richRing.fIsNaF = pRichRing->IsNaF();
    richRing.fWidth = pRichRing->getWidth();
    richRing.fWidthUnusedHits = pRichRing->UDist;
    richRing.fDistanceToTileBorder = pRichRing->DistanceTileBorder();

    if (pRichRing->IsClean() && pRichRing->getHits() > 3 && pRichRing->getPMTs() > 2)
      richRing.fProbability = fabs(pRichRing->getProb());
    else
      richRing.fProbability = -fabs(pRichRing->getProb());

    fEvent->fRICH.fRings.append(richRing);
  }

  return true;
}

