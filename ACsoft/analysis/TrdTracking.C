#include "TrdTracking.hh"

#include "AMSGeometry.h"
#include "Event.h"
#include "DetectorManager.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "TrdLayer.hh"
#include "TrdSublayer.hh"
#include "TrdSegment.hh"
#include "TrdTrack.hh"
#include "TrdTrackFitFunction.hh"
#include "SplineTrack.hh"
#include "TrackfindingHistogram.hh"

#include <TH2.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TVector2.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdTracking> "
#include <debugging.hh>

ACsoft::Analysis::TrdTracking::TrdTracking( int verboseLevel, bool fillMinosErrors ) :
  fVerboseLevel(verboseLevel),
  fFillMinosErrors(fillMinosErrors),
  fRefillTrackfindingHistos(false)
{

  fNumberOfPairsRequiredXZ = 5;
  fNumberOfPairsRequiredYZ = 3;
  double nPairsRequiredForMaximumXZ = 3.0;
  double nPairsRequiredForMaximumYZ = 3.0;

  // be conservative here, if we have two neighbouring straws on the same sublayer, treat them in the likelihood accordingly
  fMaxDistance = 2.0*ACsoft::AC::AMSGeometry::TRDTubeRadius + 0.05;
  fMaxDistanceCoarse = 2.0*fMaxDistance;

  //
  // to minimize correlations between slope and offset, calculate all offsets with respect to z(center of TRD)
  //
  fZref_full = ACsoft::AC::AMSGeometry::ZTRDCenter;
  ACsoft::Detector::Trd* shimmedTrd = ACsoft::Detector::DetectorManager::Self()->GetShimmedTrd();
  assert(shimmedTrd);
  fZref_bottom = 0.5 * ( shimmedTrd->GetTrdLayer(1)->GlobalPosition().Z() + shimmedTrd->GetTrdLayer(2)->GlobalPosition().Z() );

  const int nBinsOffset  = TMath::Nint(ACsoft::AC::AMSGeometry::TRDMaximumStrawLength / ACsoft::AC::AMSGeometry::TRDTubeRadius / 2.0);
  const double maxOffset = 0.5*ACsoft::AC::AMSGeometry::TRDMaximumStrawLength;

  const double maxSlope = tan(50.0*TMath::DegToRad());
  const int nBinsSlope  = TMath::Nint(maxSlope / 0.01); // slope is defined by 6mm over 20 layers (2.9cm each) = 6/580=~0.01

  fTrackFindingHistoXZ = new Utilities::TrackfindingHistogram( "trackFindingHistoXZ", "TRD track finding XZ;offset at z_{ref} (cm);slope (dx/dz);number of pairs",
                                                               nBinsOffset,-maxOffset,maxOffset, nBinsSlope,-maxSlope,maxSlope, nPairsRequiredForMaximumXZ );
  fTrackFindingHistoYZ = new Utilities::TrackfindingHistogram( "trackFindingHistoYZ", "TRD track finding YZ;offset at z_{ref} (cm);slope (dy/dz);number of pairs",
                                                               nBinsOffset,-maxOffset,maxOffset, nBinsSlope,-maxSlope,maxSlope, nPairsRequiredForMaximumYZ );
  fTrackFindingHistoYZ_bottom = new Utilities::TrackfindingHistogram( "trackFindingHistoYZ_bottom", "coarse TRD track finding in bottom of YZ projection;offset at z_{ref} (cm);slope (dy/dz);number of pairs",
                                                                      nBinsOffset/2,-maxOffset,maxOffset, nBinsSlope/5,-maxSlope,maxSlope, nPairsRequiredForMaximumYZ );

  fTrackFindingHistoXZ->SetStats(kFALSE);
  fTrackFindingHistoYZ->SetStats(kFALSE);
  fTrackFindingHistoYZ_bottom->SetStats(kFALSE);

  // initialize minimizer
  fMinimizer = ROOT::Math::Factory::CreateMinimizer("Minuit", "Migrad");

  fMinimizer->SetMaxFunctionCalls(10000);
  fMinimizer->SetTolerance(0.001);
  fMinimizer->SetPrintLevel(fVerboseLevel);
  fMinimizer->SetErrorDef(0.5); // FIXME correct? study based on MC deviations

  fStopwatchSegmentFinding.Reset();
  fStopwatchTrackfindingHistoFilling.Reset();
  fStopwatchMaximaFinding.Reset();
  fStopwatchSegmentRemoving.Reset();
  fStopwatchForTesting.Reset();
  fStopwatchFit.Reset();
  fStopwatchMinos.Reset();
  fStopwatchMissingHits.Reset();

  fSegmentsXZ.reserve(20);
  fSegmentsYZ.reserve(20);
  fSegmentsYZ_bottom.reserve(20);
}

