#include "TrdCandidateMatching.hh"

#include "Event.h"
#include "AnalysisParticle.hh"
#include "SplineTrack.hh"
#include "TrackFactory.hh"
#include "TrdHitFactory.hh"
#include "TrdHit.hh"

#define DEBUG 0
#define INFO_OUT_TAG "TrdCandidateMatching> "
#include <debugging.hh>

Analysis::TrdCandidateMatching::TrdCandidateMatching() :
  fNumberOfMatchedHits(0) {
  fTrdAdcScaleFactor = 1.0;

}

Analysis::TrdCandidateMatching::~TrdCandidateMatching() {

}

void Analysis::TrdCandidateMatching::DetermineActiveHitsPerLayer(const Analysis::Particle& particle) {

  const Analysis::SplineTrack* splineTrack = particle.GetSplineTrack();

  fActiveHitsPerLayer.clear();
  for (unsigned int i=0; i<AC::AMSGeometry::TRDLayers; i++)
    fActiveHitsPerLayer.push_back(std::vector<Analysis::TrdHit>());

  for (unsigned int iHit = 0; iHit < particle.TrdHits().size(); ++iHit) {
    const Analysis::TrdHit& hit = particle.TrdHits().at(iHit);

    float          distance         = hit.DistanceToTrack(*splineTrack);
    unsigned short layer            = hit.Layer();
    float          RegionOfInterest = 1.0 + layer*0.1;
    if (distance>RegionOfInterest) continue;

    fActiveHitsPerLayer.at(layer).push_back(hit);
  }

}

void Analysis::TrdCandidateMatching::DetermineCandidateHitsPerLayer() {

  fCandidateHits = fPreselection.CandidateHits();

  fCandidateHitsPerLayer.clear();
  for (unsigned int i=0; i<AC::AMSGeometry::TRDLayers; i++) fCandidateHitsPerLayer.push_back(std::vector<Analysis::TRDCandidateHit>());

  for (unsigned int iHit=0; iHit<fCandidateHits.size(); iHit++) {
    unsigned short straw = fCandidateHits[iHit].straw;
    unsigned short layer = AC::TRDStrawToLayer(straw);
    fCandidateHitsPerLayer.at(layer).push_back(fCandidateHits[iHit]);
  }
  return;
}

