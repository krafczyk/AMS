// $Id: TofTrack.h,v 1.7 2012/04/27 18:06:33 oliva Exp $

#ifndef __TofTrack__
#define __TofTrack__

#include "point.h"
#include "root.h"
#include "edep.h"

#include "TrTrack.h"
#include "TofGeometry.h"

#include "TObject.h"
#include "TSpline.h"
#include "TVirtualFitter.h"

#include <vector>
#include <cmath>

class TofClusterR;

//! The \c TofTrack class is a class for the time-of-flight (ToF) track description. 
/*!  
  A \c TofTrack is created with informations coming from 2 to 4 \c TofClusterR on different TOF planes.
  The class has methods to reconstruct a straight trajectory, a beta and a charge using only ToF-related informations.
  Be aware that results of the standalone procedure will be not good in the case of low energy particles.
  Nevertheless TofTrack could be used as an un-biased reconstruction estimator usable for efficiency estimation
  of other detectors.
  A \c TrTrackR (Tracker track) can be associated to the \c TofTrack to improve beta and charge evaluations.
*/
class TofTrack {

 public:

  //! Side type
  enum SideType {
    kAnode  = 0, /*!< Anode. */
    kDynode = 1, /*!< Dynode. */
    kMix    = 2  /*!< Anode and Dynode weighted mean. */ 
  };

  //! Mean options
  enum MeanOptions {
    kPlain   = 0x01, /*!< If set plain mean. If not set truncated mean. */
    kSqrt    = 0x02, /*!< If set sqrt of signal. If not set normal. */
    kExclude = 0x04  /*!< Exclude from evaluation clusters judged bad by HasToBeExcluded() function. */
  };

  //! Signal options
  enum SignalOptions {
    kGain     = 0x01, /*!< Gain correction */
    kMIP      = 0x02, /*!< MIP scale. */
    kMeV      = 0x04, /*!< MeV scale. */
    kPath     = 0x08, /*!< Pathlength correction. */
    kBeta     = 0x10, /*!< \f$\beta\f$ correction. */
    kRigidity = 0x20, /*!< Rigidity correction (depends on A/Z guess). */
    kBetaAdd  = 0x40, /*!< Additional \f$\beta\f$ correction. */ 
    kContin   = 0x80  /*!< use Contin corrections instead of mine whenever possible. */
  };

  //! Plane type
  enum PlaneType {
    kUpper = 0, /*!< Upper. */
    kLower = 1  /*!< Lower. */
  };

 private:

  //! Light speed constant
  static const float c_speed = 29.9792458; // light speed in cm/ns 

  //! Vector of hit pointers (... a vector of indexes instead?)
  vector<TofClusterR*> _Hits;

  //! Spatial fit track intersection at Z = 0 (cm)
  AMSPoint         _Point;
  //! Spatial fit track direction at Z = 0 (rad)
  AMSDir           _Dir;
  //! Spatial fit chi-squared (x, y)
  float            _SpatialChiSq[2];
  //! Spatial fit degrees of freedom (x, y)
  int              _SpatialNdof[2];
  //! Spatial fit residuals (cm)
  AMSPoint         _SpatialResiduals[4];

  //! Beta fit beta (v/c)
  float            _Beta;
  //! Beta fit error 
  float            _ErrInvBeta;
  //! Beta time offset (ns)
  float            _Offset;
  //! Beta fit chi-squared 
  float            _ChiSqT;
  //! Beta fit number of degrees of freedom
  int              _NdofT;
  //! Beta time residuals (ns)
  float            _TimeResiduals[4];

  //! Raw energy deposition of anode/dynode per layer (integral of TofRawCluster::edepa and TofRawCluster::edep members)
  float            _EdepRaw[2][4]; 
  //! Pathlength 
  float            _PathLength[4];

  //! Truncated mean 
  float            _Mean[3];
  //! RMS of the truncated mean  
  float            _RMS[3]; 
  //! Points in the mean calculation
  int              _NPoints[3];

  //! Pointer to the associated TrTrack
  TrTrackR*        _TrTrack;
  //! TrTrack fit ID 
  int              _Id;
  //! TOF wrt Tracker track chi-squared (x, y)
  float            _TrackChiSq[2];
  //! TOF wrt Tracker track degrees of freedom (x, y)
  int              _TrackNdof[2];
  //! TOF wrt Tracker track residuals (cm)
  AMSPoint         _TrackResiduals[4];

