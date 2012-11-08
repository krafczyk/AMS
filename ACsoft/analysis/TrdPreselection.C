#include "TrdPreselection.hh"

#include "AMSGeometry.h"
#include "Event.h"
#include "AnalysisParticle.hh"
#include "SplineTrack.hh"
#include "TrdAlignment.hh"

#include <math.h>
#include <TCutG.h>
#include <TVector3.h>
#include <Qt>

#include <algorithm>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPreselection> "
#include <debugging.hh>

Analysis::TrdPreselection::TrdPreselection() :
  fSplineTrack(0)
{
}

Analysis::TrdPreselection::~TrdPreselection() {

}


bool Analysis::TrdPreselection::Process( const Analysis::Particle& particle, float MinPathLength ) {

  // FIXME Explain return value! (is inside acceptance?)

  fSplineTrack = particle.GetSplineTrack();
  assert(fSplineTrack);

  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer)
    fCandidateHitsPerSubLayer[sublayer].clear();

  bool result = IsInsideTrdGeometricalAcceptance();

  // If you set debug > 0 we'll compare two methods: a) with lookup table (fast mode), b) without lookup table (slow mode).
  if(DEBUG){
    DEBUG_OUT << "Processing event " << particle.RawEvent()->EventHeader().Event() << " without lookup table:" << std::endl;

    ProcessWithoutLookupTable(particle, MinPathLength);
    float pathLengthInGasVolumes1 = GetCandidatePathLength();
    unsigned short candLayers1 = GetNumberOfCandidateLayers();
    unsigned short candStraws1 = GetNumberOfCandidateStraws();

    DEBUG_OUT << "Processing event " << particle.RawEvent()->EventHeader().Event() << " with lookup table:" << std::endl;

    ProcessWithLookupTable(particle, MinPathLength);
    float pathLengthInGasVolumes2 = GetCandidatePathLength();
    unsigned short candLayers2 = GetNumberOfCandidateLayers();
    unsigned short candStraws2 = GetNumberOfCandidateStraws();
    assert(fabs(pathLengthInGasVolumes1 - pathLengthInGasVolumes2) < 1e-4);
    assert(candLayers1 == candLayers2);
    assert(candStraws1 == candStraws2);
  }
  else
    ProcessWithLookupTable(particle, MinPathLength);

  return result;
}

static inline void DumpActiveStraw(unsigned short straw, unsigned short module, float pathLength, float xStraw, float yStraw, float zStraw) {

#if DEBUG > 1
  std::cout << " -> active straw " << std::setw(4) << straw
            << " (x=" << std::setw(8) << xStraw << " y=" << std::setw(8) << yStraw << " z=" << std::setw(8) << zStraw << ")"
            << " module " << std::setw(3) << module << " path length " << std::setw(8) << pathLength << std::endl;
#else
  Q_UNUSED(straw);
  Q_UNUSED(module);
  Q_UNUSED(pathLength);
  Q_UNUSED(xStraw);
  Q_UNUSED(yStraw);
  Q_UNUSED(zStraw);
#endif
}

void PushStrawToVectorIfNotPresent(unsigned short straw, std::vector<unsigned short>& straws) {

  if (std::find(straws.begin(), straws.end(), straw) != straws.end()) return;
  straws.push_back(straw);
}

