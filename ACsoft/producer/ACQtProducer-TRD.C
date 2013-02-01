#include "ACQtProducer-include.C"

#include "SplineTrack.hh"
#include "TrackFactory.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "DetectorManager.hh"
#include "pathlength_functions.hh"

bool TRDExtraHitsSortAscending(const ACsoft::TRDExtraHit& h1, const ACsoft::TRDExtraHit& h2) {

  // sort smaller residual first
  return h1.residual < h2.residual ? true : false;
}

bool ACsoft::ACQtProducer::ProduceTRDHSegments() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nHSegments = fAMSEvent->nTrdHSegment();
  for (int hsegmentIndex = 0; hsegmentIndex < nHSegments; ++hsegmentIndex) {
    TrdHSegmentR* pTrdHSegment = fAMSEvent->pTrdHSegment(hsegmentIndex);
    if (!pTrdHSegment) {
      WARN_OUT << "Problem building AC::TRDHSegment[" << hsegmentIndex << "]: "
               << "AMS ROOT File is broken! pTrdHSegment() returned null." << std::endl;
      return false;
    }

    AC::TRDHSegment trdHSegment;
    
    int nRawHits = pTrdHSegment->NTrdRawHit();
    for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
      int associatedRawHitIndex = pTrdHSegment->iTrdRawHit(rawHitIndex);

      int k = 0, kmax = (int)fTRDRawHitIndices.size();
      for (; k < kmax; ++k) {
        if (fTRDRawHitIndices[k] == associatedRawHitIndex)
          break;
      }

      trdHSegment.fRawHitIndex.append(k);
      if (k == kmax)
        fTRDRawHitIndices.push_back(associatedRawHitIndex);
    }

    fEvent->fTRD.fHSegments.append(trdHSegment);
  }

  return true;
}

static inline bool LookupRigidityForTRDHTrackIndex(const ACsoft::AC::Event* event, AMSEventR* amsEvent, int htrackIndex, float& rigidity) {

  Q_ASSERT(event);
  Q_ASSERT(amsEvent);
  rigidity = 0; // FIXME: Preserve historical behaviour.

  // Lookup particle which is associated with the given htrackIndex.
  int nParticles = (int)event->fParticles.size();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    const ACsoft::AC::Particle& particle = event->fParticles[particleIndex];
    if (particle.TRDHTrackIndex() != htrackIndex)
      continue;

    int trackerTrackIndex = particle.TrackerTrackIndex();
    if (trackerTrackIndex < 0 || !amsEvent->pTrTrack(trackerTrackIndex))
      return false;

    rigidity = amsEvent->pTrTrack(trackerTrackIndex)->GetRigidity();
    return true;
  }

  return false;
}

