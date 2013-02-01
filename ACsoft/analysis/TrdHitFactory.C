#include "TrdHitFactory.hh"

#include "TRDRawHit.h"
#include "EventHeader.h"

#include "SplineTrack.hh"
#include "DetectorManager.hh"

#define DEBUG 0
#define INFO_OUT_TAG "TrdHitFactory> "
#include <debugging.hh>

ACsoft::Analysis::TrdHitFactory::TrdHitFactory() :
  fReferenceGain(100.0)
{
}


/** Produce a TRD hit from raw hit and spline track.
  *
  * The spline track is needed for the second coordinate (coordinate along wire) and the pathlength.
  * The event header is needed for the timestamp.
  *
  * \param rhit TRD raw hit that provides the basic information for the TrdHit.
  * \param splineTrack SplineTrack that provides second coordinate and pathlength.
  * \param header Event header that provides event time for alignment correction.
  * \param steps Bit-wise OR of TrdHitProductionSteps to be used in hit production. For example:
  * \code
  *  ACsoft::Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit,*splineTrack,header,
  *                                                           ACsoft::Analysis::TrackInfo | ACsoft::Analysis::GainCorrection );
  * \endcode
  */
ACsoft::Analysis::TrdHit ACsoft::Analysis::TrdHitFactory::ProduceTrdHitFrom( const AC::TRDRawHit& rhit, const ACsoft::Analysis::SplineTrack& splineTrack,
                                                             const AC::EventHeader& header, int steps ) {

  // update the TRD geometry if necessary
  Detector::DetectorManager::Self()->UpdateIfNeeded(header.TimeStamp());

  // construct TrdHit for analysis step-by-step
  ACsoft::Analysis::TrdHit hit(rhit);

  // coordinate along wire and distance and pathlength
  if( TestOption(steps,ACsoft::Analysis::TrackInfo) )
    hit.FillInformationFromTrack(splineTrack);

  // gain correction
  if( TestOption(steps,ACsoft::Analysis::GainCorrection) ){

    double timestamp = double(header.TimeStamp());
    hit.ApplyGainCorrection(timestamp,fReferenceGain);
  }

  return hit;
}


