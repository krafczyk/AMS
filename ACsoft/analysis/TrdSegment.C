#include "TrdSegment.hh"

#include "TrdDetector.hh"
#include "DetectorManager.hh"
#include "TrdStraw.hh"

#include <TGraph.h>

#include <numeric>

ACsoft::Analysis::TrdSegment::TrdSegment(ACsoft::AC::MeasurementMode direction) :
  fDirection(direction),
  fDrawGraphTrackFinding(0)
{
  fSlopeTrackFinding = 0.;
  fOffsetTrackFinding = 0.;
  fOffsetReferenceZ = 0.;
  fNumberOfPairsCloseToMaximum = 0;

  fTrdRawHits.reserve(20);
  fTrdMissingHits.reserve(20);
}


/** Test for overlap with another segment (at least two identical raw hits on more than two different sublayers in both segments). */
bool ACsoft::Analysis::TrdSegment::OverlapsWith( const TrdSegment& other ) const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  int nIdenticalHits = 0;
  std::vector<int> sublayersInvolved(ACsoft::AC::AMSGeometry::TRDSubLayers,0);

  for( unsigned int i=0 ; i<fTrdRawHits.size() ; ++i ){

    const ACsoft::AC::TRDRawHit* hit = fTrdRawHits[i];

    for( unsigned int j=0 ; j<other.fTrdRawHits.size() ; ++j ){

      const ACsoft::AC::TRDRawHit* hit2 = other.fTrdRawHits[j];
      if( hit == hit2 ){
        ++nIdenticalHits;

        const ACsoft::Detector::TrdStraw* straw = trd->GetTrdStraw(hit->Straw());
        int hitSublayer = straw->GlobalSublayerNumber();
        sublayersInvolved.at(hitSublayer) = 1;

        int nSublayersInvoled = std::accumulate(sublayersInvolved.begin(),sublayersInvolved.end(),0);

        if( ( nIdenticalHits > 1 ) && ( nSublayersInvoled >= 3 ) ) return true;
      }
    }
  }

  return false;
}

/** Count the number of sublayers that have a hit candidate in the segment. */
int ACsoft::Analysis::TrdSegment::NumberOfSublayersInSegment() const {

  std::vector<int> pattern(ACsoft::AC::AMSGeometry::TRDSubLayers,0);

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  for( unsigned int i=0 ; i<fTrdRawHits.size() ; ++i ){
    int sl = trd->GetTrdStraw(fTrdRawHits[i]->Straw())->GlobalSublayerNumber();
    pattern.at(sl) = 1;
  }

  int nSublayers = 0;
  for( unsigned int j=0 ; j<pattern.size() ; ++j ){
    nSublayers += pattern[j];
  }

  return nSublayers;
}

/** Check if there is a raw hit in the segment on the given sublayer (0..39). */
bool ACsoft::Analysis::TrdSegment::HasRawHitOnSublayer( int globalSublayerNumber ) const {

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  for( unsigned int i=0 ; i<fTrdRawHits.size() ; ++i ){
    int sl = trd->GetTrdStraw(fTrdRawHits[i]->Straw())->GlobalSublayerNumber();
    if( sl == globalSublayerNumber ) return true;
  }

  return false;
}


/** Find the sublayer with the lowest global sublayer number that has a raw hit in this segment. */
int ACsoft::Analysis::TrdSegment::LastSublayerInSegment() const {

  int lastSublayer = ACsoft::AC::AMSGeometry::TRDSubLayers;

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  for( unsigned int i=0 ; i<fTrdRawHits.size() ; ++i ){
    int sl = trd->GetTrdStraw(fTrdRawHits[i]->Straw())->GlobalSublayerNumber();
    if( sl < lastSublayer ) lastSublayer = sl;
  }

  return lastSublayer;
}

/** Find the sublayer with the highest global sublayer number that has a raw hit in this segment. */
int ACsoft::Analysis::TrdSegment::FirstSublayerInSegment() const {

  int firstSublayer = -1;

  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  for( unsigned int i=0 ; i<fTrdRawHits.size() ; ++i ){
    int sl = trd->GetTrdStraw(fTrdRawHits[i]->Straw())->GlobalSublayerNumber();
    if( sl > firstSublayer ) firstSublayer = sl;
  }

  return firstSublayer;
}

/** Dump to console. */
void ACsoft::Analysis::TrdSegment::Dump() const {

  std::cout << "TrdSegment dir " << fDirection << ", track finding: slope " << fSlopeTrackFinding
            << " offset " << fOffsetTrackFinding << " nPairs " << fNumberOfPairsCloseToMaximum
            << " nHits " << NumberOfRawHits()
            << " nMissingHits " << NumberOfMissingHits()
            << " nSublayers " << NumberOfSublayersInSegment()
            << std::endl;
//  if( fTrdMissingHits.size() ){
//    std::cout << "  Missing hits:" << std::endl;
//    for( unsigned int i=0 ; i<fTrdMissingHits.size() ; ++i )
//      fTrdMissingHits[i].Dump();
//  }
}

/** Draw a line based on the line parameters identified in the track finding procedure to the current canvas. */
void ACsoft::Analysis::TrdSegment::Draw( float zmin, float zmax, bool rotatedSystem, Color_t lineColour, int nPoints ) const {

  if( fDrawGraphTrackFinding ) delete fDrawGraphTrackFinding;
  fDrawGraphTrackFinding = new TGraph(nPoints);

  for( int i=0 ; i<nPoints ; ++i ){
    float z    = zmin + i*(zmax-zmin)/(nPoints-1);
    float xy   = fOffsetTrackFinding + (z-fOffsetReferenceZ)*fSlopeTrackFinding;
    float xPad = rotatedSystem ? -z  : xy;
    float yPad = rotatedSystem ?  xy : z;
    fDrawGraphTrackFinding->SetPoint(i,xPad,yPad);
  }

  fDrawGraphTrackFinding->SetLineColor(lineColour);
  fDrawGraphTrackFinding->SetLineStyle(2);
  fDrawGraphTrackFinding->Draw("L");

  for( unsigned int i=0 ; i<fTrdMissingHits.size() ; ++i )
    fTrdMissingHits[i].Draw();
}

