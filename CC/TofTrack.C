// $Id$


#include "TofTrack.h"


#include "TMath.h"


ClassImp(TofTrack);


// correct final charge (from Z=3 to Z=8)
static float par_final[3]={0.9653,0.024876,-0.0024437};

// services
TSpline3* TofTrack::sqrtmip_sqrtmev_spline[2] = {0,0};
TSpline3* TofTrack::beta_top_of_tracker_layer[9] = {0,0,0,0,0,0,0,0,0};

TofTrack::TofTrack() { 
  // clear
  Clear();
}


TofTrack::TofTrack(TofClusterR* phit[], int nhits) { 
  // hit
  vector<TofClusterR*> hits; 
  for (int i=0; i<nhits; i++) hits.push_back(phit[i]); 
  Build(hits); 
}


TofTrack::TofTrack(vector<TofClusterR*> hits) { 
  // hit
  Build(hits); 
}


TofTrack::TofTrack(BetaR* beta, TrTrackR* trk_track, int trk_fit_id) {
  // check
  if (beta==0) {
    printf("TofTrack::Build-W invalid pointer to class beta.\n");
  } 
  // assignment and build
  vector<TofClusterR*> hits;
  for (int i=0; i<beta->NTofCluster(); i++) hits.push_back(beta->pTofCluster(i));
  Build(hits);
  // improvement in case of an associated track
  if ( (trk_track!=0)&&(trk_fit_id>=0) ) SetAssociatedTrTrack(trk_track,trk_fit_id);
  MakeDefaultFits();
}


bool TofTrack::Build(vector<TofClusterR*> hits) {
  // clean everything
  Clear();
  // too big/small number of hits?
  if ( (int(hits.size())>4)||(int(hits.size())<2) ) {
    printf("TofTrack::Build-W too big/small number of initializers (%d hits requested, minimum 2, maximum 4).\n",int(hits.size()));
    return false;
  }
  // zero pointers?
  for (int i=0; i<int(hits.size()); i++) {
    if (hits.at(i)==0) {
      printf("TofTrack::Build-W invalid pointer in the hit list (hit number %1d).\n",i);
      return false;
    }
  }
  // repeated layers?
  int layers[4] = {0,0,0,0};
  for (int i=0; i<int(hits.size()); i++) layers[hits.at(i)->Layer-1]++;
  for (int i=0; i<4; i++) {
    if (layers[i]>1) {
      printf("TofTrack::Build-W more than one hit on the same layer (layer %1d).\n",i+1);
      return false;
    }
  }
  // assignment
  _Hits = hits;
  // make default fits
  return MakeDefaultFits();
}


bool TofTrack::DropHit(int ii) {
  if (ii>=GetNhits()) return false;
  _Hits.erase(_Hits.begin()+ii);
  if (GetNhits()>0) MakeDefaultFits();
  return true;
}


char* PlaneName[2] = {"UTOF", "LTOF"};
void TofTrack::Print(int verbosity) {
  // general infos
  printf("TofTrack::Print-V nHhits %1d   Theta %8.5f   Phi %8.5f   Beta %8.5f   Chi2T %7.2f   Chi2XY %7.2f   Mean (%7.5f,%7.5f)   RelErr (%7.5f,%7.5f)   N (%1d,%1d)\n",
    GetNhits(),GetDir().gettheta(),GetDir().getphi(),GetBeta(),GetChiSqT(),GetChiSqXY(),
    GetMean(kAnode),GetMean(kDynode),GetRelErr(kAnode),GetRelErr(kDynode),GetNPoints(kAnode),GetNPoints(kDynode));
  // clusters infos
  for (int itof=0; itof<GetNhits(); itof++) {
    TofClusterR* cluster = GetHit(itof);
    if (cluster==0) continue;
    int layer = cluster->Layer; 
    if (verbosity>0) 
      printf("                  - TofCluster %1d belonging to track infos follow:\n",itof);
      printf("                  --- Layer %1d  Bar %2d  nRaw %1d  EDep %7.2f  EDepd %7.2f\n",
        cluster->Layer,cluster->Bar,cluster->NTofRawCluster(),cluster->Edep,cluster->Edepd
      );
    if (verbosity>1) 
      printf("                  --- A/D(MIP|MeV) = (%7.2f,%7.2f)  sqrtA/sqrtD(MIP|Beta|Path) = (%7.2f,%7.2f)\n",
        GetSignalLayer(layer,kAnode,kGain|kMIP|kMeV),
        GetSignalLayer(layer,kDynode,kGain|kMIP|kMeV),
        sqrt(GetSignalLayer(layer,kAnode,kGain|kBeta|kBetaAdd|kPath|kMIP)),
        sqrt(GetSignalLayer(layer,kDynode,kGain|kBeta|kBetaAdd|kPath|kMIP))
      );
    if (verbosity>2) 
      printf("                  --- ResX(cm) %10.5f  ResY(cm) %10.5f  ResT(ns) %10.5f\n",
        GetSpatialResidual(layer).x(),GetSpatialResidual(layer).y(),GetTimeResidual(layer)
      );
  }
}


bool TofTrack::MakeDefaultFits() {
  if (GetNhits()<1) {
    printf("TofTrack::MakeMakeDefaultFits-W no hits on TofTrack. Quit calculation.\n");
    return false;
  }
  MakeSpatialFitInGaussianApprox();
  MakeSpatialResiduals();
  MakeTimeFit();
  MakeTimeResiduals();
  MakeEdep();
  MakePathLength();
  MakeMean(kAnode);
  MakeMean(kDynode);
  MakeMean(kMix);
  if (AnAssociatedTrTrackExist()) MakeAssociatedTrTrackChiSqAndResiduals();
  return true;
}


void TofTrack::Clear() {
  _Hits.clear();
  for (int i=0; i<2; i++) { 
    _SpatialChiSq[i] = 0; 
    _SpatialNdof[i] = 0;
  }
  _Point.setp(0,0,0);
  _Dir.setp(0,0,0);
  for (int i=0; i<4; i++) _SpatialResiduals[i] = 0;
  _Beta = -1000;
  _ErrInvBeta = 0;
  _Offset = 0;
  _ChiSqT = 0;
  _NdofT = 0;
  for (int i=0; i<4; i++) _TimeResiduals[i] = 0;
  for (int itype=0; itype<2; itype++) 
    for (int ilayer=0; ilayer<4; ilayer++) 
      _EdepRaw[itype][ilayer] = 0;
  for (int ilayer=0; ilayer<4; ilayer++) 
    _PathLength[ilayer] = 0;
  for (int iside=0; iside<2; iside++) {
    _Mean[iside] = 0;
    _RMS[iside] = 0;
    _NPoints[iside] = 0;
  }
  _TrTrack = 0;
  _Id = -1;
  for (int iside=0; iside<2; iside++) {
    _TrackChiSq[iside] = 0;
    _TrackNdof[iside] = 0;
  }
  for (int i=0; i<4; i++) _TrackResiduals[i] = 0;
}


