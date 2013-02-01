#include "TrdCandidateMatching.hh"

#include "Event.h"
#include "AnalysisParticle.hh"
#include "SplineTrack.hh"
#include "TrackFactory.hh"
#include "TrdHitFactory.hh"
#include "TrdHit.hh"
#include "Utilities.hh"
#include "TrdDetector.hh"
#include "TrdStraw.hh"
#include "TrdModule.hh"
#include "TrdLayer.hh"
#include "TrdSublayer.hh"
#include "DetectorManager.hh"
#include "pathlength_functions.hh"



#define DEBUG 0
#define INFO_OUT_TAG "TrdCandidateMatching> "
#include <debugging.hh>

ACsoft::Analysis::TrdCandidateMatching::TrdCandidateMatching()
  : fNumberOfMatchedHits(0)
  , fTrdStandAloneFitXZ(-1,-1)
  , fTrdCombinedFitXZ(-1,-1)
  , fTrkFitXZ(-1,-1)
  , fTrdStandAloneFitYZ(-1,-1)
  , fTrdCombinedFitYZ(-1,-1)
  , fTrkFitYZ(-1,-1)
{
  fTrdAdcScaleFactor = 1.0;

  fActiveHitsPerLayer.reserve(AC::AMSGeometry::TRDLayers);
  fActiveHitsPerLayer.assign(AC::AMSGeometry::TRDLayers, std::vector<ACsoft::Analysis::TrdHit>());
  for (unsigned int layer = 0; layer < AC::AMSGeometry::TRDLayers; ++layer)
    fActiveHitsPerLayer.at(layer).reserve(2);

  fMeanPathLength    = 0.3;
  fXhitLayer1        = 0;
  fYhitLayer1        = 0;
  fXfitLayer1        = 0;
  fYfitLayer1        = 0;
  fHitLayer1         = false;

}

ACsoft::Analysis::TrdCandidateMatching::~TrdCandidateMatching() {

}

void ACsoft::Analysis::TrdCandidateMatching::DetermineActiveHitsPerLayer(const ACsoft::Analysis::Particle& particle) {

  // FIXME: proper implementation using PDG formula for multiple scattering, include Z dependence
  // due to multiple scattering the region of interest is rigidity dependent
  float aRig = fabs(particle.MainTrackerTrackFit()->Rigidity());
  static double R1     =5.0, R2     =50.0;
  static double deltaR1=0.1, deltaR2= 0.0;

  for (unsigned int layer = 0; layer < AC::AMSGeometry::TRDLayers; ++layer)
    fActiveHitsPerLayer.at(layer).clear();

  for (unsigned int iHit = 0; iHit < particle.TrdHits().size(); ++iHit) {
    const ACsoft::Analysis::TrdHit& hit = particle.TrdHits().at(iHit);
    float distance = hit.DistanceToTrack();
    unsigned short layer = hit.Layer();
    float delta = std::max(0.0,deltaR2/(R2-R1)*(aRig-R1) + deltaR1/(R2-R1)*(R2-aRig));
    float RegionOfInterest = 1.0 + layer * delta;
    if (distance > RegionOfInterest)
      continue;
    fActiveHitsPerLayer.at(layer).push_back(hit);
  }

}

void ACsoft::Analysis::TrdCandidateMatching::DetermineCandidateHitsPerLayer() {

  fCandidateHits.clear();
  fPreselection.GetCandidateHits(fCandidateHits);

  fCandidateHitsPerLayer.clear();
  for (unsigned int i=0; i<AC::AMSGeometry::TRDLayers; i++) fCandidateHitsPerLayer.push_back(std::vector<ACsoft::Analysis::TRDCandidateHit>());

  for (unsigned int iHit=0; iHit<fCandidateHits.size(); iHit++) {
    unsigned short straw = fCandidateHits[iHit].straw;
    unsigned short layer = ACsoft::AC::TRDStrawToLayer(straw);
    fCandidateHitsPerLayer.at(layer).push_back(fCandidateHits[iHit]);
  }
  return;
}

