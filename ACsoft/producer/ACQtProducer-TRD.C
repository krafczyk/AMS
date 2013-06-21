#include "ACQtProducer-include.C"

#include "SplineTrack.hh"
#include "TrackFactory.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "DetectorManager.hh"
#include "pathlength_functions.hh"

namespace ACsoft {

bool TRDExtraHitsSortAscending(const TRDExtraHit& h1, const TRDExtraHit& h2) {

  // sort smaller residual first
  return h1.residual < h2.residual ? true : false;
}

// FIXME: This should be a static private helper function in AC::TRDRawHit.
int HWAddress(const int Haddr){
  int Crate =  Haddr/10000;
  int   UDR = (Haddr/ 1000) %  10;
  int   UFE = (Haddr/  100) %  10;
  int    CH =  Haddr        % 100;
  return (Crate<<12) + (UDR<<9) + (UFE<<6) + CH;
}

bool ACQtProducer::ProduceTRDHSegments() {

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
        if ((fTRDRawHitIndices[k]&0xFFFF) == associatedRawHitIndex)
          break;
      }

      trdHSegment.fRawHitIndex.append(k);
      if (k == kmax)
        fTRDRawHitIndices.push_back(associatedRawHitIndex); 

      fTRDRawHitIndices[k] |= (1<<17);     // flagged "from-TRDH"
    }

    fEvent->fTRD.fHSegments.append(trdHSegment);
  }

  return true;
}

static inline bool LookupRigidityForTRDHTrackIndex(const AC::Event* event, AMSEventR* amsEvent, int htrackIndex, float& rigidity) {

  Q_ASSERT(event);
  Q_ASSERT(amsEvent);
  rigidity = 0; // FIXME: Preserve historical behaviour.

  // Lookup particle which is associated with the given htrackIndex.
  int nParticles = (int)event->fParticles.size();
  for (int particleIndex = 0; particleIndex < nParticles; ++particleIndex) {
    const AC::Particle& particle = event->fParticles[particleIndex];
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

bool ACQtProducer::ProduceTRDHTracks() {

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

bool ACQtProducer::ProduceTRDVTracks() {

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
          if ( (fTRDRawHitIndices[rawHitIndex]&0xFFFF) == associatedRawHitIndex)
            break;
        }

        if (rawHitIndex == nRawHits)
          fTRDRawHitIndices.push_back(associatedRawHitIndex); 

        fTRDRawHitIndices[rawHitIndex] |= (1<<16); // flagged "from-TRDV"
      }
    }

    fEvent->fTRD.fVTracks.append(trdVTrack);
  }

  return true;
}