 public: 

  /** @name Service members. */
  /**@{*/
  //! MIP conversion
  static TSpline3* sqrtmip_sqrtmev_spline[2]; 
  //! Top of Layer corrections
  static TSpline3* beta_top_of_tracker_layer[9];
  /**@}*/

 public:

  /** @name Basic methods. 
    * 
    * TOF Track construction/destruction, hits managing. 
    */
  /**@{*/
  //! Default constructor
  TofTrack();
  //! Constructor with array of hits 
  TofTrack(TofClusterR* phit[], int nhits);
  //! Constructor with vector of hits 
  TofTrack(vector<TofClusterR*> hits); 
  //! Constructor with BetaR (TrTrackR, and TrTrackR fit ID)
  TofTrack(BetaR* beta, TrTrackR* trk_track = 0, int trk_fit_id = 0);
  //! Destructor
  virtual ~TofTrack() { Clear(); }
  //! Builder 
  bool Build(vector<TofClusterR*> hits);
  //! Clear
  void Clear();
  //! Print on screen
  void Print(int verbosity = 0);
  //! Make fits (spatial, time, charge), use the associated tracker track knowledge if possible 
  bool MakeDefaultFits();
  //! Get number of hits
  int  GetNhits()    { return int(_Hits.size()); }
  //! Get number of hits
  int  NTofCluster() { return GetNhits(); }
  //! Get number of hits
  int  nTofCluster() { return GetNhits(); }
  //! Get pattern (bit 0/1/2/3: plane 1/2/3/4) 
  int  GetPattern();
  //! Get pattern a la Choumilov (-1 = fail; 0 = 4 hits; 1,2,3,4 = 3 hits, code corresponds to missing layer) 
  int  GetPatternChoumilov();
  //! Check if the layer is used
  bool CheckPatterLayer(int layer);
  //! Get TOF cluster   
  TofClusterR* GetHit(int ii) { return (ii<GetNhits()) ? _Hits.at(ii) : 0; }
  //! Get TOF cluster   
  TofClusterR* pTofCluster(int ii) { return GetHit(ii); }
  //! Get TOF cluster of a layer (1, ..., 4) 
  TofClusterR* GetHitLayer(int layer);
  //! Drop hit (returns true if the hit is successfully removed)
  bool DropHit(int ii);
  /**@}*/