void ACsoft::Analysis::TrdTracking::PrintProcessingTimes() const {

  INFO_OUT << "Processing times:" << std::endl;
  INFO_OUT << "Segment finding:" << std::endl;
  fStopwatchSegmentFinding.Print();
  INFO_OUT << "  Filling of trackfinding histograms:" << std::endl;
  fStopwatchTrackfindingHistoFilling.Print();
  INFO_OUT << "  Maxima finding:" << std::endl;
  fStopwatchMaximaFinding.Print();
  INFO_OUT << "  Segment removing:" << std::endl;
  fStopwatchSegmentRemoving.Print();
  INFO_OUT << "     Test:" << std::endl;
  fStopwatchForTesting.Print();
  INFO_OUT << "Track fits:" << std::endl;
  fStopwatchFit.Print();
  INFO_OUT << "Minos:" << std::endl;
  fStopwatchMinos.Print();
  INFO_OUT << "Missing hits:" << std::endl;
  fStopwatchMissingHits.Print();
}


/** Identify segments and store them in the corresponding result vectors inside the class. */
void ACsoft::Analysis::TrdTracking::FindTrdSegments( const ACsoft::AC::Event& event ) {

  fStopwatchSegmentFinding.Start(kFALSE);

  fSegmentsXZ.clear();
  fSegmentsYZ.clear();
  fSegmentsYZ_bottom.clear();

  const ACsoft::AC::TRD::RawHitsVector& trdRawHits = event.TRD().RawHits();

  DEBUG_OUT << "Getting aligned TRD" << std::endl;
  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  if( trdRawHits.size() < 2 ) return;

  //
  // make two lists of vectors, one for XZ hits, one for YZ hits
  //
  std::vector<const ACsoft::AC::TRDRawHit*> rawHitsXZ;
  std::vector<const ACsoft::AC::TRDRawHit*> rawHitsYZ;
  std::vector<const ACsoft::AC::TRDRawHit*> rawHitsYZ_bottom;
  rawHitsXZ.reserve(trdRawHits.size());
  rawHitsYZ.reserve(trdRawHits.size());
  rawHitsYZ_bottom.reserve(trdRawHits.size());
  for( unsigned int rawHitCtr = 0 ; rawHitCtr < trdRawHits.size(); ++rawHitCtr ) {
    const ACsoft::AC::TRDRawHit& rawHit = trdRawHits[rawHitCtr];
    Short_t strawNumber = rawHit.Straw();
    const ACsoft::Detector::TrdStraw* straw = trd->GetTrdStraw(strawNumber);
    ACsoft::AC::MeasurementMode dir = straw->Direction();
    if( dir == ACsoft::AC::XZMeasurement )
      rawHitsXZ.push_back(&rawHit);
    else if( dir == ACsoft::AC::YZMeasurement ){
      rawHitsYZ.push_back(&rawHit);
      if( straw->LayerNumber() < 4 )
        rawHitsYZ_bottom.push_back(&rawHit);
    }
  }

  FindTrdSegmentsForGivenDirection( rawHitsXZ, fTrackFindingHistoXZ, ACsoft::AC::XZMeasurement, true, fMaxDistance, fZref_full, fSegmentsXZ );
  FindTrdSegmentsForGivenDirection( rawHitsYZ, fTrackFindingHistoYZ, ACsoft::AC::YZMeasurement, true, fMaxDistance, fZref_full, fSegmentsYZ );
  if( !fSegmentsYZ.size() )
    FindTrdSegmentsForGivenDirection( rawHitsYZ_bottom, fTrackFindingHistoYZ_bottom, ACsoft::AC::YZMeasurement,
                                      false, fMaxDistanceCoarse, fZref_bottom, fSegmentsYZ_bottom );

  fStopwatchSegmentFinding.Stop();
}