// FIXME Add documentation!!! What is the logic behind the matched vector [] ???
void Analysis::TrdCandidateMatching::PrintMatching(const Analysis::Particle& particle, std::vector<bool> matched[]) {

    int Run    = particle.RawEvent()->RunHeader()->Run();
    int Event  = particle.RawEvent()->EventHeader().Event();

    printf("Run=%12d Event=%12d\n",Run,Event);
    printf("      ||                                              Candidate Hits                                           ||                    Active Hits \n");
    printf("layer || straw | Active Straw |  dEdX  | PathLen |    XY   | straw | Active Straw |  dEdX  | PathLen |    XY   |");
    int maxHitsPerLayer = 0;
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        if (int(fActiveHitsPerLayer[layer].size())>maxHitsPerLayer) maxHitsPerLayer = int(fActiveHitsPerLayer[layer].size());

    }
    for (int i=0; i<maxHitsPerLayer; i++) printf("| straw |   dE   |    XY   |  match |");
    std::cout << std::endl;
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        printf(" %4d |",layer);
        for (unsigned int i=0; i<2; i++) {
            if (i<fCandidateHitsPerLayer[layer].size()) {
                TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(i);
                int   direction;
                float strawXY, strawZ;
                AC::TRDStrawToCoordinates(CandidateHit.straw, direction, strawXY, strawZ);
                printf("| %5d |  %11d | %6.1f | %7.4f | %7.2f ",CandidateHit.straw, CandidateHit.ActiveStraw, CandidateHit.deDx, CandidateHit.pathLength,strawXY);
            } else {
                printf("|       |              |        |         |         ");
            }
        }
        //if (fCandidateHitsPerLayer[layer].size()<=0) printf(" |");
        printf("|");
        for (int i=0; i<maxHitsPerLayer; i++) {
            if (i<int(fActiveHitsPerLayer[layer].size())) {
                Analysis::TrdHit ActiveHit    = fActiveHitsPerLayer[layer].at(i);
                printf("| %5d | %6.1f | %7.2f | %6d |",ActiveHit.GetGlobalStrawNumber(), ActiveHit.GetAmplitude(), ActiveHit.R(), int(matched[layer].at(i)));
            } else {
                printf("|       |        |         |        |");
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

}

void Analysis::TrdCandidateMatching::MatchActiveAndCandidateHits(const Analysis::Particle& particle, bool ActiveStraws) {

    static const float MeanPathLength   = 0.6;   // cm

    std::vector<bool> matched[AC::AMSGeometry::TRDLayers];
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) matched[layer].push_back(false);
    }

    // we have only 1 candidate and 1 active hit in a layer
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        if (fActiveHitsPerLayer[layer].size()    !=1 ) continue;
        if (fCandidateHitsPerLayer[layer].size() !=1 ) continue;

        Analysis::TrdHit ActiveHit    = fActiveHitsPerLayer[layer].at(0);
        TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(0);
        float dE  = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;

        float dEdX = 0.0;
        if (ActiveHit.GetGlobalStrawNumber() == CandidateHit.straw) {
            if (CandidateHit.pathLength>=AC::AMSGeometry::TRDTubeMinPathLength) {
                dEdX = dE/CandidateHit.pathLength;
            } else {
                dEdX = dE/MeanPathLength;
            }
        } else {
            if (ActiveStraws) dEdX = dE/MeanPathLength;
            else continue;
        }
        fCandidateHitsPerLayer[layer].at(0).ActiveStraw = ActiveHit.GetGlobalStrawNumber();
        fCandidateHitsPerLayer[layer].at(0).deDx = dEdX;
        matched[layer].at(0) = true;
    }

    // try to match the remaining hits
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
            if (matched[layer].at(iAHit)) continue;
            Analysis::TrdHit ActiveHit = fActiveHitsPerLayer[layer].at(iAHit);
            float            dE        = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
            for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
                TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
                if (CandidateHit.deDx>0) continue;
                float dEdX = 0;
                if (ActiveHit.GetGlobalStrawNumber() == CandidateHit.straw) {
                    if (CandidateHit.pathLength>=AC::AMSGeometry::TRDTubeMinPathLength) {
                        dEdX = dE/CandidateHit.pathLength;
                    } else {
                        dEdX = dE/MeanPathLength;
                    }
                    fCandidateHitsPerLayer[layer].at(iCHit).ActiveStraw = ActiveHit.GetGlobalStrawNumber();
                    fCandidateHitsPerLayer[layer].at(iCHit).deDx = dEdX;
                    matched[layer].at(iAHit) = true;
                    break;
                }
            }
        }
    }

    if (ActiveStraws) {
        // one candidate hit but more than one active hit
        // => take the one with the largest amplitude
        for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
            if (fCandidateHitsPerLayer[layer].size() !=1 ) continue;
            TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(0);
            if (CandidateHit.deDx>0) continue;
            float maxAmplitude = 0.0;
            int   iMaxHit      = -1;
            for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
                if (matched[layer].at(iAHit)) continue;
                Analysis::TrdHit ActiveHit = fActiveHitsPerLayer[layer].at(iAHit);
                float            dE        = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
                if (dE>maxAmplitude) {
                    maxAmplitude = dE;
                    iMaxHit      = iAHit;
                }
            }
            if (iMaxHit<0) continue;

            Analysis::TrdHit ActiveHit = fActiveHitsPerLayer[layer].at(iMaxHit);
            float            dEdX      = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor/MeanPathLength;
            fCandidateHitsPerLayer[layer].at(0).ActiveStraw = ActiveHit.GetGlobalStrawNumber();
            fCandidateHitsPerLayer[layer].at(0).deDx = dEdX;
            matched[layer].at(iMaxHit) = true;
        }
    }

    if (DEBUG>=1) PrintMatching(particle,matched);

    // Store the assigned and unassigned hits
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
            if (matched[layer].at(iAHit)) fAssignedHits.push_back(fActiveHitsPerLayer[layer].at(iAHit));
            else fUnassignedHits.push_back(fActiveHitsPerLayer[layer].at(iAHit));
        }
    }

    fCandidateHits.clear();
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
        for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) fCandidateHits.push_back(fCandidateHitsPerLayer[layer].at(iCHit));
    }

    fNumberOfMatchedHits = 0;
    for (unsigned int iCHit=0; iCHit<fCandidateHits.size(); iCHit++) {
        if (fCandidateHits.at(iCHit).deDx>0)  fNumberOfMatchedHits++;
    }
    //printf("fCandidateHits=%4d fNumberOfMatchedHits=%4d \n",int(fCandidateHits.size()),fNumberOfMatchedHits);

}


bool Analysis::TrdCandidateMatching::Process(const Analysis::Particle& particle, float MinPathLength , bool ActiveStraws ) {

  fNumberOfMatchedHits = 0;
  fCandidateHits.clear();
  fAssignedHits.clear();
  fUnassignedHits.clear();

  bool IsInsideTRDGeometricalAcceptance = fPreselection.Process(particle, MinPathLength);

  DetermineActiveHitsPerLayer(particle);

  DetermineCandidateHitsPerLayer();

  MatchActiveAndCandidateHits(particle, ActiveStraws);

  return IsInsideTRDGeometricalAcceptance && UsefulForTrdParticleId();
}

const std::vector<Analysis::TRDCandidateHit>& Analysis::TrdCandidateMatching::CandidateHits() const{
    return fCandidateHits;
}

bool Analysis::TrdCandidateMatching::UsefulForTrdParticleId() const {
  if (!fPreselection.PassesPreselectionCuts()) return false;
  if (fNumberOfMatchedHits < 12) return false;
  return true;
}

unsigned short Analysis::TrdCandidateMatching::GetNumberOfActiveLayers() const {

  unsigned short TrdActiveLayer[AC::AMSGeometry::TRDLayers] = {0};
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) {
      unsigned short layer = AC::TRDStrawToLayer(hit.straw);
      TrdActiveLayer[layer] = 1;
    }
  }

  unsigned short nTrdActiveLayers = 0;
  for (unsigned int i=0; i<AC::AMSGeometry::TRDLayers; i++)
    nTrdActiveLayers += TrdActiveLayer[i];
  return nTrdActiveLayers;
}

unsigned short Analysis::TrdCandidateMatching::GetNumberOfActiveStraws() const {

  unsigned short activeStraws = 0;
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) ++activeStraws;
  }

  return activeStraws;
}


float Analysis::TrdCandidateMatching::GetActivePathLength() const {

  float pathLength = 0;
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) pathLength += hit.pathLength;
  }

  return pathLength;
}