void TofTrack::SetAssociatedTrTrack(TrTrackR* trk_track, int trk_fit_id) { 
  if ( (trk_fit_id<0)||(trk_track==0) ) return; 
  _TrTrack = trk_track; 
  _Id = trk_fit_id; 
}


int TofTrack::GetPattern() {
  int pattern = 0;
  for (int ilay=1; ilay<=4; ilay++) {
    if (GetHitLayer(ilay)) pattern|=(0x1<<(ilay-1));
  } 
  return pattern; 
}


int TofTrack::GetPatternChoumilov() {
  if (GetNhits()==4) return 0;
  for (int ilay=1; ilay<=4; ilay++) {
    if (GetHitLayer(ilay)==0) return ilay;
  } 
  return -1; // fail
}


TofClusterR* TofTrack::GetHitLayer(int layer) {
  for (int itof=0; itof<GetNhits(); itof++) {
    TofClusterR* cluster = GetHit(itof);
    if (cluster->Layer==layer) return cluster;
  } 
  return 0;
}


bool TofTrack::CheckPatterLayer(int layer) {
  if ( (layer<1)||(layer>4) ) return false; // warning needed
  return GetPattern()&(0x1<<(layer-1));
}


AMSPoint TofTrack::GetSpatialResidual(int layer) {
  TofClusterR* cluster = GetHitLayer(layer);
  if (cluster==0) return AMSPoint();
  return _SpatialResiduals[layer-1];
}


// coordinates of the edges of each tracker plane
static float tracker_layers_z[9] = {158.920,53.060,29.228,25.212,1.698,-2.318,-25.212,-29.228,-135.882};
static float tracker_planes_edges[9][4] = {
  {-62.14,  -47.40,   62.14,   47.40},
  {-62.14,  -40.10,   62.14,   40.10},
  {-49.70,  -43.75,   49.70,   43.75},
  {-49.72,  -43.75,   49.72,   43.75},
  {-49.71,  -36.45,   49.70,   36.45},
  {-49.72,  -36.45,   49.72,   36.45},
  {-49.72,  -43.75,   49.71,   43.75},
  {-49.72,  -43.75,   49.71,   43.75},
  {-45.62,  -29.48,   45.55,   29.53}
};
int TofTrack::GetPatternInsideTracker(float margin) {
  int pattern = 0;
  for (int ilayer=0; ilayer<9; ilayer++) {
    AMSPoint tof_point = GetPoint(tracker_layers_z[ilayer]);
    bool isinlayer = false;
    // rectangle
    if ( (tof_point.x()>tracker_planes_edges[ilayer][0]+margin)&&
         (tof_point.x()<tracker_planes_edges[ilayer][2]-margin)&&
         (tof_point.y()>tracker_planes_edges[ilayer][1]+margin)&&
         (tof_point.y()<tracker_planes_edges[ilayer][3]-margin) ) {
      if ((ilayer+1)==9) isinlayer = true;
      else {
        // circle 
        if ( (sqrt(tof_point.x()*tof_point.x()+tof_point.y()*tof_point.y())<
              tracker_planes_edges[ilayer][2]-margin) )
          isinlayer = true;
      }
    }
    if (isinlayer) pattern |= (1<<ilayer);
  }
  return pattern;
}


float TofTrack::GetTimeResidual(int layer) {
  TofClusterR* cluster = GetHitLayer(layer);
  if (cluster==0) return 0;
  return _TimeResiduals[layer-1];
}


bool TofTrack::MakeSpatialFitWithOnlyLongitudinalCoordinates() {
  // init
  _Point.setp(0,0,0);
  _Dir.setp(0,0,0);
  // 4 hits
  if (GetNhits()!=4) {
    printf("TofTrack::MakeSpatialFitWithOnlyLongitudinalCoordinates-W tracks with 4 hits requested.\n");
    return false;
  }
  // one hit on each layer
  bool layer[4] = {false,false,false,false};
  for (int itof=0; itof<GetNhits(); itof++) {
    TofClusterR* cluster = GetHit(itof);
    layer[cluster->Layer-1] = true;
  } 
  if ( (GetHitLayer(1)==0)||
       (GetHitLayer(2)==0)||
       (GetHitLayer(3)==0)||
       (GetHitLayer(4)==0) ) { 
    printf("TofTrackR::MakeSpatialFitWithOnlyLongitudinalCoordinates-W requested one hit on each layer.\n");
    return false;
  } 
  // definition
  float mm[2] = {0,0}; 
  float yy[2] = {0,0};
  int   l1[2] = {1,2};
  int   l2[2] = {4,3};
  for (int itof=0; itof<2; itof++) {
    float x1 = GetHitLayer(l1[itof])->Coo[2];
    float y1 = GetHitLayer(l1[itof])->Coo[itof];
    float x2 = GetHitLayer(l2[itof])->Coo[2];
    float y2 = GetHitLayer(l2[itof])->Coo[itof];
    mm[itof] = (y2 - y1)/(x2 - x1);
    yy[itof] = y1 - mm[itof]*x1;
  }
  float cz = 1./sqrt(1 + pow(1.*mm[0],2.) + pow(1.*mm[1],2.));
  float cx = cz*mm[0];
  float cy = cz*mm[1];
  float x  = yy[0];
  float y  = yy[1]; 
  _Point.setp(x,y,0);
  _Dir.setp(cx,cy,cz);
  return true;
}


