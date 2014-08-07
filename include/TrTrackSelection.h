//  $Id$
#ifndef __TrTrackSelection__
#define __TrTrackSelection__

#include "TrCharge.h"
#include "TrTrack.h"

class TrTrackSelection {

public:
  virtual ~TrTrackSelection() { }
  enum ESpanFlags { 
    kMaxInt   = 0x101, ///< Max  span of internal tracker (L1  && (L6||L7))
    kHalfL1N  = 0x102, ///< Half span with Layer 1N       (L1N && (L6||L7))
    kHalfL9   = 0x104, ///< Half span with Layer 9        (L1  && L9)
    kMaxSpan  = 0x108, ///< Max  span of the full tracker (L1N && L9)
    kAllPlane = 0x110  ///< Each plane has at least one hit
  };

  /// It returns bits of ESpanFlags to give available span
  static int GetSpanFlags(TrTrackR *track);

/*!
  \brief It returns a normalized difference of upper/lower half rigidities
  \param span
      Fitting span defined as ESpanFlags or
      hit pattern digit
      \li  3   Inner Tracker only (aka drop 2 external hits);
      \li  5   inner + Layer 1N (kFitLayer8)
      \li  6   inner + Layer 9  (kFitLayer9)
      \li  7   inner + Layer 1N + layer 9 (kFitLayer8 & kFitLayer9)

  \param algo
      Fitting algorithm= 0 The default algorithm choosen at recon stage 
                         1 Choutko;
                         2 Alcaraz;
                         3 Chikanian;
                       +10 mscattering off;
  \param refit    
                         0 Do not refit
			 1 Refit if does not exist
			 2 Force refit
			   (TrTrackR::DefaultMass and DefaultCharge is used)
!*/
  static double GetHalfRdiff(TrTrackR *track, int span, 
			                      int algo = 0, int refit = 0);

/*!
  \brief It returns a normalized ((r_U/s_U)^2+(r_L/s_L)^2)
    where r_U: Residual in y at the first layer with lower half fitting
          r_L: Residual in y at the last  layer with upper half fitting
	  s_U: Scattering-corrected sigma of r_U
	  s_L: Scattering-corrected sigma of r_L
    SH FIXME: For the moment only max span is supported

  \param span
      Fitting span defined as ESpanFlags of
      hit pattern digit
      \li  3   Inner Tracker only (aka drop 2 external hits);
      \li  5   inner + Layer 1N (kFitLayer8)
      \li  6   inner + Layer 9  (kFitLayer9)
      \li  7   inner + Layer 1N + layer 9 (kFitLayer8 & kFitLayer9)

  \param algo
      Fitting algorithm= 0 The default algorithm choosen at recon stage 
                         1 Choutko;
                         2 Alcaraz;
                         3 Chikanian;
                       +10 mscattering off;
  \param refit    
                         0 Do not refit
			 1 Refit if does not exist
			 2 Force refit
			   (TrTrackR::DefaultMass and DefaultCharge is used)
!*/
  static double GetHalfRessq(TrTrackR *track, int span = kMaxSpan,
			                      int algo = 0, int refit = 0);

/*!
  \brief It returns the minimum distance between track and noise hits
         on each ladders where the track passes.
  \param layerp
         layer pattern digit as in TrTrackR::iTrTrackPar
	 indicating which layers to be included.
         1111110 corresponds to layers 2,3,4,5,6,7
  \param nhmin
         number of noise hits required in each ladder
!*/
  static AMSPoint GetMinDist(TrTrackR *track, int layerp = 1111110,
			                      int nhmin = 1);

  /// GetHalfRdiff with fitid obtained by TrTrackR::iTrTrackPar
  static double GetHalfRdiff(int fitid, TrTrackR *track, int refit = 0);

  /// GetHalfRessq with fitid obtained by TrTrackR::iTrTrackPar
  static double GetHalfRessq(int fitid, TrTrackR *track, int refit = 0);

  /// It converts fitid into ESpanFlags
  static int FitidToSpan(int fitid);

  /// It converts fitid into algo
  static int FitidToAlgo(int fitid);

  /// ISS rough selection: returns a good hit pattern if it is a good track (nine-nine pattern)
  /// (-1 no track, -2 no charge 2, -3 low number of hits, -4 bad default fit)
  static int  GetPatternForGoodHelium(TrTrackR* track, float beta = 1);
  /// ISS rough selection: XY hits, a good correlation, no low signal
  static bool IsGoodHeliumHit(TrRecHitR* hit);

  ClassDef(TrTrackSelection, 1);
};

#endif
