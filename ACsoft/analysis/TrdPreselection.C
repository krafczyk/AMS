#include "TrdPreselection.hh"

#include "AMSGeometry.h"
#include "Event.h"
#include "AnalysisParticle.hh"
#include "SplineTrack.hh"
#include "TrdAlignment.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "TrdModule.hh"
#include "TrdLayer.hh"
#include "TrdSublayer.hh"
#include "DeadStrawLookup.hh"
#include "DetectorManager.hh"
#include "pathlength_functions.hh"

#include <math.h>
#include <TCutG.h>
#include <TVector3.h>
#include <Qt>

#include <algorithm>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPreselection> "
#include <debugging.hh>

ACsoft::Analysis::TrdPreselection::TrdPreselection() :
  fSplineTrack(0)
{
    strawNumbers.reserve(20);
    for (unsigned int i = 0; i < ACsoft::AC::AMSGeometry::TRDSubLayers; i++) {
        CandidateHitsPerSubLayerOnTrack[i].reserve(5);
        CandidateHitsPerSubLayerNearTrack[i].reserve(5);
    }
    CandidateHitsOnTrack.reserve(40);
    CandidateHitsNearTrack.reserve(200);
}

ACsoft::Analysis::TrdPreselection::~TrdPreselection() {

}


bool ACsoft::Analysis::TrdPreselection::Process( const ACsoft::Analysis::Particle& particle, bool AddNearTrackHitsToCandidateList, bool ExcludeDeadStraws ) {

  fSplineTrack = particle.GetSplineTrack();

  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer)
    fCandidateHitsPerSubLayer[sublayer].clear();

  bool result = IsInsideTrdGeometricalAcceptance();
  if (fSplineTrack)
    ProcessWithLookupTable(particle,AddNearTrackHitsToCandidateList, ExcludeDeadStraws);
  return result;
}

static inline void DumpActiveStraw(unsigned short straw, unsigned short module, float pathLength, float xStraw, float yStraw, float zStraw) {

  DEBUG_OUT_L(2) << " -> active straw " << std::setw(4) << straw
                 << " (x=" << std::setw(8) << xStraw << " y=" << std::setw(8) << yStraw << " z=" << std::setw(8) << zStraw << ")"
                 << " module " << std::setw(3) << module << " path length " << std::setw(8) << pathLength << std::endl;
}

void PushStrawToVectorIfNotPresent(unsigned short straw, std::vector<unsigned short>& straws) {

  if (std::find(straws.begin(), straws.end(), straw) != straws.end()) return;
  straws.push_back(straw);
}

