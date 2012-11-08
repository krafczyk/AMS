#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceACC() {

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

  return true;
}