/** Segment finding in one projection.
  *
  * \param[in] trdRawHits vector of pointer to the TRD raw hits for the given measurement direction
  * \param[in] dir projection to be considered, has to match the measurement direction of the hits in \p trdRawHits
  * \param[in] addMissingHits add missing hits to segment?
  * \param[in] maxDistance maximum distance for a hit to be added as a candidate hit on the segment
  * \param[in] refZ referenceZ at which to calculate offset (cm)
  * \param[out] resultVector vector where results will be stored in
  */
void ACsoft::Analysis::TrdTracking::FindTrdSegmentsForGivenDirection( const std::vector<const ACsoft::AC::TRDRawHit*>& trdRawHits,
                                                              ::Utilities::TrackfindingHistogram* trackFindingHisto,
                                                              const ACsoft::AC::MeasurementMode dir,
                                                              bool addMissingHits, float maxDistance, double refZ,
                                                              std::vector<ACsoft::Analysis::TrdSegment>& resultVector ) {

  DEBUG_OUT << dir << std::endl;
  resultVector.clear();

  // FIXME in busy events, removing hits successively might be a good idea

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  FillTrackfindingHistogram( trdRawHits, dir, refZ, trackFindingHisto );

  //
  // find maxima in track finding histogram
  //
  fStopwatchMaximaFinding.Start(kFALSE);
  bool stillFoundSegment = false;
  do {
    stillFoundSegment = false;
    Int_t maxOffsetBin,maxSlopeBin;

    fStopwatchForTesting.Start(kFALSE);
    Double_t pairsMaxBin = trackFindingHisto->FindMaximumBinFast(maxOffsetBin,maxSlopeBin);
    fStopwatchForTesting.Stop();
    if(pairsMaxBin==0.0) break;

    DEBUG_OUT << "Found non-zero maximum " << pairsMaxBin << " in histo " << trackFindingHisto->GetName() << " at position " << maxOffsetBin << " " << maxSlopeBin << std::endl;

    //
    // calculate weighted mean around maxima
    // and set bins in vicinity of maximum to zero
    //
    int nBinsAroundMaximum = 1;
    // find bin range for calculation of weighted mean, take care not to extend this into under/overflow
    int firstBinAroundMaximumOffset = maxOffsetBin - nBinsAroundMaximum;
    if(firstBinAroundMaximumOffset<=0) firstBinAroundMaximumOffset = 1;
    int lastBinAroundMaximumOffset = maxOffsetBin + nBinsAroundMaximum;
    if(lastBinAroundMaximumOffset>trackFindingHisto->GetNbinsX()) lastBinAroundMaximumOffset = trackFindingHisto->GetNbinsX();
    int firstBinAroundMaximumSlope = maxSlopeBin - nBinsAroundMaximum;
    if(firstBinAroundMaximumSlope<=0) firstBinAroundMaximumSlope = 1;
    int lastBinAroundMaximumSlope = maxSlopeBin + nBinsAroundMaximum;
    if(lastBinAroundMaximumSlope>trackFindingHisto->GetNbinsY()) lastBinAroundMaximumSlope = trackFindingHisto->GetNbinsY();

    int nBinsCtr = 0;
    Double_t nPairsTotal = 0.0;
    TVector2 weightedSum(0.,0.); // (offset,slope)
    Double_t sumOfWeights = 0.;
    for( int bx = firstBinAroundMaximumOffset ; bx <= lastBinAroundMaximumOffset ; ++bx ) {
      for( int by = firstBinAroundMaximumSlope ; by <= lastBinAroundMaximumSlope ; ++by ) {
        Double_t nPairsBin = trackFindingHisto->GetBinContent(bx,by);
        weightedSum  += TVector2( nPairsBin*trackFindingHisto->GetXaxis()->GetBinCenter(bx), nPairsBin*trackFindingHisto->GetYaxis()->GetBinCenter(by) );
        sumOfWeights += nPairsBin;
        nPairsTotal += nPairsBin;
        ++nBinsCtr;

        // set bins in vicinity of maximum to zero
        trackFindingHisto->SetBinContent(bx,by,0.0);
      }
    }

    if(sumOfWeights>0.0) weightedSum /= sumOfWeights;
    else WARN_OUT << "Non-positive sum of weights!" << std::endl;


    if( ( dir==ACsoft::AC::XZMeasurement && nPairsTotal >= fNumberOfPairsRequiredXZ ) ||
        ( dir==ACsoft::AC::YZMeasurement && nPairsTotal >= fNumberOfPairsRequiredYZ ) ) {

      ACsoft::Analysis::TrdSegment segment(dir);
      segment.fOffsetReferenceZ = refZ;
      segment.fOffsetTrackFinding = weightedSum.X();
      segment.fSlopeTrackFinding  = weightedSum.Y();
      segment.fNumberOfPairsCloseToMaximum = TMath::Nint(nPairsTotal);

      //
      // add candidate hits to segment based on horizontal (xy) distance
      //
      for( unsigned int rawHitCtr = 0 ; rawHitCtr < trdRawHits.size() ; ++rawHitCtr ) {

        const ACsoft::AC::TRDRawHit* rawHit = trdRawHits[rawHitCtr];
        TVector3 pos = trd->HitPosition(rawHit);
        Double_t xyHit = dir==ACsoft::AC::XZMeasurement ? pos.X() : pos.Y();
        float xySegment = segment.EvalXYFromTrackFinding(pos.Z());
        if( fabs(xyHit-xySegment) < maxDistance )
          segment.fTrdRawHits.push_back(rawHit);
      }

      if(DEBUG)
        segment.Dump();

      resultVector.push_back(segment);
      stillFoundSegment = true;
    }
  } while( stillFoundSegment );

  fStopwatchMaximaFinding.Stop();

  //
  // remove segments that share more than a few hits with another segment, choosing the stronger one
  // first, go for more sublayers present in the segment,
  // as tie-breaker, look at number of pairs in trackfinding histo
  //
  fStopwatchSegmentRemoving.Start(kFALSE);
  if( resultVector.size() == 0 ) return;
  while(true){
    bool removedSegment = false;
    for( unsigned int i=0 ; i<resultVector.size() - 1 ; ++i ){
      const ACsoft::Analysis::TrdSegment& segment1 = resultVector[i];
      for( unsigned int j=i+1 ; j<resultVector.size() ; ++j ){
        const ACsoft::Analysis::TrdSegment& segment2 = resultVector[j];

        if( segment1.OverlapsWith(segment2) ){

          if( segment1.NumberOfSublayersInSegment() > segment2.NumberOfSublayersInSegment() )
            resultVector.erase(resultVector.begin()+j);
          else if( segment1.NumberOfSublayersInSegment() < segment2.NumberOfSublayersInSegment() )
            resultVector.erase(resultVector.begin()+i);
          else{ // equal number of sublayers present in both segments
            if(segment1.NumberOfPairsCloseToMaximum() >= segment2.NumberOfPairsCloseToMaximum())
              resultVector.erase(resultVector.begin()+j);
            else
              resultVector.erase(resultVector.begin()+i);
          }
          removedSegment = true;
          break;
        }
      }
      if(removedSegment) break;
    }

    if(!removedSegment) break;
  }
  fStopwatchSegmentRemoving.Stop();

  if( addMissingHits ) {
    DEBUG_OUT << "Adding missing hits to remaining segments:" << std::endl;

    // add missing hits
    for( unsigned int i=0 ; i<resultVector.size() ; ++i ){
      if(resultVector[i].Direction() == dir )
        AddMissingHitsToSegment(trdRawHits,resultVector[i]);
      if(DEBUG) resultVector[i].Dump();
    }
  }

  //
  // remove segments with more than three missing hits in XZ or two missing hits in YZ
  // these are almost always spurious
  // exception: there is only one segment in the projection at hand
  //
  if( resultVector.size() > 1 ){
    while(true){
      bool removedSegment = false;
      for( unsigned int i=0 ; i<resultVector.size() ; ++i ){
        const ACsoft::Analysis::TrdSegment& segment = resultVector[i];
        if( ( segment.Direction() == ACsoft::AC::XZMeasurement && segment.NumberOfMissingHits() > 3 ) ||
            ( segment.Direction() == ACsoft::AC::YZMeasurement && segment.NumberOfMissingHits() > 2 ) ){
          resultVector.erase(resultVector.begin()+i);
          removedSegment = true;
          break;
        }
      }
      if(!removedSegment) break;
    }
  }

  // restore the original track finding histogram if needed
  if( fRefillTrackfindingHistos ){
    DEBUG_OUT << "Refilling trackfinding histo for drawing purposes..." << std::endl;
    FillTrackfindingHistogram( trdRawHits, dir, refZ, trackFindingHisto );
  }
}