void Analysis::TrdPreselection::ProcessWithLookupTable(const Analysis::Particle& particle, float MinPathLength) {

  assert(fSplineTrack);
  bool McEvent = false;
  if (particle.RawEvent()->MC().EventGenerators().size() != 0) McEvent = true;

  TVector3 trackPos, trackDir;
  float coordinateOffset = AC::AMSGeometry::TRDMaximumStrawLength / 2.0;
  AC::AMSGeometry* geometry = AC::AMSGeometry::Self();
  std::vector<unsigned short> strawNumbers;
  std::vector<TRDCandidateHit> CandidateHitsPerSubLayerOnTrack[AC::AMSGeometry::TRDSubLayers];
  std::vector<TRDCandidateHit> CandidateHitsPerSubLayerNearTrack[AC::AMSGeometry::TRDSubLayers];
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    float zLayer = geometry->TRDSubLayersZCoordinate[sublayer];
    fSplineTrack->CalculateLocalPositionAndDirection(zLayer, trackPos, trackDir);

    int xIntLower = floor(trackPos.x() + coordinateOffset);
    int xIntUpper = ceil(trackPos.x() + coordinateOffset);
    bool xIntLowerGood = xIntLower >= 0 && xIntLower < (int)AC::AMSGeometry::TRDMaximumStrawLengthAsInteger;
    bool xIntUpperGood = xIntUpper >= 0 && xIntUpper < (int)AC::AMSGeometry::TRDMaximumStrawLengthAsInteger;
    if (!xIntLowerGood && !xIntUpperGood) continue;

    int yIntLower = floor(trackPos.y() + coordinateOffset);
    int yIntUpper = ceil(trackPos.y() + coordinateOffset);
    bool yIntLowerGood = yIntLower >= 0 && yIntLower < (int)AC::AMSGeometry::TRDMaximumStrawLengthAsInteger;
    bool yIntUpperGood = yIntUpper >= 0 && yIntUpper < (int)AC::AMSGeometry::TRDMaximumStrawLengthAsInteger;
    if (!yIntLowerGood && !yIntUpperGood) continue;

    std::vector<unsigned short>* strawNumbersLL = 0;
    if (xIntLowerGood && yIntLowerGood) strawNumbersLL = &geometry->TRDStrawLookupTable[sublayer][xIntLower][yIntLower];

    std::vector<unsigned short>* strawNumbersLU = 0;
    if (xIntLowerGood && yIntUpperGood) strawNumbersLU = &geometry->TRDStrawLookupTable[sublayer][xIntLower][yIntUpper];

    std::vector<unsigned short>* strawNumbersUL = 0;
    if (xIntUpperGood && yIntLowerGood) strawNumbersUL = &geometry->TRDStrawLookupTable[sublayer][xIntUpper][yIntLower];

    std::vector<unsigned short>* strawNumbersUU = 0;
    if (xIntUpperGood && yIntUpperGood) strawNumbersUU = &geometry->TRDStrawLookupTable[sublayer][xIntUpper][yIntUpper];

    if (!strawNumbersLL && !strawNumbersLU && !strawNumbersUL && !strawNumbersUU) continue;

    strawNumbers.clear();
    for (unsigned int i = 0; i < (strawNumbersLL ? strawNumbersLL->size() : 0); ++i) PushStrawToVectorIfNotPresent(strawNumbersLL->at(i), strawNumbers);
    for (unsigned int i = 0; i < (strawNumbersLU ? strawNumbersLU->size() : 0); ++i) PushStrawToVectorIfNotPresent(strawNumbersLU->at(i), strawNumbers);
    for (unsigned int i = 0; i < (strawNumbersUL ? strawNumbersUL->size() : 0); ++i) PushStrawToVectorIfNotPresent(strawNumbersUL->at(i), strawNumbers);
    for (unsigned int i = 0; i < (strawNumbersUU ? strawNumbersUU->size() : 0); ++i) PushStrawToVectorIfNotPresent(strawNumbersUU->at(i), strawNumbers);
    if (strawNumbers.empty()) continue;

    std::vector<TRDCandidateHit> CandidateHitsOnTrack;
    std::vector<TRDCandidateHit> CandidateHitsNearTrack;
    TRDCandidateHit MinDistHit(0,0);
    double MinResidual = 1E99;
    bool found = false;
    for (unsigned short number = 0; number < strawNumbers.size(); ++number) {
      unsigned short straw = strawNumbers[number];
      unsigned short module = AC::TRDStrawToModule(straw);

      int direction;
      float strawXY, strawZ;
      AC::TRDStrawToCoordinates(straw, direction, strawXY, strawZ);
      Q_ASSERT(direction != -1);
      Q_ASSERT(fabs(strawZ - zLayer) < 1e-4);
      TVector3 candidateHitPoint = TVector3(strawXY, trackPos.y(), strawZ);
      if (direction == 1) candidateHitPoint = TVector3(trackPos.x(), strawXY, strawZ);

      if (!McEvent) {
          // Apply shimming correction.
          float Dx, Dy, Dz;
          float secondCoordinate = direction == 0 ? trackPos.y() : trackPos.x();
          AC::AMSGeometry::Self()->ApplyShimmingCorrection(straw, secondCoordinate, Dx, Dy, Dz);
          candidateHitPoint = TVector3(candidateHitPoint.X() + Dx, candidateHitPoint.Y() + Dy, candidateHitPoint.Z() + Dz);

          // Apply alignment.
          Double_t shift = Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(module, particle.RawEvent()->EventHeader().TimeStamp());
          if(direction == 0) candidateHitPoint.SetX(candidateHitPoint.X() - shift);
          else candidateHitPoint.SetY(candidateHitPoint.Y() - shift);
      }
      double residual = fSplineTrack->TrackResidual(direction, strawZ, strawXY);
      if (fabs(residual)<MinResidual) {
          MinResidual = fabs(residual);
          MinDistHit  = TRDCandidateHit(straw, 0.0);
          MinDistHit.residual = residual;
      }
      float pathLength = fSplineTrack->PathLength3D(direction, candidateHitPoint);
      if (pathLength <= AC::AMSGeometry::TRDTubeMinPathLength) continue;
      CandidateHitsOnTrack.push_back(TRDCandidateHit(straw, pathLength));
      CandidateHitsOnTrack.at(CandidateHitsOnTrack.size()-1).residual = residual;
      found = true;

      float xStraw = direction == 0 ? strawXY : trackPos.x();
      float yStraw = direction == 0 ? trackPos.y() : strawXY;
      DumpActiveStraw(straw, module, pathLength, xStraw, yStraw, strawZ);
    }

    if (!found && MinResidual<0.6) CandidateHitsNearTrack.push_back(MinDistHit);

    StoreNeighboringStraws(CandidateHitsOnTrack,strawNumbers);
    StoreNeighboringStraws(CandidateHitsNearTrack,strawNumbers);

    for (unsigned int iCan=0; iCan<CandidateHitsOnTrack.size(); iCan++) CandidateHitsPerSubLayerOnTrack[sublayer].push_back(CandidateHitsOnTrack.at(iCan));
    for (unsigned int iCan=0; iCan<CandidateHitsNearTrack.size(); iCan++) CandidateHitsPerSubLayerNearTrack[sublayer].push_back(CandidateHitsNearTrack.at(iCan));

  }

  // first store on track hits
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      for (unsigned int i=0; i<CandidateHitsPerSubLayerOnTrack[sublayer].size(); i++)
        fCandidateHitsPerSubLayer[sublayer].push_back(CandidateHitsPerSubLayerOnTrack[sublayer].at(i));
  }

  // if we have no on track hit in a layer, store the near track hits as candidate with a path length of 0
  if (MinPathLength>0) return;

  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; sublayer += 2) {
      unsigned int CandidatesPerLayerOnTrack = CandidateHitsPerSubLayerOnTrack[sublayer].size() + CandidateHitsPerSubLayerOnTrack[sublayer+1].size();
      if (CandidatesPerLayerOnTrack>0) continue;

      unsigned int CandidatesPerLayerNearTrack = CandidateHitsPerSubLayerNearTrack[sublayer].size() + CandidateHitsPerSubLayerNearTrack[sublayer+1].size();
      if (CandidatesPerLayerNearTrack==0) continue;

      // we have a candiate only in one sublayer
      if (CandidateHitsPerSubLayerNearTrack[sublayer].size()==1 && CandidateHitsPerSubLayerNearTrack[sublayer+1].size()==0) {
          fCandidateHitsPerSubLayer[sublayer].push_back(CandidateHitsPerSubLayerNearTrack[sublayer].at(0));
      } else if (CandidateHitsPerSubLayerNearTrack[sublayer].size()==0 && CandidateHitsPerSubLayerNearTrack[sublayer+1].size()==1) {
          fCandidateHitsPerSubLayer[sublayer+1].push_back(CandidateHitsPerSubLayerNearTrack[sublayer+1].at(0));

      // we have a candidate in both sublayers, so we use the one which is closest to the track
      } else if (CandidateHitsPerSubLayerNearTrack[sublayer].size()==1 && CandidateHitsPerSubLayerNearTrack[sublayer+1].size()==1) {
          if (CandidateHitsPerSubLayerNearTrack[sublayer].at(0).residual<CandidateHitsPerSubLayerNearTrack[sublayer+1].at(0).residual) {
              fCandidateHitsPerSubLayer[sublayer].push_back(CandidateHitsPerSubLayerNearTrack[sublayer].at(0));
          } else {
              fCandidateHitsPerSubLayer[sublayer+1].push_back(CandidateHitsPerSubLayerNearTrack[sublayer+1].at(0));
          }
      } else {
          WARN_OUT << "This should never happe !!! " << CandidateHitsPerSubLayerNearTrack[sublayer].size() << ", " << CandidateHitsPerSubLayerNearTrack[sublayer+1].size() << std::endl;
      }
  }

}