bool TofTrack::MakeSpatialFitInGaussianApprox() {
  // init
  for (int i=0; i<2; i++) {
    _SpatialChiSq[i] = 0;
    _SpatialNdof[i] = 0;
  }
  _Point.setp(0,0,0);
  _Dir.setp(0,0,0);
  // fit
  int nhit = GetNhits();
  float mm[2] = {0,0};
  float yy[2] = {0,0};
  float chi2[2] = {0,0};
  int   ndof[2] = {0,0};
  for (int icoord=0; icoord<2; icoord++) {
    double sw  = 0.;
    double sx  = 0.;
    double sx2 = 0.;
    double sy  = 0.;
    double sxy = 0.;
    for (int ihit=0; ihit<nhit; ihit++){
      double x = GetHit(ihit)->Coo[2];
      double y = GetHit(ihit)->Coo[icoord];
      double s = GetHit(ihit)->ErrorCoo[icoord]; 
      double w = 1/(s*s);
      sw  += w;
      sx  += x*w;
      sx2 += x*x*w;
      sy  += y*w;
      sxy += x*y*w;
    }
    double delta = sw*sx2 - sx*sx;
    mm[icoord] = (sw*sxy-sx*sy)/delta;
    yy[icoord] = (sx2*sy-sx*sxy)/delta;
    // chisq computation
    for (int ihit=0; ihit<nhit; ihit++) {
      double x = GetHit(ihit)->Coo[2];
      double y = GetHit(ihit)->Coo[icoord];
      double s = GetHit(ihit)->ErrorCoo[icoord];
      double i = yy[icoord] + x*mm[icoord];
      chi2[icoord] += pow(1.*(y-i)/s,2.);
    }
    ndof[icoord] = nhit-2;
  }
  // vars
  float cz = 1./sqrt(1 + pow(1.*mm[0],2.) + pow(1.*mm[1],2.));
  float cx = cz*mm[0];
  float cy = cz*mm[1];
  float x  = yy[0];
  float y  = yy[1];
  _Point.setp(x,y,0);
  _Dir.setp(cx,cy,cz);
  _SpatialChiSq[0] = chi2[0];
  _SpatialNdof[0]  = ndof[0];
  _SpatialChiSq[1] = chi2[1];
  _SpatialNdof[1]  = ndof[1];
  return true;
}


bool TofTrack::MakeSpatialFitMinimization() {
  return true;
}


bool TofTrack::MakeSpatialResiduals() {
  // init
  for (int i=0; i<4; i++) _SpatialResiduals[i] = 0;
  // calculate spatial residuals
  int nhit = GetNhits();
  for (int i=0; i<4; i++) _SpatialResiduals[i] = 0;
  for (int ihit=0; ihit<nhit; ihit++) {
    TofClusterR* cluster = (TofClusterR*) GetHit(ihit);
    if (!cluster) continue;
    int ilayer = cluster->Layer - 1;
    AMSPoint clu_point(cluster->Coo[0],cluster->Coo[1],cluster->Coo[2]);
    AMSPoint point;
    AMSDir dir;
    Interpolate(clu_point.z(),point,dir);
    _SpatialResiduals[ilayer] = clu_point - point;
  }
  return true;
}


bool TofTrack::MakePathLength() {
  // init
  for (int ilayer=0; ilayer<4; ilayer++) 
    _PathLength[ilayer] = 0;
  // calculation
  for (int ilayer=0; ilayer<4; ilayer++) {
    TofClusterR* cluster = (TofClusterR*) GetHitLayer(ilayer+1);
    if (!cluster) continue;
    float z = cluster->Coo[2];
    for (int irc=0; irc<cluster->NTofRawCluster(); irc++){
      TofRawClusterR* rawcluster = cluster->pTofRawCluster(irc);
      int ibar = rawcluster->Bar - 1;
      AMSPoint hit_point;
      AMSDir hit_dir;
      Interpolate(z,hit_point,hit_dir); 
      float length = TofGeometry::LengthInsideCounter(ilayer,ibar,hit_point.x(),hit_point.y(),hit_dir); // contin, simple un-aligned routine
      // float length = TofGeometry::PathLengthInAPaddle(ilayer,ibar,hit_point,hit_dir)); // mine, less precise for trapezoidal counters
      _PathLength[ilayer] += (length>0) ? length : 0.;
    } 
  }
  return true;
}


float TofTrack::GetPathLengthEasy(int ilayer) {
  TofClusterR* cluster = (TofClusterR*) GetHitLayer(ilayer+1);
  if (!cluster) return 0;
  float z = cluster->Coo[2];
  AMSPoint hit_point;
  AMSDir hit_dir;
  Interpolate(z,hit_point,hit_dir);
  return cluster->NTofRawCluster()/cos(hit_dir.gettheta());
}


AMSPoint TofTrack::GetPoint(float z) {
  float tanxz = GetDir().x()/GetDir().z();
  float tanyz = GetDir().y()/GetDir().z();
  float x = GetPoint().x() + tanxz*z;
  float y = GetPoint().y() + tanyz*z;
  return AMSPoint(x,y,z);
}


float TofTrack::Interpolate(float z, AMSPoint& point, AMSDir& dir) {
  float sign = -z/fabs(z);
  float len = -1;
  if (AnAssociatedTrTrackExist()) {
    // Tracker track
    len = sign*GetAssociatedTrTrack()->Interpolate(z,point,dir,GetAssociatedTrTrackFitId());
    return len;
  }
  // TOF default track 
  point = GetPoint(z);
  dir = GetDir();
  len = sign*(GetPoint() - point).norm();
  return len; 
}


float TofTrack::GetLength(float z) {
  AMSPoint tmp_point;
  AMSDir   tmp_dir;
  return Interpolate(z,tmp_point,tmp_dir);
}


bool TofTrack::MakeTimeFit(char mask) {
  // init
  _Beta = -1000;
  _ErrInvBeta = 0;
  _Offset = 0;
  _ChiSqT = 0;
  _NdofT = 0;
  // fit
  int nhit = GetNhits();
  // check mask
  if ( (mask==0)|| // no layers
       (mask==0x1)||(mask==0x2)||(mask==0x4)||(mask==0x8)|| // just one layer 
       (mask==0x3)||(mask==0xC) // upper or lower couples of layers
     ) {
    printf("TofTrack::MakeTimeFit-W requested mask is forbidden (no layers, only one layer, only upper or lower TOF)\n");
    return false;
  }
  // check number of clusters in mask
  int nhit_mask = 0;
  for (int ihit=0; ihit<nhit; ihit++) {
    TofClusterR* cluster = GetHit(ihit);
    if (((mask>>(cluster->Layer-1))&0x1)==0) continue;
    nhit_mask++;
  }
  if (nhit_mask<2) {
    // printf("TofTrack::MakeTimeFit-W not enough hits in the mask.\n"); // this could be frequent
    return false;
  }
  // calculation
  int    n   = 0;
  double sw  = 0.;
  double sx  = 0.;
  double sx2 = 0.;
  double sy  = 0.;
  double sxy = 0.;
  AMSPoint trk_point;
  AMSDir   trk_dir;
  for (int ihit=0; ihit<nhit; ihit++) {
    TofClusterR* cluster = GetHit(ihit);
    if (((mask>>(cluster->Layer-1))&0x1)==0) continue; 
    n++;
    double z = cluster->Coo[2];
    double x = GetLength(z);
    double y = cluster->Time*1e+9;    // ns
    double s = cluster->ErrTime*1e+9; // ns
    double w = 1./(s*s);
    sw  += w;
    sx  += x*w;
    sx2 += x*x*w;
    sy  += y*w;
    sxy += x*y*w;
  }
  double delta = sw*sx2 - sx*sx;
  double mm = (sw*sxy-sx*sy)/delta;
  double em = sqrt(sw/delta);
  double yy = (sx2*sy-sx*sxy)/delta;
  // chi2 
  double chi2 = 0;
  for (int ihit=0; ihit<nhit; ihit++) {
    TofClusterR* cluster = GetHit(ihit);
    if (((mask>>(cluster->Layer-1))&0x1)==0) continue; 
    double z = cluster->Coo[2];
    double x = GetLength(z);
    double y = cluster->Time*1e+9;    // ns
    double s = cluster->ErrTime*1e+9; // ns
    double i = yy + x*mm;
    chi2 += pow(1.*(y-i)/s,2.);
    // if (n==3) printf("i %1d z %10.3f x %10.3f y%10.3f\n",ihit,z,x,y);
  }
  _Beta = 1./mm/c_speed; 
  _ErrInvBeta = em*c_speed;
  _Offset = yy; // ns
  _ChiSqT = chi2;
  _NdofT = n-2;  
  // cout << _Beta << " " << _ErrInvBeta << " " << _Beta*_ErrInvBeta*100 << " " << _Offset << " " << GetPattern() << endl;
  return true;
}