bool ACsoft::ACQtProducer::ProduceTRDHTracks() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nHTracks = fAMSEvent->nTrdHTrack();
  for (int htrackIndex = 0; htrackIndex < nHTracks; ++htrackIndex) {
    TrdHTrackR* pTrdHTrack = fAMSEvent->pTrdHTrack(htrackIndex);
    if (!pTrdHTrack) {
      WARN_OUT << "Problem building AC::TRDHTrack[" << htrackIndex << "]: "
               << "AMS ROOT File is broken! pTrdHTrack() returned null." << std::endl;
      return false;
    }

    if (pTrdHTrack->status & 0xFFFFFF00) {
      WARN_OUT << "Problem building AC::TRDHTrack[" << htrackIndex << "]: "
               << "pTrdHTrack->Status=" << pTrdHTrack->status << std::endl;
    }

    AC::TRDHTrack trdHTrack;
    trdHTrack.fStatus = pTrdHTrack->status & 0xff; 

    // M.M. TRD Charge estimators - without rigidity (Bethe-Bloch) correction -  stored in AMS-RootFiles B572+
    // improve with rigidity correction
    float rigidity = 0;
    LookupRigidityForTRDHTrackIndex(fEvent, fAMSEvent, htrackIndex, rigidity);

    if (rigidity != 0.0) {
      TrdHChargeR::gethead()->GetCharge(pTrdHTrack, rigidity, 0); // compute with rigidity (Bethe-Bloch) correction
      for (int j = 0; j <= 6; ++j)
        trdHTrack.fChargesNew.append(fabs(pTrdHTrack->GetProb(j)));
    } else {
      for (int j = 0; j <= 6; ++j)
        trdHTrack.fChargesNew.append(-fabs(pTrdHTrack->GetProb(j)));
    }

    if (pTrdHTrack->NTrdHSegment() == 2) {
      trdHTrack.fHSegmentIndex.append(pTrdHTrack->iTrdHSegment(pTrdHTrack->pTrdHSegment(0)->d));     // XZ
      trdHTrack.fHSegmentIndex.append(pTrdHTrack->iTrdHSegment(1 - pTrdHTrack->pTrdHSegment(0)->d)); // YZ
    } else {
      WARN_OUT << "Problem building AC::TRDHTrack[" << htrackIndex << "]: "
               << "pTrdHTrack->NTrdHSegment() returned " << pTrdHTrack->NTrdHSegment() << " expected 2." << std::endl;
    }

    fEvent->fTRD.fHTracks.append(trdHTrack);
  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceTRDVTracks() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  int nVTracks = fAMSEvent->nTrdTrack();
  for (int vtrackIndex = 0; vtrackIndex < nVTracks; ++vtrackIndex) { 
    TrdTrackR* pTrdTrack = fAMSEvent->pTrdTrack(vtrackIndex);
    if (!pTrdTrack) {
      WARN_OUT << "Problem building AC::TRDVTrack[" << vtrackIndex << "]: "
               << "AMS ROOT File is broken! pTrdTrack() returned null." << std::endl;
      return false;
    }

    if (pTrdTrack->Status & 0xFFFF0000) {
      WARN_OUT << "Problem building AC::TRDVTrack[" << vtrackIndex << "]: "
               << "pTrdTrack->Status=" << pTrdTrack->Status << std::endl;
    }

    AC::TRDVTrack trdVTrack;
    trdVTrack.fStatus = pTrdTrack->Status & 0xFFFF; 
    trdVTrack.fChiSquare = pTrdTrack->Chi2;
    trdVTrack.fTheta = M_PI - pTrdTrack->Theta; // Tracker convention
    trdVTrack.fPhi = pTrdTrack->Phi;
    trdVTrack.fPhi += trdVTrack.fPhi > 0.0 ? -M_PI : +M_PI; // Tracker convention
    trdVTrack.fX = pTrdTrack->Coo[0];
    trdVTrack.fY = pTrdTrack->Coo[1];
    trdVTrack.fZ = pTrdTrack->Coo[2];
    trdVTrack.fPattern = pTrdTrack->Pattern;

    int nSegments = pTrdTrack->NTrdSegment();
    for (int segmentIndex = 0; segmentIndex < nSegments; ++segmentIndex) {
      TrdSegmentR* pTrdSegment = pTrdTrack->pTrdSegment(segmentIndex);
      if (!pTrdSegment) {
        WARN_OUT << "Problem building AC::TRDVTrack[" << vtrackIndex << "]: "
                 << "AMS ROOT File is broken! pTrdTrack->pTrdSegment(" << segmentIndex << ") returned null." << std::endl;
        return false;
      }

      int nClusters = pTrdSegment->NTrdCluster();
      for (int clusterIndex = 0; clusterIndex < nClusters; ++clusterIndex) {
        TrdClusterR* pTrdCluster = pTrdSegment->pTrdCluster(clusterIndex);
        if (!pTrdCluster) {
          WARN_OUT << "Problem building AC::TRDVTrack[" << vtrackIndex << "]: "
                   << "AMS ROOT File is broken! pTrdSegment(" << segmentIndex << ")->pTrdCluster(" << clusterIndex << ") returned null." << std::endl;
          return false;
        }

        int associatedRawHitIndex = pTrdCluster->iTrdRawHit();

        int nRawHits = fTRDRawHitIndices.size();
        int rawHitIndex = 0;
        for(; rawHitIndex < nRawHits; ++rawHitIndex) {
          if (fTRDRawHitIndices[rawHitIndex] == associatedRawHitIndex)
            break;
        }

        if (rawHitIndex == nRawHits)
          fTRDRawHitIndices.push_back(associatedRawHitIndex);
      }
    }

    fEvent->fTRD.fVTracks.append(trdVTrack);
  }

  return true;
}

