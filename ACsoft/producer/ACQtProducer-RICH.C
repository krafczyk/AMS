#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceRICH() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nRings = max(fMaxRICHRings + 1, min(5, fAMSEvent->nRichRing()));
  for (int ringIndex = 0; ringIndex < nRings; ++ringIndex) {
    RichRingR* pRichRing = fAMSEvent->pRichRing(ringIndex);
    if (!pRichRing) {
      WARN_OUT << "Problem building AC::RICHRing[" << ringIndex << "]: "
               << "AMS ROOT File is broken! pRichRing() returned null." << std::endl;
      return false;
    }

    AC::RICHRing richRing;
    richRing.fStatus = pRichRing->Status;

    if (pRichRing->Used > 0xffff)
      WARN_OUT << "Problem building AC::RICHRing[" << ringIndex << "]: "
               << "Used: " << pRichRing->Used << " way too large." << std::endl;

    richRing.fNumberOfHits = min(0xffff, pRichRing->Used);
    richRing.fBeta = pRichRing->Beta;
    richRing.fBetaError = pRichRing->ErrorBeta;
    richRing.fX = pRichRing->AMSTrPars[0];
    richRing.fY = pRichRing->AMSTrPars[1];
    richRing.fZ = pRichRing->AMSTrPars[2];
    richRing.fTheta = M_PI - pRichRing->AMSTrPars[3];    // Tracker Convention
    richRing.fPhi = pRichRing->AMSTrPars[4];  
    richRing.fPhi += richRing.fPhi > 0.0 ? -M_PI : M_PI; // Tracker Convention
    richRing.fChargeSquareEstimate = sqrt(pRichRing->getCharge2Estimate());

    if (pRichRing->IsClean() && pRichRing->getExpectedPhotoelectrons() > 2 && pRichRing->getPMTs() > 2)
      richRing.fProbability = fabs(pRichRing->getProb());
    else
      richRing.fProbability = -fabs(pRichRing->getProb());

    fEvent->fRICH.fRings.append(richRing);
  }

  return true;
}
