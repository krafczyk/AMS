#include "TrdHitFactory.hh"

#include "TRDRawHit.h"
#include "EventHeader.h"

#include <SplineTrack.hh>

#define DEBUG 0
#define INFO_OUT_TAG "TrdHitFactory> "
#include <debugging.hh>

Analysis::TrdHitFactory::TrdHitFactory() :
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
  * \param steps Bit-wise OR of TrdHitProductionSteps to be used in hit production. For example, in Calibration::TrdAlignment::ProcessEvent():
  * \code
  *  Analysis::TrdHit hit = fTrdHitFactory->ProduceTrdHitFrom(rhit,*splineTrack,header,
  *                                                           Analysis::SecondCoord | Analysis::ShimGlobalAlign );
  * \endcode
  */
Analysis::TrdHit Analysis::TrdHitFactory::ProduceTrdHitFrom( const AC::TRDRawHit& rhit, const Analysis::SplineTrack& splineTrack, const AC::EventHeader& header, int steps ) {

  // construct TrdHit for analysis step-by-step
  Analysis::TrdHit hit(rhit);

  // coordinate along wire
  if( TestOption(steps,Analysis::SecondCoord) ){
    float secondCoord = ( rhit.Direction() == AC::XZMeasurement ? splineTrack.InterpolateToZ(rhit.Z()).Y() : splineTrack.InterpolateToZ(rhit.Z()).X() );
    hit.SetCoordinateAlongWire(secondCoord);
  }

  // global alignment and shimming
  if( TestOption(steps,Analysis::ShimGlobalAlign) )
    hit.ApplyShimmingCorrection();

  double timestamp = double(header.TimeStamp());
  // alignment correction
  if( TestOption(steps,Analysis::AlignmentCorrection) )
    hit.ApplyAlignmentCorrection(timestamp);

  // calculate and set pathlength 2D / 3D
  if( TestOption(steps,Analysis::Pathlength) )
    hit.FillPathlengthFromTrack(splineTrack);

  // gain correction
  if( TestOption(steps,Analysis::GainCorrection) )
    hit.ApplyGainCorrection(timestamp,fReferenceGain);

  return hit;
}