bool TofTrack::MakeTimeResiduals() { 
  // init
  for (int i=0; i<4; i++) _TimeResiduals[i] = 0;
  // calculate residuals
  int nhit = GetNhits();
  for (int ihit=0; ihit<nhit; ihit++) {
    TofClusterR* cluster = (TofClusterR*) GetHit(ihit);
    if (!cluster) continue;
    int ilayer = cluster->Layer - 1;
    float t0 = cluster->Time*1e+9;
    float t1 = GetTime(cluster->Coo[2]);
    _TimeResiduals[ilayer] = t0-t1;
  }
  return true; 
}


float TofTrack::GetTime(float z) { 
  float len = GetLength(z);
  return GetOffset() + len/(GetBeta()*c_speed); 
}


bool TofTrack::MakeMean(int type, int mean_opt, int sig_opt, float mass_on_Z) {
  // init
  _Mean[type] = 0;
  _RMS[type] = 0;
  _NPoints[type] = 0;
  // calculation
  if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return false;
  int   nhit = GetNhits();
  int   npoints = 0;
  float higher = 0; 
  float mean = 0;
  float rms = 0;
  for (int ihit=0; ihit<nhit; ihit++) {
  int layer = GetHit(ihit)->Layer;
    float signal = GetSignalLayer(layer,type,sig_opt,mass_on_Z);
    if (signal<1e-06) continue;
    if (mean_opt&kSqrt) {
      mean += sqrt(signal);
      rms += signal;
      if (sqrt(signal)>higher) { higher = sqrt(signal); }
    }
    else {
      mean += signal;
      rms += signal*signal;
      if (signal>higher) { higher = signal; }
    }
    npoints++;
    // cout << npoints << " " << ihit << " " << signal << " " << mean << " " << rms << endl;
  }
  // calculation
  if (mean_opt&kPlain) {
    if (npoints<1) {
      // printf("TofTrack::MakeMean-W number of points less than 1, is not possible to make a mean.\n");
      _Mean[type] = 0;
      _RMS[type] = 0;
      _NPoints[type] = 0;
      return false;
    }
    mean /= npoints;
    rms  /= npoints;
  }
  else {
    if (npoints<2) {
      // printf("TofTrack::MakeMean-W number of points less than 2, is not possible to make a truncated mean.\n");
      _Mean[type] = 0;
      _RMS[type] = 0;
      _NPoints[type] = 0;
      return false;
    }
    mean -= higher;
    rms  -= higher*higher;
    npoints--;
    mean /= npoints;
    rms  /= npoints;
  } 
  rms = sqrt(fabs(mean*mean - rms));
  _Mean[type]    = mean;
  _RMS[type]     = rms;
  _NPoints[type] = npoints; 
  // cout << type << " ----------------> " << mean << " " << rms << " " << npoints << endl;
  // kContin
  // COME PROPAGO LA RMS, POSSO APPLICARE QUESTO AI PIANI? PROBABILMENTE SI
  // if(_Charge>1.669 && _Charge<8.511) _Charge=_Charge/(par_final[0]+par_final[1]*_Charge+par_final[2]*_Charge*_Charge);

  return true;
}


bool TofTrack::IsGoodAnodeForCharge(TofRawClusterR* rawcluster) {
  if (!rawcluster) return false;
  bool goodano = true;
  for (int iside=0; iside<2; iside++) {
    if (rawcluster->adca[iside]<1e-06) goodano = false;   
  } 
  float edep = 0;
  if (goodano) edep = rawcluster->edepa; 
  return (edep>1e-06);
}


bool TofTrack::IsGoodAnodeForCharge(TofClusterR* cluster) {
  if (!cluster) return false;
  bool goodano = true;
  for (int index=0; index<cluster->NTofRawCluster(); index++) {
    TofRawClusterR* rawcluster = cluster->pTofRawCluster(index);
    if (!rawcluster) return false;
    goodano = goodano && IsGoodAnodeForCharge(rawcluster);
  }
  return goodano;
}


// number of dynodes on TOF planes
static int ndynodes[4][10] = {
  {3,2,2,2,2,2,2,3,0,0},
  {2,2,2,2,2,2,2,2,0,0},
  {2,2,2,2,2,2,2,2,2,2},
  {3,2,2,2,2,2,2,3,0,0}
};
bool TofTrack::IsGoodDynodeForCharge(TofRawClusterR* rawcluster) {
  if (!rawcluster) return false;
  // Condition on dynode measurement (from Contin)
  // check that all dynodes (2 or 3 per side) of that counter have a valid measurement
  int ilayer = rawcluster->Layer - 1;
  int ibar = rawcluster->Bar - 1;
  // counting number of dynode measurements per side
  bool gooddyn = true;
  int  ngooddyn[2] = {0,0};
  for (int iside=0; iside<2; iside++) {
    for (int idyn=0; idyn<3; idyn++) {
      if (rawcluster->adcdr[iside][idyn]>1e-06) ngooddyn[iside]++;
    }
    if ( (ilayer==2)&&(ibar==0)&&(iside==1) ) continue; // this one is ok (calibration done with 1 side only)
    if (ngooddyn[iside]<ndynodes[ilayer][ibar]) gooddyn = false;
  }
  float edep = 0;
  if (gooddyn) edep = rawcluster->edepd;
  return (edep>1e-06);
}