/** Fill trackfinding histogram for the given direction.
  *
  * \param[in] trdRawHits vector of %TRD raw hits in direction \p dir
  *
  * \attention The vector given as input must only contain raw hits for direction \p dir.
  */
void ACsoft::Analysis::TrdTracking::FillTrackfindingHistogram( const std::vector<const ACsoft::AC::TRDRawHit*>& trdRawHits,
                                                       const ACsoft::AC::MeasurementMode dir, double refZ,
                                                       ::Utilities::TrackfindingHistogram* trackFindingHisto ) {

  if(!(trdRawHits.size())) return;

  fStopwatchTrackfindingHistoFilling.Start(kFALSE);

  DEBUG_OUT << "Resetting track-finding histo" << std::endl;
  trackFindingHisto->Reset();

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  //
  // for each pair of TRD raw hits, calculate slope and offset
  //
  for( unsigned int rawHitCtr1 = 0 ; rawHitCtr1 < trdRawHits.size() - 1; ++rawHitCtr1 ) {

    const ACsoft::AC::TRDRawHit* rawHit1 = trdRawHits[rawHitCtr1];
    Short_t strawNumber1 = rawHit1->Straw();
    const ACsoft::Detector::TrdStraw* straw1 = trd->GetTrdStraw(strawNumber1);
    TVector3 pos1 = straw1->GlobalPosition();

    for( unsigned int rawHitCtr2 = rawHitCtr1+1 ; rawHitCtr2 < trdRawHits.size() ; ++rawHitCtr2 ) {

      const ACsoft::AC::TRDRawHit* rawHit2 = trdRawHits[rawHitCtr2];
      Short_t strawNumber2 = rawHit2->Straw();
      const ACsoft::Detector::TrdStraw* straw2 = trd->GetTrdStraw(strawNumber2);
      TVector3 pos2 = straw2->GlobalPosition();

      // ignore pairs on same sublayer
      if( straw1->LayerNumber()    == straw2->LayerNumber() &&
          straw1->SublayerNumber() == straw2->SublayerNumber() ) continue;

      Double_t dz = pos2.Z() - pos1.Z();
      Double_t dxy = dir == ACsoft::AC::XZMeasurement ? pos2.X() - pos1.X() : pos2.Y() - pos1.Y();
      Double_t slope = dxy/dz;
      Double_t offset = dir == ACsoft::AC::XZMeasurement ? pos1.X() - (pos1.Z() - refZ) * slope : pos1.Y() - (pos1.Z() - refZ) * slope;
      trackFindingHisto->FillFast(offset,slope);
    }
  }
  // end of loop over all pairs of TRD raw hits

  fStopwatchTrackfindingHistoFilling.Stop();
}

