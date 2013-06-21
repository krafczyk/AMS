#include "TrdTrackFitFunction.hh"

#include "TrdTrack.hh"
#include "TrdSegment.hh"
#include "TRDRawHit.h"
#include "DetectorManager.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "pathlength_functions.hh"

#include <iomanip>

#define DEBUG 0
#include <debugging.hh>

TF1* ACsoft::Analysis::TrdTrackFitFunction::fTrackDistanceLikelihoodFunction = 0;

ACsoft::Analysis::TrdTrackFitFunction::TrdTrackFitFunction( const ACsoft::Analysis::TrdTrack& track ) {

  if(!fTrackDistanceLikelihoodFunction) {

    fTrackDistanceLikelihoodFunction = new TF1("trackDistanceLikelihoodFunction",
                                               "0.5*[0]*(TMath::Erf((x+0.5*[1])/[2])-TMath::Erf((x-0.5*[1])/[2]))",
                                               -10., 10. );
    fTrackDistanceLikelihoodFunction->SetParName(0,"plateau");
    fTrackDistanceLikelihoodFunction->SetParName(1,"width");
    fTrackDistanceLikelihoodFunction->SetParName(2,"sharpness");
    // sharpness from alignment fits, deteriorates towards top of TRD due to msc
    fTrackDistanceLikelihoodFunction->SetParameters(1.0,ACsoft::AC::AMSGeometry::TRDTubeDiameter,0.08);
  }

  fGlobalSublayerNumbersXZ.reserve(40);
  fHitPositionsXZ.reserve(40);
  fHitDirectionsXZ.reserve(40);
  fMissingHitPositionsXZ.reserve(20);
  fMissingHitDirectionsXZ.reserve(20);
  fMissingHitGapsXZ.reserve(20);

  fGlobalSublayerNumbersYZ.reserve(40);
  fHitPositionsYZ.reserve(40);
  fHitDirectionsYZ.reserve(40);
  fMissingHitPositionsYZ.reserve(20);
  fMissingHitDirectionsYZ.reserve(20);
  fMissingHitGapsYZ.reserve(20);

  const ACsoft::Analysis::TrdSegment* xzSegment = track.SegmentXZ();
  const ACsoft::Analysis::TrdSegment* yzSegment = track.SegmentYZ();

  StorePositionsAndDirections(xzSegment);
  StorePositionsAndDirections(yzSegment);

  fOffsetReferenceZ = xzSegment->OffsetReferenceZ();

}

/** Store positions and directions of the straws belonging to the raw hits in the given segment, for fast lookup during fit. */
void ACsoft::Analysis::TrdTrackFitFunction::StorePositionsAndDirections( const ACsoft::Analysis::TrdSegment* segment ) {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  ACsoft::AC::MeasurementMode dir = segment->Direction();

  for( unsigned int i=0 ; i<segment->NumberOfRawHits() ; ++i ){

    const ACsoft::AC::TRDRawHit* hit = segment->RawHit(i);
    Short_t strawNumber = hit->Straw();
    const ACsoft::Detector::TrdStraw* straw = trd->GetTrdStraw(strawNumber);
    if(dir == ACsoft::AC::XZMeasurement){
      fGlobalSublayerNumbersXZ.push_back(straw->GlobalSublayerNumber());
      fHitPositionsXZ.push_back(straw->GlobalPosition());
      fHitDirectionsXZ.push_back(straw->GlobalWireDirection());
    }
    else{
      fGlobalSublayerNumbersYZ.push_back(straw->GlobalSublayerNumber());
      fHitPositionsYZ.push_back(straw->GlobalPosition());
      fHitDirectionsYZ.push_back(straw->GlobalWireDirection());
    }
  }

  for( unsigned int i=0 ; i<segment->NumberOfMissingHits() ; ++i ) {
    const ACsoft::Analysis::TrdMissingHit& mhit = segment->MissingHit(i);
    if(dir == ACsoft::AC::XZMeasurement){
      fMissingHitPositionsXZ.push_back(mhit.Position());
      fMissingHitDirectionsXZ.push_back(mhit.Direction());
      fMissingHitGapsXZ.push_back(mhit.GapWidth());
    }
    else{
      fMissingHitPositionsYZ.push_back(mhit.Position());
      fMissingHitDirectionsYZ.push_back(mhit.Direction());
      fMissingHitGapsYZ.push_back(mhit.GapWidth());
    }
  }
}

/** The function called by Minuit during the fit.
 *
 * parameters:
 * - 0 slope XZ
 * - 1 offset XZ (at reference Z)
 * - 2 slope YZ
 * - 3 offset YZ (at reference Z)
 *
 */
