//  $Id: TrTrackSelection.h,v 1.1 2011/02/08 14:26:04 shaino Exp $
#ifndef __TrTrackSelection__
#define __TrTrackSelection__

#include "TrTrack.h"

class TrTrackSelection {

public:
  enum ESpanFlags { 
    kMaxInt   = 0x001, ///< Max  span of internal tracker (L1  && (L6||L7))
    kHalfL1N  = 0x002, ///< Half span with Layer 1N       (L1N && (L6||L7))
    kHalfL9   = 0x004, ///< Half span with Layer 9        (L1  && L9)
    kMaxSpan  = 0x008, ///< Max  span of the full tracker (L1N && L9)
    kAllPlane = 0x010  ///< Each plane has at least one hit
  };

  /// It returns bits of ESpanFlags to give available span
  static int GetSpanFlags(TrTrackR *track);

/*!
  \brief It returns a normalized difference of upper/lower half rigidities
  \param span
      Fitting span defined as ESpanFlags

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
      Fitting span defined as ESpanFlags

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


  ClassDef(TrTrackSelection, 1);
};

#endif