  /** @name Standalone spatial reconstruction 
    *
    * Method TofTrack::MakeSpatialFitInGaussianApprox takes all the \a longitudinal and \a transverse coordinates
    * of the clusters involved in the track and makes a straight line fit with least-square method on \a x and \a y
    * coordinates. In this approximation longitudinal and transverse coordinates have both a gaussian error given
    * by the TrTofClusterR::CooErr method.
    *
    */
  /**@{*/
  //! Get point (linear fit)
  AMSPoint     GetPoint() { return _Point; }
  //! Get direction (linear fit)
  AMSDir       GetDir() { return _Dir; }
  //! Get point at a given z (linear fit)
  AMSPoint     GetPoint(float z);
  //! Interpolator, returns length from z=0 (from associated track, or if absent, from linear fit)
  float        Interpolate(float z, AMSPoint& point, AMSDir& dir);
  //! Get length at a given z (from associated track, or if absent, from linear fit)
  float        GetLength(float z);
  //! Get the path length on a layer (sum of the intersections of track with paddles) 
  float        GetPathLength(int layer) { return ( (layer<=4)&&(layer>=1) ) ? _PathLength[layer-1] : 0; }
  //! Get the most simple computation of pathlenght possible (NTrRawCluster/cos(theta), this could overestimate pathlenght)
  float        GetPathLengthEasy(int layer);
  //! Get X spatial fit chi-squared (linear fit)
  float        GetChiSqX() { return _SpatialChiSq[0]; }
  //! Get Y spatial fit chi-squared (linear fit)
  float        GetChiSqY() { return _SpatialChiSq[1]; }
  //! Get XY spatial fit combined chi-squared (linear fit)
  float        GetChiSqXY() { return GetChiSqX() + GetChiSqY(); }
  //! Get X spatial fit number of degrees of freedom (linear fit)
  int          GetNdofX() { return _SpatialNdof[0]; }
  //! Get Y spatial fit number of degrees of freedom (linear fit)
  int          GetNdofY() { return _SpatialNdof[1]; }
  //! Get XY spatial fit combined number of degrees of freedom (linear fit)
  int          GetNdofXY() { return _SpatialNdof[0] + _SpatialNdof[1]; }
  //! Get X spatial fit reduced chi-squared (linear fit)
  float        GetRedChiSqX() { return (GetNdofX()>0) ? GetChiSqX()/GetNdofX() : 0; }
  //! Get Y spatial fit reduced chi-squared (linear fit)
  float        GetRedChiSqY() { return (GetNdofY()>0) ? GetChiSqY()/GetNdofY() : 0; }
  //! Get XY spatial fit reduced combined chi-squared (linear fit)
  float        GetRedChiSqXY() { return (GetNdofXY()>0) ? GetChiSqXY()/GetNdofXY() : 0; }
  //! Asking the track interpolation to be inside Tracker layers (bit 0, ..., 9 --> layerj 1, ..., 9) (linear fit) 
  int          GetPatternInsideTracker(float margin = 10 /*cm*/);
  //! Get spatial residual by layer (cm) (from associated track, or if absent, from linear fit)
  AMSPoint     GetSpatialResidual(int layer);
  //! Make a spatial fit in a gaussian approximation (errors given by Choumilov)
  bool         MakeSpatialFitInGaussianApprox();
  //! Make a spatial fit using only longitudinal coordinates (definition, not fit), requires 4 hits
  bool         MakeSpatialFitWithOnlyLongitudinalCoordinates();
  //! Make a spatial fit using real residual PDFs and minimization 
  bool         MakeSpatialFitMinimization();
  //! Make the spatial residuals (with associated track, or if absent, with linear fit)
  bool         MakeSpatialResiduals();
  /**@}*/

  /** @name Beta reconstruction 
    *
    * Beta reconstruction is given by the linear fit between the path of the particle
    * and the time (more details here ...).  
    *
    */
  /**@{*/
  //! Get beta (for the current time fit)
  float        GetBeta() { return _Beta; }
  //! Get the inverse beta error
  float        GetErrInvBeta() { return _ErrInvBeta; }
  //! Get time offset (ns)
  float        GetOffset() { return _Offset; }
  //! Get reduced beta-fit chi-squared
  float        GetChiSqT() { return _ChiSqT; }
  //! Get reduced beta-fit chi-squared
  int          GetNdofT() { return _NdofT; }
  //! Get reduced beta-fit chi-squared
  float        GetRedChiSqT() { return (GetNdofT()>0) ? GetChiSqT()/GetNdofT() : 0; }
  //! Get time at height z (use associated track if possible) 
  float        GetTime(float z);
  //! Get time residual by layer (ns)
  float        GetTimeResidual(int layer);
  //! Make a time fit with and all the requested points (bit mask), use the linear fit if the associated track doesn't exist
  bool         MakeTimeFit(char mask = 0xF);
  //! Make a time fit with the TOF track spatial fit knowledge and layer 2 and 3 
  bool         MakeTimeFitInner() { return MakeTimeFit(0x6); }
  //! TO BE DONE: Make a temporal fit with CDSA correction 
  // bool         MakeTimeFitCDSA(float mass, int charge) { return false; }
  //! Make the time residuals
  bool         MakeTimeResiduals(); 
  /**@}*/

