#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceACC() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nAntiClusters = fAMSEvent->nAntiCluster();
  for (int antiClusterIndex = 0; antiClusterIndex < nAntiClusters; ++antiClusterIndex) {
    AntiClusterR* pAntiCluster = fAMSEvent->pAntiCluster(antiClusterIndex);
    if (!pAntiCluster) {
      WARN_OUT << "Problem building AC::ACCCluster[" << antiClusterIndex << "]: "
               << "AMS ROOT File is broken! pAntiCluster() returned null." << std::endl;
      return false;
    }

    AC::ACCCluster accCluster;
    accCluster.fStatus = pAntiCluster->Status;
    accCluster.fPhi = pAntiCluster->Coo[1];
    accCluster.fZ = pAntiCluster->Coo[2];
    accCluster.fEnergy = pAntiCluster->Edep;

    float minimumTime = std::numeric_limits<float>::max() - 1;
    int nTimes = pAntiCluster->Ntimes;
    for (int timeIndex = 0; timeIndex < nTimes; ++timeIndex)
      minimumTime = fabs(pAntiCluster->Times[timeIndex]) < fabs(minimumTime) ? pAntiCluster->Times[timeIndex] : minimumTime;
    accCluster.fTime = minimumTime;

    fEvent->fACC.fClusters.append(accCluster);
  }
  
  int nRawSides = fAMSEvent->nAntiRawSide();
  for (int antiRawSideIndex = 0; antiRawSideIndex < nRawSides; ++antiRawSideIndex) {
    AntiRawSideR* pAntiRawSide = fAMSEvent->pAntiRawSide(antiRawSideIndex);
    if (!pAntiRawSide) {
      WARN_OUT << "Problem building AC::ACCRawSide[" << antiRawSideIndex << "]: "
               << "AMS ROOT File broken! pAntiRawSide() returned null." << std::endl;
      return false;
    }

    AC::ACCRawSide accRawSide;
    accRawSide.fHWAddress = pAntiRawSide->swid;
    accRawSide.fStatus = pAntiRawSide->stat;
    accRawSide.fTemperature = pAntiRawSide->temp;
    accRawSide.fADC = pAntiRawSide->adca;

    for (int hitIndex = 0; hitIndex < pAntiRawSide->nftdc; ++hitIndex)
      accRawSide.fFTDCHits.append(pAntiRawSide->ftdc[hitIndex]);

    for (int timeIndex = 0; timeIndex < pAntiRawSide->ntdct; ++timeIndex)
      accRawSide.fTDCTimes.append(pAntiRawSide->tdct[timeIndex]);

    fEvent->fACC.fRawSides.append(accRawSide);
  }

  return true;
}