void ACsoft::Analysis::TrdCandidateMatching::PrintMatching(const ACsoft::Analysis::Particle& particle) {

  int   Run      = particle.RawEvent()->RunHeader()->Run();
  int   Event    = particle.RawEvent()->EventHeader().Event();
  float Rigidity = particle.MainTrackerTrackFit()->Rigidity();

  printf("Run=%12d Event=%12d Rigidity=%9.2f Track Hit in Plane 1: %2d\n",Run,Event,Rigidity,int(fHitLayer1));
  printf("      ||                                              Candidate Hits                                                                                 ||                    Active Hits \n");
  printf("layer || straw | Active Straw |  dEdX  | PLenTrk | PLenTrd |  Chi2  |    XY   | straw | Active Straw |  dEdX  | PLenTrk | PLenTrd |  Chi2  |    XY   |");
  int maxHitsPerLayer = 0;
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    if (int(fActiveHitsPerLayer[layer].size())>maxHitsPerLayer) maxHitsPerLayer = int(fActiveHitsPerLayer[layer].size());

  }
  for (int i=0; i<maxHitsPerLayer; i++) printf("| straw |   dE   |    XY   | match |  Chi2  |");
  std::cout << std::endl;
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    printf(" %4d |",layer);
    for (unsigned int i=0; i<2; i++) {
      if (i<fCandidateHitsPerLayer[layer].size()) {
        TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(i);
        float strawXY = CandidateHit.xy;
        printf("| %5d |  %11d | %6.1f | %7.4f | %7.4f | %6.1f | %7.2f ",CandidateHit.straw, CandidateHit.ActiveStraw, CandidateHit.deDx, CandidateHit.pathLength, CandidateHit.pathLengthTrdTrack,CandidateHit.Chi2,strawXY);
      } else {
        if (fMissingHitsPerLayer[layer].size()>0) {
          TRDMissingHit  MissingHit = fMissingHitsPerLayer[layer].at(0);
          float XY = MissingHit.x;
          if (MissingHit.d==1) XY = MissingHit.y;
          printf("| %5d |  %11d | %6.1f | %7.4f | %7.4f | %6.1f | %7.2f ",MissingHit.straw1, MissingHit.straw2, -1.0, -1.0, -1.0 , MissingHit.Chi2, XY);
        } else {
          printf("|       |              |        |         |         |        |         ");
        }
      }
    }
    printf("|");
    for (int i=0; i<maxHitsPerLayer; i++) {
      if (i<int(fActiveHitsPerLayer[layer].size())) {
        const ACsoft::Analysis::TrdHit& ActiveHit = fActiveHitsPerLayer[layer].at(i);
        printf("| %5d | %6.1f | %7.2f | %5d | %6.1f |",ActiveHit.GlobalStrawNumber(), ActiveHit.GetAmplitude(), ActiveHit.R(), int(fActiveHitsMatched[layer].at(i)), fActiveHitsChi2[layer].at(i));
      } else {
        printf("|       |        |         |       |        |");
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

}

void ACsoft::Analysis::TrdCandidateMatching::MatchActiveAndCandidateHits(const ACsoft::Analysis::Particle& particle, bool AddNearTrackHits) {

  // reset
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    fActiveHitsMatched[layer].clear();
    fActiveHitsChi2[layer].clear();
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
      fActiveHitsMatched[layer].push_back(false);
      fActiveHitsChi2[layer].push_back(-1.0);
    }
  }

  // we have only 1 candidate and 1 active hit in a layer
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    if (fActiveHitsPerLayer[layer].size()    !=1 ) continue;
    if (fCandidateHitsPerLayer[layer].size() !=1 ) continue;

    ACsoft::Analysis::TrdHit ActiveHit    = fActiveHitsPerLayer[layer].at(0);
    TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(0);
    float dE  = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;

    if (ActiveHit.GlobalStrawNumber() != CandidateHit.straw) {
      if (!AddNearTrackHits) continue;
      fCandidateHitsPerLayer[layer].at(0).ActiveStraw = ActiveHit.GlobalStrawNumber();
    }
    fCandidateHitsPerLayer[layer].at(0).deDx        = dE;
    fCandidateHitsPerLayer[layer].at(0).xy          = ActiveHit.R();
    fCandidateHitsPerLayer[layer].at(0).z           = ActiveHit.Z();
    fActiveHitsMatched[layer].at(0) = true;
  }

  // try to match the remaining hits
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
      if (fActiveHitsMatched[layer].at(iAHit)) continue;
      const ACsoft::Analysis::TrdHit& ActiveHit = fActiveHitsPerLayer[layer].at(iAHit);
      float            dE        = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
      for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
        const TRDCandidateHit& CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
        if (CandidateHit.deDx>0) continue;
        if (ActiveHit.GlobalStrawNumber() == CandidateHit.straw) {
          fCandidateHitsPerLayer[layer].at(iCHit).deDx  = dE;
          fCandidateHitsPerLayer[layer].at(iCHit).xy    = ActiveHit.R();
          fCandidateHitsPerLayer[layer].at(iCHit).z     = ActiveHit.Z();
          fActiveHitsMatched[layer].at(iAHit)           = true;
          break;
        }
      }
    }
  }

  fMissingHitsPerLayer.clear();
  if (AddNearTrackHits) {
    // one candidate hit but more than one active hit
    // => take the one with the largest amplitude
    for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
      if (fCandidateHitsPerLayer[layer].size() !=1 ) continue;
      const TRDCandidateHit& CandidateHit = fCandidateHitsPerLayer[layer].at(0);
      if (CandidateHit.deDx>0) continue;
      float maxAmplitude = 0.0;
      int   iMaxHit      = -1;
      for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
        if (fActiveHitsMatched[layer].at(iAHit)) continue;
        const ACsoft::Analysis::TrdHit& ActiveHit = fActiveHitsPerLayer[layer].at(iAHit);
        float            dE        = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
        if (dE>maxAmplitude) {
          maxAmplitude = dE;
          iMaxHit      = iAHit;
        }
      }
      if (iMaxHit<0) continue;

      const ACsoft::Analysis::TrdHit& ActiveHit = fActiveHitsPerLayer[layer].at(iMaxHit);
      float            dE        = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
      fCandidateHitsPerLayer[layer].at(0).ActiveStraw = ActiveHit.GlobalStrawNumber();
      fCandidateHitsPerLayer[layer].at(0).deDx        = dE;
      fCandidateHitsPerLayer[layer].at(0).xy          = ActiveHit.R();
      fCandidateHitsPerLayer[layer].at(0).z           = ActiveHit.Z();
      fActiveHitsMatched[layer].at(iMaxHit)           = true;
    }

  }

  // fit the spline tracker track with a straight line
  testSplineTracksDeviationFromStraighLine(particle);

  // Refit the track using TRD hits and TRD Missing Hits
  fMissingHitsPerLayer.clear();
  fMissingHitsPerLayer.assign(AC::AMSGeometry::TRDLayers, std::vector<ACsoft::Analysis::TRDMissingHit>());
  int NumberOfCalls = 0;
  int Reason        = 0;


  // fill the vectors for the line fits
  std::vector<TrdTrackPoint> TrdTrackPointsXZ, TrdTrackPointsYZ;
  TrdTrackPointsXZ.reserve(40);
  TrdTrackPointsYZ.reserve(40);

  if (AddNearTrackHits) {
    while (Reason>=0) {
      LinearRegression(particle, Reason, TrdTrackPointsXZ, TrdTrackPointsYZ);
      NumberOfCalls ++;
      if (NumberOfCalls>10) {
        std::cout << std::endl;
        WARN_OUT << "Exceeding number of calls !!!  Run=" << particle.RawEvent()->RunHeader()->Run() << " Event=" << particle.RawEvent()->EventHeader().Event() << std::endl;
        PrintMatching(particle);
        break;
      }
    }

  } else {
    Reason = -1;
    LinearRegression(particle, Reason, TrdTrackPointsXZ, TrdTrackPointsYZ);
  }

  if (DEBUG>=1) PrintMatching(particle);

  // Store the assigned and unassigned hits
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
      if (fActiveHitsMatched[layer].at(iAHit)) fAssignedHits.push_back(fActiveHitsPerLayer[layer].at(iAHit));
      else fUnassignedHits.push_back(fActiveHitsPerLayer[layer].at(iAHit));
    }
  }

  fCandidateHits.clear();
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) fCandidateHits.push_back(fCandidateHitsPerLayer[layer].at(iCHit));
  }

  int iFlag=1; // default 1, i.e. use pathlength calculated from tracker track, 0: don't normalise to pathlength, 2: use pathlength from trd track
  CalculateDeDx(AddNearTrackHits,iFlag);

}