bool TofTrack::IsGoodDynodeForCharge(TofClusterR* cluster) {
  if (!cluster) return false;
  bool gooddyn = true;
  for (int index=0; index<cluster->NTofRawCluster(); index++) {
    TofRawClusterR* rawcluster = cluster->pTofRawCluster(index);
    if (!rawcluster) return false;
    gooddyn = gooddyn && IsGoodDynodeForCharge(rawcluster);
  }
  return gooddyn;
}


bool TofTrack::MakeEdep() {
  // init
  for (int itype=0; itype<2; itype++) 
    for (int ilayer=0; ilayer<4; ilayer++) 
      _EdepRaw[itype][ilayer] = 0;
  // prepare values
  for (int itof=0; itof<GetNhits(); itof++) {
    TofClusterR* cluster = (TofClusterR*) GetHit(itof); 
    if (!cluster) continue;
    int ilayer = cluster->Layer-1;
    // give 0 in case of bad charge measurement
    bool goodano = IsGoodAnodeForCharge(cluster);
    bool gooddyn = IsGoodDynodeForCharge(cluster);
    // Anode
    if (goodano) {
      for (int index=0; index<cluster->NTofRawCluster(); index++) {
        TofRawClusterR* rawcluster = (TofRawClusterR*) cluster->pTofRawCluster(index);
        if (!rawcluster) continue;
        float edep_raw = rawcluster->edepa;
        _EdepRaw[0][ilayer] += edep_raw; 
      }   
    }
    // Dynode
    if (gooddyn) {
      for (int index=0; index<cluster->NTofRawCluster(); index++) {
        TofRawClusterR* rawcluster = (TofRawClusterR*) cluster->pTofRawCluster(index);
        if (!rawcluster) continue;
        float edep_raw = rawcluster->edepd;
        _EdepRaw[1][ilayer] += edep_raw;   
      } 
    }
  }
  return true; 
}


float TofTrack::GetSignalLayer(int layer, int type, int sig_opt, float mass_on_Z) { 
  if ( (layer<1)||(layer>4) ) return 0;
  if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return 0;
  TofClusterR* cluster = (TofClusterR*) GetHitLayer(layer);
  if (!cluster) return 0;

  float edep = _EdepRaw[type][layer-1];
  int   bar  = cluster->Bar; 

  /////////////////////////
  // Gain correction 
  /////////////////////////

  // remark: in Contin approach the gain are applied after mip correction
  if ( (sig_opt&kGain)&&(type!=kMix)&&(!(sig_opt&kContin)) ) {
    edep = GainCorrection(edep,layer,bar,type);
  }

  /////////////////////////
  // MIP correction (mixing also belongs to this)
  /////////////////////////

  if (sig_opt&kMIP) {
    // if just one side calculate it
    if (type!=kMix) { 
      if (sig_opt&kContin) edep *= MipCorrectionContin(edep,layer,bar,type);  
      else                 edep *= MipCorrection(edep,type);
    }
    // if mix proceed with mixing
    else { 
      float edep_a = _EdepRaw[0][layer-1];
      float edep_d = _EdepRaw[1][layer-1];
      if (sig_opt&kGain) {
        edep_a = GainCorrection(edep_a,layer,bar,kAnode);
        edep_d = GainCorrection(edep_d,layer,bar,kDynode);
      }
      if (sig_opt&kContin) { 
        edep_a *= MipCorrectionContin(edep_a,layer,bar,kAnode);
        edep_d *= MipCorrectionContin(edep_d,layer,bar,kDynode);
      }
      else {
        edep_a *= MipCorrection(edep_a,kAnode);
        edep_d *= MipCorrection(edep_d,kDynode);
      }
      // first simple guess
      edep = edep_a;
      if ( (edep_a>3.5*3.5)&&(edep_d>0) ) edep = edep_d;
      // iterative procedure (deact.)
      for (int iiter=0; iiter<0; iiter++) {
        // get sigma 
        float sigma_a = GetMipResolution(edep,kAnode);
        float sigma_d = GetMipResolution(edep,kDynode);
        // evaluate weights
        float w_a = 1./(sigma_a*sigma_a)/(1./(sigma_a*sigma_a) + 1./(sigma_d*sigma_d));
        float w_d = 1./(sigma_d*sigma_d)/(1./(sigma_a*sigma_a) + 1./(sigma_d*sigma_d));
        // update mip value
        edep = edep_a*w_a + edep_d*w_d;
      }
    }
  }

  /////////////////////////
  // MeV correction (only if MIP is already applied)
  /////////////////////////

  if (sig_opt&kMeV) edep *= 1.67; 

  /////////////////////////
  // Pathlength correction
  /////////////////////////
 
  if (sig_opt&kPath) edep *= PathLengthCorrection(layer); 
  
  /////////////////////////
  // Beta correction
  /////////////////////////
 
  if (sig_opt&kContin) {
    edep /= BetaCorrectionContin1(type);
  }
  else { 
    float betagamma_corr = 1;
    if      ( (kBeta&sig_opt)&&(kRigidity&sig_opt) ) betagamma_corr = BetaRigidityCorrection(layer,mass_on_Z);
    else if (kBeta&sig_opt)                          betagamma_corr = BetaCorrection(layer);
    else if (kRigidity&sig_opt)                      betagamma_corr = RigidityCorrection(layer,mass_on_Z);
    if (betagamma_corr<=0.) betagamma_corr = 1;
    edep /= betagamma_corr;

    // apply only with just beta ... 
    if ((kBeta&sig_opt)&&(kBetaAdd&sig_opt)) edep = AdditionalBetaCorrection(edep);
  }

  return edep;
}


