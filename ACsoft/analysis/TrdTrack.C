#include "TrdTrack.hh"

#include "Event.h"
#include "TRDRawHit.h"
#include "TrdSegment.hh"
#include "AMSGeometry.h"
#include "TrdDetector.hh"
#include "TrdSublayer.hh"
#include "TrdStraw.hh"
#include "DetectorManager.hh"
#include "pathlength_functions.hh"

#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdTrack> "
#include <debugging.hh>

ACsoft::Analysis::TrdTrack::TrdTrack( TrdSegment* xzSegment, TrdSegment* yzSegment ) :
  ACsoft::Analysis::StraightLineTrack(0.,0.,0.,0.,0.),
  fSegmentXZ(xzSegment),
  fSegmentYZ(yzSegment)
{
  assert(fSegmentXZ && fSegmentXZ->Direction() == ACsoft::AC::XZMeasurement );
  assert(fSegmentYZ && fSegmentYZ->Direction() == ACsoft::AC::YZMeasurement );

  fSigmaSlopeXZ = 0.; fSigmaOffsetXZ = 0.;  fSigmaSlopeYZ = 0.; fSigmaOffsetYZ = 0.;

  fMinimizationValue = -1.0;
  fMinimizationNumberOfCalls = 0;

  fMinusLogL_XZ = -0.5;
  fMinusLogL_YZ = -0.5;

  fActualHitsOnTrackXZ.reserve(25);
  fActualHitsOnTrackYZ.reserve(25);

  fNumberOfHitsWithinOneTubeDiameter = -1;
  fNumberOfHitsWithinTwoTubeDiameters = -1;
  fNumberOfHitsNearTrackOnMissingSublayers = -1;
}

ACsoft::Analysis::TrdTrack::~TrdTrack() {

  // neither segments nor raw hits are deep copies, so do nothing here
}


/** Returns a vector showing the number of actual hits on track for a given layer number. */
std::vector<int> ACsoft::Analysis::TrdTrack::GetHitPattern() const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  std::vector<int> hitPattern(ACsoft::AC::AMSGeometry::TRDLayers,0);

  for( unsigned int i=0 ; i<fActualHitsOnTrackXZ.size() ; ++i ){
    int layerNumber = trd->GetTrdStraw(fActualHitsOnTrackXZ[i]->Straw())->LayerNumber();
    ++hitPattern.at(layerNumber);
  }
  for( unsigned int i=0 ; i<fActualHitsOnTrackYZ.size() ; ++i ){
    int layerNumber = trd->GetTrdStraw(fActualHitsOnTrackYZ[i]->Straw())->LayerNumber();
    ++hitPattern.at(layerNumber);
  }

  return hitPattern;
}

/** Count the number of sublayers that have at least one on-track hit. */
unsigned int ACsoft::Analysis::TrdTrack::NumberOfSublayersWithHit() const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  std::vector<int> hitPattern(ACsoft::AC::AMSGeometry::TRDSubLayers,0);

  for( unsigned int i=0 ; i<fActualHitsOnTrackXZ.size() ; ++i ){
    int sublayerNumber = trd->GetTrdStraw(fActualHitsOnTrackXZ[i]->Straw())->GlobalSublayerNumber();
    hitPattern.at(sublayerNumber) = 1;
  }
  for( unsigned int i=0 ; i<fActualHitsOnTrackYZ.size() ; ++i ){
    int sublayerNumber = trd->GetTrdStraw(fActualHitsOnTrackYZ[i]->Straw())->GlobalSublayerNumber();
    hitPattern.at(sublayerNumber) = 1;
  }

  int count = 0;
  for( unsigned int i=0 ; i<hitPattern.size() ; ++i )
    count += hitPattern[i];

  return count;
}


/** Find the actual hits on track and count hits near track.
  *
  * While hits are added generously to segments, in this function only hits
  * that the best-fit line parameters indicate a non-zero pathlength for are counted as lying on the track.
  *
  * The \c fActualHitsOnTrackXZ and \c fActualHitsOnTrackYZ vectors are filled by this function.
  *
  * \param[in] event Event needed for access to %TRD raw hits
  *
  * \returns number of hits on track found
  */