//
// Is the spline track a straight line ?
//
void ACsoft::Analysis::TrdCandidateMatching::testSplineTracksDeviationFromStraighLine(const ACsoft::Analysis::Particle& particle) {

  fTrkFitXZ.Chi2 = -1;
  fTrkFitYZ.Chi2 = -1;

  const ACsoft::Analysis::SplineTrack* splineTrack     = particle.GetSplineTrack();
  Detector::Trd* trd = Detector::DetectorManager::Self()->GetAlignedTrd();
  // use TRD center to minimize correlations between the 2 fit parameters
  float OffsetZ = trd->Position().Z();

  // How straight is the spline track ?
  std::vector<double> v_sx, v_sy, v_sxz, v_syz, v_sxe, v_sye;
  for (unsigned int subLayer=0; subLayer<AC::AMSGeometry::TRDSubLayers; ++subLayer) {
    float zSublayer = trd->GetTrdSublayer(subLayer)->GlobalPosition().Z();
    short layer  = subLayer/2;
    int   direction = 0;
    if (layer<4 || layer>15) direction = 1;
    TVector3 point = splineTrack->InterpolateToZ(zSublayer);
    if (direction==0) {
      v_sx.push_back(point.X());
      v_sxz.push_back(point.Z()-OffsetZ);
      v_sxe.push_back(0.002);
    } else {
      v_sy.push_back(point.Y());
      v_syz.push_back(point.Z()-OffsetZ);
      v_sye.push_back(0.002);
    }
  }

  double parX_0, e_parX_0, parX_1, e_parX_1, rhox;
  int    NdofX = v_sxz.size()-2;
  double Chi2X = Utilities::FastLineFit(v_sxz,v_sx,v_sxe,parX_0,e_parX_0,parX_1,e_parX_1,rhox);
  fTrkFitXZ.Ndof = NdofX;
  fTrkFitXZ.Chi2 = Chi2X;

  double parY_0, e_parY_0, parY_1, e_parY_1, rhoy;
  int    NdofY = v_syz.size()-2;
  double Chi2Y = Utilities::FastLineFit(v_syz,v_sy,v_sye,parY_0,e_parY_0,parY_1,e_parY_1,rhoy);
  fTrkFitYZ.Ndof = NdofY;
  fTrkFitYZ.Chi2 = Chi2Y;

  float maxResidualX = 0.0;
  for (unsigned int i=0; i<v_sx.size(); i++) {
    float residual = v_sx.at(i) - (parX_0 + parX_1*v_sxz.at(i));
    if (fabs(residual)>fabs(maxResidualX)) {
      maxResidualX = residual;
    }
  }
  float maxResidualY = 0.0;
  for (unsigned int i=0; i<v_sy.size(); i++) {
    float residual = v_sy.at(i) - (parY_0 + parY_1*v_syz.at(i));
    if (fabs(residual)>fabs(maxResidualY)) {
      maxResidualY = residual;
    }
  }

  DEBUG_OUT << "Fit SplineTrack with straight line" << std::endl;
  DEBUG_OUT << "Chi2X=" << Chi2X/NdofX << " maxResidualX=" << maxResidualX << std::endl;
  DEBUG_OUT << "Chi2Y=" << Chi2Y/NdofY << " maxResidualY=" << maxResidualY << std::endl;

  TVector3 Point1XZ, Point2XZ;
  Point1XZ.SetZ(AC::AMSGeometry::ZTOFUpper);
  Point2XZ.SetZ(Point1XZ.Z()+1.0);
  Point1XZ.SetX(parX_0 + parX_1*(Point1XZ.Z()-OffsetZ));
  Point2XZ.SetX(parX_0 + parX_1*(Point2XZ.Z()-OffsetZ));
  fTrkFitXZ.Position  = Point1XZ;
  fTrkFitXZ.Direction = (Point2XZ - Point1XZ).Unit();

  TVector3 Point1YZ, Point2YZ;
  Point1YZ.SetZ(AC::AMSGeometry::ZTOFUpper);
  Point2YZ.SetZ(Point1YZ.Z()+1.0);
  Point1YZ.SetY(parY_0 + parY_1*(Point1YZ.Z()-OffsetZ));
  Point2YZ.SetY(parY_0 + parY_1*(Point2YZ.Z()-OffsetZ));
  fTrkFitYZ.Position  = Point1YZ;
  fTrkFitYZ.Direction = (Point2YZ - Point1YZ).Unit();
}


void ACsoft::Analysis::TrdCandidateMatching::LinearRegression(const ACsoft::Analysis::Particle& particle, int &Reason, std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ) {

  if (DEBUG>0) std::cout << std::endl;
  DEBUG_OUT << "Reason=" << Reason << std::endl;

  // fill the vectors for the line fits
  BuildTrdTrackPoints(particle, TrdTrackPointsXZ, TrdTrackPointsYZ);

  // nothing to fit ?
  if (TrdTrackPointsXZ.size()<1 && TrdTrackPointsYZ.size()<1) {
    Reason = -1;
    return;
  }

  // fit the TRD track
  FitTrdTrackCombinedWithSeedTrack(TrdTrackPointsXZ, TrdTrackPointsYZ);

  // nothing to optimize and no chance for a TRD stand alone track ?
  if (TrdTrackPointsXZ.size()<4 && TrdTrackPointsYZ.size()<4) {
    Reason = -1;
    return;
  }

  // Start the iterative procedure only if Reason>=0
  if (Reason>=0) {
    // remove a hit if it dominates the chi2 and the straw != ActiveStraw
    bool HitRemoved = RemoveActiveHits();
    // try again
    if (HitRemoved) {
      Reason = 1;
      return;
    }

    // Do we have any active hits which could fill the empty layers ?
    bool HitAdded = AddActiveHits();
    // try again
    if (HitAdded) {
      Reason = 2;
      return;
    }

    // Generate Missing Hits
    bool MissingHitsGenerated = GenerateMissingHits(particle);
    // try again
    if (MissingHitsGenerated) {
      Reason = 3;
      return;
    }

    // Recalculate the path length
    RecalculatePathLength(0);
    RecalculatePathLength(1);
  }

  // Fit a straight line to the TRD only information without external constraints
  FitTrdTrackStandAlone(TrdTrackPointsXZ, TrdTrackPointsYZ);

  Reason = -1;
  return;

}