// Gain corrections derived from charge fits (13/03/2012, from data of 21 - 27 Jul 2011)
// - 1 to 5 for Anode
// - 3 to 26 for Dynode 
// - weighted average taken as reference
static float gain[2][4][10] = {
  {
    { 1.05146, 0.99696, 0.98649, 0.97592, 1.03980, 1.05058, 1.00354, 1.09879, 0.00000, 0.00000},
    { 1.04739, 0.92608, 0.98702, 1.03066, 0.99747, 0.98353, 0.96229, 1.02770, 0.00000, 0.00000},
    { 0.98343, 0.90052, 0.95432, 0.99632, 0.96770, 0.99568, 1.10881, 1.03183, 1.03971, 0.99537},
    { 1.00598, 0.97466, 0.98498, 0.96357, 0.95598, 0.97826, 0.94580, 1.00056, 0.00000, 0.00000}
  },
  {
    { 0.92576, 1.11303, 0.99665, 1.01248, 1.03516, 0.93332, 1.03060, 0.96570, 0.00000, 0.00000},
    { 1.05242, 1.05529, 1.03865, 1.05059, 0.92349, 0.94953, 1.03178, 1.03404, 0.00000, 0.00000},
    { 0.77197, 0.95997, 0.97525, 1.04559, 1.01314, 1.04504, 1.00056, 1.03697, 1.00661, 0.78773},
    { 0.93688, 1.06749, 1.04388, 1.04277, 0.99129, 1.02205, 1.02473, 0.94377, 0.00000, 0.00000}
  }
};
static float offset[2][4][10] = {
  {
    {-0.09558,-0.00476, 0.05336, 0.06116,-0.05336,-0.05478,-0.00903,-0.16561, 0.00000, 0.00000},
    {-0.06577, 0.11539, 0.02307,-0.08126, 0.02514, 0.03197, 0.08406,-0.05003, 0.00000, 0.00000},
    { 0.07012, 0.16754, 0.06834, 0.04464, 0.06109, 0.02737,-0.15619,-0.05296,-0.07611,-0.20588},
    {-0.01492, 0.05953, 0.00582, 0.03310, 0.04998,-0.00512, 0.08839,-0.07051, 0.00000, 0.00000}
  },
  {
    {-2.28623, 0.82704,-0.39660, 1.39577, 0.45081,-0.41171, 0.47342,-1.33278, 0.00000, 0.00000},
    {-1.92190, 0.64258, 0.83392, 0.16555,-0.13895,-0.08189, 0.58053,-1.20940, 0.00000, 0.00000},
    { 0.43416, 1.38068, 0.07189, 0.83665, 0.78576, 0.44166, 0.70979, 1.47709,-0.05613,-0.94480},
    {-1.81669, 1.43083, 0.25532,-0.29895, 0.24762, 0.65398, 0.82261,-3.08559, 0.00000, 0.00000}
  }
};
/* 
// OLD (Nov 2011)
static float gain[2][4][10] = {
  {
    { 1.06706, 0.99886, 0.98243, 0.96850, 1.01227, 1.02646, 1.01055, 1.10529, 0.00000, 0.00000},
    { 1.05487, 0.95663, 0.97557, 0.99726, 0.98373, 0.95860, 0.97390, 1.05268, 0.00000, 0.00000},
    { 1.09080, 0.93329, 0.96787, 0.99936, 0.95677, 0.97454, 1.05479, 1.02358, 1.03741, 1.02835},
    { 1.05720, 0.97355, 0.98157, 0.93337, 0.94642, 0.96097, 0.95131, 1.01680, 0.00000, 0.00000},
  },
  {
    { 0.95315, 1.10532, 0.97885, 1.00038, 1.02502, 0.92870, 1.01835, 0.99838, 0.00000, 0.00000},
    { 1.05256, 1.03358, 1.02744, 1.04505, 0.91943, 0.94239, 1.03299, 1.04784, 0.00000, 0.00000},
    { 0.80671, 0.95313, 0.95947, 1.03418, 0.99582, 1.03020, 0.99448, 1.05605, 1.01778, 0.83308},
    { 0.97272, 1.08592, 1.02533, 0.99888, 0.97577, 1.00064, 1.03175, 0.94114, 0.00000, 0.00000},
  }
};
static float offset[2][4][10] = {
  {
    {-0.04117,-0.02352, 0.01789, 0.03275,-0.04914,-0.05182,-0.00870,-0.09358, 0.00000, 0.00000},
    {-0.03501, 0.05724, 0.02435,-0.02412,-0.00142, 0.06583, 0.04825,-0.04651, 0.00000, 0.00000},
    {-0.05384, 0.13734, 0.04686, 0.01950, 0.05789, 0.03666,-0.06960,-0.03695,-0.05181,-0.18229},
    {-0.03386, 0.03475,-0.03483, 0.05900, 0.03194,-0.00364, 0.08404, 0.02923, 0.00000, 0.00000},
  },
  {
    {-2.36567, 1.38597,-0.01381, 0.73685, 0.17202,-0.92961, 0.82465,-1.47262, 0.00000, 0.00000},
    {-1.34966, 1.81403, 1.26250, 0.34617,-0.21597, 0.10139, 0.50848,-1.29763, 0.00000, 0.00000},
    { 0.60658, 1.98548, 0.74123, 1.02645, 1.07772, 1.04145, 0.27905, 0.25338,-0.24944,-1.18985},
    {-1.47328, 0.83810, 0.19836, 0.57684, 0.21271, 0.96655, 0.51702,-1.97556, 0.00000, 0.00000},
  }
};
*/
float TofTrack::GainCorrection(float edep, int layer, int bar, int type) {
  if ( (type!=kAnode)&&(type!=kDynode) ) return 0;
  if ( (layer>4)||(layer<1) ) return 0;
  if ( (bar<1)||(bar>10) ) return 0; 
  if (edep<1e-6) return 0; 
  return (edep-offset[type][layer-1][bar-1])/gain[type][layer-1][bar-1]; 
}


// pathlength correction (without alignment)
float TofTrack::PathLengthCorrection(int layer) {
  float path = GetPathLength(layer);
  if (path<=0.0) return 0.;
  if (path< 1.0) return 1.;
  if (path< 1.5) return 1/path;
  if (path< 2.0) return 0.5;
  return 1/path;
}


// sqrt(MeV) to sqrt(N. MIP) = saturation+Birks (15/03/2012)
static Int_t    npoints_a = 14;
static Double_t sqrtmip_a[14] = {  0.000000,  1.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,  7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000};
static Double_t sqrtmev_a[14] = {  0.000000,  1.294222,  2.603823,  3.692677,  4.617331,  5.350646,  5.992558,  6.519936,  6.985784,  7.793730,  8.378469,  8.825577, 10.321602, 13.313652};
static Int_t    npoints_d = 13;
static Double_t sqrtmip_d[13] = {  0.000000,  2.000000,  3.000000,  4.000000,  5.000000,  6.000000,  7.000000,  8.000000, 10.000000, 12.000000, 14.000000, 26.000000, 50.000000};
static Double_t sqrtmev_d[13] = {  0.000000,  2.414691,  3.640587,  4.808496,  5.926581,  6.880665,  7.810671,  8.680826, 10.259000, 11.680345, 13.001746, 20.513462, 35.536894};
/* OLD (Nov 2011)
static Int_t    npoints_a = 13;
static Double_t sqrtmip_a[13] = {0,   1,   2,   3,   4,   5,   6,   7,   8,  10,  12,   26,  100};
static Double_t sqrtmev_a[13] = {0,1.29,2.58,3.65,4.54,5.28,5.90,6.46,6.89,7.59,8.19,10.04,19.82};
static Int_t    npoints_d = 14; 
static Double_t sqrtmip_d[14] = {0,   2,   3,   4,   5,   6,   7,   8,   10,   12,   14,   16,   26,  100};
static Double_t sqrtmev_d[14] = {0,2.27,3.57,4.74,5.80,6.77,7.68,8.50,10.03,11.43,12.80,14.22,20.25,64.87};
*/
float TofTrack::MipCorrection(float edep, int type) {
  if (edep<1e-06) return 1;
  if ( (type!=kAnode)&&(type!=kDynode) ) return 1; // error message?
  // init if needed
  if (sqrtmip_sqrtmev_spline[type]==0) 
    sqrtmip_sqrtmev_spline[type] = (type==kAnode) ? 
    new TSpline3("sqrtmip_sqrtmev_spline_a",sqrtmev_a,sqrtmip_a,npoints_a) :
    new TSpline3("sqrtmip_sqrtmev_spline_d",sqrtmev_d,sqrtmip_d,npoints_d);
  // eval
  float x = sqrt(edep);
  float y = sqrtmip_sqrtmev_spline[type]->Eval(x);
  return y*y/(x*x);
}