/** Draw the histograms used for track finding. */
void ACsoft::Analysis::TrdTracking::DrawTrackFindingHistograms() {

  if( !fRefillTrackfindingHistos )
    WARN_OUT << "Call SetRefillTrackfindingHistos(true) when creating the TrdTracking object if you want to draw the trackfinding histograms!" << std::endl;

  TCanvas* canvas = new TCanvas("trdTrackFindingCanvas", "TRD track finding");
  canvas->Divide(3,1);

  canvas->cd(1);
  fTrackFindingHistoXZ->Draw("colz");

  canvas->cd(2);
  fTrackFindingHistoYZ->Draw("colz");

  canvas->cd(3);
  fTrackFindingHistoYZ_bottom->Draw("colz");
}


/** Combine segments into tracks: TRD standalone algorithm.
  *
  * If there is exactly one XZ segment and exactly one YZ segment, combine the two into a track and fit it.
  *
  * \retuns Number of tracks created.
  */
int ACsoft::Analysis::TrdTracking::CreateTrdTracksFromSegments_TrdStandalone( std::vector<TrdTrack>& resultVector,
                                                                      const ACsoft::AC::Event& event ) {

  resultVector.clear();

  if(fSegmentsXZ.size() == 1 && fSegmentsYZ.size() == 1) {

    ACsoft::Analysis::TrdTrack track( &(fSegmentsXZ[0]), &(fSegmentsYZ[0]) );

    bool minimizationSuccess = FitAndAddResults(track,event);

    if(minimizationSuccess)
      resultVector.push_back(track);
  }

  return resultVector.size();
}