void ACsoft::Analysis::TrdCandidateMatching::FitTrdTrackWithStraightLine(unsigned int direction, std::vector<TrdTrackPoint>& FitPoints, unsigned int first, unsigned int last, TrackFitResult& Result) {
  // FIXME with full 3D alignment, this approach is no longer consistent. Do *one* 3D fit instead, with a chi2 based on distances between (skew) lines
  Result.Chi2 = -1;
  Result.Ndof = -1;

  if (direction == 0) {
    DEBUG_OUT << "------------------ XZ Fit---------------------" << std::endl;
  } else {
    DEBUG_OUT << "------------------ YZ Fit---------------------" << std::endl;
  }

  // use TRD center to minimize correlations between the 2 fit parameters
  static const float OffsetZ = AC::AMSGeometry::ZTRDCenter;

  static std::vector<double>* v_x = 0;
  static std::vector<double>* v_z = 0;
  static std::vector<double>* v_e = 0;
  if (!v_x) {
    v_x = new std::vector<double>;
    v_x->reserve(40);
    v_z = new std::vector<double>;
    v_z->reserve(40);
    v_e = new std::vector<double>;
    v_e->reserve(40);
  } else {
    v_x->clear();
    v_z->clear();
    v_e->clear();
  }

  for (unsigned int i=first; i<last; i++) {
    v_x->push_back(FitPoints.at(i).xy);
    v_z->push_back(FitPoints.at(i).z);
    v_e->push_back(FitPoints.at(i).sigmaXY);
  }

  double par_0, e_par_0, par_1, e_par_1, rho;
  int   Ndf  = v_x->size() - 2;
  float chi2 = Utilities::FastLineFit(*v_z, *v_x, *v_e, par_0, e_par_0, par_1, e_par_1, rho);
  DEBUG_OUT << "Chi2= " << chi2 << std::endl;
  DEBUG_OUT << "par0= " << par_0 << " +/- " << e_par_0 << std::endl;
  DEBUG_OUT << "par1= " << par_1 << " +/- " << e_par_1 << std::endl;
  DEBUG_OUT << "rho = " << rho << std::endl;
  Result.Chi2 = chi2;
  Result.Ndof = Ndf;
  if (chi2<0) return;

  // Resid and Chi2 contribution for each point
  for (unsigned int i=first; i<last; i++) {
    FitPoints.at(i).Resid = v_x->at(i - first) - (par_0+par_1*v_z->at(i - first));
    FitPoints.at(i).Chi2  = pow(FitPoints.at(i).Resid/v_e->at(i - first),2);
  }

  // calculate CHi2 for all active hits
  static float sigmaXY_TRD = 2.0*AC::AMSGeometry::TRDTubeRadius/sqrt(12.0);
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
      const ACsoft::Analysis::TrdHit& ActiveHit = fActiveHitsPerLayer[layer].at(iAHit);
      if (ActiveHit.Orientation() != (int)direction) continue;
      float Chi2_A = pow((ActiveHit.R() - (par_0 + par_1*(ActiveHit.Z()-OffsetZ)))/sigmaXY_TRD,2);
      fActiveHitsChi2[layer].at(iAHit) = Chi2_A;
    }
  }

  TVector3 Point1, Point2;
  Point1.SetZ(AC::AMSGeometry::ZTOFUpper);
  Point2.SetZ(Point1.Z()+1.0);
  if (direction == 0) {
    Point1.SetX(par_0 + par_1*(Point1.Z()-OffsetZ));
    Point2.SetX(par_0 + par_1*(Point2.Z()-OffsetZ));
  } else {
    Point1.SetY(par_0 + par_1*(Point1.Z()-OffsetZ));
    Point2.SetY(par_0 + par_1*(Point2.Z()-OffsetZ));
  }
  Result.Position  = Point1;
  Result.Direction = (Point2 - Point1).Unit();

}

void ACsoft::Analysis::TrdCandidateMatching::FitTrdTrackStandAlone(std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ) {

  // we need at least 2 internal TRD points
  if (TrdTrackPointsXZ.size()<4) return;
  if (TrdTrackPointsYZ.size()<4) return;

  FitTrdTrackWithStraightLine(0, TrdTrackPointsXZ, 1, TrdTrackPointsXZ.size() - 1, fTrdStandAloneFitXZ);
  FitTrdTrackWithStraightLine(1, TrdTrackPointsYZ, 1, TrdTrackPointsYZ.size() - 1, fTrdStandAloneFitYZ);
}

void ACsoft::Analysis::TrdCandidateMatching::FitTrdTrackCombinedWithSeedTrack(std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ) {

  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    fActiveHitsChi2[layer].clear();
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) fActiveHitsChi2[layer].push_back(-1.0);
  }

  FitTrdTrackWithStraightLine(0, TrdTrackPointsXZ, 0, TrdTrackPointsXZ.size(), fTrdCombinedFitXZ);
  FitTrdTrackWithStraightLine(1, TrdTrackPointsYZ, 0, TrdTrackPointsYZ.size(), fTrdCombinedFitYZ);

  // CHi2 contributions for the candidate hits
  for (unsigned int i=0; i<TrdTrackPointsXZ.size(); i++) {
    if (TrdTrackPointsXZ.at(i).id != 1) continue;
    short layer = TrdTrackPointsXZ.at(i).layer;
    short iHit  = TrdTrackPointsXZ.at(i).iHit;
    fCandidateHitsPerLayer[layer].at(iHit).Chi2 = TrdTrackPointsXZ.at(i).Chi2;
  }
  for (unsigned int i=0; i<TrdTrackPointsYZ.size(); i++) {
    if (TrdTrackPointsYZ.at(i).id != 1) continue;
    short layer = TrdTrackPointsYZ.at(i).layer;
    short iHit  = TrdTrackPointsYZ.at(i).iHit;
    fCandidateHitsPerLayer[layer].at(iHit).Chi2 = TrdTrackPointsYZ.at(i).Chi2;
  }

  // CHi2 contributions for the missing hits
  for (unsigned int i=0; i<TrdTrackPointsXZ.size(); i++) {
    if (TrdTrackPointsXZ.at(i).id != 2) continue;
    short layer = TrdTrackPointsXZ.at(i).layer;
    short iHit  = TrdTrackPointsXZ.at(i).iHit;
    fMissingHitsPerLayer[layer].at(iHit).Chi2 = TrdTrackPointsXZ.at(i).Chi2;
  }
  for (unsigned int i=0; i<TrdTrackPointsYZ.size(); i++) {
    if (TrdTrackPointsYZ.at(i).id != 2) continue;
    short layer = TrdTrackPointsYZ.at(i).layer;
    short iHit  = TrdTrackPointsYZ.at(i).iHit;
    fMissingHitsPerLayer[layer].at(iHit).Chi2 = TrdTrackPointsYZ.at(i).Chi2;
  }

}