// charge correction: from sqrt(MeV) to sqrt(N. MIP) (saturation / Birks)
static float Anode_MipCorr[3] = {1.371,-0.092854,0.032713};
static float Dynode_MipCorr[3] = {1.149,3.3e-3,-5e-6};
// bar equalization factor at Z=2 for anodes
static float ampl_norm_a[4][10]={
  1.003,1.008,1.002,1.004,0.994,0.993,1.002,0.996,0,0,
  0.995,1.006,1.001,0.994,0.998,1.005,1.002,0.998,0,0,
  1.016,1.022,1.006,0.995,1.003,0.998,0.983,0.990,0.993,1.010,
  0.993,1.000,0.996,1.002,1.001,1.001,1.006,1.003,0,0
};
// bar equalization factor at Z=6 for dynodes
static float ampl_norm_d[4][10]={
  1.025,0.965,1.001,0.999,0.989,1.029,0.981,1.003,0,0,
  0.993,0.984,0.999,1.002,1.045,1.027,0.983,0.989,0,0,
  1.089,0.998,0.999,0.985,0.992,0.983,0.994,0.976,0.980,1.076,
  1.006,0.971,0.989,1.000,1.008,0.988,0.982,1.032,0,0
};
float TofTrack::MipCorrectionContin(float edep, int layer, int bar, int type) {
  if ( (layer<1)||(layer>4) ) return 1; // error message?
  if ( (bar<1)||(bar>10) ) return 1; // error message?
  float x = (type==kAnode) ? sqrt(edep)/Anode_MipCorr[0] : sqrt(edep)/Dynode_MipCorr[0];
  float xx = x*x;
  float xxx = xx*x;
  float xxxx = xxx*x;
  float y = (type==kAnode) ?
    (1.-Anode_MipCorr[1]-Anode_MipCorr[2])*x+Anode_MipCorr[1]*xx+Anode_MipCorr[2]*xxx :
    sqrt(((1-Dynode_MipCorr[1]*xx)-sqrt((1-Dynode_MipCorr[1]*xx)*(1-Dynode_MipCorr[1]*xx)-4*Dynode_MipCorr[2]*xxxx))/(2*Dynode_MipCorr[2]*xx));
  y = (type==kAnode) ? y*ampl_norm_a[layer-1][bar-1] : y*ampl_norm_d[layer-1][bar-1];
  return y*y/edep;
}



float TofTrack::BetaCorrection(int layer) {
  float beta = GetBeta();
  // return AMSEnergyLoss::GetBetaCorrectionTofLayer(layer,beta); 
  return AMSEnergyLoss::GetTofLayerLogBetaGammaCorrectionFromBeta(layer,beta);
}


float TofTrack::RigidityCorrection(int layer, float mass_on_Z) {
  float rigidity = GetAssociatedTrTrackRigidity();
  return AMSEnergyLoss::GetTofLayerLogBetaGammaCorrectionFromRigidity(layer,rigidity,mass_on_Z);
}


float TofTrack::BetaRigidityCorrection(int layer, float mass_on_Z) {
  float beta = GetBeta();
  float rigidity = GetAssociatedTrTrackRigidity();
  return AMSEnergyLoss::GetTofLayerLogBetaGammaCorrection(layer,beta,rigidity,mass_on_Z);
}


// f(beta,layer)
// - dependent from plane and beta direction
static float A_BetaCorr[4]  = { 1.299212,1.150553,0.570712,0.080902 };
static float B_BetaCorr[4]  = { 0.629134,0.451629,-0.348635,-0.881912} ;
static float b0_BetaCorr[4] = { 0.932879,0.941124,1.000000,1.000000};
float TofTrack::BetaCorrection_Old(int layer) {
  /* 
     - Simplified fitting function:
       f(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + C
                 beta>beta0, k (the TOF beta "saturates")     
       Continuity imposed on beta0 (no derivative continuity)
       C = k - A/beta0^2 - B*log(beta0)/beta0^2
     - Beta Correction:
       g(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + 1 - A/beta0^2 - B*log(beta0)/beta0^2
                 beta>beta0, 1
     - Physical folding: 
       use abs(beta) instead of beta 
       use inverted coefficients for external planes (from layer 1,2,3,4 to 4,3,2,1);
  */ 
  // beta
  float beta = GetBeta();
  if ( (beta<-10)||(beta>10) ) return 1; // error message?
  if ( (layer<1)||(layer>4) ) return 1; // error message?
  // layer
  int ilayer = layer - 1;
  if (beta<0) ilayer = 3 - ilayer;
  // folding
  beta = fabs(beta);
  // return value
  if (beta>=b0_BetaCorr[ilayer]) return 1; 
  return A_BetaCorr[ilayer]/pow(beta,2) + 
         B_BetaCorr[ilayer]*log(beta)/pow(beta,2) + 
         1 - 
         A_BetaCorr[ilayer]/pow(b0_BetaCorr[ilayer],2) - 
         B_BetaCorr[ilayer]*log(b0_BetaCorr[ilayer])/pow(b0_BetaCorr[ilayer],2);
}