  /** @name Charge reconstruction 
    *
    * - Standalone: beta correction is performed wih standalone beta + theta correction is based on standalone TOF fit
    * - Associated: beta correction is performed using the refined beta measurement + theta from associated track
    *
    */
  /**@{*/
  //! Evaluate the goodness of the Anode signal measurement
  /*!
   *  This corresponds to check if the measurements coming from the two Anode sides is greater
   *  than zero.    
   */ 
  static bool  IsGoodAnodeForCharge(TofRawClusterR* rawcluster);
  //! Evaluate the goodness of the Anode signal measurement 
  /*!
   *  The TofRawClusterR associated to this cluster should have a good Anode measurement.
   */
  static bool  IsGoodAnodeForCharge(TofClusterR* cluster);
  //! Evaluate the goodness of the Dynode signal measurement
  /*!
   *  This corresponds to check if the measurements coming from the 2/3 dynodes on the 2 sides of
   *  the counters are greater than zero. This condition should be fullfilled for a valid charge
   *  measurement on Dynode side.
   *
   *  The signal coming from Dynode is zero when signal is under threshold. This happens 
   *  for charge 1 and for some dynode also for charge 2 or 3. 
   */
  static bool  IsGoodDynodeForCharge(TofRawClusterR* rawcluster);
  //! Evaluate the goodness of the Dynode signal measurement
  /*!
   *  The TofRawClusterR associated to this cluster should have a good Dynode measurement.
   */
  static bool  IsGoodDynodeForCharge(TofClusterR* cluster);
  //! Get signal on a layer for the given set of corrections for a particular measurement
  /*!
   * The signal is given by the sum of signal on the counters (this means that 2 signals are merged in 1 measurement)
   * - where \c sig_opt is a combination of #SignalOptions (ex. to have \f$E_{dep}\f$ in MeV use \c sig_opt = #TofTrack::kGain|#TofTrack::kMIP|#TofTrack::kMeV)
   * - if \c sig_opt = 0 you will get the default value: ADC counts * \c adc2mev, where \c adc2mev value is given by Choumilov.
   */
  float        GetSignalLayer(int layer, int type, int sig_opt, float mass_on_Z = 0.938);
  //! Get floating point charge evaluation for a single layer 
  float        GetChargeLayer(int layer, int type = kMix, float mass_on_Z = 0.938) { return sqrt(GetSignalLayer(layer,type,kGain|kMIP|kPath|kBeta|kBetaAdd,mass_on_Z)); }
  //! Get the maximum signal on ToF
  float        GetMaxChargeLayer(int type = kMix, float mass_on_Z = 0.938);
  //! Evaluate energy deposition for a given layer
  float        GetEdepLayer(int layer, int type = kMix, float mass_on_Z = 0.938) { return GetSignalLayer(layer,type,kGain|kMIP|kMeV,mass_on_Z); } 
  //! Get the floating point charge estimator for Upper/Lower plane 
  float        GetChargePlane(int plane, int type = kMix, float mass_on_Z = 0.938);
  //! Evaluate charge ratio (upper/lower charge estimations) 
  float        GetChargeRatio(int type = kMix, float mass_on_Z = 0.938);
  //! Evaluate charge asymmetry: (lower-upper)/(lower+upper) 
  float        GetChargeAsymmetry(int type = kMix, float mass_on_Z = 0.938);
  //! Get the gain correction
  static float GainCorrection(float edep, int layer, int bar, int type);
  //! Get the pathlength correction 
  float        PathLengthCorrection(int layer);
  //! MIP correction (nMIP/MeV for the given edep in MeV)
  static float MipCorrection(float edep, int type);
  //! MIP correction (nMIP/MeV for the given edep in MeV), a gain correction is also implemented 
  static float MipCorrectionContin(float edep, int layer, int bar, int type);
  //! Beta correction (f(beta,Z=1), uses the internal beta, different for each layer and for pos/neg beta)
  float        BetaCorrection(int layer);
  //! Beta correction (f(beta,Z=1), uses the internal beta, different for each layer and for pos/neg beta)
  float        BetaCorrection_Old(int layer);
  //! Beta correction (applied to energy deposition)
  float        BetaCorrectionContin1(int type);
  //! Beta correction (applied to charge estimator)
  float        BetaCorrectionContin2(float edep, int type);
  //! Rigidity correction (depends on A/Z guess)
  float        RigidityCorrection(int layer, float mass_on_Z = 0.938);
  //! Beta and Rigidity correction (best ranges selected)
  float        BetaRigidityCorrection(int layer, float mass_on_Z = 0.938);
  //! Additional beta correction (depends on the signal itself) [TMP?]
  float        AdditionalBetaCorrection(float edep);
  //! Weighting function used for Anode/Dynode mixing  
  static float GetMipResolution(float mip, int type);
  //! Get current mean 
  float        GetMean(int type = kMix) { return ( (type==kAnode)||(type==kDynode)||(type==kMix) ) ? _Mean[type] : 0; } 
  //! Get current mean RMS 
  float        GetRMS(int type = kMix) { return ( (type==kAnode)||(type==kDynode)||(type==kMix) ) ? _RMS[type] : 0; }
  //! Get relative error (RMS/Mean)
  float        GetRelErr(int type = kMix) { if ( (type!=kAnode)&&(type!=kDynode)&&(type!=kMix) ) return 0; return (fabs(GetMean(type))>1e-10) ? GetRMS(type)/GetMean(type) : 0; }
  //! Get current number of points used to make the mean (n+1 in case of truncated mean)
  int          GetNPoints(int type = kMix) { return ( (type==kAnode)||(type==kDynode)||(type==kMix) ) ? _NPoints[type] : 0; }
  //! Get TOF floating point charge estimator Anode/Dynode (TO-DO: it may request refit ...)
  float        GetQ(int type = kMix) { return GetMean(type); }
  //! Make mean 
  bool         MakeMean(int type, int mean_opt = kPlain|kSqrt, int sig_opt = kGain|kMIP|kPath|kBeta|kBetaAdd, float mass_on_Z = 0.938);
  //! Calculate the energy deposition
  bool         MakeEdep();
  //! Calculate the pathlength in material (unit of paddle height)
  bool         MakePathLength();
  /**@}*/