void Analysis::TrdPreselection::StoreNeighboringStraws(std::vector<TRDCandidateHit> &CandidateHits, std::vector<unsigned short> &strawNumbers) {

    // determine for the candidates the neighboring straws
    for (unsigned int iCan=0; iCan<CandidateHits.size(); iCan++) {
        for (unsigned short number = 0; number < strawNumbers.size(); ++number) {
            unsigned short straw = strawNumbers[number];
            // is this straw a neighbor of the candidate straw and not used by another candidate
            // (which happens if we have more than 1 hit per sublayer) ?
            if (straw != CandidateHits.at(iCan).straw) {
                bool hasBeenUsed = false;
                for (unsigned int jCan=0; jCan<CandidateHits.size(); jCan++) {
                    if (straw == CandidateHits.at(jCan).straw) {
                        hasBeenUsed = true;
                        break;
                    }
                }
                if (!hasBeenUsed) CandidateHits.at(iCan).v_NeighborStraws.push_back(straw);
            }
        }
    }
}

void Analysis::TrdPreselection::ProcessWithoutLookupTable(const Analysis::Particle& particle, float MinPathLength) {

  assert(fSplineTrack);
  bool McEvent = false; // FIXME this can be improved
  if (particle.RawEvent()->MC().EventGenerators().size() != 0) McEvent = true;

  TVector3 trackPos, trackDir;
  AC::AMSGeometry* geometry = AC::AMSGeometry::Self();

  std::vector<TRDCandidateHit> MinDistHits;
  double MinResiduals[AC::AMSGeometry::TRDSubLayers];
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      TRDCandidateHit hit(0,0);
      MinDistHits.push_back(hit);
      MinResiduals[sublayer]=1E99;
  }

  for (unsigned short straw = 0; straw < AC::AMSGeometry::TRDStraws; ++straw) {
    int direction;
    float strawXY, strawZ;
    AC::TRDStrawToCoordinates(straw, direction, strawXY, strawZ);
    Q_ASSERT(direction != -1);

    fSplineTrack->CalculateLocalPositionAndDirection(strawZ, trackPos, trackDir);

    unsigned short module = AC::TRDStrawToModule(straw);
    if (!geometry->TRDModuleGeometries[module].contour->IsInside(trackPos.x(), trackPos.y())) continue;

    TVector3 candidateHitPoint = TVector3(strawXY, trackPos.y(), strawZ);
    if (direction == 1) candidateHitPoint = TVector3(trackPos.x(), strawXY, strawZ);

    if (!McEvent) {
        // Apply shimming correction.
        float Dx, Dy, Dz;
        float secondCoordinate = direction == 0 ? trackPos.y() : trackPos.x();
        AC::AMSGeometry::Self()->ApplyShimmingCorrection(straw, secondCoordinate, Dx, Dy, Dz);
        candidateHitPoint = TVector3(candidateHitPoint.X() + Dx, candidateHitPoint.Y() + Dy, candidateHitPoint.Z() + Dz);

        // Apply alignment.
        Double_t shift = Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(module, particle.RawEvent()->EventHeader().TimeStamp());
        if(direction == 0) candidateHitPoint.SetX(candidateHitPoint.X() - shift);
        else candidateHitPoint.SetY(candidateHitPoint.Y() - shift);
    }
    double residual = fSplineTrack->TrackResidual(direction, strawZ, strawXY);
    unsigned short sublayer = geometry->TRDModuleGeometries[module].sublayer;
    if (fabs(residual)<MinResiduals[sublayer]) {
        MinResiduals[sublayer] = fabs(residual);
        MinDistHits[sublayer]  = TRDCandidateHit(straw, 0.0);
    }
    float pathLength = fSplineTrack->PathLength3D(direction, candidateHitPoint);
    if (pathLength <= AC::AMSGeometry::TRDTubeMinPathLength) continue;

    fCandidateHitsPerSubLayer[sublayer].push_back(TRDCandidateHit(straw, pathLength));

    float xStraw = direction == 0 ? strawXY : trackPos.x();
    float yStraw = direction == 0 ? trackPos.y() : strawXY;
    DumpActiveStraw(straw, module, pathLength, xStraw, yStraw, strawZ);
  }

  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      if (fCandidateHitsPerSubLayer[sublayer].size()>0) continue;
      if (MinPathLength<=0 && MinResiduals[sublayer]<0.6) fCandidateHitsPerSubLayer[sublayer].push_back(MinDistHits[sublayer]);
  }

  //FIXME: No idea how to implement the concept of neighboring straws here

}