/* Bethe-Block function up to beta0 */
static float Anode_BetaCorr[2]  = { 1.13685e-01,4.71671e+02 };
static float Dynode_BetaCorr[2]  = { 5.02055e-02,2.11278e+04 };
static float Anode_BetaCorr2[2]  = { 1.2887,-0.05238 };
static float Dynode_BetaCorr2[2]  = { 1.5927,-0.09895 };
static float Beta0_BetaCorr = 0.95;
float TofTrack::BetaCorrectionContin1(int type) {
  // beta
  float beta = GetBeta();
  if ( (beta<-10)||(beta>10) ) return 1; // error message?
  // folding
  beta = fabs(beta);
  float gamma = 1/sqrt(1 - beta*beta);
  float gamma0 = 1/sqrt(1 - Beta0_BetaCorr*Beta0_BetaCorr);
  // return value
  if (beta>=Beta0_BetaCorr) return 1;
  float beta_corr = (type==kDynode) ?
    (Dynode_BetaCorr[0]/(beta*beta))*(log(Dynode_BetaCorr[1]*gamma*gamma*beta*beta)-beta*beta)+
    1.-(Dynode_BetaCorr[0]/(Beta0_BetaCorr*Beta0_BetaCorr))*(log(Dynode_BetaCorr[1]*gamma0*gamma0*Beta0_BetaCorr*Beta0_BetaCorr)-
    Beta0_BetaCorr*Beta0_BetaCorr) :
    (Anode_BetaCorr[0]/(beta*beta))*(log(Anode_BetaCorr[1]*gamma*gamma*beta*beta)-beta*beta)+
    1.-(Anode_BetaCorr[0]/(Beta0_BetaCorr*Beta0_BetaCorr))*(log(Anode_BetaCorr[1]*gamma0*gamma0*Beta0_BetaCorr*Beta0_BetaCorr)-
    Beta0_BetaCorr*Beta0_BetaCorr);
  return beta_corr;
}


float TofTrack::BetaCorrectionContin2(float edep, int type) {
  // beta
  float beta = GetBeta();
  if ( (beta<-10)||(beta>10) ) return 1; // error message?
  // folding
  beta = fabs(beta);
  float x = sqrt(edep);
  float c = (type==kAnode) ? Anode_BetaCorr2[0]+Anode_BetaCorr2[1]*x : Dynode_BetaCorr2[0]+Dynode_BetaCorr2[1]*x;
  float charge=x/(c+(1-c)*beta);
   return charge*charge;
}


// Double_t pars_add_beta[10] = {0,1.03202,0.183082,0.793141,-1.19169,0.549029,0,0,0,0};
// Double_t pars_add_beta[10] = {0,1.02402,-1.28205,0.594407,0.0900581,0.597897,-3.89486,3.38096,0,0};
Double_t pars_add_beta[10] = {0,1.02378,-0.20000,0.582207,0.0688473,1.123270,-5.25774,4.38939,0,0};
float TofTrack::AdditionalBetaCorrection(float edep) {
  if (edep<=1e-06) return 0.;
  Double_t logbetagamma = AMSEnergyLoss::GetLogBetaGammaFromBeta(GetBeta());
  if (logbetagamma<=-100) return 0.;
  Double_t x0 = 1;
  Double_t y0 = 1;
  Double_t x1 = 6*sqrt(TMath::Max(0.,AMSEnergyLoss::line_to_line(&logbetagamma,&pars_add_beta[0]))/pars_add_beta[1]);
  Double_t y1 = 6;
  Double_t m  = (y1-y0)/(x1-x0);
  Double_t q  = y0 - m*x0;
  return pow(sqrt(edep)*m + q,2);
}


// very rought fit from my charge evaluation done in Dec 2011 up to Iron
// this should be evaluated using only plane informations ... 
static float par_res[2][3] = {  
  {0.191928,-0.0660268,0.013552},
  {0.120437,0.0266065,0.000115201}
};
float TofTrack::GetMipResolution(float mip, int type) {
  if ( (type!=kAnode)&&(type!=kDynode) ) return 0;
  float resolution = 0;
  for (int ipar=0; ipar<3; ipar++) resolution += par_res[type][ipar]*pow((double)mip,(double)ipar);
  return resolution;
}


float TofTrack::GetMaxChargeLayer(int type, float mass_on_Z) {
  float max = 0;
  for (int ilayer=0; ilayer<4; ilayer++) {
    float value = sqrt(GetSignalLayer(ilayer+1,type,kGain|kMIP|kPath|kBeta|kBetaAdd,mass_on_Z));
    if (value>max) max = value;
  }
  return max;
}


float TofTrack::GetChargePlane(int plane, int type, float mass_on_Z) {
  if ( (plane!=kUpper)&&(plane!=kLower) ) return 0.;
  if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return 0;
  float sum = 0;
  int   n = 0;
  for (int i=0; i<2; i++) {
    int layer = plane*2 + i + 1;
    float value = GetChargeLayer(layer,type,mass_on_Z);
    if (value<1e-06) continue;
    sum += value;
    n++;
  }
  if (n==0) return 0;
  return sum/n;
}


float TofTrack::GetChargeRatio(int type, float mass_on_Z) {
  if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return 0;
  float upper = GetChargePlane(kUpper,type,mass_on_Z);
  float lower = GetChargePlane(kLower,type,mass_on_Z);
  if (lower<1e-6) return 0;
  return upper/lower;
}


float TofTrack::GetChargeAsymmetry(int type, float mass_on_Z) {
  if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return 0;
  float upper = GetChargePlane(kUpper,type,mass_on_Z);
  float lower = GetChargePlane(kLower,type,mass_on_Z);
  if (lower+upper<1e-6) return 0;
  return (lower-upper)/(lower+upper);
}


bool TofTrack::MakeAssociatedTrTrackChiSqAndResiduals() {
  for (int iside=0; iside<2; iside++) {
    _TrackChiSq[iside] = 0;
    _TrackNdof[iside] = 0;
  }
  for (int itof=0; itof<GetNhits(); itof++) {
    TofClusterR* cluster = (TofClusterR*) GetHit(itof);
    if (!cluster) continue;
    if (!AnAssociatedTrTrackExist()) continue;
    AMSPoint tof_point(cluster->Coo[0],cluster->Coo[1],cluster->Coo[2]);
    AMSPoint trk_point;
    AMSDir   trk_dir;
    GetAssociatedTrTrack()->Interpolate(tof_point.z(),trk_point,trk_dir,GetAssociatedTrTrackFitId());
    _TrackResiduals[itof] = tof_point - trk_point;
    float value_x = (tof_point.x()-trk_point.x())/cluster->ErrorCoo[0];
    float value_y = (tof_point.y()-trk_point.y())/cluster->ErrorCoo[1];
    _TrackChiSq[0] += value_x*value_x;
    _TrackChiSq[1] += value_y*value_y;
    _TrackNdof[0]++;
    _TrackNdof[1]++; 
  } 
  if ( (_TrackNdof[0]==0)||(_TrackNdof[1]==0) ) return false;
  return true;
}