/** Combine segments into tracks: Based on existing tracker SplineTrack.
  *
  * Finds the segments in XZ and YZ closest to the SplineTrack, combines them into a track and fits the track.
  *
  * \retuns Number of tracks created.
  */
int ACsoft::Analysis::TrdTracking::CreateTrdTracksFromSegments_SplineTrack( const ACsoft::Analysis::SplineTrack* splineTrack,
                                                                    std::vector<TrdTrack>& resultVector,
                                                                    const ACsoft::AC::Event& event ) {

  resultVector.clear();
  if( !splineTrack ) return 0;

  //
  // we test the difference between spline track and segments at three positions: trd top, center, bottom,
  // and call the sum of the absolute deltaZ values our norm
  //

  if(fSegmentsXZ.size() >= 1 && fSegmentsYZ.size() >= 1) {

    unsigned int xzIndex = 0;
    unsigned int yzIndex = 0;
    float xzSmallestNorm = 1.e10;
    float yzSmallestNorm = 1.e10;

    for( unsigned int i=0 ; i<fSegmentsXZ.size() ; ++i ){
      float normXZ = 0.;
      normXZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDUpper).X()  - fSegmentsXZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDUpper) );
      normXZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDCenter).X() - fSegmentsXZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDCenter) );
      normXZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDLower).X()  - fSegmentsXZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDLower) );
      if( normXZ < xzSmallestNorm ){
        xzSmallestNorm = normXZ;
        xzIndex = i;
      }
    }
    for( unsigned int i=0 ; i<fSegmentsYZ.size() ; ++i ){
      float normYZ = 0.;
      normYZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDUpper).Y()  - fSegmentsYZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDUpper) );
      normYZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDCenter).Y() - fSegmentsYZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDCenter) );
      normYZ += fabs( splineTrack->InterpolateToZ(ACsoft::AC::AMSGeometry::ZTRDLower).Y()  - fSegmentsYZ[i].EvalXYFromTrackFinding(ACsoft::AC::AMSGeometry::ZTRDLower) );
      if( normYZ < yzSmallestNorm ){
        yzSmallestNorm = normYZ;
        yzIndex = i;
      }
    }

    ACsoft::Analysis::TrdTrack track( &(fSegmentsXZ[xzIndex]), &(fSegmentsYZ[yzIndex]) );

    bool minimizationSuccess = FitAndAddResults(track,event);

    if(minimizationSuccess)
      resultVector.push_back(track);
  }

  return resultVector.size();
}


int ACsoft::Analysis::TrdTracking::CreateTrdTracksFromSegments_GeneratorTrack( std::vector<TrdTrack>& resultVector,
                                                                       const ACsoft::AC::Event& /*event*/ ) {

  resultVector.clear();

  // FIXME implement!
  WARN_OUT << "Implement me along the lines of CreateTrdTracksFromSegments_SplineTrack() !" << std::endl;

  return resultVector.size();
}


