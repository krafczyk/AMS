
#include "CutoffCalculator.hh"

#include <math.h>
#include <TMath.h>

#include "Event.h"
#include "AnalysisParticle.hh"
#include "Utilities.hh"


#define DEBUG 0
#define INFO_OUT_TAG "CutoffCalculator> "
#include <debugging.hh>


/** Initialize CutoffCalculator
  *
  */
Analysis::CutoffCalculator::CutoffCalculator(){

}

/** Destructor
  *
  */
Analysis::CutoffCalculator::~CutoffCalculator(){

}

/** Stoermer
  */
float Analysis::CutoffCalculator::CalculateCutoff( const Analysis::Particle& particle ) {

  const AC::EventHeader& evtHead = particle.RawEvent()->EventHeader();

  float cutoffRigidity = 0.;

  // ...variables needed for calculation
  float latitude = evtHead.MagneticLatitude();
  float altitude = evtHead.ISSAltitude();
  float rEarth = 6371.2; // km
  float MR2 = 57.156297; // in GV. Earth magnetic dipole moment (7.94e22 Am^2) divided by Earth radius squared.

  if( !particle.MainTrackerTrackFit() ) return 99999.; // FIXME ...
  float eventTheta = particle.MainTrackerTrackFit()->ThetaLayer1();
  float eventPhi = particle.MainTrackerTrackFit()->PhiLayer1();
  TVector3 eventInGTODFrame(sin((90. - eventTheta)*TMath::DegToRad())*cos(eventPhi*TMath::DegToRad()), sin((90. - eventTheta)*TMath::DegToRad())*sin(eventPhi*TMath::DegToRad()), cos((90.- eventTheta)*TMath::DegToRad()));
  // FIXME add documentation
  TVector3 eventInMAGFrame = Utilities::TransformAMS02toMAG(evtHead.ISSYaw()*TMath::DegToRad(), evtHead.ISSPitch()*TMath::DegToRad(), evtHead.ISSRoll()*TMath::DegToRad(), evtHead.ISSAltitude(), evtHead.ISSLongitude()*TMath::DegToRad(), (90. - evtHead.ISSLatitude())*TMath::DegToRad(), evtHead.ISSVelocityLongitude()*TMath::DegToRad(), (90. - evtHead.ISSVelocityLatitude())*TMath::DegToRad(), eventInGTODFrame);

  float eventThetaMAG = eventInMAGFrame.Theta();
  float eventPhiMAG = eventInMAGFrame.Phi();
  // calculating Stoermer cutoff rigidity
  float nominator = MR2 * pow(rEarth/(rEarth + altitude),2)  * pow(TMath::Cos(latitude), 4);
  float denominator = pow( 1 + sqrt(1- pow(TMath::Cos(latitude),3)*TMath::Sin(eventThetaMAG)*TMath::Sin(eventPhiMAG)),2);
  cutoffRigidity = ( denominator!=0.0 ? nominator / denominator : 99999. );

  DEBUG_OUT << "Cutoff: " << cutoffRigidity << std::endl;
  return cutoffRigidity;
}