double ACsoft::Analysis::TrdTrackFitFunction::DoEval( const double* parameters ) const {

  TVector3 trackPosition(parameters[1],parameters[3],fOffsetReferenceZ);
  TVector3 trackDirection(parameters[0],parameters[2],1.0);

  return EvalLikelihood(trackPosition,trackDirection);
}


/** Calculate -log(L). */
double ACsoft::Analysis::TrdTrackFitFunction::EvalLikelihood( const TVector3& trackPosition, const TVector3& trackDirection ) const {

  double minusLogL_XZ = EvalLikelihood(trackPosition,trackDirection,ACsoft::AC::XZMeasurement);
  double minusLogL_YZ = EvalLikelihood(trackPosition,trackDirection,ACsoft::AC::YZMeasurement);

  return minusLogL_XZ + minusLogL_YZ;
}


/** Calculate the contribution to -log(L) from straws and missing hits of direction \p dir. */
double ACsoft::Analysis::TrdTrackFitFunction::EvalLikelihood( const TVector3& trackPosition, const TVector3& trackDirection, const ACsoft::AC::MeasurementMode dir ) const {

  const std::vector<TVector3>& hitPositions = ( dir==ACsoft::AC::XZMeasurement ? fHitPositionsXZ : fHitPositionsYZ );
  const std::vector<TVector3>& hitDirections = ( dir==ACsoft::AC::XZMeasurement ? fHitDirectionsXZ : fHitDirectionsYZ );
  const std::vector<TVector3>& missingHitPositions = ( dir==ACsoft::AC::XZMeasurement ? fMissingHitPositionsXZ : fMissingHitPositionsYZ );
  const std::vector<TVector3>& missingHitDirections = ( dir==ACsoft::AC::XZMeasurement ? fMissingHitDirectionsXZ : fMissingHitDirectionsYZ );
  const std::vector<int>&      globalSublayerNumbers = ( dir==ACsoft::AC::XZMeasurement ? fGlobalSublayerNumbersXZ : fGlobalSublayerNumbersYZ );
  const std::vector<double>&   missingHitGaps = ( dir==ACsoft::AC::XZMeasurement ? fMissingHitGapsXZ : fMissingHitGapsYZ );

  // handle the case of more than one hit on the same sublayer: choose the smallest contribution for each
  std::vector<double> minusLogL_fromSublayer(ACsoft::AC::AMSGeometry::TRDSubLayers,0.0);
  std::vector<int> hitsForGlobalSublayer(ACsoft::AC::AMSGeometry::TRDSubLayers,0);


  for( unsigned int i=0 ; i<hitPositions.size() ; ++i ) {

    double hitDistanceToTrack = Distance(trackPosition,trackDirection,hitPositions[i],hitDirections[i]);
    int globalSublayerOfHit   = globalSublayerNumbers[i];

    double term = -log(fTrackDistanceLikelihoodFunction->Eval(hitDistanceToTrack));
    DEBUG_OUT << "entry " << std::setw(3) << i << ": " << term << " (hits on sl so far: " << hitsForGlobalSublayer[globalSublayerOfHit] << ")" << std::endl;

    // store -log(L) for first hit on sublayer in any case
    if( hitsForGlobalSublayer.at(globalSublayerOfHit) == 0 ){
      minusLogL_fromSublayer[globalSublayerOfHit] = term;
    }
    // for additional hits: replace -log(L) contribution if smaller than existing one
    else{
      if( term < minusLogL_fromSublayer[globalSublayerOfHit] ) {
        minusLogL_fromSublayer[globalSublayerOfHit] = term;
      }
    }

    ++hitsForGlobalSublayer[globalSublayerOfHit];
  }

  double minusLogL = 0.0;

  for( unsigned int i=0 ; i<minusLogL_fromSublayer.size() ; ++i ) {
    DEBUG_OUT_L(2) << "contrib from sl " << std::setw(3) << i << ": " << minusLogL_fromSublayer[i] << std::endl;
    minusLogL += minusLogL_fromSublayer[i];
  }

  // contributions from missing hits
  for( unsigned int i=0 ; i<missingHitPositions.size() ; ++i ) {

    double mhitDistanceToTrack = Distance(trackPosition,trackDirection,missingHitPositions[i],missingHitDirections[i]);
//    // use the same likelihood function, but effectively reduce width parameter
//    double term = -log(fTrackDistanceLikelihoodFunction->Eval(mhitDistanceToTrack/fMissingHitGaps[i] * ACsoft::AC::AMSGeometry::TRDTubeDiameter));
    // simple chi2 term for missing hits
    static const double sqrt12 = sqrt(12.0);
    double pull = mhitDistanceToTrack/missingHitGaps[i]/sqrt12;
    double term = 0.5*pull*pull;
    DEBUG_OUT << "mhit entry " << std::setw(3) << i << ": " << term << " distance " << mhitDistanceToTrack << std::endl;
    minusLogL += term;
  }

  return minusLogL;
}

