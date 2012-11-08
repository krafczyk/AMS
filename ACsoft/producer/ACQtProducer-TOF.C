#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceTOFBetas() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);
 
  int nBetas = fAMSEvent->nBeta();
  for (int betaIndex = 0; betaIndex < nBetas; ++betaIndex) {
    BetaR* pBeta = fAMSEvent->pBeta(betaIndex);
    if (!pBeta) {
      WARN_OUT << "Problem building AC::TOFBeta[" << betaIndex << "]: "
               << "AMS ROOT File is broken! pBeta() returned null." << std::endl;
      return false;
    }

    AC::TOFBeta tofBeta;
    tofBeta.fBeta = pBeta->BetaC;

    if (pBeta->Pattern <= 4)
      tofBeta.fBetaError = abs(pBeta->ErrorC); // nLay==3,4
    else
      tofBeta.fBetaError = -abs(pBeta->ErrorC); // nLay==2

    // Lookup particle which is associated with this TOFBeta object.
    int trackerTrackIndex = -1;
    int nParticles = (int)fEvent->fParticles.size();
    for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
      const AC::Particle& particle = fEvent->fParticles[particleIndex];
      if (particle.TOFBetaIndex() == betaIndex) {
        trackerTrackIndex = particle.TrackerTrackIndex();
        break;
      }
    }

    if (trackerTrackIndex >= 0) { 
      // Try to associate the AC::TOFBeta object with AC::TOFCluster objects.
      TrTrackR* pTrTrack = fAMSEvent->pTrTrack(trackerTrackIndex);
      TofTrack* TOFTrack = new TofTrack(pBeta, pTrTrack);
      for (int layer = 1; layer <= 4; ++layer) {
        float charge = TOFTrack->GetChargeLayer(layer);
        if (charge < 1e-3)
          continue;

        tofBeta.fChargesNew.append(charge);

        // Find associated TOFCluster.
        int nClusters = fAMSEvent->nTofCluster();
        int clusterIndex = 0;
        for (; clusterIndex < nClusters; ++clusterIndex) { 
          if (fAMSEvent->pTofCluster(clusterIndex) == TOFTrack->GetHitLayer(layer)) 
            break;
        }

        if (clusterIndex < nClusters)
          tofBeta.fTOFClusterIndex.append(clusterIndex);  
        else {
          WARN_OUT << "Problem building AC::TOFBeta[" << betaIndex << "]: "
                   << "TofTrack: no hit found for layer " << layer << std::endl;
          tofBeta.fTOFClusterIndex.append(-layer);
        } 
      }

      delete TOFTrack;
    }

    fEvent->fTOF.fBetas.append(tofBeta);
  }

  return true;
}

bool ACQtProducer::ProduceTOFClusters() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nClusters = fAMSEvent->nTofCluster();
  for (int clusterIndex = 0; clusterIndex < nClusters; ++clusterIndex) { 
    TofClusterR* pTofCluster = fAMSEvent->pTofCluster(clusterIndex);
    if (!pTofCluster) {
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "AMS ROOT File is broken! pTofCluster() returned null." << std::endl;
      return false;
    }

    if (pTofCluster->Status & 0x1) {
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "status: " << pTofCluster->Status << std::endl;

      int nRawClusters = pTofCluster->NTofRawCluster();
      for (int rawClusterIndex = 0; rawClusterIndex < nRawClusters; ++rawClusterIndex)
        WARN_OUT << "\traw cluster[" << rawClusterIndex << "] status: " << pTofCluster->pTofRawCluster(rawClusterIndex)->Status << std::endl;
    }

    AC::TOFCluster TOFCluster;
    TOFCluster.fStatus = pTofCluster->Status;
    TOFCluster.fLayerBar = AC::TOFCluster::GenerateBitsFromFlags(pTofCluster->Layer, pTofCluster->Bar);

    if (TOFCluster.Layer() != pTofCluster->Layer)
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "Layer don't match, got: " << TOFCluster.Layer() << " expected: " << pTofCluster->Layer << std::endl;

    if (TOFCluster.Bar() != pTofCluster->Bar)
      WARN_OUT << "Problem building AC::TOFCluster[" << clusterIndex << "]: "
               << "Bar don't match, got: " << TOFCluster.Bar() << " expected: " << pTofCluster->Bar << std::endl;

    TOFCluster.fX = pTofCluster->Coo[0];
    TOFCluster.fY = pTofCluster->Coo[1];
    TOFCluster.fZ = pTofCluster->Coo[2];
    TOFCluster.fTime = pTofCluster->Time;
    TOFCluster.fTimeError = pTofCluster->ErrTime;
    TOFCluster.fEnergy = pTofCluster->Edep;

    fEvent->fTOF.fClusters.append(TOFCluster);
  }

  return true;
}

bool ACQtProducer::ProduceTOF() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  bool successfull = ProduceTOFBetas();
  successfull &= ProduceTOFClusters();
  return successfull;
}