bool ACsoft::ACQtProducer::ProduceTRD() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  fTRDRawHitIndices.clear();
  fTRDExtraHits.clear();
  fEvent->fTRD.fNumberOfHits = fAMSEvent->nTrdRawHit();

  if (!ProduceTRDVTracks())
    return false;

  if (!ProduceTRDHTracks())
    return false;

  if (!ProduceTRDHSegments())
    return false;

  static Analysis::TrackFactory fTrackFactory;
  Detector::Trd* unalignedTrd = Detector::DetectorManager::Self()->GetUnalignedTrd();
  // Add more TrdRawHits if close to TrTrack - up to 50 in decreasing amplitude order

  // Map from TrackerTrack index to SplineTrack object, usable for z-interpolations.
  std::map<int, Analysis::SplineTrack*> splineTracks;
  std::vector<int> trackerTracksInTRD;

  int nParticles = (int)fEvent->fParticles.size();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    const AC::Particle& particle = fEvent->fParticles[particleIndex];
    int trackerTrackIndex = particle.TrackerTrackIndex();
    if (trackerTrackIndex < 0)
      continue;

    if (trackerTrackIndex >= (int)fEvent->fTracker.Tracks().size()) {
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Error finding TRD extra hits." << std::endl;
      return false;
    }

    int j = 0;
    for (; j < (int)trackerTracksInTRD.size(); ++j) {
      if (trackerTracksInTRD[j] == trackerTrackIndex)
        break;
    }

    if (j != (int)trackerTracksInTRD.size())
      continue;

    const AC::TrackerTrack& track = fEvent->fTracker.Tracks()[trackerTrackIndex];
    if (track.TrackFits().size()) {
      trackerTracksInTRD.push_back(trackerTrackIndex);
      Analysis::SplineTrack* splineTrack = fTrackFactory.CreateSplineTrackFrom(track.TrackFits()[0]);
      splineTracks.insert(std::make_pair(trackerTrackIndex, splineTrack));
    }
  }

  int nRawHits = fAMSEvent->NTrdRawHit();
  for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
    TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(rawHitIndex);
    if (!pTrdRawHit) {
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Error finding TRD extra hits." << std::endl;
      return false;
    }

    int k = 0, kmax = (int)fTRDRawHitIndices.size();
    for (; k < kmax; ++k) {
      if (fTRDRawHitIndices[k] == rawHitIndex)
        break;
    }

    if (k == kmax) {
      AC::TRDRawHit trdRawHit;
      trdRawHit.fHWAddress = pTrdRawHit->Haddr;

      TRDExtraHit trdExtraHit;
      trdExtraHit.hitIndex = -1;
      trdExtraHit.amplitude = pTrdRawHit->Amp;
      trdExtraHit.residual = 999.9;

      for (int j = 0; j < (int)trackerTracksInTRD.size(); ++j) {
        int trackIndex = trackerTracksInTRD[j];

        Analysis::SplineTrack* splineTrack = splineTracks[trackIndex];
        assert(splineTrack);

        const Detector::TrdStraw* trdstraw = unalignedTrd->GetTrdStraw(trdRawHit.Straw());
        assert(trdstraw);

        TVector3 trackPos, trackDir;
        splineTrack->CalculateLocalPositionAndDirection(trdstraw->GlobalPosition().Z(), trackPos, trackDir);
        float residual = Distance(trdstraw->GlobalPosition(), trdstraw->GlobalWireDirection(), trackPos, trackDir);

        if (residual < 3.0 + (float)pTrdRawHit->Layer / 9.5) { // 3 cm for TRD-Bottom (layer-0)    5 cm for TRD-Top (layer-19)
          trdExtraHit.hitIndex = rawHitIndex; // candidate
          trdExtraHit.residual = min(residual, trdExtraHit.residual);  // keep closest Resid to TrTrack
        }
      }

      if (trdExtraHit.hitIndex >= 0)
        fTRDExtraHits.push_back(trdExtraHit);
    }
  }

  // sort according to rising residual
  std::sort(fTRDExtraHits.begin(), fTRDExtraHits.end(), &TRDExtraHitsSortAscending); 

  int NHused  = (int)fTRDRawHitIndices.size();
  int NHtrack = (int)fTRDExtraHits.size();
  int NHtot   = nRawHits;
 
  // Add fTRDExtraHits
  for (int i = 0; i < (int)fTRDExtraHits.size(); ++i) fTRDRawHitIndices.push_back(fTRDExtraHits[i].hitIndex);


  // if still less than 256 hits, add more:
  if( (int)fTRDRawHitIndices.size() < 256 ) {

    for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
      TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(rawHitIndex);

      int k = 0, kmax = (int)fTRDRawHitIndices.size();
      for (; k < kmax; ++k) {
        if (fTRDRawHitIndices[k] == rawHitIndex)
          break;
      }

      if (k == kmax) fTRDRawHitIndices.push_back(rawHitIndex);   // hit was not yet in fTRDRawHitIndices

      if( (int)fTRDRawHitIndices.size() == 256 ) break;          // enough ...

    }
  }


  // Save TrdRawHits:
  nRawHits = min(256, (int)fTRDRawHitIndices.size());
  for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
    TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(rawHitIndex);
    if (!pTrdRawHit) {
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Error saving TRD extra hits." << std::endl;
      return false;
    }

    if (fTRDRawHitIndices[rawHitIndex] > (int)fAMSEvent->NTrdRawHit() - 1)
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Raw hit index out of range." << std::endl;

    AC::TRDRawHit trdRawHit; 
    trdRawHit.fHWAddress = fAMSEvent->pTrdRawHit(fTRDRawHitIndices[rawHitIndex])->Haddr;
    trdRawHit.fADC = (UShort_t) (fAMSEvent->pTrdRawHit(fTRDRawHitIndices[rawHitIndex])->Amp * 8 + 0.5);
    fEvent->fTRD.fRawHits.append(trdRawHit);
  }

#ifndef AMS_ACQT_SILENCE_COMMON_WARNINGS
  h1_nTrdRawHit_All->Fill((float)NHtot);
  h1_nTrdRawHit_Used->Fill((float)NHused);
  h1_nTrdRawHit_Extra->Fill((float)NHtrack);
  h1_nTrdRawHit_UsPlEx->Fill((float)(NHused+NHtrack));
  h1_nTrdRawHit_Saved->Fill((float)nRawHits);
#endif

  // printf("TRD RawHits  used/extra/all %4d/%4d/%4d\n",NHused,NHtrack,NHtot);

  return true;
}