void ACsoft::Analysis::TrdCandidateMatching::BuildTrdTrackPoints(const ACsoft::Analysis::Particle& particle, std::vector<TrdTrackPoint>& TrdTrackPointsXZ, std::vector<TrdTrackPoint>& TrdTrackPointsYZ) {

  DEBUG_OUT << "BuildTrdTrackPoints..." << std::endl;

  // use TRD center to minimize correlations between the 2 fit parameters
  static float OffsetZ = AC::AMSGeometry::ZTRDCenter;

  const ACsoft::Analysis::SplineTrack* splineTrack     = particle.GetSplineTrack();
  TVector3  PointInUpperTof = splineTrack->InterpolateToZ(AC::AMSGeometry::ZTOFUpper);
  static float sigmaX_Tracker     = 0.005;             // track track error at Upper Tof
  static float sigmaY_Tracker     = 0.005;             // track track error at Upper Tof
  static float sigmaXY_TRD        = 2.0*AC::AMSGeometry::TRDTubeRadius/sqrt(12.0);

  TrdTrackPointsXZ.clear();
  TrdTrackPointsYZ.clear();

  // first point from the tracker track position at the upper ToF
  // assuming multiple scattering is mainly happening in the upper ToF and in the lower TRD carbon fibre disc
  TrdTrackPointsXZ.push_back(TrdTrackPoint(0,PointInUpperTof.X(),PointInUpperTof.Z()-OffsetZ,sigmaX_Tracker));
  TrdTrackPointsYZ.push_back(TrdTrackPoint(0,PointInUpperTof.Y(),PointInUpperTof.Z()-OffsetZ,sigmaY_Tracker));

  // Candidate Hits
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
      TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
      if (CandidateHit.deDx<=0) continue;
      if (CandidateHit.d==0) {
        TrdTrackPointsXZ.push_back(TrdTrackPoint(1,CandidateHit.xy,CandidateHit.z - OffsetZ,sigmaXY_TRD,layer,iCHit));
      } else if (CandidateHit.d==1) {
        TrdTrackPointsYZ.push_back(TrdTrackPoint(1,CandidateHit.xy,CandidateHit.z - OffsetZ,sigmaXY_TRD,layer,iCHit));
      } else {
        WARN_OUT << "Invalid Straw direction !" << CandidateHit.d << std::endl;
      }
    }
  }

  // Missing Hits
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iMHit=0; iMHit<fMissingHitsPerLayer[layer].size(); iMHit++) {
      TRDMissingHit  MissingHit = fMissingHitsPerLayer[layer].at(iMHit);
      if (MissingHit.d==0) {
        TrdTrackPointsXZ.push_back(TrdTrackPoint(2,MissingHit.x,MissingHit.z - OffsetZ,MissingHit.sigma,layer,iMHit));
      } else if (MissingHit.d==1) {
        TrdTrackPointsYZ.push_back(TrdTrackPoint(2,MissingHit.y,MissingHit.z - OffsetZ,MissingHit.sigma,layer,iMHit));
      } else {
        WARN_OUT << "Invalid Missing Hit direction !" << MissingHit.d << std::endl;
      }
    }
  }

  // FIXME: proper implementation using PDG formula for multiple scattering, include Z dependence
  // due to multiple scattering the region of interest is rigidity dependent
  float aRig = fabs(particle.MainTrackerTrackFit()->Rigidity());
  static float R1       =5.0, R2       = 50.0;
  static float sigmaX_R1=0.1, sigmaX_R2= sigmaX_Tracker;
  static float sigmaY_R1=0.1, sigmaY_R2= sigmaY_Tracker;
  float        sigmaX   = std::max(sigmaX_Tracker,sigmaX_R2/(R2-R1)*(aRig-R1) + sigmaX_R1/(R2-R1)*(R2-aRig));
  float        sigmaY   = std::max(sigmaY_Tracker,sigmaY_R2/(R2-R1)*(aRig-R1) + sigmaY_R1/(R2-R1)*(R2-aRig));
  if (fHitLayer1) {
    TrdTrackPointsXZ.push_back(TrdTrackPoint(0,fXhitLayer1,AC::AMSGeometry::ZTrackerLayer1-OffsetZ,sigmaX_Tracker));
    TrdTrackPointsYZ.push_back(TrdTrackPoint(0,fYhitLayer1,AC::AMSGeometry::ZTrackerLayer1-OffsetZ,sigmaY_Tracker));
  } else {
    TrdTrackPointsXZ.push_back(TrdTrackPoint(0,fXfitLayer1,AC::AMSGeometry::ZTrackerLayer1-OffsetZ,sigmaX));
    TrdTrackPointsYZ.push_back(TrdTrackPoint(0,fYfitLayer1,AC::AMSGeometry::ZTrackerLayer1-OffsetZ,sigmaY));
  }

  if (DEBUG>=1) {
    std::cout << "XZ: layers="; for (unsigned int i=0; i<TrdTrackPointsXZ.size(); i++) printf(" %4d,",TrdTrackPointsXZ.at(i).layer); std::cout << std::endl;
    std::cout << "XZ: hits  ="; for (unsigned int i=0; i<TrdTrackPointsXZ.size(); i++) printf(" %4d,",TrdTrackPointsXZ.at(i).iHit); std::cout << std::endl;
    std::cout << "YZ: layers="; for (unsigned int i=0; i<TrdTrackPointsYZ.size(); i++) printf(" %4d,",TrdTrackPointsYZ.at(i).layer); std::cout << std::endl;
    std::cout << "YZ: hits  ="; for (unsigned int i=0; i<TrdTrackPointsYZ.size(); i++) printf(" %4d,",TrdTrackPointsYZ.at(i).iHit); std::cout << std::endl;
  }

}


