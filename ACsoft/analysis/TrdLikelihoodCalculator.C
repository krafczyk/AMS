
#include "TrdLikelihoodCalculator.hh"

#include <math.h>

#include "AMSGeometry.h"
#include "Settings.h"
#include "TrdPreselection.hh"
#include "TrdHit.hh"

#define DEBUG 0
#include <debugging.hh>

ACsoft::Analysis::TrdLikelihoodCalculator::TrdLikelihoodCalculator( TrdLikelihoodStrategy strategy ) :
  fLikelihoodStrategy(strategy),
  fTrdMinDeDx(0.),
  fTrdMaxDeDx(ACsoft::AC::AMSGeometry::TRDMaxADCCount / ::AC::Settings::TrdTubeDefaultMinPathLength),
  fMinimumPdfValue(::AC::Settings::TrdMinimumPdfValue),
  fLastPdfQueriesOk(false)
{
}

/** Fill a vector of generic hit data needed for likelihood calculations based on a vector of candidate hits.
  *
  * Hits are selected based on the strategy (with respect to under- and overflow bins) chosen at construction time.
  *
  * \param[in] candidateHits input list of candidate hits
  * \param[out] hitData output vector, to be used in ComputeTrdLikelihood()
  *
  */
void ACsoft::Analysis::TrdLikelihoodCalculator::PrepareGenericHitData( const std::vector<TRDCandidateHit>& candidateHits, std::vector<GenericHitData>& hitData ) const {

  hitData.clear();
  hitData.reserve(candidateHits.size());

  for( unsigned int i=0; i<candidateHits.size(); ++i ) {

    const ACsoft::Analysis::TRDCandidateHit& hit = candidateHits.at(i);
    if( fLikelihoodStrategy == ACsoft::Analysis::RejectUnderflows && hit.deDx < fTrdMinDeDx+0.5 ) continue;
    if( fLikelihoodStrategy == ACsoft::Analysis::RejectUnderflowsAndOverflows && (hit.deDx < fTrdMinDeDx+0.5 || hit.deDx > fTrdMaxDeDx-0.5) ) continue;
    hitData.push_back(ACsoft::Analysis::GenericHitData(ACsoft::AC::TRDStrawToLayer(hit.straw),hit.deDx)); // FIXME use detector geometry structure instead
  }
}

/** Fill a vector of generic hit data needed for likelihood calculations based on a vector of trd hits.
  *
  * Hits are selected based on the strategy (with respect to under- and overflow bins) chosen at construction time.
  *
  * \param[in] trdHits input list of trd hits
  * \param[out] hitData output vector, to be used in ComputeTrdLikelihood()
  *
  */
void ACsoft::Analysis::TrdLikelihoodCalculator::PrepareGenericHitData( const std::vector<TrdHit>& trdHits, std::vector<GenericHitData>& hitData ) const {

  hitData.clear();
  hitData.reserve(trdHits.size());

  for( unsigned int i=0; i<trdHits.size(); ++i ) {

    const ACsoft::Analysis::TrdHit& hit = trdHits.at(i);
    double dedx = hit.GetDeDx();
    if( fLikelihoodStrategy == ACsoft::Analysis::RejectUnderflows && dedx < fTrdMinDeDx+0.5 ) continue;
    if( fLikelihoodStrategy == ACsoft::Analysis::RejectUnderflowsAndOverflows && (dedx < fTrdMinDeDx+0.5 || dedx > fTrdMaxDeDx-0.5) ) continue;
    hitData.push_back(ACsoft::Analysis::GenericHitData(hit.Layer(),dedx));
  }

}

/** Calculate %TRD likelihood based on the chosen strategy for a list of TRDCandidateHit objects. */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeTrdLikelihood( ACsoft::Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<TRDCandidateHit>& hits ) const {

  std::vector<ACsoft::Analysis::GenericHitData> hitData;
  PrepareGenericHitData(hits,hitData);

  return ComputeTrdLikelihood(particle,pXe,rigidity,hitData);
}

/** Calculate %TRD likelihood based on the chosen strategy for a list of TrdHit objects. */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeTrdLikelihood( ACsoft::Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<TrdHit>& hits ) const {

  std::vector<ACsoft::Analysis::GenericHitData> hitData;
  PrepareGenericHitData(hits,hitData);

  return ComputeTrdLikelihood(particle,pXe,rigidity,hitData);
}