  /** @name Associated Track methods 
    * 
    */
  /**@{*/
  //! Set an associated TrTrack 
  void         SetAssociatedTrTrack(TrTrackR* trk_track, int trk_fit_id = 0);
  //! Set the TrTrack fit id
  void         SetAssociatedTrTrackFitId(int trk_fit_id) { _Id = trk_fit_id; }
  //! Get the associated TrTrack 
  TrTrackR*    GetAssociatedTrTrack() { return _TrTrack; }
  //! Get the associated fit ID
  int          GetAssociatedTrTrackFitId() { return _Id; }
  //! Is there any associated Tracker track
  bool         AnAssociatedTrTrackExist() { return (_TrTrack!=0)&&(_Id>=0); } 
  //! Get the rigidity of the associated track with the wanted fit id
  float        GetAssociatedTrTrackRigidity() { return (AnAssociatedTrTrackExist()) ? _TrTrack->GetRigidity(_Id) : 0; }
  //! Get X chi-squared with respect to tracker track 
  float        GetAssociatedTrTrackChiSqX() { return _TrackChiSq[0]; }
  //! Get Y chi-squared with respect to tracker track
  float        GetAssociatedTrTrackChiSqY() { return _TrackChiSq[1]; }
  //! Get XY chi-squared with respect to tracker track
  float        GetAssociatedTrTrackChiSqXY() { return GetAssociatedTrTrackChiSqX() + GetAssociatedTrTrackChiSqY(); }
  //! Get X number of degrees of freedom 
  int          GetAssociatedTrTrackNdofX() { return _TrackNdof[0]; }
  //! Get Y number of degrees of freedom 
  int          GetAssociatedTrTrackNdofY() { return _TrackNdof[1]; }
  //! Get XY combined number of degrees of freedom
  int          GetAssociatedTrTrackNdofXY() { return _TrackNdof[0] + _TrackNdof[1]; }
  //! Get X reduced chi-squared with respect to tracker track
  float        GetAssociatedTrTrackRedChiSqX() { return (GetAssociatedTrTrackNdofX()>0) ? GetAssociatedTrTrackChiSqX()/GetAssociatedTrTrackNdofX() : 0; }
  //! Get Y reduced chi-squared with respect to tracker track
  float        GetAssociatedTrTrackRedChiSqY() { return (GetAssociatedTrTrackNdofY()>0) ? GetAssociatedTrTrackChiSqY()/GetAssociatedTrTrackNdofY() : 0; }
  //! Get XY reduced combined chi-squared with respect to tracker track 
  float        GetAssociatedTrTrackRedChiSqXY() { return (GetAssociatedTrTrackNdofXY()>0) ? GetAssociatedTrTrackChiSqXY()/GetAssociatedTrTrackNdofXY() : 0; }
  //! Calculate chi-squared and residuals with respect to tracker track 
  bool         MakeAssociatedTrTrackChiSqAndResiduals();
  /**@}*/

  ClassDef(TofTrack,1);
};

#endif