bool ACQtProducer::ProduceTRD() {

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  fTRDRawHitIndices.clear();
  fTRDExtraHits.clear();
  fEvent->fTRD.fNumberOfHits = fAMSEvent->nTrdRawHit();

  // Inside AMS we don't produce any TRDVTracks/HTracks/HSegments, as that's not needed for TrdQt standalone processing.
#ifndef AMS_ACQT_INTERFACE
  if (!ProduceTRDVTracks())
    return false;

  if (!ProduceTRDHTracks())
    return false;

  if (!ProduceTRDHSegments())
    return false;
#endif

  static Analysis::TrackFactory fTrackFactory;
  Detector::Trd* shimmedTrd = Detector::DetectorManager::Self()->GetShimmedTrd();
  // Add more TrdRawHits if close to TrTrack - up to 50 in decreasing amplitude order

  // Map from TrackerTrack index to SplineTrack object, usable for z-interpolations.
  std::map<int, Analysis::SplineTrack*> splineTracks;
  std::vector<int> trackerTracksInTRD;

  int nTrackerTracks = (int)fEvent->fTracker.fTracks.size();
  for (int trackerTrackIndex = 0; trackerTrackIndex < nTrackerTracks; ++trackerTrackIndex) {

    // skip bad TrTracks:
    const AC::TrackerTrack& track = fEvent->fTracker.Tracks()[trackerTrackIndex];
    if (!track.TrackFits().size())                        continue;
    if (track.TrackFits()[0].ChiSquareNormalizedX()>15.0) continue;
    if (track.TrackFits()[0].ChiSquareNormalizedY()>15.0) continue;

    trackerTracksInTRD.push_back(trackerTrackIndex);
    Analysis::SplineTrack* splineTrack = fTrackFactory.CreateSplineTrackFrom(track.TrackFits()[0]);
    splineTracks.insert(std::make_pair(trackerTrackIndex, splineTrack));
  }

  int nRawHits = fAMSEvent->NTrdRawHit();
  for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
    TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(rawHitIndex);
    if (!pTrdRawHit) {
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Error finding TRD extra hits." << std::endl;
      return false;
    }

    AC::TRDRawHit trdRawHit;
    trdRawHit.fHWAddress = HWAddress(pTrdRawHit->Haddr);

    TRDExtraHit trdExtraHit;
    trdExtraHit.hitIndex = -1;
    trdExtraHit.amplitude = pTrdRawHit->Amp;
    trdExtraHit.residual = 999.9;

    for (int j = 0; j < (int)trackerTracksInTRD.size(); ++j) {
      int trackIndex = trackerTracksInTRD[j];

      Analysis::SplineTrack* splineTrack = splineTracks[trackIndex];
      assert(splineTrack);

      const Detector::TrdStraw* trdstraw = shimmedTrd->GetTrdStraw(trdRawHit.Straw());
      assert(trdstraw);

      TVector3 trackPos, trackDir;
      splineTrack->CalculateLocalPositionAndDirection(trdstraw->GlobalPosition().Z(), trackPos, trackDir);
      float residual = Distance(trdstraw->GlobalPosition(), trdstraw->GlobalWireDirection(), trackPos, trackDir);

      if (residual < 3.0 + (float)pTrdRawHit->Layer / 9.5) { // 3 cm for TRD-Bottom (layer-0)    5 cm for TRD-Top (layer-19)
        trdExtraHit.hitIndex = rawHitIndex; // candidate
        trdExtraHit.residual = min(residual, trdExtraHit.residual);  // keep closest Resid to TrTrack
      }
    }

    int k = 0, kmax = (int)fTRDRawHitIndices.size();
    for (; k < kmax; ++k) {
      if ( (fTRDRawHitIndices[k]&0xFFFF) == rawHitIndex)
        break;
    }

    if (trdExtraHit.hitIndex >= 0){ // hit close to TrackerTrack
      if (k == kmax)
        fTRDExtraHits.push_back(trdExtraHit);
      else
        fTRDRawHitIndices[k] |= (1<<18); // flagged "from-TRK"
    }
  }

  // sort according to rising residual
  std::sort(fTRDExtraHits.begin(), fTRDExtraHits.end(), &TRDExtraHitsSortAscending); 

  int NHused  = (int)fTRDRawHitIndices.size();
  int NHtrack = (int)fTRDExtraHits.size();
  int NHtot   = nRawHits; // from AMSEvent

  // Add fTRDExtraHits
  for (int i = 0; i < (int)fTRDExtraHits.size(); ++i)
    fTRDRawHitIndices.push_back(fTRDExtraHits[i].hitIndex | 1<<18 ); // flagged "from-TRK"

  // add remaining TrdRawHits:
  for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {
    TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(rawHitIndex);

    int k = 0, kmax = (int)fTRDRawHitIndices.size();
    for (; k < kmax; ++k) {
      if ( (fTRDRawHitIndices[k]&0xFFFF) == rawHitIndex)
        break;
    }

    if (k == kmax) fTRDRawHitIndices.push_back(rawHitIndex);   // hit was not yet in fTRDRawHitIndices

    if(  fStreamSelection==0 && (int)fTRDRawHitIndices.size() == 256 ) break;          // enough for SAA ...

  }


  // Save TrdRawHits:
  nRawHits = (int)fTRDRawHitIndices.size(); // from List
  if( fStreamSelection==0 ) nRawHits = min(256, nRawHits); // restrict to 256 hits inside SAA

  for (int rawHitIndex = 0; rawHitIndex < nRawHits; ++rawHitIndex) {

    if ( (fTRDRawHitIndices[rawHitIndex] & 0xFFFF) > (int)fAMSEvent->NTrdRawHit() - 1 )
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Raw hit index out of range." << std::endl;

    TrdRawHitR* pTrdRawHit = fAMSEvent->pTrdRawHit(fTRDRawHitIndices[rawHitIndex] & 0xFFFF);
    if (!pTrdRawHit) {
      WARN_OUT << "Problem building AC::TRD: AMS ROOT File is broken! Error saving TRD extra hits." << std::endl;
      return false;
    }


    AC::TRDRawHit trdRawHit;

    // FIXME: This should use a static private helper function from AC::TRDRawHit.
    int HWAdr = HWAddress(pTrdRawHit->Haddr) | ( (fTRDRawHitIndices[rawHitIndex] & 0x70000)>>3 );
    trdRawHit.fHWAddress =  HWAdr;
    trdRawHit.fADC = (UShort_t) (pTrdRawHit->Amp * 8 + 0.5);
    fEvent->fTRD.fRawHits.append(trdRawHit);
  }
  // printf("TRDhits used/track/event %4d / %4d / %4d  StreamSelection:%2d  saved: %4lu\n ", NHused,NHtrack,NHtot,fStreamSelection,fEvent->fTRD.fRawHits.size());

#ifndef DISABLE_ACQT_CONTROL_PLOTS
  h1_nTrdRawHit_All->Fill((float)NHtot);
  h1_nTrdRawHit_Used->Fill((float)NHused);
  h1_nTrdRawHit_Extra->Fill((float)NHtrack);
  h1_nTrdRawHit_UsPlEx->Fill((float)(NHused+NHtrack));
  h1_nTrdRawHit_Saved->Fill((float)nRawHits);
#endif

  return true;
}

}