unsigned int ACsoft::Analysis::TrdTrack::FillHitInformation( const ACsoft::AC::Event& event ) {

  const ACsoft::AC::TRD::RawHitsVector& trdRawHits = event.TRD().RawHits();

  //
  // fill vectors containing the actual hits on track
  //
  fActualHitsOnTrackXZ.clear();
  fActualHitsOnTrackYZ.clear();

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  unsigned int nHits = 0;

  for( unsigned int i=0 ; i<fSegmentXZ->NumberOfRawHits() ; ++i ) {

    const ACsoft::AC::TRDRawHit* rawHit = fSegmentXZ->RawHit(i);
    double pathlength = Pathlength3d(ACsoft::AC::AMSGeometry::TRDTubeRadius,
                                     trd->HitPosition(rawHit),trd->HitDirection(rawHit),
                                     ReferencePosition(),DirectionVector());
    if( pathlength > 0.0 ){
      fActualHitsOnTrackXZ.push_back(rawHit);
      ++nHits;
    }
  }

  for( unsigned int i=0 ; i<fSegmentYZ->NumberOfRawHits() ; ++i ) {

    const ACsoft::AC::TRDRawHit* rawHit = fSegmentYZ->RawHit(i);
    double pathlength = Pathlength3d(ACsoft::AC::AMSGeometry::TRDTubeRadius,
                                     trd->HitPosition(rawHit),trd->HitDirection(rawHit),
                                     ReferencePosition(),DirectionVector());
    if( pathlength > 0.0 ){
      fActualHitsOnTrackYZ.push_back(rawHit);
      ++nHits;
    }
  }

  //
  // count number of hits that are not on track but close to the track
  //
  int nHitsWithinOneTubeDiameter  = 0;
  int nHitsWithinTwoTubeDiameters = 0;
  int nHitsNearTrackOnMissingSublayers = 0;

  for( unsigned int i=0 ; i<trdRawHits.size() ; ++i ) {

    const ACsoft::AC::TRDRawHit& rawhit = trdRawHits[i];
    if( HasActualHitInStraw(rawhit.Straw()) ) continue;

    const ACsoft::Detector::TrdStraw* straw = trd->GetTrdStraw(rawhit.Straw());
    double distanceToStraw = DistanceToLine(straw->GlobalPosition(),straw->GlobalWireDirection());
    if( distanceToStraw <= ACsoft::AC::AMSGeometry::TRDTubeDiameter )
      ++nHitsWithinOneTubeDiameter;
    if( distanceToStraw <= 2.0*ACsoft::AC::AMSGeometry::TRDTubeDiameter )
      ++nHitsWithinTwoTubeDiameters;

    int rawHitSublayerNumber = straw->GlobalSublayerNumber();
    if( HasActualHitInSublayer(rawHitSublayerNumber) ) continue;
    if( distanceToStraw <= 1.5*ACsoft::AC::AMSGeometry::TRDTubeDiameter )
      ++nHitsNearTrackOnMissingSublayers;
  }

  // fill track
  fNumberOfHitsWithinOneTubeDiameter = nHitsWithinOneTubeDiameter;
  fNumberOfHitsWithinTwoTubeDiameters = nHitsWithinTwoTubeDiameters;
  fNumberOfHitsNearTrackOnMissingSublayers = nHitsNearTrackOnMissingSublayers;

  return nHits;
}


/** Check if the straw with the given global straw number contains a hit that belongs to this track. */
bool ACsoft::Analysis::TrdTrack::HasActualHitInStraw( Short_t globalStrawNumber ) const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  ACsoft::AC::MeasurementMode dir = trd->GetTrdStraw(globalStrawNumber)->Direction();

  if( dir == ACsoft::AC::XZMeasurement ){
    for( unsigned int i=0 ; i<fActualHitsOnTrackXZ.size() ; ++i ){
      if(fActualHitsOnTrackXZ[i]->Straw() == globalStrawNumber ) return true;
    }
  }
  else{
    for( unsigned int i=0 ; i<fActualHitsOnTrackYZ.size() ; ++i ){
      if(fActualHitsOnTrackYZ[i]->Straw() == globalStrawNumber ) return true;
    }
  }

  return false;
}

/** Check if the sublayer with the given global sublayer number contains a hit that belongs to this track. */
bool ACsoft::Analysis::TrdTrack::HasActualHitInSublayer( Short_t globalSublayerNumber ) const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  ACsoft::AC::MeasurementMode dir = trd->GetTrdSublayer(globalSublayerNumber)->Direction();

  if( dir == ACsoft::AC::XZMeasurement ){
    for( unsigned int i=0 ; i<fActualHitsOnTrackXZ.size() ; ++i ){
      if(trd->GetTrdStraw(fActualHitsOnTrackXZ[i]->Straw())->GlobalSublayerNumber() == globalSublayerNumber ) return true;
    }
  }
  else{
    for( unsigned int i=0 ; i<fActualHitsOnTrackYZ.size() ; ++i ){
      if(trd->GetTrdStraw(fActualHitsOnTrackYZ[i]->Straw())->GlobalSublayerNumber() == globalSublayerNumber ) return true;
    }
  }

  return false;
}