void ACsoft::Analysis::TrdPreselection::ProcessWithLookupTable(const ACsoft::Analysis::Particle& particle, bool AddNearTrackHitsToCandidateList, bool ExcludeDeadStraws) {

  // FIXME give this function a more meaningful name

  assert(fSplineTrack);

  TVector3 trackPos, trackDir;
  DEBUG_OUT << "Getting aligned TRD" << std::endl;
  ACsoft::Detector::Trd* alignedTrd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  CandidateHitsOnTrack.clear();
  CandidateHitsNearTrack.clear();
  for (unsigned int i = 0; i<ACsoft::AC::AMSGeometry::TRDSubLayers; i++) {
      CandidateHitsPerSubLayerOnTrack[i].clear();
      CandidateHitsPerSubLayerNearTrack[i].clear();
  }
  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    float zSublayer = alignedTrd->GetTrdSublayer(sublayer)->GlobalPosition().Z();
    fSplineTrack->CalculateLocalPositionAndDirection(zSublayer, trackPos, trackDir);

    bool ok = ACsoft::AC::AMSGeometry::Self()->CollectStrawCandidates(sublayer,trackPos.X(),trackPos.Y(),strawNumbers);
    if(!ok || strawNumbers.empty()) continue;

    CandidateHitsOnTrack.clear();
    CandidateHitsNearTrack.clear();
    TRDCandidateHit MinDistHit(0,0);
    double MinResidual = 1E99;
    bool found = false;
    for (unsigned short number = 0; number < strawNumbers.size(); ++number) {

      unsigned short straw = strawNumbers[number];

      const ACsoft::Detector::TrdStraw* trdstraw = alignedTrd->GetTrdStraw(straw);
      unsigned short module = trdstraw->ModuleNumber();

      ACsoft::AC::MeasurementMode direction = trdstraw->Direction();
      TVector3 candidateHitPoint = trdstraw->GlobalPosition();
      float secondCoordinate = direction == ACsoft::AC::XZMeasurement ? trackPos.y() : trackPos.x();

      // exclude dead straws from the candidate list
      if (ExcludeDeadStraws && ACsoft::Utilities::DeadStrawLookup::Self()->IsStrawDead(straw, secondCoordinate, particle.TimeStamp()))
        continue;

      // we ignore rotations here
      if (direction == ACsoft::AC::XZMeasurement) candidateHitPoint.SetY(secondCoordinate);
      else candidateHitPoint.SetX(secondCoordinate);

      double residual = Distance(trdstraw->GlobalPosition(),trdstraw->GlobalWireDirection(),trackPos,trackDir);
      if (fabs(residual)<MinResidual) {
          MinResidual         = fabs(residual);
          MinDistHit          = TRDCandidateHit(straw, 0.0);
          MinDistHit.residual = residual;
          MinDistHit.d        = short(direction);
          MinDistHit.xy       = candidateHitPoint.X();
          if (direction==1) MinDistHit.xy = candidateHitPoint.Y();
          MinDistHit.z        = candidateHitPoint.Z();
      }
      float pathLength = Pathlength3d(ACsoft::AC::AMSGeometry::TRDTubeRadius,
                                      trdstraw->GlobalPosition(),trdstraw->GlobalWireDirection(),trackPos,trackDir);
      if (pathLength <= ::AC::Settings::TrdTubeDefaultMinPathLength) continue;

      TRDCandidateHit hit(straw, pathLength);
      hit.d        = direction;
      hit.xy       = candidateHitPoint.X();
      if (direction==1) hit.xy = candidateHitPoint.Y();
      hit.z        = candidateHitPoint.Z();
      hit.residual = residual;
      CandidateHitsOnTrack.push_back(hit);
      found = true;

      float xStraw = direction == 0 ? trdstraw->GlobalPosition().X() : trackPos.x();
      float yStraw = direction == 0 ? trackPos.y() : trdstraw->GlobalPosition().Y();
      DumpActiveStraw(straw, module, pathLength, xStraw, yStraw, trdstraw->GlobalPosition().Z());
    }

    if (!found && MinResidual < 2.*ACsoft::AC::AMSGeometry::TRDTubeRadius) CandidateHitsNearTrack.push_back(MinDistHit);

    StoreNeighboringStraws(CandidateHitsOnTrack,strawNumbers);
    StoreNeighboringStraws(CandidateHitsNearTrack,strawNumbers);

    CandidateHitsPerSubLayerOnTrack[sublayer].insert(CandidateHitsPerSubLayerOnTrack[sublayer].end(), CandidateHitsOnTrack.begin(), CandidateHitsOnTrack.end());
    CandidateHitsPerSubLayerNearTrack[sublayer].insert(CandidateHitsPerSubLayerNearTrack[sublayer].end(), CandidateHitsNearTrack.begin(), CandidateHitsNearTrack.end());
  }

  // first store on track hits
  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    fCandidateHitsPerSubLayer[sublayer].insert(fCandidateHitsPerSubLayer[sublayer].end(),CandidateHitsPerSubLayerOnTrack[sublayer].begin(),CandidateHitsPerSubLayerOnTrack[sublayer].end());
  }

  // if we have no on track hit in a layer, store the near track hits as candidate with a path length of 0
  if (!AddNearTrackHitsToCandidateList) return;

  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; sublayer += 2) {
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


void ACsoft::Analysis::TrdPreselection::StoreNeighboringStraws(std::vector<TRDCandidateHit>& CandidateHits, const std::vector<unsigned short>& strawNumbers) {

  // FIXME explain this function.

  for (unsigned int iCan=0; iCan<CandidateHits.size(); iCan++) {
    CandidateHits.at(iCan).v_NeighborXY.reserve(10);
    CandidateHits.at(iCan).v_NeighborZ.reserve(10);
    CandidateHits.at(iCan).v_NeighborStraws.reserve(10);
  }

  TVector3 trackPos, trackDir;

  DEBUG_OUT << "Getting aligned TRD" << std::endl;
  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();

  // determine for the candidates the neighboring straws
  for (unsigned int iCan=0; iCan<CandidateHits.size(); iCan++) {
    float zLayer = CandidateHits.at(iCan).z;
    for (unsigned short number = 0; number < strawNumbers.size(); ++number) {
      unsigned short straw = strawNumbers[number];
      // is this straw a neighbor of the candidate straw and not used by another candidate
      // (which happens if we have more than 1 hit per sublayer) ?
      if (straw == CandidateHits.at(iCan).straw)
        continue;
      bool hasBeenUsed = false;
      for (unsigned int jCan=0; jCan<CandidateHits.size(); jCan++) {
        if (straw == CandidateHits.at(jCan).straw) {
          hasBeenUsed = true;
          break;
        }
      }
      if (hasBeenUsed)
        continue;
      CandidateHits.at(iCan).v_NeighborStraws.push_back(straw);

      ACsoft::Detector::TrdStraw* trdstraw = trd->GetTrdStraw(straw);
      ACsoft::AC::MeasurementMode direction = trdstraw->Direction();
      fSplineTrack->CalculateLocalPositionAndDirection(zLayer, trackPos, trackDir);

      TVector3 candidateHitPoint = trdstraw->GlobalPosition();

      // FIXME this is all rather ugly. with the new Trd structure, there is no need to do this x/y separation anymore
      if (direction == ACsoft::AC::YZMeasurement) candidateHitPoint.SetX(trackPos.x());
      else candidateHitPoint.SetY(trackPos.y());

      float xy = candidateHitPoint.X();
      if (direction==ACsoft::AC::YZMeasurement) xy = candidateHitPoint.Y();
      float z  = candidateHitPoint.Z();
      CandidateHits.at(iCan).v_NeighborXY.push_back(xy);
      CandidateHits.at(iCan).v_NeighborZ.push_back(z);
    }
  }
}

bool ACsoft::Analysis::TrdPreselection::IsInsideTrdGeometricalAcceptance() const {

  if (!fSplineTrack) return false;

  TVector2 coordinatesFirst = ACsoft::Analysis::TrdPreselection::PointInLayer(0);
  TVector2 coordinatesLast  = ACsoft::Analysis::TrdPreselection::PointInLayer(ACsoft::AC::AMSGeometry::TRDLayers - 1);

  ACsoft::AC::AMSGeometry* geometry = ACsoft::AC::AMSGeometry::Self();
  if (!geometry->TRDFirstLayerContour->IsInside(coordinatesFirst.X(), coordinatesFirst.Y()))
    return false;
  if (!geometry->TRDLastLayerContour->IsInside(coordinatesLast.X(), coordinatesLast.Y()))
    return false;
  return true;
}

float ACsoft::Analysis::TrdPreselection::GetCandidatePathLength() const {

  float pathLengthSum = 0;
  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    for (unsigned short i = 0; i < fCandidateHitsPerSubLayer[sublayer].size(); ++i)
      pathLengthSum += fCandidateHitsPerSubLayer[sublayer][i].pathLength;
  }
  return pathLengthSum;
}