bool Analysis::TrdPreselection::IsInsideTrdGeometricalAcceptance() const {
        
  TVector2 coordinatesFirst = Analysis::TrdPreselection::PointInLayer(0);
  TVector2 coordinatesLast = Analysis::TrdPreselection::PointInLayer(AC::AMSGeometry::TRDLayers - 1);
  
  AC::AMSGeometry* geometry = AC::AMSGeometry::Self();
  if (!geometry->TRDFirstLayerContour->IsInside(coordinatesFirst.X(), coordinatesFirst.Y()))
    return false;
  if (!geometry->TRDLastLayerContour->IsInside(coordinatesLast.X(), coordinatesLast.Y()))
    return false;
  return true;
}

float Analysis::TrdPreselection::GetCandidatePathLength() const {

  float pathLengthSum = 0; 
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    for (unsigned short i = 0; i < fCandidateHitsPerSubLayer[sublayer].size(); ++i)
      pathLengthSum += fCandidateHitsPerSubLayer[sublayer][i].pathLength;
  }
  return pathLengthSum;
}

unsigned short Analysis::TrdPreselection::GetNumberOfCandidateLayers() const {

  bool SubLayers[AC::AMSGeometry::TRDSubLayers];
  for (unsigned short sublayer=0; sublayer<AC::AMSGeometry::TRDSubLayers; ++sublayer) SubLayers[sublayer]=false;
  for (unsigned short sublayer=0; sublayer<AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      for (unsigned short iHit=0; iHit<fCandidateHitsPerSubLayer[sublayer].size(); iHit++) SubLayers[sublayer]=true;
  }

  unsigned short candidateLayers = 0;
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; sublayer += 2)
    candidateLayers += (SubLayers[sublayer] || SubLayers[sublayer + 1]);
  return candidateLayers;
}

