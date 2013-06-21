#include "ACQtProducer-include.C"

bool ACsoft::ACQtProducer::ProduceACC() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  if ( fAMSEvent->Version() <= 630 ) fAMSEvent->RebuildAntiClusters();

  int nAntiClusters = fAMSEvent->nAntiCluster();
  for (int antiClusterIndex = 0; antiClusterIndex < nAntiClusters; ++antiClusterIndex) {
    AntiClusterR* pAntiCluster = fAMSEvent->pAntiCluster(antiClusterIndex);
    if (!pAntiCluster) {
      WARN_OUT << "Problem building AC::ACCCluster[" << antiClusterIndex << "]: "
               << "AMS ROOT File is broken! pAntiCluster() returned null." << std::endl;
      return false;
    }
  
    AC::ACCCluster accCluster;
    accCluster.fStatus = pAntiCluster->Npairs;
    accCluster.fTime = pAntiCluster->time;
    accCluster.fZ = pAntiCluster->unfzeta;
    accCluster.fPhi = pAntiCluster->phi;
    accCluster.fEnergy = pAntiCluster->Edep;
  
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