/** Find missing hits for the given segment.
  *
  *
  * \returns number of missing hits found
  */
int ACsoft::Analysis::TrdTracking::AddMissingHitsToSegment( const std::vector<const ACsoft::AC::TRDRawHit*>& trdRawHits, ACsoft::Analysis::TrdSegment& segment ) {

  fStopwatchMissingHits.Start(kFALSE);

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  segment.fTrdMissingHits.clear();

  // loop TRD sublayers (but not outermost ones)
  for( int slNumber = 1 ; slNumber < int(ACsoft::AC::AMSGeometry::TRDSubLayers)-1 ; ++slNumber ) {

    if( segment.HasRawHitOnSublayer(slNumber) ) continue;

    const ACsoft::Detector::TrdSublayer* sublayer = trd->GetTrdSublayer(slNumber);
    if( segment.Direction() != sublayer->Direction() ) continue;

    // not last or first sublayer on segment
    if( slNumber <= segment.LastSublayerInSegment() || slNumber >= segment.FirstSublayerInSegment() ) continue;

    // identify straws closest to segment
    float zSublayer = sublayer->GlobalPosition().Z();
    ACsoft::AC::MeasurementMode segmentDir = segment.Direction();
    float xSegment  = segmentDir == ACsoft::AC::XZMeasurement ? segment.EvalXYFromTrackFinding(zSublayer) : 0.0;
    float ySegment  = segmentDir == ACsoft::AC::YZMeasurement ? segment.EvalXYFromTrackFinding(zSublayer) : 0.0;
    std::vector<unsigned short> strawCandidates;
    ACsoft::AC::AMSGeometry::Self()->CollectStrawCandidates(slNumber,xSegment,ySegment,strawCandidates);

    DEBUG_OUT << "On Sublayer " << slNumber << ": " << strawCandidates.size() << " straw candidates for missing hits around "
              << xSegment << " " << ySegment << std::endl;
    if(DEBUG){
      for(unsigned int i=0 ; i<strawCandidates.size();++i)
        trd->GetTrdStraw(strawCandidates[i])->Dump();
    }

    // check if segment has one left and right immediate neighbour straw
    const ACsoft::Detector::TrdStraw* leftNeighbour = 0;
    const ACsoft::Detector::TrdStraw* rightNeighbour = 0;
    float xySegment = segmentDir == ACsoft::AC::XZMeasurement ? xSegment : ySegment;
    for( unsigned int i=0 ; i<strawCandidates.size(); ++i ) {
     const ACsoft::Detector::TrdStraw* straw = trd->GetTrdStraw(strawCandidates[i]);
     float xyStraw = segmentDir == ACsoft::AC::XZMeasurement ? straw->GlobalPosition().X() : straw->GlobalPosition().Y();
     float delta = xySegment - xyStraw;
     if( delta > 0.0 && delta < ACsoft::AC::AMSGeometry::TRDTubeDiameter) leftNeighbour = straw;
     if( delta < 0.0 && -delta < ACsoft::AC::AMSGeometry::TRDTubeDiameter ) rightNeighbour = straw;
    }

    if( !leftNeighbour || !rightNeighbour ) continue;

    // and no raw hit in either neighbour
    bool rawHitInLeftNeighbour = false;
    bool rawHitInRightNeighbour = false;

    for( unsigned int rh=0 ; rh<trdRawHits.size() ; ++rh ){
      const ACsoft::AC::TRDRawHit* rawhit = trdRawHits[rh];
      if(rawhit->Straw() == leftNeighbour->GlobalStrawNumber()) rawHitInLeftNeighbour = true;
      if(rawhit->Straw() == rightNeighbour->GlobalStrawNumber()) rawHitInRightNeighbour = true;
    }

    if( rawHitInLeftNeighbour || rawHitInRightNeighbour ) continue;

    // then: add missing hit!
    TVector3 missingHitPos = 0.5*( rightNeighbour->GlobalPosition() + leftNeighbour->GlobalPosition() );
    TVector3 missingHitDir = 0.5*( rightNeighbour->GlobalWireDirection() + leftNeighbour->GlobalWireDirection() );
    double gap = segmentDir == ACsoft::AC::XZMeasurement ? rightNeighbour->GlobalPosition().X() - leftNeighbour->GlobalPosition().X() :
                                                   rightNeighbour->GlobalPosition().Y() - leftNeighbour->GlobalPosition().Y();
    gap = fabs(gap); // FIXME generalize Distance() function to the case of parallel lines and use it here
    gap -= ACsoft::AC::AMSGeometry::TRDTubeDiameter;

    ACsoft::Analysis::TrdMissingHit mhit(slNumber,missingHitPos,missingHitDir,gap,segmentDir);
    DEBUG_OUT << "Add missing hit on sl " << slNumber << ":" << std::endl;
    if(DEBUG) mhit.Dump();
    segment.fTrdMissingHits.push_back(mhit);
  }

  fStopwatchMissingHits.Stop();
  return segment.NumberOfMissingHits();
}