unsigned short Analysis::TrdPreselection::GetNumberOfCandidateStraws() const {

  unsigned short candidateStraws = 0;
  for (unsigned short sublayer=0; sublayer<AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      for (unsigned short iHit=0; iHit<fCandidateHitsPerSubLayer[sublayer].size(); iHit++) candidateStraws++;
  }
  return candidateStraws;
}

TVector2 Analysis::TrdPreselection::PointInLayer(unsigned short layer) const {

  assert(fSplineTrack);
  assert(layer < AC::AMSGeometry::TRDLayers);

  AC::AMSGeometry* geometry = AC::AMSGeometry::Self();
  float zUpperLayer = geometry->TRDSubLayersZCoordinate[layer * 2];
  float zLowerLayer = geometry->TRDSubLayersZCoordinate[layer * 2 + 1];
  Q_ASSERT(zUpperLayer > zLowerLayer);
 
  float zLayer = (zLowerLayer + zUpperLayer) / 2.0;
  TVector3 vector = fSplineTrack->InterpolateToZ(zLayer);
  return TVector2(vector.x(), vector.y());
}

std::vector<Analysis::TRDCandidateHit> Analysis::TrdPreselection::CandidateHits() const {

  std::vector<TRDCandidateHit> result;
  for (unsigned short sublayer = 0; sublayer < AC::AMSGeometry::TRDSubLayers; ++sublayer) {
      for (unsigned short iHit=0; iHit<fCandidateHitsPerSubLayer[sublayer].size(); iHit++)
          result.push_back(fCandidateHitsPerSubLayer[sublayer].at(iHit));
  }
  return result;
}

bool Analysis::TrdPreselection::PassesPreselectionCuts() const {
  if (GetNumberOfCandidateLayers() <= 15  ) return false;
  if (GetCandidatePathLength()     <=  7.0) return false;
  return true;
}