bool ACsoft::Analysis::TrdCandidateMatching::RemoveActiveHits() {

  bool HitRemoved = false;
  // Find Candidate Hit with highest contribution to the Chi2
  float Chi2Max_X =  0, Chi2Max_Y  =  0;
  int   iX_max    = -1, iY_max     = -1;
  int   layX_max  = -1, layY_max   = -1;
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
      TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
      if (CandidateHit.deDx<=0) continue;
      float Chi2 = fCandidateHitsPerLayer[layer].at(iCHit).Chi2;
      if (CandidateHit.d==0 && fTrdCombinedFitXZ.Chi2/fTrdCombinedFitXZ.Ndof>2.0) {
        if (Chi2 > Chi2Max_X) {
          Chi2Max_X = Chi2;
          iX_max    = iCHit;
          layX_max  = layer;
        }
      } else if (CandidateHit.d==1 && fTrdCombinedFitYZ.Chi2/fTrdCombinedFitYZ.Ndof>2.0) {
        if (Chi2 > Chi2Max_Y) {
          Chi2Max_Y = Chi2;
          iY_max    = iCHit;
          layY_max  = layer;
        }
      }
    }
  }

  // remove a hit if: it dominates the chi2 and the straw != ActiveStraw
  DEBUG_OUT << "Candidates to be removed" << std::endl;
  DEBUG_OUT << "iX_max=" << iX_max << " layX_max=" << layX_max << " Chi2Max_X=" << Chi2Max_X << " fChi2X=" << fTrdCombinedFitXZ.Chi2 << std::endl;
  bool removeXhit = false;
  if (iX_max>=0 && fCandidateHitsPerLayer[layX_max].at(iX_max).ActiveStraw!=-1 && fCandidateHitsPerLayer[layX_max].size()>2) {
    if (fTrdCombinedFitXZ.Chi2>2.0 && Chi2Max_X/fTrdCombinedFitXZ.Chi2>0.6) removeXhit = true;
    //if (Chi2Max_X>20.0) removeXhit = true;
  }

  if (removeXhit) {
    DEBUG_OUT << "X-Hit removed" << std::endl;
    // clear the entry in the book keeping vector matched
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layX_max].size(); ++iAHit) {
      if (fActiveHitsPerLayer[layX_max].at(iAHit).GlobalStrawNumber()!=fCandidateHitsPerLayer[layX_max].at(iX_max).straw &&
          fActiveHitsPerLayer[layX_max].at(iAHit).GlobalStrawNumber()!=fCandidateHitsPerLayer[layX_max].at(iX_max).ActiveStraw) continue;
      DEBUG_OUT << "X: Reset fActiveHitsMatched" << std::endl;
      fActiveHitsMatched[layX_max].at(iAHit) = false;
      break;
    }
    // reset the candiate Hit
    fCandidateHitsPerLayer[layX_max].at(iX_max).ActiveStraw = -1;
    fCandidateHitsPerLayer[layX_max].at(iX_max).deDx        =  0;
    fCandidateHitsPerLayer[layX_max].at(iX_max).Chi2        = -1;
    for (unsigned int i=0; i<fCandidateHits.size(); i++) {
      if (fCandidateHits.at(i).straw != fCandidateHitsPerLayer[layX_max].at(iX_max).straw) continue;
      DEBUG_OUT << "X: Reset candidate Hit" << std::endl;
      fCandidateHitsPerLayer[layX_max].at(iX_max).xy          = fCandidateHits.at(i).xy;
      fCandidateHitsPerLayer[layX_max].at(iX_max).z           = fCandidateHits.at(i).z;
      fCandidateHitsPerLayer[layX_max].at(iX_max).pathLength  = fCandidateHits.at(i).pathLength;
      break;
    }
    HitRemoved = true;
  } else {
    DEBUG_OUT << "No Active Hits in X removed" << std::endl;
  }

  DEBUG_OUT << "iY_max=" << iY_max << " layY_max=" << layY_max << " Chi2Max_Y=" << Chi2Max_Y << " fChi2Y=" << fTrdCombinedFitYZ.Chi2 << std::endl;
  bool removeYhit = false;
  if (iY_max>=0 && fCandidateHitsPerLayer[layY_max].at(iY_max).ActiveStraw!=-1 && fCandidateHitsPerLayer[layY_max].size()>1) {
    if (fTrdCombinedFitYZ.Chi2>2.0 && Chi2Max_Y/fTrdCombinedFitYZ.Chi2>0.6) removeYhit = true;
    //if (Chi2Max_Y>20.0) removeYhit = true;
  }
  if (removeYhit) {
    DEBUG_OUT << "Y-Hit removed" << std::endl;
    // clear the entry in the book keeping vector matched
    for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layY_max].size(); ++iAHit) {
      if (fActiveHitsPerLayer[layY_max].at(iAHit).GlobalStrawNumber()!=fCandidateHitsPerLayer[layY_max].at(iY_max).straw &&
          fActiveHitsPerLayer[layY_max].at(iAHit).GlobalStrawNumber()!=fCandidateHitsPerLayer[layY_max].at(iY_max).ActiveStraw) continue;
      DEBUG_OUT << "Y: Reset fActiveHitsMatched" << std::endl;
      fActiveHitsMatched[layY_max].at(iAHit) = false;
      break;
    }
    // reset the candiate Hit
    fCandidateHitsPerLayer[layY_max].at(iY_max).ActiveStraw = -1;
    fCandidateHitsPerLayer[layY_max].at(iY_max).deDx        =  0;
    fCandidateHitsPerLayer[layY_max].at(iY_max).Chi2        = -1;
    for (unsigned int i=0; i<fCandidateHits.size(); i++) {
      if (fCandidateHits.at(i).straw != fCandidateHitsPerLayer[layY_max].at(iY_max).straw) continue;
      DEBUG_OUT << "Y: Reset candidate Hit" << std::endl;
      fCandidateHitsPerLayer[layY_max].at(iY_max).xy          = fCandidateHits.at(i).xy;
      fCandidateHitsPerLayer[layY_max].at(iY_max).z           = fCandidateHits.at(i).z;
      fCandidateHitsPerLayer[layY_max].at(iY_max).pathLength  = fCandidateHits.at(i).pathLength;
      break;
    }
    HitRemoved = true;
  } else {
    DEBUG_OUT << "No Active Hits in Y removed" << std::endl;
  }


  return HitRemoved;
}


bool ACsoft::Analysis::TrdCandidateMatching::AddActiveHits() {
  bool HitAdded = false;

  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
      TRDCandidateHit  CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
      if (CandidateHit.deDx>0 && CandidateHit.Chi2<2.0) continue;
      double Chi2_Best=1E99;

      int   iAHit_Best = -1;
      for (unsigned int iAHit=0; iAHit<fActiveHitsPerLayer[layer].size(); iAHit++) {
        if (fActiveHitsMatched[layer].at(iAHit)) continue;
        float Chi2 = fActiveHitsChi2[layer].at(iAHit);
        if (Chi2<Chi2_Best) {
          Chi2_Best  = Chi2;
          iAHit_Best = iAHit;
        }
      }
      if (iAHit_Best<0) continue;

      ACsoft::Analysis::TrdHit ActiveHit = fActiveHitsPerLayer[layer].at(iAHit_Best);

      double Chi2_Limit=10.0;
      if (CandidateHit.deDx>0) Chi2_Limit = CandidateHit.Chi2;
      Chi2_Limit = std::min(2.0,Chi2_Limit);
      bool AddHit = false;
      if (Chi2_Best<=Chi2_Limit) AddHit = true;
      if (abs(ActiveHit.GlobalStrawNumber()-CandidateHit.straw)<=1 && Chi2_Best<10.0) AddHit = true;
      DEBUG_OUT << "Best Active Hit for straw=" << CandidateHit.straw << " found: straw=" << ActiveHit.GlobalStrawNumber() << " Chi2_Best=" << Chi2_Best << " Chi2_Limit=" << Chi2_Limit << " AddHit=" << AddHit << std::endl;

      if (AddHit) {
        DEBUG_OUT << "Hit added: Chi2_Best=" << Chi2_Best << " iAHit_Best=" << iAHit_Best << std::endl;
        float dE = ActiveHit.GetAmplitude() * fTrdAdcScaleFactor;
        fCandidateHitsPerLayer[layer].at(iCHit).ActiveStraw = ActiveHit.GlobalStrawNumber();
        fCandidateHitsPerLayer[layer].at(iCHit).deDx        = dE;
        fCandidateHitsPerLayer[layer].at(iCHit).xy          = ActiveHit.R();
        fCandidateHitsPerLayer[layer].at(iCHit).z           = ActiveHit.Z();
        fActiveHitsMatched[layer].at(iAHit_Best) = true;
        HitAdded = true;
      } else {
        DEBUG_OUT << "No Hit added" << std::endl;
      }
    }
  }
  return HitAdded;
}

