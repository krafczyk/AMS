#include "TrdHitFactory.hh"

#include "TRDRawHit.h"
#include "Event.h"

#include "AnalysisParticle.hh"
#include "SplineTrack.hh"
#include "TrdTrack.hh"
#include "DetectorManager.hh"

#define DEBUG 0
#define INFO_OUT_TAG "TrdHitFactory> "
#include <debugging.hh>

ACsoft::Analysis::TrdHitFactory::TrdHitFactory() :
  fReferenceGain(100.0)
{
}


/** Produce TRD hits from all raw hits in given event, based on spline track.
  *
  * TrdHits are produced regardless of pathlength calculated based on given input track.
  *
  * \param[in] event Input event.
  * \param[in] splineTrack SplineTrack that provides second coordinate and pathlength.
  * \param[in] steps Bit-wise OR of TrdHitProductionSteps to be used in hit production.
  * \param[out] resultVector Produced hits will be stored in this vector.
  */
void ACsoft::Analysis::TrdHitFactory::ProduceTrdHitsFrom( const ACsoft::AC::Event& event, const ACsoft::Analysis::SplineTrack& splineTrack,
                                                  int steps, std::vector<ACsoft::Analysis::TrdHit>& resultVector ) {

  unsigned int nTrdHits = event.TRD().RawHits().size();

  resultVector.clear();
  resultVector.reserve(nTrdHits);

  for( unsigned int i=0 ; i<nTrdHits ; ++i ) {

    const ACsoft::AC::TRDRawHit& rhit = event.TRD().RawHits()[i];
    ACsoft::Analysis::TrdHit hit = ProduceTrdHitFrom(rhit, splineTrack, steps);
    resultVector.push_back(hit);
  }
}

/** Produce TRD hits from all raw hits in given event, based on %TRD track.
  *
  * TrdHits are produced regardless of pathlength calculated based on given input track.
  *
  * \param[in] event Input event.
  * \param[in] trdTrack TrdTrack that provides second coordinate and pathlength.
  * \param[in] steps Bit-wise OR of TrdHitProductionSteps to be used in hit production.
  * \param[out] resultVector Produced hits will be stored in this vector.
  */
void ACsoft::Analysis::TrdHitFactory::ProduceTrdHitsFrom( const ACsoft::AC::Event& event, const ACsoft::Analysis::TrdTrack& trdTrack,
                                                  int steps, std::vector<ACsoft::Analysis::TrdHit>& resultVector ) {

  unsigned int nTrdHits = event.TRD().RawHits().size();

  resultVector.clear();
  resultVector.reserve(nTrdHits);

  for( unsigned int i=0 ; i<nTrdHits ; ++i ) {

    const ACsoft::AC::TRDRawHit& rhit = event.TRD().RawHits()[i];
    ACsoft::Analysis::TrdHit hit = ProduceTrdHitFrom(rhit, trdTrack, steps);
    resultVector.push_back(hit);
  }
}


/** Produce a TRD hit from raw hit and spline track.
  *
  * The spline track is needed for the second coordinate (coordinate along wire) and the pathlength.
  *
  * \param rhit TRD raw hit that provides the basic information for the TrdHit.
  * \param splineTrack SplineTrack that provides second coordinate and pathlength.
  * \param steps Bit-wise OR of TrdHitProductionSteps to be used in hit production. For example:
  * \code
  *  ACsoft::Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit,*splineTrack,header,
  *                                                           ACsoft::Analysis::TrackInfo | ACsoft::Analysis::GainCorrection );
  * \endcode
  */
ACsoft::Analysis::TrdHit ACsoft::Analysis::TrdHitFactory::ProduceTrdHitFrom( const ACsoft::AC::TRDRawHit& rhit, const ACsoft::Analysis::SplineTrack& splineTrack,
                                                             int steps ) {

  // construct TrdHit for analysis step-by-step
  ACsoft::Analysis::TrdHit hit(rhit);

  // coordinate along wire and distance and pathlength
  if( TestOption(steps,ACsoft::Analysis::TrackInfo) )
    hit.FillInformationFromTrack(splineTrack);

  // gain correction
  if( TestOption(steps,ACsoft::Analysis::GainCorrection) ) {

    hit.ApplyGainCorrection(fReferenceGain);
  }

  return hit;
}


/** Produce a TRD hit from raw hit and %TRD track.
  *
  * The %TRD track is needed for the second coordinate (coordinate along wire) and the pathlength.
  *
  * \param rhit TRD raw hit that provides the basic information for the TrdHit.
  * \param trdTrack TrdTrack that provides second coordinate and pathlength.
  * \param steps Bit-wise OR of TrdHitProductionSteps to be used in hit production. For example:
  * \code
  *  ACsoft::Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit,*trdTrack,header,
  *                                                           ACsoft::Analysis::TrackInfo | ACsoft::Analysis::GainCorrection );
  * \endcode
  */
ACsoft::Analysis::TrdHit ACsoft::Analysis::TrdHitFactory::ProduceTrdHitFrom( const ACsoft::AC::TRDRawHit& rhit, const ACsoft::Analysis::TrdTrack& trdTrack,
                                                             int steps ) {

  // construct TrdHit for analysis step-by-step
  ACsoft::Analysis::TrdHit hit(rhit);

  // coordinate along wire and distance and pathlength
  if( TestOption(steps,ACsoft::Analysis::TrackInfo) )
    hit.FillInformationFromTrack(trdTrack);

  // gain correction
  if( TestOption(steps,ACsoft::Analysis::GainCorrection) ){

    hit.ApplyGainCorrection(fReferenceGain);
  }

  return hit;
}

/** Produce TRD hits from raw hits without any information that would require a track. */
void ACsoft::Analysis::TrdHitFactory::ProduceBasicTrdHitsFrom( const ACsoft::AC::Event& event, std::vector<TrdHit>& resultVector ) {

  unsigned int nTrdHits = event.TRD().RawHits().size();

  resultVector.clear();
  resultVector.reserve(nTrdHits);

  for( unsigned int i=0 ; i<nTrdHits ; ++i ) {

    const ACsoft::AC::TRDRawHit& rhit = event.TRD().RawHits()[i];
    resultVector.push_back(ACsoft::Analysis::TrdHit(rhit));
  }
}


