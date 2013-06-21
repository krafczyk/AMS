#ifndef TRDLIKELIHOODCALCULATOR_HH
#define TRDLIKELIHOODCALCULATOR_HH

#include "TrdPdfLookup.hh"

/** Analysis classes */
namespace ACsoft {

namespace Analysis {

/** Defines treatment of empty straws and overflow hits in the calculation of %TRD likelihoods
  *
  */
enum TrdLikelihoodStrategy {
  AllHits = 0,
  RejectUnderflows = 1,
  RejectUnderflowsAndOverflows = 2
};


/** Generic structure needed for %TRD likelihood calculations.
  *
  * This structure contains all the information needed for the calculation of
  * %TRD likelihoods in a generic way.
  */
struct GenericHitData {

  GenericHitData( int _layerNumber, double _dedx ):
    layerNumber(_layerNumber),
    dedx(_dedx) { }

  int layerNumber; ///< %TRD layer number of hit (0..19)
  double dedx;     ///< dE/dx (ADC/cm) energy loss value of hit

};

class TRDCandidateHit;
class TrdHit;

/** Unification of %TRD likelihood calculations.
  *
  * The purpose of this class is to make sure that all %TRD likelihoods are
  * calculated in a coherent way. There are several subtle points that affect
  * the determination of the %TRD likelihood for a given particle hypothesis:
  *
  *  - there are several methods to treat empty straws, i.e. straws apparently traversed
  *    by a charged particle but without an energy deposition above threshold, and hits with
  *    an energy deposition at the upper edge of the dynamic range
  *  - the treatment of dE/dx values in the underflow or overflow bins needs care
  *  - the individual pdf values in the likelihood product have to be regularized lest a very small or even zero value
  *    brings the entire likelihood product down to zero, rendering -log(L) undefined
  *  - the calculation has to be done in a way that is numerically stable
  *
  */
class TrdLikelihoodCalculator
{

public:

  explicit TrdLikelihoodCalculator( TrdLikelihoodStrategy strategy );

  double ComputeTrdLikelihood( Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<GenericHitData>& hitData ) const;
  double ComputeTrdLikelihood( Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<TRDCandidateHit>& hits ) const;
  double ComputeTrdLikelihood( Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<TrdHit>& hits ) const;

  bool LastPdfQueriesOk() const { return fLastPdfQueriesOk; }

  double ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<GenericHitData>& hitData ) const;
  double ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<TRDCandidateHit>& hitData ) const;
  double ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<TrdHit>& hitData ) const;

  void PrepareGenericHitData( const std::vector<TRDCandidateHit>& candidateHits, std::vector<GenericHitData>& hitData ) const;
  void PrepareGenericHitData( const std::vector<TrdHit>& trdHits, std::vector<GenericHitData>& hitData ) const;

private:

  TrdLikelihoodStrategy fLikelihoodStrategy;

  double fTrdMinDeDx;
  double fTrdMaxDeDx;
  double fMinimumPdfValue;

  mutable bool fLastPdfQueriesOk;
};

}

}

#endif // TRDLIKELIHOODCALCULATOR_HH