unsigned short ACsoft::Analysis::TrdPreselection::GetNumberOfCandidateLayers() const {

  bool SubLayers[ACsoft::AC::AMSGeometry::TRDSubLayers];
  for (unsigned short sublayer=0; sublayer<ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    SubLayers[sublayer] = (fCandidateHitsPerSubLayer[sublayer].size() > 0);
  }

  unsigned short candidateLayers = 0;
  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; sublayer += 2)
    candidateLayers += (SubLayers[sublayer] || SubLayers[sublayer + 1]);
  return candidateLayers;
}

unsigned short ACsoft::Analysis::TrdPreselection::GetNumberOfCandidateStraws() const {

  unsigned short candidateStraws = 0;
  for (unsigned short sublayer=0; sublayer<ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer) {
    candidateStraws += fCandidateHitsPerSubLayer[sublayer].size();
  }
  return candidateStraws;
}

TVector2 ACsoft::Analysis::TrdPreselection::PointInLayer(unsigned short layer) const {

  // if there is no spline track, we better return a point which is for sure outside the TRD acceptance
  if (!fSplineTrack) return TVector2(-9999.0, -9999.0);

  assert(layer < ACsoft::AC::AMSGeometry::TRDLayers);

  DEBUG_OUT << "Getting aligned TRD" << std::endl;
  ACsoft::Detector::Trd* trd = ACsoft::Detector::DetectorManager::Self()->GetAlignedTrd();
  float zLayer = trd->GetTrdLayer(layer)->GlobalPosition().Z();
  TVector3 vector = fSplineTrack->InterpolateToZ(zLayer);
  return TVector2(vector.x(), vector.y());
}

void ACsoft::Analysis::TrdPreselection::GetCandidateHits(std::vector<ACsoft::Analysis::TRDCandidateHit>& result) const {

  result.reserve(40);
  for (unsigned short sublayer = 0; sublayer < ACsoft::AC::AMSGeometry::TRDSubLayers; ++sublayer)
    result.insert(result.end(), fCandidateHitsPerSubLayer[sublayer].begin(), fCandidateHitsPerSubLayer[sublayer].end());
}

bool ACsoft::Analysis::TrdPreselection::PassesPreselectionCuts() const {
  if (GetNumberOfCandidateLayers() <= 15  ) return false;
  if (GetCandidatePathLength()     <=  7.0) return false;
  return true;
}