/** Calculate %TRD likelihood.
  *
  * \param[in] particle %Particle hypothesis
  * \param[in] pXe Xenon partial pressure
  * \param[in] rigidity Rigidity of particle
  * \param[in] hitData vector containing the layer numbers and dE/dx values for each relevant hit
  *
  * Under- and overflow hits are treated with care.
  * Individual pdf values are regularized to avoid zero likelihood values.
  *
  * After the call to this function, use LastPdfQueriesOk() to check if good pdf values were used in the calculation of the likelihood.
  */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeTrdLikelihood( ACsoft::Utilities::ParticleId particle, double pXe, double rigidity, const std::vector<GenericHitData>& hitData ) const {

  ACsoft::Utilities::TrdPdfLookup* lookup = ACsoft::Utilities::TrdPdfLookup::Self();

  rigidity = fabs(rigidity);
  if(hitData.empty()) return 0.;

  unsigned int nHits = hitData.size();

  double weight = 1.0/double(nHits);

  double likelihood = 1.0;

  bool allQueriesOk = true;

  for( unsigned int i = 0 ; i < nHits ; ++i ) {

    const ACsoft::Analysis::GenericHitData& hit = hitData[i];

    double value = 0.;
    if( hit.dedx < fTrdMinDeDx+0.5 )
      value = lookup->GetTrdPdfValue(pXe, rigidity, hit.layerNumber, fTrdMinDeDx, particle);
    else if( hit.dedx > fTrdMaxDeDx-0.5 )
      value = lookup->GetTrdPdfValue(pXe, rigidity, hit.layerNumber, fTrdMaxDeDx, particle);
    else
      value = lookup->GetTrdPdfValue(pXe, rigidity, hit.layerNumber, hit.dedx, particle);

    value = std::max(fMinimumPdfValue,value);

    bool queryOk = lookup->LastQueryOk();
    allQueriesOk &= queryOk;

    likelihood *= pow(value, weight);
  }

  DEBUG_OUT << "pid " << particle << " pXe " << pXe << " rigidity " << rigidity << " nHits " << hitData.size() << " -> " << likelihood << " ok: " << allQueriesOk << std::endl;

  fLastPdfQueriesOk = allQueriesOk;

  return likelihood;
}



/** Calculate %TRD likelihood ratio used for electron-proton separation.
  *
  * Calculates the likelihood ratio
  * \f[
  * L_{TRD}=-\log\left(\frac{L_e}{L_e+L_p}\right)
  * \f]
  * where \f$L_e\f$ is the electron likelihood and \f$L_p\f$ is the proton likelihood.
  *
  * \param[in] pXe Xenon partial pressure
  * \param[in] rigidity Rigidity of particle
  * \param[in] hitData vector containing the layer numbers and dE/dx values for each relevant hit
  *
  */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<GenericHitData>& hitData ) const {

  double likelihoodProt = ComputeTrdLikelihood(ACsoft::Utilities::Proton,pXe,rigidity,hitData);
  bool protonQueryOk = LastPdfQueriesOk();

  double likelihoodElec = ComputeTrdLikelihood(ACsoft::Utilities::Electron,pXe,rigidity,hitData);
  bool electronQueryOk = LastPdfQueriesOk();

  fLastPdfQueriesOk = protonQueryOk && electronQueryOk;

  double logl = -log(likelihoodElec/(likelihoodElec+likelihoodProt));

  return logl;
}

/** Calculate %TRD likelihood ratio used for electron-proton separation for a list of TRDCandidateHit objects. */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<TRDCandidateHit>& hits ) const {

  std::vector<ACsoft::Analysis::GenericHitData> hitData;
  PrepareGenericHitData(hits,hitData);

  return ComputeElectronProtonLikelihood(pXe,rigidity,hitData);
}

/** Calculate %TRD likelihood ratio used for electron-proton separation for a list of TrdHit objects. */
double ACsoft::Analysis::TrdLikelihoodCalculator::ComputeElectronProtonLikelihood( double pXe, double rigidity, const std::vector<TrdHit>& hits ) const {

  std::vector<ACsoft::Analysis::GenericHitData> hitData;
  PrepareGenericHitData(hits,hitData);

  return ComputeElectronProtonLikelihood(pXe,rigidity,hitData);
}