bool ACsoft::Analysis::TrdTracking::FitAndAddResults( ACsoft::Analysis::TrdTrack& track, const ACsoft::AC::Event& event ) {

  fStopwatchFit.Start(kFALSE);

  // FIXME find and remove outliers?

  // fit track
  fMinimizer->Clear();
  ACsoft::Analysis::TrdTrackFitFunction fitfunction(track);
  fMinimizer->SetFunction(fitfunction);

  fMinimizer->SetVariable(0,"slopeXZ", track.SegmentXZ()->SlopeTrackFinding(),0.001);
  fMinimizer->SetVariable(1,"offsetXZ",track.SegmentXZ()->OffsetTrackFinding(),1.0);
  fMinimizer->SetVariable(2,"slopeYZ", track.SegmentYZ()->SlopeTrackFinding(),0.001);
  fMinimizer->SetVariable(3,"offsetYZ",track.SegmentYZ()->OffsetTrackFinding(),1.0);

  bool minimizationSuccess = fMinimizer->Minimize();

  fStopwatchFit.Stop();

  // get the best-fit parameters and the corresponding uncertainties
  const double *parameter = fMinimizer->X();
  const double *parErrors = fMinimizer->Errors();

  fStopwatchMinos.Start(kFALSE);

  double minosErrors[4] = { 0., 0., 0., 0. };

  if( fFillMinosErrors ){
    for( int iPar = 0 ; iPar < 4 ; ++iPar ){
      double lowerError = 0.;
      double upperError = 0.;
      if( !fMinimizer->GetMinosError(iPar, lowerError, upperError) ){
        DEBUG_OUT << "Could not get MINOS error for parameter " << iPar << std::endl;
        continue;
      }
      minosErrors[iPar] = 0.5*( fabs(lowerError) + fabs(upperError) );
    }
  }

  fStopwatchMinos.Stop();

  track.fSlopeXZ = parameter[0];
  track.fSigmaSlopeXZ = parErrors[0];
  track.fOffsetXZ = parameter[1];
  track.fSigmaOffsetXZ = parErrors[1];
  track.fSlopeYZ = parameter[2];
  track.fSigmaSlopeYZ = parErrors[2];
  track.fOffsetYZ = parameter[3];
  track.fSigmaOffsetYZ = parErrors[3];
  track.fReferenceZ = track.SegmentXZ()->OffsetReferenceZ();

  track.fMinosSigmaSlopeXZ  = minosErrors[0];
  track.fMinosSigmaOffsetXZ = minosErrors[1];
  track.fMinosSigmaSlopeYZ  = minosErrors[2];
  track.fMinosSigmaOffsetYZ = minosErrors[3];

  track.fMinimizationValue = fMinimizer->MinValue();
  track.fMinimizationNumberOfCalls = fMinimizer->NCalls();

  track.fMinusLogL_XZ = fitfunction.EvalLikelihood(track.ReferencePosition(),track.DirectionVector(),ACsoft::AC::XZMeasurement);
  track.fMinusLogL_YZ = fitfunction.EvalLikelihood(track.ReferencePosition(),track.DirectionVector(),ACsoft::AC::YZMeasurement);

  track.FillHitInformation( event );

  return minimizationSuccess;
}