bool ACsoft::Analysis::TrdCandidateMatching::GenerateMissingHits(const ACsoft::Analysis::Particle& particle) {

  bool MissingHitsGenerated = false;
  const ACsoft::Analysis::SplineTrack* splineTrack = particle.GetSplineTrack();

  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    if (fActiveHitsPerLayer[layer].size()>0) continue;
    if (fMissingHitsPerLayer[layer].size()>0) continue;
    if (fCandidateHitsPerLayer[layer].size()!=1) continue;
    TRDCandidateHit CandidateHit1 = fCandidateHitsPerLayer[layer].at(0);

    TVector3        Point         = splineTrack->InterpolateToZ(CandidateHit1.z);
    float           MinDistance   = 2*AC::AMSGeometry::TRDTubeRadius + 0.05; // account for the 0.5mm stringer gap
    int             SecondStraw   = -1;
    TVector3        FirstPoint;
    TVector3        SecondPoint;

    FirstPoint.SetZ(CandidateHit1.z);
    FirstPoint.SetX(CandidateHit1.xy);
    FirstPoint.SetY(Point.Y());
    if (CandidateHit1.d==1) {
      FirstPoint.SetX(Point.X());
      FirstPoint.SetY(CandidateHit1.xy);
    }
    int direction = CandidateHit1.d;

    for (unsigned int i=0; i<CandidateHit1.v_NeighborStraws.size(); i++) {
      int   straw     = CandidateHit1.v_NeighborStraws.at(i);
      float xy        = CandidateHit1.v_NeighborXY.at(i);
      float z         = CandidateHit1.v_NeighborZ.at(i);
      float distance2 = xy-Point.X();
      if (direction==1) distance2 = xy-Point.Y();
      if (fabs(distance2)<MinDistance) {
        MinDistance     = distance2;
        SecondStraw     = straw;
        SecondPoint.SetZ(z);
        SecondPoint.SetX(xy);
        SecondPoint.SetY(Point.Y());
        if (direction==1) {
          SecondPoint.SetX(Point.X());
          SecondPoint.SetY(xy);
        }
      }
    }
    if (SecondStraw<0) continue;
    float x = 0.5*(FirstPoint.X()+SecondPoint.X());
    float y = 0.5*(FirstPoint.Y()+SecondPoint.Y());
    float z = 0.5*(FirstPoint.Z()+SecondPoint.Z());
    TRDMissingHit MissingHit = TRDMissingHit(x,y,z);
    MissingHit.straw1 = CandidateHit1.straw;
    MissingHit.straw2 = SecondStraw;
    MissingHit.d      = direction;
    float gap         = fabs(FirstPoint.X()-SecondPoint.X()) - 2*AC::AMSGeometry::TRDTubeRadius;
    if (direction==1) gap = fabs(FirstPoint.Y()-SecondPoint.Y()) - 2*AC::AMSGeometry::TRDTubeRadius;
    DEBUG_OUT << "Missing Hit added in layer=" << layer << " straw1=" << MissingHit.straw1 << " straw2=" << MissingHit.straw2 << " gap=" << gap << std::endl;
    MissingHit.sigma  = gap/sqrt(12.0);
    fMissingHitsPerLayer[layer].push_back(MissingHit);
    MissingHitsGenerated = true;
  }

  return MissingHitsGenerated;
}


void ACsoft::Analysis::TrdCandidateMatching::RecalculatePathLength(unsigned int direction) {

  TVector3 trackPos, trackDir;
  if (direction == 0) {
    trackPos = fTrdCombinedFitXZ.Position;
    trackDir = fTrdCombinedFitXZ.Direction;
  } else {
    trackPos = fTrdCombinedFitYZ.Direction;
    trackDir = fTrdCombinedFitYZ.Direction;
  }
  for (unsigned int layer=0; layer<AC::AMSGeometry::TRDLayers; layer++) {
    for (unsigned int iCHit=0; iCHit<fCandidateHitsPerLayer[layer].size(); iCHit++) {
      TRDCandidateHit CandidateHit = fCandidateHitsPerLayer[layer].at(iCHit);
      if (CandidateHit.d != (int)direction) continue;
      TVector3 StrawPos;
      StrawPos.SetZ(CandidateHit.z);
      if (direction == 0) {
        StrawPos.SetX(CandidateHit.xy);
      } else {
        StrawPos.SetY(CandidateHit.xy);
      }
      Double_t pathLength = pathlength3d(direction, trackPos, trackDir, StrawPos, AC::AMSGeometry::TRDTubeRadius);
      fCandidateHitsPerLayer[layer].at(iCHit).pathLengthTrdTrack = pathLength;
    }
  }

}

void ACsoft::Analysis::TrdCandidateMatching::TrackerPointInPlane1(const ACsoft::Analysis::Particle& particle) {

  fXfitLayer1        = particle.MainTrackerTrackFit()->XLayer1();
  fYfitLayer1        = particle.MainTrackerTrackFit()->YLayer1();
  fXhitLayer1        = 0;
  fYhitLayer1        = 0;
  fHitLayer1         = false;

  // FIXME: take formula for multiple scattering angle from PDG
  float         aRig                 = fabs(particle.MainTrackerTrackFit()->Rigidity());
  static double R1                   = 5.0, R2      = 50.0;
  static double deltaR1              = 1.0, deltaR2 =  0.05;
  float         MinDistanceLayer1    = std::max(deltaR2,deltaR2/(R2-R1)*(aRig-R1) + deltaR1/(R2-R1)*(R2-aRig));

  const AC::TrackerTrack* TRKtrk  = particle.MainTrackerTrack();
  const AC::TrackerTrack::ReconstructedHitsVector& rhits = TRKtrk->ReconstructedHits();
  for (unsigned int i=0; i<rhits.size(); ++i) {
    const AC::TrackerReconstructedHit& hit = rhits[i];
    if (hit.Layer()==1) {
      double distance = sqrt(pow(fXfitLayer1-hit.X(),2)+pow(fYfitLayer1-hit.Y(),2));
      if (distance<MinDistanceLayer1) {
        MinDistanceLayer1 = distance;
        fXhitLayer1       = hit.X();
        fYhitLayer1       = hit.Y();
        fHitLayer1        = true;
      }
    }
  }
  DEBUG_OUT << "fHitLayer1=" << fHitLayer1 << " fXhitLayer1=" << fXhitLayer1 << " fYhitLayer1=" << fYhitLayer1 << std::endl;

}

