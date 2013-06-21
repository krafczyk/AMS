#ifndef TRDTRACKFITFUNCTION_HH
#define TRDTRACKFITFUNCTION_HH

#include <TMinuitMinimizer.h>
#include <TVector3.h>
#include <TF1.h>

#include "TRDRawHit.h"

namespace ACsoft {

namespace Analysis {

class TrdTrack;
class TrdSegment;

/** Helper class for %TRD tracking: Function to be minimized by Minuit.
 *
 * We use a maximum-likelihood approach for the determination of the best-fit parameters
 * of the %TRD track:
 * For raw hits, the likelihood is assumed to have the same shape as the one used in the fits of 1D
 * effective alignment shifts (a top-hat function convoluted with a Gaussian). For missing hits, in order
 * to get convergence of the fit in one go, a chi2 contribution is used.
 *
 */
class TrdTrackFitFunction : public ROOT::Math::IMultiGenFunction {

public:

  explicit TrdTrackFitFunction( const TrdTrack& track );

  /** Number of parameters */
  virtual unsigned int NDim() const { return 4; }

  /** Defines the actual function call. */
  virtual double DoEval(const double* parameters) const;

  virtual IBaseFunctionMultiDim* Clone() const { return new TrdTrackFitFunction(*this); }

  double EvalLikelihood( const TVector3& trackPosition, const TVector3& trackDirection ) const;
  double EvalLikelihood( const TVector3& trackPosition, const TVector3& trackDirection, const AC::MeasurementMode dir ) const;

private:

  void   StorePositionsAndDirections( const Analysis::TrdSegment* segment );

private:

  static TF1* fTrackDistanceLikelihoodFunction;

  std::vector<int> fGlobalSublayerNumbersXZ; ///< list of global sublayer numbers for each track point in XZ direction, kept for fast reference
  std::vector<TVector3> fHitPositionsXZ;     ///< list of hit positions in XZ direction, kept for fast reference
  std::vector<TVector3> fHitDirectionsXZ;    ///< list of hit directions in XZ direction, kept for fast reference
  std::vector<int> fGlobalSublayerNumbersYZ; ///< list of global sublayer numbers for each track point in YZ direction, kept for fast reference
  std::vector<TVector3> fHitPositionsYZ;     ///< list of hit positions in YZ direction, kept for fast reference
  std::vector<TVector3> fHitDirectionsYZ;    ///< list of hit directions in YZ direction, kept for fast reference

  std::vector<TVector3> fMissingHitPositionsXZ;  ///< list of missing hit positions in XZ direction, kept for fast reference
  std::vector<TVector3> fMissingHitDirectionsXZ; ///< list of missing hit directions in XZ direction, kept for fast reference
  std::vector<double>   fMissingHitGapsXZ;       ///< list of gap sizes for missing hits in XZ direction, kept for fast reference
  std::vector<TVector3> fMissingHitPositionsYZ;  ///< list of missing hit positions in YZ direction, kept for fast reference
  std::vector<TVector3> fMissingHitDirectionsYZ; ///< list of missing hit directions in YZ direction, kept for fast reference
  std::vector<double>   fMissingHitGapsYZ;       ///< list of gap sizes for missing hits in YZ direction, kept for fast reference

  float fOffsetReferenceZ; ///< same as used for trd segments
};

}

}

#endif // TRDTRACKFITFUNCTION_HH