/** Count the number of layers that have at least one on-track hit. */
unsigned int ACsoft::Analysis::TrdTrack::NumberOfLayersWithHit() const {

  std::vector<int> hitPattern = GetHitPattern();
  int nLayersWithHit = 0;
  for( unsigned int i=0 ; i<hitPattern.size() ; ++i )
    if( hitPattern[i] ) ++nLayersWithHit;

  return nLayersWithHit;
}

/** Find the layer number of the first hit (from the top of the %TRD on track). */
int ACsoft::Analysis::TrdTrack::FirstLayerNumberFromTop() const {

  std::vector<int> hitPattern = GetHitPattern();
  int currentLayerNumber = int(hitPattern.size()) - 1;
  for( std::vector<int>::reverse_iterator rit = hitPattern.rbegin() ; rit != hitPattern.rend() ; ++rit ) {
    if(*rit) return currentLayerNumber;
    else --currentLayerNumber;
  }

  // empty track: should not happen!
  WARN_OUT << "Empty track!" << std::endl;
  return -1;
}

/** Find the layer number of the last hit (from the bottom of the %TRD on track). */
int ACsoft::Analysis::TrdTrack::LastLayerNumberFromBottom() const {

  std::vector<int> hitPattern = GetHitPattern();
  for( unsigned int i=0 ; i<hitPattern.size() ; ++i )
    if( hitPattern[i] ) return int(i);

  // empty track: should not happen!
  WARN_OUT << "Empty track!" << std::endl;
  return -1;
}

/** Dump to console. */
void ACsoft::Analysis::TrdTrack::Dump() const {

  std::cout << "  [TrdTrack]" << std::endl;
  std::cout << "  XZ segment " << fSegmentXZ << ": "; fSegmentXZ->Dump();
  std::cout << "  YZ segment " << fSegmentYZ << ": "; fSegmentYZ->Dump();
  std::cout << "  Slope XZ: " << fSlopeXZ << "+-" << fSigmaSlopeXZ << " (minos: " << fMinosSigmaSlopeXZ << ")"
            << "  Slope XY: " << fSlopeYZ << "+-" << fSigmaSlopeYZ << " (minos: " << fMinosSigmaSlopeYZ << ")"<< std::endl;
  std::cout << "  Offset XZ: " << fOffsetXZ << "+-" << fSigmaOffsetXZ << " (minos: " << fMinosSigmaOffsetXZ << ")"
            << "  Offset YZ: " << fOffsetYZ << "+-" << fSigmaOffsetYZ << " (minos: " << fMinosSigmaOffsetYZ << ")" << "  zref=" << fReferenceZ << std::endl;
  std::cout << "  Minimization: -log(L)= " << fMinimizationValue << " (XZ: " << fMinusLogL_XZ << ", YZ: " << fMinusLogL_YZ << ")"
            << " nCalls= " << fMinimizationNumberOfCalls << std::endl;
  std::cout << "  Theta: " << Theta()*TMath::RadToDeg() << " deg, Phi: " << Phi()*TMath::RadToDeg() << " deg" << std::endl;
  std::cout << "  nHitsOnTrack: " << NumberOfHitsOnTrack() << ", nLayersWithHit: " << NumberOfLayersWithHit() << ", nSublayersWithHit: " << NumberOfSublayersWithHit()
            << ", first layer: " << FirstLayerNumberFromTop() << ", last layer: " << LastLayerNumberFromBottom() << std::endl;
  std::cout << "  off-track: " << fNumberOfHitsWithinOneTubeDiameter << " within tube-D, " << fNumberOfHitsWithinTwoTubeDiameters << " within two tube-D, "
            << fNumberOfHitsNearTrackOnMissingSublayers << " near track on missing sublayers" << std::endl;

}

/** Dump detailed information about all raw hits in segments and all hits on track. */
void ACsoft::Analysis::TrdTrack::DumpIndividualHits() const {

  std::cout << "[TrdTrack hits in XZ segment]" << std::endl;
  for( unsigned int i=0 ; i<fSegmentXZ->NumberOfRawHits() ; ++i )
    fSegmentXZ->RawHit(i)->Dump();
  std::cout << "[TrdTrack hits in YZ segment]" << std::endl;
  for( unsigned int i=0 ; i<fSegmentYZ->NumberOfRawHits() ; ++i )
    fSegmentYZ->RawHit(i)->Dump();

  std::cout << "[TrdTrack actual hits on track in XZ direction]" << std::endl;
  for( unsigned int i=0 ; i<fActualHitsOnTrackXZ.size() ; ++i )
    fActualHitsOnTrackXZ[i]->Dump();
  std::cout << "[TrdTrack actual hits on track in YZ direction]" << std::endl;
  for( unsigned int i=0 ; i<fActualHitsOnTrackYZ.size() ; ++i )
    fActualHitsOnTrackYZ[i]->Dump();

}