//
// iFlag=0: don't use pathlength normalisation
// iFlag=1: use pathlength from tracker track
// iFlag=2: use pathlength from TRD track
void ACsoft::Analysis::TrdCandidateMatching::CalculateDeDx(bool AddNearTrackHits, int iFlag ) {

  fNumberOfMatchedHits = 0;
  for (unsigned int iCHit=0; iCHit<fCandidateHits.size(); iCHit++) {
    TRDCandidateHit CandidateHit = fCandidateHits.at(iCHit);
    if (CandidateHit.deDx<=0)  continue;

    float pathLength = 0;
    if (iFlag==1) pathLength = CandidateHit.pathLength;
    else if (iFlag==2) pathLength = CandidateHit.pathLengthTrdTrack;
    if (pathLength<=0 && AddNearTrackHits) pathLength = fMeanPathLength;
    if (pathLength<=0) continue;

    if (iFlag>0) fCandidateHits.at(iCHit).deDx = CandidateHit.deDx/pathLength;
    fNumberOfMatchedHits++;
  }
  DEBUG_OUT << "fCandidateHits=" << fCandidateHits.size() << " fNumberOfMatchedHits=" << fNumberOfMatchedHits << std::endl;

}

bool ACsoft::Analysis::TrdCandidateMatching::Process(const ACsoft::Analysis::Particle& particle, bool AddNearTrackHits ) {

  fNumberOfMatchedHits = 0;
  fCandidateHits.clear();
  fAssignedHits.clear();
  fAssignedHits.reserve(40);
  fUnassignedHits.clear();
  fUnassignedHits.reserve(40);

  fMissingHitsPerLayer.clear();
  fMissingHitsPerLayer.assign(AC::AMSGeometry::TRDLayers, std::vector<ACsoft::Analysis::TRDMissingHit>());

  TrackerPointInPlane1(particle);

  bool IsInsideTRDGeometricalAcceptance = fPreselection.Process(particle, AddNearTrackHits);

  DetermineActiveHitsPerLayer(particle);

  DetermineCandidateHitsPerLayer();

  MatchActiveAndCandidateHits(particle, AddNearTrackHits);

  return IsInsideTRDGeometricalAcceptance && UsefulForTrdParticleId();
}

const std::vector<ACsoft::Analysis::TRDCandidateHit>& ACsoft::Analysis::TrdCandidateMatching::CandidateHits() const{
  return fCandidateHits;
}

bool ACsoft::Analysis::TrdCandidateMatching::UsefulForTrdParticleId() const {
  if(!fPreselection.PassesPreselectionCuts()) return false;
  if(fNumberOfMatchedHits < 12) return false;
  if(NumberOfUnassignedHits() >= 11) return false;
  return true;
}

unsigned short ACsoft::Analysis::TrdCandidateMatching::GetNumberOfActiveLayers() const {

  unsigned short TrdActiveLayer[AC::AMSGeometry::TRDLayers] = {0};
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const ACsoft::Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) {
      unsigned short layer = ACsoft::AC::TRDStrawToLayer(hit.straw);
      TrdActiveLayer[layer] = 1;
    }
  }

  unsigned short nTrdActiveLayers = 0;
  for (unsigned int i=0; i<AC::AMSGeometry::TRDLayers; i++)
    nTrdActiveLayers += TrdActiveLayer[i];
  return nTrdActiveLayers;
}

unsigned short ACsoft::Analysis::TrdCandidateMatching::GetNumberOfActiveStraws() const {

  unsigned short activeStraws = 0;
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const ACsoft::Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) ++activeStraws;
  }

  return activeStraws;
}


float ACsoft::Analysis::TrdCandidateMatching::GetActivePathLength() const {

  float pathLength = 0;
  for (unsigned int i=0; i<fCandidateHits.size(); i++) {
    const ACsoft::Analysis::TRDCandidateHit& hit = fCandidateHits[i];
    if (hit.deDx>0) pathLength += hit.pathLength;
  }

  return pathLength;
}

const ACsoft::Analysis::TrackFitResult& ACsoft::Analysis::TrdCandidateMatching::GetTrdTrackCombinedFit(unsigned int direction) const {

  return direction == 0 ? fTrdCombinedFitXZ : fTrdCombinedFitYZ;
}

const ACsoft::Analysis::TrackFitResult& ACsoft::Analysis::TrdCandidateMatching::GetTrdTrackStandAloneFit(unsigned int direction) const {

  return direction == 0 ? fTrdStandAloneFitXZ : fTrdStandAloneFitYZ;
}

const ACsoft::Analysis::TrackFitResult& ACsoft::Analysis::TrdCandidateMatching::GetTrkTrackFit(unsigned int direction) const {

  return direction == 0 ? fTrkFitXZ: fTrkFitYZ;
}

float ACsoft::Analysis::TrdCandidateMatching::GetTrdTrkAngle(unsigned int direction) const {

  float alphaTrd = 0, alphaTrk = 0;
  if (direction == 0) {
    alphaTrd = TMath::ATan2(fTrdStandAloneFitXZ.Direction.X(),fTrdStandAloneFitXZ.Direction.Z());
    alphaTrk = TMath::ATan2(fTrkFitXZ.Direction.X(),fTrkFitXZ.Direction.Z());
  } else if (direction == 1) {
    alphaTrd = TMath::ATan2(fTrdStandAloneFitYZ.Direction.Y(),fTrdStandAloneFitYZ.Direction.Z());
    alphaTrk = TMath::ATan2(fTrkFitYZ.Direction.Y(),fTrkFitYZ.Direction.Z());
  } else
    WARN_OUT << "Invalid value for direction= " << direction << "! Valid values are 0=XZ-Plane and 1=YZ-Plane." << std::endl;

  return alphaTrd - alphaTrk;
}

